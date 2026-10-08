# Original CODE.2 2A380 / 2A584 sprite rasterization checkpoint

**Date:** 2026-10-08. **Branch:** `demo-v1`.  
**State:** Both original opaque/transparent four-plane sprite-raster paths
are implemented against the existing native `VgaPlanarMemory`.
**No complete Windows CMake/CTest build or reference-DOS pixel comparison
has run for this new source. Do not infer live game rendering equivalence.**

## Immutable DOS evidence

User-owned `CODE.2` SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Original object-1 file base: `0x3AF24`; addresses below are
object-local logical offsets.

Original `2A380..2A477` is the **normal sprite rasterizer**.
`2A584..2A6B0` is the **shaded sprite rasterizer**.
Both call the previously reconstructed `2AB38` VGA
pixel-address/plane routine.

### Original resource header and frame pointer

| Original resource field | 2A380/2A584 interpretation |
| --- | --- |
| `WORD +0` | Unsigned 16-bit per-frame byte stride |
| `WORD +4` | Unsigned 16-bit number of raster rows |
| `BYTE +6` | Source byte width unit, doubled for plane-row copying |
| `BYTE +7 & 0x80` | Transparent/sentinel-copy branch |
| `header + 8 + DWORD(frame_index)*WORD(+0)` | First frame byte, original wrapping x86 32-bit MUL/address arithmetic |

The source byte at `WORD+2` is preserved but not consumed by
these entry paths. Signed-negative font/glyph frame IDs are
represented as their original 32-bit bit patterns, allowing
backward frame references **only if the original backing memory
is captured and bounded**.

### Four original VGA planes

Both paths begin with the original map mask `AH=0x11`
shifted left by `column&3`, output through VGA sequencer
port `0x3C4` (index 2), and draw four source-plane blocks.
Between blocks, `ROL AH,1` advances the write mask and
`ADC EDI,0` advances the physical address on bit-7 carry.
Each pixel/byte write goes through the existing
`VgaPlanarMemory::write(address, mask, color)` primitive.
The output result records the final map mask, but the live
VGA sequencer **register itself is not yet persisted**.

The row delta is `DWORD[EFF9] - (2*BYTE[resource+6])`.
In the normal opaque path, `REP MOVSW` copies
`2*width` bytes per row in each plane, so the
effective address stride is `DWORD[EFF9]`.

### Original transparent branch

When bit 7 at resource +7 is set, the first byte of frame
data is a single **raw sentinel value**. The rest of the
frame is consumed across four consecutive plane blocks.
Each source byte equal to the sentinel advances the
destination by one byte **without writing**; other bytes
are written. Even a sentinel of `0x00` is a valid
original input; no modern alpha interpretation is used.

The transparent loop first doubles EBX, then uses its
**low byte BL** as the per-row byte counter. Thus
`width=129` produces `BL=2` while the row delta
still subtracts `258`. Its next row can move backward
by 176 physical bytes when `EFF9=80`. This original
8-bit overflow behavior is preserved, not normalized.

A transparent BL of zero enters an unbounded original
`DEC/JNE` loop, and is rejected as an unsupported
bounded-host capture.

### Shaded 2A584 is a pointer-based lookup, NOT arithmetic tinting

The original `2A590` and `2A595` instructions write
the **full 32-bit shader pointer** into original code
operands `2A60B` and `2A672`. The instructions
`2A609` and `2A670` then execute
`MOV AL, [EBX + patched_pointer]`, with
`EBX=unsigned raw source byte`. This is a 256-entry
byte lookup table in original DOS memory; **it is not
a subtraction, brightness rule, or guessed palette**.

For transparent shading, `2A667` tests the **raw input**
against the sentinel **before** `2A670` performs lookup.
A transparent source byte is never palette-mapped or written.
A shaded opaque sprite maps every source byte and writes
each result.

