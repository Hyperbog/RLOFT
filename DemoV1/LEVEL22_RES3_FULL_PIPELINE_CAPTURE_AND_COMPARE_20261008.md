# Larger Level 22 RES3 rendering + DOS comparison checkpoint

**Branch:** `demo-v1` · **Date:** 2026-10-08 EDT · **Status:** source
committed; current-HEAD MSVC/CTest and actual DOS parity **not run**.
GitHub Actions remains **manual dispatch only**.

## Evidence / priority

The project user's 30 native Level 22 captures have **7,649 / 7,649
non-drawable `0x93` wall runs**. Both original `LEVELS.DAT` and
`LEV022.DAT` contain outdoor/open interior cells, a perimeter, and
no ordinary drawable RES4 walls. Level 22's actual source-backed
content includes one type-9 NPC at (13,7) and 110 placed objects
(types 62, 63, 64). Therefore wall SINTAB code does not explain
the lack of visible change in those test views; the RES3 pipeline
must be evaluated.

## Renderer: two-stage per-original-source fault isolation

**Previously:** both the RES3 per-scene `WorldSceneRenderPlanModel::build`
and optional `Type7VgaFrameTraceModel::try_build` were capable of
throwing before the presenter could draw otherwise-valid objects.
The previous checkpoint isolated only VGA trace creation. A malformed
**earlier** RES3 source could still abort *all* scene planning.

**Now:**
- Existing strict `WorldSceneRenderPlanModel::build` and
  `Type7VgaFrameTraceModel::build` remain fail-fast for explicit
  verification workflows.
- New `WorldSceneRenderPlanModel::try_build` shares exact planning
  logic, catches **per-call** source-local errors, and returns
  one ordered plan slot for **every** original CODE.2 draw call.
- A failed plan slot carries `source_unresolved=true`,
  its original `call` identity, and a separately recorded
  `WorldSceneRenderPlanFailure` with call index, class, original
  source index, absolute RES3 resource ID and failure text.
  It has **unknown** visibility, never fabricated geometry.
- The downstream type-7 VGA trace treats these slots as
  **unresolved null traces**, not successful fully-occluded objects.
  Valid neighboring calls may still use original SINTAB microcode,
  sequencer masks and transactional VGA execution in draw order.
- `ExplorationSurfaceCoverage::complete` now fails on
  unresolved planning or rasterizer errors, even if no other
  visible columns remain. A malformed unknown-shape source
  does **not** count as a successfully drawn or hidden sprite.

No RES3 asset, original VGA pixel arithmetic, game world state,
collision, random number flow or original interaction target
selection was rewritten.

## Source provenance and precise object outcomes

The native `WorldSceneRecord` carries
`source_type_id` from the **actual original** NPC catalog
or placed/loose object source, appended *after* the existing
stable scene fields. This is **not** part of the 0x16-byte DOS
runtime record. It changes no RES3 lookup/ranking semantics.
It makes Level 22's original type-9 NPC and types 62/63/64
objects distinguishable in the capture without guessing by
sprite image.

The source-aligned `ExplorationPresentedFrame` outcome vector
classifies each original call as `planning-failed`,
`occluded`, `trace-unavailable`, `rasterized`, or
`raster-failed`. A successful renderer invocation means
successful native source-backed bytecode execution; it is not a
DOS parity assertion and may involve zero opaque pixel changes.

Optional TSV v2 fields now include `scene_plan_failures` and
source type on `scene_record`; per-call `scene_plan_failure`
and `scene_outcome` rows; explicit `planned/unresolved`
on each `scene_call`; and earlier `scene_call_trace_failure`
and `scene_raster_failure` rows. All original v1 fields
and raw `.idx`/`.pal` pixel bytes remain unchanged.
The preview window title now includes original sprite plan
errors in RES3 displayed totals.

## Future exact indexed-frame DOS comparison

New standalone `tools/compare_indexed_captures.py` uses ONLY
Python standard library, compares complete **320×400 8-bit
palette-index** images and **768-byte original six-bit VGA
RGB** palettes, and matches only the exact
(level, integer cell X, integer cell Y, facing) tuple.

