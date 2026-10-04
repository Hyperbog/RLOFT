# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `435048740e3afb3d891f51e4e7f6f4df9149a917` — `Integrate CODE.2 decompilation batch 66`.
- Verified translated instruction bytes: **191,324**.
- Proven non-code/data bytes: **3,820**.
- Reconciled object bytes: **195,144 / 239,698 = 81.4124% (81.41%)**.
- Unresolved object bytes: **44,554**.
- Last integrated instruction ownership fold: `2D004..2DF2B` (instructions through `2DF29`, proven separator through `2DF2B`).
- Next independently decoded instruction boundary: **`2DF2C`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2DEB0..2DF8F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2DEB0, 0x2DF90)` — 224 bytes.
- SHA-256: `07f0e398efd79f069d5371169da63fb67872e5739207737645659f7c1a6a0dbf`.
- Narrow boundary window: `[0x2DF00, 0x2DF60)` — 96 bytes.
- Narrow SHA-256: `07a999e7a1b9ecd567aeec17ae3b1e5120572bda82eec45d11fb5c3e74297126`.
- Narrow boundary bytes:
  `000089d0c1fa1ff7fb891558c60000eb0ac7056ccf000001000000c70548f0000000000000c95f5e5bc38bc05356575589e581ec08000000817d20680100007c0781452098feffff837d20007d0781452068010000803d2fc00000000f85aa00`

Interpretation to re-prove on resume, not merely trust: `2DF29` is the final Batch-66 `RET`; `2DF2A..2DF2B` is the `MOV EAX,EAX` compiler separator; `2DF2C` begins the next independent four-register-prologue routine with an 8-byte local frame and normalizes argument `+20h` into the `[0,167h]` range before checking `C02F`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 67 bytes are authorized by this checkpoint. Any analysis at or beyond `2DF2C` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 66 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2DF2C`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
