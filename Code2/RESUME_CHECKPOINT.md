# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `aba5bd492ba017b982844fbc0b5dae5ec6dec5b3` — `Integrate CODE.2 decompilation batch 55`.
- Verified translated instruction bytes: **169,055**.
- Proven non-code/data bytes: **3,577**.
- Reconciled object bytes: **172,632 / 239,698 = 72.0206% (72.02%)**.
- Unresolved object bytes: **67,066**.
- Last integrated instruction ownership unit: `27F24..28738`.
- Inter-routine separators through `2873B` are directly proven non-code/data and counted in the residual ledger.
- Next independently decoded instruction boundary: **`2873C`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `286E0..287C0` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x286E0, 0x287C0)` — 224 bytes.
- SHA-256: `4436200b17d9af911eca06990cd06bd7471ebf2a45400b62c19eeb564c0a1c51`.
- Narrow boundary window: `[0x28720, 0x28780)` — 96 bytes.
- Narrow SHA-256: `0a58164a2fd55bb5378f1f4852a9ebf67e066999cd43c2458430d4a13af8597d`.
- Narrow boundary bytes:
  `0000007413837c240400740cff742404e87b89000083c404c38d400083ec04a150f00000a3f07002000fbf403283f803752e8b1530ef00008d04950000000029d001c089c2c1e00429d08d5032c704246400000089d0c1fa1ff73c2483c404c3`

Interpretation to re-prove on resume, not merely trust: `2871C` is the last Batch-55 free helper and returns at `28738`; `28739..2873B` is LEA alignment; `2873C` begins the next routine with `SUB ESP,4`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 56 bytes are authorized by this checkpoint. Any analysis beyond `2873B` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 55 merely because the overlap is re-disassembled. Verify the overlap and accounting, then proceed strictly forward from `2873C`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
