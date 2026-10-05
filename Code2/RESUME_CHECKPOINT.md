# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `6372f3c015bff274ad89f009ae98d108cb988593` — `Integrate CODE.2 decompilation batch 98`.
- Verified translated instruction bytes: **214,281**.
- Proven non-code/data bytes: **4,275**.
- Reconciled object bytes: **218,556 / 239,698 = 91.1797% (91.18%)**.
- Unresolved object bytes: **21,142**.
- Last integrated ownership fold: `33A7C..33A9F`.
- Next independently decoded instruction boundary: **`33AA0`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33A60,0x33B40)` — 224 bytes; SHA-256 `20602d7061502126f896eb5ed316084e8b6464457cb0ff9e0ff70320ffb89016`.
- Narrow `[0x33A90,0x33AF0)` — 96 bytes; SHA-256 `996fe138c803c9b63b37eaf84437cc852f037780c9cf9a5abf6a9c02c62081f0`.
- Narrow bytes: `89eae8b15600008b45005d5f5e5bc30083ec286a0c6a008d44242450e87fd2ffff83c40c8cdac7042400030000c7442404330000008b44242cc744240800000000894424148d44241c508d44240450506a31668954242ce890ffffff83c41083`.

Interpretation: `33A9E` is the final RET; `33A9F` is an unreferenced zero separator; `33AA0` begins the next independently called DPMI-style wrapper. No Batch 99 bytes are counted.
