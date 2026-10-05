# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `79d1a849e01c3d1ecf0466a14e69f256665288d9` — `Integrate CODE.2 decompilation batch 116`.
- Verified translated instruction bytes: **222,129**.
- Proven non-code/data bytes: **4,471**.
- Reconciled object bytes: **226,600 / 239,698 = 94.5356% (94.54%)**.
- Unresolved object bytes: **13,098**.
- Last integrated ownership fold: `358D4..35A0B`.
- Next independently decoded instruction boundary: **`35A0C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35990,0x35A70)` — 224 bytes; SHA-256 `4470ab5305088033d53eca6b57aa66c2ea04bc2eecbd1688058daf896f2aafaa`.
- Narrow `[0x359D0,0x35A30)` — 96 bytes; SHA-256 `8e6b572c5e9d26585f64d832de919ce5fef1636cedc4ca1475266494a6534108`.
- Narrow bytes: `005055e8a0feffff83c4144603bc24ac0000003b74244472d7f68424e000000001750955e8ff26000083c4048d470f81c4cc0000005d5f5ec38d400056575581ecd00200008b9c24e0020000a174580300c78424b4020000000000008984249c`.

Interpretation: `358D4..35A08` is the translated compact loader and `35A09..35A0B` is proven alignment. Batch 115's alternate I/O path is documented correctly as seek-read-tell. `35A0C` begins a larger independently called loader.
