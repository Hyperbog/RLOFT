# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `e839f18d938c6093ac2699c7545eea36d79b6a38` — `Integrate CODE.2 decompilation batch 82`.
- Verified translated instruction bytes: **209,081**.
- Proven non-code/data bytes: **4,255**.
- Reconciled object bytes: **213,336 / 239,698 = 89.0020% (89.00%)**.
- Unresolved object bytes: **26,362**.
- Last integrated ownership fold: `32620..3263B`.
- Next independently decoded instruction boundary: **`3263C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x325C0,0x326A0)` — 224 bytes; SHA-256 `a534e48e9656d36516cb1902b47027267867e21df02a515f8808b2200f277aec`.
- Narrow `[0x32600,0x32660)` — 96 bytes; SHA-256 `d2a96041dc424aa6a0627db05640c0ba97ddb89e536b8b3afd93417f356202b6`.
- Narrow bytes: `ffff83c40cff742408e842fbffff83c40453e8c5ecffff83c4045bc300000000c80000005657518b7d0c8b4d108b35e4550200ff5508595f5ec9c390c82c0000535657813d00c70000930000000f84fa020000b8000800000fb6154def0000f7`.

Interpretation: `3263A` is the final RET; `3263B` is an unreferenced NOP; `3263C` begins the next independent renderer routine. No Batch 83 bytes are counted.
