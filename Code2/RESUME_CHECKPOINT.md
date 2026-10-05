# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `3ae7bac67814974f31591a2e4847c6e42232c27f` — `Integrate CODE.2 decompilation batch 83`.
- Verified translated instruction bytes: **209,871**.
- Proven non-code/data bytes: **4,257**.
- Reconciled object bytes: **214,128 / 239,698 = 89.3324% (89.33%)**.
- Unresolved object bytes: **25,570**.
- Last integrated ownership fold: `3263C..32953`.
- Next independently decoded instruction boundary: **`32954`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x328D0,0x329B0)` — 224 bytes; SHA-256 `a99a89b476a127cc5ad6cb006537b2696ee0ef37076cc9c3d0941ae167b0bac5`.
- Narrow `[0x32920,0x32980)` — 96 bytes; SHA-256 `d3e658d4448c48d94f3448772defa2b81ef338b273975edc8ee4d2a1b387e54d`.
- Narrow bytes: `fcffff83c40cff45f88aa6a8c6000080fc007419b00266bac40366efff75f0ff75f8ff75fce8d6fcffff83c40c5f5e5bc9c387dbc8000000c705e8fc0000ffffffffc705ecfc0000ffffffff833d50cf0000007527a158c60000390504fd0000`.

Interpretation: `32951` is the final RET; `32952..32953` is unreferenced XCHG EBX,EBX alignment; `32954` begins the next independent routine. No Batch 84 bytes are counted.
