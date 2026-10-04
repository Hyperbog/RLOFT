# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `2f863627e28f7cbe86a88a8ef59bcbf249ac702d` — `Integrate CODE.2 decompilation batch 63`.
- Verified translated instruction bytes: **181,039**.
- Proven non-code/data bytes: **3,773**.
- Reconciled object bytes: **184,812 / 239,698 = 77.1020% (77.10%)**.
- Unresolved object bytes: **54,886**.
- Last integrated instruction ownership fold: `2B088..2B6CC`.
- Batch-63 compiler separators `2B186..2B187`, `2B1BA..2B1BB`, `2B2F3`, and `2B6CD..2B6CF` are directly proven alignment and counted in the residual ledger.
- `2B2F4` preserves signed IDIV quotient/remainder behavior and writes target word 0 into the mutable data-segment cache at `2B608 + q*2` on its mode-1/remainder-zero path.
- Next independently decoded instruction boundary: **`2B6D0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2B650..2B72F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2B650, 0x2B730)` — 224 bytes.
- SHA-256: `2c28862c66da6bb018cd50179f3ee6c6c531520d01a6d60ec703adcd4339512a`.
- Narrow boundary window: `[0x2B690, 0x2B6F0)` — 96 bytes.
- Narrow SHA-256: `fa599e86f0a27363cc7ce2f99829aaa381e2e218f92b5862eef013b5f0f92252`.
- Narrow boundary bytes:
  `c1fa1ff77d2489c38b45ec8b55ecc1fa1ff7fb89c28b45180fb7400401d08b5518668942048b451c01c0668b90007102008b451866015004c95f5e5bc38d40005356575589e581ec20000000c745e400000000c745e8000000000fbf05a25802`

Interpretation to re-prove on resume, not merely trust: `2B690..2B6CC` is the final divisor/phase tail of Batch 63 and returns at `2B6CC`; `2B6CD..2B6CF` is `LEA EAX,[EAX+0]` alignment. `2B6D0` begins the next independent routine with four saved registers, a 20h-byte local frame, zeroed locals `-1Ch/-18h`, and a signed-count scan of `258A2`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 64 bytes are authorized by this checkpoint. Any analysis at or beyond `2B6D0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 63 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2B6D0`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
