# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `000669e03b6abb9f83d2250baba5539e643cba8b` — `Integrate CODE.2 decompilation batch 81`.
- Verified translated instruction bytes: **209,054**.
- Proven non-code/data bytes: **4,254**.
- Reconciled object bytes: **213,308 / 239,698 = 88.9903% (88.99%)**.
- Unresolved object bytes: **26,390**.
- Last integrated ownership fold: `324CC..3261F`.
- Next independently decoded instruction boundary: **`32620`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x325A0,0x32680)` — 224 bytes; SHA-256 `e914b8e4218820b3de9165204c6220c4bd29d07b747a92aa9035108ddee30154`.
- Narrow `[0x325F0,0x32650)` — 96 bytes; SHA-256 `85c2553d62623a7755ad0c1a85ee51b6bf4c860ab3f4a8a4ee0069f215228e35`.
- Narrow bytes: `04950000000029d00344241050e8eaecffff83c40cff742408e842fbffff83c40453e8c5ecffff83c4045bc300000000c80000005657518b7d0c8b4d108b35e4550200ff5508595f5ec9c390c82c0000535657813d00c70000930000000f84fa`.

Interpretation: `3261B` is the final RET; `3261C..3261F` is four zero padding bytes with no incoming direct reference; `32620` begins the next independent thunk. No Batch 82 bytes are counted.
