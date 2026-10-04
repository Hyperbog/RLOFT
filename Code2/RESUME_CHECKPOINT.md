# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `1967a449bff0140ff1591fde07c062eb630e7404` — `Integrate CODE.2 decompilation batch 53`.
- Last metadata/protocol commit before this checkpoint: `eb8ae7b1bf804a8e14daa9366bc1537964261664`.
- Verified translated instruction bytes: **164,226**.
- Proven non-code/data bytes: **3,457**.
- Reconciled object bytes: **167,683 / 239,698 = 69.9559% (69.96%)**.
- Unresolved object bytes: **72,015**.
- Last integrated ownership unit: exact 135-entry CS dword table `25DFC..26017` plus item-use dispatcher code `26018..273E6`.
- `273E7..273EF` remains uncounted/unresolved in Batch 53.
- Next independently decoded instruction boundary: **`273F0`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses in `Code2.cpp`/progress/ledger are **object-relative**. For extracted-object `objdump -b binary`, use the tracked address directly. Virtual address is `0x10000 + object_offset` only when a VA is explicitly needed.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `273A0..27480` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x273A0, 0x27480)` — 224 bytes.
- SHA-256: `9209e78cd9ac389ed79c1de349f5c425ac4bf83dce69802d376ba8d27366a6e2`.
- Narrower boundary window `[0x273E0, 0x27420)` SHA-256: `660feb69f9d1e68ae2572b9217ff9f332834947791c57e67774f6f30a12f2962`.
- Boundary bytes at `273E0..2741F`:
  `c4045d5f5e5bc3000000000000000000c8000000575653518b3dbcfd000081c763560000b928000000b711b300bee8d8000066bace0366b8054066ef66bac403`

Interpretation to re-prove on resume, not merely trust: Batch 53's code returns before the nine zero bytes at `273E7..273EF`; `273F0` starts `C8 00 00 00` (`ENTER 0,0`).

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** Batch 54 analysis had only begun. No Batch 54 bytes are authorized for accounting from chat notes alone.

Preliminary direction only: the block beginning at `273F0` appears runtime/graphics-heavy (planar VGA/sprite/page conversion and related low-level helpers). Treat this only as a lead. Re-run the mandatory overlap check, establish complete routine/table ownership, and re-prove any CODE.1 similarity before integrating anything.

## Resume rule

Do not start by reworking Batch 53. Verify a small overlap across its endpoint, confirm accounting and hashes, then proceed forward from `273F0`. If the overlap fails, stop forward work and repair the prior checkpoint first.
