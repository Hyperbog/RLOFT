# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `caaef69a73d361a1171cb348538ba89d1cd85cc0` — `Integrate CODE.2 decompilation batch 97`.
- Verified translated instruction bytes: **214,246**.
- Proven non-code/data bytes: **4,274**.
- Reconciled object bytes: **218,520 / 239,698 = 91.1647% (91.16%)**.
- Unresolved object bytes: **21,178**.
- Last integrated ownership fold: `33A38..33A7B`.
- Next independently decoded instruction boundary: **`33A7C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33A20,0x33B00)` — 224 bytes; SHA-256 `0fdc03f66e880476d640ec992a83cff79cba3935ff2b887a497fde97b5ca81f6`.
- Narrow `[0x33A50,0x33AB0)` — 96 bytes; SHA-256 `e5efc88a737d8cdb7a098f8967cd0355dc48ceb0ac1363cc64a18b2818f1864d`.
- Narrow bytes: `56000083c404c331c0c38b542404b43bcd21d1d0d1c885c07d0f25ffff000050e86256000083c404c331c0c3535657558b6c241c8b7424148b7c24188b5c242089eae8b15600008b45005d5f5e5bc30083ec286a0c6a008d44242450e87fd2ff`.

Interpretation: `33A7B` is the final RET of the DOS chdir wrapper; `33A7C` begins the independently called generic interrupt-frame adapter. No Batch 98 bytes are counted.
