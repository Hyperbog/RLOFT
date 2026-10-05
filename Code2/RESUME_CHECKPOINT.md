# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `30af6599a27daef620dff5b5be5c7925845fc19b` — `Integrate CODE.2 decompilation batch 93`.
- Verified translated instruction bytes: **213,812**.
- Proven non-code/data bytes: **4,272**.
- Reconciled object bytes: **218,084 / 239,698 = 90.9828% (90.98%)**.
- Unresolved object bytes: **21,614**.
- Last integrated ownership fold: `33808..338C7`.
- Next independently decoded instruction boundary: **`338C8`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33880,0x33960)` — 224 bytes; SHA-256 `9de8ceae8ac69859ac24250ba47db3b43b76962a80f94db974048d45c9a61c54`.
- Narrow `[0x338C0,0x33920)` — 96 bytes; SHA-256 `51460d13bec4b25832a6b99433838b178200bf840d23ec3d931d4665589d7022`.
- Narrow bytes: `891e61c9c387db90c8080000608b7d148b75108b4d0cb800008000ba000000008b5d0cf7fb8945fcb8000100002b450cd1f88b5d0cc1fb0303d883fb007d148b45fcf7dbf7eb8945f8bb00000000b900010000b850000000f7e303f88bc103c3`.

Interpretation: `338C4` is the final RET; `338C5..338C7` is unreferenced alignment; `338C8` begins the next independently called routine. No Batch 94 bytes are counted.
