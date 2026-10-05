# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. Live repository/binary evidence wins on disagreement.

## Last committed substantive decompilation state

- Last substantive batch commit: `368b77d1c23c7d3787bb53a3ec981982c60bbe8a` — `Integrate CODE.2 decompilation batch 76`.
- Verified translated instruction bytes: **207,406**.
- Proven non-code/data bytes: **4,226**.
- Reconciled object bytes: **211,632 / 239,698 = 88.2911% (88.29%)**.
- Unresolved object bytes: **28,066**.
- Last integrated ownership fold: `31E74..31F93`.
- Next independently decoded instruction boundary: **`31F94`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`; length: `0x3A852`.

## Mandatory overlap anchor for next resume

- Window `[0x31F10,0x31FF0)` — 224 bytes; SHA-256 `2de7e980d24eefbbdfe764af960d644ef4ea9ab6da0e4adc905396c3b166eede`.
- Narrow `[0x31F60,0x31FC0)` — 96 bytes; SHA-256 `c0919b9c8fdb60265c160ce2c59c14e3457a75df30b0896878fcd16e5c96d65e`.
- Narrow bytes: `3d1cf90000007d0ac7051cf9000000000000e8fdfeffff03051cf9000050ff3520f90000e88bfaffff83c4085d5f5e5bc38d40005356575589e581ec08000000a14ccf0000c1e8108945fc0fb745fc3dffff00007507c745fc170000000fb755`.

Interpretation: `31F90` is the last RET; `31F91..31F93` is LEA alignment; `31F94` starts the next independent routine. No Batch 77 bytes are counted.
