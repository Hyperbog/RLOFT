# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `a83c3afe1e6cd9e5795da9884fa4326c631378ff` — `Integrate CODE.2 decompilation batch 89`.
- Verified translated instruction bytes: **212,789**.
- Proven non-code/data bytes: **4,267**.
- Reconciled object bytes: **217,056 / 239,698 = 90.5539% (90.55%)**.
- Unresolved object bytes: **22,642**.
- Last integrated ownership fold: `333BC..334C3`.
- Next independently decoded instruction boundary: **`334C4`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33440,0x33520)` — 224 bytes; SHA-256 `d06e1c392a10f77f1c1951774868c8117d767670b9b3ac78ffdc015446ce0fe9`.
- Narrow `[0x33480,0x334E0)` — 96 bytes; SHA-256 `278c34db2127df98a8ff4fd0e615012f0756d92de6841ad22b209071025a2f1d`.
- Narrow bytes: `008987205a010081bf205a010093000000750ac70520220200000000008b87185a01003b05202202007d05a3202202003b0534bf00007e05a334bf00005f5e5bc9c387dbc80000005357560fb705195602000fb71d1b5602000fafc3a30cfd00`.

Interpretation: `334C1` is the final RET; `334C2..334C3` is unreferenced `XCHG EBX,EBX` alignment; `334C4` begins the next independent routine. No Batch 90 bytes are counted.
