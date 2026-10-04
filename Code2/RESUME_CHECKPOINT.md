# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `d0e36dd24ba9e529523f7360244495be14b2194c` — `Integrate CODE.2 decompilation batch 65`.
- Verified translated instruction bytes: **187,462**.
- Proven non-code/data bytes: **3,802**.
- Reconciled object bytes: **191,264 / 239,698 = 79.7937% (79.79%)**.
- Unresolved object bytes: **48,434**.
- Last integrated instruction ownership fold: `2C294..2D003` (instructions through `2D001`, proven separator through `2D003`).
- Batch-65 compiler separators `2C7F7..2C7FF`, `2C8E2..2C8E3`, `2C9D9..2C9DB`, `2CAD3`, `2CC85..2CC87`, `2CEB6..2CEB7`, and `2D002..2D003` are directly proven alignment and counted in the residual ledger.
- Preserve `2C294`'s dynamic clip/global reloads and compiler-order rectangle restore, `2CAD4`'s independent party branches, and `2CEB8`'s low-word transition copy plus post-callback `258EC` reload.
- Next independently decoded instruction boundary: **`2D004`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2CFF0..2D0CF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2CFF0, 0x2D0D0)` — 224 bytes.
- SHA-256: `9f68d05e36bfe5d05de60305f4e46354216dc65453c4d631fc38e1d78bc43796`.
- Narrow boundary window: `[0x2D000, 0x2D060)` — 96 bytes.
- Narrow SHA-256: `f45147203dc64635216a67d07a730ad3cfb44f12803e208f987fff1c976d115d`.
- Narrow boundary bytes:
  `5bc38bc05356575589e581ec100000006a3ee8c5f1feff83c40485c00f84e00000008b451401c00fbf80225602003d420200007c138b451401c00fbf80225602003d470200007e05e9b0000000833dc8c60000087409833dc8c60000127502eb`

Interpretation to re-prove on resume, not merely trust: `2D001` is the final Batch-65 `RET`; `2D002..2D003` is a `MOV EAX,EAX` compiler separator; `2D004` begins the next independent routine with the four-register prologue, a `10h` local frame, and `PUSH 3Eh / CALL 1C1DCh` before routing on `[25622 + argument*2]` and `C6C8`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 66 bytes are authorized by this checkpoint. Any analysis at or beyond `2D004` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 65 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2D004`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
