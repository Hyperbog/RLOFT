# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `1727f97f58fd2d6a1c56825a7e65303cf61dd49f` — `Integrate CODE.2 decompilation batch 74`.
- Verified translated instruction bytes: **206,586**.
- Proven non-code/data bytes: **4,218**.
- Reconciled object bytes: **210,804 / 239,698 = 87.9457% (87.95%)**.
- Unresolved object bytes: **28,894**.
- Last integrated ownership fold: `318B4..31C57` (instructions through `31C56`, proven alignment at `31A12..31A13` and `31C57`).
- Next independently decoded instruction boundary: **`31C58`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `31BD0..31CAF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x31BD0, 0x31CB0)` — 224 bytes.
- SHA-256: `d7beccad5fc4d39703a3ee9f346390e098c4186b1e6563c1e7920e67324e785b`.
- Narrow boundary window: `[0x31C20, 0x31C80)` — 96 bytes.
- Narrow SHA-256: `df34a6fcf0ee3fcda666c67e16af452f5fdfd6351734977da7db4bc75734dea2`.
- Narrow boundary bytes:
  `09817dd4b20000007c02eb188b55d483ea32b87f00000029d089c28a45f8888214fe0000e951feffff8b45dc66a3fafc0000c95f5e5bc3905356575589e581ec2c000000c745d403000000c745d8040000008b45d4408945e8a130c700004089`.

Interpretation to re-prove on resume: `31C56` is the final Batch-74 `RET`; `31C57` is one NOP byte with no incoming direct reference; `31C58` begins the next independent routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 75 bytes are authorized by this checkpoint. Any analysis at or beyond `31C58` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 74 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `31C58`.
