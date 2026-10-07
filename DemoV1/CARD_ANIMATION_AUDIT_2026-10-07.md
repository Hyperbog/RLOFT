# CODE.1 card transition audit — 2026-10-07

## Scope

This audit rechecked the character-creation card animation against the supplied original `CODE.1` binary, the reconstructed `Code1/Code1.cpp`, and the DOSBox recording used for live comparison.

## 10A4 fly-in

The 12-step card-flight path remains correct after the earlier raw-arithmetic fixes. It uses the original resource sequence at DGROUP 0964 and the original A5C8 flags at 0970. No subjective timing or interpolation is introduced.

## 07E4 reveal

The raw instruction stream at object-1 offsets 0C45..0E29 exposes a semantic error in the previous high-level reconstruction.

For the new-card half (`7 <= k < 12`) the executable performs two distinct calculations:

```
offset = 12 - k
scale  = 6 - offset   // k - 6
screen_column += 4 * offset
9C44(direction, scale, 6, 1, 1, image, row, screen_column)
```

The previous reconstruction incorrectly reused `offset` as the `9C44` scale numerator. For a 48-pixel-wide card that produces the backwards width sequence:

```
40, 32, 24, 16, 8, 48
```

The original binary produces:

```
8, 16, 24, 32, 40, 48
```

This exactly matches the smooth seam-to-face expansion visible in DOSBox.

## 07E4 phases 0 and 1

The earlier conclusion that phases 0/1 are globally invisible was too broad. They call `55DC`, which copies a rectangle from the draw page to the opposite page. However, the encoded rectangle is:

```
row = 0xEC
column = 0
height = 0xA4
width = 0x140
```

That is the lower UI region (rows 236..399). The card slots are above it, so the card reveal itself first becomes visible when phase 2 performs `53E4`. The host's visible-card phase mapping therefore remains phase 2 onward, but its comments were corrected to describe the actual reason.

## C9D4/CACC scaling

The raw scaler uses a denominator-half accumulator and discrete source-column replication/skipping. DemoV1's horizontal scaling helper matches that nearest-neighbor/Bresenham-style behavior; no bilinear interpolation belongs in this path.

## Result

The canonical Code1 reconstruction and every DemoV1 07E4 reveal path now use `k-6` as the expansion numerator while retaining `12-k` only for the horizontal origin offset.
