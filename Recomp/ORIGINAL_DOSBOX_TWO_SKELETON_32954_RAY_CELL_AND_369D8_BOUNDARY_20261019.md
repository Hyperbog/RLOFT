# Genuine DOSBox two-skeleton camera — corrected 32954 DDA index, remaining 369D8 boundary

**User-provenanced untouched original save:** `SAVE09(2).zip`, slot name
`CHATGPT`; saved game `SAVE.DAT` SHA-256
`28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
Matching `LEV005.DAT` SHA-256
`b058964ce39d931e89417ef7f36f93640e48c57cbfdc9d5b363457fba238a672`.
Neither original ZIP nor screenshot/pixel data is added to Git history.

Original DOSBox screenshot shows **two visible skeleton actors**.
Camera loaded by compiled original 07938/181B4:
Level 5, saved heading **198 degrees**, saved position 1419090 / 1436586.
Original INIT consumed 1879 bytes, NPCS 5731, OBJECTS 21844.
Primary actor #38 type 4, cell 1302, saved bit 0x40, projected
screen 45, scale 411, signed depth 111760; #70 type 4,
cell 1365, saved bit 0x40, projected screen 176, scale 775, depth 59190.
The standalone original LEV005 grid contains 22 cells with 0x40,
including those two actors. These *serialized prior-state* bits are
not accepted as current visibility until a fresh original 334C4 -> 32954.

## Confirmed bug: halved cell address in reconstructed 32954

Old native `CODE2_UNTOUCHED_SOURCE_RES3_CAMERA_FIND` on exact
untouched user save:
`level=5 heading=198 primary_actors=90 ray_marked_allowed=0
all_actors_near_plane_pass=40 candidate=NO`.
Original raycaster incorrectly computed `index=left_expr>>1` or
`index=right_expr>>1` when `left_expr` / `right_expr`
were ALREADY the actual `row * width + column` **cell indexes**.
The consumer `0x198E0 + index*2` correctly converts cells to bytes
after that. An additional right shift sent the sweep into the wrong
half of the map.

For an illustrative ray at the user camera, `row=21`, `column=21`,
`width=64` generates cell **1365**. The previous left-index expression
used **682** instead, where the original saved grid does not indicate
the same visible corridor. No save, ray mask or actor byte is mutated
to correct this: both DDA source index terms in canonical
`Code2/Code2.cpp` have been fixed.

Windows MSVC source gate **74/74**:
https://github.com/Hyperbog/RLOFT/actions/runs/39204508334 .
Linux source gate **74/74**:
https://github.com/Hyperbog/RLOFT/actions/runs/39204452104 .
CODE.1 + CODE.2 canonical link:
https://github.com/Hyperbog/RLOFT/actions/runs/39204452086 .

The correction was independently regression-tested on **untouched
original owner saves**, with original near-plane/ray gates:
- Level22 heading270: `ray_marked_allowed=1 ray_marked_projectable=1`,
  signed depth 930544.
- Level21 heading216: `ray_marked_allowed=1 ray_marked_projectable=1`,
  signed depth 116048.
Both now enforce source-derived visibility via
`.github/workflows/recomp-genuine-camera-census.yml`, verified at
https://github.com/Hyperbog/RLOFT/actions/runs/39205179902 (3/3 source
camera fixtures pass). These are **genuine original save cameras**,
not synthetic monster placements.

## Next authentic boundary exposed: 36CEC -> 369D8 RES3 source pointer

The previously all-zero preserved-save search rerun now identifies
**four** near-plane/ray-visible saved-camera candidates across 10
snapshots. It invokes genuine original 2C294 for the first time,
then the integrated 36CEC/369D8 raster host fails bounded validation
with:

`CODE2_SOURCE_36CEC_RASTER_FAILURE: 369D8 source adjustment outside allocation`.

Additional 2C294 sprite draws from non-primary passes also hit this
boundary, even in a camera with zero ray-marked primary actors.
The host's current `Original369d8Context.source_body=image`
assumes image-start is the correct caller-supplied `coordinate`
for reconstructed original `function_369d8`, which actually computes
`callback_esi = coordinate + signed16(record+2)`.
Original EH7 RES3 display lists contain NEGATIVE signed source
adjustments, so an image-start anchor can go before the decoded
allocation. Genuine type-7 decompressed sprite shapes from original
RES3 show e.g. ID 315 has source adjustments -36..1880; this is
not a malformed original asset. A correct anchor from original
`36CEC`/ `36B14` caller state must be recovered and bounded,
**not guessed** or silently clamped.

Failing, correctly non-accepted genuine sprite CI:
https://github.com/Hyperbog/RLOFT/actions/runs/39204705856
(checked 10, candidate files 4, full-scene raster failures 5)
and
https://github.com/Hyperbog/RLOFT/actions/runs/39204705893
(level-specific native world integration failures).
Do not portray these workflows as green. Previous 0-sprite
"passes" were missing this newly reached sprite boundary.

**Action:** inspect the original 36CEC caller's `coordinate`
argument and type-7 display-list source addressing, implement
accurate base and effective clipped bounds; add a genuine original
sourced sample regression, then use the EXACT untouched `SAVE09(2)`
ZIP for a fresh two-skeleton capture/ray/source-pixel test. No new
DOSBox save or new camera is needed. The screenshot plus save establish
the expectation, not a DOS VGA pixel-parity proof.

**No independent original DOS indexed planar frame/DAC palette oracle
yet, so DOS pixel parity is NOT VERIFIED.**
