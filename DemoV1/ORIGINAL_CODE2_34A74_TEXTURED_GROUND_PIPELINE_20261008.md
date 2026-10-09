# Original CODE.2 textured ground — SINTAB perspective, raster and VGA milestone

**Date:** 2026-10-08 EDT · **Branch:** `demo-v1`
**Status:** Source-backed pipeline implemented and regression target registered,
but **not yet compiled on current-HEAD Windows CI**. The live demo still
uses its **known-incomplete flat lower fill** until the original
runtime-selected RES4 type-1 resource can be proven and routed.

## Why this is a required native visual correction

The user's original DOSBox fresh-party screenshots clearly show
a green/red patterned forest floor. The native demo instead renders
a flat gradient below the backdrop. These are not equivalent.

In supplied original `CODE.2`, immutable SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`,
the initial DGROUP byte `C026` is **1**. Routine `1F369`
checks it: when zero, control transfers to flat `33DE0`;
when nonzero, `1F372..1F3C8` selects resource bank **4**,
computes a per-row span, and calls **`34A74`**. Hence the
shipped outdoor mode is textured, not the current native flat fill.

Do not confuse this with the RES4 **type-6 wall texture**
(`SurfaceTextureArchive`). `2F29C` checks the runtime
resource entry's type byte for **1** and returns its payload
pointer. `34A74` then adds **2** to that pointer (`FF54`)
before performing 64×64 indexed texel reads. Exact selection
requires the original mutable `258F8` resource-base state
plus original level header selectors `2561D/2561F`;
that mapping needs additional independent source proof.

## First major new discovery: the perspective inputs already exist

Previous project notes treated the runtime perspective arrays
at `193E0` and `19660` as unavailable, which prevented
an original FF58 step generator. Direct disassembly of
`CODE.2 3838..38C3` establishes their origin:

| Runtime region | Data source in original SINTAB |
|---|---|
| `C65C`, read by `34275` | First 0x708 bytes, sine entries (450 × 4) |
| `19660`, read by `34998` | Next 0x100-byte projection_A table after sine/tangent |
| `193E0`, read by `34998` | Following 0x100-byte projection_B table |

`DemoV1::RendererTables` **already parses all three** as
`sine_q16_signmag`, `projection_a`, and `projection_b`.
No synthetic perspective lookup or guessed angle sampling is needed.

The newly implemented
`OriginalGroundTextureRaster::generate_ff58(tables, heading)`
reconstructs the original CODE.2 `34998` algorithm:

- `relative=360-C658` where `C658` is original heading 0..359;
  the origin angle is `relative-32` and the delta angle is
  `relative+90`.
- `34270` subtracts 270 degrees before falling into `34275`;
  both routes normalize to 0..359 and fold the quadrant into
  0..90 for an original sine WORD lookup.
- The magnitude is `(u16(sine)+1)>>2`; quadrants 2 and 3
  are negated, with original **signed 16-bit** patch semantics.
- For all **128** original scanline records, multiply
  perspective_B by the two origin coefficients and
  perspective_A by the two delta coefficients. The original
  `IMUL DX; SHLD DX,AX,3` is equivalent to extracting
  bits 13..28 of the signed product, preserving 16-bit
  wrapping. Origins use their low byte, deltas their low word.
- Each record is the authentic six bytes:
  `u16 delta_U, u16 delta_V, u8 origin_U, u8 origin_V`.
  `decode_ff58` also accepts an exactly 768-byte
  DOS live-memory dump for independent comparison.

The normal exploration frame now stores these source-derived
128 records and the original `C048/C044` texture phases from
the high byte of each 16.16 coordinate's **low** word.
Opt-in frame capture v2 includes the original camera phases
and step records at rows **0,79,80,127** to expose the
renderer's phase/mip transition. These metadata rows do not
alter game pixels.

## Second major milestone: original 34A74 VGA texture raster

The new `OriginalGroundTextureRaster::draw` accepts **only**
the original six-byte perspective records, 2-byte-header plus
4096-byte original 64×64 tile, and original FE15..FF14
256-byte shade source from immutable CODE.2.

The x86 inner loop `34350` contains **64 STOSB** destinations
per plane, not 80; thus 64 original planar bytes × four
VGA sequencer masks = the exact **256-pixel** centered viewport,
x32..287. Its source address uses two `SHLD ESI,...,6`
instructions and equals
`(uint32(V)>>26)*64 + (uint32(U)>>26)`.
Each texel is then subtracted by the original shade byte,
with unsigned 8-bit wrap (no RGB interpolation).

`34A74` follows the original 80/48 scanline split:

- **First 80 rows:** `342E4` calls 34350 with VGA
  map masks `0x08,0x04,0x02,0x01`, separate one-pixel
  samples for all four lanes of each planar byte.
- **Next 48 rows:** `34954` calls 34350 with map
  masks `0x0C` and `0x03`, so each texture
  sample colors two adjacent physical planes.
- Lower caller `1F3B4`: first planar offset
  **0x50F8** (screen x32, y259), step **-80**
  original VGA bytes per row, shade direction **-1**.
- Upper caller `1F343`: first planar offset
  **0x0148** (screen x32, y4), step **+80**,
  shade direction **+1**.
- Original shade pointer starts at
  `FE94 + (0x7F * FF4C)` and advances
  by `-FF4C` per row. The full reachable
  shading range is **FE15..FF14**, not just
  the FE94..FF14 table currently used by
  the flat-only renderer.

Output is staged transactionally and not committed
if the original tile, perspective bounds, row spacing
or VGA 256-pixel viewport is invalid. The verified
behavior uses actual palette indices, not
procedural grass, color matching, or screenshot guesses.

## Regression coverage

The new `original_ground_texture_raster_tests` is registered
in the same manual-only Windows CTest workflow.

- Original sine folding at headings 270, 0, and 90,
  signed delta/origin IMUL16+SHLD behavior, all 128 FF58 steps.
- Exact 768-byte FF58 decode, rejection of truncated dumps.
- Synthetic original-layout 2-byte-header/64×64 indexed tile,
  texture coordinate wrap, low-byte palette subtraction,
  original C044/C048 origin-phase additions.
- First 80 fully independent 4-plane rows, last
  48 paired-mask rows, with expected **26,624**
  source texel reads and **32,768** output
  pixel writes for a 128-row lower viewport.
- Both upper and lower original VGA source positions,
  exact x32..287/y4..131/y132..259 clipping.
- Transactional rejection of invalid geometry
  and malformed source lengths.
- Existing capture CTest now verifies original
  FF58 row telemetry and mode-flag provenance.

**Expected total:** 163 registered CTests on the standard
Windows Python-enabled runner (previously 162 registered
in this source branch, versus 160 in the last green #640).

No test result is claimed for the current branch HEAD.

## What still prevents the LIVE flat-fill replacement

Although the perspective generator and original raster core
are now represented as native code, they must not yet replace
the live lower surface because the per-level
`2F29C(bank=4, dynamic_base+signed(header+7))` resource
selection is not fully resolved in the native runtime.
The selected resource must be verified to be the original
**type-1** 64×64 tile with a two-byte header, rather than
an unrelated RES4 type-6 wall texture.

Also pending: original `FF50` row-span calculation,
the dynamic `C026/C027` state and `34DE8` follow-on
draw stage. These can change what part of the
texture is visible or whether the original flat
path is correct in another location. Never use
`C026` as permanently immutable gameplay state;
the binary-provided **initial** byte is only the
verified initial setting.

The current native flat fill is explicitly marked
`flat-fill-only;34A74-pending` in frame telemetry.
The new FF58 rows let future DOS live-memory captures
test the perspective calculation independently,
before validating exact original indexed pixels.

**Formal demo acceptance remains 1/5 gates (20%).**
No GitHub Actions run was dispatched. Builds remain
manual-only, with Linux off by default.

## Level 22 selector provenance and asset access boundary

The independently supplied original Level 22 header at
`LEV022.DAT` physical file offset `0x1D` has signed
ground selectors:

- level header `+5 = 0xFFFF` (signed -1, upper selector);
- level header `+7 = 0x0037` (decimal **55**, lower selector).

Original `CODE.2 1F388..1F3C3` forms a runtime
resource ID from **signed header+7 PLUS dynamic resource
base `DGROUP:258F8`**, calls `2F29C(bank=4)`, and only
after the original type-1 selector succeeds sets
`FF54=C040+2` for the 64×64 source tile. The runtime
base is initialized through `CODE.2 16E90` from a
resource loader structure and is **not yet independently
verified** at Level 22 fresh-entry time. Consequently
**RES4 entry 55 is a relative selector, not yet a
verified absolute texture ID**.

The original **1,975,542-byte** Strahd RES4 binary is
preserved in repository branch `DnD-Classics` under
`MASTER/Ravenloft - Strahd's Possession/RLOFT/RES4`
(Git object SHA `30d1b3377ddddd0e9ae44f6dc2fa9ca1d7f0fad0`).
However, the present GitHub source connector decodes
repository files as UTF-8 and cannot retrieve this
binary file as raw bytes. Do not infer its tile
contents, compression state or entry indexes from a
different archive or a similarly named asset.

