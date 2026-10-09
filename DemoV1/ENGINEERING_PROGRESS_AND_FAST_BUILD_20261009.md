# Demo v1 — engineering progress and faster manual Windows builds

**Updated:** 2026-10-09 · **Branch:** `demo-v1` · **Scope:** the
**Demo-v1 playable native Strahd's Possession reconstruction**, not
the user's entire original game collection or completed original DOS
game. This is the **canonical rolling engineering-progress measure**.

## Correcting the misleading old "20%" number

Previous documents repeatedly called the project "20% done" because
**one of five formal demo acceptance gates was recorded complete**.
That ratio was **never a meaningful measure of incremental
engineering progress**. Significant reverse engineering, integration,
asset decoding and drawing happened without advancing those five
very coarse gates. We must not show that 20% again as the main
progress/whole-game-completion percentage.

As of this checkpoint the auditable **demo engineering source
milestone coverage is 56.25%**: 32 equal, explicitly bounded
deliverables, 12 source-implemented, 12 partly reconstructed/
integrated, and 8 not yet implemented.

Rules:

- A **source-implemented** deliverable contributes 1 point:
  its specifically defined source-level capability is committed,
  NOT necessarily DOS-correct, fully tested, or visually accepted.
- A **partial** deliverable contributes 0.5 point: real relevant
  source exists but critical integration/behavior is missing.
- An **open** deliverable contributes 0 points: no complete
  claimed capability. Each slice has equal weight 1/32.
- Thus **(12 + 12×0.5 + 8×0) / 32 = 56.25%**.
- Transition **open→partial** or **partial→source-implemented**
  adds **1.5625 percentage points**. Do not raise this score
  for documentation edits, commits, extra lines of code or
  tests that do not actually cross an explicit deliverable.
- Use date/revision and note changed task IDs on future updates.
  Reclassify honestly downward when new tests expose mistakes.
- This is an *initial provisional work-breakdown baseline*;
  it is NOT a measured fraction of total original game logic,
  overall game completion, or 56% fidelity.

Separately track:
**Windows verified:** latest successful manual build #640
(160/160 at source SHA
`47d5194a0c87109ff9c26df7794c960d492ccb99`);
**current source:** expected 164 CTests, compilation unverified;
**DOS fidelity:** native pixels/controls/gameplay have material
mismatches against fresh-party DOS screenshots; **formal
acceptance:** legacy 1/5 gates, a coarse certification scoreboard
rather than engineering percent.

## The 32 stable Demo-v1 engineering milestones

| ID | Bounded source deliverable | State | Evidence / remaining qualification |
| --- | --- | --- | --- |
| 01 | Original loose/GOG media discovery | Implemented | `GameSource` install/ISO reading |
| 02 | Original RES headers, compression and type decoding | Implemented | `ResArchive` EH/LZSS/type definitions |
| 03 | SINTAB table, sine, projection and scale decoder | Implemented | `RendererTables` source prefix/block parsers |
| 04 | Original LEVELS/LEV grids and header parser | Implemented | `ExplorationStateCodec` |
| 05 | Source NPC/object/scene catalogs | Implemented | Original NPCS, OBJECTS, INIT descriptors |
| 06 | Original four-actor 0x920-byte party image layout | Implemented | `PartyImage` binary transport |
| 07 | Original CODE.1-produced CHARS transfer encoder | Implemented | `build_character_transfer` |
| 08 | Original CODE.2 fresh level/pose constants | Implemented | Level22 (4.5,4.5), 270 degrees |
| 09 | Indexed VGA buffer and original palette data | Implemented | 320×400 indexed pixels, 6-bit DAC |
| 10 | Original sweep cell-index address correction | Implemented | CODE.2 left/right cell index undoubling |
| 11 | Original RES3 scene-source descriptor decoder | Implemented | Original type-7 geometry and rasters for supported calls |
| 12 | Original RES4 ground ID and guarded 34A74 core | Implemented | INIT+0x0E + signed level+7; only open Level22 path |
| 13 | Complete cold-start/intro/menu sequence | Partial | Screens modeled, not certified seamless parity |
| 14 | Full interactive CODE.1 creation flow/UI | Partial | Editor/code sources present, DOS behavior unchecked |
| 15 | Seamless CODE.1→CHARS→CODE.2 native handoff | Partial | Integrated, awaiting verified live fresh-party comparison |
| 16 | Player input cadence, movement and entity collision | Partial | Source modeled, unbuilt and scripted effects missing |
| 17 | Original SINTAB walls and occlusion | Partial | Supported runs; other original spans unresolved |
| 18 | Environment backdrop, horizon and tree art | Partial | Native RES8/RES3 source art, visible differences |
| 19 | Non-open-field floor clipping and upper textures | Partial | Full original FF50/C026/C027 state not modeled |
| 20 | Inventory modal presentation and equipment interaction | Partial | Original records present, user-visible mismatch |
| 21 | Automap exploration and modal UI | Partial | Original logic models, DOS screenshot mismatch |
| 22 | Exact party HUD and portraits | Partial | Some original sprites/chrome, not aligned to DOS |
| 23 | Core combat targeting/damage systems | Partial | Numerous reconstructed routines, incomplete runtime linkage |
| 24 | Original audio/MIDI/video synchronization | Partial | Asset parsers and host audio, exact pacing uncertain |
| 25 | Authentic save/load new-game vs restored-state flow | Open | Must not substitute SAVE.DAT for CHARS |
| 26 | Full event-script and authored interaction side effects | Open | Conditional collision events remain suppressed |
| 27 | Continuous quest/world-state progression to endgame | Open | No verified full playthrough |
| 28 | Original time-driven NPC movement/AI encounter behavior | Open | Assassin placement/motion not faithfully timed |
| 29 | DOS-equivalent weapon/hand combat animation timeline | Open | Original screenshots differ |
| 30 | Original PIT/IRQ gameplay cadence measured in DOS | Open | Current 54,925µs tick is provisional |
| 31 | Complete fresh-game-to-finale seamless native playthrough | Open | No evidence of start-to-finish parity |
| 32 | Source-aligned DOS indexed-frame and interaction acceptance | Open | No controlled matching DOS palette/index captures |

