# CODE.2 2A6B0 — Original clipped four-plane VGA sprite rasterizer

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Original executable:** user-owned `CODE.2`, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
The original logical CODE.2 object-1 base is physical file offset `0x3AF24`.

**Scope:** Original native `29414` six-DWORD clipping output now has a
bounded native `2A6B0` raster backend for real transparent and opaque
clipped sprite bytes. This is reusable original-engine behavior, **not**
an end-to-end accepted DOS-native Level 22 demo frame.

## Assembly audit: entire 2A6B0..2A89E procedure

The original `2A6B0(frame,row,column,scratch,resource)`
reads the six source clipping DWORDs at original scratch offsets
`+0,+4,+8,+12,+16,+20`, created by the existing
`29414` model. It transforms them into **ten mutable
original video DWORD globals**:

| Original DGROUP | Source meaning |
| --- | --- |
| `F00D` | Top skipped sprite bytes, clip[0] >> 2 |
| `F011` | Bottom skipped bytes, clip[4] >> 2 |
| `F015` | Visible row count, clip[8] |
| `F019` | Per-plane left bytes, (clip[12]+3) >> 2 |
| `F01D` | Right skip bytes, clip[16] >> 2, or zero |
| `F021` | Visible per-plane byte width, rounded from clip[20] plus clip[16]&3 |
| `F025` | Original low-two-bit alignment of clip[12] |
| `F029` | Nonzero flag when clip[16] != 0 |
| `F02D` | Initial completely skipped planar passes for narrow clips |
| `F031` | Initial number of source planes remaining after skips |

The routine calls original `2AB38` with live
`EFED, EFF1, F001` to compute the VGA aperture byte and
plane. It generates `AH = ROL(0x11,(plane + 4 -
(clip[12]&3))&3)`, and reads original sprite header
`WORD[resource]` frame stride, `BYTE[resource+7]`
transparency flag, and frame offset `8 + frame*stride`.
Initial fully skipped planes each advance the source by
`stride>>2`, rotate `AH`, and decrement `F031`.

For every remaining source plane the original writes the
**raw** VGA sequencer `OUT 03C4h,AX` value, copies
visible bytes row-by-row in *forward* source order, and
advances both source and destination by original clipped
row strides. It restores its saved VGA destination start
between plane passes, then rotates `AH`. When the
plane mask crosses plane 3 into plane 0, it can adjust
**F019, F01D, F021**, the VGA byte start, and the live
row remainder. Those changes can make a later plane
narrower or wider than the preceding one. The native
model reproduces these transitions rather than using a
single modern rectangle width for all four planes.

The original `2A798` initializes transparency comparison
key `DH=0xFF` even for opaque sprites. When the
header's `0x80` bit is set, it instead takes the
**first byte of the sprite frame** as transparency
key and advances past that sentinel. The byte at
`2A7A4` **self-modifies executable CODE.2 operand
`2A816`**, changing the immediate in original
`2A814: CMP BYTE [ESI],imm8`. Only byte values
that differ from that live key are copied to VGA.
The native model preserves raw comparisons before
any color transformation, skips transparent VGA writes,
and records all selected physical plane masks.

The original DWORD `F009` is also updated to the
live row counter after the sequencer OUT; the native
bounded result exposes its final value. Original
zero-iteration `DEC/JNE` loops have no positive count
guard and could read unchecked DOS memory; the native
model reports **unsupported** for incomplete captures
rather than assuming original DOS safe behavior or
writing partially checked host pixels.

## Native source and binding

Added:

- `include/rloft/combat_original_clipped_raster.hpp`
- `src/combat_original_clipped_raster.cpp`
- `tests/combat_original_clipped_raster_tests.cpp`
- `include/rloft/combat_original_clipped_raster_binding.hpp`
- `src/combat_original_clipped_raster_binding.cpp`
- `tests/combat_original_clipped_raster_binding_tests.cpp`

Extended `CombatOriginalSpriteCodePatchModel` with
`patch_2a6b0`. When the caller supplies its **mutable
original object-1 code image**, the model validates
original CMP opcode `80 3E` at `2A814/2A815`,
then writes ONLY the captured immediate byte at
`2A816`. This source mutation occurs at the same
relative stage as the original executable: after
frame-sentinel selection but **before** the first
VGA pixel operation. The existing `2A584`
two-DWORD shaded lookup patch remains untouched.

The `CombatOriginalClippedRasterBinder` plugs
this raster behind existing `29414` clipped
sprite callback, preserving the full `2A380`
callback, original `29804` record layer, original
`2AE30` predicate/position chain, and live
`F001/EFED/EFF1/EFF9` video state.
Optional mutable CODE.2 image is propagated to the
clipped raster callback. **No guessed image, glyph
color, resource header or host mouse position** is
supplied in place of DOS data.

## Verification

- Added and independently compared all **154** original
  instruction-byte signatures at `2A6B0..2A89E`
  against the *actual uploaded* CODE.2 bytes:
  **154/154 PASS**. The full verifier source ledger
  now has **1,263 named signatures / 1,255 distinct
  logical instruction addresses**. This is not a
  percentage of total original code.
- The **exact committed**
  `combat_original_clipped_raster.cpp` Git blob
  SHA-1 `164e035c7bd0514913d75d4cd6343f8e78e3b4c5`
  was reproduced byte-for-byte in a local strict C++17
  harness and passed **Debug, optimized Release, and UBSan**
  tests (all with assertions enabled). The
  **exact committed binder** Git blob
  `b0a5121c5e57b52b05a92b54bb37cd2dcfab2e6b`
  likewise passed all three modes.
- Focused regression cases cover full four-plane opaque
  transfers, transparent frame sentinels, original
  default opaque `0xFF` key, negative/top/right/left
  clip-derived skips, narrow one-pixel/one-plane
  draw, plane-3-to-plane-0 carry changing later
  widths, live F001 VGA page selection, original
  code-byte patch and explicit unsupported
  incomplete source/VGA captures.
- Two new assertion-enabled CTests are **registered**
  in `DemoV1/CMakeLists.txt`, but a consolidated
  current-HEAD Windows MSVC build and the complete
  set of repository CTests have **not** run.
  The previous full Windows run
  [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
  passed 130/130 tests at historical commit
  `40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
  It cannot validate changes made after that commit.
- GitHub Actions remains **manual-only** and was not run.

## Next exact remaining pixel boundary

Original `2A8A0..2AA9A` is a separate
**color-adjusted clipped sprite** procedure with an
additional DWORD color-adjustment argument: original `2A8AC`
self-modifies immediate `2AA12`, and
`2A99C` patches transparency comparison
immediate `2AA0D`. The inner path uses
`LODSB; SUB AL,imm8; STOSB` on nontransparent
bytes; the initial mask uses **SHL AH,CL**
rather than the first raster's **ROL AH,CL**.
Do not clone `2A6B0` without verifying these
instruction differences. Additional source-backed
pointer services `33C90/1F820` and DOS-native
screen/IRQ reference comparisons remain
outstanding.

**Formal demo acceptance:** **1/5 major gates**.
Engineering work is advancing, but the entire
demo completion percentage is not objectively
measured.
