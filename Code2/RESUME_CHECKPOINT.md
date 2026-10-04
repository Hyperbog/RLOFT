# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `e64173d7ecfd8e604202c2286c932e78feb6396b` — `Integrate CODE.2 decompilation batch 64`.
- Verified translated instruction bytes: **184,044**.
- Proven non-code/data bytes: **3,780**.
- Reconciled object bytes: **187,824 / 239,698 = 78.3586% (78.36%)**.
- Unresolved object bytes: **51,874**.
- Last integrated instruction ownership fold: `2B6D0..2C293`.
- Batch-64 compiler separators `2BB9F`, `2BDF1..2BDF3`, `2BFEA..2BFEB`, and `2C22F` are directly proven alignment and counted in the residual ledger.
- The four population passes preserve per-iteration signed count reloads, callback-sensitive metadata reloads, and the `190h` cursor cap; `2C230` preserves the compiler's signed truncation-toward-zero division sequence.
- Next independently decoded instruction boundary: **`2C294`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2C210..2C2EF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2C210, 0x2C2F0)` — 224 bytes.
- SHA-256: `cf07d19e1c5c4d247d1670bcf93584511934d42810eff6ea27a59cca500d1b8a`.
- Narrow boundary window: `[0x2C250, 0x2C2B0)` — 96 bytes.
- Narrow SHA-256: `c1a91027965c1612cd9d298d9c8d3091f2964053f50039fb73deb32e79bf9672`.
- Narrow boundary bytes:
  `5514c1fa1f2bc2d1f8bb8400000029c38b45148b5514c1fa1fc1e2031bc2c1f80301c3895df88b45148b5514c1fa1f2bc2d1f80145f88b45f88945fc8b45fcc95f5e5bc35356575589e581ec40000000b9040000008d7ddcbe70ac0200f3a568`

Interpretation to re-prove on resume, not merely trust: `2C230..2C293` is the final Batch-64 signed layout-scalar helper and returns at `2C293`; `2C294` begins the next independent routine with the four-register prologue, a `40h`-byte local frame, and a four-dword `REP MOVSD` copy from the Batch-61 table at `2AC70`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 65 bytes are authorized by this checkpoint. Any analysis at or beyond `2C294` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 64 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2C294`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
