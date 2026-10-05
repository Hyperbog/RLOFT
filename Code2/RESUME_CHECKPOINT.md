# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `a39f548b5f0a4da0f7dd8d70651ac6b5398c1355` — `Integrate CODE.2 decompilation batch 105`.
- Verified translated instruction bytes: **216,362**.
- Proven non-code/data bytes: **4,310**.
- Reconciled object bytes: **220,672 / 239,698 = 92.0625% (92.06%)**.
- Unresolved object bytes: **19,026**.
- Last integrated ownership fold: `34270..342E3`.
- Next independently decoded instruction boundary: **`342E4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x342A0,0x34380)` — 224 bytes; SHA-256 `dfbe6a862574bc8dd38fc0bf84e10df16dba1e3d048d280ce733277bb8bc0908`.
- Narrow `[0x342D0,0x34330)` — 96 bytes; SHA-256 `4f30b67009a7a461a31b3b29e4d3bec0690e24d48267cc61845c885129e26c23`.
- Narrow bytes: `b704bb40c1e802f7c2020000007402f7d8c387db8f05580201008b3d40ff000066bac40366b8020866ef5a58c1e502c1e302e8490000008b3d40ff000066bac40366b8020466ef5a58e8320000008b3d40ff000066bac40366b8020266ef5a58`.

Interpretation: `342E1` is the shared helper RET; `342E2..342E3` is unreferenced `XCHG EBX,EBX` alignment; `342E4` begins the independently called stack-sharing renderer. No Batch 106 bytes are counted.
