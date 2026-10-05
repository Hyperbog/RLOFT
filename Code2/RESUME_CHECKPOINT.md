# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `10918c14ed7498bd94feee2cb93af16584e42d5c` — `Integrate CODE.2 decompilation batch 72`.
- Verified translated instruction bytes: **204,632**.
- Proven non-code/data bytes: **4,212**.
- Reconciled object bytes: **208,844 / 239,698 = 87.1280% (87.13%)**.
- Unresolved object bytes: **30,854**.
- Last integrated ownership fold: `30D30..314AF` (instructions through `314AA`, proven alignment/padding through `314AF`).
- Next independently decoded instruction boundary: **`314B0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `31430..3150F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x31430, 0x31510)` — 224 bytes.
- SHA-256: `6f4b9f178a7cf0c68848b214c1180280a1fa9b0d4dee32020529522bf87ae5e7`.
- Narrow boundary window: `[0x31480, 0x314E0)` — 96 bytes.
- Narrow SHA-256: `923273f978aafc806aa9a183c2a1ccea069c79d9b6beb434bb3561db55b8ba96`.
- Narrow boundary bytes:
  `5e5bc390ff742404e8ab25000083c404c38b54240485d27508e8d070000031c0c352e8b968000083c404c300000000005356575589e581ec200000008b451401c00fbf8050b702008945f48b55148d149500000000a164b7020001d08b008945`

Interpretation to re-prove on resume: `314AA` is the final Batch-72 `RET`; `314AB..314AF` is five zero padding bytes with no incoming direct reference; `314B0` begins the next independent application routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 73 bytes are authorized by this checkpoint. Any analysis at or beyond `314B0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 72 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `314B0`.
