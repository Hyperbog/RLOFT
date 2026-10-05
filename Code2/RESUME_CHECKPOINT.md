# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `850c681c044199a397cd1fdc26c49337b54b7002` — `Integrate CODE.2 decompilation batch 111`.
- Verified translated instruction bytes: **220,638**.
- Proven non-code/data bytes: **4,438**.
- Reconciled object bytes: **225,076 / 239,698 = 93.8998% (93.90%)**.
- Unresolved object bytes: **14,622**.
- Last integrated ownership fold: `35286..35417`.
- Next independently decoded instruction boundary: **`35418`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x353C0,0x354A0)` — 224 bytes; SHA-256 `311411d5e4a05a73aa9bc251571dfc9324d8f68e944d92bdb33cfd50704c8461`.
- Narrow `[0x353F0,0x35450)` — 96 bytes; SHA-256 `794d181d678b2b9605534d67fe1d31c4d08c22387b40e828fa049f34e5efb3dc`.
- Narrow bytes: `38382d313939342e20416c6c207269676874732072657365727665642e8d400003000000636f6e00fb83e4fc89e3891d48260100891d3426010066b8240066a340260100bb5241485029c0b430cd21a26b26010088256c26010089c129f6bf81`.

Interpretation: `3539F` is the decimal-parser RET; `353A0` is a two-byte jump to `35418`; `353A2..35417` is proven embedded runtime data skipped by that jump, with `35414` directly referenced as the `con` string. No Batch 112 bytes are counted.