At runtime, `ResArchive` already parses original
RES offset tables and can decode original EH type-1
entries. The current `RavenloftBootAssets::type1`
dispatch intentionally does **not** expose RES4 as
a standard 2D boot-image bank; it must be introduced
as a dedicated, source-verified floor resource path.

**Acceptance invariant:** never activate the
34A74 floor in the live viewport until exact source
entry, texture content and mutable C026/C027 and
FF50 row-span state have all been checked. The
current flat-fill image remains an explicitly
incomplete diagnostic, not an authentic DOS forest.


## Dedicated original RES4 candidate probe (subsequent source continuation)

[Original-source audit and manual Windows probe](ORIGINAL_LEVEL22_RES4_GROUND_SOURCE_AUDIT_20261008.md)
now enumerate valid raw/LZSS EH type-1 64×64 tiles directly
from the user's original installed `RES4`, report
resource indexes and diagnostic decoded-image fingerprints,
and preserve the strict relative selector + **unverified
258F8 base** boundary. `verified_tile_source`
bridges only explicitly selected original 4096-byte pixels
into the existing 34A74 raster, with no fabricated texels.
It is not connected to live display until the exact
original DOS resource base and row-span are known.

A new CTest verifies real EH header, offset table,
type-1 decompression and the original 34A74 first
row. The Windows build includes `Probe-Level22-Ground.cmd`
for reading actual original RES4 without GitHub connector
UTF-8 binary limitations. All builds remain manual;
164 CTests are now expected, unrun at current HEAD.
