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


## Frame trace integration and left-pass repair (October 7, 2026)

`ExplorationRuntime` now loads the original CODE.2 VGA mask tables
(if CODE.2 is available), captures the initial `0xC029` individual-column
mode and `0xEF4D` video page, and preserves both as **mutable runtime
state**. CODE.2's supplied initial values are `1` and `0`, respectively.
The port does not yet hook all DOS input paths that modify those globals.
Callers can explicitly change them through the scene mode/page setters.

`type7_vga_frame_trace` combines each live `WorldSceneRenderPlan`
with the original RES3 shape and selected SINTAB block to produce a
call-order-preserving trace. A trace vector maintains **one slot per
ordered draw call**. Empty/occluded plans retain empty traces. Calls with
unsupported horizontal runs (`width==0` or `width>7`) retain an empty
optional slot and increment `unresolved_wide_runs`: a partial trace must
not masquerade as a faithful one, nor abort the entire game frame.

The `ExplorationRuntimeFrame::scene_vga_trace` optional remains unset
when the source cannot supply CODE.2. The presenter reports the
`type7_unvalidated_wide_runs` count when a diagnostic trace is present.
Neither a trace nor the screen clipping plan is treated as pixel output;
the existing `type7_scene_calls_pending` and
`type7_visible_columns_pending` remain authoritative for unfinished
rasterization.

### Correction at CODE.2 `3708F -> 370AE`

A re-audit of the immutable machine instructions identified an important
control-flow boundary for the left-side type-7 traversal. After a
multi-column run with the **left endpoint blocked and the right
endpoint visible**, CODE.2 trims and draws the visible suffix, then
jumps to `370AE`, entering the **right-hand pass directly**. The
left-side loop does **not** visit any further left columns.

A preliminary C++ correction incorrectly used `break` on the single
merged left/right loop, which would have omitted the right pass too.
The final version sets `left_pass_finished`, skips remaining left
columns, and still executes the subsequent right source columns.
The regression now requires that a drawable next-left column is
suppressed **and** the first right-hand column is still present.

The following are **not** finished or certified:
- executable SINTAB-generated-code instruction decoding / emulation;
- exact scaled VGA color and vertical texture writes;
- final viewport presentation and original click-to-interact dispatch;
- full Linux/Windows CMake build and all registered tests (GitHub
  Actions presently ends both jobs before step 1).


## SINTAB source-data closure and first real pixel execution (October 7, 2026)

The original `SINTAB` file was uploaded to the Project conversation and
audited as immutable controlling evidence:

- **333,757 bytes**, SHA-256
  `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`
- **127 generated rasterizer blocks** and **512 final padding bytes**
- **259,457 generated x86 code bytes**
- **16,383 scale-code patch offsets**, all aligned to an instruction
  boundary in the original unmodified code bodies
- **11,587** `mov al,[esi+disp32]` source reads, paired with the same number
  of `mov ah,al` and `sub eax,ecx` operations
- **11,947** `mov [edi+disp32],al` and **11,963**
  `mov [edi+disp32],ah` indexed VGA writes
- all **23,910** pixel-write displacements are multiples of **80** bytes,
  with original offsets from 0 through 20,400
- the only additional unmodified code instruction is each block's
  terminal `ret`; there are no unclassified opcodes in this corpus

An independent canonical-file execution exercise ran all 127 original
SINTAB code bodies with bounded synthetic source/VGA memory, completing
**58,798** generated instructions (including 127 terminal returns), with
the same **11,587** source reads and **23,910** indexed writes. Another
**635** executions inserted a temporary `RET` at representative original
patch-table positions and verified bounded termination. This exercises
original binary instructions using a separate Python reference interpreter;
it does **not** certify a successful C++ demo build or match against
complete DOS screenshots.

These figures are reproducible with:

```sh
python3 DemoV1/tools/audit_sintab_opcode_inventory.py /path/to/SINTAB
```

