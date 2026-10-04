# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `30f78b3b634fc20791cb70dde28cff59f5e97143` — `Integrate CODE.2 decompilation batch 57`.
- Verified translated instruction bytes: **173,550**.
- Proven non-code/data bytes: **3,678**.
- Reconciled object bytes: **177,228 / 239,698 = 73.9380% (73.94%)**.
- Unresolved object bytes: **62,470**.
- Last integrated instruction ownership fold: `29414..29921`.
- Batch-57 inter-routine separators plus the directly proven zero-fill pad `29922..2992F` are counted in the residual ledger.
- Next independently decoded instruction boundary: **`29930`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `298D0..299B0` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x298D0, 0x299B0)` — 224 bytes.
- SHA-256: `a4973ee51c1d36e76d3903af9a3bf2a1cf8c9e358fbf40469bd73fb88c53653e`.
- Narrow boundary window: `[0x29910, 0x29970)` — 96 bytes.
- Narrow SHA-256: `b29683c503d3a4965eb6ffc3e75b4d73b18b31cedccbb2f1fa78c044331eb397`.
- Narrow boundary bytes:
  `14ff742414e86a0c000083c414e9364e00000000000000000000000000000000c800000056575251538b4508a3e1ef00008125e1ef00008000000083e07fa3e5ef0000c705f5ef0000c8000000c705f9ef000050000000c70501f0000000000a`

Interpretation to re-prove on resume, not merely trust: `298F0` is the last Batch-57 lookup wrapper; after its `2A584` call it tail-jumps to `2E758` at `2991D`. Its instruction ownership ends at `29921`; `29922..2992F` is directly proven zero-fill padding. `29930` begins the next independent routine with `ENTER 0,0`, followed by `PUSH ESI / PUSH EDI / PUSH EDX / PUSH ECX / PUSH EBX`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 58 bytes are authorized by this checkpoint. Any analysis at or beyond `29930` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 57 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `29930`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
