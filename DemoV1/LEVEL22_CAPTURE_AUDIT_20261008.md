# Level 22 user capture audit — 2026-10-08 (EDT)

**Evidence:** `RLOFT-Demo-Captures.zip` uploaded by the project user.
The private uploaded binary data (game content) is **not committed**.
Source comparison: immutable original `LEV022.DAT` and
`LEVELS.DAT` supplied separately in the project; current native
`demo-v1` renderer and original Level 22 resource-bank decoding.

## What was captured

Two opt-in sessions, with **14** and **16** native frames; **30 total**.
Every frame has a 128,000-byte 320×400 indexed framebuffer,
a 768-byte RGB palette, and a v1 capture TSV. All 30 palettes
are byte-identical. Captures cover level 22 cells
`(4,4)` through `(8,6)`; camera headings approximately
261..308 degrees. The first seven indexed captures from the two
runs are byte-for-byte identical; this establishes deterministic
repeatability for those identical native views, not DOS parity.

Across all TSVs:

- **7,649 coalesced wall-run rows**:
  **7,649 ordinary/main and zero prepass**.
- Every wall-run resource ID is decimal **147 = 0x93**,
  the established non-drawable sentinel.
- Widths: **7,618** runs of width 1 and **31** runs of width 2.
  Therefore none exercises the new width-8 or wider wall paths.
- Across all frames, native `main_runs` and `prepass_runs`
  counters are **0 rasterized / 0 pending / 0 failed**.
  There were no wall-render failures to debug.
- The native framebuffer pixels differ with camera heading and
  some viewpoint changes. For example, session two frames 7 and 8
  share heading 308 but move from cell (4,4) to (5,5);
  **13,883 / 65,536 (21.18%)** of the 256×256 indexed viewport
  sample positions differ. The fixed lower HUD is unchanged.

## Cross-check against original game content

Both original level-22 archive and direct-save input encode a
**30×15** cell grid: **364 code-0 interior cells**, exactly
**86 code-1 perimeter cells**. There are no interior wall-cell
codes other than 0. The original level header primary-A and
primary-B resource banks each map code-1 to **0x93**,
explicitly excluded by `ViewSweepRun::drawable()`.

This is strong independent evidence that the all-0x93 native wall
runs from these views are **expected for Level 22**.
Do not call them an unconnected SINTAB backend or force a
non-authentic RES4 texture to make its wall renderer look active.

The original LEVELS.DAT Level 22 has **one NPC** of catalog type
**9**, whose compact record places it at **cell (13,7)**, and
**110 placed-object** compact records:
52 type **62**, 42 type **63**, 16 type **64**. Zero compact
loose-item records. These are project source records; current
native animation/visibility of each has *not* been certified
by this capture.

The authentic Level 22 task is to validate source-backed
`PlacedObjectSceneModel`, `NpcSceneModel`, RES3/type-7 VGA
raster coverage, projection and occlusion, plus Level 22
interaction/assassin/relic behavior. The new ordinary SINTAB
wall renderer should instead be evaluated in a genuine
wall-bearing level with original RES4 content and a DOS
reference capture.

## Acceptance impact

Last observed manually requested Windows run
[#638](https://github.com/Hyperbog/RLOFT/actions/runs/37864049601)
compiled, ran **159/159** CTests successfully, packaged
`Capture-Level22.cmd` and committed its Windows binary.

The 30 images provide **native host frame output only**:
no original DOS VGA frame, live DGROUP memory, original IRQ
timing or gameplay-state replay was provided. The captures
prove neither original DOS visual equivalence nor that all
placed-object sprites have been rendered.

**Formal demo acceptance remains 1/5 gates (20%)**; full
engineering progress is unmeasured.

## Recommended next verified work

1. Extend *opt-in only* Level 22 forensic TSV with type-7
   scene records, draw calls, visible-column counts,
   pending/rasterized/failure counts and exact failures.
   Preserve existing exported v1 fields and unmodified
   indexed/6-bit-palette bytes; version any schema additions.
2. Capture views nearer the source-documented NPC at (13,7)
   and representative placed-object types 62,63,64.
3. Verify native RES3 sprite/scene pixels against original
   DOS indexed frames and compare collision/interaction
   behavior. Do not invent animations or skip unsupported
   shape geometry.
4. Test ordinary SINTAB wall fidelity using a **different**
   actual wall-bearing level; do not use open Level 22 as
   its acceptance fixture.

## Opt-in telemetry follow-up (source-only)

The above user captures use TSV v1, whose schema does **not**
include `world_scene_records` or type-7 raster counts. Therefore
their visible trees cannot be attributed to RES3 scene execution
or RES8 environment overlays from TSV alone.

New TSV v2 opt-in instrumentation adds the original-world scene
record/draw-call pipeline, source indexes, resource IDs, visible
columns, executed/failed type-7 raster counts and failure reasons.
Only optional metadata output changed. The source patch has not
been checked by a new Windows CI run; build #638 remains the
last observed successful binary.
