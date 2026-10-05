# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `ef717e747d99dbc47754d2f1d6b4f384793e8453` — `Integrate CODE.2 decompilation batch 90`.
- Verified translated instruction bytes: **213,069**.
- Proven non-code/data bytes: **4,267**.
- Reconciled object bytes: **217,336 / 239,698 = 90.6708% (90.67%)**.
- Unresolved object bytes: **22,362**.
- Last integrated ownership fold: `334C4..335DB`.
- Next independently decoded instruction boundary: **`335DC`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33550,0x33630)` — 224 bytes; SHA-256 `ce94eb1374ca36b57d7e3e093c2b67b6caca5ba251a652ff242505d1519a274d`.
- Narrow `[0x33590,0x335F0)` — 96 bytes; SHA-256 `68ab7d0454918234d068d3050dc6338ee5c27aca8357b68a4e7ea35e078621af`.
- Narrow bytes: `05ee550200a324c700000fbf05f6550200a320c700000fbf05d6550200a31cc700000fbf05d2550200a318c70000c7052cc70000ffffffffc7052022020000080000e87df3ffff5e5f5bc9c3c800000060be185a0100bf00000000c705d0fc00`.

Interpretation: `335DB` is the final RET and `335DC` begins the next independent routine with no separator bytes. No Batch 91 bytes are counted.
