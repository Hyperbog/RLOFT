# Ravenloft Demo Build v1


## 2026-10-10 actor-modal mode prefix — source verified, Windows unbuilt

Original CODE.2 `11AD8..11B68` is now an isolated native callback adapter with source-ordered `291B0`, 1-or-4 `11268` actor repaint calls and one deferred `11B63→F790` call. It preserves the full selected `6F90` DWORD on the flag-bit-2 four-actor route; normal mode toggles only one actor's low BYTE. The original binary check now proves **16 instruction windows and 11 near-call targets**, including the `12F94` return that prevents an actor-toggle bit-2 event from falling into the generic `1307C` modal cleanup. Standalone strict GCC/Clang native regression passed; CMake registry **212 tests**, while the last authorized Windows Full #654 remains **210/210 PASS on an earlier revision**. The actual DOS flag/button mapping, post-F790 `11B6B..11C19` actions, live Win32 drawing, RNG/IRQ and DOS acceptance are still blocked and **not** claimed implemented. No build dispatched. [Detailed checkpoint](LEVEL22_F790_ACTOR_TOGGLE_NATIVE_PREFIX_20261010.md).


## Repository-first instructions for every development continuation

**Mandatory before implementing or committing gameplay code:** read the [original-source pre-commit verification charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md#mandatory-source-verification-gate--applies-before-every-gameplay-code-commit) and [native continuity/pre-commit protocol](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md#binding-pre-commit-reconstruction-protocol--read-for-every-native-code-batch). Use `Code1/Code1.cpp` and `Code2/Code2.cpp` as the **primary reconstructed implementation references**, then independently confirm all changed gameplay behaviors against original version-identified CODE and other game-file bytes. **No speculative original-game functionality is allowed into Git.** Record callsite/offset proof, dependencies and source-derived tests before committing, then track Windows build and DOS acceptance separately. The continuity documentation in Git is authoritative across conversations. Build and CI runs remain manually user-authorized. Prioritize full original Level22 playability through its mist ending before independent Level23 work; defer all modernization until original full-game recreation is complete.

## 2026-10-10 original RES2 archive-slot correction — source verified, unbuilt

The first manual **Build #651** diagnostic upload has **313 events, 96 indexed frames, 3 F12 markers and 48 renderer traces**; encumbrance opens without the former fatal error, but the party modal still shows movement/viewport art in the wrong actor columns. A full original `CODE.2` byte/hash audit and six exact instruction-window matches uncovered the root cause: source `2E7F0→2F29C` indexes the numbered **RES2** archive directly for all these actor chrome/background/inventory/equipment `slot=2` calls. The native presenter incorrectly used **RES0[INIT[0] + item]**. All affected native drawing wrappers now resolve **RES2[item]** exactly, without offsets; complete-versus-selected frame semantics remain unchanged. The tests cross-check **10 exact original instruction signatures** and twelve source-used RES2 IDs. Native-only diagnostic draw audit capacity is now **4,096** (was 1,024). [Source/trace evidence, original offsets and verification gates](LEVEL22_CODE2_RES2_BANK_ROUTING_FIELD_AUDIT_20261010.md). **Unbuilt since Full #651 (182/182); Windows UI and DOS parity pending**. No automatic build or Level23 work.

## 2026-10-10 Full Windows build #651 verified — 182/182 tests

[Full #651](https://github.com/Hyperbog/RLOFT/actions/runs/38026056424) compiled and passed **all 182 CTests**, including original `RES2` loading, source-correct full-frame modal blits, `CODE.2` native contract and diagnostic-recorder regressions. Published artifact: `Ravenloft-Demo-v1-Windows-x64` with `Diagnose-Level22.cmd`; EXE SHA-256 `f622113e0813cd16826b8a2e92fc40ae31f7ea6358780191d822459738c09786`. [Full reproducibility, source SHA, failure/retry explanation and first user test instructions](FULL_WINDOWS_BUILD_CHECKPOINT_20261010_RUN651.md). **The fixed Level22 UI still awaits user interaction/diagnostic ZIP and original-DOS comparison.** No completion-percentage adjustment or independent Level23 work. Workflow restored **manual-only**.

## Mandatory objective: fully recreate the code to run the original game

The [Original-Game Code Reconstruction Charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) is binding. **Complete the full code recreation necessary to execute the entire original game faithfully** before considering anything modern. This encompasses every original subsystem, startup/character creation, level, encounter, mechanic, event, cinematic, ending and SAVE/LOAD operation, including the original bugs and quirks. The native code being built is this reconstruction—not merely disposable research and not a preliminary Phase I. Check its behavior against the original DOS game; it need not literally run the original DOS instruction stream. **Only after the complete original game recreation is done** may a separate modern branch be considered for changes, fixes, features, modding or QoL. No modernization is authorized yet.


## Level22 RES2 modal artwork and actor-major draw order — verified source, unbuilt

A renewed canonical `Code2.cpp` audit and four original CODE.2 binary instruction windows confirmed **three omitted whole-resource drawing calls**: encumbrance `RES2:0x25` at row `0x73`, container transition at `0xC4`, and opened-container art at `0xCE`. These now all use the original `2E510→2E4A4` complete-descriptor renderer, not frame zero. The encumbrance burden label now uses original `lane+6, span=0x3C`; `112EC→11268→1118C→10BBC` actor iteration/outline ordering is reflected in native per-actor container presentation. A new offline canonical-to-native contract regression includes optional strict original-file hash and four original instruction-signature checks. [original CODE.2 modal/RES2, text-geometry and actor-order audit](LEVEL22_CODE2_MODAL_RES2_SOURCE_ORDER_AUDIT_20261010.md). **182 registered CTests, source only**; last green Windows Full #649 remains **179/179**. DOS visual/interaction acceptance remains outstanding; no build authorized.

## Level22 inventory backdrop original-source correction — unbuilt

[Original CODE.2 2E510 full-descriptor blit audit and fix](LEVEL22_SOURCE_MODAL_FULLFRAME_REPAIR_20261009.md): a direct object-1 disassembly proves the original `2E510` iterates **all resource frames**, advancing by the image's per-frame height; `2975C` draws only **one chosen frame**. The #649 field diagnostic shows the native party modal incorrectly drew frame 0 of source `200×(320×1)` backdrops as a single horizontal line. The active/inactive actor bases, inventory grid and body/equipment background now use the faithful source whole-resource operation; status badges remain one-frame. A regression exercises scanline extent, clipping and call order. **181 registered CTests, not yet built**; the last green Full build remains #649 (179/179). This source fix does not certify DOS pixels until the user retests.

## Uploaded Level22 diagnostic session — crash and graphics evidence

The user uploaded the Build #649 `Diagnose-Level22.cmd` ZIP. [Full machine-readable forensic findings](LEVEL22_DIAGNOSTIC_SESSION_1791588336634_AUDIT_20261009.md): **125 event records, 52 indexed captures, 4 F12 markers and 16 environmental render traces**. The final click at original VGA **(68,16)** lies exactly within the first actor's encumbrance hit rectangle, proving it reached the original RES2:0x25 request missing from #649. The new native source catches/logs modal-render failures at their point of origin. The inventory draw TSV also proves several incorrect/unfinished **200×(320×1)** source sprite presentation calls. The automap contains a real explored-cell shape; do not assume it is wholly unrendered. No user character/save data was committed; no new build triggered.

## Level22 original RES2 encumbrance crash fix — source only

[Build #649 click-crash root cause and fix](LEVEL22_RES2_ENCUMBRANCE_CRASH_20261009.md): the original `PartyModalPresenter` correctly requests **RES2:0x25** for encumbrance and other RES2 entries for container art, but the asset-bank router omitted RES2 entirely. The shared `RavenloftBootAssets::type1(2,index)` now lazily decodes the real original `RES2` archive through the existing source reader, with a synthetic original-format regression. **180 CTests now registered; this fix has NOT been rebuilt**. The latest green Full Windows build is still #649 with 179/179 tests. The crash was not caused by an outdated EXE or absent game media. Diagnostic session ZIPs (never automatically uploaded) are under `%TEMP%\\RLOFT-Demo-Diagnostics`.

## Level22 diagnostic flight recorder — Windows Full #649 verified

A new opt-in **`--dev-trace`** mode and **`Diagnose-Level22.cmd`** launcher record source-referenced renderer captures, original 320×400 indexed frames from **inventory, automap, viewport, top menus and quit confirmation**, input/UI events, state hashes, a source-ordered **per-draw Type1 sprite/fill/restore operation TSV** for diagnosing overlapping party UI, and an **F12 defect-marker screenshot plus the 256 preceding event records**. The launcher waits for the game to exit and packages the bounded session as **one ZIP** in `%TEMP%\\RLOFT-Demo-Diagnostics`. The gameplay state and original files are untouched. [Usage, format, bounds and status](LEVEL22_DEV_TRACE_CHECKPOINT_20261009.md). **Full Windows build #649 passed 179/179 CTests**, including `dev_session_trace_tests`; the resulting Windows artifact includes `Diagnose-Level22.cmd`. [Exact binary and test provenance](FULL_WINDOWS_BUILD_CHECKPOINT_20261009_RUN649.md). **Manual recorder ZIP testing and original-DOS equivalence remain outstanding.**

## Current verified build and continuing game-engine checkpoint

**Full Windows build #649 passed 179/179 registered CTests** at source commit `6241a807baf53449256e5517687430283fdf2ca3` on 2026-10-09. The Windows executable and the new forensic launcher `Diagnose-Level22.cmd` are included in the [successful build artifact](https://github.com/Hyperbog/RLOFT/actions/runs/38003217534). [Build provenance and SHA-256 audit](FULL_WINDOWS_BUILD_CHECKPOINT_20261009_RUN649.md). The old #648 record remains for historical comparison.

The one-time marker was removed and the workflow restored **byte-for-byte to manual-only**. Full native CTest acceptance is **179/179**, but **Level22 visual/UI trace validation, full original gameplay, and original DOS parity are not yet accepted**.

## Required completion scoreboard — every engine system and subsystem

The canonical [full percentage scorecard](
PROGRESS_SUBSYSTEM_REPORT_20261009.md) now tracks
**20 major systems and 122 independently scored subsystems**,
each with original-source, native-implementation,
live-wiring, authored-test and current-DOS-acceptance
evidence plus the next blocker.

**Current provisional weighted readiness: 49.8%.**
The 40%-weighted original-DOS acceptance gate is still
**0% at the current revision**. Full Windows CTest
success validates native consistency, **not** original
DOS pixels, timing, action or story equivalence.
The separate historical 32-task engineering ledger
remains **56.25% (18/32)** and must not be averaged
with the readiness estimate.

Future functionality changes update the [canonical
per-subsystem CSV](PROGRESS_SUBSYSTEMS.csv), its
20 parent percentages and global weighted roll-up
in the same Git checkpoint. A registered offline
`subsystem_progress_tracker_tests` check validates
the authored report when Python3 is available at
configure time. The current source registry has **179 CTests** (one unbuilt new diagnostic test). and the 19-test Quick selection is unchanged. All 178 registered tests passed in Full Windows build #648; original-DOS equivalence remains unaccepted.
Builds remain manual unless explicitly authorized.

## Level22-first acceptance rule — complete playability before Level23

**Level22 must be fully playable and faithful before independent Level23 game development.** The original mist sequence and its necessary Level23 load call at frame index 20 are treated as the END of Level22, not a new Level23 workstream. Finish original gameplay combat/use, Assassin defeat and drops, four relics, menus/UI, pickup/source feedback, original effect and 22-frame mist, and current-head DOS parity first. See the [binding Level22 gate and pickup audit](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md).

**Part 9 source checkpoint:** original CODE.2 `21704` conditional `BF44` pickup messages have been reconstructed, and the authoritative runtime now retains original BF44 from Assassin death through pickups. The renderer for original `1C21C` text/ribbon is still incomplete. [Original message global audit](LEVEL22_BF44_LIVE_GLOBAL_20261009.md). **178 CTests authored/registered, not yet built**; old verified Windows Full build #647 remains 172/172 at older source.

## Level22 original picked-item text and IRQ scheduling

[Original pickup ribbon source audit](LEVEL22_ORIGINAL_PICKUP_IRQ_RIBBON_20261009.md): CODE.2 `22510..225DF` now composes the real item pickup string from the **original D67C resource pointer** and `OBJECTS.DAT` name, and the native Level22 click queues the original `1C21C` deadline `IRQ30700+0x5A`. The original four relics need no unresolved enchantment suffix. **Actual original VGA text presentation/expiry remains unwired**, so this is not yet a visible completed ribbon. Current CTest registration: **178**; last built/verified 172 at older source.

## Part 9 — original Level22 mist code reconstruction

The [original source-order mist sequence](LEVEL22_ORIGINAL_MIST_FRAME_SEQUENCE_20261009.md) recreates the exact **22-frame command order** and Level23 load after frame index 20; the [original camera staging model](LEVEL22_ORIGINAL_MIST_STAGING_20261009.md) reconstructs 10-degree turns and whole-cell steps before the effect. A direct [original Level23 archive audit](ORIGINAL_LEVEL23_DESTINATION_AUDIT_20261009.md) confirms 64×64 geometry and the source-authenticated (9.5,58.5), facing 180 destination. The [F540 item-removal correction](LEVEL22_F540_DUPLICATE_ITEM_REMOVAL_AUDIT_20261009.md) preserves original direct-item duplicate scanning. These are **unbuilt source/test changes**, not a fabricated working cinematic.

The [original numbered effect `1CD48` gate](ORIGINAL_NUMBERED_EFFECT_1CD48_AUDIT_20261009.md) also now reconstructs the original **event 2 → persistent bit `0x82`** history semantics. Event-history code is not a substitute for original effect rendering or live event timing.

## Part 9 — reusable level state and in-session engine bridge

[Continuity checkpoint](LEVEL_TRANSITION_STATE_CHECKPOINT_20261009.md): Guarded `ExplorationRuntime::transition_to_level` and an in-memory visited-level store preserve original party, inventory, event flags, time, NPC and object mutations, and automap. **No source event exit invokes this yet, and full disk SAVE/LOAD remains open.** Builds are manual.

## Developer Level22 testbed — read-only cross-system instrumentation

The current **small slice** is an opt-in testbed for
the future full original game, not a separate release
engine. The existing `--explore-level22` preview
now accepts **`--dev-inspect`** to show read-only
party, inventory, world and story-flag counts and
stable diagnostic digests in the **Win32 window
caption**, alongside existing live render coverage
and coordinates. It never changes CHARS, the
original 320×400 framebuffer, item placements,
original event flags, or gameplay timers. The
flag is **invalid outside the explicit preview
path**, so the real startup and character creation
remain unaffected.

The reusable `DevLevelDiagnostics` API fingerprints
five separate domains (camera/clock, party raw
bytes, inventory/6FA0 containers, mutable level
world and story flags). Existing snapshot data
can be compared to detect accidental state
resets during combat, puzzle actions, cinematic
handoffs or future cross-level transitions.
These hashes are **diagnostics**, not original
game SAVE checksums.

A new `dev_level_diagnostics_tests` target
covers independent mutations, source level
event/control records, NPC HP, item coordinates,
inventory/cursor state, story flag 0x44,
and returning to an identical snapshot.
**172 CTests were registered and all passed in build
#647 at `fb4a0145`.** Subsequent per-level terrain
source changes require a future manual build.
The historical engineering score remains
**56.25%**, inventory **PARTIAL**.

[Developer Level22 testbed and validation
matrix](DEV_LEVEL_TESTBED_20261009.md) ·
[Continuous-game integration contract](
CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).


## Long-term integration requirement — one complete game

**The current level demo is a vertical slice of the complete
original game, not a standalone product.** The required
release flow is the original CODE.2 startup/cinematics →
CODE.1 character creation (or valid existing CHARS) →
shared CODE.2 exploration/Level 22 → original in-game
cinematics and events → subsequent authored levels,
with **the same persistent party and game state** across
all transitions. Each future level must enter through
the preceding level's original event/exit and return
control to the same game session; developer preview
switches cannot replace the actual startup pipeline.

The verified source already routes CHARS-present startup,
newly completed character creation, and opt-in preview
into one `run_exploration()`. A general in-session
level-transition/persistent-world lifecycle is **not yet
implemented**, so "seamless" is a binding completion
target, not a claim of existing runtime acceptance.

**[Read the permanent continuous-game integration
contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).**
All changes are committed; Windows builds remain
manual and user-authorized.


## Part 8 continuation — native Level16 event 8 now connected

The four original `SEED OF MORNINGLORD`
locations at Level16 now reach a native
source-order complete-event handoff rather than
the previous diagnostic-only pending state.
The original `21880` chain is:

`2195C` adds the fourth item 0x89 →
`1C124(0x44)` sets persistent story event bit →
`13EF8(8)` plays **`CINE08.ANM`** (182
original frames) →
`15050(0x73)` grants **HOLY SYMBOL OF
RAVENKIND**. The earlier CODE.2-derived
cursor/party/world reward implementation is
now called AFTER real event playback, with
original item ID, source identification bits
and family-C 300-record fallback.

`Level16RitualCompletionModel` preflights
original placement and reward branches
without prematurely modifying the game.
The world, cursor and story flag are then
advanced in source order for event playback.
The existing `AnmCinematic` and Win32
`play_cinematic` host function load and
display the supplied event-8 resource using
the startup audio/MIDI bridges. The inventory
modal hides its cursor and detaches gameplay
click input during the blocking cinematic,
then restores input, cursor, audio/palette
presentation flow, scene-cache freshness
and retrace pacing afterward.

Native Escape, Quit, missing resource, and
failed playback have explicit recovery paths:
the current in-memory fourth seed, story bit
and reward are rolled back to avoid duplicate
or incomplete quest completion. This is a
**native safety choice**, not yet a
DOS-certified cancellation/teardown behavior.

Extended assertions in existing
`inventory_world_drop_tests` verify the
original flag/seed/reward order during
callbacks, repeat-event suppression,
Escape/Quit/exception recovery and no
silent completion without an event presenter.
The existing exploration-runtime tests
check no invocation before world
initialization. **No builds or tests were
run.** Registered CTests **170**, Quick
**19**; engineering score **56.25%**,
inventory milestone **PARTIAL**.

The source `13C88` branch is now audited:
original event 8 selects common exit target
`0x13E33` from nine post-event DWORD entries,
not the event-specific level-relocation cases.
Actual `13C88` music/palette/resource
restoration, DOS Escape semantics, frame/tick
comparisons and full-world SAVE/LOAD still
require live native-vs-DOS acceptance.


## Part 8 continuation — complete Holy Symbol item placement and CINE08 source identity

Original CODE.2 `1511F..1513C` calls the original
`2195C` world-item constructor when the
`HOLY SYMBOL OF RAVENKIND` (object 0x73)
cannot fit in either the empty cursor or
the first free ordinary party slot.
The call's **final clear-held flag is zero**;
unlike an ordinary dropped inventory item,
the reward world spawn cannot consume or
replace the player's currently held object.

Native `InventoryWorldDropModel::
spawn_holy_symbol_reward_15050` now
reuses the original forward-placement,
player-position fallback, Q16/cell validation,
known-bit = 0, 300-object oldest-first
eviction and family-C append semantics.
`HolySymbolRewardModel::grant_with_world_15050`
provides the complete original allocation
precedence: cursor, ordinary party slot,
and real world fallback. A failed fallback
returns `WorldPlacementRejected` without
mutating the party, cursor or live world.

Further original CODE.2 audit establishes the
numbered presentation at `13EF8(8)` resolves
to **`CINE08.ANM`** through `13AD8/139AC`
and the `CINE00.ANM` executable template.
The original animation is **182 frames**.
Existing native `AnmCinematic` decoding and
Win32 startup playback already support the
ANM format; a safe exploration-modal event-8
handoff with source-order flag/cursor/reward
updates and quit/Escape behavior is still
required. The final triggering fourth seed
remains deliberately unconsumed with
`SourceSpecialActionPending` until that
handoff can run. Do not call the reward
helper without the original event.

Existing inventory-world-drop assertions
now include the source world-spawn reward
branch, correct unidentified 0x73 record,
unchanged original held object, original
300-entry eviction, no partial mutation on
rejected placement and cursor/party priority
even with invalid world geometry. **Registered
tests remain 170 / Quick 19.** Source has
not been compiled or CTest-executed for this
checkpoint. No build was initiated.
Engineering remains **56.25%** and inventory
remains **PARTIAL**.


## Part 8 extended — Level16 Seed of Morninglord and Holy Symbol reward

The supplied CODE.2 original source at `21880..2195A`
and embedded object-1 bytes `207E0:0D 13 0B 15
0D 17 0F 15` now define the exact Level16
`SEED OF MORNINGLORD` four-site ritual
predicate. The four source cells are
(13,19), (11,21), (13,23), (15,21).
A live loose item of original OBJECTS ID
**137/0x89** must occupy every site
(only the integer/high-16 bits of its
original Q16 X and Y coordinates count).
The source exits unless the level is 16
and persistent event bit **0x44** is unset.

A new `Level16SeedRitualModel` scans the
source family-C loose-item records, handles
duplicate placements and the original
300-record oldest-item eviction rule,
and projects the three verified completion
actions: set bit 0x44; run numbered
interactive event **8** through `13EF8`;
and invoke `15050(0x73)` to grant the
**HOLY SYMBOL OF RAVENKIND**. Ordinary
non-completing Level16 drops are now enabled
instead of being categorically blocked.
A fourth/triggering seed returns explicit
`SourceSpecialActionPending` without
consuming the held item or prematurely
setting event 0x44, because original event
presentation remains unfinished.

The original `15050` Holy Symbol reward
case is now independently implemented
through `HolySymbolRewardModel`: an
empty cursor receives the unidentified
object; otherwise the first empty ordinary
inventory slot in active-actor order
receives it and clears its known bit.
If both cursor and ordinary slots are full,
the helper reports its original world
spawn fallback **without fabricating it**.
The helper is not yet called by the
unimplemented numbered-event sequence.

Existing `inventory_world_drop_tests`
has source-driven cases for the four
locations, Q16 fractions, duplicate and
wrong items, event flags, bounded world
array eviction, final placement rollback,
and reward cursor/party/world priority.
**170 CTests and 19 Quick tests remain
registered. No new build or tests have
been executed.** The native game is still
partial, including original event-8
presentation and final reward integration.
Engineering score stays **56.25%**.


## Part 8 extended — multi-actor original containers and phase artwork

New CODE.2 source verification at `12E70/11C3C`
closes the original per-actor click-routing gap:
inventory container grid clicks now select the
actor at the clicked 80-pixel column, verify that
actor's `6FA0` subtype/index/full-width equipped
hand item, and use the source descriptor grid.
One open actor panel no longer closes another.
The native selected `open_container` is an
interaction convenience, NOT a saved global owner.

Original SAVEs can now restore **multiple fully
open phase3 actor containers** when each matches
its own original OBJECTS descriptor/hand unambiguously.
The 62 serialized container records, all four
raw 6FA0 contexts and all other original SAVE
bytes retain their original encoding. A currently
unselected native view does not invalidate other
active source contexts.

A new original disassembly distinction is also
preserved: **110B4 animated closure** leaves
`FF FF 00 FF`, whereas **21648 hand transfer**
writes `FF FF FF FF` before an optional direct
phase3 reopen. These paths no longer share an
incorrect sentinel. Source CODE.2 `10AA7`
transition art for phases 1/2 is now rendered
using original RES2 bank 2 graphic
`0x14+2*subtype+phase-1` at row `0xC4`;
phase3 keeps original `9+subtype` artwork
at row `0xCE`.

Existing SAVE, inventory, hit-testing and
inventory-screen assertions now cover cross-actor
selection, simultaneous state roundtrips,
close/reset semantics, corrupt context rejection,
and all seven source transition-art subtypes.
**Registered CTests remain 170; Quick 19**.
No build or test execution was dispatched.
The original animation phase/IRQ/page timing
and full-world SAVE/LOAD are still pending.
Inventory and game fidelity remain **partial**;
engineering tracker **56.25%**.


## Part 8 continuation — original container UI 6FA0 state restored

The original CODE.2 disassembly closed an important SAVE/UI
compatibility gap. The final 16 SAVE.DAT bytes are **four
independent 4-byte actor container contexts**:
`[subtype,descriptor,phase,object_low_byte]`.
The source opens through phases 0,1,2,3
(`10FAC..110B3`), closes through 3,2,1,0
(`110B4..1118A`), and may set phase3 directly on
container-to-hand transfer (`21693`). **Closing leaves
`FF FF 00 FF`, not `FF FF FF FF`**. Both inactive
forms are now accepted by the guarded native
inventory SAVE bridge and preserved byte-for-byte.

A full-open phase3 SAVE can now restore its interactive
hand container when the original OBJECTS definitions,
source descriptor subtype/index, and exactly one of
the actor's two signed-16-bit equipped hand objects
agree. The saved object identity is deliberately
only **one byte** (`6FA3`), so the binding explicitly
checks low-byte collisions instead of assuming that
the byte uniquely identifies a full object ID.
It fails before mutation for missing/ambiguous object
evidence, invalid selector, wrong subtype or owner.

The native inventory tracks all four actor contexts,
writes live context updates to original-format SAVE,
preserves the post-close phase-0 sentinel, and carries
context records along when party characters exchange
positions. The presenter draws complete phase3
container contents for every actor lane using the
real original RES1/RES2 graphics. Native
`open_container` denotes only the **current clicked
interaction selection**. All four original actor
contexts now load and remain independent; pointer
clicks switch selection by the clicked 80-pixel
actor column, and original SAVE export verifies each
active context. Interrupted partial-animation SAVE
states still roundtrip as raw bytes but remain
load-unsupported until original page-flip/IRQ
progression is integrated.

Regression assertions for original 6FA0 phase progression,
closed-state bytes, phase3 roundtrips, 8-bit ID aliasing,
ambiguous/corrupt hand associations, source-safe rejection,
and actor-context swaps were added to THREE existing test
targets. **CTest total remains 170; Quick 19**;
no Windows or DOS execution of current source has
been claimed. The original SAVE inventory codec remains
a bounded inventory-only integration, not the game's
fully correct user-facing SAVE/LOAD workflow.
Engineering 56.25%; inventory PARTIAL.


## Part 8 continuation — live inventory SAVE snapshot bridge

`ExplorationRuntime` now exposes bounded inventory
`import_matching_save_inventory` /
`export_matching_save_inventory` backed by the
original-format `InventorySaveCodec`. Imported live
state includes the complete party actor array (items,
item-known bits and Mage spell matrices), 32 story
flags, cursor identity/known and CODE.2-defined
persistent container contents/state. Subsequent
inventory actions can be overlaid onto the retained
unaltered original SAVE image.

**Deliberately fail-closed:** an inventory-only
operation requires initialized native gameplay and
a saved level, coordinates, facing and time-of-day
exactly matching the active world; source moving
effects, non-inactive original 6FA0 container phases
and native open-container views are not accepted.
Export additionally requires no world-scene revision
since import. This avoids presenting an internally
inconsistent partial reconstruction as a valid
whole-world SAVE. No user-facing LOAD/SAVE menu
or disk-file IO has been enabled.

The original supplied SAVE is verified as
`0x2448` bytes, with its 62-state bank at
`0x9A7..0xD50`, 438 serializable item words,
and inactive trailing UI contexts. Existing
`inventory_save_codec_tests` and
`exploration_runtime_tests` now include
positive live-state roundtrips and source-world/
container rejection cases. **170 registered
CTests; 19 Quick**. No Windows compilation/test
was initiated. Inventory remains PARTIAL,
and engineering score remains **56.25%**.


## Part 8 continuation — lossless original SAVE.DAT inventory projection

Original shipped `SAVE.DAT` and `CODE.2` hashes have
been reverified. The new `inventory_save_codec.hpp/.cpp`
preserves the original SAVE layout while decoding and
re-encoding inventory-owned data: the 4×0x248 character
records (including all 21 inventory IDs/known bits and
Mage spell matrix), held item/known pair, 32 global
event bytes, 62 variable-width container records and
four raw container-UI context records. Class-3
transient-effect count `N` shifts later original offsets
by `N*0x16`; the codec rejects mis-sized/corrupt input
rather than using the original zero-effect fixture's
offsets universally.

The source CODE.2 63rd descriptor remains **unserialized**;
the unusual state-61/62 object selector is preserved.
The emitter keeps every non-inventory SAVE byte unchanged,
including independent clocks, level/world position,
effect records, roster and charged items. New regression
cases cover exact roundtrips and 0,3,150 dynamic records.
This is an **inventory-only byte codec**, not a completed
native save UI or full world-state load: original container
open/menu phase restoration, live session integration,
and complete on-disk SAVE acceptance remain pending.
**170 registered CTests**, 19 Quick; the new test and
other current source are NOT Windows build-verified.
User-requested manual Full build is still deferred.
Overall source progress remains 56.25%; inventory PARTIAL.


## Part 8 continuation — original Mage scribing messages and IRQ expiry

The original CODE.2 source at `116D5..11798` and
`13118..131BB` now drives native scribe feedback. Original
object-2 `6D9C/6DA0/6DA4` DWORD pointers resolve the
**exact original success, full-capacity, and already-known
text**, rather than hardcoded approximations. Success prefixes
`T[0x42+bank*10+entry]` and appends ` SPELL SCRIBED.`.
Both success and rejection set `6DAC=1` and `6DA8=
30700() + 0x36`. The inventory loop expires only when the
unsigned observed IRQ tick is **greater than** the deadline,
restoring the ordinary item ribbon. Entry to `13118` resets
the pending message. The native presenter now renders this
message even after successful scribing empties the held cursor.

The new source-backed state and existing inventory CTest target
include assertions for all three message strings, T prefix,
timer equality, expiration, no observed tick and original
unsigned DWORD wrap behavior. Rendering retains the
**previously provisional** native ribbon backing and font
until DOS screenshot/pixel parity work; the Win32 observed
IRQ tick is an exploration-epoch BIOS-cadence fallback,
not proven byte-identical DOS interrupt phase. In particular,
this is not a completed 2975C/F66C VGA ribbon reconstruction.

**All current-revision tests remain unexecuted on Windows;**
170 registered CTests, 19 manual Quick tests. Inventory and
SAVE/LOAD remain incomplete; per the user's request no manual
Windows build has been dispatched. Engineering 56.25%.


## Part 8 — Tome/Parchment persistent-flag interaction (source only)

The original CODE.2 `117A0/11857..11891` inventory dispatcher
uses object `0x35`, **Tome of Evil Artifacts and Cursed Items**,
with exactly four held Parchment Fragments `0x61..0x64`.
It invokes `1C124(fragment-0x61)` to OR story-event bits 0..3
into the persistent 32-byte DGROUP `0x22200` vector and calls
`1A344(-1,0)` to clear the cursor, leaving the Tome in place.
This exact event mutation is now connected to the live
`ExplorationSessionState::persistent_event_flags` through
`InventoryRuntimeState::swap_ordinary_slot` and the Win32 modal,
with source-specific regressions for all four, repeats and
no unexpected inventory/flag mutation.

The source also removes the overbroad ban on otherwise ordinary
moving of `0x35` and held `0xA6` Mage Spell Book. The original
`11808` book+scroll `0xA7..0xC9` learning branch is now
source-implemented through CODE.2 `11630` using its six
original scroll-ID range pairs (object-1 `EEB0`),
original `6153` Intelligence capacity, actor's 8×10
Mage matrix, and unconditional `0xBA` story-flag update.
Duplicate/full attempts leave the held scroll in place;
success clears the scribed matrix byte and held scroll.
Original message text and IRQ expiration are now source-modeled
and shown in the provisional native item ribbon. Exact DOS
font/decorative pixels, tick phase, SAVE/LOAD and runtime
acceptance remain outstanding.
The earlier blanket classification of `0x35` as a potion was
incorrect and is superseded. The exact `2D5D0` inventory
drop predicate is source-implemented in the preceding checkpoint,
but its mutable global `F048` behavior remains open.

**At that earlier source checkpoint:** 169 registered CTests
and 19 manual Quick tests. No new Windows build, DOS parity proof or SAVE
roundtrip has been performed. Overall engineering 56.25%;
inventory milestone still partial. Manual Full build remains
deferred until complete original inventory behavior and visual
acceptance blockers are closed.

## Current inventory-only source checkpoint (full completion still pending)

[Original inventory fidelity workstream](INVENTORY_FIDELITY_WORKSTREAM_20261009.md)
tracks the user's nine DOS inventory screenshots as the
active **single-system development priority**. Since
the last milestone, source-backed corrections now
include: original CODE.2 **20 equipment pickup
rectangles** from both race-dependent 12C00 arrays;
**different-subtype recursive container nesting**
with original weight/hand-pair rules and cycle
preflight; original **RES2[9+subtype]** container
surrounds and **RES2[0x25]** MAX/CURRENT burden
panel; and a live **D-key → family-C world drop**
using original 2195C position/facing, known state
and 300-item FIFO.

These are committed source, not a finished original
inventory. CODE.2 special item uses, Level16 item
0x89 quest side effect, exact 2D5D0 drop collision,
mutable open-container SAVE/LOAD roundtrips,
ancillary effects/condition popups and final
pixel/color/character comparisons remain open.
**At that earlier checkpoint, 169 CTests** were registered,
and the manual-only Quick mode covered **19** focused tests.
The latest source registry is now 170 CTests.
No current-source Windows build has run.
Per user direction, DO NOT dispatch the Windows
build until every original inventory behavior and
visual acceptance blocker is addressed.
The 32-milestone overall Demo-v1 engineering
score has not been artificially raised for
submilestone changes.

## Inventory-only workstream — functionality in source, not DOS accepted

[Current full inventory implementation and remaining 8 acceptance blockers](INVENTORY_FIDELITY_WORKSTREAM_20261009.md).
The original class/race starting equipment matrix, 12 ordinary item
slots, category-directed 9 equipment slots, original hand-open
containers and nested slot swaps, item description and dynamic RES1
held cursor, original CODE.2 Strength-based carrying limits,
and drag-and-release party reordering are now connected to the live
`PartyPanels` native view.

Important original CODE.2 source evidence:
`12E70` inventory dispatch, `117A0` known-bit swap,
`12180` equipment classification, `11C3C`
container cells, `21404` insertion, `12988`
encumbrance panel, `12470` party order, and
`7E8C` 21×5 carrying-strength table.
The MAX140/CURRENT17 example in the user's original
screenshot follows Strength12 and 34 half-pound units exactly.

**Inventory is not yet complete or verified.**
Open gates include original red and stone panel graphics/palette,
special item/spell use, nested category-1 recursion, original
world item dropping, persistent SAVE behavior, complete source
hit-descriptor variants and live DOS pixel comparisons.
These are explicit blockers before the user-requested
**manual Full Windows build**.

Current source has **168 CTests** registered; the Quick
manual mode has 18 focused tests. These newer source changes
are NOT yet MSVC-compiled. Last green manual #640 was at
an older source revision. Do not dispatch a build or
announce entire inventory functioning yet.

## Inventory-only development focus: every original class/race starter kit

[Full original CODE.1 starter-equipment matrix](ORIGINAL_CODE1_ALL_CLASS_RACE_STARTING_EQUIPMENT_20261009.md).
The original 1.2a CODE.1 stores **15 class families × 6 races
= 90 source rows**: 44 have real starting equipment;
46 are all-FFFF prohibited combinations. The native
`CharacterTransferTables` now parses and validates the
entire table and `build_character_transfer` directly
uses its **exact 21-slot starting-loadout model**
for both newly created party members. Native startup
continues to use original CODE.1 → CHARS → CODE.2,
never unrelated `SAVE.DAT` party equipment.

All 15 class families, all six races, complete
14-word→21-slot mapping, source-correct second actor
category-1 duplicate-item removal, and the original
Halfling/Gnome Cleric examples are covered in the new
`character_starting_equipment_tests`.

The next **manual-only** Windows build now expects
**165 CTests**, including this exhaustive equipment
test. The default Quick mode includes twelve targeted
CTests, the existing full mode runs everything.
`Audit-Starting-Equipment.cmd` prints literal
OBJECTS.DAT item names and IDs for all 44 kits
from the user's original installed media.
This code is **committed but currently unbuilt**.

**Next inventory work:** original equipment slots,
held-item labels, container UI, stat/encumbrance and
visual/palette fidelity against the nine original
DOS inventory screenshots. No claiming full
inventory parity from starter-loadout completeness.

## Current development: 56.25% Demo-v1 engineering source coverage

[Canonical 32-milestone engineering tracker and faster manual builds](ENGINEERING_PROGRESS_AND_FAST_BUILD_20261009.md):
**12 source-implemented, 12 partial, 8 open**. The transparent
rule awards one point per source-implemented bounded deliverable
and half a point per partial: `(12 + 12×0.5)/32 = 56.25%`.
A partial milestone becoming source-complete advances the
tracker by **1.5625 percentage points**. The earlier repeated
"20%" meant just **one of five giant formal acceptance gates**,
and was misleading as an engineering-progress number.
All older 20% statements below are historical acceptance
references, not current developer progress.

**Verified Windows:** last green manual #640 (160/160 at an
older commit); current expected 164 CTests unbuilt.
**DOS fidelity:** original screenshots still disagree in
movement, ground/trees, NPCs, inventory and combat.
These three measures must remain separate.

[Manual demo-v1 workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/demo-v1-build.yml)
now offers **Quick (default)** and **Full** build modes.
Quick builds the EXE/ground probe and nine focused tests;
Full builds all 164 tests. MSVC compilation is parallelized,
with best-effort incremental build cache. The #640
7m40s job spent **6m26s compiling**, only 16 seconds
testing, so reducing compiled test binaries is the
immediate optimization. Neither mode runs automatically;
no new build speed has been measured yet.

## Current source milestone — live original 34A74 forest floor in open Level22

[Complete guarded live raster checkpoint](LEVEL22_LIVE_ORIGINAL_34A74_OPENFIELD_GROUND_20261008.md):
native first-person presentation now conditionally renders the
**actual original RES4 type-1 64×64 indexed terrain texture**
with original SINTAB FF58 perspective, 34A74 four-plane/
two-plane VGA pixel loop and FE15..FF14 shading.

The live path is restricted to an independently
source-backed condition: fresh Level22, authentic
`INIT+0x0E` + signed level header +7 resource
selection, verified decoded 4096-byte original tile,
128 original perspective rows, original C026=1,
and **all 256** source sweep rays with both
resources equal to non-drawable **0x93**.
Original CODE.2 335C8 / 1F2A5 / 1F372 proves
the no-wall sentinel requires exactly **128**
full lower terrain scanlines.

Any missing/malformed resource, partial sweep,
non-0x93 wall or unsupported clipped/dynamic
ground state leaves the previous flat fallback
unchanged and reports the source-specific reason.
The opt-in capture metadata records real ground
VGA writes, selected source IDs and current
render/fallback status; the window title shows
`Ground RES4 textured` versus `Ground flat fallback`.

**This is committed source, NOT yet a built or
DOS-verified visual release.** 164 registered
CTests are expected in the next manually dispatched
Windows build; latest successful Windows run #640
was at older source. Other visual/combat/inventory
gaps remain. Formal accepted gates still **1/5**.

## Original ground RES4 absolute source now resolved from INIT

[Source/disassembly checkpoint](ORIGINAL_LEVEL22_RES4_INIT_BASE_RESOLUTION_20261008.md):
original `CODE.2 16E55..16E90` reads the RES4 bank
base from `INIT+0x0E` and stores it at `258F8`.
`CODE.2 1F388..1F398` adds that value to the
**signed level-header+7** ground selector. This is
now implemented in `OriginalGroundRes4Audit::resolve`:
the real media's `INIT`, `LEVELS.DAT` Level 22
and `RES4` determine the exact original type-1
64×64 tile, with strict decoded-byte validation.

Level 22 native gameplay startup now loads and
validates the actual selected tile (once), recording
any failure rather than substituting unrelated art.
Optional capture TSV v2 includes actual RES4 base,
signed relative selector, absolute entry and error.
`Probe-Level22-Ground.cmd` now prints the resolved
entry from original files as well as other candidates.
These are **source-only**, awaiting the next manual
Windows build and original-resource capture.

**The live lower surface remains flat**, not
because the texture index is unknown anymore,
but because original `FF50` dynamic row clipping,
`C026/C027` branch state and upper/lower draw
composition still require an independent faithful
runtime model. The original `34998/34A74` core
and decoded source bytes are available to complete
that step, without generating procedural grass.
There are **164 registered CTests expected** if
Python is installed; last successful manual #640
at an older SHA ran 160/160. Formal acceptance
remains **1/5 gates (20%)**. No automatic build
has been initiated.

## Original RES4 forest-floor identification — next manual build

[Original RES4 source-candidate audit](ORIGINAL_LEVEL22_RES4_GROUND_SOURCE_AUDIT_20261008.md):
the original game files are preserved in `DnD-Classics`,
and the native `ResArchive` can already parse EH type-1
raster entries from those binary bytes at runtime.

The newly committed `OriginalGroundRes4Audit` scans the
actual installed `RES4` for all decodable, one-image,
64×64 indexed sources, identifies raw versus LZSS
compression, flags invalid entries, and fingerprints
each original decoded tile. It never assumes that the
Level 22 signed *relative* selector `55` means
absolute `RES4[55]`; original `CODE.2 1F388`
adds mutable `258F8` before looking up the texture.

The next manual Windows artifact will include
`rloft_res4_ground_probe.exe` and
`Probe-Level22-Ground.cmd`. Running that launcher
with original installed media produces a small
non-extractive report of source indexes, checksums
and **unverified** implied bases. Synthetic regression
fixtures exercise authentic type-1 raw/LZSS resource
framing and the 34A74 indexed sampler. **164 CTests
are expected**, with Python installed.

The original SINTAB ground projection and textured
raster cores are implemented separately, but the
live Level 22 foreground remains **flat** until
original runtime bank selection and row-span state
are independently verified. No estimated terrain
pixels or procedural graphics are introduced.
The new source is **not yet built**; #640 is the
latest successful earlier Windows run.
Formal acceptance remains **1/5 gates (20%)**.
No Actions run was started.

## Original DOS forest floor — source-backed 34A74 raster core now implemented

[Full CODE.2 34998/34A74 reconstruction](ORIGINAL_CODE2_34A74_TEXTURED_GROUND_PIPELINE_20261008.md):
the original DOS forest terrain does **not** use a uniform
palette fill. The native source now reconstructs all **128**
authentic six-byte perspective rows from SINTAB
`sine_q16_signmag` and `projection_a/b` (source
`CODE.2 3838,34270,34998`). It also implements the
original pixel kernel `34A74 -> 342E4/34954 -> 34350`:
64×64 texture wrapping, FE15..FF14 shading, 256-pixel
clipped viewport, 80 four-plane rows plus 48 paired-plane
rows, with transactional validation.

Actual original SINTAB values at perspective rows
**0,79,80,127** are now regression fixtures. Every
native exploration snapshot generates original FF58
row steps and camera texture phases, and the optional
TSV records them for DOS parity investigation.

**The old flat lower fill is NOT yet replaced in live
pixels.** The original RES4 bank-4/type-1 ground texture
selector depends on a mutable resource base at `258F8`
and per-level signed selector at `2561F`; its exact
runtime selection is still under examination. Neither
a guessed RES4 type-6 wall tile nor procedural grass
may be substituted. The real RES4 archive is preserved
in the DnD-Classics Git branch.

These are source-only changes pending the next manual
Windows CI run. The prior green #640 is older;
**163 CTests are expected** in a standard Python-enabled
build. Formal acceptance remains **1/5 gates**.

## Current source checkpoint — faithful fresh-party movement and blocking

[Original CODE.2 binary-backed audit](ORIGINAL_FRESH_START_TIMING_COLLISION_AND_GROUND_AUDIT_20261008.md) confirms the fresh
CODE.1->CHARS->CODE.2 entry is **level 22 at (4.5,4.5),
facing 270**, directly from `CODE.2` instructions,
not inferred from `SAVE.DAT`. Do NOT shift the starting
cell arbitrarily to match an outdoor screenshot; align
original camera, timer, NPC live state and texture raster.

The staged source separates original two-IRQ-tick
gameplay movement from VGA refresh and connects NPC and
placed-object collision after cell geometry. The
original `2CC88` conditional class-1 type 0x54
transition is now gated on **state 0x40 and level
0x17 or 0x0A**; other overlap cases continue without
blocking. Type 0x7A retains its unimplemented authored
action, rather than fabricated collision effects.

**Visual gap still open:** supplied `CODE.2` initially
sets `C026=1` and `0x34A74` draws textured ground.
The native presenter still uses flat `0x33DE0` fill.
Forest terrain, NPC AI, inventory and weapon/hand
animations are not yet DOS-equivalent.

**Build:** newer source-only work has NOT been Windows
compiled; latest green manual #640 passed 160 tests at
an older commit. 162 CTests are now registered when
Python3 is present. GitHub Actions is **manual-only**.
Formal acceptance stays **1/5 gates (20%)**.

## Original DOS screenshot correction: freshly created party, NOT a saved game

The player's nine original DOSBox screenshots depict a party **freshly
created through CODE.1** and entering Level 22. They are *not*
a restored SAVE.DAT party. See the new
[original DOS screenshot fidelity specification](ORIGINAL_DOS_FRESH_PARTY_FIDELITY_REFERENCE_20261008.md).

The default fidelity path must remain: new game -> CODE.1
character selection/stat generation/equipment -> 0x920-byte CHARS
handoff -> CODE.2 fresh startup -> Level22 exploration. An attempted
SAVE.DAT party substitution in the optional walkaround was
**reverted**; only a future explicit LOAD GAME flow may use saved
party records. The opt-in diagnostic shortcut uses real CHARS if
present, otherwise explicitly synthetic actors.

**Unbuilt source-only repairs currently staged:** host simulation
is no longer advanced on every VGA refresh (CODE.2 original two
IRQ-tick loop; provisional host tick interval); 2C800 NPC and
2CC88 placed-object/tree collision follows 2D5D0 cell checks;
Release-enabled tests cover both model changes and lossless
two-actor CHARS handoff. Original 34A74 textured forest foreground
remains unimplemented and cannot be represented as graphics parity.
The original screenshot also exposes incomplete NPC simulation,
starting-pose verification, inventory and attack animation.
None of this advances the formal acceptance above 1/5.

## Windows #640 source-matched Level 22 ray correction verified

[Detailed 16-frame post-fix audit](LEVEL22_640_POSTFIX_RENDER_AUDIT_AND_EXTENDED_CAPTURE_20261008.md):
manual Windows [build #640](https://github.com/Hyperbog/RLOFT/actions/runs/37872583683)
compiled source `47d5194a0c87109ff9c26df7794c960d492ccb99`,
passed **160/160** registered tests and matches the source revision
in all frames of the new user ZIP `1791512367563.zip`.
There is **no sweep failure** in these captured poses.

The original index-scaling correction now has visible native
effects: six camera-matched pre/post pairs differ by
**332–462 exact indexed pixels**, and native RES3 raster calls
increase (e.g. 105→167 at cell 4,4, facing 270).
The new archive reports **2,127 successful RES3 scene draws**,
**127 fully occluded** calls, zero source/trace/raster errors
and 16 unique frames. This does **not** certify parity with DOS.

The prior optional capture budget of 16 snapshots exhausts
before reaching the Level 22 NPC at (13,7). A **new
source-only** extension allows `--capture-frames=N`,
strictly bounded from 1 through 128, with the
`Capture-Level22.cmd` launcher requesting **64** changed-pose
frames. Bare `--capture-render` still defaults to 16;
the uncapped fatal `failure.tsv` remains independent
of the image budget. This later extension **has not yet
been compiled or tested by Windows CI** and still needs
a future *manual-only* build. The formal 1/5 acceptance
gate count remains unchanged.

## Second user capture — fatal ray forensics even after 16 snapshots

[Latest 16-frame source audit](LEVEL22_SECOND_CAPTURE_AND_FATAL_SWEEP_EVIDENCE_20261008.md)
follows original Level 22 cells (4,4)→(19,4), heading 270.
The new ZIP logs **1,157 successful RES3 scene raster calls**,
369 occluded calls, zero RES3 errors, and only expected
non-drawable 0x93 outdoor boundary walls. All generated RES3
source widths in that archive are 1–4, so it does **not**
test the new 8–255 individual-mode raster support. Its last
captured pose is NOT the later exception's identified pose.

The ZIP also demonstrates a forensic blind spot: ordinary
`--capture-render` stops after 16 successful frame captures,
so a later sweep exception is absent from the archive.
The optional capture mode now writes independent
`failure.tsv` in the same session directory if
`runtime.build_frame()` fails, regardless of the snapshot
budget. It preserves exact source revision, raw camera,
map dimensions and original exception/ray diagnostics.
Normal frame TSVs now carry `source_revision` as well.

**No current-HEAD Windows build has run.** Build #639 is
an older pre-sweep-fix binary. Start a NEW manual `demo-v1`
workflow rather than rerunning #639, and use the newly
downloaded Windows artifact. Acceptance remains 1/5 gates.

## Crash fix: original CODE.2 ray-cell addressing (next manual build)

[Verified binary-source diagnosis](LEVEL22_SWEEP_CELL_INDEX_CORRECTION_20261008.md):
the user-provided 16-frame Level 22 capture and screenshot exposed
a repeatable near-cardinal `first-person sweep failed to leave
malformed level` exception. The level is authentic and well-formed.
Original CODE.2 doubles flattened cell indices for 16-bit
word addressing and then undoes the doubling before the bounds
check. The native code wrongly halved those already-undoubled
indices, traversing incorrect cells and sometimes looping.

Both original left/right ray paths now preserve the source
cell index. A new assertion-enabled regression uses the
original Level 22 perimeter, exact source SINTAB sine/tangent
constants and a camera from the user's archive. Existing
fixture expectations were corrected. No world data or
original DOS assets were modified.

**Source fix requires a fresh manual Windows build.**
Previously green #639 predates the fix. All earlier native
ray visibility/occlusion and framebuffer captures were
affected by this bug and cannot certify DOS parity.

## Current next-build graphical milestone — original C029 wide RES3 pixels

[Full original-code checkpoint](LEVEL22_CODE2_C029_INDIVIDUAL_RES3_WIDE_PIXELS_20261008.md):
the supplied CODE.2 initial `C029=1` enables an original
**one-pixel-at-a-time RES3 sprite raster mode**. Previously, the
native engine incorrectly rejected ALL RES3 source runs wider
than seven even in this mode. It now keeps the original run
width unchanged and executes certified width-one VGA passes
and generated SINTAB code for each destination column,
supporting source spans **8..255** when C029 is individual.
Grouped mode continues to reject unsupported widths above
seven; a 256-column source span still needs separate proof.

Tests cover actual eight-pixel, two-VGA-byte indexed SINTAB
raster output, 12-column complete descriptors, 19/255-column
VGA masks, exact ECX odd-column parity, and original
256-ray clipping. The opt-in capture additionally reports
the active C029 mode, original unsplit scene run widths
and 16.16 source camera coordinates. No approximations
or changes to copyrighted resources were introduced.

**Source-only until a new manually initiated Windows build.**
Last green run #638 preceded this renderer update; full
DOS pixel/IRQ parity and acceptance remain pending.

## Full RES3 source-isolated planning and indexed comparison (current source)

[Large source checkpoint](LEVEL22_RES3_FULL_PIPELINE_CAPTURE_AND_COMPARE_20261008.md):
both independent RES3 per-object planning **and** VGA trace generation
now preserve valid later scene objects when an unrelated source fails.
Planning failures occupy their correct original CODE.2 draw slots,
never count as fully occluded, and fail the completeness score. The
presenter additionally records per-call `planning-failed`,
`occluded`, `trace-unavailable`, `rasterized`, and
`raster-failed` classifications.

Optional Level 22 captures now include native metadata for the
original source catalog type (NPC 9, placed 62/63/64), original
resource and source indices, per-source stage/outcome and all
explicit failures, without altering the exact 320x400 indexed image.
A new asset-free Python tool
[`tools/compare_indexed_captures.py`](tools/compare_indexed_captures.py)
can compare native raw frames against **separately supplied** DOS
pixel and six-bit VGA palette captures, rejecting incorrect
provenance labels, lengths, duplicate poses and unmatched frames.
It never fabricates a DOS reference.

All builds remain manual-only. A new Windows build is required
to check these changes; previously green run #638 contained
159 tests, while the updated source expects **160 CTests**
with Python available. **Formal acceptance remains 1/5 gates.**

## RES3 source-isolated Level 22 scene tracing — next manual build

[Source checkpoint](LEVEL22_RES3_SCENE_TRACE_ISOLATION_20261008.md):
the optional type-7 VGA trace builder no longer drops all
visible scene objects when an unrelated RES3 draw call has a
malformed/unsupported descriptor. It retains original call
order, successful per-source traces, unsupported-wide null
slots and the exact failed source identity. The strict original
verification API remains fail-fast.

New v2 opt-in TSV rows `scene_call_trace_failure` show
call index, original source class/index, absolute RES3 ID
and error reason. The Level 22 preview title now reports
`RES3 calls rendered/total` and `RES3 trace errors`
in addition to SINTAB wall counts. No original framebuffer
pixels were approximated or source asset data modified.

**Not yet manually built/tested on Windows at current HEAD.**
Last green manual Windows build #638 (159/159 tests) predates
this patch; formal acceptance remains **1/5 gates (20%)**.

## Level 22 original-content correction — native capture evidence

[Audit of 30 user-supplied indexed frames](LEVEL22_CAPTURE_AUDIT_20261008.md):
Level 22 is an original 30×15 **outdoor/open-field map** with
**0x93** non-drawable perimeter resources. All **7,649**
captured wall-run rows were non-drawable. No expected wall pixels
were suppressed by the new SINTAB backend in those views.
Thus do not use Level 22 for ordinary RES4 wall-render fidelity;
use an actually wall-bearing level instead.

Level 22 source-data priorities are **RES3 world-scene sprites**:
one type-9 NPC at (13,7), 110 placed objects (52 type 62,
42 type 63, 16 type 64), with appropriate DOS pixel and
interaction verification. The opt-in `--capture-render`
TSV now reports **v2 scene-record/draw-call/type-7 raster
diagnostics** without changing the original-resolution pixels.
The v2 source change has not been rebuilt; previously green
Windows run #638 was at earlier source.

## Diagnostic build: raw Level 22 frame export

[Capture checkpoint](LEVEL22_NATIVE_FRAME_CAPTURE_CHECKPOINT_20261008.md):
the optional `Capture-Level22.cmd` launch mode writes a maximum of
16 changed-position snapshots under
`%TEMP%\RLOFT-Demo-Captures\<session>\`.
Every snapshot contains the original-resolution **native** indexed
frame (`.idx`, 128,000 bytes), its palette (`.pal`, 768 bytes),
and `.tsv` wide-wall `C6F4` geometry, immutable CODE.2 VGA mask
bytes, coverage counts and failures. The launcher is staged in the
manually built Windows artifact. The captured mask bytes are
**shipped static data, not DOS live RAM**. A true DOS reference
trace/frame is still required before fidelity claims are made.

This is committed source, **not a newly built EXE**. The current
formal acceptance remains 1/5 gates (20%).

## Bounded wider ordinary walls (original CODE.2 mask audit)

[Original 335DC/336F7/3263C mask checkpoint](LEVEL22_CODE2_WIDE_MAIN_SURFACE_MASK_BOUNDARY_20261008.md):
the native ordinary wall renderer now accepts exact eight-column
coalesced runs beginning at pixel phases 0, 1, and 2, verified
directly against original CODE.2 mask bytes. The original never
splits C6F4 into seven-column chunks. Phase 3 width 8 and all
widths 9+ remain explicitly unsupported until source/runtime
state is verified; no guessed overdraw is introduced.

The voluntary `--explore-level22` preview title displays current
`SINTAB walls rendered/total` and `Wall errors` without altering
original-resolution pixels. Source code and regressions are committed,
but the newest executable has **not** been built at this HEAD.
Formal release acceptance remains **1/5 gates (20%)**.

## Current visible-rendering milestone — ordinary SINTAB wall pixels

[Original CODE.2 main wall checkpoint](LEVEL22_ORIGINAL_SINTAB_MAIN_WALL_CHECKPOINT_20261008.md):
`3263C/FCD0=0` ordinary in-range RES4 surface runs now execute the
actual SINTAB generated rasterizer, in their original VGA plane masks
and shade pairs. This is **connected directly to the live Level 22
exploration presenter**, after the preliminary SINTAB pass and before
large-extent fallback walls and type-7 scene objects.

The executor covers original group widths 1..7 and retains all
unsupported wider/invalid runs as diagnostic pending work rather than
fabricating image pixels. It is backed by registered original-mask,
page-isolation and transaction/rollback regressions, but **has not
yet passed a new complete Windows build or original DOS frame
comparison**. The last green Windows EXE predates this change:
download it only as the prior baseline, not as evidence of these walls.
Formal release acceptance remains **1/5 gates**, separate from
unmeasured engineering completion.



## Native original clipped VGA sprite renderer (CODE.2 2A6B0)

[Source checkpoint](LEVEL22_CODE2_2A6B0_CLIPPED_VGA_CHECKPOINT_20261008.md):
the original `2A6B0` clipped-sprite backend used by
`29414` now has native **four-plane VGA pixel copying**.
It reads the six-DWORD `29414` clipping record,
reproduces mutable `F00D–F031` graphics counters,
frame-transparent sentinels, per-plane sequencer masks,
forward source order and the unusual case where
a plane-3 rollover changes the number of physical
bytes drawn on subsequent planes.

The `2A7A4` original self-modification is also
implemented: when original mutable CODE.2 bytes
are supplied, the live comparison key changes
the **single code byte at 2A816**. The injected
service under `29414` is now source-backed;
existing full `2A380` raster remains connected.

**154/154 original CODE.2 instruction signatures**
were verified, bringing the cumulative executable
audit to **1,263 named / 1,255 distinct
instruction locations**. The exact committed
`2A6B0` core and callback binder passed
isolated strict C++17 Debug, Release and UBSan
tests against interface-compatible original
memory/VGA captures. Two new assertion-enabled
CTests are CMake-registered, **not yet included
in a consolidated Windows MSVC build**.

Next: audit `2A8A0` color-adjusted clipped
sprite copying, including its separate
code-patched immediates `2AA0D/2AA12` and
the SHL-vs-ROL plane mask difference; continue
authentic DOS pointer/game-state integration
and reference pixel/IRQ comparisons.

**Demo acceptance remains 1/5 formal gates**.
Engine progress has increased without yet
passing another end-to-end acceptance gate.



## Latest source-backed overlay predicates and sprite clipping (29414)

[Original CODE.2 29414/2AD18/2ADE0 checkpoint](LEVEL22_CODE2_29414_PREDICATES_CHECKPOINT_20261008.md)
implements the previously injected `2AD18` and
`2ADE0` object-type predicates, including the
real `1C1DC` world-state bitset at DGROUP
`22200`, and `29414` signed viewport clipping.
The 29414 model writes the original six-DWORD
(24-byte) scratch record at `EF74+EFD4*24`,
updates source `EF69/EF6B/EF6D/EF6F` WORDs,
and chooses full `2A380`, clipped `2A6B0`
or no raster (original EAX 2/1/0).
It calls `2E758` before the resource null check.

Adapters now install the genuine predicates beneath
`2AE30` and connect `29414` beneath the existing
`29804` draw-record stack. The shared original
`A6BC` and `D680` modes are synchronized
or sampled live instead of frozen as independent
overlay mouse and probe snapshots. An integrated
CTest exercises the full original callback chain
through the real overlay resource `880D`.

**105/105 original CODE.2 opcode checks matched**,
bringing the ledger to **1,109 named checks / 1,101
distinct addresses**. The two exact committed core
source blobs passed independent strict C++17 Debug,
Release and UBSan smoke tests. All three new
assertion-enabled CTests are registered, but
full current-HEAD Windows compilation and DOS
reference frame comparisons remain open.
The main remaining pixel backend is original
`2A6B0` clipped VGA rasterization.

Formal demo acceptance remains **1 of 5 gates**;
overall engineering completion is not a measured
single percentage.



## Original overlay pointer, hit-test and draw-record milestone

[Original 1A3B8 / 2AE30 / 29804 source checkpoint](LEVEL22_CODE2_POSITION_PROBE_DRAW_CHECKPOINT_20261008.md)
adds shared-engine native source models for the three remaining
original overlay entry services. `1A3B8` preserves independent
mouse-clamping modes, source-ordered `33C90/1F820`
callbacks and all three original position WORD outputs
(`9E31/9E33/9E35`). `2AE30` scans genuine
opposite-scene-page **22-byte records**, performs original
signed inclusive hit bounds, early type/frame filtering
and 0/1/2 classifications with authentic supplied
`2AD18/2ADE0` predicates. `29804` manages the
original `EFD4` nested drawing stack and
`EF74 + EFD4*0x18` record addresses before
calling real `29414`.

The three adapters compose with existing `1A288`
without replacing earlier four-plane VGA copying,
page selection, signed backing clipping or
reference-count behavior. **96 new CODE.2 opcode
checks** passed, raising the cumulative audit to
**1,004 named / 996 unique instruction sites**.
The exact source files passed isolated strict
C++17 Debug, Release and UBSan checks. Four new
assertion-enabled CTests are registered, but a
current-HEAD full Windows build and DOS frame
comparisons have not run.

The next original-code dependencies are `29414`
record rendering, `2AD18/2ADE0` predicates
and the `33C90/1F820` pointer services, followed
by authentic game runtime memory binding.
Formal demo acceptance is **1/5 gates**;
overall engineering completion is unmeasured.



## Original 2A114/2A178 four-plane overlay backing bytes

[Original VGA backing checkpoint](LEVEL22_CODE2_2A114_2A178_VGA_BACKING_CHECKPOINT.md)
adds native source-backed **actual four-plane VGA capture and
restoration** below the previously reconstructed `292BC`
and `2932C` signed overlay-clipping services. `2A114`
reads planes 0–3 into four contiguous blocks of original
DGROUP backing bytes while selecting graphics-controller
read-map index `04h`. `2A178` restores those blocks
using sequencer map masks `1,2,4,8` and finishes
with the exact original **`OUT 03C4h,FF02h`**.
The VGA state now preserves both raw `0xFF`
and effective lower-four-bit mask `0x0F`.

The original **zero-width restore** case is reproduced:
starting at column 3 still transfers one physical byte
per plane, whereas an aligned zero-width restore
writes no pixels but still updates hardware mask state.
An injected native binder supplies both raster services
to the existing clipped overlay chain, reusing live
`F001/EFED/EFF1/EFF9` rather than hardcoded geometry.

**53 new original CODE.2 signatures** raise the
byte-verification ledger to **908 named / 900
distinct instruction sites**. The exact committed
raster source passed isolated strict C++17
Debug/Release/UBSan tests against compatible VGA/DGROUP
interfaces. The two new registered CTests and full
Windows EXE have **not** yet been built at current HEAD.
Remaining fidelity work: original `1A3B8` coordinate
selection, `2AE30` hit testing, `29804` drawing,
live game-resource binding and DOS A/B frame/IRQ checks.

Formal demo acceptance is **1/5 gates**, distinct
from unmeasured overall engineering progress.



## Original 292BC/2932C backing-image clipping reconstructed

[Original backing clip checkpoint](LEVEL22_CODE2_OVERLAY_BACKING_CLIP_CHECKPOINT.md)
adds source-verified native clipping for the original
32×16 overlay capture/restore. `292BC` and
`2932C` use signed DWORD coordinates, signed
WORD EF3F/EF41 bounds and exact wrapping arithmetic.
They differ in a significant original quirk:
capture requires **positive** final column count,
while restoration can still call `2A178` when
the column count is **zero**.

The new reusable binding connects these rules to
the earlier `1A288/1AB20` overlay lifecycle;
the underlying VGA `2A114/2A178` raster backends
remain explicit, without invented backing pixels.
Of 30 sampled original instructions, 29 add new
auditor anchors (the 2939A return was previously
checked), bringing the executable audit to
**855 named checks at 847 unique addresses**.
The exact source and binder passed isolated strict
C++17 Debug, Release and UBSan checks.
Both new CTests are registered but await a
fresh full Windows build.

Formal release acceptance remains **1/5 gates**,
and the engineering completion percentage remains
unmeasured; this is additional reusable game-engine
behavior, not a newly accepted end-to-end demo gate.



## Original VGA page-stack integration (CODE.2 2E590 / 2E5D4)

[Source and integration checkpoint](LEVEL22_CODE2_VGA_PAGE_STACK_CHECKPOINT.md):
native overlay callbacks now execute the original DOS
`F0D0` **byte-depth VGA page stack** rather than
treating `2E590/2E5D4` as stub page changes.
The stack saves `EF4D` into `2B644`, preserving
its distinction from original `EFE9`, and
recalculates `F001` through `291B0/2AB6C`
with live `F005`. Original diagnostic calls at
depth 10/0, their continuation semantics and byte
wraparound are explicitly modeled.

A new binder connects page push/pop to `1A288/1AB20`
and synchronizes `EFE9/F001/F005` with the actual
`2A078` VGA-copy model. No hardcoded Level 22
page number was used. **23/23 original instruction
checks passed**, raising the executable audit to
**826 named / 818 distinct sites**. The exact committed
core source and binding passed isolated strict
C++17 Debug, Release and UBSan checks; the two
registered checked-in CTests have not yet run in a
complete Windows build on current `demo-v1`.

The next source-backed graphics boundary is
`292BC/2932C` 32×16 VGA overlay backing-image
capture/restore (including their *different*
zero-width clipping gates) and its lower-level
`2A114/2A178` four-plane operations.
Formal demo acceptance remains **1 of 5 gates**;
the development completion percentage is unmeasured.



## Original 1A288/1AB20 overlay activation and restoration checkpoint

The [original overlay source checkpoint](LEVEL22_CODE2_OVERLAY_1A288_1AB20_CHECKPOINT.md)
closes another shared-engine behavior boundary: the original
`1A288` first-activation and nested-reference increments,
`1AB20` last-reference backing restoration, signed WORD
wrap, and conditional `2AE30/29804` mode selection.
`CombatOriginalOverlayModel` operates on the SAME live
overlay globals used by `1A8C8` and can attach its
callbacks to the existing authentic `2A078` VGA
copy and `30CD4` backing-transfer services.

**49 new x86 opcode signatures matched** owned CODE.2,
raising the audit to **803 named / 795 distinct sites**.
The source-equivalent branch model passed isolated strict
C++17 Debug, Release and UBSan tests. A CMake-registered
integration CTest also exercises the full source-order
`1A8C8 → 1A288 → VGA copy → 30CD4 → 1AB20`
callback sequence, but it has **not** been run in a
fresh complete Windows build.

Original page stack `2E590/2E5D4`, actual
`292BC/2932C` overlay backing pixels,
`1A3B8/2AE30/29804` graphic/mouse behavior
and DOS frame/IRQ comparisons remain outstanding.
Formal demo acceptance remains **1/5 gates**;
engineering completion is tracked separately.


## Latest CODE.2 VGA latch copy and overlay backing transfer

[Original 2A078 / 30CD4 checkpoint](LEVEL22_CODE2_2A078_30CD4_COPY_CHECKPOINT.md)
now implements the shared screen compositor's **actual four-plane
VGA rectangle copy**. `2A078` uses VGA graphics-controller
write mode 1 and source-read hardware latches, with the original
`0x0F` sequencer plane mask, temporary destination page selection,
and exact forward `REP MOVSB` semantics. This is **not**
a packed indexed-pixel copy or `memmove`: overlapping VGA
copy regions can overwrite later source bytes.

A distinct `30CD4` native model now reproduces the original
unsigned-overlap direction choice and `0x400`-byte
DGROUP overlay backing transfer. Both routines are connected
to the prior `1A8C8` compositor through reusable
callbacks. Its direct path can write actual VGA pixels;
its buffered path can also move captured backing bytes
when real `1A288/1AB20` collaborators are supplied.
The original `1C258` message callback can invoke this
VGA copy before reading the independent `30700` IRQ time.

Original code audit: **70/70 new instruction signatures matched**
the original `CODE.2`, increasing the total to
**754 named checks at 746 distinct code locations**.
The exact committed 2A078 and 30CD4 sources passed
independent strict C++17 Debug, optimized Release and
UndefinedBehaviorSanitizer tests against compatible local
memory interfaces. The checked-in new CTest targets are
registered but **have not received a complete Windows build**.

Next: reconstruct actual `1A288` overlay suspension and
`1AB20` overlay restoration; connect real DOS resource
buffers and verify frame/IRQ traces against DOS. The earlier
130/130 Windows CI success belongs to its older source
SHA, not to this newer code. Formal release acceptance
remains **1 of 5 gates**; engineering completion is
tracked separately without an invented percentage.


## Current original 1A8C8 compositor checkpoint — signed row/column overlap

The [1A8C8 original compositor checkpoint](LEVEL22_CODE2_COMPOSITE_1A8C8_CHECKPOINT.md)
adds a reusable native **original source-order compositor wrapper**
with direct/no-overlay, direct/outside-overlay and
buffered/overlap paths. It preserves the original signed
32-row × 16-column overlay intersection, full WORD
reference counter, temporary 9E39 active page, A6BA
busy writes, and original 1,024-byte backing-buffer
transfer through `30CD4`. An adapter connects the wrapper
to the previously reconstructed `1C258` timed-message
renderer without sampling 30700 IRQ time early.

The actual `2A078` VGA graphics-controller plane copy
and nested `1A288/1AB20` original overlay services
remain injected, **not** replaced with guessed pixels.
An additional **31/31 executable instruction checks**
passed against owned `CODE.2`; the cumulative ledger is
**684 named checks at 676 distinct code addresses**.
An isolated C++17 source-equivalent smoke regression
passed Debug, optimized Release and UBSan; the new
checked-in CMake/CTest target has **not** yet run on Windows.
Prior Windows 130/130 CTest certification belongs to its
earlier pinned source SHA only.

**Project progress:** 1/5 formal demo acceptance gates
remain completed, while implementation progress is
tracked separately and is not assigned an unsupported
overall engineering percentage. See the
[progress scorecard](DEMO_PROGRESS_SCORECARD_20261008.md).


## Progress reporting — acceptance is not development completion

**Formal release acceptance: 1 of 5 major demo gates (20%).**
This is a coarse count of **accepted milestones**, not a
measure of implementation work, engineering hours, original
instruction coverage, or the percentage of the full game
completed. Substantial cross-cutting source reconstruction
and subsystem tests have been added since the first gate
was accepted, without yet earning another end-to-end gate.
Do **not** read historical “20%” checkpoint labels in
this README as “only 20% of development is complete.”

The [current development-versus-acceptance scorecard](DEMO_PROGRESS_SCORECARD_20261008.md)
lists the completed/partial subsystems and actual blockers.
The overall development percentage remains **unmeasured**
until an effort-weighted scope audit. Windows build #634
passed 130/130 CTests at an earlier pinned commit; newer
source requires a fresh consolidated test run, and original
DOS runtime/frame/timing equivalence remains to be proved.


## Latest original VGA sprite machine-state and asset-reader bridge

The [sprite machine-state checkpoint](ORIGINAL_SPRITE_MACHINE_BINDING_CHECKPOINT_20261008.md)
extends the completed original `2A380/2A584` pixel-copy
models to their missing DOS-observable side effects: exact
32-bit shader-pointer writes to both original executable
operands, the persistent VGA sequencer plane-mask register
(index `02h` at `03C4h`), and transparent-row global
`F009`. The shaded patch occurs before sprite-memory
reader callbacks, matching original `2A590/2A595` order.

`CombatOriginalSpriteRasterBinder` now links the existing
original normal/shaded sprite dispatcher to **actual planar
VGA writes** using injected **original DOS resource/shader
pointer readers**, with `2E758/F0C8` callback refresh
after rasterization. This binding is shared engine code:
no Level 22-only geometry, invented sprite bytes or
arithmetic tint rules were inserted.

Five newly added original-byte signatures matched CODE.2.
The audit now holds **653 named checks at 645 distinct
instruction sites**. Focused strict C++17 Debug/Release/
UBSan tests passed for matching source blobs, machine-state
patches against the original owned CODE.2, pixel writes,
callback sequencing and resource capture boundaries.
**No new full Windows build is claimed.** The last
green 130/130 Windows CTest belongs to the prior
pinned build source. DOS reference-screen compositor,
actual memory-resource provider and real-game rendering
acceptance remain open; faithful-demo progress is
**20% (1/5 major gates)**.


## Latest original sprite raster checkpoint — real four-plane VGA writes

The [2A380/2A584 raster source checkpoint](LEVEL22_CODE2_SPRITE_RASTER_CHECKPOINT.md)
extends the original `291B0 -> 2939C/293EC -> 2A380/2A584`
path beyond callback planning: the new native source **writes actual
opaque and transparent sprite bytes into `VgaPlanarMemory`** using
the original frame header, width/height, four-plane write mask,
`EFF9` row pitch, transparent sentinel and DWORD frame addressing.

A key disassembly correction: shaded `2A584` performs a lookup
through an **original 256-entry DOS byte table**, whose 32-bit
pointer is self-patched into two CODE.2 instructions. It does not
perform a generic tint or arithmetic subtraction. The model uses
only caller-captured table bytes, preserves sentinel checks before
lookup, and reports the code-patch pointer for future live-state
integration. Malformed inputs fail closed rather than inventing pixels.

The original-byte audit now includes **648 named instruction checks
at 640 distinct logical code sites**. All **71 new** normal/shaded
raster checks matched the supplied original `CODE.2`.
The **exact committed implementation source** passed isolated strict
C++17 Debug, Release and UBSan harness tests with a compatible
VGA memory interface, including all 320 starting columns. These
commits have **NOT** received a new full Windows CMake/CTest build.
The previous green 130/130 Windows run #634 applies to its
earlier pinned source SHA only.

The current blocker is **live DOS sprite/resource/shader memory
binding, original VGA sequencer and self-modifying-code state,
`1A8C8` composition, and pixel/timing comparison against DOS**.
Until then this is real source-backed component rasterization,
not a certified end-to-end playable HUD. Overall fidelity-demo
acceptance remains **20% (1/5 major gates)**.


## Current DOS message renderer frontier — exact glyph and sprite callbacks

The [303C8/2939C/293EC original renderer checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
adds two more source-backed native execution layers after the
verified **130/130 Windows CTests at earlier pinned SHA**:

- `3060C -> 303C8` now uses the original `2F29C`
  font selection, signed DOS character indexes, per-glyph
  `F56C` normal/shaded choice, re-read font pointer `2B67C`,
  unsigned width byte and wrapping `2B680` column advancement.
  A `bind_display_3060c` adapter connects this sequence
  to the existing original IRQ-timed `1C258` callback driver.
- `2939C/293EC` now dispatch to the original normal/shaded
  sprite-raster callbacks with null-resource early returns
  and observe the optional `2E758/F0C8` refresh **after**
  rasterization, including callback-mutated F0C8 changes.

The immutable-code ledger contains **557 named x86 byte
checks at 549 distinct offsets**, a direct Strength data
reference, nine SHA256-checked DGROUP spans and one hashed
CS span. All **48 new original instruction sites matched**
the supplied `CODE.2`. Focused local source-equivalent
strict C++17 Debug, Release and UBSan tests passed.
These new CMake tests are **NOT** included in the older
green 130/130 Windows runner result.

Actual `2A380/2A584` planar sprite pixels, `2E758`
refresh effects and `1A8C8` composition remain to be
implemented and compared with DOS reference frames.
No Windows font API or approximate indexed-font drawing
has been silently substituted. Overall faithful-gameplay
acceptance remains **20% (1/5 gates)**.


## Latest verified Windows build — 130/130 CTests passed

The actual [Windows x64 GitHub Actions run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
**passed compilation, all 130 CTests, Windows EXE artifact
upload, and committed the refreshed EXE**.
The executable SHA-256 is
`09a3ce4cb5e5d804eee4647413915be7c871789f22cf26e72f13e3f71ff59b35`.
Initial run #632 revealed three outdated regression-test
compilation errors; #633 compiled but identified one
test fixture incompatible with the original `0x7240`
actor mask. Both were corrected without altering DOS
gameplay rules. Full chronology:
[2026-10-08 build and renderer checkpoint](WINDOWS_BUILD_AND_IRQ_RENDERER_CHECKPOINT_20261008.md).

**Source after this verified Windows run:** added an
independently tested original `1C258/1C2C4` renderer
callback execution bridge and `291B0` VGA-page calculation.
The bridge executes the original collaborators in temporal
order with a post-render IRQ observation, but does **not**
implement their underlying image/text/GDI/VGA pixel effects.
It adds 22 verified executable opcode anchors, bringing
the audit to **509 named checks / 501 distinct addresses**.
The newly added CTest has strict isolated C++17
Debug/Release/UBSan validation, **not** a second complete
Windows build. The green 130/130 result belongs to
the precise earlier source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
Faithful-demo DOS gameplay acceptance remains **20% (1/5)**.
GitHub Actions is restored to manual-only operation.


## Latest source checkpoint — DDA4 level-up text and IRQ display lifecycle

The [3210F / 1C258 / 1C2C4 checkpoint](LEVEL22_DDA4_MESSAGE_FORMAT_DISPLAY_CHECKPOINT.md)
closes the **original level-up message source**: CODE.2 DGROUP
+67E4 points to the verified format `%s gains a level.` at +548.
The original 3210F wrapper writes its final NUL in buffer
+759C and 1C21C copies it to the timed queue +14580.
A new read-only C++ model preserves the actor name's DOS
bytes, destination-buffer tail and unsupported out-of-bounds
source cases.

The original `1C258` display and `1C2C4` expiration sequence
now has a separately tested **ordered event model**. It records
the original render-service calls, the pending-flag clear
**after** drawing, and the later original IRQ observation.
An expired message may be displayed and erased in the same
call. No renderer, real-time clock or global state mutation
is substituted for the verified original calls.

Original executable audit: **487 named x86 byte checks at
479 distinct logical instruction addresses**, one direct
Strength table, nine hashed DGROUP data spans and one hashed
CS data span. The new 30 opcodes and two original format/data
hashes passed. The exact committed model sources and headers
passed focused strict Debug, optimized Release and UBSan
compilation/execution using original CODE.2. **No complete
repository CMake/CTest or Windows EXE build was executed**.
The five-main-gate faithful demo acceptance remains **20%**.


## Current original DDA4 callback checkpoint — spell state and IRQ queue

The [3DE4/3EA4/1C21C checkpoint](LEVEL22_DDA4_SPELL_IRQ_CHECKPOINT.md)
adds source-backed, **read-only** C++ projections for the two
different original level-dependent spell-state byte sweeps and
the independently timed `1C21C` message enqueue. All three
original spell/count tables are read from an owned CODE.2 and
fingerprint-checked; original copy/expiry behavior uses a
32-bit IRQ-tick observation and the exact +0x5A deadline,
without a substituted host clock.

The original-code ledger now contains **457 named opcode
checks, 449 distinct instruction addresses**, one directly
verified Strength table, seven hashed DGROUP tables and one
hashed CS jump table. The 50 new source-byte signatures and
three original table hashes passed. A separate strict C++17
Debug/Release/UBSan source-equivalent differential verifier
passed 960 original-table-driven actor-state cases and IRQ
deadline checks; the **full checked-in CMake test suite and
Windows application have NOT been rebuilt**.

These models do not yet execute the original live DDA4
level-up sequence, which additionally requires actual
3210F formatted output and 1C258..1C326 message
presentation/expiry behavior. Faithful-demo release
acceptance remains **20% (1/5 major gates)**.


## New DC7C level-up HP reconstruction — original CODE.2

The [DC7C HP checkpoint](LEVEL22_DC7C_HP_PROGRESSION_CHECKPOINT.md)
now includes a read-only source reconstruction of original
single-die class HP rolls, the two 19-byte signed Constitution
adjustment banks, all six CS jump-table branches, original
level-10/11 caps, the **clamp-before-multiclass** rule, one
30CA2 RNG draw per call and 16-bit current/max HP wrapping.
The tables are loaded from a user-owned `CODE.2` image
and fingerprint-checked, not embedded or redistributed.

The original-byte audit has grown to **407 named checks,
399 distinct logical instruction addresses**, plus one direct
Strength table, four hashed DGROUP tables and one hashed CS
jump table. The exact committed read-only model and regression
passed isolated strict C++17 Debug and Release tests against
the original CODE.2 data; the original-backed test caught
and corrected a mistaken fixture value for Constitution 15
before final validation.

**No live XP/HP changes, no original-timing emulation, and no
rebuilt Windows EXE are claimed.** The `3EA4`/`3DE4`
class services and `1C21C` IRQ message side effects must be
preserved before the full DDA4 sequence can mutate state.
Overall faithful-demo acceptance remains **20% (1/5)**.


## Latest DDA4 callback control — original 50DC and timed service sequence

The [50DC / DDA4 service-order checkpoint](LEVEL22_DDA4_50DC_SERVICE_ORDER_CHECKPOINT.md)
adds independently audited, read-only models of the original
class-selection predicate and its call sequence after a class
level changes. The original 50DC selector uses the status
mask `0xF240`, prefers class code `3` in zero mode,
has a class-`1` level-9 exception, and searches code `4`
in nonzero mode. A nonnegative result optionally calls
`3EA4` or `3DE4`. **Even with a negative result**, the
original still calls presentation `3210F`, timed message
`1C21C`, and RNG-consuming HP service `DC7C` in order.

The original executable audit now has **378 named byte
checks covering 370 distinct instruction offsets**, plus one
direct and two hashed original data references. The exact committed focused C++ sources/tests
passed isolated strict C++17 Debug and Release harnesses;
registered full CMake/Windows tests remain **unexecuted**.
This does **not** apply live levels, HP, XP, combat statistics
or IRQ timers. Full original game-behavior acceptance
remains **20% (1/5 major demo gates)**.


## Latest preservation checkpoint — original class levels and attack target

The [DDA4/DBD4 source checkpoint](LEVEL22_DDA4_THRESHOLDS_DBD4_CHECKPOINT.md)
adds bounded C++ models for six original class tables (20 XP thresholds each)
and the original class-specific DBD4 attack-target coefficients.
All source tables are loaded from the user-owned CODE.2 executable,
fingerprint checked and never embedded/distributed as asset dumps.
The CODE.2 verifier now has **347 original instruction signatures,
one direct Strength data anchor and two hashed original data tables**.
An isolated strict C++17 core test passed against original CODE.2 in
Debug and Release; the full Windows game has **not** been rebuilt.

Importantly, original level changes invoke 50DC/3EA4/3DE4/3210F/1C21C/DC7C
before DBD4 recalculates the attack target. These side effects are
**not** currently simulated and the new models are read-only.
Native XP, level-up and party-stat mutation remain gated on full
original instruction-order validation. Overall release acceptance
remains **20% (1/5 major gates)**.


## DDA4 original experience distribution — next source checkpoint

The [DDA4 experience checkpoint](LEVEL22_DDA4_EXPERIENCE_CHECKPOINT.md)
reconstructs eligible-party count (original status mask 0xF240),
rounded per-member experience, three class-track count/rounding,
minimum-one award rules and 32-bit class XP wrap. It returns a
**read-only projection**; level-up threshold scans, class-change
callbacks, and recalculated attack targets are NOT implemented.
The original-byte verifier now contains **318 code signatures plus one
Strength table**. A source-equivalent isolated C++17 test passed,
but registered CMake/Windows tests and end-to-end demo acceptance
have not been rerun. **Overall demo: 20% (1/5 gates).**


## New original-file result: Level 22 cannot invoke E0F8 for the Assassin

**The E284 `0x258AA` tracker is resolved for the supplied original Level 22
archive and mutable save.** `LEVELS.DAT` level 22 and `LEV022.DAT`
both store signed creature-type filters **(-1,-1,-1)** at fixed level-header
offsets `0x292/0x294/0x296`. Original CODE.2's `E50F..E539` scan
sign-extends each filter and compares against the zero-extended NPC type,
so Assassin type `9` cannot call `E0F8`. It reaches the original
corpse-marker branch **without E0F8 RNG draws or timer reseeding**.

The [updated E0F8 checkpoint](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md)
records original binary/hash verification and this important refinement
to the earlier research. `LevelState::respawn_npc_types()` now exposes
the actual saved header values; the Level22 encounter and its live
`ExplorationRuntime` caller pass the real level state, reject any
unsupported matching tracker before modifying the world, and use the
original no-match corpse-marker model. The Level22 C++ regression
fixtures now cover signed header parsing and matched-tracker rejection.
This is committed **source only**; no consolidated native/Windows tests
or updated EXE are claimed. Complete combat/presentation/timing
acceptance remains open and the overall five-gate demo stays at **20%**.


## Original E0F8 respawn source-model checkpoint

The [E0F8 source checkpoint](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md)
adds a **read-only** reconstruction of the original IRQ-tick/290D0 BIOS
fallback RNG reseed, two 30CA2 draws per candidate, low-byte map
obstruction, two separate 15-cell party-distance tests, live NPC cell
occupancy, three-attempt limit and exact 22-byte slot/HP mutation plan.
Its local isolated C++17 tests passed with Release assertions enabled.
The CMake test target is registered but the current branch has **not**
received a consolidated Windows build. On the original 30x15 Level 22
map the routine cannot find a candidate at least 15 y-cells from an
in-bounds party, but a *real invocation still consumes six RNG draws*;
the actual 258AA tracker and original IRQ observations remain unknown.
The offline verifier now has **293 CODE.2 instruction signatures plus one
Strength data table**. This **does not** connect E0F8 to live combat,
change shipped behavior or advance overall demo acceptance (20%, 1/5).


## Project-wide timing fidelity — mandatory

**Walking pace, turn response, animation frame dwell, card flips, combat
recovery, NPC reactions and story transitions must match the original game's
timekeeping**, not merely its final graphics or state. See the
[Timing Fidelity Contract](TIMING_FIDELITY_CONTRACT.md), grounded in
[Preservation Archive Chapter 17](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md).

The original main loop has an anchored two-game-tick target
(`201A7..2022B`, `2E608/2E614`). This must be reconciled with the native
host's nominal 70.0863-Hz **display** refresh; the two are not interchangeable.
No universal milliseconds-per-tick claim is made without measured original
runtime evidence. Every new gameplay subsystem requires explicit timing,
interruptibility, audio/animation ordering and regression acceptance.
Overall five-major-gate demo completion remains **20%**; cadence equality is
not yet certified.

## Private-repository development and build policy

**Automatic GitHub Actions Windows builds are disabled on `demo-v1`.**
`.github/workflows/demo-v1-build.yml` has **manual `workflow_dispatch`
only**, to conserve private-repository Actions minutes. Continue committing
faithful gameplay reconstruction, regression tests and archival research
without repeatedly building preview binaries. Re-enable push builds only
after explicitly deciding to do so when the repository becomes public.

The **last previously tested Windows executable** is from
[build #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
(102/102 CTests), but it **predates** the recently verified correction
of the hand readiness offset and the new combat-entry/target-selection
source. Do not treat its tests as validation of current HEAD.

The current source-only work includes
[original attack/target preflight and offline verification](LEVEL22_COMBAT_ENTRY_CHECKPOINT.md).
An original-asset offline byte audit now covers **273 original CODE.2
machine-code anchors plus one signed Strength data table**. Seventeen combat C++ test targets are registered,
but **have not been executed** since the private-repository build pause.
The [conditional 24478 hand-eligibility model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
preserves the original requirement for observed DOS heap-adjacent
bytes; it is not yet connected to live attacks.
This deliberate pause does not alter the **overall faithful-demo
acceptance: 20% (1/5 release gates)**.


## Playable Level 22 walkaround preview (opt-in)

**Windows x64:** use the newly added `Explore-Level22.cmd` packaged next
to `ravenloft_demo_v1.exe` in the latest Windows Actions artifact. It
loads the actual Level 22 without requiring the intro or character-creation
sequence. If CHARS is present it is used read-only; otherwise the preview
uses temporary in-memory EXPLORER/GUIDE characters. Your installed game files
are not saved or modified by this mode.

Use arrow keys or the numeric keypad to move and turn; press Esc to exit.
Alternatively run `ravenloft_demo_v1.exe --explore-level22` (optionally
followed by a quoted installation path). The normal no-flag boot flow is
unchanged. The original world/environment is loaded, but incomplete
main-wall rasterization can leave some foliage/walls/scene pixels absent.
The preview is not a full gameplay demo.

[Walkaround instructions, implementation and acceptance checklist](EXPLORATION_WALKAROUND_CHECKPOINT.md).
**Initial walkaround checkpoint: 100% (5/5 bounded gates)**:
[Windows build #602](https://github.com/Hyperbog/RLOFT/actions/runs/37716513471)
passed 101/101 CTests and published the direct-entry EXE with a launcher.
User-provided Windows screenshots subsequently confirmed original-asset outdoor
scenery, HUD, expanded top menu and automap. These captures also show an
unresolved black-background character panel and questionable automap/modal
background isolation. Locomotion fidelity, wall rasterization, complete
gameplay and original DOS indexed-frame equality remain unverified.
Overall faithful-demo acceptance is still **20% (1/5 major release gates)**.


Goal: reproduce the original shipped behavior from boot through character creation while reading assets from a legally installed/owned copy of *Ravenloft: Strahd's Possession*.

## E284 NPC death and event follow-on — type-specific source checkpoint

The [E284 death-follow-on checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md)
now reconstructs the **positive-HP versus corpse paths**, the
distinct original presentation-resource expressions for injury
and death, and the ordered special-death branches for NPC types
`0x08/0x09/0x12/0x18/0x21/0x50`. Level 22's Assassin
type `0x09` now uses the audited `E284` four-drop ID and
placement-seed plan (`88/6E,87/78,E9/82,74/8C`), removing
a second hardcoded death-spawn loop.

The original `E0F8` respawn routine is now **instruction-audited**:
it samples the independent `30700` IRQ tick, draws two `30CA2`
values for each of up to three placement attempts, checks map
collision/proximity, and can roll new NPC HP on success.
This source evidence **does not imply native E0F8 integration**.

The `0x258AA` three-entry signed type tracker and `E0F8`
respawn callback are modeled **only with observed caller inputs**;
type `0x18` still requires external condition/RNG evidence.
Original global story `BF44`, presentation/audio `38110`,
and `1DDA4` award consequences are not yet fully connected.
**32 original E284 death-branch opcodes** and **15 subsequent E0F8
respawn-body opcodes** have been verified, bringing the cumulative
offline verifier to **273 instructions plus one data table**.
The tests are source-registered and not yet compiled in Windows.

## Recovered Strength table and HARM/E284 event ordering

The [Strength, HARM and E284 classification checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md)
recovers the **exact signed DGROUP:684B modifiers for scores 0–25**
from immutable CODE.2 object-2 data. The original modifier for
Strength 18 without a qualifying exceptional-strength class is
**+2**, and the E5F0 percentile branch remains separate.
Unsupported scores 26–255 require actual adjacent DGROUP
bytes because the executable performs no index bounds check.

The new `CombatHarmPreludeModel` preserves the original
**1d4 → conditional direct HP clamp → provisional zero damage
→ C910 minimum-one injury → E284 second HP write** sequence.
It does not roll dice or mutate an NPC merely to inspect it.

The shared `E284` injury prelude now also applies the original
`3ACDC` NPC type classification to BYTE `npc+0x0B`, *before*
the HP-positive/death branch. This applies to the Level 22
Assassin on both nonlethal and lethal damage and preserves the
existing four-quest-item drop handling. The source-only offline
audit is now **226 x86 instruction anchors and one 26-byte
DGROUP data anchor**. No new Actions build has been run.

## Original 22E40, Rod and E284 injury reconstruction

The [class-level, Rod and E284 injury checkpoint](LEVEL22_CLASS_LEVEL_ROD_INJURY_CHECKPOINT.md)
closes three additional original combat source dependencies:

- **Vampiric Touch `0x7F`:** `22E40` returns the signed
  level of the *first matching class-4 lane* (`+0x15..+0x17`,
  levels `+0x20..+0x24`), defaulting to 1 when absent.
  The damage planner now derives its d6 count directly from
  the original `PartyImage`.
- **Rod of Smiting `0x4A`:** `E5F0` compares the Boolean
  return of `C80C` to 20, so its apparent charge-consuming
  critical path is **unreachable through that normal caller**.
  NPC types `0x17/0x1F` reach an extra `2d8+6 per die`
  roll; other types retain initial damage.
- **NPC injury `E284`:** preserves original reaction flags,
  type-`0x0F` subtraction exemption, type-`0x08` HP floor,
  and signed/word-wrap arithmetic. The existing Level 22
  Assassin type-`0x09` damage path now shares this original
  prelude before its established XP, death sound, and four drops.

**39 additional immutable CODE.2 opcode signatures** were checked
locally, bringing the audit to **212**. The source and CMake targets are committed but have not received
a new Windows build. The normal-score **DGROUP:684B Strength
table has since been recovered**; full E284 creature death effects,
actual RNG/tick traces and playable attack input remain open.

## E5F0 damage dice and C910 weapon restrictions — source checkpoint

The [damage-dice and creature-gate checkpoint](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md)
now reconstructs the ordinary original `OBJECTS.DAT` dice parameters
(`+0x38` count, `+0x39` sides, `+0x3A` bonus per die), the **actual
nine-entry `E5CC` temporary spell-hand jump table**, exceptional-strength
modifier thresholds, and the `C910` creature/weapon damage gate.

The model handles HARM's special pre-gate NPC-HP-clamp decision
without applying it prematurely, and distinguishes Rod of Smiting's
separate `0x4A` charge/type branch. Character modifiers that use
the **original DGROUP `0x684B` Strength modifiers for scores 0–25**
have since been recovered byte-for-byte from CODE.2 object 2;
Vampiric Touch's `22E40` class-level helper is also reconstructed.

The new models **do not roll dice, change NPC health, change hand
recovery timers, or launch an animation**. Three new C++ test targets
are registered for later intentional verification; the original
that earlier source stage added 47 verified signatures (then 173);
the current audit has **226 instruction anchors plus one data-table anchor**.
Overall faithful-demo acceptance remains **20% (1/5 major gates)**.

## Pre-attack status service E578 — original actor effects

The original CODE.2 logical `E578` executes **after initial target
selection but before target validation and the hit roll**. It clears
a pending `+0x132` effect bit for all four actors, then clears
selected-actor `+0x133` bit02h and its `+0x15A` state, marking
the original `CF5A` HUD redraw flags. The source model
`CombatAttackStatusService` and synthetic regression are committed,
but are not yet wired into live attacks or compiled under deferred CI.
See the [E5F0 timing and E578 checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md).

## Original E5F0 target selection and attack timing — source checkpoint

[Attack target and timing checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md)
now reconstructs the original explicit-target-vs-`DB94` preference
order, special rear-party `+0x35 & 3 == 2` restriction and action
recovery based on the **full original OBJECTS WORD +0x35 bits 2..5**.
The result is **9 × ((word >> 2) & 15)** gameplay maintenance ticks
for a regular item, or 18 for an empty/pseudo hand, optionally halved
by the original actor effect mask. On a successful hit the original
`E5F0` stores **A7E4 + half the interval** as the *hit-only*
hand-ready deadline and records the resulting damage. Its next call,
original CODE.2 logical **`24B48`**, presents the hand action and,
when the interval is nonzero, stores a **second, miss-inclusive**
action-busy deadline using the **later A7E4 reading + full interval**.
The two dwords and clock observations cannot be merged.

The new native routines are **side-effect-free**, consume the
**captured dispatched item ID** as original E5F0 does, and do not
change the character state, choose invented damage or run animations.
Timing may not be equated with VGA refresh or host milliseconds.
Both new C++ regression targets await a deliberate consolidated build;
21 additional original executable signatures passed local byte audit.

**Bounded E5F0 target/timing source checkpoint: 67% (4/6)**.
Overall faithful-demo acceptance remains **20% (1/5 release gates)**.

## Full 24478 pair decision — conditionally reconstructed

The [raw-heap-conditioned original hand decision model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
now accounts for ordinary, pseudo, both-empty and mixed negative-item
`24478` paths **as a side-effect-free candidate calculation**.
A negative item reads raw bytes before the original allocator base;
the model demands the actual observed original category alone for
both-empty, or category/weight/qualifier for a mixed pair.
This does not make unarmed attacks playable: raw DOS heap values,
attack/use event ordering, hit/damage, audio and recovery timing are
unverified.

**Bounded original pair-decision source checkpoint: 67% (4/6 gates)**.
Overall faithful-demo release acceptance remains **20%**.

## Original 24478 empty/pseudo short circuits

The [empty/pseudo early-gate checkpoint](LEVEL22_HAND_PAIR_EARLY_GATES_CHECKPOINT.md)
models original `244CE..246E9` and distinguishes its first-actor
`status >= 0x4000` threshold from `240A8`'s **separate**
`status == 0x8000` branch. The `24478` pair evaluator now
supports **both hands empty**, but only with the original observed
OBJECTS pre-base category byte, or **both hands holding pseudo
spell items `0x7E..0x86`**; both shortcut directly to their distinct
action-state busy checks. Mixed **positive pseudo+ordinary** pairs follow the normal
qualifier/weight arbiter after pseudo overrides. A mixed `-1`
with any nonempty hand can also be **modeled** when its actual
original allocator-adjacent category, weight and qualifier bytes
are supplied explicitly; absent evidence is rejected.
[Original heap-conditional pair checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
documents the exact three byte offsets. The authentic DOS heap values
and temporal combat execution remain outstanding. No attack is enabled and current source
has not been compiled since private CI was paused.

## Original 24478 ordinary two-hand arbitration

The [ordinary two-hand arbitration checkpoint](LEVEL22_HAND_PAIR_ARBITRATION_CHECKPOINT.md)
originally modeled CODE.2 `24478` for **two ordinary positive equipped items**; the later [conditional full pair model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md) now extends that source to the original pseudo and `-1` branches,
using the original independent `240A8` eligibility/classification results,
literal OBJECTS `+0x35 & 3` and `+0x30` weight fields, character class
groups, `23EE4` pair exemptions, and each hand's `+0x10B/+0x10F`
action-state busy gate. The source/tests are registered for an eventual
manual build; **there is no newly verified Windows binary or playable attack**.

The original loader at `268C8` allocates OBJECTS through a wrapper
returning a pointer 10 bytes into an allocation. `240A8(-1)` reads the
pre-table byte `OBJECTS_POINTER-0x12`, *outside* the allocated object
record and even before the low-level allocation base. The helper can
model the empty-hand decision only when that actual raw byte is supplied.
The full pair model now handles `-1` and pseudo items **conditional
on all raw original bytes actually read by the selected path**.
Original heap behavior and game-time action pacing remain separate
research gates.

**Bounded ordinary pair checkpoint: 60% (3/5 gates)**. Overall faithful
demo release acceptance remains **20% (1/5 gates)**.

## Original 240A8 candidate and 24478 hand legality — source-only progress

The latest [single-hand eligibility checkpoint](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md)
implements original `240A8` for **nonnegative object IDs** and permits the original
empty-hand (`-1`) branch **only with the true OBJECTS pre-base category byte**,
including its
`33FC4/34060` exception sets, signed class and level gates, two original
spell-ID intervals, party-mode rules and the literal OBJECTS `+0x35 & 3`
qualifier. Its regression target is registered but not yet compiled.

The source deliberately **does not invent a default `240A8(-1)` unarmed category**:
the original helper reads memory before the allocated OBJECTS buffer.
The original loader's pointer arithmetic has now been recovered, but
the allocator-adjacent **runtime byte value** is still unknown. Conditional
`24478` arbitration now supports `-1` and pseudo hands; **capturing the real
original heap values and connecting timed attack input** remain prerequisites
to live attacks.
The [offline original-byte audit](tools/verify_combat_entry_bytes.py)
had reached **87 opcode anchors at that earlier checkpoint**; the [current verifier](tools/verify_combat_entry_bytes.py) contains **273**, including the 15 then-newly audited `24478` empty/pseudo short-circuit instructions (both original party-status checks remain distinct) matched
against original CODE.2. This is source evidence, not a new game build.

**Bounded nonnegative `240A8` checkpoint: 75%** (compiled/original-runtime
comparison pending). **Overall faithful-demo release acceptance: 20%**.

## Level 22 live HUD item handling (original CODE.2 214B8 / 211D8)

The [live HUD hand and actor-panel checkpoint](LEVEL22_HAND_INTERACTION_CHECKPOINT.md)
extends ordinary full-inventory swaps into the playable world UI. Supported
normal-gameplay **right-click hand tiles** exchange their held items, preserving
item identity, the identified bit and the original selected-hand ready-deadline
reset. A **right-click on the actor inventory panel** places an ordinary
held item in the actor's first free ordinary slot without opening the modal.
An inventory-full result leaves the item safely on the cursor. These native
right-click bindings are provisional pending DOS event-flag trace confirmation.

The original `0F238` known-bit early return and `211D8` direct-stash
known-bit non-update are preserved rather than replaced by generic inventory
semantics. Special equipment, container and spell-hand branches remain
explicitly unsupported. The original `214B8` separate use/attack path is
not yet enabled: original target selection, damage, cooldown and feedback
must be reconstructed first.

**Hand interaction checkpoint: 60% (3/5 gates) on current source**,
pending a deferred manual CI rerun and installed-game playtest. Historical
[Windows CI run #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
passed 102/102 for the *earlier binary*, but did not validate the later
`+0x19E` readiness offset correction. Overall faithful demo: **20%**.

## Level 22 functional gameplay integration (ordinary inventory)

The next [inventory interaction checkpoint](LEVEL22_INVENTORY_INTERACTION_CHECKPOINT.md)
adds **original CODE.2 12E70 ordinary 3x4 slot clicks** and the bounded
`117A0` held-item/identified-bit exchange to the existing four-party inventory
modal. Players can now take a previously picked-up loose item from the held
cursor into an empty normal inventory slot, freeing the cursor for another
pickup. The model tests also cover the full sequence of three Assassin relic
pickups/stashes followed by the fourth item completing the original Level 22
possession predicate.

Temporary spell-hand objects, occupied container insertion, equipment and
hand action rules remain gated. Combat and the 22-frame Level 23 transition
are not yet enabled. Automated Windows compilation and an original-asset
inventory interaction playtest remain separate acceptance gates.

**Inventory subcheckpoint: 83% (5/6 bounded gates)** after Windows 102/102 CTests; real gameplay interaction remains unverified.
**Overall faithful-demo release acceptance: 20% (1/5 gates)**.

## Level 22 combat accuracy — original hit test

The separate [CODE.2 C80C combat hit-check checkpoint](LEVEL22_COMBAT_HIT_CHECKPOINT.md)
implements the original ordered player-to-NPC threshold calculation as a
bounded, deterministic native primitive: held-target auto-hit, weapon and
Strength adjustments, burden, Slow, Bless/Aid, Stone Giant Gauntlets and
the original **modified-d20** automatic-hit/miss rules. Synthetic tests
are registered in CMake, but the input command, hit target, RNG, damage,
weapon category gate, NPC reaction and hand cooldown have **not** been
connected. Attacks are intentionally not dispatched by arbitrary clicks.

**Hit-check arithmetic subcheckpoint: 100% (3/3 bounded gates)** after
Windows 102/102 CTests; full combat is still unimplemented and overall
faithful-demo acceptance remains 20%.

## Extended integration milestone — Level 22

The demo's original boot/character-creation milestone has been extended to
**all systems integrated for the Level 22 Assassin/relic sequence**.
The earlier boot/controller documentation below remains valid but no longer
describes the full current scope.

The original SINTAB 127-block rasterizer interpreter, original RES3 type-7
pixel execution path, and whole-scene VGA trace integrity are documented in
[TYPE7_RENDERER_CHECKPOINT.md](TYPE7_RENDERER_CHECKPOINT.md).
The separate original CODE.2 surface-prepass investigation and its
signed/unsigned SINTAB underflow boundary are tracked in
[SURFACE_PREPASS_CHECKPOINT.md](SURFACE_PREPASS_CHECKPOINT.md).
A **transactional SINTAB pixel executor is now connected to supported
RES4 surface prepass runs**, with per-run diagnostics and pending coverage.
Ordinary SINTAB main-pass wall rendering and DOS-frame equivalence remain open.
Viewport-selected Assassin damage and loose-item pickup bindings, including
snapshot-invalidating world state, are documented in
[LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md](LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md).

**Do not label the demo finished yet.** Ordinary SINTAB wall rendering, final
original-frame equivalence, remaining host combat/inventory-input bindings,
and relic-transition integration remain open.
Development commits continue on `demo-v1`; the Windows CI workflow handles
native-source changes while Linux is manual opt-in.


## Certified preservation reference

The complete original-engine preservation documentation is mirrored as
[PreservationArchive/](../PreservationArchive/00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md)
from the pinned `code1-decompilation` source, including the 22 completed
static chapters and the separate 91-task future research register. See
[PRESERVATION_INTEGRATION_ROADMAP.md](PRESERVATION_INTEGRATION_ROADMAP.md)
for the original-code-to-native source map, open acceptance tests, and the
resolved two-routine CODE.2 reconstruction reconciliation across branches.
Both branches now share the original-byte-corrected `Code2/Code2.cpp` blob
`87f6638921c53a32bde59735515fbc175e2c96ee`; see
[the recorded CODE.2 errata](../Code2/POST_CERTIFICATION_ERRATA.md).
The historical 2026-10-05 source-certification hashes remain provenance
for the pre-correction revision, not the present source.

The native demo is a **separate** workstream. GitHub Actions run #572 passed
full Linux and Windows compilation/tests (99/99 Linux tests; Windows
executable published), but original-frame equivalence and the full Level 22
Assassin/relic/transition gameplay remain unverified. Automatic Linux jobs
are intentionally paused to save Actions minutes; Windows builds and tests
remain available for native changes.

## Asset policy

The demo does **not** bundle Ravenloft game data.

Normal use is:

1. locate an installed Ravenloft copy automatically;
2. identify the original media source;
3. read/extract only the files required by the native runtime;
4. preserve original resource IDs, palette indices, timing, and data semantics.

A pre-extracted resource folder remains supported for development and preservation testing, but it is not the intended end-user requirement.

## Installed-copy discovery

The native source layer currently supports:

- the GOG Ravenloft product registry entry (product ID `1432905053`) on Windows;
- `RLOFT_GAME_DIR` as an explicit override;
- common GOG Games / GOG Galaxy roots;
- common Steam roots;
- a directly supplied game installation directory;
- a directly supplied `game.gog` path;
- a loose directory containing Ravenloft `RES0`, `INIT`, and `T`.

Candidate installations are accepted only when Ravenloft-specific media anchors are present.

## game.gog

GOG's installed CD image can be consumed directly. The demo has a small read-only ISO-9660 reader and does not require 7-Zip or a manual extraction step.

Supported data-sector layouts:

- 2048-byte ISO sectors;
- 2352-byte raw Mode-1 sectors with 2048-byte user data;
- 2352-byte sectors whose ISO user data begins at offset 24.

ISO version suffixes such as `;1` are normalized during lookup. The indexer walks subdirectories so resource files do not need to be at the image root.

## Runtime-side files

For files that belong to the writable installed game rather than the CD resource image, lookup checks both:

- the installation root;
- the installation root's `RLOFT` directory.

This keeps the original distinction between installed runtime state and CD/resource media available to later compatibility work.

## Current boot milestone

The binary-proven transition implemented so far is:

```text
R.EXE supervisor
  -> CODE.2 startup
  -> CHARS-absent cold-start presentation
  -> CODE.2 four-entry startup menu
  -> menu index 2
  -> legacy result 10
  -> R.EXE launches CODE.1
  -> character-creation workflow
```

The native RES0/COLORS cold-start renderer and startup-menu presentation are implemented. The Windows host also composites the original Ravenloft cursor from the selected installation's RES0 data rather than using a host-system pointer.


## CODE.1 character-creation boundary

Direct re-disassembly of the certified CODE.1 object closes the first three
controller states:

```text
state 0  0x1CD0  choose one of the two original player-character slots
state 1  0x1DD8  sex selection
                  result 3 -> sex 0 / MALE
                  result 4 -> sex 1 / FEMALE
state 2  0x1E64  race selection
                  results 1..6 -> race codes 0..5
```

The state-0 presentation is now rendered from the installed game data.
CODE.1 loads COLORS palette 8 before the controller. Its first screen uses
RES7 graphics, including the two vertically emitted background strips at
RES7:24/25, the fixed RES7:30..38 control family, RES7:42, and the two
state-0 selector graphics.

The observable CODE.1 opening presentation at `0x45FC..0x4976` is implemented,
including its 70 displayed frames, palette reveal, opening music ramp, and
RES6 one-shot sound cues. The reconstructed controller then continues through
the character-creation workflow, its transition animations, stat generation
and editing, portrait selection, naming, final acceptance, CHARS serialization,
and the status-10 supervisor handoff back to CODE.2.

For Demo v1's requested cold-start-to-character-creation slice, the remaining
unimplemented CODE.2 behavior is outside the target path: the CHARS-present
startup/import branch and the full visible behavior of startup-menu entries 0
and 1.


## New original 2AB38 planar VGA pixel-address checkpoint

The [glyph/sprite rendering checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
also now includes a source-backed `2AB38` address/plane model:
the original combines two **mutable** row-shift globals
`EFED/EFF1`, unsigned `column>>2`, `column&3` as
the VGA plane, and the original `F001` linear page base.
It preserves wrapping x86 DWORD arithmetic and never
invents constant screen pitch or a default DOS video page.

**20 additional original signatures passed**, raising
the executable audit to **577 named checks at 569 distinct
addresses**. Local strict C++17 Debug, Release and UBSan
pixel-address model tests passed. This calculation is
registered in the native CMake/CTest source but is
**not included in the prior green Windows 130/130 run**.
The actual original sprite plane writes and display
compositor remain unintegrated; faithful gameplay
acceptance stays **20% (1/5 main gates)**.
