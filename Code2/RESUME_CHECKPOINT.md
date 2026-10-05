# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `0eaaa77fdebd6a3cb2872287d5031a36fa3fcf6e` — `Integrate CODE.2 decompilation batch 121`.
- Verified translated instruction bytes: **224,357**.
- Proven non-code/data bytes: **4,520**.
- Reconciled object bytes: **228,877 / 239,698 = 95.4856% (95.49%)**.
- Unresolved object bytes: **10,821**.
- Last integrated ownership fold: `369D8..36B9B`.
- Next independently decoded instruction boundary: **`36B9C`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x36B50,0x36BD0)` — 128 bytes; SHA-256 `d481c821666967c4dda4207e7a61e96a6e185e8974fae4bf965394efa3842218`.
- Narrow `[0x36B70,0x36BD0)` — 96 bytes; SHA-256 `5c1c428a375bf5c096c81e4de2b2107ae7d64a20efd15cfc7fad52a1e90fd3fa`.
- Overlap bytes are verification only and must not be recounted.

## Batch 121 interpretation
- `369D8..36B12`: three-plane runtime callback renderer with temporary callback-image byte patching.
- `36B13`: one-byte NOP alignment.
- `36B14..36B98`: single/multi-pass wrapper with AL/AH packed-state swapping.
- `36B99..36B9B`: three-byte no-op alignment.
- `36B9C` begins the next runtime helper and remains uncounted.
