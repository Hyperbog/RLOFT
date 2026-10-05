# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `32ed9b8f7b295059d919c27ae061375123df1f91` — `Integrate CODE.2 decompilation batch 102`.
- Verified translated instruction bytes: **215,232**.
- Proven non-code/data bytes: **4,296**.
- Reconciled object bytes: **219,528 / 239,698 = 91.5852% (91.59%)**.
- Unresolved object bytes: **20,170**.
- Last integrated ownership fold: `33DE0..33E6B`.
- Next independently decoded instruction boundary: **`33E6C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33E20,0x33F00)` — 224 bytes; SHA-256 `f7c4638bf829a632dca020f1edfa47e6941e93428c8b5ab99271c2ba479fbb8f`.
- Narrow `[0x33E50,0x33EB0)` — 96 bytes; SHA-256 `b268bab46565892164e0ff8f0742fcc80abab03ba1d2c2e67c4d30553643f6bc`.
- Narrow bytes: `c42a8394fe0000b940000000f3aa83c7104b75eb5b595a5f5ec9c390c810000060c745fcffffff7fc745f801000080c745f4ffffff7fc745f0010000808b7508bf2cfd0000b908000000f3a5b904000000be00000000bb000000008b8330fd00`.

Interpretation: `33E25` and `33E6A` are the two Batch-102 RETs; `33E26..33E27` and `33E6B` are unreferenced compiler alignment; `33E6C` begins the next independently called routine. No Batch 103 bytes are counted.
