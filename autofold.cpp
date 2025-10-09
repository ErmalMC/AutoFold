#include <Servo.h>

const int buttonPin = 2;
const int servoPins[3] = { 3, 5, 4 };

const int DOWN_ANGLE = 120;
const int LIFT_ANGLE = 0;

struct SmoothProfile {

  unsigned long finishMs;
  unsigned long descendMs;
  unsigned long holdMs;

};

SmoothProfile P[3] = {

  { 700, 800, 800 },
  { 900,1000,1000 },
  { 700,1400,600 }

};

int liftOffset[3] = { 0, 6, 6 };
int extraLift[3] = { 50, 70, 0 };

const unsigned long START_GAP_MS = 1000;
const unsigned long debounceMs = 30;

const float MAX_SPEED_DEG_PER_SEC = 80.0f;
const float PER_SERVO_MAX_SPEED[3] = {80.0f, 80.0f , 160.0f};

enum Phase { IDLE=0, MOVING_UP, HOLD, MOVING_DOWN, DONE };

struct SV {

  Servo sv;
  Phase phase;
  int curPos;
  int startPos;
  int target;
  unsigned long moveStart;
  unsigned long moveDur;
  unsigned long phaseStart;
  unsigned long doneAt;
  unsigned long lastWriteMs;
  bool started;
};

SV S[3];

bool running = false;
int lastReading = HIGH;
int stableState = HIGH;
unsigned long lastDebounceTime = 0;

unsigned long nowMillis() { return millis(); }

float easeInOutCubic(float t) {

  if (t < 0.5f) return 4.0f*t*t*t;
  float f = -2.0f*t + 2.0f;
  return 1.0f - (f*f*f)/2.0f;
}

void writeSafe(int idx, int ang, unsigned long t = 0) {

  ang = constrain(ang, 0, 180);
  S[idx].sv.write(ang);
  S[idx].curPos = ang;
  S[idx].lastWriteMs = (t == 0) ? nowMillis() : t;
}

void initServo(int idx, int pin) {

  S[idx].sv.attach(pin);
  S[idx].phase = IDLE;
  S[idx].curPos = DOWN_ANGLE;
  S[idx].startPos = DOWN_ANGLE;

  int t = LIFT_ANGLE + liftOffset[idx] - extraLift[idx];
  S[idx].target = constrain(t, 0, 180);
  S[idx].moveStart = 0;
  S[idx].moveDur = 0;
  S[idx].phaseStart = 0;
  S[idx].doneAt = 0;
  S[idx].lastWriteMs = nowMillis();
  S[idx].started = false;
  writeSafe(idx, DOWN_ANGLE, S[idx].lastWriteMs);

}

void startSmoothMoveUp(int idx) {

  S[idx].startPos = DOWN_ANGLE;
  int delta = abs(S[idx].target - S[idx].startPos);
  unsigned long neededMs = (unsigned long)((delta / MAX_SPEED_DEG_PER_SEC) * 1000.0f);
  unsigned long base = max(40UL, P[idx].finishMs);
  S[idx].moveDur = max(base, neededMs);
  S[idx].moveStart = nowMillis();
  S[idx].phase = MOVING_UP;
  S[idx].started = true;

}

void startSmoothMoveDown(int idx) {
  S[idx].startPos = S[idx].target;
  int delta = abs(DOWN_ANGLE - S[idx].startPos);
  unsigned long neededMs = (unsigned long)((delta / MAX_SPEED_DEG_PER_SEC) * 1000.0f);
  unsigned long base = max(40UL, P[idx].descendMs);
  S[idx].moveDur = max(base, neededMs);
  S[idx].moveStart = nowMillis();
  S[idx].phase = MOVING_DOWN;

}

