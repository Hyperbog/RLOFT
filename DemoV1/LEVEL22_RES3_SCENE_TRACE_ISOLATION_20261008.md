# Level 22 RES3 per-scene VGA trace isolation — 2026-10-08

**Branch:** `demo-v1` · **Status:** committed source and registered
existing CTests updated; no manual Windows build has yet run at this SHA.

## Why this matters

The user-supplied 30 native Level 22 captures show that all 7,649
wall runs are non-drawable `0x93` open-field boundaries. Original
Level 22 gameplay content instead comprises 110 placed objects and
one NPC, with projected RES3 scene shapes.

Previously `Type7VgaFrameTraceModel::try_build` delegated an entire
frame to the strict `build` operation in a single try/catch. If
even one source-backed RES3 shape failed (e.g. malformed descriptor
or unsupported SINTAB offset), the exception discarded **all**
previously successful original scene traces. The native
`ExplorationPresenter` then received no per-call trace and skipped
every drawable RES3 scene object in that frame.

## Faithful bounded change

The existing strict `Type7VgaFrameTraceModel::build` remains
fail-fast: it still validates every source and propagates errors.
A shared per-scene helper maintains the same trace semantics:

- Each visible original CODE.2 36B14 call is validated with its
  original resource ID, SINTAB tables, VGA page and individual/grouped
  mode. The mutable ECX lookup state remains per scene as before.
- Unvalidated width zero or width 8+ is still a **null** slot,
  recorded as unresolved, never rasterized with invented VGA masks.
- Fully hidden calls still produce an empty valid trace; they
  are not counted as lost visible sprites.
- In the **optional recovery** API, a thrown local error produces
  a null slot with **call index, source class, source index,
  absolute RES3 ID and original exception text**. Other calls,
  including later ones, remain eligible for the existing
  transactional original VGA pixel executor.
- A globally invalid VGA page or missing nonempty-scene shape
  resolver remains fatal and returns no frame trace. This is
  not a blanket suppression of invalid input.
- Neither original DOS source files nor the RES3/SINTAB pixel
  executor's drawing algorithm or ordering were changed.

The runtime frame now retains `scene_vga_scene_failures`;
opt-in frame-capture TSV v2 adds a
`scene_call_trace_failure\tindex\tclass\tsource\tresource\treason`
row per isolated error. Existing v1/v2 fields and the indexed
image/palette files are unchanged.

## Regression and release boundary

The existing `type7_vga_frame_trace_tests` now provides a
five-call sequence: valid source, throwing source, wide source,
hidden source, valid source. It asserts the two successful
original traces survive, strict mode still throws, the wide
source stays unresolved, and the correct failed source identity
is retained. The registered `exploration_frame_capture_tests`
checks lossless inclusion of the matching TSV error.

These tests are **registered but unrun** for the new source SHA.
Last previously observed successful manual Windows build is #638
(159/159), which predates this patch. No Actions build was
started. Source-only correctness does **not** establish current
Windows compile success, DOS indexed-pixel equivalence, NPC
animation equivalence or complete original object coverage.

Next manual build: `demo-v1` workflow, Linux unchecked; after
success, capture views containing actual placed objects and the
type-9 NPC at (13,7). Use v2 `scene_video_calls`,
`scene_columns`, `scene_call` and
`scene_call_trace_failure` rows to separate projection/occlusion,
unvalidated generated-code widths and individual source failures.
Formal demo acceptance is still **1/5 gates (20%)**.


## Later larger pipeline continuation

[Full source checkpoint](LEVEL22_RES3_FULL_PIPELINE_CAPTURE_AND_COMPARE_20261008.md)
adds the earlier missing RES3 **planning** isolation, preserves
unresolved call slots, prevents false completed/occluded classifications
and introduces source-type/per-object capture outcomes. It also
provides an independent raw indexed native-vs-DOS comparator.
This follow-up is source-only pending new manual Windows CTest.
