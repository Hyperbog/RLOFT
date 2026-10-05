# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `618468634dbf30a67766d67a51611879abed56a3` — `Integrate CODE.2 decompilation batch 117`.
- Verified translated instruction bytes: **223,532**.
- Proven non-code/data bytes: **4,472**.
- Reconciled object bytes: **228,004 / 239,698 = 95.1214% (95.12%)**.
- Unresolved object bytes: **11,694**.
- Last integrated ownership fold: `35A0C..35F86`.
- `35F87` is proven one-byte alignment; `35F88` begins the already integrated timer/event family.
- The sequential runtime frontier now joins previously reconciled code. Continue by selecting the next range from the remaining unresolved islands; do not re-count `35F88+`.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35F40,0x35FC0)` — 128 bytes; SHA-256 `6087fade6f4d46311150e01d36440c6f30d845c897e0e4bd945dd652c3786628`.
- Narrow `[0x35F70,0x35FD0)` — 96 bytes; SHA-256 `8fbabc4f61a242b137def96eefce4e8057502f6e19bb05d0eee9cc4b81b16788`.

Interpretation: `35A0C..35F86` is the translated large loader, `35F87` is alignment, and `35F88` is already-owned timer/event code. Batch 117 preserved the loader's raw 02D0h frame aliasing, flag-1 memory/file split, flag-4 allocation path, 16-byte alignment rule, local dword tables, and relocation validation/store ordering. Any analysis beyond this committed accounting is `UNCOUNTED / NOT COMMITTED`.
