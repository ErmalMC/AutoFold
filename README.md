# AutoFold

AutoFold is a robotics prototype that automates T-shirt folding using servo-actuated wooden panels and Arduino-based control logic.

## Overview

Manual folding is repetitive and time-consuming. AutoFold explores a low-cost, practical approach to automate that workflow with simple mechanical parts and embedded control.


## How It Works

The system uses servo motors to drive hinged folding panels through a timed sequence:

1. Place shirt on the folding surface.
2. Trigger the servo sequence.
3. Side and top panels fold in order to complete the fold.

## Bill of Materials (BOM)

| Component | Purpose | Link |
|---|---|---|
| MG996R servo motor | Panel actuation | https://www.temu.com/goods.html?_bg_fs=1&goods_id=601099978329292&sku_id=17594173369570&auto_show_sku_panel=1&_x_sessn_id=5xehshcgyu&refer_page_name=bgt_order_detail&refer_page_id=10045_1754862491460_dygp0c7eiw&refer_page_sn=10045 |
| Power adapter | External power | https://www.temu.com/goods.html?_bg_fs=1&goods_id=601099653071662&sku_id=17594484498166&_x_sessn_id=5xehshcgyu&refer_page_name=bgt_order_detail&refer_page_id=10045_1754862529812_0mazjjbojn&refer_page_sn=10045 |
| Arduino kit | Controller and wiring components | https://www.temu.com/goods.html?_bg_fs=1&goods_id=601099531137420&sku_id=17592279329690&_x_sessn_id=5xehshcgyu&refer_page_name=bgt_order_detail&refer_page_id=10045_1754862646352_64kdh2u7x9&refer_page_sn=10045 |
| Hinges | Mechanical joints | https://www.temu.com/goods.html?_bg_fs=1&goods_id=601099594696062&sku_id=17592512717410&_x_sessn_id=5xehshcgyu&refer_page_name=bgt_order_detail&refer_page_id=10045_1754862675821_p5hgm3rq3d&refer_page_sn=10045 |
| Mounts | Servo/panel mounting | https://www.temu.com/goods.html?_bg_fs=1&goods_id=601100699506918&sku_id=17596924725816&_x_sessn_id=5xehshcgyu&refer_page_name=bgt_order_detail&refer_page_id=10045_1754862696217_57i6mt1zke&refer_page_sn=10045 |
| Wood base (1000 x 500 x 18 mm) | Frame and support | https://merkurmak.mk/shop/products/scx-ploca-lepena-smreka-1000x500x18-mm |

## Build Process

1. Cut and assembled the wood base.
2. Added a lower compartment for electronics.
3. Drilled routing holes for cable management.
4. Installed top folding panels and mounted servos.
5. Iterated on timing/angles until sequence was stable.

## Development Notes

- Duration: about 1 month (including part delivery time).
- Main issue: servo stalling under load.
- Mitigation: increased acceleration time and reduced peak lift angle.

## Results

- Produced a functioning folding prototype.
- Demonstrated measurable time savings for repetitive folding.
- Validated a simple mechanical design that can be improved incrementally.

## Limitations

- Motor torque limits supported payload.
- Folding angle range is constrained under heavier load.

## Future Improvements

- Use higher-torque motors.
- Upgrade power delivery with safe voltage/current headroom.
- Improve panel geometry and weight distribution.
- Add configurable folding presets for different shirt sizes.

## Tech Stack

- Arduino (C++)
- MG996R servo motors
- Wood + hinge mechanical frame

