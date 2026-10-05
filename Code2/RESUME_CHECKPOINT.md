# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `c84ddd8b77f82909f8beeb7f442812881ed9b43c` — `Integrate CODE.2 decompilation batch 79`.
- Verified translated instruction bytes: **208,043**.
- Proven non-code/data bytes: **4,241**.
- Reconciled object bytes: **212,284 / 239,698 = 88.5631% (88.56%)**.
- Unresolved object bytes: **27,414**.
- Last integrated ownership fold: `32150..3221F`.
- Next independently decoded instruction boundary: **`32220`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x321A0,0x32280)` — 224 bytes; SHA-256 `bfa31ddb1a1fd9dbbbdbe9529fcb80df102fc5f49859e7c55bfd7be53240b094`.
- Narrow `[0x321F0,0x32250)` — 96 bytes; SHA-256 `982f57630290c4771c0097eaecfeb6445ac3af719fd7f4578ec4b24364fe4406`.
- Narrow bytes: `130fafd601ea89d0c1fa1ff7f988034339fb75e1ff4424088344240403817c2408ff0000007e8583c40c5d5f5e5bc3905356575583ec0c8b5424208a44242c884424088a442430884424098a4424348844240a8b4424282b442424b900030000`.

Interpretation: `3221E` is the final RET; `3221F` is one unreferenced NOP; `32220` starts the next independent routine. No Batch 80 bytes are counted.
