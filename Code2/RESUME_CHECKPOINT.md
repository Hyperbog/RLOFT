# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `a5b586d11e21cc65cdd20b1bf87aa83f548c3a6f` — `Integrate CODE.2 decompilation batch 67`.
- Verified translated instruction bytes: **193,485**.
- Proven non-code/data bytes: **3,903**.
- Reconciled object bytes: **197,388 / 239,698 = 82.3486% (82.35%)**.
- Unresolved object bytes: **42,310**.
- Last integrated ownership fold: `2DF2C..2E7EF` (instructions through `2E7EE`, proven table/separators through `2E7EF`).
- Next independently decoded instruction boundary: **`2E7F0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2E770..2E84F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2E770, 0x2E850)` — 224 bytes.
- SHA-256: `9d5cef77c3b7cd4983ef3c39500b7bd8dc6498a99156197bedb6c6d4c9764716`.
- Narrow boundary window: `[0x2E7C0, 0x2E820)` — 96 bytes.
- Narrow SHA-256: `e0b7a0a835e3dcd5df5595e990fe55768c3149b5d24263a4610dcbdd418c266d`.
- Narrow boundary bytes:
  `8d44240850e8222b000083c40c530fbf442404506861f00000e80e2b000083c40c53e8f52a000083c40483c4045bc3905356575583ec54e86cffffff8b44246cc1e00450e84328000083c404a3b8f000008b44246cc744245000000000a3b4f0`

Interpretation to re-prove on resume, not merely trust: `2E7EE` is the final Batch-67 `RET`; `2E7EF` is a `NOP` separator; `2E7F0` begins the next independent routine with `PUSH EBX/ESI/EDI/EBP`, a `54h` stack frame, calls `2E768`, allocates `arg*16` bytes via `3104C`, and stores the result at `F0B8`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 68 bytes are authorized by this checkpoint. Any analysis at or beyond `2E7F0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 67 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2E7F0`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
