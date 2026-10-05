# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `972ef7c087260abe0cd088b9fca1d89e2b93bcad` — `Integrate CODE.2 decompilation batch 92`.
- Verified translated instruction bytes: **213,623**.
- Proven non-code/data bytes: **4,269**.
- Reconciled object bytes: **217,892 / 239,698 = 90.9027% (90.90%)**.
- Unresolved object bytes: **21,806**.
- Last integrated ownership fold: `337C8..33807`.
- Next independently decoded instruction boundary: **`33808`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x337C0,0x338A0)` — 224 bytes; SHA-256 `189e107ed72f53d3a61736b823da7bc41951ad40ffac815cf210064d1a2820b8`.
- Narrow `[0x33800,0x33860)` — 96 bytes; SHA-256 `6814ab28b587f683e2b1a5c7b171b63d4fbff281f8db9331eff29fcc69fe54e5`.
- Narrow bytes: `041f5f595bc9c390c8000000608b4510ba1c000000f7e28bf88b75148b87205a010089068b75188b87285a0100c1e80689068b872c5a0100ba000000000fbf1d19560200f7f38b752089168b752489068b751c66c706ffffbb00000000bf0000`.

Interpretation: `33806` is the final RET; `33807` is an unreferenced NOP alignment byte; `33808` begins the next independently called routine. No Batch 93 bytes are counted.
