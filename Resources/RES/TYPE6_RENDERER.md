# RES type-6 column-major surface textures

Status: **the supplied type-6 resources are confirmed as LZSS-compressed first-person renderer textures stored as 128-texel vertical columns. The two supplied body sizes represent 32x128 and 64x128 source textures.**

The immutable RES files and original CODE.2 bytes are controlling evidence.

## Load and column-selection path

Type 6 is loaded by the shared `37590` LZSS wrapper. After decompression the runtime allocation begins with the preserved 16-bit decoded-size word, followed by the decoded body.

First-person renderer setup constructs a 0x1C-byte render record. At CODE.2 `32F93..32FA0` it derives the record field later stored at `+0x10` as:

```text
column_selector = (0x255E8 >> 4) & 0x0FC0
```

Therefore the stored selector can change only in `0x40` increments.

At `33647..33664` the renderer selects archive 4, loads the type-6 resource through `2F29C`, skips the leading decoded-size word, and computes:

```text
source = decoded_body + 2 * render_record.field_0x10
```

Because `field_0x10` advances in `0x40` steps, the effective source pointer advances in exactly:

```text
2 * 0x40 = 0x80 = 128 bytes
```

The equivalent changed-record path at `336B7..336D4` performs the same calculation.

Thus the body is addressed in non-overlapping 128-byte blocks; this is not a merely numerical factorization of the decoded size.

## Vertical sampler `338C8`

The principal consumer receives:

```text
arg1 : color/shading subtraction
arg2 : requested output height
arg3 : selected source column pointer
arg4 : VGA destination pointer
```

CODE.2 `338D3..338E5` computes:

```text
step = 0x800000 / output_height
```

`0x800000 == 128 << 16`, so the fixed-point source domain is exactly 128 samples.

For each output scanline, `3393F..3394F`:

1. obtains `source_index` from the high 16 bits of the fixed-point accumulator;
2. reads one byte from `source_column[source_index]`;
3. subtracts the low byte of `arg1`;
4. writes the resulting palette-index byte to VGA memory;
5. advances destination by `0x50` bytes;
6. advances the fixed-point source accumulator.

`0x50 == 80` bytes is one 320-pixel VGA planar scanline. The routine therefore vertically rescales one 128-texel source column to the requested on-screen height.

The clipping/centering arithmetic at `338E8..3392C` keeps the output within the engine's 256-line first-person working region.

## Supplied dimensions

The complete type-6 corpus contains only:

- 134 decoded bodies of 4,096 bytes;
- 1,185 decoded bodies of 8,192 bytes.

With the executable-proved 128-byte column stride:

```text
4096 / 128 = 32 columns
8192 / 128 = 64 columns
```

Therefore the supplied resources are exactly:

- **32 x 128 texels** — 134 resources;
- **64 x 128 texels** — 1,185 resources.

Storage is column-major: each contiguous 128-byte block is one complete vertical texture column.

## Rendering context

Calls at `32886`, `328B3`, and `328E4` pass the selected column pointer to `338C8` while selecting VGA planes and advancing destination offsets. This places type 6 directly in first-person planar scene rendering.

The byte values are written as palette indices after a per-draw subtraction, establishing a shading/color-bias operation on the source texels.

The safe project classification is therefore **column-major first-person surface/wall texture raster**. A narrower art/editor label such as “wall set,” “floor texture,” or a particular material class remains consumer-dependent and is not generalized to every archive-4 entry without index-level mapping.

## Validation boundary

`analyze_res.py` verifies for every supplied type-6 resource that:

- LZSS output equals the declared decoded size;
- the compressed stream is consumed exactly;
- decoded size is divisible by the executable-proved 128-byte column stride;
- supplied column count is exactly 32 or 64.

## Remaining work

- map individual archive-4 resource IDs to level/environment material selectors;
- correlate the 32-column versus 64-column families with renderer geometry and level themes;
- export representative textures in row-major viewing form without changing the canonical column-major bytes.
