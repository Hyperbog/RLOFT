# Type-7 scene rasterizer — portable execution boundary

**Branch:** `demo-v1` only. This is implementation continuity, not a revision
to the certified `code1-decompilation` archive or the historical game files.

## Certified evidence

- Original `CODE.2`: 558,689 bytes; SHA-256
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Logical object-1 begins at file offset `0x3AF24`; the active type-7
  renderer is `0x36CEC..0x372F3`.
- The external `SINTAB` package is structurally reconstructed, but its
  executable generated-code bytes are required for actual pixel semantics.
- Type-7 native 1:1 color and transparency can already be reconstructed from
  the canonical RES3 compiled shape (see `decode_type7_native_raster`).
  This is source reconstruction, not proof of scaled on-screen compositing.

## Integrated horizontal/vertical call preparation

| Stage | Original routine | Portable code | Status |
| --- | --- | --- | --- |
| NPC/placed/loose scene records | `2B6D0..2C22E` | scene emitters, runtime | integrated |
| Sorted scene drawing and call variants | `2C294..2C6B6` | `world_scene_draw_calls` | modeled |
| Scale block, center phase, render bias | `36CFA..36DAD` | `type7_column_traversal` | modeled |
| Mode-dependent code segment searches | `36B9C,36BD8,36DB8..36E94` | `type7_rasterizer_setup` | modeled |
| Reversible RET/JMP code patches | `36C10,36C44,36C90,36CBC` | `type7_rasterizer_patch` | modeled |
| Column scaling and wall-ray rejection | `36EBD..372F3` | `type7_column_traversal` | modeled |
| Per-visible-column descriptor entry/stop | `369D8` | `type7_microcode_plan` | modeled |
| Executing generated rasterizer / final pixels | `369C0` -> SINTAB code | not yet implemented | **open** |

No invisible type-7 item is made clickable; the presenter reports
`type7_scene_calls_pending` and `type7_visible_columns_pending`.

## VGA plane-pass reconstruction (new)

From the canonical CODE.2 data object at file `0x75F24`, the exact 32-byte
lookup arrays are at object-2 offsets `0xC668`, `0xC688`, and `0xC6A8`.

`369D8` computes its table slot as
`((destination_column & 3) << 3) + run_length`.
The first VGA plane-mask pass is always issued, and the second/third are
conditional on their respective nonzero mask bytes. Each mask is passed to
`3395C`, which writes sequencer register `02h` through VGA port `0x3C4`.
For each of the four column phases and active run lengths 1..7, the combined
set bits of the three masks cover exactly the requested pixels.

The `36B14` dispatcher has an important two-way split:

1. If dgroup byte `0xC029` is zero, `369D8` is called once using the
   current lookup word; afterwards `36B14` swaps its low two bytes.
2. If the byte is nonzero, `36B14` saves the original column/count,
   iterates one destination column at a time with count=1, and passes
   either the unchanged or low-byte-swapped lookup word according to column
   parity. It restores column/count at the end.

**Do not merge these execution paths or pretend the three-pass mask
lookup is always called for multi-column runs.**

The new `type7_vga_plane_masks` model reconstructs the immutable CODE.2
tables at runtime and validates all 28 active mask triplets. It currently
exposes exact masks and conditional pass counts without dispatching x86.

The `type7_vga_raster_dispatch` model now also reconstructs the complete
`36B14 -> 369D8 -> 369C0` dispatch ABI without invoking generated code. It
records each VGA destination byte address, sequencer mask, renderer lookup word
(unchanged or low-byte-swapped), and pass order, including page base selection.
Both `0xC029` paths are independently regression-tested using original
mask-table-equivalent fixtures. This call-level trace is kept separate from
the currently per-pixel scene-column model: original grouped-run segmentation
must be preserved before integrating it into final framebuffer drawing.

The new `vga_planar_memory` backend implements four distinct 64-KiB
planes in the original `0xA0000..0xAFFFF` aperture. A masked byte write
modifies only selected planes. Pages 0/1 use 32-KiB slices; for an ordinary
320x400 indexed page, byte `y*80+x/4` and plane `x&3` represent pixel
`(x,y)`. It can import/export the existing `IndexedFramebuffer`.

The original `369D8` destination calculation is:

```
video_base = 0xA0000 + 0x8000 * BYTE[0xEF4D]
byte_addr  = video_base
           + 80 * DWORD[0x10450]
           + DWORD[0xC6E0]
           + arithmetic_shift_right(DWORD[0x105E3], 2)
```

For additional plane passes, the routine increments the destination by 1
or 2 bytes. The helper `369C0` calls the SINTAB-generated entrypoint
with `EDI=byte_addr`, `ESI=shape_body+signed_adjustment`, and
`ECX=runtime_lookup_word` (possibly byte-swapped by `36B14`).

The destination-address calculation and `369C0` argument ABI are evidence
for the upcoming interpreter; they are **not** a claim that the original
generated instructions have been translated.