**Do not commit the original SINTAB asset into the project source tree.**
At runtime, `RendererTables::load` reads it from the original game install.

### Portable byte-exact generated-code interpreter

`sintab_pixel_microcode` is a restricted, non-executable-memory interpreter
of exactly the six original x86 instruction forms. It also accepts the
temporary `ret` and `jmp short` instructions generated by CODE.2's
self-modifying rasterizer setup. It models the original `AL` / `AH`
aliasing, wrapping EAX subtraction, ESI source-byte loads, and VGA
sequencer-mask stores into `VgaPlanarMemory`, without invoking native x86.

`resource_assets` now exposes `decode_type7_body` /
`res3_type7_body`, preserving the **original decompressed RES3 body**.
CODE.2 `369D8` adds each descriptor's signed adjustment to that body
pointer, which is exactly the source base the interpreter receives.

`type7_scene_pixel_executor` resolves the selected native scale block,
applies the `36C10/36C44` mode patches to a private byte buffer,
temporarily replaces the `369D8` descriptor stop byte with `0xC3`,
executes its entrypoint for each original VGA plane pass, restores the
descriptor stop byte and all mode patches, and retains the original
`RendererScaleBlock` unchanged.

Synthetic unit regressions verify AL/AH output, byte-swapped lookup
arithmetic, two-plane VGA masks, descriptor-boundary return patches,
source/VGA memory bounds, and ordinal descriptor/pass output.

An optional test path `RLOFT_SINTAB_PATH=/path/to/SINTAB` causes the C++
unit test to audit the full original 127-block SINTAB corpus without
embedding proprietary data in the repository.

### Exploration framebuffer integration

`ExplorationPresenter::present` now attempts **actual type-7 indexed pixel
drawing** after surfaces and before party-panel HUD chrome. It imports
the existing framebuffer into an original 4-plane VGA page, executes
only validated scene traces, and exports the finished page back to the
320x400 indexed framebuffer.

Each scene call is executed **transactionally** on a private planar-memory
copy: failures cannot leave partially drawn sprites. A successful call
reduces the pending type-7 scene/column counts and increases rasterized
calls, columns and indexed writes. Unsupported run widths or missing
original RES3 data remain pending, not approximated.

This represents genuine execution of the original SINTAB sprite
microcode, but **does not establish complete pixel equivalence for the
entire game**. Remaining tasks include:

1. Compile and run all CMake tests on real Linux/Windows jobs; GitHub
   Actions currently fails before step 1.
2. Execute the rasterizer against supplied actual RES3 shape bodies,
   compare VGA indexed output to DOS reference frames across a variety
   of scales/modes, and verify the complete original horizontal clipping
   and render-phase state machine.
3. Implement the still-pending SINTAB surface/wall rendering and prepass
   paths separately (original microcode source is now available).
4. Verify full Level 22 Assassin visualization, viewport targeting,
   combat/relic interactions and transition behavior end-to-end.

## Canonical pixel-equivalence regression vectors (October 7, 2026)

All 127 original SINTAB generated code blocks were further checked for **pixel values**, not only opcode decoding. A separate Python reference executes each with deterministic source bytes `source[i] = (i * 31 + 17) & 0xFF` and fresh zeroed VGA planes. Each 21,000-byte plane-0 result is FNV-1a-64 hashed; the block index and little-endian per-block digest are then combined.

| ECX lookup | 127-block FNV-1a checksum |
| --- | --- |
| `0x0102` | `0x0658C6DB74B80324` |
| `0x3412` | `0x0D284A7FB40210EE` |

Both expected fingerprints are now enforced by the canonical-asset path of `sintab_pixel_microcode_tests`. A standalone strict C++17 reference built with `-Werror -fsanitize=address,undefined` independently reproduced the same fingerprints from the real SINTAB.

Three descriptor-cutoff cases use original block 31 with entry patch index 0 and stop patch index 16:

