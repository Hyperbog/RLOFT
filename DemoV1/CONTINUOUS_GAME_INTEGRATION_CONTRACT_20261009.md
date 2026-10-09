# Demo v1 → Complete Ravenloft: continuous-game integration contract


## Highest-priority project requirement — finish recreating the entire original game code

**[Original-Game Code Reconstruction Charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) governs this project.** The current goal is to **recreate all code needed to run the complete original Ravenloft game faithfully**: original startup, character creation, mechanics (including bugs and quirks), data/graphics/audio, timing, story, cinematics, gameplay transitions, endgame and SAVE/LOAD. This is the entire current development objective, **not a preliminary Phase I** or a completed research-only stage. Reconstructed native code is the implementation being built and compared against original DOS behavior; literal re-execution of each original DOS instruction is not required.

**Only once the full original-game code recreation is finished** will a modern port even be considered, on a **separate future branch**. Until then, do not implement intentional bug fixes, changed gameplay, new features, modding-facing game changes or QoL redesign. Preserve original assets unchanged. **Commit every checkpoint; builds require manual user authorization.**

**Build verified:** [Full Windows #648](https://github.com/Hyperbog/RLOFT/actions/runs/37998467852) passed **178/178 CTests** at checkout `fb091234`. Native EXE SHA-256 `b2d868b35f9e283c24746aa96b1aa94d671377be81008b1433277ef359468f16`. The manual-only workflow was restored and the one-time trigger removed. [Full provenance](FULL_WINDOWS_BUILD_CHECKPOINT_20261009_RUN648.md). **DOS acceptance stays 0%.**

## Level22 user-assisted forensic diagnostics

The [opt-in developer flight recorder](LEVEL22_DEV_TRACE_CHECKPOINT_20261009.md) is source-authored: `--dev-trace` and `Diagnose-Level22.cmd` capture real UI/Win32 input, exact original-resolution indexed native pixels/palette, per-domain state hashes, native modal Type1 sprite/fill/restore command coordinates and code2 source-derived raster TSV, with F12 markers and a ZIP on exit. It does **not** alter the actual game or create new game mechanics. This addresses the user-observed broken inventory/portrait placement and incomplete automap while preserving the independent original-DOS comparison requirement. **179 authored CTests, one beyond the last 178/178 Windows Full build; no new build authorized or dispatched.**

## Explicit Level22-first delivery gate

**Complete the full original Level22 as a playable and verified single slice before any independent Level23 gameplay, AI, asset or story implementation.** The Level22 ending is allowed to include its real 22-frame mist effect and the original `181B4(23)` handoff after frame index 20; that obligatory source instruction does not mean the project has begun Level23 gameplay work. Prioritize original combat, interaction, inventory, HUD/menu, original pickup-message codes, exact Assassin and relic flow, and the entire original mist sequence. Keep all work in the same original executable integration path. [Level22 gate and original BF44 source audit](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md).

## Mandatory system and subsystem percentage reporting

Every ongoing native-development checkpoint maintains
independent percentages for **20 major systems and
122 named subsystems** (expanding the list when a real
new subsystem is found). The [canonical scorecard](
PROGRESS_SUBSYSTEM_REPORT_20261009.md) and its
[CSV data](PROGRESS_SUBSYSTEMS.csv) distinguish
original source proof, native code, live runtime,
authored regression and current-head DOS acceptance.
These evidence-weighted estimates are **not**
percentages of total developer effort.

**Current baseline: 49.8% weighted readiness.**
Level lifecycle and continuity is **47.9%** across
seven subcomponents. The newly generalized authentic
per-level RES4 ground selector is **50.0%** with a
source-authenticated Level23-like archive selection
test, but **no live next-level traversal yet**.
The original 32-task engineering ledger remains
**56.25% (18/32)**, independent of this score.

**Last verified Windows build:** [Full #647](
https://github.com/Hyperbog/RLOFT/actions/runs/37987825463)
compiled and passed **172 of 172 CTests** at source
commit `fb4a0145`; artifact and provenance published.
The post-build Level23 terrain-binding change and
associated test have **not yet been rebuilt**.
Original DOS acceptance stays **0%** at current
source revision. The temporary push trigger was
removed; CI/builds are again **manual dispatch
only**, except when newly authorized by the user.

`--dev-inspect` remains read-only and
`--dev-interact` explicitly opts into normal
production item interactions inside the Level22
testbed. Neither bypasses the original production
startup or CODE.1 character creation. Live source-event-driven cross-level transitions, complete retained-world runtime behavior, full SAVE/LOAD and DOS visual/IRQ acceptance remain open engineering milestones.

**Part 9 update:** The source now contains in-memory visited-world snapshots and `ExplorationRuntime::transition_to_level` for event-provided destinations. It is not yet called by an authored original exit; current-source compilation, original DOS acceptance and complete disk-world persistence remain outstanding. Registered CTests: 178.

**Subsequent original-source work:** The CODE.2 `21F70..223F0` camera-stage and 22-frame Level22 mist order have been reconstructed as typed source traces; source archive confirms the original Level23 64×64 destination (9.5,58.5), facing 180. These models and the direct-item F540 fidelity repair are **not built/accepted**. [Mist timeline](LEVEL22_ORIGINAL_MIST_FRAME_SEQUENCE_20261009.md) · [Camera staging](LEVEL22_ORIGINAL_MIST_STAGING_20261009.md) · [Level23 archive audit](ORIGINAL_LEVEL23_DESTINATION_AUDIT_20261009.md).

The original `1CD48` effect dispatch event-history check has been decoded and recreated for original-mist event ID 2 (persistent flag `0x82`). This is a **source-only gate** pending the actual effect interpreter. [Event history audit](ORIGINAL_NUMBERED_EFFECT_1CD48_AUDIT_20261009.md).

The runtime also now tracks original CODE.2 BF44 from Level22 Assassin death (literal 1) and conditional pickup-message writes, and exposes the value without fabricating graphics. **178 CTests registered, not built.** [BF44 integration](LEVEL22_BF44_LIVE_GLOBAL_20261009.md).

The same Level22 pickup path now queues original `22510..225DF` pickup strings via CODE.2 `D67C` prefix and source `OBJECTS.DAT` names with IRQ-tick-based `1C21C` scheduling. It uses the existing game click/clock path and does **not** invent a text toast. The exact original VGA ribbon draw/expiry is still pending. [Original source audit](LEVEL22_ORIGINAL_PICKUP_IRQ_RIBBON_20261009.md).

## Opt-in reusable development slice (current implementation)

Level22 is the first integrated testbed. An explicit
`--explore-level22 --dev-inspect` invocation now
activates **read-only**, five-domain state diagnostics
(camera, raw party, inventory/containers, mutable
world, story flags) in the Win32 window title.
The diagnostics do **not** manufacture gameplay,
save CHARS, change original VGA pixels or bypass
real character creation in the normal startup.
They are intended to be reused during subsequent
level transition testing to detect accidental
party/inventory/quest resets without changing
the underlying authoritative session.

Source: [Level22 dev-level testing
matrix](DEV_LEVEL_TESTBED_20261009.md).
Currently registered CTests: **178** / Quick **19**;
Full build #647 verified the earlier 172 CTests at `fb4a0145`.
The later cross-level ground change is source-only.
Engineering score **56.25%** and inventory
PARTIAL remain unchanged.


**Established:** 2026-10-09  
**Target:** faithful, complete *Ravenloft: Strahd's Possession* gameplay—not a self-contained Level 22 demo or a collection of disconnected level executables.  
**Branch:** `demo-v1`. This is a forward-looking integration/acceptance contract. It does **not** claim all transitions are already implemented or play-tested.

## The required player-facing flow

The complete native game must preserve the **original source-defined sequence** for each starting condition:

- **No existing runtime `CHARS`:** original CODE.2 cold-start presentation and cinematics (`CINE00.ANM`, `CINE12.ANM`, including original skip/return behavior) → startup menu → original CODE.1 character creation → original binary character-transfer image → CODE.2 gameplay entry → the source-defined first level (currently Level 22) → every further level transition, event, and ending in original game order.
- **Valid existing `CHARS`:** follow the original CODE.2 CHARS-present startup path into the correct saved/fresh gameplay entry, without replaying incompatible new-game character creation or manufacturing a different party.
- **An in-game story cinematic (for example `CINE08.ANM`):** suspend the *same* live exploration session → play the original numbered event via the shared ANM/audio/presentation engine → restore that session with the original event effects and level/party state. **Do not** restart cold startup, re-create characters, or replace the party with a demo test fixture.
- **Level changes:** leave the current level through its original CODE.2 event/transition → load the destination level and its authored assets/world state into the *same* game session → continue with the same living characters, inventory, spells, persistent story flags, timing state, and world continuity as specified by the original files.

The `--explore-level22` entry is a **developer-only preview/test shortcut**, not the intended release entry or a parallel replacement for full startup.

## Verified current source architecture

The current source already provides real shared pathways, not an isolated mockup:

| Existing source | Verified responsibility | Still to complete |
| --- | --- | --- |
| `DemoV1/src/win32_main.cpp` / `BootFlow` | CODE.2 boot control, cold-start intro `CINE00/CINE12`, menu handoff to CODE.1 character workflow, or CHARS-present exploration entry | Whole-title end-to-end native/manual acceptance |
| `CharacterCreationFlow` / `build_character_transfer` | Original character creation and binary CODE.1→CODE.2 party image | Full source-equivalent acceptance against original |
| `run_exploration()` | **One common** exploration entry for preview, valid existing CHARS, and a newly created character image; the ANM/audio/MIDI players are now shared with Level16 event 8 | Persist runtime across *later* level changes rather than re-entering this fresh-start function |
| `ExplorationRuntime::load_fresh()` / `initialize_gameplay_state()` | Original `CHARS`-derived fresh party, renderer tables, live Level 22 world, inventory, events, resources | General `transition_to_level` / exact CODE.2 level lifecycle without recreating global state |
| `InventoryRuntimeState`, world, party and SAVE codecs | Source-backed item/equipment/spell/container state and original save-byte projections | Whole-world save/load/reload, original transition-side effects and persistent state roundtrips |
| `AnmCinematic` / `play_cinematic()` | Original intro and in-game numbered event resource playback; CINE08 connected as source-order Level16 quest event | Exact DOS timing, interruption, palette/audio teardown and event-specific side effects |

This table distinguishes **implemented source wiring** from **accepted behavior**. Present test counts and completed source-level checkpoints cannot substitute for Windows compilation, CTests or DOS behavioral comparison.

## Non-negotiable integration invariants

1. **Single authoritative game/session state.** Keep the canonical party actor bytes (4×0x248), known-item flags, held cursor, equipment, Mage spell matrix, persistent story flags, status/conditions, experience, position, clocks, RNG, per-level world changes and quest state. Do not fork a private party or mock game state into a level module.
2. **Source-defined transitions.** New level work must be callable from a gameplay event/exit in the preceding level; it is not enough to provide a test-only `--levelXX` entry. The source files determine which cinematic/transition/position/facing/time occurs before and after loading.
3. **Per-level data as content, not separate implementations.** Build a reusable original `LEVELS.DAT`, `LEVxxx.DAT`, object/resource/NPC/animation loader and renderer whose level-specific content and behavior are selected by the original level identity. Keep special-event handlers bounded and source-traced rather than hard-coding the full game around Level 22.
4. **No party reset across cinematics or level entry.** Hand off control to one shared cinematic system, then restore the existing gameplay session, input, palette, audio, clocks, pending effects and scene cache according to verified CODE.2 semantics.
5. **Persistent-world fidelity.** In-game transitions and SAVE/LOAD must carry world NPC state, loose items, traps, event flags, dynamic control records, timed effects, identified equipment, nested containers, and the original saved globals. An inventory-only SAVE projection is not sufficient as a full game save.
6. **Full regression chain.** For every newly implemented level, add source-backed test cases for entering from the preceding game state, maintaining the exact party, executing local events, transferring to another level and saving/loading the resulting state. Preserve coverage for starting with and without CHARS and for skipped/completed cinematics.
7. **Provenance over convenience.** All level/event behavior must be traceable to original CODE.1/CODE.2, DAT and RES/ANM sources. Source quirks (e.g. 6FA0 four actor records and post-close sentinel) remain preserved until disproven by original runtime evidence. No fabricated handoff dialogue, teleport, reward, or animation.
8. **Committing and building are distinct.** Commit and push every implementation, test, fix and continuity checkpoint to GitHub. Windows builds/CI workflow dispatch remain **manual/user-authorized**. Lack of a build does not excuse uncommitted source, and uncompiled code must not be reported as formally accepted.

## Per-level completion gates

A level is not "finished" merely because it renders. Acceptance requires its original entrance and preceding transition, asset selection, movement/collisions, interactive objects, inventory uses, NPC state, spell/combat consequences, scripts/story events, cinematics, exits to the next level, and return/re-entry behavior, plus persistent state and source-vs-native presentation tests. Validate cross-level party/state preservation as a separate gate.

**Integration sequence:** complete and validate the current Level 22 vertical slice with the original cold-start/character-creation transfer intact; generalize the source-defined level lifecycle and state preservation; implement successive levels with their original transitions; run a final original start-to-finish playthrough and SAVE/LOAD roundtrip audit. Level numbers are not assumed to be gameplay traversal order: **CODE.2's level/event graph is authoritative**.

**Current status at writing:** source connections from both startup routes to shared exploration exist, and source-derived CINE08 playback is wired. General in-session level transitions, whole-world persistence and full original runtime acceptance **remain unfinished**. Overall tracker last reported **56.25%** with inventory **PARTIAL**. Windows build remains deferred until the user requests it.
