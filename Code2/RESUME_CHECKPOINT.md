# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `738ad19b49f41de6e84d755c1a181c8cd6656007` — `Integrate CODE.2 decompilation batch 60`.
- Verified translated instruction bytes: **176,964**.
- Proven non-code/data bytes: **3,720**.
- Reconciled object bytes: **180,684 / 239,698 = 75.3799% (75.38%)**.
- Unresolved object bytes: **59,014**.
- Last integrated instruction ownership fold: `2A114..2A6AF`.
- Batch-60 separators `2A177`, `2A1E5..2A1E7`, `2A256..2A257`, and `2A305..2A307` are directly proven non-code/data and counted in the residual ledger.
- `2A1E8..2A20B` is now instruction-accounted by Batch 60 but reuses the pre-existing address-identical reconstruction of `function_2a1e8` rather than duplicating its C++ definition.
- Next independently decoded instruction boundary: **`2A6B0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2A650..2A730` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2A650, 0x2A730)` — 224 bytes.
- SHA-256: `d68f06359d49788d9c0c2a3454b7a48057bf48f53ad5425e0e4c64d78a2d1436`.
- Narrow boundary window: `[0x2A690, 0x2A6F0)` — 96 bytes.
- Narrow SHA-256: `dca857861f566dce8a6c1d0d1e9c3d1f616bae9508f8f8ee9d0ca599cff90792`.
- Narrow boundary bytes:
  `59665a664a7411d0c483d7005266bac403b00266ef5aebb35d5b595a5f5ec9c3c800000056575251538b7d148b7518c70529f0000000000000c7052df00000000000008b07c1e802a30df000008b4704c1e802a311f000008b4708a315f00000`

Interpretation to re-prove on resume, not merely trust: the final Batch-60 table-translation blitter restores registers and returns at `2A6AF`; its self-modified table displacement and plane-stepping loop remain code-owned. `2A6B0` begins the next independent clipped blitter with `ENTER 0,0`, the five-register save sequence, clip-structure pointer loads, and initialization of `F029/F02D`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 61 bytes are authorized by this checkpoint. Any analysis at or beyond `2A6B0` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 60 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2A6B0`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
