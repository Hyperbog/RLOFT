# Original CODE.2 C029 individual-column RES3 wide-rendering milestone

**Branch:** `demo-v1` · **Date:** 2026-10-08 EDT · **State:** implementation
and test source committed; Windows CI has NOT yet built the current SHA.
Actions builds remain manual-only. No DOS pixel parity is asserted.

## Critical previously missing visual coverage

Prior to this checkpoint, native RES3 sprite tracing rejected any
visible source-column run with width greater than seven, regardless
of the original renderer's **C029 dispatch mode**. Thus perfectly
ordinary large NPC/placed-object shapes could remain invisible even
when source RES3, geometry, occlusion and SINTAB descriptors were
available and supported.

The supplied unmodified `CODE.2` file identity is SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
At object-2 file offset `0x75F24 + 0xC029 = 0x81F4D`,
the shipped initial mode byte is **`01`**. This is source data,
not a claim that runtime code never changes C029.

Original CODE.2 routine `36B14` branches on this variable:

- `C029==0`, original `36B1A..36B40`: invoke `369D8` **once**
  with the full original run length as the VGA mask-table index,
  then persist `XCHG AL,AH` on the shade lookup. Only grouped
  lengths **1..7** have certified `C668/C688/C6A8` mask bytes.
  Do NOT silently split an eight-column grouped source.
- `C029!=0`, original `36B42..36B95`: loop over each original
  destination column, temporarily pass `105DF=1` to `369D8`,
  advance `105E3` by one, independently swap lookup byte order
  on odd destination X, and restore the global lookup at the end.
  Every one-pixel invocation uses only **source-authenticated width-1**
  CODE.2 mask entries, regardless of the original source run width.

This dispatcher is distinct from the main-wall `3263C/C6F4` path,
which does not use C029 individual-column source iteration.
Do not transfer the RES3 solution to wall rendering.

## Native implementation changes

The same original `Type7VgaRasterDispatchModel` is still called
from `Type7VgaSceneTraceModel` with original, unsplit
`Type7VisibleRun` geometry.

- A run in individual mode can now have width **1..255**,
  limited explicitly by the existing uint8 dispatch argument.
  The 256-ray horizontal viewport bound is independently
  validated before any original SINTAB code executes.
  Width 256 is an **unverified capacity boundary** and remains
  pending; do not narrow its count or infer wrapping semantics.
- The grouped path **remains 1..7 only**. No artificial
  segmentation, alias of mask phase slots, or shaded pixel
  approximation is used.
- A visible wide run is now accepted in both frame trace and
  scene trace ONLY when current C029 is individual mode.
- `Type7VgaTraceValidator` checks each column's exact phase,
  plane mask, byte offset, low-byte shade swap, restore state,
  call order and complete source-destination descriptor list.
- The unchanged `Type7ScenePixelExecutor` runs the certified
  original generated SINTAB pixel instructions on each selected
  VGA lane, using its existing whole-scene transactional write.
- Remaining unknown grouped runs, width 256, invalid source
  geometry or other unmodeled resources stay explicit
  source-indexed failures/pending calls.

## Verification work

The existing assertion-enabled test targets now include:

- `type7_vga_raster_dispatch_tests`: 19- and 255-column
  C029=1 runs, mask parity, sequential physical byte addresses,
  ECX parity/restore; grouped eight-wide still rejected.
- `type7_vga_scene_trace_tests`: complete 12-wide scene
  trace and expanded SINTAB descriptor equality; grouped
  twelve-wide rejected.
- `type7_vga_frame_trace_tests`: eight-wide individual call
  accepted, grouped eight-wide unresolved, 256-wide still
  unresolved, and source-isolated malformed neighbors preserved.
- `type7_vga_trace_validation_tests`: independent eight-wide
  mask/pass/address validation, tampering and 256-ray viewport
  overflow rejected.
- `type7_scene_pixel_executor_tests`: 16 actual generated-code
  descriptor invocations across an eight-pixel source run,
  24 indexed stores with authentic odd/even color lane swap,
  two physical VGA bytes, correct plane isolation.

The optional native Level 22 TSV v2 capture now additionally
includes `scene_individual_columns_mode`, raw original 16.16
camera coordinates and facing, and one `scene_run` row per
original source span (call index, source X/index, screen X,
unmodified width, mode, verified/unverified status).
This is outside the game framebuffer and never changes
original resources or gameplay. Updated capture CTest
writes a second synthetic scene for explicit source mode/
coordinate metadata regression, and checks Win32 file
handles are closed before removing test files.

**Not a passed CI milestone:** source files were reviewed
and committed, but no current-HEAD compilation or CTest
run and no actual DOS indexed frame comparison has yet run.
Last observed successful manual Windows #638 passed 159/159
at an earlier SHA. Current source expects **160 CTests**
if Python3 is installed.

## Next manual test

1. Manually run GitHub workflow `demo-v1-build.yml` on
   `demo-v1`; leave Linux opt-in unchecked.
2. Use `Capture-Level22.cmd` to visit original placed objects
   of source types 62, 63 and 64 and the NPC near (13,7).
3. Inspect the new `scene_run` rows, `scene_outcome` rows,
   rasterized-vs-pending RES3 counters and source-indexed
   failures. Specifically search for **original widths 8..255**
   using C029 individual mode, now rasterized with exact
   one-column VGA masks.
4. Compare actual native indexed/palette frames with separate
   original DOS captures and original event timing when
   available. Do not infer DOS fidelity solely from passing
   native pixel unit tests.

Formal demo acceptance remains **1/5 gates (20%)**;
whole-game engineering completion is not quantified.
