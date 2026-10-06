# COLORS VGA palette bank

Status: **the supplied `COLORS` file is reconstructed as a 15-record VGA palette bank. CODE.2 routine `325B0` proves the record stride, palette-index seek rule, RGB triplet width, full/partial color-range load interface, and post-load palette-brightness transform. The leading count byte is structurally exact but is not consulted by this CODE.2 loader.**

## Controlling evidence

- supplied file: `COLORS`
- bytes: `11,521` (`0x2D01`)
- SHA-256: `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5`
- open helper: CODE.2 `32588`
- palette-bank loader: CODE.2 `325B0..3261B`
- post-load transform: CODE.2 `32150..3221E`
- palette application wrapper: CODE.2 `29234`

The immutable file and certified CODE.2 object are controlling evidence.

## Exact physical layout

The supplied file is exactly:

```text
+0x0000  uint8 palette_count = 15
+0x0001  Palette palette[15]

Palette:
    uint8 rgb[256][3]     // R,G,B triplets; 768 bytes
```

Therefore:

```text
file_size = 1 + palette_count * 256 * 3
          = 1 + 15 * 768
          = 11,521

palette[i] offset = 1 + i * 768
```

All 15 supplied records are exactly 768 bytes and all RGB component bytes are in `0..62`, within the VGA 6-bit DAC domain `0..63`.

The first byte is safely preserved as `palette_count` because its value is 15 and the remaining file is exactly 15 complete records. However, CODE.2 `325B0` does **not** read or bounds-check this byte; it computes the requested palette offset directly. The semantic name is therefore reconstructed from exact file geometry, not from an executable read of byte zero.

## CODE.2 palette loader `325B0`

After opening the supplied filename through `32588 -> 312A0`, the routine computes:

```text
seek_offset = 1 + palette_index * 768
```

The instruction sequence is exact:

```text
3 * palette_index
<< 8
+ 1
```

which is `palette_index * 0x300 + 1`.

It then computes:

```text
read_bytes = color_count * 3
destination = palette_buffer + first_color * 3
```

and reads directly into that destination.

A safe behavioral signature is:

```text
load_palette_range(
    palette_buffer,
    first_color,
    palette_index,
    filename,
    color_count
)
```

The common full-palette call is:

```text
first_color = 0
color_count = 256
```

so exactly one 768-byte record is loaded.

The interface can address a color subrange, although the observed main calls normally request 256 colors and use an already allocated 768-byte staging palette.

## Filename proof

The static filename pointers passed to `325B0` resolve in CODE.2's data object to strings beginning at `COLORS`. Observed pointer values include:

```text
0x0388 -> "COLORS."
0x0390 -> "COLORS"
0x0590 -> "COLORS"
0x0654 -> "COLORS"  (substring inside neighboring static text)
0x0CF4 -> "COLORS"
0x0CFC -> "COLORS."
0x1128 -> "COLORS"
```

The helper simply forwards the pointer to the file-open routine. Thus the executable/resource linkage does not depend on extension guessing or community tooling.

Observed fixed palette indices at call sites include `0`, `9`, `12`, `13`, and `14`; other sites pass dynamic palette indices, including the current world/scene palette selection.

## Post-load transform `32150`

`325B0` calls `32150(palette_buffer)` after reading. The routine processes all 256 RGB triplets in place.

It obtains an intensity/brightness-style setting from the active video/configuration structure, clamps it to `0..63`, and returns immediately when it is zero.

For each RGB triplet it finds:

```text
m = max(r,g,b)
```

and computes a target maximum approaching 63 as the setting increases:

```text
boost  = ((63 - m) * level + 31) / 63
target = m + boost
```

For each nonzero channel and nonzero `m`, it rescales proportionally with integer rounding:

```text
channel = (channel * target + floor(m/2)) / m
```

Black remains black. Relative RGB ratios are approximately retained while the triplet's maximum component is raised toward the VGA maximum.

The settings UI stores a normal user-selectable value in the range `0..14` and copies it into the same configuration byte before reloading/applying the palette. The transform itself nevertheless clamps its input against `0..63`; preserve that executable behavior rather than narrowing the implementation to the observed UI range.

The safe project term is **palette brightness/intensity normalization**; the original DreamForge source-level option name is not claimed.

## Palette application

Multiple call sites immediately pass the transformed 768-byte staging buffer to `29234`, the same palette-application wrapper independently used by the cinematic subsystem. ANM chunk type 0 likewise carries 768-byte palette payloads, reinforcing the engine-wide 256×RGB palette representation.

The COLORS file stores the source palette banks. Brightness/intensity adjustment occurs after loading in RAM and does not alter the on-disk bytes.

## Corpus validation

`analyze_colors.py` verifies:

- exact immutable file SHA-256;
- `palette_count == 15` for the supplied file;
- exact `1 + count*768` file geometry;
- 256 RGB triplets per palette;
- all component values in `0..63`;
- exact offsets and per-palette SHA-256 values.

No palette records overlap and no bytes remain after record 14.

## Preservation rules

A lossless reader/editor should:

1. preserve the leading byte exactly even though current CODE.2 does not consult it;
2. preserve record order and the exact 768-byte RGB payloads;
3. retain 6-bit component values directly rather than silently expanding them to 8-bit and rounding them back;
4. model executable palette brightness as a runtime transform, not an on-disk rewrite;
5. bounds-check palette indices in modern tooling even though original `325B0` relies on its callers;
6. reproduce the original 11,521 bytes exactly when no edit is requested.
