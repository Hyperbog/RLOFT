# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `0ade6bf4418a1add7de1d8cfd02f5cafcaf194f4` — `Integrate CODE.2 decompilation batch 103`.
- Verified translated instruction bytes: **215,558**.
- Proven non-code/data bytes: **4,298**.
- Reconciled object bytes: **219,856 / 239,698 = 91.7221% (91.72%)**.
- Unresolved object bytes: **19,842**.
- Last integrated ownership fold: `33E6C..33FB3`.
- Next independently decoded instruction boundary: **`33FB4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33F60,0x34040)` — 224 bytes; SHA-256 `9dd8aaae8eb30395e01448b555ecf991c9ffbad7f301e512ff7be0020e1497d7`.
- Narrow `[0x33FA0,0x34000)` — 96 bytes; SHA-256 `5d72ac112b7b9eb18aef22b356e4545966086b09ad7ec9b431dd35c296e56b5b`.
- Narrow bytes: `000000597309ff0520fd00004975e961c9c387dbff0dc8fd000075138b35ccfd0000bfc4fd0000e8600000007222c3a1d8fd00000105d0fd0000a1dcfd00000105d4fd0000a1e4fd00000105e0fd0000ff0decfd000075118b35f0fd0000bfe8`.

Interpretation: `33FB1` is the Batch-103 RET; `33FB2..33FB3` is unreferenced `XCHG EBX,EBX` alignment; `33FB4` is a forward helper directly called twice by `33E6C`. No Batch 104 bytes are counted.
