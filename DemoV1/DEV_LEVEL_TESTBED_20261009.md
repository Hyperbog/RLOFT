# Level 22 — reusable native development testbed

## Universal percentage tracking

All work on this dev slice, including original startup,
character creation, inventory, movement, pixels, audio,
puzzles, combat and later-level handoffs, is tracked
through **120 individually scored subsystems** in
20 major groups. The canonical percentage table and
per-subsystem blockers are maintained in
[the progress scorecard](PROGRESS_SUBSYSTEM_REPORT_20261009.md).
Provisional readiness at adoption **48.8%**;
the older 56.25% engineering ledger remains
independent. On every source checkpoint, update
the applicable subsystem percentage and its
roll-up, without treating unexecuted tests as passes.

CTest registration **172** (including the offline
scorecard consistency check), Quick **19**.
No build, CI or test was initiated.


**Branch:** `demo-v1`  
**Scope:** a small, source-faithful vertical slice used to exercise each original game subsystem before the same runtime is expanded to the other levels.  
**Status:** development instrumentation added; Windows build and 171 registered CTests **not yet executed**.  
**Architecture reference:** [complete-game integration contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).

## Rules of this testbed

1. **Level 22 is a testing location, not a replacement game.** It runs through the existing `ExplorationRuntime` and original asset loaders used for actual progression, with the source-defined fresh Level 22 position (4.5,4.5), facing 270. No second inventory renderer, duplicate combat state, or hardcoded separate character system.
2. **Do not alter production startup.** `--explore-level22` remains explicitly opt-in. The ordinary executable still goes through original CODE.2 cinematics/menu and CODE.1 character creation or the original existing `CHARS` fast-entry path. Existing `CHARS` bytes take precedence in preview; if absent, the preview's two synthetic actors are **never written to CHARS**.
3. **Gameplay fidelity beats convenience.** Development helpers observe existing world, party and graphics data; they do not add invented items, triggers, respawn conditions, boss deaths, cinematic outcomes, or free rewards to source state.
4. **One working engine.** Inventory, combat, world events, spells, renderers and cinematics authored for the testbed are implemented in reusable production modules. Later levels are selected through original level resources and scripts, not separate game forks.
5. **No build during normal development.** Commit code/tests/docs continuously. Windows builds and any CI execution are user-authorized/manual, and new or unexecuted assertions do not imply passed tests.

## Opt-in developer inspection

Use an original legal installation with:

```text
ravenloft_demo_v1.exe --explore-level22 --dev-inspect <original installation directory>
```

`--explore-level22` is the preexisting source-backed dev walkaround, while the **new** `--dev-inspect` switch enables additional read-only state instrumentation. It is **rejected unless the explicit preview flag is also present**, so production startup/character creation cannot silently acquire developer instrumentation.

The inspector does **not** change the indexed 320×400 DOS framebuffer, apply synthetic source data, change character bytes, save files, or alter game loop timing. It extends only the Win32 **window caption** already used by the development preview.

The diagnostic caption now reports:

| Metric | Observation |
| --- | --- |
| Active actors | Count from original actor status words |
| NPCs / placed objects / loose items | Counts from the live LevelWorldState |
| Open containers | Phase-3 records from all four actor `6FA0` contexts |
| Persistent story bits | Number of set bits in the original 32-byte event bank |
| Party digest | Deterministic FNV-1a over the **exact** original four 0x248-byte party records |
| Inventory digest | Cursor ID/known byte, all 63 in-memory descriptor metadata/items, four original 6FA0 actor records, optional current click selection |
| World digest | Original cell words, group1/group2 10-byte control records, active-animation count, live NPCs, placed and loose family-C objects, including Q16 coordinates, flags, types and HP |
| Story digest | All 32 original event bytes, separate from party and world |
| State changes | Read-only, monotonically increasing observation counter when at least one tracked domain changes between displayed frames |

Camera/clock have a **separate 64-bit digest** for regression comparisons; the original preview caption already shows level, cell and facing, so its camera digest is not duplicated in the visible title. Each displayed Party/Inv/World/Story hex digest shows the low eight hexadecimal digits of its full **64-bit** underlying digest.

