# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 04 — VGA Display Architecture, Planar Rasterization, Palettes, and First-Person Rendering

**Status:** PRIMARY GRAPHICS-PIPELINE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the original graphics pipeline from video-mode initialization through final VGA writes:

- the BIOS-mode-13h-derived display configuration;
- unchained four-plane 256-color VGA memory;
- CPU draw-page versus visible display-page ownership;
- 320-pixel addressing and 80-byte planar scanlines;
- 200-line and 400-line page configurations;
- clipping and page-local coordinate translation;
- screen capture/restore;
- direct pixel, line, rectangle, and sprite drawing;
- palette streaming and runtime palette transformation;
- first-person RES type-6 surface-texture sampling;
- RES type-7 compiled scene-shape rendering;
- SINTAB scale-specific renderer blocks;
- environment/backdrop presentation;
- cinematic framebuffer geometry where it intersects the common VGA subsystem.

The immutable CODE.1/CODE.2 bytes and resource files remain controlling evidence. CODE.1 is especially useful for the low-level graphics primitives because the CODE.2 reconstruction established a direct instruction-equivalent counterpart family with a fixed code-address relocation for the large VGA/image/sprite block. [CODE.2 continuity](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md) [CODE.1 reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 2. Terminology

The machine code establishes the following sequence:

1. ensure BIOS video mode `13h`;
2. reprogram VGA sequencer, graphics-controller, and CRTC registers;
3. disable the packed/chain behavior expected from ordinary BIOS mode 13h;
4. operate the framebuffer through four independently selectable planes;
5. use 80 address bytes per logical 320-pixel row.

The safest preservation term is **unchained four-plane 256-color VGA mode derived from BIOS mode 13h**. The original binary does not provide a source-level mode name, so the archive should describe the actual register/memory behavior instead of assigning a later conventional nickname. [Code1.cpp `function_9ca0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 3. Video-mode initialization

CODE.1 `0x9CA0..0x9DF3` records the core geometry/state:

```text
row pitch               0x50 bytes = 80 bytes per plane
video aperture base     0xA0000
plane count             4
logical width           0x140 = 320 pixels

normal page step        0x400 paragraphs = 0x4000 bytes
mode-3 page step        0x800 paragraphs = 0x8000 bytes

normal logical height   0x0C8 = 200
mode-3 logical height   0x190 = 400
```

It begins from BIOS mode `13h` if required, then modifies VGA registers through ports `0x3C4/0x3C5`, `0x3CE/0x3CF`, and `0x3D4/0x3D5`. The exact read/modify/write sequence is retained in the canonical reconstruction. [Code1.cpp `function_9ca0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

The ordinary shutdown path restores BIOS text mode 3. [Code1.cpp `function_9df4`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 4. Pixel-to-planar address mapping

The coordinate translator at CODE.1 `0xAEA8..0xAEDB` proves:

```text
byte_address = active_page_base + y * 80 + floor(x / 4)
plane        = x & 3
```

It computes `y*80` as `y*16 + y*64`. [Code1.cpp `function_aea8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

Thus:

```text
x=0 -> plane 0, byte 0
x=1 -> plane 1, byte 0
x=2 -> plane 2, byte 0
x=3 -> plane 3, byte 0
x=4 -> plane 0, byte 1
```

and `320 / 4 = 80` bytes per scanline per plane. The same `0x50` stride appears independently in the type-6 sampler and cinematic player. [Type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md) [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)

## 5. Draw page and visible display page

The subsystem separates the page used for CPU drawing/reading from the page displayed by the VGA CRTC.

CODE.1 higher-level display state keeps:

```text
DGROUP +0x23E5   active CPU/draw page
DGROUP +0x23E6   visible display page
```

The draw page changes through `6588 -> A35C -> AEDC`. The visible page changes through `65A4 -> A36C -> AF64`. [Code1.cpp display/page functions](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

`AEDC` computes:

```text
active_page_base = 0xA0000 + page * page_step
```

where page step is `0x4000` in the 200-line configuration and `0x8000` in the 400-line configuration. [Code1.cpp `function_aedc`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

`AF64` writes CRTC start-address registers `0x0C` and `0x0D`, with interrupts temporarily disabled, then synchronizes to retrace. [Code1.cpp `function_af64`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

This is true page flipping: a completed draw page can become visible by changing CRTC start address rather than copying an entire frame.

## 6. Page geometry and padding

For 320×200:

```text
80 * 200 = 16,000 = 0x3E80 bytes/plane
page step = 0x4000
padding   = 0x180 = 384 bytes
```

Four `0x4000` pages fill the 64 KiB per-plane window.

For 320×400:

```text
80 * 400 = 32,000 = 0x7D00 bytes/plane
page step = 0x8000
padding   = 0x300 = 768 bytes
```

Two `0x8000` pages fill the same 64 KiB per-plane window.

These values are derived directly from the executable's page-step constants and address translator. [Code1.cpp `function_9ca0`, `function_aedc`, `function_aea8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 7. 320×200 is not a universal internal geometry

The executable-proven initializer supports both 320×200 and 320×400 configurations, while the cinematic player decodes type-2 packets into 320×400 chunky frames before planar conversion. [Code1.cpp `function_9ca0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)

The first-person type-6 sampler independently clips/centers into a 256-line working region. [Type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

Later port/remaster documentation must therefore distinguish visible presentation size, active VGA page size, first-person working region, cinematic decoded-frame geometry, and UI clip rectangles.

## 8. Retrace synchronization

CODE.1 polls port `0x3DA`, bit `0x08`, through two complementary synchronization helpers. One waits for the next retrace start; the other brackets the opposite transition. CRTC page-start changes and DAC palette writes are coordinated with these boundaries. [Code1.cpp `function_af34`, `function_af4c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

A native implementation need not busy-poll VGA hardware, but it should preserve the original presentation ordering for fidelity tests.

## 9. Direct pixel access

CODE.1 `0x9E14` translates `x,y`, selects read/write plane state, performs the encoded dummy video-memory read, and writes the pixel byte. The dummy read was specifically restored during the semantic audit because it loads VGA latches. [Code1.cpp `function_9e14`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [CODE.1 final audit checkpoint 10a](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

`0x9E60` selects the requested read plane and returns the byte at the translated planar address. [Code1.cpp `function_9e60`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 10. Line and rectangle rasterization

`0x9EB0..0xA00D` computes first/last byte plane masks, middle span, row stride, and raster state for planar rectangle fills. [Code1.cpp `function_9eb0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

`0xA05C..0xA219` handles vertical, horizontal, and general Bresenham-style lines. Plane masks progress:

```text
1 -> 2 -> 4 -> 8 -> 1
```

and the byte address advances on the `8 -> 1` wrap. The final audit corrected an earlier reconstruction that had incorrectly modeled eight mask phases. [Code1.cpp `function_a05c`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [CODE.1 final audit checkpoint 10b](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

## 11. Clipping

CODE.1 `0x6654..0x6851` wraps lower-level display operations with signed clipping against current logical width/height. Rectangle clipping adjusts negative origins and truncates right/bottom extents. [Code1.cpp `function_66c0`, `function_6744`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

The image/sprite clipping family beginning at `0x6854` separately tracks clipped source offsets and retained spans before entering planar sprite routines. The final audit corrected source-offset initialization and machine-width arithmetic there. [CODE.1 final audit checkpoint 5b](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)

## 12. Page copying

CODE.1 `0xA390` copies the current active page into another page using VGA copy mode and all-plane writes. `0xA3E8` performs cross-page rectangle copies by temporarily changing source-page addressing, computing the source, restoring the destination page, and copying row spans. [Code1.cpp `function_a390`, `function_a3e8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 13. Screen-region capture and restore

`0xA484` captures a rectangle by selecting VGA read planes 0..3 and copying each plane into consecutive memory. `0xA4E8` restores that representation with write masks 1,2,4,8. [Code1.cpp `function_a484`, `function_a4e8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

The native capture format is therefore four plane blocks rather than a conventional packed row-major bitmap.

## 14. Runtime image/sprite drawing

CODE.1 `0xA6F0` renders runtime images in four VGA plane passes. It derives the initial plane from `x&3`, writes rows into the selected plane, rotates the plane mask, and advances the destination byte address as phase wraps. [Code1.cpp `function_a6f0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

The runtime descriptor supplies frame stride, row count, per-plane row width, optional transparency-key behavior, and frame data. Original source-level field names remain unknown.

## 15. Transparency

Descriptor flag bit `0x80` selects a literal palette-index transparency key. When set, the first frame byte is the key and equal source bytes leave the destination unchanged. [Code1.cpp `function_a6f0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

A native renderer may represent this as alpha internally, but compatibility serialization should preserve the original key byte.

## 16. Palette-index subtraction

CODE.1 `0xA7E8` subtracts a supplied low-byte value from each nontransparent source palette index before writing. The type-6 first-person sampler performs the same class of byte-sized subtraction. [Code1.cpp `function_a7e8`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [Type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

The safe term is **palette-index subtraction/shading bias**; exact visual meaning depends on the caller.

## 17. Palette translation

CODE.1 `0xA8F4` translates each written source byte through a 256-entry table. The reconstruction records that this table replaces the semantic role of original self-patched lookup displacement state. [Code1.cpp `function_a8f4`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

The graphics system therefore supports direct copy, subtracting copy, and table-translated copy, with transparent/clipped variants.

## 18. VGA palette hardware

CODE.1 `0xA010..0xA058` writes the DAC index to port `0x3C8` and RGB components to `0x3C9`. Output is broken into bursts of at most `0xC0` bytes with synchronization between bursts. [Code1.cpp `function_a010`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

Higher-level wrappers send either all `0x300` bytes or selected color ranges. [Code1.cpp `function_6618`, `function_6634`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)

## 19. COLORS palette bank

`COLORS` contains one count byte followed by 15 palettes of 256 RGB triplets. Every component lies in the VGA DAC 0..63 domain. CODE.2 seeks directly to `1 + palette_index*768` and can load partial ranges. [COLORS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/COLORS/FORMAT.md)

The leading count byte is structurally exact but is not used as a bounds guard by the active loader.

## 20. Runtime palette intensity transform

After loading COLORS data, CODE.2 `0x32150..0x3221E` raises each RGB triplet's maximum component toward 63 according to a runtime level and proportionally rescales nonzero channels using integer rounding. Black remains black. [COLORS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/COLORS/FORMAT.md)

This is an in-memory transformation; source palette bytes remain unchanged.

## 21. Cinematic palette sharing

ANM chunk type 0 supplies 768 palette bytes and applies them through the same CODE.2 palette-application family used by normal game presentation. Type 1 performs the blackout/reset path. [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)

## 22. ANM framebuffer geometry

ANM type-2 image packets decode 400 rows of 320 chunky pixels before planar conversion. Type-8 packets contain four `0x5000` plane blocks. The destination begins at page offset `0x0FF0`:

```text
0x5000 / 80 = 256 scanlines
0x0FF0 / 80 = 51 scanlines
```

so the raw type-8 image occupies scanlines 51..306 inside the 320×400 page. [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)


## 23. First-person type-6 textures are column-major

RES type 6 decodes to one of two body sizes:

```text
4096 bytes = 32 columns * 128 texels
8192 bytes = 64 columns * 128 texels
```

The renderer's source selector advances in `0x40` units and is doubled before indexing, producing an exact `0x80` / 128-byte source-column stride. The selected body therefore consists of complete vertical columns, not row-major scanlines. [RES type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

The physical interpretation is:

```text
column 0: 128 vertical texels
column 1: 128 vertical texels
...
```

This is a storage fact established by the executable's address arithmetic, not merely by factoring the decoded byte counts.

## 24. Type-6 vertical sampling

CODE.2 `0x338C8` receives one selected 128-texel source column and a requested output height. It computes:

```text
step = 0x800000 / output_height
```

Since:

```text
0x800000 = 128 << 16
```

the source domain is exactly 128 samples expressed through fixed-point phase arithmetic. For each destination row, the routine selects a source texel from the high phase bits, applies the low-byte subtraction/shading value, writes one palette-index byte, advances the destination by `0x50`, and advances the source phase. [RES type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

The CODE.2 reconstruction preserves the original signed division, arithmetic shifts used during clipping, low-byte loop-count behavior, source-index formation, and destination stride. [CODE.2 continuity, Batch 94](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

The sampler clips/centers against a 256-line first-person working region. That 256-line domain is another reason not to reduce the entire original renderer to a single abstract 320×200 surface.

## 25. RES4 contains two texture-width families

The active RES4 archive contains 667 type-6 resources and divides exactly at INIT value `600`:

```text
RES4:0..599     600 resources -> 64 x 128 texels
RES4:600..666    67 resources -> 32 x 128 texels
```

INIT `+0x0E = 600` is therefore the exact base of the secondary/narrow texture namespace. [INIT format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md) [RES type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

The dimensions are proven. A universal editor-facing label such as “wall texture” is intentionally withheld because individual material/level use is narrower than the storage format itself.

## 26. First-person scene objects use compiled shapes, not bitmap sprites

RES type 7 is structurally different from type 6 and ordinary UI raster resources.

Its decoded body begins:

```text
+0x00 uint16 source_width
+0x02 int16  x_min
+0x04 int16  x_max
+0x06 int16  y_min
+0x08 int16  y_max

+0x0A uint16 column_list_offset[x_max-x_min+1]
```

Each column pointer addresses a zero-terminated sequence of six-byte descriptors:

```text
int16 raster_stop_selector
int16 signed_adjustment
int16 raster_entry_selector
...
int16 0
```

The pointer itself may be odd; the original code performs unaligned word reads and supplied resources rely on that behavior. [RES type-7 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)

Across all 8,362 supplied type-7 bodies, the reconstructed geometry, pointer-table bounds, list termination, and selector constraints validate. Source widths are only 64, 96, or 128. [RES type-7 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)

The visible result may be called a sprite informally, but the original serialized form is a **compiled scene-shape column display list**.

## 27. Shape geometry is consumed before rasterization

CODE.2 `0x21BF0` reads the type-7 geometric header and scales its bounds into the runtime scene record. Later first-person scene paths repeatedly call `0x36CEC`, passing the scene record's resolved archive-3 resource ID. [RES type-7 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)

Thus one type-7 object supplies both:

- projected-bound geometry;
- per-column compiled draw instructions.

The renderer does not first decode a conventional 2D bitmap and then scale that bitmap.

## 28. RES3 scene identity is source-class-relative

Four INIT words partition all 4,234 RES3 type-7 entries:

```text
source class 0 base    0 -> RES3 0..2168
source class 1 base 2169 -> RES3 2169..3315
source class 2 base 3316 -> RES3 3316..4157
source class 3 base 4158 -> RES3 4158..4233
```

The common translation is:

```text
absolute_RES3_id =
    source_base[source_class]
  + bank_local_shape_id
```

The proved source families are:

```text
0  NPC/creature shapes
1  placed object/item family
2  loose/pickable OBJECTS family
3  transient/moving effects
```

[RES3 source-bank map](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)

This mapping belongs to scene construction, not the rasterizer itself. By the time the type-7 renderer requests archive 3, the local ID has already been translated to an absolute RES3 entry.

## 29. SINTAB is a complete renderer support package

The external `SINTAB` file is not merely a sine table.

Its supplied top-level structure contains:

```text
450 dwords  signed-magnitude Q16 sine family
900 dwords  Q16 tangent family
128 words   projection table A
128 words   projection table B
8 dwords    renderer constants
256 dwords  signed view-angle table
509 words   runtime lookup table
127         variable scale blocks
512 bytes   preserved zero trailer
```

CODE.2 loader `0x3838..0x3A63` consumes the fixed data and 127 scale blocks; the final zero trailer is serialized but ignored by the active loader. [SINTAB format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SINTAB/FORMAT.md)

Every scale block begins:

```text
block +0x000  uint16 code_patch_offset[129]
block +0x102  uint16 scale_step[129]
block +0x204  byte   rasterizer_x86[]
```

The loader publishes `block + 0x204`, the code-body address, into the runtime scale-code pointer table at DGROUP `0x18FE0`. All 127 supplied generated code bodies terminate in `RET`. [SINTAB format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SINTAB/FORMAT.md)

## 30. Type-7 descriptors select spans of scale-specific renderer code

The first and third words in each active type-7 descriptor are even byte selectors in the same 129-entry domain used by each SINTAB scale block.

CODE.2 `0x369D8` uses the stop selector to locate a byte in the selected scale-specific renderer body, saves the original byte, temporarily replaces it with `0xC3` (`RET`), resolves the entry selector, invokes the selected renderer entry through the register-call bridge at `0x369C0`, then restores the original byte. [RES type-7 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md) [CODE.2 continuity, Batches 120–122](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

Neighboring renderer helpers likewise consult the two 129-word control tables and temporarily alter selected control bytes before restoring them. The behavior is deliberate runtime specialization of the pre-generated scale code.

The historical path is therefore:

```text
type-7 column descriptor
        |
        v
selected SINTAB scale block
        |
        v
entry/stop selectors
        |
        v
bounded generated renderer span
        |
        v
planar VGA writes
```

## 31. Type-7 column traversal is centered on source width

CODE.2 `0x36CEC` traverses the type-7 pointer table around:

```text
source_width / 2
```

The left side is walked toward `x_min`; the right side is walked toward `x_max`. Duplicate pointer values are legal and allow several source columns to share a descriptor list. [RES type-7 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)

The renderer derives a scale selection from projected size, selects from the SINTAB pointer table, and uses the serialized runtime lookup and scale-step data while producing the projected shape. [SINTAB format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SINTAB/FORMAT.md)

## 32. The faithful port should translate SINTAB behavior, not discard it

A portable engine should not need writable executable x86 buffers as its primary renderer. But replacing the scale blocks with generic bitmap scaling without validation would remove original renderer behavior.

The preservation-compatible strategy is:

1. parse and retain the original SINTAB tables and scale blocks;
2. translate their behavior into safe portable C++ or equivalent renderer operations;
3. preserve the original integer lookup/selection rules;
4. compare translated pixel output against the original encoded semantics;
5. keep the original SINTAB bytes as canonical evidence.

The committed SINTAB analyzer already validates all block sizes, control tables, code ranges, terminators, aggregate payload size, and the 512-byte trailer. [SINTAB format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SINTAB/FORMAT.md)

## 33. Environment-profile backdrops are view-angle driven

The final INIT section contains two environment/backdrop profiles selected by `LEVELS.DAT` header byte `+0x290` when its value is 0 or 1; `0xFF` uses an alternate/default level path.

Each placement contains a resource selector, azimuth, draw width, and screen Y coordinate. CODE.2 `0x314B0` subtracts the current camera/view angle from the stored azimuth, wraps the result, projects horizontal screen position, resolves the RES8 graphic, clips by width, and draws at the stored Y position. [INIT environment profiles](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/ENVIRONMENT_PROFILES.md)

The safe preservation term is **environment-profile backdrop/panorama placement**. Exact original editor terminology remains unknown.

## 34. Rendering is strongly integer/fixed-point

The certified graphics code repeatedly depends on original machine-width arithmetic, including:

- signed `IDIV`;
- arithmetic versus logical shifts;
- 16-bit truncation and wrap;
- low-byte loop counts;
- modulo-2^32 multiply/add;
- byte-width palette-index arithmetic;
- signed clipping comparisons;
- `SHLD`-based source indexing;
- partial-register state.

Examples occur in the type-6 sampler, projected raster setup, and compiled-shape renderer. [CODE.2 continuity, Batches 94 and 103–109](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

A floating-point rewrite may look visually close while differing at clipping edges, sample boundaries, or exact palette indexes. The faithful port should therefore preserve integer reference behavior or regression fixtures for these paths.

## 35. Runtime renderer specialization occurs in multiple families

The SINTAB path is the clearest example, but the CODE.2 graphics audits identify additional runtime control/immediate patch sites in clipped blitters and adjacent rasterizer setup routines. The certification record preserves those patches and their restoration ordering rather than normalizing them away. [CODE.2 continuity, Batches 61, 107, 108, 121, 122](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

A modern renderer does not need to reproduce writable-code implementation mechanics. It does need to reproduce the resulting parameterized raster behavior.

## 36. UI, world, and cinematic rendering share one VGA substrate

The source families are distinct:

```text
RES0 / RES1 / RES2 / RES7 / RES8 / RES9 type 1
    -> raster/UI/image presentation

RES4 type 6
    -> vertically sampled first-person surfaces

RES3 type 7
    -> compiled projected scene objects

ANM
    -> streamed cinematic frames/deltas
```

but they converge on the same page, palette, plane-selection, clipping, and VGA-memory machinery. [Resources final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md)

A faithful native architecture should therefore keep **legacy decoding** separate from **legacy presentation semantics**. The renderer may share one final indexed-color surface abstraction while preserving the different source representations that feed it.

## 37. Historical graphics dependency graph

```text
                         COLORS
                            |
                            v
                     palette staging
                            |
                            v
                         VGA DAC
                            ^
                            |
              +-------------+-------------+
              |                           |
              |                        ANM palette
              |
LEVELS / INIT / NPCS / OBJECTS
              |
              v
      scene-record construction
              |
       +------+------------------+
       |                         |
       v                         v
  RES4 type 6                RES3 type 7
  128-texel columns          compiled shapes
       |                         |
       |                         v
       |                      SINTAB
       |                 lookup + scale code
       |                         |
       +------------+------------+
                    |
                    v
          first-person rasterization
                    |
                    v
             planar VGA pages

Parallel presentation:
RES0 / RES1 / RES8 / RES9 type-1 graphics
                    |
                    v
         planar image/sprite paths
                    |
                    v
             same VGA pages
```

The diagram is a reconstructed dependency model, not a claim about DreamForge's original source-file organization.

## 38. Faithful-port implementation boundary

A modern implementation can separate the original data model from host presentation:

```text
LegacyGraphicsData
    |
    +-- indexed palettes
    +-- runtime raster images
    +-- type-6 texture columns
    +-- type-7 compiled shapes
    +-- SINTAB tables/scale programs
    |
    v
FaithfulRasterModel
    |
    +-- original coordinate domains
    +-- original integer/fixed-point arithmetic
    +-- original clipping
    +-- palette-index operations
    +-- draw/display-page semantics
    |
    v
HostPresentationBackend
    |
    +-- software surface
    +-- platform window
    +-- optional hardware acceleration
```

The host backend is not the historical model. It is a transport/presentation layer below which the faithful behavior can be tested independently.

## 39. Suggested native compatibility concepts

A later C++ implementation should make the historical distinctions explicit rather than burying them in renderer globals. Suitable conceptual types include:

```text
LegacyVideoMode
    width = 320
    height = 200 or 400
    plane_pitch = 80
    page_stride = 0x4000 or 0x8000

LegacyPageState
    draw_page
    display_page

LegacyPlanarAddress
    byte_offset
    plane

LegacyPalette
    256 RGB triplets in original 0..63 component domain

LegacyRasterImage
    frame/layout/transparency metadata
    preserved palette-index bytes

LegacyTextureColumn
    128 source texels

LegacyCompiledShape
    geometry
    column-pointer table
    descriptor lists

LegacyScaleProgram
    SINTAB control tables
    canonical serialized program bytes
```

These are proposed native concepts, not recovered original C/C++ declarations.

## 40. Required graphics regression classes

Before the faithful port's graphics layer is considered certified, regression coverage should include the following.

### 40.1 Address translation

Verify:

```text
(x, y, page) -> original byte offset + plane
```

including page boundaries and edge coordinates.

### 40.2 Page behavior

Verify:

- changing the draw page does not implicitly change the visible page;
- changing the visible CRTC page does not corrupt draw-page state;
- full-page copies;
- cross-page rectangle copies;
- 200-line and 400-line page-stride behavior.

### 40.3 Primitive rasterization

Verify:

- direct pixel read/write;
- VGA-latch-dependent writes where observable;
- horizontal/vertical/general lines;
- plane-mask wrap;
- rectangle edge masks;
- signed clipping at every display edge.

### 40.4 Runtime raster images

Verify:

- frame selection;
- direct copy;
- transparency-key copy;
- palette-index subtraction;
- palette-table translation;
- clipped forms;
- capture/restore round trips.

### 40.5 Palette behavior

Verify:

- exact COLORS record reads;
- partial palette-range reads;
- 0..63 source component preservation;
- runtime intensity transform;
- full 768-byte palette state;
- ANM palette application.

### 40.6 Type 6

Verify:

- 32×128 and 64×128 families;
- exact 128-byte source-column selection;
- fixed-point source indices for representative output heights;
- vertical clipping/centering;
- byte subtraction/wrap;
- `0x50` planar destination stride.

### 40.7 Type 7 / SINTAB

Verify:

- header geometry;
- unaligned column-list pointers;
- duplicate list pointers;
- left/right traversal;
- scale-block selection;
- selector resolution;
- descriptor stop/entry behavior;
- portable replacement output against the original scale-program semantics.

### 40.8 Cinematics

Verify:

- type-2 320×400 decode;
- chunky-to-planar conversion;
- type-3 sparse planar deltas;
- type-8 four-plane 320×256 region at `0x0FF0`;
- page/palette sequencing.

The future **Faithful Native Port — Architecture and Compatibility Certification** should link these tests back to the original addresses and resource formats documented here.

## 41. Modern remaster boundary

After faithful execution is frozen and certified, the remaster may intentionally replace or extend:

- VGA page flipping with a modern presentation/swap model;
- palette-index shading with true-color or shader lighting;
- software scaling with GPU rendering;
- scale-program execution with modern geometry/raster pipelines;
- fixed logical output with arbitrary resolutions;
- original camera restrictions with free mouselook;
- original UI scaling with high-resolution interfaces.

Those are modernization features, not corrections to the historical renderer.

The project should continue to distinguish:

```text
original behavior
        |
        v
faithful native implementation
        |
        v
intentional remaster behavior
```

so a programmer can always determine which layer is responsible for a rendering result.

## 42. Correction C-03 — “320×200 framebuffer” is too narrow

Any project wording that describes the original renderer universally as a 320×200 framebuffer should be interpreted more precisely.

The executable proves:

```text
logical width              320
supported logical height   200 or 400
planar pitch               80 bytes/row/plane
page stride                0x4000 or 0x8000
```

The cinematic path independently proves a 320×400 decoded-frame model and a raw 256-line planar region. The first-person type-6 sampler independently uses a 256-line working region. [Code1.cpp `function_9ca0`](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp) [ANM format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md) [Type-6 renderer](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)

This is a **documentation clarification only**. It does not change the certified CODE.1/CODE.2 reconstruction.

## 43. Historical unknowns retained

The graphics pipeline is behaviorally reconstructed to the level required for faithful implementation, but historical/source-level questions remain:

1. DreamForge's original names for the two display configurations;
2. original names for many VGA globals and runtime image fields;
3. original authoring-tool names for type-7 descriptor fields;
4. identity and implementation details of the original generator that produced SINTAB scale-program bodies;
5. whether the development team used a conventional contemporary nickname for the unchained VGA mode;
6. original editor terminology for environment/backdrop profiles;
7. complete named-art indexing for every RES3 shape and RES4 texture;
8. exact intended visual labels for every palette-index subtraction/translation path.

These are preservation `UNKNOWN`s. They do not block faithful rendering of the supplied build.

## 44. Source ledger

### Executable reconstruction

- [Code1/Code1.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/Code1.cpp)
- [Code1/FINAL_BINARY_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md)
- [Code2/CODE2_CONTINUITY_ADDENDUM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)
- [Code2/FINAL_SEMANTIC_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)
- [Code2/FINAL_CERTIFICATION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

### Resource/renderer evidence

- [Resources/RES/TYPE6_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE6_RENDERER.md)
- [Resources/RES/TYPE7_RENDERER.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_RENDERER.md)
- [Resources/RES/TYPE7_SOURCE_BANKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)
- [Resources/MISC/SINTAB/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/SINTAB/FORMAT.md)
- [Resources/MISC/COLORS/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/COLORS/FORMAT.md)
- [Resources/MISC/INIT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/FORMAT.md)
- [Resources/MISC/INIT/ENVIRONMENT_PROFILES.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/MISC/INIT/ENVIRONMENT_PROFILES.md)
- [Resources/ANM/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/ANM/FORMAT.md)
- [Resources/FINAL_RESOURCE_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/FINAL_RESOURCE_AUDIT.md)

## 45. Architectural checkpoint

The graphics foundation is now:

```text
BIOS mode 13h
    |
    v
VGA register reconfiguration
    |
    v
unchained four-plane 256-color framebuffer
    |
    +-- 320 pixels wide
    +-- 80 bytes/row/plane
    +-- 200 or 400 logical rows
    +-- 0x4000 or 0x8000 page stride
    |
    +--> CPU draw-page selection
    |
    +--> independent CRTC display-page selection
    |
    +--> planar capture / restore / page copy
    |
    +--> UI/runtime raster images
    |
    +--> type-6 column texture sampler
    |
    +--> type-7 compiled shape
            |
            v
          SINTAB scale block
            |
            v
       scale-specific renderer behavior
            |
            v
         VGA planes

Palette side:
COLORS / ANM palette
    -> optional runtime intensity transform
    -> VGA DAC
```

This closes the graphics architecture at the historical-system level. The next preservation chapter should move upward into **world and level representation**: LEVELS.DAT and LEV state, coordinate systems, cell words, geometry, movement/collision, doors, dynamic cell records, scene-source generation, and the boundary between immutable level definitions and mutable runtime state.
