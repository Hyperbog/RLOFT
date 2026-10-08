# Demo v1 — development progress versus formal acceptance
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
