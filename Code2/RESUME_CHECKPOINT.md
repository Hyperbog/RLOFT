# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `a4a22ef2ac7b61bd87f85b929dcf231d30955061` — `Integrate CODE.2 decompilation batch 59`.
- Verified translated instruction bytes: **175,537**.
- Proven non-code/data bytes: **3,711**.
- Reconciled object bytes: **179,248 / 239,698 = 74.7808% (74.78%)**.
- Unresolved object bytes: **60,450**.
- Last integrated instruction ownership fold: `29CEC..2A113`.
- Batch-59 compiler separators through `2A103` are directly proven non-code/data and counted in the residual ledger.
- Next independently decoded instruction boundary: **`2A114`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2A0B0..2A190` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2A0B0, 0x2A190)` — 224 bytes.
- SHA-256: `3343dce744d59dec585723b1801a054b38828291901cef5692c49b662e3dfd0a`.
- Narrow boundary window: `[0x2A0F0, 0x2A150)` — 96 bytes.
- Narrow SHA-256: `543eb594e41d8a9bd4b0af90a8715cb3851b9a840546482d62224250e1983f4a`.
- Narrow boundary bytes:
  `f55d66bace0366b8054066ef5b595a5f5ec9c390c8000000518b4d08e8ff0a000059c9c3c800000056575251538b5d148b75188b550c8b4d108b7d0853568bc28bd9e8010a00008bf35b03d98bd383c203c1ea02598b1df9ef00002bdafc66b8`

Interpretation to re-prove on resume, not merely trust: `2A078` is the final substantial Batch-59 planar copy helper and returns at `2A102`; `2A103` is proven one-byte NOP alignment; `2A104` is the final Batch-59 display-start wrapper and returns at `2A113`; `2A114` begins the next independent routine with `ENTER 0,0`, followed by the five-register save sequence and planar source setup.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 60 bytes are authorized by this checkpoint. Any analysis at or beyond `2A114` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 59 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2A114`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
