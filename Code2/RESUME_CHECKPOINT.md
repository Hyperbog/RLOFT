# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `d6d1556e15b0ea2c5ca5deabdb24433ee9f15e43` — `Integrate CODE.2 decompilation batch 123`.
- Verified translated instruction bytes: **224,767**.
- Proven non-code/data bytes: **4,565**.
- Reconciled object bytes: **229,332 / 239,698 = 95.6754% (95.68%)**.
- Unresolved object bytes: **10,366**.
- Last independently integrated island: `372F6..3736C`.
- Next independently decoded instruction boundary: **`3736D`**.
- The earlier large helper `36CEC..372F5` remains unresolved and must not be counted through overlap.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x372B6,0x373C8)` — 274 bytes; SHA-256 `cccc69f3ea9f01660fc5a5fcb3022d991e7243bb42ca2d26199ca71f989bc417`.
- Batch-owned narrow `[0x372F6,0x3736D)` — 119 bytes; SHA-256 `6bd1174aca75fe65210f87fb0ebe484e9cb6060f82f597e6251a6bf66403ad67`.
- Overlap bytes are verification only and must not be recounted.

## Batch 123 interpretation
- `372F6..37338`: PC speaker/PIT channel-2 start helper; divisor threshold `0x12`, counter `0x1234DC/divisor`, conditional port-61 enable and PIT mode write, then low/high bytes to port 42h.
- `37339..37347`: speaker gate disable helper.
- `37348..3736C`: 37-byte NUL-terminated lowercase radix digit alphabet, directly referenced by `37398`.
- `3736D` begins the next conversion helper and remains uncounted.
