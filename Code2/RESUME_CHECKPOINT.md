# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `7f56608e91bad699293eb05d90721ebc96a912b8` — `Integrate CODE.2 decompilation batch 94`.
- Verified translated instruction bytes: **213,958**.
- Proven non-code/data bytes: **4,274**.
- Reconciled object bytes: **218,232 / 239,698 = 91.0446% (91.04%)**.
- Unresolved object bytes: **21,466**.
- Last integrated ownership fold: `338C8..3395B`.
- Next independently decoded instruction boundary: **`3395C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33910,0x339F0)` — 224 bytes; SHA-256 `fd64cc8e4ec6f8d9ea448c0349be3b6ff3edf53cbc4b8d58c94a0aa620bf8656`.
- Narrow `[0x33940,0x339A0)` — 96 bytes; SHA-256 `e5389100a9b14f17b8ef7d9ce023f4b8e2cfe5c3182b2e8f234ace27b73a7910`.
- Narrow bytes: `a4de100375108a062ac1880783c75003da86e9fecc75e361c9c387dbc800000066528a6508b00266bac40366ef665ac9c353568b44240c8b5424108b5c241489c6eb0b8a0a84c97409424b88084085db75f185db74074bc6000040ebf589f05e`.

Interpretation: `33959` is the final RET; `3395A..3395B` is unreferenced `XCHG EBX,EBX` alignment; `3395C` begins the next independent VGA sequencer helper. No Batch 95 bytes are counted.
