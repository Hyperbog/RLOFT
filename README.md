# RLOFT

![CODE.1 progress](https://img.shields.io/badge/CODE.1-100.0%25-brightgreen)
![CODE.2 progress](https://img.shields.io/badge/CODE.2-0.4%25-red)
![Overall progress](https://img.shields.io/badge/Overall-22.6%25-red)

**CODE.1:** 100.0 / 100%  
**CODE.2:** 0.4 / 100%  
**Overall:** 22.6 / 100%

Current contiguous CODE.1 conversion/classification point: **0x11301 (complete)**.

CODE.1 object 1 is fully converted or explicitly classified. The tail from
**0xCBF0 through 0x11301** is linked Watcom/DOS runtime-support code rather than
Ravenloft game-authored mechanics; the original bytes remain the authority for
DOS-era runtime behavior.

Current contiguous CODE.2 conversion/classification point: **0x02A7**.

CODE.2 **0x0000..0x000F** is the original INT3/self-jump/NOP prefix.
CODE.2 **0x0010..0x0072** is reconstructed in `EntitySelector0010`;
**0x0073** is alignment. CODE.2 **0x0074..0x00B7** is reconstructed in
`Routine0074`. CODE.2 **0x00B8..0x01F9** is reconstructed in
`PartyRoutine00B8`; **0x01FA..0x01FB** is alignment. CODE.2
**0x01FC..0x0270** is reconstructed in `Routine01FC`. **0x0271..0x02A7**
is an embedded 14-entry dword table plus padding, not executable code.
Reconstruction continues at **0x02A8**.
