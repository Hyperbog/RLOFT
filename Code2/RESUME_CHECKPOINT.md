# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `bccdd9e3dd86b3836b4bde28470751833d8165bf` — `Integrate CODE.2 decompilation batch 95`.
- Verified translated instruction bytes: **214,028**.
- Proven non-code/data bytes: **4,274**.
- Reconciled object bytes: **218,302 / 239,698 = 91.0738% (91.07%)**.
- Unresolved object bytes: **21,396**.
- Last integrated ownership fold: `3395C..339A1`.
- Next independently decoded instruction boundary: **`339A2`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33960,0x33A40)` — 224 bytes; SHA-256 `7f2e8b802fd8b354ee95c06de55156364eadf9e3ec41c3b79a76c26b042435db`.
- Narrow `[0x339A0,0x33A00)` — 96 bytes; SHA-256 `82b52b929b46d678db6fc7bbb5823db5d8d1e4385c83fe991a3773052cf45dfe`.
- Narrow bytes: `5bc353528b44240c8b5424108b5c241451505289dab41acd21595ab44ecd21e8c856000089dae827000000595a5bc38b4424045289c2b41acd21e836000000b44fcd21e8a4560000e8050000005ac331c0c3803d622601000975195053515657`.

Interpretation: `33970` and `339A1` are independent RETs for the two Batch-95 helpers; there are no separator bytes; `339A2` begins the DOS DTA/find-first wrapper. No Batch 96 bytes are counted.
