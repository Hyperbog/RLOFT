# Level 22 — reusable native development testbed

## Native Level22 opt-in diagnostic session (pending next manual build)

The source now includes [`--dev-trace` / `Diagnose-Level22.cmd`](LEVEL22_DEV_TRACE_CHECKPOINT_20261009.md): source-identity manifests, per-screen indexed 320×400 captures, palette bytes, state hashes, native Type1 sprite/fill/restore call ordering and destination coordinates, input and modal transitions, F12 marker/recent-event snapshots, existing RES3/SINTAB renderer traces, and a single automatically ZIP-packaged session. The original file tree remains read-only. The new `dev_session_trace_tests` brings the current authored registry to **179**, compared with **178 verified in Full build #648**. **Source is unbuilt; no Windows or DOS behavior acceptance claimed.**

## Current status and universal percentage tracking

**Green Full Windows checkpoint:** [build #648](
https://github.com/Hyperbog/RLOFT/actions/runs/37998467852)
compiled and passed all **178/178 registered CTests**
at source revision `fb0912346a831dad21d2960e99863cc8f446173a`. The
Windows executable was published and committed, and the
workflow restored **manual-only**.
[Exact EXE SHA-256/build provenance](FULL_WINDOWS_BUILD_CHECKPOINT_20261009_RUN648.md).
**No original DOS visual/timing/gameplay acceptance is claimed**.

**Post-build development:** the production runtime now
selects original RES4 ground textures using the
currently loaded level's signed header selector and
the original INIT resource base, instead of rejecting
all levels other than 22. A synthetic Level23 selector
fixture validates archive identity without fallback
artwork. This source-binding change and its regression **passed the Full #648 Windows native build**; original playable transition and DOS acceptance remain incomplete.

The canonical [percentage report](
PROGRESS_SUBSYSTEM_REPORT_20261009.md) tracks
**122 individual subsystems and 20 parent systems**
with original-source, implementation, runtime,
authored-test and DOS-equivalence gates.
**Provisional readiness: 49.8%.** New authenticated
cross-level ground selection is **50.0%**; Level
lifecycle and continuity is **47.9%**; Verification
and developer testbed is **50.0%**. The historical
**56.25%** 32-task engineering metric remains separate.

**Current source CTest registry:** 179 (one unbuilt trace regression); Quick: 19. Full Windows #648 compiled and passed **178/178**, including original Level22 pickup-message source and event-timing targets. This is not original-DOS parity or evidence of a completed playable Level22. Future builds are manual unless explicitly
authorized. An opted-in `--explore-level22 --dev-inspect
--dev-interact` run is available for human interaction
tests using the published executable.

**Branch:** `demo-v1`  
**Scope:** small source-faithful vertical slice; all proven runtime code is reused as later levels are integrated.  
**Architecture reference:** [continuous-game contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).

## Level22 completion gate before Level23

The current mandate is to make **all of original Level22 fully playable first**, using original mechanics and DOS comparison—not only to create a graphical development environment. The real Level22 finale may run through the original mist's 22 frames and its source-mandated frame-20 loader boundary, but **no independent Level23 gameplay development begins until the Level22 acceptance gate is closed**. The [Level22-original completion matrix](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md) records that rule. Original message assignments and `BF44` live state are now source-reconstructed but not yet drawn through the original message renderer.

**Current native original Level22 pickup message checkpoint:** a world item picked by the game's existing Level22 click route is now queued with source CODE.2 pointer-resolved prefix, original OBJECTS name and source IRQ deadline. This does **not** yet draw the banner into indexed VGA. [Pickup source/IRQ audit](LEVEL22_ORIGINAL_PICKUP_IRQ_RIBBON_20261009.md).

## Rules of this testbed

1. **Level 22 is a testing location, not a replacement game.** It runs through the existing `ExplorationRuntime` and original asset loaders used for actual progression, with the source-defined fresh Level 22 position (4.5,4.5), facing 270. No second inventory renderer, duplicate combat state, or hardcoded separate character system.
2. **Do not alter production startup.** `--explore-level22` remains explicitly opt-in. The ordinary executable still goes through original CODE.2 cinematics/menu and CODE.1 character creation or the original existing `CHARS` fast-entry path. Existing `CHARS` bytes take precedence in preview; if absent, the preview's two synthetic actors are **never written to CHARS**.
3. **Gameplay fidelity beats convenience.** Development helpers observe existing world, party and graphics data; they do not add invented items, triggers, respawn conditions, boss deaths, cinematic outcomes, or free rewards to source state.
4. **One working engine.** Inventory, combat, world events, spells, renderers and cinematics authored for the testbed are implemented in reusable production modules. Later levels are selected through original level resources and scripts, not separate game forks.
5. **No build during normal development.** Commit code/tests/docs continuously. Windows builds and any CI execution are user-authorized/manual, and new or unexecuted assertions do not imply passed tests.

## Source-backed developer interactions (new)

An additional **`--dev-interact`** option makes the small
Level22 preview useful for testing live item behavior.
Invoke it explicitly alongside `--explore-level22`:

```text
ravenloft_demo_v1.exe --explore-level22 --dev-inspect --dev-interact <original installation directory>
```

When enabled, the preview permits **only the original
existing production gameplay handlers** previously
blocked in the preview: clicking a loose Level22 world
item when the cursor is empty, and exchanging an item
on a character's hand tile with the pointer's
secondary click. Neither handler fakes a hit, monster
death, item spawning, identified status, quest event,
or saving. Existing original object, collision,
stale-frame and held-cursor guards still apply.

This is **not** the same as `--dev-inspect`.
The inspector continues to be read-only, and a
plain `--explore-level22` retains the earlier
noninteractive-preview restrictions. `--dev-interact`
is rejected unless the command also explicitly
requests `--explore-level22`. Normal startup,
character creation and CHARS gameplay retain their
preexisting permissions. This mode never writes
original CHARS or SAVE data. A source-backed live
world state may change in memory because the user
explicitly performs an ordinary gameplay action.

`DevLevelInteractionPolicy` supplies small C++17
constexpr permission predicates. Assertions in the
existing `dev_level_diagnostics_tests` check the
entire valid/invalid mode matrix, including that
normal gameplay remains enabled and a preview
without `--dev-interact` stays protected.

**Build checkpoint accepted:** Full Windows build
#647 passed all 172 CTests at `fb4a0145`, and
the original one-shot trigger has been removed.
The current HEAD has additional source-only
per-level ground binding changes. The earlier HEAD
contained the previously uncompiled in-game CINE08
runtime event handoff, multi-actor container UI,
state inspection and this new interactive preview.
Before extending integration or claiming demo
acceptance, the user should manually dispatch
the Windows [Demo v1 build workflow](
https://github.com/Hyperbog/RLOFT/actions/workflows/demo-v1-build.yml)
on branch **`demo-v1`** with **`build_mode=full`**;
keep `run_linux=false` unless explicitly wanted.
Full is necessary to compile/execute both new
developer diagnostics and the Python progress
validator; Quick's 19 focused tests exclude them.
Do not dispatch automatically. Any failure must be
addressed at its exact build SHA before advancing
new gameplay acceptance scores.

The canonical subsystem table now tracks **122**
individual subsystems. The added **opt-in world
pickup and hand exchange** subsystem has provisional
readiness **60.0%** (20/20/20/20/0, source,
implementation, live wiring, authored tests and
unexecuted DOS acceptance); the
**Verification and developer testbed** parent
is **49.3%** across seven rows. The weighted
overall now measures **48.9%** after rounding,
the prior 56.25% engineering-task metric is
unchanged, and the CTest registry remains
**172 / Quick 19**.

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
