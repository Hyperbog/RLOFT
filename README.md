# RLOFT

![CODE.1 progress](https://img.shields.io/badge/CODE.1-100.0%25-brightgreen)
![CODE.2 progress](https://img.shields.io/badge/CODE.2-0.0%25-red)
![Overall progress](https://img.shields.io/badge/Overall-22.6%25-red)

**CODE.1:** 100.0 / 100%  
**CODE.2:** 0.0 / 100%  
**Overall:** 22.6 / 100%

Current contiguous CODE.1 conversion/classification point: **0x11301 (complete)**.

CODE.1 object 1 is fully converted or explicitly classified. The tail from
**0xCBF0 through 0x11301** is linked Watcom/DOS runtime-support code rather than
Ravenloft game-authored mechanics; the original bytes remain the authority for
DOS-era runtime behavior.

Current contiguous CODE.2 conversion/classification point: **0x00B7**.

CODE.2 **0x0000..0x000F** is the original INT3/self-jump/NOP prefix.
CODE.2 **0x0010..0x0072** is reconstructed in `EntitySelector0010`;
**0x0073** is the following alignment NOP. Reconstruction continues at
**0x0074**.
