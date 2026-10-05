# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `ebafaacf9df970de0c8a5cdc12192ee2e20844c9` — `Integrate CODE.2 decompilation batch 133`.
- Verified translated instruction bytes: **229,335**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **234,007 / 239,698 = 97.6258% (97.63%)**.
- Unresolved object bytes: **5,691**.
- Last integrated ownership fold: `384AA..385AF`.
- Next independently decoded instruction boundary: **`385B0`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x38570,0x38630)` — 192 bytes; SHA-256 `5db1769db761188be424725d5e230e139245bbd78e122b43e07586521258ea1d`.
- Window `[0x38590,0x38610)` — 128 bytes; SHA-256 `a563b3e0df695978fa4b2d4312beb4d7b7a7ef3c43947f82000a0d9919853942`.
- Batch-owned `384AA..385AF` SHA-256: `2b8cfb9980b8ae9c6ccb071daa6d4f6a9926a8ab0fd336a01eb7a78b8d5881e6`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 133 interpretation
- `384AA..3854D`: DOS current-directory builder with optional exact-size allocation.
- `3854E..3856D`: forwarding wrapper plus ASCII lowercase-to-uppercase helper.
- `3856E..385AF`: all-stream wrapper plus masked stream traversal/flush.
- `385B0` begins the next independently decoded routine, a larger formatted-output family.