It requires **independently provided** reference `.idx`,
`.pal` and manifest; a DOS manifest must declare
`provenance\toriginal-DOS` and
`format\trloft-dos-indexed-capture-v1`.
That is a **self-declared provenance label, not a cryptographic
attestation** of emulator provenance. Native-to-native
comparison is allowed only when explicitly requested and
labelled. Mismatched frame lengths, malformed palette values,
duplicate poses and invalid provenance **fail closed**.
The report contains exact byte mismatch counts, first
differing pixel coordinates, palette mismatch counts,
SHA-256 hashes and unmatched poses. A partial overlap
cannot silently return all-frame success.

For example, after supplying a real **external DOS emulator**
capture in the same raw layout:

```powershell
python DemoV1/tools/compare_indexed_captures.py --native C:\native-captures --reference C:\dos-reference --out comparison.json
```

The script NEVER builds or invents DOS reference frames.
Equal pose alone does not imply equal sub-cell position,
animation tick, DAC change timing, interrupt order or
NPC/state snapshot. An all-zero byte diff measures exact
captured bytes, not full game fidelity.

Example DOS reference manifest `frame_0001.tsv`, assembled
**only with actual emulator-dumped binary pixel/palette data**:

```text
format  rloft-dos-indexed-capture-v1
provenance  original-DOS
dimensions  320  400
palette  256x3  VGA-6bit-raw
level  22
cell  4  4
facing  261
```

Tabs, not spaces, delimit fields. This format is a comparison
transport, not evidence that DOS emulator acquisition has
already occurred.

## Regression and build boundaries

Extended preexisting assertion-enabled CTests:
- `world_scene_render_plan_tests`: valid / corrupt / valid
  planning with strict-vs-recoverable contracts and call order.
- `type7_vga_frame_trace_tests`: failed earlier source stays
  a null trace, not hidden/successful.
- `exploration_presenter_tests`: unknown source, truly
  occluded source and valid visible source are distinct;
  incomplete status cannot be falsely accepted.
- `npc_scene_tests`, `placed_object_scene_tests`,
  `loose_item_scene_tests`: exact original source catalog IDs.
- `exploration_frame_capture_tests`: exact source ID,
  source-type, planning failure and per-source outcome rows
  while preserving lossless indexed and DAC files.

New Python `indexed_capture_comparison_tests` (six synthetic
fixtures) is registered when Python3 is available. This gives
**160 registered CTests expected on the standard Windows
runner**, versus 159 in previously successful manual build #638.

**No current-HEAD Windows CTest or original DOS comparison was
run by this source-only pass.** Manual Actions workflow dispatch
is unchanged; Linux remains opt-in/off by default. Formal
demo acceptance **remains 1/5 gates (20%)**. Engineering
completion has no defensible whole-game percentage.

Next: manual Windows CI; if successful, capture Level 22
with v2 telemetry and inspect source-indexed projections
around actual placed objects and original NPC. Then obtain
actual DOS indexed frames and compare exact bytes, with
separate IRQ/state-tick parity evidence.


## Final static-audit hardening before manual build

A final source pass hardened two fail-closed invariants:

- Strict `Type7VgaFrameTraceModel::build` explicitly rejects a
  `source_unresolved` native planning placeholder with
  `std::invalid_argument`. Its optional `try_build` still keeps
  that call as a null trace, preserving valid neighbors.
- `ExplorationPresenter::surface_coverage` counts the greater of
  logged planning failures and actual unresolved plan slots,
  preventing a missing error-list entry from making an unknown
  sprite appear fully occluded. Regression tests include both
  the strict exception and the missing-metadata case.
- The frame comparator returns a nonzero CLI status for *any*
  unmatched pose even when all overlapping poses match bytes.
  A partial intersection is a diagnostic, never proof of
  full captured-sequence equivalence.

The latest audit verified that the updated source and tests exist
at the `demo-v1` branch HEAD and that the workflow is still
`workflow_dispatch` only. GitHub Actions run #638 remains the
latest completed run, so **none of this new code is claimed to
have passed a current-HEAD Windows CTest or DOS parity check**.