## Validation and remaining risk

`DemoV1/CMakeLists.txt` registers targeted tests for native type-7
reconstruction, traversal, microcode offsets, patching, VGA mask parsing,
VGA raster dispatch, and planar memory. An independent local strict C++17/sanitizer smoke test
using the uploaded CODE.2 bytes checked all 28 triplets and simulated
three sequential masked VGA memory writes for each one. These local
checks are narrower than a full build of the demo library.

GitHub Actions is currently failing before the first job step on both
platforms; a full CMake compilation and integration test remains
**uncertified**. The code that actually interprets the generated SINTAB
instructions, including source texel writes, vertical scaling, shading,
transparency and the wall/scene final composite, remains **unimplemented**.
Do not count renderer or interactive-gameplay fidelity as closed yet.

## Next verified implementation boundary

1. Load the original SINTAB generated-code bytes through `GameSource`.
2. Decode/validate its actual executed opcode subset without guessing.
3. Interpret only those certified opcodes with the exact `369C0` register
   and VGA write-mask semantics, feeding `VgaPlanarMemory`.
4. Compare interpreted output against native type-7 source rasters and
   controlled reference frames at several scales and both render modes.
5. Only after pixel output is real, let the presenter consume the generated
   draw plans and enable original viewport hit dispatch.


## Follow-up: contiguous depth-clipped run fidelity (October 7, 2026)

A direct re-audit of the immutable CODE.2 object-1 instructions at
`36F2E..37094` (left side) and `37140..3729B` (right side)
identified an error in the earlier `visible_columns` implementation:
it independently rejected each wall-occluded destination pixel.
**The DOS renderer does not always behave that way.** It first derives
a contiguous scaled run from one type-7 source column and applies
the following distinction:

- `scale_step == 0`: no destination draw.
- `scale_step == 1`: gate that single destination against the
  original signed wall depth and argument 6.
- `scale_step > 1`: trim the run at viewport edges, then compare
  **only the first and last** destination ray depths. If both are
  drawable, dispatch the whole span through `36B14`. If one endpoint
  is blocked, trim that contiguous edge inward until the first
  unblocked ray is found. If both endpoints are blocked, suppress the
  entire run even when an interior ray is unblocked.
- The left/right code paths advance their independent horizontal
  cursors differently. For the right side, the final trimmed width
  determines the start of the following source column.

This means that a 3-pixel run with front-of-wall depths at its first
and last pixels remains a **3-pixel submitted span** even when the
middle ray depth is behind a wall. The former independent-ray filter
introduced holes that did not exist in the original draw calls.

The repaired implementation is
`type7_column_traversal::visible_runs`, which returns original
`36B14`-sized groups. `visible_columns` now only expands the
accepted groups for the per-destination microcode descriptor model.
`world_scene_render_plan` retains **both** `visible_runs` and
`visible_columns`. It does not claim that either has yet rendered
pixels.

New regression `type7_visible_runs_tests` covers the two-endpoint
case, both-endpoints-hidden case, left/right edge trimming, the
shifted right cursor after trimming, and viewport-edge clipping.

## Follow-up: stateful VGA scene trace (October 7, 2026)

A direct dump of CODE.2's object-2 data establishes
`BYTE[0xC029] == 1` at game start (physical file
`0x75F24 + 0xC029 == 0x81F4D`). The instruction at `0x6617`
can XOR it with 1 later. Thus the initially active `36B14`
path is the per-destination-column form, but the grouped-run form
is also reachable. The `type7_vga_plane_masks` decoder now
exposes the **initial** value without treating it as immutable
live gameplay state.

The new `type7_vga_scene_trace` composes the retained
`Type7VisibleRun` records, the original `369D8` source-column
descriptor list, the exact SINTAB entry/stop operands, and the
original three VGA mask passes. It specifically models the
`36B14` byte-swap boundary correctly:

- `369D8` traverses **all** entries in one source column's display
  list within the same VGA pass; this is not one `36B14` call per
  descriptor;
- with `0xC029 == 0`, one grouped `369D8` is dispatched per run,
  then `XCHG AL,AH` changes the saved lookup word for the next run;
- with `0xC029 != 0`, the original run is dispatched one destination
  column at a time, parity-selecting the lookup byte order; the
  caller-visible global lookup is restored after that run.

Regression `type7_vga_scene_trace_tests` covers the distinction
using two source-column runs, one with **two descriptors**, plus
page-1 VGA addresses and the byte-swapped lookup sequence. An invalid
run width greater than the certified 1..7 mask-table domain is
rejected rather than silently narrowed to an unrelated valid slot.

**Boundary:** The trace is an execution-input record, not a completed
pixel output. This stage does not claim that the SINTAB-generated
instructions are decoded, interpreted, or rasterized. The scene
presenter continues to count visible type-7 columns as pending, and
unrendered objects remain non-clickable.
