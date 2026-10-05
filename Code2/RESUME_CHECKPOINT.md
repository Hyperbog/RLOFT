# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `e6d112ceecd878bdb2132a59d8ce46b059174d05` — `Integrate CODE.2 decompilation batch 88`.
- Verified translated instruction bytes: **212,527**.
- Proven non-code/data bytes: **4,265**.
- Reconciled object bytes: **216,792 / 239,698 = 90.4438% (90.44%)**.
- Unresolved object bytes: **22,906**.
- Last integrated ownership fold: `332B4..333BB`.
- Next independently decoded instruction boundary: **`333BC`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33340,0x33420)` — 224 bytes; SHA-256 `9bbd52a3e5738767b6bf848abbfb0af1d475adb687457691279939029c22c5c0`.
- Narrow `[0x33380,0x333E0)` — 96 bytes; SHA-256 `ed8cfebf5398043051891070a2a71940b5a9bc707e5a21b2f72710f64496052c`.
- Narrow bytes: `bf205a010093000000750ac70520220200000000008b87185a01003b05202202007d05a3202202003b0534bf00007e05a334bf00005f5e5bc9c387dbc800000053565766813df055020000807219a1f05502000fb71de4fc000081eb00800000`.

Interpretation: `333B9` is the final RET; `333BA..333BB` is unreferenced `XCHG EBX,EBX` alignment; `333BC` begins the next independent routine. No Batch 89 bytes are counted.
