# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `d9f834e7af39915d50352b130ef78b67e1b7a406` — `Integrate CODE.2 decompilation batch 68`.
- Verified translated instruction bytes: **195,658**.
- Proven non-code/data bytes: **3,974**.
- Reconciled object bytes: **199,632 / 239,698 = 83.2848% (83.28%)**.
- Unresolved object bytes: **40,066**.
- Last integrated ownership fold: `2E7F0..2F0B3` (instructions through `2F0B1`, proven tables/separators through `2F0B3`).
- Proven ABI repair in this batch: `28020` and `286C4` retain an ignored first stack argument and consume their stream from the second.
- Next independently decoded instruction boundary: **`2F0B4`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2F030..2F10F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2F030, 0x2F110)` — 224 bytes.
- SHA-256: `a5e30b22aeb2f4c40ae1ff1e585cd312de4a7e2f2e3a76d5e3abfa844e1c27cf`.
- Narrow boundary window: `[0x2F080, 0x2F0E0)` — 96 bytes.
- Narrow SHA-256: `d3386e4b85e390dccef48af30d43af9658e2c7f69c743388d8eb5f00ca2889ab`.
- Narrow boundary bytes:
  `048b5c240801d08d14dd0000000001da8b580cc6441a02008b580c66c7441a0300008b580cc7441a0500000000ff48045bc38bc05356575583ec148b35b8f0000031edb9ffffffff892c24833db4f00000000f8e7c000000896c240489f00344`

Interpretation to re-prove on resume, not merely trust: `2F0B1` is the final Batch-68 `RET`; `2F0B2..2F0B3` is a `MOV EAX,EAX` separator; `2F0B4` begins the next independent four-register-prologue routine with a `14h` local frame, loads `F0B8`, initializes EBP=0 and ECX=-1, then begins a signed scan of the slot/item arrays.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 69 bytes are authorized by this checkpoint. Any analysis at or beyond `2F0B4` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 68 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2F0B4`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
