# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `c12b39b843d8427c1b8cc12bc36d1e2ed837ada1` — `Integrate CODE.2 decompilation batch 120`.
- Verified translated instruction bytes: **223,909**.
- Proven non-code/data bytes: **4,516**.
- Reconciled object bytes: **228,425 / 239,698 = 95.2970% (95.30%)**.
- Unresolved object bytes: **11,273**.
- Last integrated ownership fold: `369A7..369D7`.
- Next independently decoded instruction boundary: **`369D8`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x36970,0x36A10)` — 160 bytes; SHA-256 `a8d4a90ed98196de59517df1d6909c63491233acdccf41e243da470637c7aa04`.
- Narrow `[0x36990,0x369F0)` — 96 bytes; SHA-256 `e4aff1185e7663787b0eeb04b1793730035c68d2d42fb1c62423447857a1b0c3`.
- Overlap bytes are verification only and must not be recounted.

## Batch 120 interpretation
- `369A7..369B1`: 32-bit modular absolute-value helper.
- `369B2..369BF`: fourteen zero bytes, directly proven non-code.
- `369C0..369D7`: register-call adapter loading `ECX/ESI/EDI` before the indirect call.
- `369D8` begins the next runtime renderer and remains uncounted.
