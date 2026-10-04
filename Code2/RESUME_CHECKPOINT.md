# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `693da84fcdf39f392c4175f58022712aaad8a6cd` — `Integrate CODE.2 decompilation batch 54`.
- Verified translated instruction bytes: **167,023**.
- Proven non-code/data bytes: **3,537**.
- Reconciled object bytes: **170,560 / 239,698 = 71.1562% (71.16%)**.
- Unresolved object bytes: **69,138**.
- Last integrated instruction ownership unit: `273F0..27F22`.
- `273E7..273EF` and the Batch-54 inter-routine separators are now directly proven non-code/data and counted in the residual ledger.
- `27F23` is proven one-byte NOP alignment and is already counted as non-code.
- Next independently decoded instruction boundary: **`27F24`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `27ECC..27FAC` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x27ECC, 0x27FAC)` — 224 bytes.
- SHA-256: `f0c5c69d10099faf0bd6bdfd6445aad71f344910be9c4c5eec5684bd41df9dff`.
- Narrow boundary window: `[0x27EF9, 0x27F59)` — 96 bytes.
- Narrow SHA-256: `f98cffdd659e2a7eba5f4a846883c0d46a2652ad263c9c1a91718e3d84467619`.
- Narrow boundary bytes:
  `ff356cee0000e86ae7000083c4040fb70568ee000050e8d48e0000c70540ee00000000000083c4045bc39053565755833d40ee0000000f84b4000000803d70ee0000000f84a7000000ff3544ee0000ff742418e84b73000083c4088d70048b28`

Interpretation to re-prove on resume, not merely trust: `27ECC` is the final Batch-54 teardown routine; it returns at `27F22`; `27F23` is NOP alignment; `27F24` begins the next routine with `PUSH EBX / PUSH ESI / PUSH EDI / PUSH EBP`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 55 bytes are authorized by this checkpoint. Any analysis beyond `27F23` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 54 merely because the overlap is re-disassembled. Verify the overlap and accounting, then proceed strictly forward from `27F24`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
