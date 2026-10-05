# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins on disagreement.

## Last committed substantive decompilation state
- Last substantive batch commit: `80fddbfdd37663387bdf8a1b25887e40fbbbcd9b` — `Integrate CODE.2 decompilation batch 77`.
- Verified translated instruction bytes: **207,762**.
- Proven non-code/data bytes: **4,226**.
- Reconciled object bytes: **211,988 / 239,698 = 88.4396% (88.44%)**.
- Unresolved object bytes: **27,710**.
- Last integrated ownership fold: `31F94..320F7`.
- Next independently decoded instruction boundary: **`320F8`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start `0x3AF24`, length `0x3A852`.

## Mandatory overlap anchor for next resume
- Window `[0x32070,0x32150)` — 224 bytes; SHA-256 `7a8892428b345013bdff25696746c94b4bd43cb50781935d3bdc55d54a4e15b2`.
- Narrow `[0x320C0,0x32120)` — 96 bytes; SHA-256 `742245f950f9ad3b4b5070b9873e5d769927ee2be3a0d7a7e9326b5bcca49aef`.
- Narrow bytes: `45fc01c00fbf8024f90000a320f90000e89ffdffff03051cf9000050ff3520f90000e82df9ffff83c4080fb745fca3ccf90000c95f5e5bc3538b4424088b108d5a0189188a5c240c881aff40105bc35383ec048d44241468f820030089442404`.

Interpretation: `320F7` is the final RET; `320F8` immediately begins the next independent byte-output callback. No Batch 78 bytes are counted.
