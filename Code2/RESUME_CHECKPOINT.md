# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `5fa532dcb06fd93d9395810c43cd02a95ea63fd0` — `Integrate CODE.2 decompilation batch 125`.
- Verified translated instruction bytes: **225,364**.
- Proven non-code/data bytes: **4,624**.
- Reconciled object bytes: **229,988 / 239,698 = 95.9491% (95.95%)**.
- Unresolved object bytes: **9,710**.
- Last integrated translated/alignment pieces extend through `375FC`; byte `375FD` remains unresolved.
- Next independently decoded instruction boundary: **`375FE`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x375BE,0x37680)` — 194 bytes; SHA-256 `b7d8fd0dd4201b0fd0ec96304b97b027adb2d9d5a8c7d641ce4be754bbef9ee2`.
- Overlap bytes are verification only and must not be recounted.

## Batch 125 interpretation
- `374D2..374DF`: 14 proven zero alignment bytes.
- `374E0..3757B`: 28-byte header clone plus variable-tail read helper.
- `3757C..37588`: free wrapper.
- `37589..3758F`: 7 proven zero alignment bytes.
- `37590..375EE`: two-byte length-prefixed allocation/fill helper.
- `375EF`: explicit NOP separator.
- `375F0..375FC`: second free wrapper.
- `375FD` remains unresolved; `375FE` begins the next decoded routine.
