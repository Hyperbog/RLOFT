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