`CombatOriginalSpriteRasterModel::draw_2a584` therefore
requires a separately captured original 256-byte table.
The model returns the observed full DWORD shader pointer
in `shader_pointer_self_patched`; it does **not yet apply**
the two self-modifying CODE.2 operand writes to live emulated
code memory. That side effect must precede raster execution
when connected to the game.

Another instruction-order difference matters for malformed
resources: normal opaque `2A380` uses `REP MOVSW`
which copies zero bytes when width is zero; shaded opaque
`2A584` starts `LODSB` before `DEC ECX/JNE`, so
width zero underflows its loop counter. The shaded case
is rejected rather than silently substituting the normal
zero-width behavior.

## Native code and test registration

New reusable source components:

- `DemoV1/include/rloft/combat_original_sprite_raster.hpp`
- `DemoV1/src/combat_original_sprite_raster.cpp`
- `DemoV1/tests/combat_original_sprite_raster_tests.cpp`

Registered in `rloft_boot_flow` and CTest as
`combat_original_sprite_raster_tests`, with assertions
enabled even in Release.

The native model accepts an explicit *captured DOS memory*
view, original resource pointer and frame bits,
original EFED/EFF1/F001/EFF9 VGA video state and
optional separate shader lookup-memory view.
It performs a **two-phase host safety check** of source
bytes and destination aperture before writes, returning
unsupported on missing data. This is not an assertion
that the unguarded DOS binary itself safely rejected
malformed resources.

Tests cover synthetic original-format sprites: 320
different starting columns, all four plane rotations,
opaque copies, transparent sentinel preservation,
non-first frame indexing, signed-negative frames,
`width=129` transparent BL overflow, source bounds,
invalid destination addresses, genuine original
zero-width asymmetry, full 32-bit shader pointers,
shade-LUT colors, transparency-before-shading, and
missing shader tables. No proprietary art is committed.

The **exact committed implementation source blob**
SHA is `f83e5dc2f3b9b7756b8f40ffe13b877ae817a5b3`.
A copy with matching Git blob SHA compiled and executed
in an isolated C++17 test harness using an interface-compatible
four-plane VGA memory class. Debug, optimized Release
(`-DNDEBUG` with test assertions active) and UBSan
(`-fsanitize=undefined`) all passed with
`-Wall -Wextra -Werror -pedantic`. This is **not a
compilation of the entire repository's actual CTest
target or original Windows executable**.

## Original-byte audit and current limits

36 new instruction signatures for normal `2A380`
and 35 for shaded `2A584`, including the three
additional zero-width-specific anchors, matched the
immutable uploaded CODE.2:

- **648 named instruction-byte checks**
- **640 distinct logical instruction addresses**
- Existing one direct Strength table, nine hashed DGROUP
  spans and one hashed CS span retained.

The original shader lookup **contents** and live shader
pointer must come from the real game runtime/art.
The model's synthetic XOR shader table is a regression
fixture only, not a claim that DOS uses XOR.

Remaining fidelity gates:

1. Bind original sprite resource pointers and shader
   tables from original game resources to the new
   raster API through the existing
   `CombatOriginalSpriteRuntimeModel` and
   `CombatOriginalTextRuntimeModel` callbacks.
2. Record the original self-modifying shader pointer writes
   and final VGA sequencer mask state in live runtime memory.
3. Reconstruct original `1A8C8` compositing and the
   deferred `2E758/F0C8` refresh target from source.
4. Compare DOS screenshot/framebuffer planes, original
   IRQ observations and resource-specific behavior against
   native output; run complete Windows CMake/CTest
   **on this newer source** before claiming a green build.

The last complete Windows run, [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281),
**passed 130/130 tests at its earlier pinned source SHA
`40e3f14b1072a7a0180ea1b0b4c1cea437137631`**.
These new raster commits are subsequent and not included
in that result. GitHub Actions remains manual-only.

**Overall faithful-demo acceptance remains 20% (1/5 major gates).**
