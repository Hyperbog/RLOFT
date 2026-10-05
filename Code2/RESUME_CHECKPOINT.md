# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `3ee867dc4bd2bcca929f0ed4a4a5f8dca2b7285d` — `Integrate CODE.2 decompilation batch 85`.
- Verified translated instruction bytes: **211,460**.
- Proven non-code/data bytes: **4,260**.
- Reconciled object bytes: **215,720 / 239,698 = 89.9966% (90.00%)**.
- Unresolved object bytes: **23,978**.
- Last integrated ownership fold: `32F4C..32F8B`.
- Next independently decoded instruction boundary: **`32F8C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x32F00,0x32FE0)` — 224 bytes; SHA-256 `4efbcc0572a56d61918b6fc51eb9878e9fb3bafb16299aaa2600f13aeb99af33`.
- Narrow `[0x32F50,0x32FB0)` — 96 bytes; SHA-256 `8e447082e3576aa4d1435e728db88a83f71fca81fa3cda596cee73e05b0f1542`.
- Narrow bytes: `c600001c05185a0100c70000000000c7400493000000c7400893000000c7400c00000000c7401000000000c7401400000000c740180000000058c390c8100000535657a1e8550200c1e80425c00f00008bf08975fc813d105602000000ffff75`.

Interpretation: `32F8A` is the final RET; `32F8B` is an unreferenced NOP; `32F8C` begins the next independent routine. No Batch 86 bytes are counted.