void stepServo(int idx) {

  unsigned long t = nowMillis();
  SV &sv = S[idx];

  switch (sv.phase) {
    case IDLE: break;

    case MOVING_UP: {

      unsigned long elapsed = t - sv.moveStart;
      float progress = (sv.moveDur == 0) ? 1.0f : float(elapsed) / float(sv.moveDur);

      if (progress >= 1.0f) {

        writeSafe(idx, sv.target, t);
        sv.phase = HOLD;
        sv.phaseStart = t;

      } else {

        float e = easeInOutCubic(progress);
        float posf = sv.startPos + (sv.target - sv.startPos) * e;
        int desired = int(round(posf));

        unsigned long dt = (t > sv.lastWriteMs) ? (t - sv.lastWriteMs) : 1;
        int maxDelta = (int)ceil((MAX_SPEED_DEG_PER_SEC * dt) / 1000.0f);

        if (maxDelta < 1) maxDelta = 1;
        int delta = desired - sv.curPos;
        if (abs(delta) > maxDelta) {
          desired = sv.curPos + (delta > 0 ? maxDelta : -maxDelta);
        }
        writeSafe(idx, desired, t);
      }

    } break;

    case HOLD:

      if (t - sv.phaseStart < P[idx].holdMs) {
        if ((t - sv.phaseStart) % 200 < 20) writeSafe(idx, sv.target, t);
      } else {
        startSmoothMoveDown(idx);
      }
      break;

    case MOVING_DOWN: {

      unsigned long elapsed = t - sv.moveStart;
      float progress = (sv.moveDur == 0) ? 1.0f : float(elapsed) / float(sv.moveDur);
      if (progress >= 1.0f) {
        writeSafe(idx, DOWN_ANGLE, t);
        sv.phase = DONE;
        sv.doneAt = t;
      } else {
        float e = easeInOutCubic(progress);
        float posf = sv.startPos + (DOWN_ANGLE - sv.startPos) * e;
        int desired = int(round(posf));

        unsigned long dt = (t > sv.lastWriteMs) ? (t - sv.lastWriteMs) : 1;
        int maxDelta = (int)ceil((PER_SERVO_MAX_SPEED[idx] * dt) / 1000.0f);
        if (maxDelta < 1) maxDelta = 1;
        int delta = desired - sv.curPos;
        if (abs(delta) > maxDelta) {
          desired = sv.curPos + (delta > 0 ? maxDelta : -maxDelta);
        }
        writeSafe(idx, desired, t);
      }

    } break;

    case DONE: break;

  }
}

void manageSequenceProgress() {

  if (!running) return;

  for (int i = 0; i < 3; ++i) stepServo(i);

  if (!S[0].started && S[0].phase == IDLE) startSmoothMoveUp(0);

  if (!S[1].started && S[0].phase == DONE && (nowMillis() - S[0].doneAt >= START_GAP_MS)) {
    startSmoothMoveUp(1);
  }

  if (!S[2].started && S[1].phase == DONE && (nowMillis() - S[1].doneAt >= START_GAP_MS)) {
    startSmoothMoveUp(2);
  }

  if (S[0].phase == DONE && S[1].phase == DONE && S[2].phase == DONE) {
    running = false;
  }

}

void beginSequence() {

  for (int i = 0; i < 3; ++i) {

    S[i].phase = IDLE;
    S[i].curPos = DOWN_ANGLE;
    S[i].started = false;
    S[i].doneAt = 0;
    writeSafe(i, DOWN_ANGLE);

  }

  running = true;
  startSmoothMoveUp(0);

}

void setup() {

  for (int i = 0; i < 3; ++i) initServo(i, servoPins[i]);
  pinMode(buttonPin, INPUT_PULLUP);
  lastReading = digitalRead(buttonPin);
  stableState = lastReading;
  lastDebounceTime = nowMillis();
  delay(50);

}



void loop() {

  int reading = digitalRead(buttonPin);
  unsigned long t = nowMillis();

  if (reading != lastReading) lastDebounceTime = t;
  if ((t - lastDebounceTime) > debounceMs) {

    if (reading != stableState) {

      stableState = reading;
      if (stableState == LOW && !running) {

        beginSequence();
      }
    }
  }
  lastReading = reading;
  manageSequenceProgress();

}