# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `7d421e00191a6e84129e288382d6b4696e44e531` — `Integrate CODE.2 decompilation batch 71`.
- Verified translated instruction bytes: **202,780**.
- Proven non-code/data bytes: **4,144**.
- Reconciled object bytes: **206,924 / 239,698 = 86.3270% (86.33%)**.
- Unresolved object bytes: **32,774**.
- Last integrated ownership fold: `305A8..30D2F` (instructions through `30D25`, proven table/alignment/padding through `30D2F`).
- Next independently decoded instruction boundary: **`30D30`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `30CB0..30D8F` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x30CB0, 0x30D90)` — 224 bytes.
- SHA-256: `e3a6c3c5a8290d8c5d8a420755d1304238bf125800b30fbad888bfcf7de40b93`.
- Narrow boundary window: `[0x30D00, 0x30D60)` — 96 bytes.
- Narrow SHA-256: `8882f49487c40880cb0d48469a1bcb6f0808660b8d1ae3de0050b18e9ac1b645`.
- Narrow boundary bytes:
  `66f3a511c9464766f3a407fc5f5ec38cda89c7068ec251c1e902f3a55983e103f3a4075f5ec3000000000000000000008b4424048b5424088b4c240c88d6c1e20888f2c1e20888f2e8836c00008b442404c35383ec0c89e050e8156d000083c4`

Interpretation to re-prove on resume: `30D25` is the final Batch-71 `RET`; `30D26..30D2F` is ten zero padding bytes with no incoming direct reference; `30D30` begins the next independent memory-fill/runtime-wrapper routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 72 bytes are authorized by this checkpoint. Any analysis at or beyond `30D30` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 71 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `30D30`.
