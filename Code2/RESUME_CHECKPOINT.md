# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `d79193d622f48b78f6d5204124d60e06b75914db` — `Integrate CODE.2 decompilation batch 100`.
- Verified translated instruction bytes: **214,774**.
- Proven non-code/data bytes: **4,278**.
- Reconciled object bytes: **219,052 / 239,698 = 91.3867% (91.39%)**.
- Unresolved object bytes: **20,646**.
- Last integrated ownership fold: `33B44..33C8F`.
- Next independently decoded instruction boundary: **`33C90`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33C40,0x33D20)` — 224 bytes; SHA-256 `1d5f4fb438465649a96bd801110c47bfbe660e13a3f0a5a610f4b110f646101f`.
- Narrow `[0x33C80,0x33CE0)` — 96 bytes; SHA-256 `71e364f3d6588164b389625f5cec1a5aee62b012ec1307d7658d6dc12a8b10da`.
- Narrow bytes: `e83916000083c404b8a2b70200c95bc353e8aaa6ffff85c074138b4c240c8b542408b80400000031db01c9cd335bc390535589e583ec0cc745fc000000008b45fc8945f88b45f88945f4e871a6ffff85c0743ab803000000cd3366899df4ffff`.

Interpretation: `33C8F` is the final RET with no separator bytes; `33C90` begins the next independently called mouse-position setter. No Batch 101 bytes are counted.
