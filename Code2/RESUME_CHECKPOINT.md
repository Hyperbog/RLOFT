# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `f0d773e3fe455b85d0e6b59f639a8ebc79ebe79a` — `Integrate CODE.2 decompilation batch 84`.
- Verified translated instruction bytes: **211,397**.
- Proven non-code/data bytes: **4,259**.
- Reconciled object bytes: **215,656 / 239,698 = 89.9699% (89.97%)**.
- Unresolved object bytes: **24,042**.
- Last integrated ownership fold: `32954..32F4B`.
- Next independently decoded instruction boundary: **`32F4C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x32EC0,0x32FA0)` — 224 bytes; SHA-256 `4b32e9af1a5996bbd7577c4ff66cb7ac8d2445f921fe7d2f16ec4072f9004a97`.
- Narrow `[0x32F20,0x32F80)` — 96 bytes; SHA-256 `367439d0111ea61333ffc433b4b1b5f4368573639edd7bcf5c4840fca1c86754`.
- Narrow bytes: `f0550200a1e848020066b80080a3e8550200e8850400000fbf0df25502000fbf1dea550200e968faffff87db506b05fcc600001c05185a0100c70000000000c7400493000000c7400893000000c7400c00000000c7401000000000c740140000`.

Interpretation: `32F45..32F49` is the final Batch-84 jump back into shared sweep/outer control flow; `32F4A..32F4B` is unreferenced `XCHG EBX,EBX` alignment; `32F4C` begins the next independent helper. No Batch 85 bytes are counted.
