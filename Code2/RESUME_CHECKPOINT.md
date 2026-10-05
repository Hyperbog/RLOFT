# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `e1386f04654dad377a961479574e504c3b1ad74c` — `Integrate CODE.2 decompilation batch 108`.
- Verified translated instruction bytes: **218,605**.
- Proven non-code/data bytes: **4,319**.
- Reconciled object bytes: **222,924 / 239,698 = 93.0020% (93.00%)**.
- Unresolved object bytes: **16,774**.
- Last integrated ownership fold: `34A74..34BAF`.
- Next independently decoded instruction boundary: **`34BB0`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x34B60,0x34C40)` — 224 bytes; SHA-256 `3a732f6a6dbe15a2a6484c6b4a41a7d93bf2713fd56dce40fcc8a5a5d5cce6ec`.
- Narrow `[0x34B80,0x34BE0)` — 96 bytes; SHA-256 `d50a9fa33350273aaafdaaded3722efcb3e36d46372e76a578d4c8cd0bc65394`.
- Narrow bytes: `03d35052e8cbfdffff5e5f592b3d4cff000083c606a144ff0000010540ff00008a07a2774b03004975ab61c9c30000005356575589e581ec4c000000c745f400000000837df4047c0de96f0100008b45f4ff45f4ebed8b45f48d048500000000`.

Interpretation: `34BAC` is the Batch-108 RET; `34BAD..34BAF` is unreferenced zero padding; `34BB0` begins the next independently called routine. No Batch 109 bytes are counted.
