# Original CODE.2 overlay predicates and sprite clipping

**Date:** 2026-10-08 · **Branch:** `demo-v1`  
**Provenance:** User-owned original `CODE.2` SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
CODE.2 object-1 begins at physical file offset `0x3AF24`.
Instruction addresses below are original object-local x86 offsets.

## New complete bounded predicate bodies

Original `2AD18` classifies a passed DWORD object type:

| Original type | Original result |
| --- | --- |
| `0x3B` | First test `1C1DC(0x48)`: if nonzero return false; otherwise return `1C1DC(0x35)` |
| `0x3C` | First test `1C1DC(0x48)`: if nonzero return false; otherwise return `1C1DC(0x36)` |
| `0x3A` | Always false, **despite** falling within the usual accepted numeric interval |
| `0x22..0x46` except special cases | True |
| `0x52` | True |
| All remaining signed DWORD type values | False |

Original `2ADE0` returns true for signed types
`0x48,0x49,0x4A,0x4D,0x4F`, false otherwise.

The underlying original `1C1DC` reads the real DWORD bitset at
`DGROUP:0x22200 + 4*(index/32)` and masks the bit `index%32`.
Specifically: key `0x48` reads bit 8 of DWORD `22208`; key
`0x35` reads bit 21 of DWORD `22204`; key `0x36` reads
bit 22 of the same DWORD. **The bitset is sampled live on
each original predicate call**, and the second bit lookup
does not happen when `0x48` was set.

New `CombatOriginalOverlayPredicatesModel` implements these
classifications and installs original callbacks into the
previous `2AE30` record scanner without replacing its
authentic DOS table-word provider. Missing required original
bitset DWORDs return **unsupported** rather than a made-up
world-state value. Numeric branches that do not read this
bitset remain independent of it.

## 29414 original sprite clipping and scratch record

Original `29414(mode,row,column,resource,record)` calls
`2E758` **before checking whether the resource pointer is
zero**; resource==0 then returns 0 without accessing the
sprite header or scratch record.

With a valid sprite resource, the original interprets
`WORD[resource+4]` as the row count and
`BYTE[resource+6]*8` as the column span, reading these
header fields at their original call sites. The function
clips the sprite against **four live DWORD globals**:
`EF55` first row, `EF59` last row,
`EF5D` first column, and `EF61` last column.
It uses x86 DWORD wrap and signed `JLE/JG` tests,
not host signed-integer subtraction or viewport guesses.

The original `29804` wrapper provides scratch address
`EF74 + EFD4*0x18`. `29414` writes its 24 bytes
as exactly six little-endian DWORDs:

| Offset | Original value |
| --- | --- |
| `+0` | Top-clipped rows multiplied by original byte-width×8 |
| `+4` | Bottom-clipped rows multiplied by original byte-width×8 |
| `+8` | Remaining visible row count |
| `+12` | Left-clipped column count |
| `+16` | Right-clipped column count |
| `+20` | Remaining visible column span |

The original also stores `WORD[EF6B]` active output column,
`WORD[EF6D]` remaining row count,
`WORD[EF6F]` remaining column span, and
`WORD[EF69]` output row **before raster dispatch**.

The return value and raster dispatch depend on original
resource-size comparisons:

- **2:** Both original sprite dimensions equal the
  post-clip counts. Call the existing full `2A380`
  sprite rasterizer directly.
- **1:** Sprite is clipped but both visible counts are
  strictly positive as signed DWORDs. Invoke original
  clipped rasterizer `2A6B0` with the mutable
  six-DWORD scratch record.
- **0:** Null resource or empty clipped dimensions.
  No lower-level raster call occurs. `2E758` has
  nevertheless already run.

The native `CombatOriginalSpriteClipModel` implements
these operations against caller-owned original DGROUP
bytes. It rejects uncaptured resource fields or scratch
memory rather than producing invented graphics.
`CombatOriginalSpriteClipBinder` supplies this genuine
`29414` service to the earlier `29804` EFD4
record stack.

