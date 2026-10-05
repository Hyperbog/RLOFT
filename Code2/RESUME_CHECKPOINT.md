# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `5188853c72ff3ff9327d2a5a3d4b1a63ef1cf47e` — `Integrate CODE.2 decompilation batch 134`.
- Verified translated instruction bytes: **229,754**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **234,426 / 239,698 = 97.8006% (97.80%)**.
- Unresolved object bytes: **5,272**.
- Last integrated ownership fold: `38867..38A09` (independent of still-unowned `385B0..38866`).
- Next independently decoded instruction boundary: **`38A0A`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x389CA,0x38A8A)` — 192 bytes; SHA-256 `0ad56774348dad1390f546331c7a16371ba91662b012344328a35866e86e91f6`.
- Window `[0x389EA,0x38A6A)` — 128 bytes; SHA-256 `e4c9bff32a2b8043992ef8e02a2e467269ffd900ee1405a6e913b6373e050ab2`.
- Batch-owned `38867..38A09` SHA-256: `17deff407d631e6e05710b7e16a05b03075ad62a72f42620a000a6cfbf24b086`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 134 interpretation
- `38867..389A6`: width/precision/length parser for the formatted-output core.
- `389A7..38A09`: flag-prefix parser.
- `385B0..38866` is the formatter core and remains deliberately unowned until reconstructed as a whole.
- `38A0A` begins the next independent string-length helper.