**Important:** These are deterministic debugging fingerprints, **not original DOS checksums**, not cryptographic authentication and not a complete SAVE.DAT encoder. A digest collision remains possible. They are designed for comparing state snapshots across actions, detecting unintended cross-subsystem mutations and verifying that subsequent levels preserve party, inventory and story bytes. The level header and static assets are deliberately excluded from the mutable-world digest. Mage scroll *source metadata* and presentation feedback are outside the inventory mutation digest.

## Suggested developer acceptance matrix

This is a **test plan**, not a declaration that each category passes.

| Reusable game subsystem | Validate inside the Level 22 slice | Future level expansion |
| --- | --- | --- |
| Original boot and party transfer | Enter via regular cold startup and real CODE.1 party; compare with preview and existing-CHARS route | No party reset on any level transition |
| World and movement | Source Q16 movement, turning, grid/wall collision, world hit geometry, door/control state | Load original exits, portals, vertical transitions |
| DOS rendering | Source RES/SINTAB/VGA palette/texture and animated actor overlays | Same graphics pipeline and level-specific textures |
| Party/inventory | Cursor and known bits, 21 original slots, equipment restrictions, nested containers, party reordering | Same party/inventory bytes across transitions |
| Spells | Scribing, Mage matrix, charges, spell use and status effects | No skill resets; original authored level/script spell effects |
| Combat | Hit/damage/stat/class/XP changes, NPC injury/death, dropped items, IRQ feedback | Different monsters/AI from the same source catalogs |
| Objects and puzzles | Placement, pickup, special interactions, authored flags and chained events | Script/event identifiers from each original level |
| Cinematics and audio | ANM packet sequences, sampled sound, XMIDI, interrupted and completed events | Original story events including CINE08 with shared live session |
| UI | Original startup/menu/character/party/map/quit/input-modal behaviors | No level-specific UI rewrite |
| Persistence | Event bit bank, item/container raw records, world NPCs, loose objects, clocks and moving effects | Exact multi-level original save/load plus re-entry |
| Transition lifecycle | Leave Level 22 by an authenticated original event without rebuilding globals | Every new level uses the same session and resource loader |
| DOS acceptance | Source-defined framebuffer/cursor/IRQ chronology compared with DOSBox original | Replay representative transitions and full original campaign |

## New reusable diagnostics API

`DevLevelDiagnostics::capture(const ExplorationSessionState&, const LevelWorldState&, const InventoryRuntimeState&)` returns a five-domain snapshot with per-domain 64-bit fingerprints. `compare(before, after)` returns five independent change flags: camera, party, inventory, world and story flags. Both functions are **read-only**; source data is hashed explicitly in little-endian byte order rather than through host-dependent C++ structure padding or `size_t` layouts.

The new `dev_level_diagnostics_tests` CTest exercises unchanged deterministic snapshots, one-domain changes, original CODE.1 party payload changes, source container phase3 records, known-bit and cursor mutations, event-bit `0x44`, NPC health, family-C loose Q16 objects, cell words, group1/group2 level control records, and active animation count. It verifies restoration to the same snapshot after reversing changes, and 8-digit hex display formatting.

**Registered regression targets are now 171 CTests**, increased by one dedicated diagnostic test. The existing manual Quick selection remains 19. The source checkpoint is committed, but not compiled, executed or accepted on Windows or DOS.

## What remains before expanding to other levels

Level 22's currently reconstructed inventory and Level16 cinematic work still has outstanding DOS fidelity, original save/load and action-category gaps; diagnostics do **not** close them. The developer slice should first prove original input→state→render→event behavior without source shortcuts and then validate handoffs from the real startup/character flow. Next build a **source-defined general level-transition lifecycle** with persistent party/world state rather than calling fresh gameplay initialization after each change. Only after that should later level content be added through the existing loaders.

**Formal engineering score remains 56.25% and inventory remains PARTIAL;** a developer tool does not independently advance a DOS fidelity acceptance criterion.