**Implemented=12 · Partial=12 · Open=8 · 56.25% engineering
source coverage.** The unverified current-HEAD Windows state
does not invalidate the fact that source has been written; it
prevents treating these implementations as validated features.
A failed new build must be triaged and the classifications
reassessed.

## The measured old Windows build and the bottleneck

Manual Windows run #640:
[GitHub Actions run](https://github.com/Hyperbog/RLOFT/actions/runs/37872583683).

- Job: **7m 40s** from 02:01:38 to 02:09:18 UTC.
- Native MSVC CMake build: **6m 26s** (02:02:16–02:08:42).
- All **160 CTests: 16 seconds** (02:08:42–02:08:58).
- Other checkout, CODE.2 syntax check, CMake configure,
  artifact upload and in-git EXE refresh: ~74 seconds.

This demonstrates that running tests is not the 10-minute
bottleneck: **building all 164 individual test executables**
along with the 200-ish-source library is.

## New selectable manual build workflow

The `.github/workflows/demo-v1-build.yml` workflow remains
**workflow_dispatch ONLY**. It now has the explicit choice
`build_mode`:

| Mode | Default? | Build | Tests | Assurance |
| --- | --- | --- | --- | --- |
| `quick` | **Yes** | `ravenloft_demo_v1`, `rloft_res4_ground_probe`, and 9 targeted tests, plus dependencies | 9 targeted CTests | Fast iteration, **NOT full suite verified** |
| `full` | No, explicitly select | CMake default ALL_BUILD, 164 CTests + demo | Entire suite | Full Windows build verification |

Nine quick tests: `exploration_session_tests`,
`exploration_preview_tests`,
`exploration_gameplay_cadence_tests`,
`original_entity_collision_tests`,
`exploration_runtime_motion_tests`,
`original_ground_texture_raster_tests`,
`original_ground_res4_audit_tests`,
`exploration_presenter_tests`, and
`exploration_frame_capture_tests`.

Other latency improvements:

- Both Windows CMake build modes use MSBuild
  `--parallel 2`; CTest modes use `--parallel 2`.
- The main native `rloft_boot_flow` target uses
  MSVC `/MP2` to compile two source files concurrently
  in that otherwise monolithic static library.
- `actions/cache@v4` restores/saves the build directory
  with a source-revision key and prefix fallback. This
  is **experimental best-effort incremental reuse**;
  source checkout timestamps, compiler change, cache
  expiry and stored directory size can cause a miss or
  force recompilation. Do not promise cache-based speed.
- Neither mode automatically starts on pushes, docs,
  release tags, pull requests, or source commits. Linux
  remains opt-in and off by default.
- Both modes stage the real `ravenloft_demo_v1.exe`,
  `Probe-Level22-Ground.cmd` plus its executable,
  `Capture-Level22.cmd`, `Explore-Level22.cmd`,
  `build-source-sha.txt` and new
  `build-verification-mode.txt` (literal
  `quick` or `full`). A quick artifact must
  not be relabeled as passing all 164 CTests.
- The pre-existing artifact git-commit step still
  updates the latest demo EXE, and preserves its
  built-source SHA even if the branch moves meanwhile.

**Next manual run:** choose `quick` to see whether MSVC
build time decreases significantly versus #640. On
major checkpoints and before declaring DOS fidelity or
shipping acceptance, select `full` and require all
164 CTests green. There is **no verified new timing**
yet; any predicted reduction is provisional.

### Faster local Windows builds

If a Visual Studio C++ toolchain is installed, after
the first CMake configure/build a developer can build
only the native EXE instead of the whole test suite:

```powershell
cmake -S DemoV1 -B build/demo-v1 -A x64
cmake --build build/demo-v1 --config Release --target ravenloft_demo_v1 --parallel 2
```

This is a local workflow, not a prebuilt downloadable
EXE and not proof of passing tests. It reuses the same
local MSBuild objects on successive builds.

The same distinction between **engineering coverage**,
**verified Windows tests**, and **measured original DOS
fidelity** must be reported in every future checkpoint.


## Inventory workstream — original class/race starters

[Exact CODE.1 family/race starter matrix](ORIGINAL_CODE1_ALL_CLASS_RACE_STARTING_EQUIPMENT_20261009.md):
the original CODE.1 15×6 source has **44 populated
and 46 unavailable entries**, all read dynamically
from the original executable. The native
CHARS builder now uses a single 21-slot
source-backed loadout model; test coverage
checks all 90 rows, both actors, starter special
container suppression, and the reference Halfling/
Gnome Cleric source bytes. The manual-only
workflow now registers **165 tests** (including
12 Quick targeted tests) and stages
`Audit-Starting-Equipment.cmd` plus its CLI.

This tightens **existing already-implemented**
deliverable 07's fidelity. It does NOT arbitrarily
award another whole-game percentage or claim
inventory visual parity. Separate equipment
interaction, descriptions, container UI,
stats, encumbrance and source graphics remain open.
