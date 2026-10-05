# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `42ed7e4fb0ee6fbab3b254227aab9eba38fc5a84` — `Integrate CODE.2 decompilation batch 114`.
- Verified translated instruction bytes: **221,731**.
- Proven non-code/data bytes: **4,465**.
- Reconciled object bytes: **226,196 / 239,698 = 94.3671% (94.37%)**.
- Unresolved object bytes: **13,502**.
- Last integrated ownership fold: `35680..35877`.
- Next independently decoded instruction boundary: **`35878`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x357F0,0x358D0)` — 224 bytes; SHA-256 `346b8672efe32399aaec41fd3c473d478cdec6f91d732835a675b61099a23b8a`.
- Narrow `[0x35860,0x358C0)` — 96 bytes; SHA-256 `7b3b9d08d56d4f000e36725fdf51a74623cf6e48dc7e85594aa756e3eb134648`.
- Narrow bytes: `07b80100000061c9c300000000000000202000002020000056578b74240c8b5424108b5c241cf644241401741e8b7c241889d901d65789c8c1e902f2a58ac880e103f2a45f8d041a5f5ec36a005256e8532b000083c40c53ff74241c56e84428`.

Interpretation: `35680..35868` is the translated fixed-point/projection helper family. `35797` is proven NOP alignment; `35869..3586F` is zero alignment; `35870..35877` contains two referenced `00002020h` dwords. `35878` begins the next independently called runtime copy/I/O helper.
