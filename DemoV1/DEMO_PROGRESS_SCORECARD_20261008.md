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