| Patch configuration | Source reads | Pixel stores | Expected FNV-1a |
| --- | ---: | ---: | --- |
| Descriptor stop only | 16 | 16 | `0xD17D98730D9A77C5` |
| Mode-0 RET patches from index 8 | 8 | 8 | `0xFF8A6C8D22C4A0C5` |
| Nonzero mode, 4 leading JMP patches | 12 | 12 | `0x44E1511AB24CEDC5` |

These are also asserted by `type7_scene_pixel_executor_tests` when `RLOFT_SINTAB_PATH` points to the original asset. Their independent reference calculations are now in `DemoV1/tools/reference_sintab_pixels.py` and can be reproduced with `python3 DemoV1/tools/reference_sintab_pixels.py /path/to/SINTAB`.

The original SINTAB scale-step histogram is `0:2015, 1:4128, 2:4128, 3:4128, 4:1984`; therefore a normal projected source-column run never exceeds four pixels. The original patch offset tables contain **4,669 adjacent equal-offset pairs**. An EB patch across such an alias can create `EB FE` (jump to itself); interpreter execution is guarded and the new unit test checks this failure mode.

**Remaining limitations:** reference matching covers deterministic SINTAB code inputs, not full live RES3 DOS screenshot equivalence. Surface/wall and prepass SINTAB rendering remains incomplete. The new repository C++ tests are still not independently build-certified because GitHub Actions fails before executing steps.

## Local verification when GitHub Actions is unavailable

GitHub Actions runner capacity or billing limits are **not a source-code commit blocker**. Continue committing validated changes to `demo-v1`; do not report zero-step Actions failures as C++ compiler/test failures.

The following command runs the original-data bytecode, pixel-fingerprint, descriptor-patch, and CODE.2 VGA-mask audits entirely **offline** with Python 3:

```sh
python3 DemoV1/tools/verify_renderer_offline.py /path/to/SINTAB /path/to/CODE.2
```

On Windows, use `py -3` in place of `python3`. Original binaries stay outside Git. The runner checks both canonical SHA-256 identities, all 127 SINTAB blocks, six original instruction forms, 16,383 patch boundaries, 28 CODE.2 phase/run masks, both whole-corpus pixel fingerprints, and the three real descriptor-cutoff vectors. It stops with a nonzero exit status if a check fails.

An asset-free offline regression exercises the CODE.2 mask verification logic with synthetic fixture bytes, including corruption cases:

```sh
python3 DemoV1/tools/test_offline_renderer_verify.py
```

CTest also registers this asset-free Python regression automatically when Python is installed. **Do not conflate** these original-data checks with compiling the C++ renderer or successfully running the full demo; those remain separate requirements.

## Whole-scene VGA execution integrity (October 7, 2026)

The CODE.2 binary was re-read at `369C0..36B95` and `36DAD..36EBA` to close both the SINTAB selector ABI and the VGA dispatch state. In particular, `36EA8` **subtracts `0x204`** from the scale-code pointer before installing it at global `0x105D7`; `369D8` then resolves the original 16-bit descriptors against the 129-word patch prefix, using **nonnegative even selectors in the range 0..0x100**. The previous C++ descriptor-offset representation was correct. Regression now explicitly rejects negative even selectors instead of risking an apparent signed-relative reinterpretation.

However, the original portable type-7 pixel executor trusted an externally built VGA trace without validating all masks and lookup transitions. This could allow corrupted or incomplete traces to be counted as successful sprite rendering.

`Type7VgaTraceValidator` now validates:

- Every destination run's width is in the certified CODE.2 1..7 range, and every destination column is representable.
- The **original `0xC029` branch** (grouped versus individual columns), retained explicitly even for a single-column run.
- Exact three-pass planar masks calculated independently from the horizontal phase and run width; no missing, duplicated, or additional VGA passes.
- The VGA physical-byte increment between lane changes and within sequential pass addresses.
- Per-column parity selection of the original ECX lookup word, including the full value rather than just the low 16 bits.
- `36B14` grouped lookup-byte swapping versus individual-mode lookup restoration, and preservation of the original ECX chain across consecutive source columns.
- Exact run count, source identity, horizontal geometry, and the scene's final lookup word.

