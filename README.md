# RLOFT

![CODE.1 progress](https://img.shields.io/badge/CODE.1-100.0%25-brightgreen)
![CODE.2 progress](https://img.shields.io/badge/CODE.2-2.4%25-red)
![Overall progress](https://img.shields.io/badge/Overall-22.6%25-red)

**CODE.1:** 100.0 / 100%  
**CODE.2:** 2.4 / 100%  
**Overall:** 22.6 / 100%

Current contiguous CODE.1 conversion/classification point: **0x11301 (complete)**.

CODE.1 object 1 is fully converted or explicitly classified. The tail from
**0xCBF0 through 0x11301** is linked Watcom/DOS runtime-support code rather than
Ravenloft game-authored mechanics; the original bytes remain the authority for
DOS-era runtime behavior.

Current contiguous CODE.2 conversion/classification point: **0x11A7**.

CODE.2 **0x0000..0x000F** is the original INT3/self-jump/NOP prefix.
CODE.2 **0x0010..0x0072** is reconstructed in `EntitySelector0010`;
**0x0073** is alignment. CODE.2 **0x0074..0x00B7** is reconstructed in
`Routine0074`. CODE.2 **0x00B8..0x01F9** is reconstructed in
`PartyRoutine00B8`; **0x01FA..0x01FB** is alignment. CODE.2
**0x01FC..0x0270** is reconstructed in `Routine01FC`. **0x0271..0x02A7**
is an embedded 14-entry dword table plus padding, not executable code.
CODE.2 **0x02A8..0x06B8** is reconstructed in `Dispatcher02A8`, including
the original 13-way jump table at **0x0274..0x02A7**. **0x06B9..0x06BF** is
zero alignment. CODE.2 **0x06C0..0x06FC**, **0x0700..0x0791**,
**0x0794..0x07E1**, and **0x07E4..0x0838** are reconstructed in
`SearchRoutines06C0`; intervening bytes are original alignment. CODE.2
**0x083C..0x0930** and **0x0934..0x09AF** are reconstructed in
`EntitySearch083C`; **0x0839..0x083B** and **0x0931..0x0933** are alignment.
CODE.2 **0x09B0..0x0B6B** is reconstructed in `TileCode09B0`. CODE.2
**0x0B6C..0x0BF9** is reconstructed in `DrawCell0B6C`; **0x0BFA..0x0BFB**
is alignment. CODE.2 **0x0BFC..0x0CAF** is reconstructed in
`ProjectedDraw0BFC`. CODE.2 **0x0CB0..0x10C2** is reconstructed in
`SceneCell0CB0`; **0x10C3** is alignment. CODE.2 **0x10C4..0x11A4** is\nreconstructed in `FrameDraw10C4`; **0x11A5..0x11A7** is alignment. Reconstruction\ncontinues at **0x11A8**.
