# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `9b9d5a698ff9ec464f745d6a900581882f568cb3` — `Integrate CODE.2 decompilation batch 131`.
- Verified translated instruction bytes: **228,141**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **232,813 / 239,698 = 97.1276% (97.13%)**.
- Unresolved object bytes: **6,885**.
- Last integrated ownership fold: `37E77..38105`.
- Next independently decoded instruction boundary: **`38106`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x380C6,0x38186)` — 192 bytes; SHA-256 `64ae089496d8616bf492114d28f1ab5a63a1f2df5c32c10acf7145fb5602a80b`.
- Window `[0x380E6,0x38166)` — 128 bytes; SHA-256 `8727615a1cd9e1cf53cc758eb34fc2451592f45320468d69d820e07bb778d466`.
- Batch-owned `37E77..38105` SHA-256: `c2131bceeadda4ec9b70cf3ec8256f8bcc6572a106c62d0040cf4c4048d9ede7`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 131 interpretation
- `37E77..37E98`: forwarding wrapper into the general open/create helper.
- `37E99..38074`: DOS open-first/create-on-demand descriptor state machine.
- `38075..380E0`: descriptor publication plus device IOCTL bit-20h update.
- `380E1..380F7`: mode-62h create/open wrapper.
- `380F8..38105`: descriptor-close forwarding wrapper.
- `38106` begins the next independently decoded routine.
