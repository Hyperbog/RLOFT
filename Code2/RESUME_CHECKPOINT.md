# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `79b0ef8cdea90f3037ea5b24d38dd1f56b42d787` — `Integrate CODE.2 decompilation batch 124`.
- Verified translated instruction bytes: **225,087**.
- Proven non-code/data bytes: **4,602**.
- Reconciled object bytes: **229,689 / 239,698 = 95.8243% (95.82%)**.
- Unresolved object bytes: **10,009**.
- Last independently integrated island: `3736D..374D1`, with data `3740E..37432`.
- `374D2..374DF` remains unresolved; next independent routine begins at **`374E0`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x3732D,0x37560)` — 563 bytes; SHA-256 `fb03d884626319eb3660df9b273a8fa25074d61efbed16ea3b8b77d00cf2ff6a`.
- Batch-owned `[0x3736D,0x374E0)` — 371 bytes; SHA-256 `f3a491f2e6135bce5c6ed930471e07a9b85a43be572f539af57c1862afe97104`.
- Overlap bytes are verification only and must not be recounted.

## Batch 124 interpretation
- `3736D..3740D`: first unsigned/signed/wrapper radix conversion family.
- `3740E..37432`: second 37-byte lowercase radix alphabet.
- `37433..374D1`: second unsigned/signed/wrapper conversion family.
- Preserve 32-bit wrapping negation for signed decimal conversion; minus is emitted only for radix 10.
