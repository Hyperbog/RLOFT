# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `c1f89b6c4fcf5b0f2bd66aee9c9c268212f90879` — `Integrate CODE.2 decompilation batch 87`.
- Verified translated instruction bytes: **212,265**.
- Proven non-code/data bytes: **4,263**.
- Reconciled object bytes: **216,528 / 239,698 = 90.3337% (90.33%)**.
- Unresolved object bytes: **23,170**.
- Last integrated ownership fold: `33124..332B3`.
- Next independently decoded instruction boundary: **`332B4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33230,0x33310)` — 224 bytes; SHA-256 `4712ce1994faccbd47b51192b4fba09d2f88d9fe446e45f5689b108d4fac5fe6`.
- Narrow `[0x33280,0x332E0)` — 96 bytes; SHA-256 `f7cfa466fc67aba57bd2c8f59442c3d467e70c3d8c1dbeab344fcdd6f58c4b93`.
- Narrow bytes: `0000750ac70520220200000000008b86185a01003b05202202007d05a3202202003b0534bf00007e05a334bf00005f5e5bc9c390c800000053565766813de855020000807219a1e85502000fb71de4fc000081eb008000002bc3c1e804eb17a1`.

Interpretation: `332B2` is the final RET; `332B3` is an unreferenced NOP; `332B4` begins the next independent routine. No Batch 88 bytes are counted.
