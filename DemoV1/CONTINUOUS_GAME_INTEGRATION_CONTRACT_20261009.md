# Demo v1 → Complete Ravenloft: continuous-game integration contract


## Mandatory system and subsystem percentage reporting

Every ongoing native-development checkpoint must maintain a
percentage **for every one of the 20 major systems and all
120 named subsystems** (expand the list as new subsystems are
discovered). The [canonical progress scorecard](
PROGRESS_SUBSYSTEM_REPORT_20261009.md) records source tracing,
implementation, runtime integration, authored regression and
DOS-current-revision acceptance using a fixed evidence-weighted
score. The [scoring CSV](PROGRESS_SUBSYSTEMS.csv) is the
canonical input, cross-checked by the
`subsystem_progress_tracker_tests` CTest.

Current baseline **48.8% provisional readiness**, distinct from
the historical **56.25% (18/32)** task score. DOS acceptance is
currently **0% at this revision** because no current-head
build or DOS comparison has been run. Source-only changes
cannot promote that acceptance score. Test registrations
now **172** (Quick 19); CI/build execution remains
user-authorized and manual.


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
Registered CTests now **171** / Quick **19**,
but the new source has not been built or run.
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
