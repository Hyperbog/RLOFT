# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `2e205937f7680a27270861fa71511120547c0a3a` — `Integrate CODE.2 decompilation batch 86`.
- Verified translated instruction bytes: **211,866**.
- Proven non-code/data bytes: **4,262**.
- Reconciled object bytes: **216,128 / 239,698 = 90.1668% (90.17%)**.
- Unresolved object bytes: **23,570**.
- Last integrated ownership fold: `32F8C..33123`.
- Next independently decoded instruction boundary: **`33124`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x330A0,0x33180)` — 224 bytes; SHA-256 `e1b54aabe39f0ecfde55daa067c7f964ead49173503593fe89c9fe41c8339ce6`.
- Narrow `[0x330E0,0x33140)` — 96 bytes; SHA-256 `a99b83525e82cb255470ab7649baee815a771d601a8c610d0da99a79dc1224ca`.
- Narrow bytes: `0089862c5a010081be205a010093000000750ac70520220200000000008b86185a01003b05202202007d05a3202202003b0534bf00007e05a334bf00005f5e5bc9c387dbc8100000535657a1f0550200c1e80425c00f00008bf08975fc813d04`.

Interpretation: `33121` is the final RET; `33122..33123` is unreferenced `XCHG EBX,EBX` alignment; `33124` begins the next independent routine. No Batch 87 bytes are counted.
