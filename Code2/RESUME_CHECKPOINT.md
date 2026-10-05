# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `ad8c9041c01753b996adfa373bc92f9c60c49799` — `Integrate CODE.2 decompilation batch 96`.
- Verified translated instruction bytes: **214,178**.
- Proven non-code/data bytes: **4,274**.
- Reconciled object bytes: **218,452 / 239,698 = 91.1363% (91.14%)**.
- Unresolved object bytes: **21,246**.
- Last integrated ownership fold: `339A2..33A37`.
- Next independently decoded instruction boundary: **`33A38`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x339F0,0x33AD0)` — 224 bytes; SHA-256 `665b94b19505c57589c07ac86e3db0ab9e06b374ce7955ac47b2755f40009c4b`.
- Narrow `[0x33A10,0x33A70)` — 96 bytes; SHA-256 `b5e553d2d2c0d3cc3ff6944e8245c42794009c2edcf89586d5d42db21fba3ea6`.
- Narrow bytes: `5e595b58c3803d622601000975195053515657b42fcd2189df89d6b92b000000f3a45f5e595b58c38b542404b439cd21d1d0d1c885c07d0f25ffff000050e88456000083c404c331c0c38b542404b43bcd21d1d0d1c885c07d0f25ffff000050`.

Interpretation: `33A37` is the final RET of the mode-9 pre-find DTA shuttle; `33A38` begins the independently called DOS mkdir wrapper. No Batch 97 bytes are counted.
