# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `0294c04a57aa0f81b43bf43173f187f8118474ce` — `Integrate CODE.2 decompilation batch 112`.
- Verified translated instruction bytes: **221,173**.
- Proven non-code/data bytes: **4,438**.
- Reconciled object bytes: **225,611 / 239,698 = 94.1230% (94.12%)**.
- Unresolved object bytes: **14,087**.
- Last integrated ownership fold: `35418..3562E`.
- Next independently decoded instruction boundary: **`3562F`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x355D0,0x356B0)` — 224 bytes; SHA-256 `ecd5a645438b040bd0094921dc102f1ee2372b4faf128e4c08e6d6257aaa3cd9`.
- Narrow `[0x35600,0x35660)` — 96 bytes; SHA-256 `9dbad0bf97988fd2037d161af95bc52432793c9e91a635cb035b7790a5c411ae`.
- Narrow bytes: `ab88d180e103f3aa3eb80cbd020083c00f24f0a33826010089353c260100b8ff000000e8ba3f000029ede85a3f000058eb235250ba1454030066b8013dcd216689c35a89d6fcac3c0075fb89f129d149b440cd215850b800000000baff000000`.

Interpretation: `35418..3562E` is the translated DOS/extender startup body. `3562F` begins a shared continuation with `POP EAX`; it is reached by fall-through after `CALL 39589` and independently by the earlier `352E9` jump, so no `3562F+` bytes are counted in Batch 112.