**Unimplemented pixel boundary:** The actual
clipped rasterizer `2A6B0` is not yet reconstructed.
An initial assembly audit identified its
`DWORD[F00D/F011/F015/F019/F01D/F021/F025/F029/F02D/F031]`
intermediate state and self-modifying transparency-key byte
at logical `2A816`. These effects still need individual
verification and native four-plane implementation. Full
`2A380` normal-sprite rasterization is already reconstructed
separately but still requires authentic live game backing
data for end-to-end use.

## Shared original DOS global aliases corrected

The original `WORD[A6BC]` is shared between
`1A3B8` mouse position mode and `1A288` subsequent
sprite-mode selection; the native position adapter
now carries the **post-callback live value** into the
overlay's mode selector.

The original `BYTE[D680]` is read by both
`1A3B8` and `2AE30`. The probe service
now supports a live D680 supplier instead of
requiring a stale independent capture. Existing
isolated callers retain the previous snapshot
field when no live provider has been installed.

## Implementation and tests

New shared source:

- `include/rloft/combat_original_overlay_predicates.hpp`
- `src/combat_original_overlay_predicates.cpp`
- `tests/combat_original_overlay_predicates_tests.cpp`
- `include/rloft/combat_original_sprite_clip.hpp`
- `src/combat_original_sprite_clip.cpp`
- `include/rloft/combat_original_sprite_clip_binding.hpp`
- `src/combat_original_sprite_clip_binding.cpp`
- `tests/combat_original_sprite_clip_tests.cpp`
- `tests/combat_original_overlay_predicate_clip_chain_tests.cpp`

All three new CTest targets are CMake-registered and present in
the forced-assertion target list. The integrated callback test
drives the original chain
`1A288 → 2AE30 → 2AD18 → 1C1DC → 29804 → 29414 → 2A380`,
using original resource pointer `0x880D`. It also checks
the source-derived subtlety that setting world-state bit
`0x48` changes a matching `0x3B` overlay to fallback
drawing **mode 0**, rather than eliminating the draw.

**105 new original-byte opcode signatures were checked 105/105**
against immutable `CODE.2`, covering `1C1DC`,
`2AD18`, `2ADE0`, and `29414`; the cumulative
verifier now holds **1,109 named instruction anchors at
1,101 distinct original logical addresses**. This is
not whole-game source-code coverage or a completion score.

The exact committed C++ implementation sources were independently
copied and Git-blob-matched to:

- `combat_original_overlay_predicates.cpp` SHA-1
  `a44eb4e8344d7509da95ce3280c2844a74404d5a`.
- `combat_original_sprite_clip.cpp` SHA-1
  `1412230d3d8fd236e1fd798da15ee8fd3bb27fe2`.

A focused source-equivalent C++17 smoke harness for these two
implementations passed strict
`-Wall -Wextra -Werror -pedantic` Debug,
optimized Release (assertions enabled), and
UndefinedBehaviorSanitizer testing. Cases included the
live world-bit gate, null sprite pointer and pre-render
refresh, full raster dispatch, signed negative starting
coordinates, top/bottom/left/right cropping, exact 24-byte
scratch contents and empty clipped rectangles.

The three **checked-in full CTests**, including the
combined previous overlay lifecycle, have been registered
but have **not yet been executed in a complete Windows
CMake/MSVC run at this branch HEAD**. The last verified
full Windows build, [Actions run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281),
passed 130/130 at earlier source
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
It does not cover the newer code. GitHub Actions stays
**manual-only** and no build was requested/triggered here.

## Next gates

Reconstruct the source-backed `2A6B0` clipped
sprite rasterizer with its VGA sequencer,
transparency, clipped row strides and mutable DGROUP
state. Then bind original `33C90` and `1F820`
cursor service effects, supply source-authentic
DOS scene tables, shader/sprite headers and buffers,
and compare actual DOS/native plane bytes, original
IRQ ordering and the Level 22 demo transition.

**Formal demo acceptance stays 1/5 gates.**
Engineering completion is tracked separately, without
a fabricated numerical whole-project estimate.