`Type7ScenePixelExecutor::draw` runs the **whole-scene validation before any generated-code mutation or planar video writes**, so an inconsistent trace cannot produce even a partial direct-call render. The presenter continues to supply an additional per-scene transactional framebuffer copy for exceptions during actual SINTAB execution. The new `type7_vga_trace_validation_tests` target includes adversarial altered masks, pass indices, addresses, reordered/missing columns, incorrect ECX values and both one-column mode outcomes. Existing original-dispatch and scene-executor tests now also invoke these checks.

**Validation boundary:** Source/test registration and GitHub commits are confirmed, but a complete local CMake build is still required to certify these C++ changes. GitHub Actions runner availability remains independent of the development commits.

## Transactional type-7 pixel execution and Level 22 viewport actions

October 7, 2026 continuation.

### A failure after a successful pixel write no longer corrupts VGA memory

CODE.2 `369D8` enumerates one original RES3 display-list descriptor at a time; for each descriptor it temporarily inserts the stop `RET` byte and executes the scale-code for up to three planar masks before restoring the byte. A reread of `369C0..36B12` confirmed that **descriptor-major, plane-pass-minor** order already used by `Type7ScenePixelExecutor` is correct.

Previously `Type7ScenePixelExecutor::draw` wrote into the caller's `VgaPlanarMemory` directly. A malformed late descriptor could throw after an earlier descriptor had written pixels; only the caller in `ExplorationPresenter` protected against this. The executor now stages the *entire* scene's 4-plane VGA buffer and commits it only after all passes and SINTAB code-restoration checks complete. This makes **direct API calls transactional too**. A new regression starts with deliberately nonzero plane pixels, allows the first descriptor to write, then supplies an out-of-bounds signed source offset to the second descriptor; it requires the original plane bytes to survive unchanged.

The exploration presenter no longer performs a redundant second 256-KiB VGA copy. Resource-cache allocation occurs before the atomic executor invocation, avoiding misreporting a completed render if cache allocation subsequently fails.

### Level 22 viewport combat and loose-item dispatch

`Level22ViewportTargetModel` resolves the original CODE.2 `223F4` selection outcome to its **live NPC or loose-item source index**; the screen record index is not a native family record index. The resolver admits an Assassin only for source class 0 with original `Level22Story::kAssassinNpcType`, and a loose item only for source class 2. Stale out-of-bounds records raise an explicit error rather than redirecting an action to the wrong game object.

`ExplorationRuntime` now provides `damage_level22_assassin_at` and `pickup_level22_loose_item_at`, connecting its pre-existing target selection and faithful damage/loot mutations. The attack action uses **flags=0**, ignoring source-class-2 pickups and preserving click rank among eligible NPCs; pickup uses **flags=1**, enabling the original class-2 gate. Both reuse the existing original hit rectangles and live encounter routines. The new test covers source class and live index validity as well as overlapping NPC/item rectangles and render ranks.

These methods are now available to the input/UI layer; **binding physical mouse actions to these methods and validating the full Assassin death-to-relic-to-mist sequence remains pending**. This change is not a claim that entire combat is playable yet.

### Runtime diagnostics without GitHub Actions

`ExplorationPresentedFrame::type7_failures` now contains the failed original RES3 resource ID, source class/index, and exception message. The original coverage pending counts remain unchanged when a sprite fails. A missing original source, invalid SINTAB instruction, or wrong trace no longer disappears behind a generic failed-count statistic.

All changes and new tests are committed directly to `demo-v1`. The full project C++ build is still unverified because GitHub Actions jobs have been failing before any steps execute.
