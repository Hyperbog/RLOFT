# Demo v1 — development progress versus formal acceptance

**Verified corrected-ray native capture (Windows #640):**
Manual Actions #640 at source `47d5194a0c87109ff9c26df7794c960d492ccb99`
passed **160/160 Windows CTests**. The new 16-frame
user ZIP embeds that same revision in every manifest.
It traverses cells (4,4) to (8,6) and exercises
**2,127 rasterized RES3 scene calls**, 127 occluded
calls, zero raster/trace/plan errors, 25,282 rasterized
source columns, and 521,921 indexed VGA writes.
Six exactly camera-matched comparisons against the old
buggy-ray capture differ by 332–462 indexed pixels with
unchanged palettes. This verifies measurable native
rendering progress, **not DOS framebuffer parity**;
no sweep failure is recorded in the 16 supplied frames.

**Subsequent source-only capture extension:** original
16-frame cap still cuts off before reaching the NPC
at (13,7); new `--capture-frames=N` is validated for
1..128 and the optional .cmd requests 64 captures,
without affecting normal gameplay or uncapped fatal
failure.tsv. The next Windows build is **manual-only**
and must verify this changed source. Formal acceptance
**remains 1/5 (20%)**.
[Complete audit](LEVEL22_640_POSTFIX_RENDER_AUDIT_AND_EXTENDED_CAPTURE_20261008.md).

**Second user native frame archive and uncapped fatal sweep telemetry
(2026-10-08):** A new 16-frame ZIP covers outdoor Level22
cells (4,4)→(19,4) at heading 270, with 1,157 successful
RES3 draws, 369 occluded scenes and no captured RES3 errors.
No RES3 width >=8 was used, and all 4,080 wall runs were
non-drawable 0x93. The previous capture's **16-frame cap**
still prevented recording the actual later sweep failure pose.
Opt-in captures now append a `failure.tsv` for an exception
from `build_frame()`, regardless of frame count, and stamp
the checked-out source revision into normal frames.
The new file and a regression for its exact source/pose/error
metadata are committed but **not yet Windows-compiled**.
Previous manual build #639 predates the earlier cell-index fix.
[Capture and failure audit](LEVEL22_SECOND_CAPTURE_AND_FATAL_SWEEP_EVIDENCE_20261008.md).
No additional accepted gate.

**User-reported near-cardinal first-person sweep crash
— source-addressing correction (2026-10-08):**
Original CODE.2 32BF6/32BFA/32C1F and 32C07/32C10/32D09
confirm flattened cell indices are converted to 16-bit
byte addresses with SHL and restored with SHR. Native
`trace_ray` previously applied the SHR again, halving
both indices and selecting incorrect cells. This explains
the replayed guard exhaustion and makes prior native
visibility, occlusion and pixel captures **non-certifying**.
Both branches are fixed and a source-backed Level 22
regression added; **current-HEAD Windows build pending**.
[Root cause and evidence](LEVEL22_SWEEP_CELL_INDEX_CORRECTION_20261008.md).

**High-impact graphical fidelity checkpoint — original CODE.2
C029 individual-column scene mode (2026-10-08):**
The native RES3 scene pipeline now supports original source
runs 8..255 wide when C029 selects per-pixel mode, which is
**the value of the shipped original CODE.2 initial byte**.
Previously the engine rejected all widths above 7
indiscriminately, potentially suppressing large scene
objects and NPCs on outdoor Level 22. The native pixel
interpreter itself remains unchanged and now receives
original per-pixel source runs without grouped-mask
fabrication. The grouped/width-256 boundaries remain
explicitly unverified. Tests cover actual planar VGA
stores, shade swaps, original masks, descriptor sequence
and ray viewport bounds. This source has **not been
built on current-HEAD Windows CI** and no DOS frame was
compared. Formal accepted gates **remain 1/5**.
[Detailed source proof](LEVEL22_CODE2_C029_INDIVIDUAL_RES3_WIDE_PIXELS_20261008.md).

**Larger original RES3 scene pipeline checkpoint (2026-10-08):**
The last source pass fixed frame-wide failure propagation at
*both* source-indexed RES3 planning and VGA tracing. Invalid
individual shapes remain unresolved without inventing pixels;
other valid scene calls retain original order and can render.
The static completion score now reports failed planning and
failed rasterization, rather than conflating unknown objects
with fully occluded scenes. Original catalog type IDs are
retained as native metadata, and the opt-in capture includes
per-source outcomes. A raw 320x400 indexed frame/6-bit DAC
native-versus-real-DOS comparator was also added, with a new
Python CTest and strict source/provenance checks.

**Build status:** last passed manual #638 had 159/159 tests.
These commits are **not yet tested** by a new workflow; 160
CTest registrations are expected with Python. No new formal
acceptance gate is passed, no actual DOS reference has been
collected, and whole-game engineering completion is unknown.
[Full checkpoint](LEVEL22_RES3_FULL_PIPELINE_CAPTURE_AND_COMPARE_20261008.md).

**RES3 per-scene fault isolation (2026-10-08 source checkpoint):**
Previous optional type-7 trace generation discarded every
scene in a frame if any RES3 call threw. The updated
`try_build` preserves independently valid ordered scene
traces and reports only the failed source as a null slot
with original class, index, resource, call order and reason.
Unsupported wide calls remain unresolved; globally invalid
page/resolver remains fatal. Existing frame trace and capture
regressions now exercise partial recovery. The voluntary
native preview title exposes RES3 call and trace-error
counters. This patch **has not run on Windows CI** and
adds no formal accepted gate.
[Checkpoint](LEVEL22_RES3_SCENE_TRACE_ISOLATION_20261008.md).

**Corrected Level 22 visual acceptance scope (2026-10-08):**
Analysis of 30 user indexed frames and both immutable Level 22
records shows all 7,649 emitted wall runs are non-drawable resource
0x93 in an open-field perimeter level. No SINTAB main-wall
visual improvement can be demonstrated in the captured Level 22
views, irrespective of engine backend status. The source archive
holds one type-9 NPC and 110 placed objects: future Level 22
visual evidence must prioritize original RES3 sprite/scene
coverage, not RES4 walls. The optional capture TSV is extended
to v2 with source-indexed scene counts and failures (source-only,
new Windows build pending).
[Full audit](LEVEL22_CAPTURE_AUDIT_20261008.md).

**Optional native diagnostic capture checkpoint (2026-10-08):**
A `--capture-render` Level 22 mode and packaged
`Capture-Level22.cmd` launcher now serialize up to 16 changed-pose
320×400 native indexed frames, palettes, exact generated wall-run
geometry, original static CODE.2 mask-byte readings, and failure
reports. The new `exploration_frame_capture_tests` regression is
CMake-registered, **not yet run on the current HEAD**. This capture
is not a DOS reference and does not establish pixel equivalence.
[Evidence and limitations](LEVEL22_NATIVE_FRAME_CAPTURE_CHECKPOINT_20261008.md).

**Subsequent original wide-wall bounded checkpoint (2026-10-08):**
Source-backed C6F4 coalesced wall widths now include exact width
8 at phase 0, 1, or 2, where original CODE.2 masks select eight
physical pixels. Phase 3 reads past the source mask array and
remains unsupported. The native preview title now reports SINTAB
rendered/total counts and wall errors for user playtests.
Registered CTests and documentation are committed; current-HEAD
MSVC/visual comparison remain pending.
[Width boundary](LEVEL22_CODE2_WIDE_MAIN_SURFACE_MASK_BOUNDARY_20261008.md).

**Next source checkpoint — original ordinary SINTAB walls (2026-10-08):**
The `3263C/FCD0=0` main wall rasterizer is now connected to the
actual Level 22 presenter for original grouped widths 1..7, using
source RES4 bytes, SINTAB generated instructions, CODE.2 planar VGA
masks and existing original shade arithmetic. Bounded unit tests
and failure telemetry are registered. A current-HEAD Windows CTest
run and reference DOS pixel/IRQ comparison are **pending**; long
unsupported coalesced runs still need original treatment. This is
an implemented visible-rendering path, *not* another accepted
release gate and not a newly compiled/downloadable EXE.
[Checkpoint](LEVEL22_ORIGINAL_SINTAB_MAIN_WALL_CHECKPOINT_20261008.md).

**Baseline:** 2026-10-08. **Branch:** `demo-v1`.  
**Purpose:** Prevent the historical five-gate release score from being mistaken for total engineering completion.

## Two different measures

**Formal release acceptance: 1/5 major gates (20%).** This is a *discrete acceptance count*; each gate contributes exactly 20 percentage points irrespective of effort. A subsystem can acquire substantial verified source work without crossing a new gate. A jump to 40% therefore would not imply that another 20% of actual labor was performed in that interval. The five-gate score is retained for historical comparability and must never be called an estimate of engineering hours, feature implementation coverage, or full-game completion.

**Development progress: substantial, but not quantitatively measured.** The program has reusable core implementations, many exact original-byte regression anchors, and native Level 22 behavior, but we do not currently have an effort-weighted, coverage-based work breakdown sufficient to give an objective overall percentage. Do not promote a subjective 40%, 50%, etc. to a certified completion rate. If reporting a rough planning estimate, label it expressly as low-confidence opinion.

## Current functional scorecard

| Workstream | Demonstrated progress | Remaining criterion blocking faithful-demo completion |
| --- | --- | --- |
| Boot and character creation | Implemented native original cold start and CODE.1 creation sequence; past Windows builds verified | Preserve accepted entry flow while proving any still-unverified full game references |
| Level 22 exploration and map | Direct-entry playable walkaround, authentic level content, automap/HUD, partial native rendering | Ordinary type-6 main-wall SINTAB, correct scene/pixel composition and original movement/IRQ timing traces |
| Combat and character progression | Extensive original CODE.2 reverse engineering, native attack/HP/XP/hand/level-up models and focused tests | Actual source-ordered input→attack→damage→death→drops→XP runtime integration with shared RNG and clocks |
| Inventory and object state | Bounded original item/hand swaps, cursor inventory interactions and tests | Full use/equip/container semantics and DOS state equivalence for edge cases |
| DOS VGA/HUD/sprites | Real native planar VGA writes, transparent and shaded original sprite raster, original font/glyph service order, code self-patching and sequencer state | Bind actual runtime resource/shader memory, implement 1A8C8 composition/refresh, compare original DOS VGA plane contents |
| Story, transition, audio, timing | Original data/event archaeology and bounded native transition/cinematic code; reconstructed independent timing domains | Complete Assassin relic and Level 23 transition, exact audio/animation/event order, full original-DOS A/B trace |
| Test/build confidence | Windows x64 #634 built successfully with **130/130 CTests**, plus subsequent strict local component tests | Rebuild and run consolidated Windows CTest suite at *current branch HEAD*; verify reference DOS runtime, frames, game-state/timing equivalence |

The test result **130/130** belongs specifically to earlier Windows Actions run [#634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281), source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`. It does *not* cover CTests added after that run.

The **653 named CODE.2 instruction-byte checks at 645 distinct sites** are evidence that many original routines have been checked; they are **not** an executable coverage percentage, a measure of the game's total engine size, or proof of native runtime equivalence.

## Rules for subsequent progress reporting

1. State **formal acceptance** as `X/5 gates`, not simply `X%` without explanation.
2. Report meaningful newly integrated source and tests independently from acceptance.
3. Distinguish **read-only model**, **callback or isolated implementation**, **connected runtime**, **Windows verified**, and **reference-DOS verified**.
4. Never attribute an older green Windows build to source introduced after that build.
5. Add a numerical engineering-completion estimate only after defining and weighting the remaining subsystem work, or explicitly mark it as a low-confidence subjective planning estimate.

**Bottom line:** Saying only “20% overall complete” understates the real source-reconstruction progress while overstating what the single passing release gate measures. Retain **20% as formal acceptance only**; the development-completion percentage is **not yet objectively measured**.


## Subsequent source checkpoint: original 1A8C8 composition (2026-10-08)

The [CODE.2 1A8C8 checkpoint](LEVEL22_CODE2_COMPOSITE_1A8C8_CHECKPOINT.md)
adds signed original overlay-intersection and
original 1,024-byte backing-buffer movement order
to the native timed-message rendering pipeline.
It also connects the compositor's callback interface
to the existing 1C258 message renderer. This is
genuine additional reusable-engine implementation;
the nested 2A078/1A288/1AB20 visual effects and
reference-DOS framebuffer comparisons remain open.

The original executable-byte audit now has 684 named
checks at 676 unique sites. The newly added CTest
is registered but has not been included in a fresh
full Windows build. Thus the formal acceptance
score remains **1/5**; engineering completion remains
**unmeasured**, not 20%.


## Subsequent 2A078 / 30CD4 reusable-engine work

The source checkpoint
[original 2A078 VGA latch copy and 30CD4 DGROUP transfer](LEVEL22_CODE2_2A078_30CD4_COPY_CHECKPOINT.md)
adds **actual four-plane source-backed VGA rectangle
writes** to the 1A8C8 compositor's direct path, and
the original 1024-byte overlay-buffer transfer to
its buffered path when genuine overlay collaborators
are available. It preserves the key difference between
forward, potentially aliasing VGA `REP MOVSB` and
overlap-aware DGROUP `30CD4` moves.

70 new original-byte signatures matched CODE.2; total
is **754 named / 746 distinct code locations**.
Committed 2A078 and 30CD4 core source passed isolated
strict C++17 Debug/Release/UBSan testing. New CTests
are CMake-registered but not covered by the last complete
Windows MSVC run. Original overlay suspension/restore
and DOS pixel/IRQ comparison are still unaccepted.
These accomplishments improve shared engine coverage
but do not constitute another formal end-to-end gate.


## Latest original overlay implementation — 1A288/1AB20

Source-backed native callbacks now preserve the
original 1A288/1AB20 16-bit overlay reference
counter, first-activation capture, conditional
draw modes, and last-reference restoration
ordering, including the full 1A8C8 compositor
handoff. The source checker has **803 named
x86 byte checks at 795 distinct addresses**.
Focused local Debug, Release and UBSan checks
passed for the overlay branch engine; the
new integrated CTest awaits a complete Windows
run at this branch HEAD.

These are additional reusable engine milestones.
The underlying page-stack, graphics capture and
DOS timing/pixel equivalence remain unaccepted,
so **1 of 5 formal release gates** is still the
correct discrete acceptance score, not the
percentage of engineering completed.


## Original VGA page-stack progression — 2026-10-08

New source-backed 2E590/2E5D4 native execution
now preserves the actual BYTE F0D0 page-stack,
saved EF4D distinct from EFE9, and dynamic
F001 addressing through original F005.
The overlay presentation callbacks can use
this shared VGA selector for both suspend and
restore, and the binding synchronizes actual
2A078 copy-model page addresses. The code
byte verifier now has **826 named checks at
818 distinct addresses**.

The exact committed implementation and binder
passed isolated strict C++17 Debug, Release and
UBSan tests; the new integrated repository CTests
are registered but await a current-HEAD Windows
build. The original overlay backing read/write
service pixels and DOS reference comparisons
remain unaccepted. **Formal release acceptance
is still 1/5 gates**, while engineering completion
is not objectively weighted.


## 292BC/2932C original backing-clip source milestone

The reusable overlay renderer now includes
the original signed clipping and backend
dispatch differences for 292BC capture and
2932C restore, including original zero-width
restore behavior. Its adapters compose with
the reconstructed 1A288/1AB20 lifecycle
and the previously completed 2E590/2E5D4
page stack.

The original executable audit is now
**855 named byte checks / 847 distinct
instruction locations**. Exact committed
component sources and bindings passed
isolated Debug/Release/UBSan checks, but
their newly registered CTests and pixel
effects remain outside the historical
Windows 130/130 runner result. The next
faithful-graphics acceptance blocker is
2A114/2A178 four-plane backing bytes and
the remaining original overlay art/position
services. Formal release acceptance remains
1/5 gates; development progress is not a
measured global percentage.


## Subsequent native VGA backing capture/restore milestone

The original backing-image capture and restore
calls `2A114` and `2A178` now have native
reusable four-plane VGA byte models and a
binder for existing `292BC/2932C` clipping.
The graphics-controller read-map selection and
sequencer raw `FF02h` write are preserved,
including the zero-logical-column restore
path. Exact new CODE.2 source signatures:
**53**, bringing the ledger to **908 named /
900 distinct instruction sites**.

The committed source passed isolated strict
C++17 Debug, Release and UBSan tests. The
two new CTests are CMake-registered but need a
fresh Windows run and eventual DOS pixel
comparison. Shared graphics implementation
has advanced further, but **1 of 5 major
formal demo gates** remains accepted;
engineering completion is not objectively
quantified.


## Original 1A3B8 / 2AE30 / 29804 subsystem progress

Source-backed shared overlay position, opposite-page
record hit testing and nested rendering bookkeeping
are now part of the native C++17 codebase, with a
test that binds all three models through the
existing original 1A288 activation/1AB20 restoration.
They preserve original 9E35 output, source callbacks,
22-byte entity records and EFD4 scratch-record depth.
Source-byte ledger: **1,004 named / 996 distinct
instruction anchors**, from 96 new checks.

Core sources passed independent Debug, optimized
Release and UndefinedBehaviorSanitizer local tests;
new registered CTests and complete Windows EXE still
require a current-HEAD build. Full native overlays
still depend on true 29414, 2AD18, 2ADE0, 33C90,
1F820 and source runtime memory. Release acceptance
therefore remains **1/5 formally passed gates**,
not an estimate of engineering completion.


## Source-backed type predicates and clipped sprite state — current checkpoint

The 2AD18/2ADE0 original type predicates and
1C1DC live DGROUP world-bit reads are now
implemented and linked to the prior 2AE30
opposite-page hit scanner. Original 29414
source-level row/column sprite cropping is
also native, with exact EF74 24-byte records,
EF69..EF6F output WORDs and original 0/1/2
dispatch. The previous EFD4 drawing wrapper
can invoke 29414. The original A6BC/D680
globals are now explicitly shared or observed
across the overlay position/probe adapters.

New opcode signatures **105/105** matched
CODE.2, yielding **1,109 named and 1,101
distinct original instruction sites**. The
two committed core C++ sources passed
source-identical focused Debug/Release/UBSan
tests. Their three newly registered CTests
and integrated full Windows build are not
yet verified on current branch HEAD.
Actual 2A6B0 clipped pixel rendering and
DOS frame/gameplay reference comparison
remain open. Formal release acceptance
stays **1/5 gates** and engineering progress
remains unweighted.


## Original 2A6B0 clipped four-plane VGA raster milestone

CODE.2 `2A6B0` now executes clipped
opaque/transparent four-plane source bytes,
through a real `29414` callback binder.
The original graphics globals F00D..F031,
transparent sentinel, forward per-plane
copy and mutable literal at CODE:2A816
are modeled. The original-code audit
now covers **1,263 named signatures /
1,255 unique instruction offsets**.

The committed raster and binder passed
focused strict C++17 Debug, Release and
UndefinedBehaviorSanitizer checks. Two
new assertion-enabled CTests are
registered, but no complete Windows
CTest or DOS reference pixel equivalence
was run for this newer branch HEAD.
Next: 2A8A0 shaded clipped variant and
original scene/cursor/IRQ state integration.

**Formal demo acceptance remains 1/5
major gates (20% discrete acceptance).**
That percentage is **not** a weighted
estimate of actual engineering completion.
