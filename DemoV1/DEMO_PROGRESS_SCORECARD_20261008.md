# Demo v1 — development progress versus formal acceptance

## Current verified build and canonical percentage reporting

**Last fully verified Windows source revision:** `fb4a0145`
([Full build #647](https://github.com/Hyperbog/RLOFT/actions/runs/37987825463)):
**172/172 registered CTests passed**, executable artifact
published and build provenance committed. This is a
Windows/native regression gate, **not original DOS
graphics, timing, event or gameplay equivalence**.
The workflow was restored to **manual dispatch only**.
Subsequent generalization of current-level RES4
terrain-resource binding, its regression, and these
documentation changes have **not been rebuilt**.

**Provisional weighted source-readiness: 48.9%**
across **20 major systems and 122 separately scored
subsystems**. Use the [complete per-subsystem report](
PROGRESS_SUBSYSTEM_REPORT_20261009.md) and [canonical
scores](PROGRESS_SUBSYSTEMS.csv) for all changes.
Source mapping contributes 10%, native code 20%,
live integration 20%, authored regression coverage
10%, and full **current-revision DOS acceptance 40%**.
The DOS gate remains 0% until independently proved
against the original game, even after a green
Windows CTest run.

The Level22 opt-in `--dev-interact` and read-only
`--dev-inspect` options remain separate from
normal cold startup. The new **multi-level original
RES4 ground selector** scores **50.0%**, and the
expanded Level lifecycle/continuity system scores
**37.9%** across seven named subsystems. Windows
build reproducibility advances from 55.0% to
**60.0%** on the verified CI evidence; the
Verification/developer testbed system becomes
**50.0%**. The global weighted readiness is
**48.9%**, compared with 48.8% previously.

The historical separate **32-task engineering ledger
remains 56.25% = 18/32 task-equivalents**.
These two metrics are not directly comparable and
must never be averaged. The CTest catalog holds 172
registered tests; its Quick selection remains 19.
After each relevant code change, the canonical CSV,
the 20 system percentages and the 122 individual
subsystem scores must be kept consistent.

## Level22 dev testbed — read-only integration diagnostics

A dedicated **opt-in** `--explore-level22
--dev-inspect` mode now extends the existing
development preview *window title only*.
`DevLevelDiagnostics` captures stable, separate
64-bit FNV-1a fingerprints of original party
image bytes, cursor/inventory/container state,
live level/world state (NPCs, objects, cell
words, ten-byte control records, animation count),
story event flags and current camera/clock.
It reports counts and diagnostic hex digests,
allowing future level-transition tests to see
whether unrelated character/quest state has
been rebuilt. It writes no original game
state and is prohibited during normal startup
or CODE.1 character creation.

New `dev_level_diagnostics_tests` covers
separately changed domains and restoration to
unchanged digest. **CTest registry: 171**
(previous checkpoint 170), **Quick 19**.
Current source and new assertions remain
uncompiled and unexecuted on Windows.
No build or CI job was invoked.

This is test instrumentation, **not a new
DOS fidelity gate completion**: engineering
score stays **56.25%**, inventory PARTIAL.
The slice still needs end-to-end original
startup acceptance, in-session level transition,
full-world persistence and DOS visual checks.
See [dev testbed acceptance matrix](
DEV_LEVEL_TESTBED_20261009.md).


## Architecture gate — integrated original complete-game path

The demo is the first vertical slice of **one continuous
native Ravenloft game**, never an independently shipped
Level 22 application. Full acceptance requires the original
boot and cinematics, CODE.1 party creation (or valid CHARS),
the shared gameplay runtime, and original level-to-level
transitions preserving all party/world/event/time state.
Original CODE.2 level/event graph, not numerical level
order, controls progression. Developer direct-to-Level22
preview is test-only.

**Existing in source:** cold-start and CHARS-present
paths, character-image transfer, shared exploration entry,
and internal CINE08 event presentation. **Not yet accepted:
general in-session level transition, complete world
SAVE/LOAD, cross-level continuity and end-to-end
start-to-finish original runtime testing.**
No change to engineering **56.25%**, inventory **PARTIAL**,
or user's manual-only build policy.

See [continuous-game integration contract](
CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md).


## Part 8 — Level16 ritual live CINE08 event pipeline

The original fourth-`SEED OF MORNINGLORD`
inventory-drop event (CODE.2 `21880`)
now enters a live native event-8 cinematic
callback. The already reconstructed
world-item `2195C` constructor, persistent
story bit `0x44`, original `CINE08.ANM`
(182 frames) and `15050(0x73)` HOLY
SYMBOL OF RAVENKIND reward are now
integrated in the executable's original
source ordering. Separate preflights
prevent invalid world placement or
reward loss. Completed playback awards
one Holy Symbol, in original
cursor → active-party ordinary slot →
family-C world order, with unidentified
bit and source 300-item capacity.

The Win32 modal now reuses its existing
native cinematic/AUDIO/MIDI player
instances, suspends inventory clicks
during event 8, restores input afterward,
stops cinematic audio and invalidates
stale scene caches. Escaped, Quit, missing
or invalid event resources and callback
exceptions do not mark a quest completed.
Cancellation rolls back the temporary
live seed+flag state as a **native
safety policy**; original `13C88`
DOS teardown/cancellation behavior
remains acceptance-unverified.

Existing regression targets received
source-order, once-only completion and
abort/retry assertions; total
**170 CTests / 19 Quick** unchanged.
No Windows build, CTest execution, or
DOS-vs-native playback comparison was
performed. Engineering tracker
**56.25%**, inventory **PARTIAL**.
Manual Windows builds remain user-controlled.


## Part 8 continuation — full isolated Holy Symbol reward allocation

A new original `15050(0x73)` fallback bound to the
already reconstructed `2195C` world-item constructor
completes the **isolated** HOLY SYMBOL OF RAVENKIND
reward's three allocation routes: empty held cursor,
first free ordinary slot of the first active actor,
and a real unidentified family-C loose-item spawn
when inventory is full. The world fallback uses the
original `2195C` arguments including screen seed
`0x80`, known `0`, forward placement `1`,
and **do-not-clear-cursor `0`**. The source's
300-loose-item eviction and collision/coordinate
fallback rules remain shared with ordinary drops.
An unusable world leaves original cursor/party/
world unchanged and reports a distinct rejection.

The `13EF8(8) → 13AD8 → 139AC` call chain is now
tied to the source's literal `CINE00.ANM` filename
template and **`CINE08.ANM`** resource, which the
original ANM inventory lists as **182 frames**.
The native ANM decoder and startup playback path
exist, but the exploration-modal handoff and its
source-order flag/reward transaction are not
connected. The fourth Seed-of-Morninglord drop
therefore remains **pending**, not a completed
ritual. The new world reward helper is NOT called
in the pending branch.

Expanded assertions cover source reward full-world
placement, held-object preservation, known-byte
zero, eviction, world-failure atomicity and
cursor/party first choices, plus the source movie
identity. No new CTest target; **170 CTests
registered / 19 Quick**. No manual build,
CTest run or live game acceptance was initiated.
Formal inventory **PARTIAL**, engineering
**56.25%**, awaiting event-8 handoff and
remaining DOS fidelity gates.


## Part 8 extended — original Level16 Seed ritual and Holy Symbol grant

The original CODE.2 `21880` Level16
item `0x89` predicate is reconstructed:
the four embedded `207E0` site pairs
(13,19), (11,21), (13,23), (15,21)
each require a family-C loose object
of type 137, using original Q16
coordinate high words. Persistent
story flag 0x44 suppresses repeat
completion. The ordinary drop path
now admits non-completing Level16
seeds, correctly models source
oldest-item eviction at 300 entries,
and refuses only an actual triggering
fourth placement until the authored
follow-on calls are available.

The original follow-on sequence is
verified as `1C124(0x44)`,
numbered interactive event `13EF8(8)`,
and `15050(0x73)` rewarding
the HOLY SYMBOL OF RAVENKIND.
The latter reward-transfer operation
is now separately source modeled
for empty cursor, first free active
actor ordinary slot 0..11 (known
bit cleared), and guarded world
spawn fallback. Reward transfer is
NOT prematurely invoked on a
blocked Level16 completion event.

Expanded existing CTest assertions
cover every site, flag gating,
wrong/duplicate items, Q16 fraction,
max-300 source eviction, fourth
placement fail-closed behavior,
item grant priority, and full-party
fallback. No new CTest target:
**170 registered tests / 19 Quick**.
No Windows build or test execution
has occurred for this source.
Numbered event 8 presentation
and full completion remain
unimplemented; engineering tracker
**56.25%**, inventory PARTIAL.


## Part 8 extended — concurrent actor container state and DOS phase art

A direct CODE.2 x86 audit certified native 12E70
inventory click selection from the clicked 0x50-wide
actor column and the relevant original 6FA0
actor-specific container record. All four active
phase3 contexts can now be validated/restored
against source OBJECTS hand identity, and can be
selected/clicked independently without implicitly
closing other actor panels. Source SAVE remains
62 counted container records plus four original
four-byte actor contexts; interaction selection
itself adds NO invented serialized bytes.

Original 21648 hand-transfer all-FF reset is now
separate from the original 110B4 animated close
result FF FF 00 FF. Native presenter also selects
original RES2 bank2 phases 1/2 via
0x14+2*subtype+phase-1 at row C4h and phase3
9+subtype at row CEh. Regression cases extend
existing inventory_save_codec_tests,
inventory_runtime_tests and inventory_screen_tests
for multiple actors, source hand binding, dual
reset semantics and all animation resource IDs.

**Still pending:** original VGA retrace/page-flip
timing and DOS pixel comparisons; live replay
of partial SAVED animation phases; all-world
SAVE/LOAD and unrelated game state; Level16
special item and other inventory interactions.
No new CTest target: **170 registered / 19 Quick**.
No Windows build, CTest or gameplay execution
was initiated. Overall engineering **56.25%**,
inventory PARTIAL; no formal acceptance upgrade.


## Part 8 checkpoint — saved container UI contexts

The original CODE.2 6FA0 four-byte per-actor container UI
state is now preserved and source-restored. Both fresh
FF FF FF FF and post-close FF FF 00 FF inactive forms
are accepted. Full-open phase3 records can be bound to
one unambiguous hand by the original OBJECTS catalogue,
descriptor selector and subtype. The original record
contains only the object's low ID byte; aliasing is
explicitly checked. The per-actor renderer draws
phase3 container contents across all four panels,
and party reordering swaps their original records.

Original 0..3 open and 3..0 close phase transitions
are modeled, but partial animation timing, multiple
concurrent interactive views and full session SAVE/LOAD
are still unfinished. Regression assertions were
added to three existing tests. Registered CTests 170,
Quick tests 19. Current source is uncompiled and
unexecuted on Windows, as the manual build remains
deferred. Engineering progress is still 56.25%;
inventory milestone remains partial.


## Part 8 — guarded inventory SAVE runtime integration

The source now binds the original-byte `InventorySaveCodec`
to the initialized `ExplorationRuntime`, importing
a compatible original SAVE into live actor equipment,
Mage spell cells, known flags, cursor, 32 story
flags and 62 persistent containers, and overlaying
subsequent inventory state onto the original image.

**Compatibility guards:** equal source/world level,
A/B coordinates, facing and 16.16 time; no saved
moving/transient effect records; original 6FA0
contexts now permit both historical inactive forms
and one unambiguously resolved fully-open phase3
record. A pre-existing native hand container prevents
import; validated active views may be exported.
Export also rejects a changed world-scene revision. The incomplete full-game save menu is
not wired to this partial interface.

Existing tests gain positive state capture and
failure-atomic world mismatch cases. Total CTests
remain **170**, Quick **19**; all added current
code is still unbuilt/unexecuted on Windows.
Engineering milestones remain **56.25%**;
inventory remains **PARTIAL**. The next Full
Windows build stays deferred by user request.


## Part 8 — original SAVE inventory read/write byte projection

Original `SAVE.DAT` is 9,288 bytes with 62 persistent
container states holding 438 item slots and 62 state
bitsets. Original `CODE.2` descriptor 62 is excluded.
A new source-backed `InventorySaveCodec` decodes and
re-encodes the original cursor, 4×0x248 actor inventory
and Mage matrices, identified-item bitsets, 32 story
flag bytes, 62 container contents/slot flags and four
raw trailing UI contexts while preserving all other
SAVE bytes, including independent clock and transient
effects. Original variable-length effect count is
honored; synthetic tests cover 0/3/150 effects and
malformed boundaries. Exactly one new CTest is
registered: **170 total; Quick remains 19**.

**This is not live full SAVE/LOAD.** Original per-actor
container phase restoration, complete session/world
state and native menu integration remain open. No
current Windows compilation, CTest run or DOS
roundtrip is claimed; deferred Full build policy
unchanged. Engineering tracker remains 56.25%
and inventory milestone remains partial.


## Part 8 continued — original Mage inventory message state

Original `CODE.2 116D5..11798` Mage scroll scribing
now emits the exact original success, already-known
or Intelligence-capacity failure text from the
object-2 `6D9C/6DA0/6DA4` source pointers.
Success prefixes the real `T` spell name.
`6DAC` pending and `6DA8=30700()+0x36`
are projected; the modal `13118/13184`
resets this state on entry and expires it only
on an **unsigned IRQ tick greater than the
deadline**, restoring the ordinary held-item
description. The native modal displays the
source text even when the success path clears
the cursor.

**Acceptance remains open:** ribbon artwork,
palette and glyph parity versus DOS; host
IRQ phase is an existing BIOS-cadence proxy;
the other inventory item actions and native
SAVE/LOAD path remain unfinished. New assertion
cases are committed but not execution-tested.
169 registered CTests (Quick 19), old Windows
green #640 is not current; 56.25% engineering
milestones, inventory PARTIAL. Full build
deferred per user instruction.


## Part 8 source checkpoint — original Mage scroll learning

The live inventory now includes original CODE.2
`11630` Mage Spell Book scribing for 35 authored scroll
IDs, authenticated object-1 `EEB0` range table and
object-2 `6153` Intelligence capacity, actor spell
matrix low-nibble tests, and persistent event bit
`0xBA` even when a scribe attempt fails. Valid
learning clears the exact matrix byte and consumes
the held scroll; a known/full target retains it.
The previous Tome/Parchment Fragment `0x35` +
`0x61..0x64` persistent flags and Mode-0x190
`2D5D0` world-drop checks are also implemented
in native source with documented limitations.

**Source-only and incomplete:** original spell-learning
messages/timing, SAVE/LOAD roundtrips, Level16
`0x89` side effect, other item spells and DOS visual/
temporal parity remain open. Neither the existing
CTest count (169; Quick 19) nor the 32-task
engineering score (56.25%) is increased by these
submilestone edits. The latest Windows-green build
is still older #640 (160/160). Manual Windows
Full build remains deferred as requested.


## Active one-system-at-a-time inventory workstream

[Detailed source and DOS acceptance boundaries](INVENTORY_FIDELITY_WORKSTREAM_20261009.md).
The native inventory now has source implementations
for the full CODE.1 class/race starter matrix,
original equipment-hit tables (20 descriptors),
all ordinary/typed equipment swaps, live RES1
held item, original RES2 subtyped container
surrounds and burden-panel backdrop, recursive
nested-container item/known/capacity exchanges,
stats, encumbrance, party reorder and D-key
family-C world drops.

**Not accepted:** current implementation still
has special item/quest effects, original 2D5D0
collision, SAVE/load persistence, effects modals,
and unverified DOS colors/frames. **169 registered
CTests, no current Windows CI pass.** The earlier
last green run #640 is older source.
GitHub Actions remains manual only;
user specifically requests building only once
the **entire** inventory is complete.

**Overall engineering source tracker remains
56.25%** until its separately defined partial
inventory milestone becomes source-complete;
this does not mean 56.25% DOS-faithful behavior.

## Inventory focus: source systems integrated, DOS parity not yet verified

[Detailed CODE.2 inventory reconstruction and completion gates](INVENTORY_FIDELITY_WORKSTREAM_20261009.md).
Normal fresh-party class/race equipment, inventory
slots and known bits, nonspell equipment placement,
nested hand-container item movement, original half-pound
weights/encumbrance, per-class stats, original RES1
held-item cursor, drag-party ordering and world
actor-reference remapping are now integrated in source.
Current source registers **168 CTests**, including
the new item-description, 21-row encumbrance and
party-reorder tests. All newer changes remain UNBUILT.

The user specifically requires the **ENTIRE original
inventory system functioning before building**. Eight
tracked gates remain open: original palette/ornaments,
descriptor-exact equipment hit geometry, special
item actions, general nested containers, world
dropping, persistent save/load inventory state,
ancillary modal controls and DOS live parity.
No Windows workflow was dispatched.

Keep the original 32-unit demo engineering baseline
separate; this pass adds substantial code inside the
already-partial inventory milestone but does not
certify it complete by fiat. Legacy 1/5 formal
acceptance is not a development-progress percentage.

## Inventory-first milestone: original starting class/race loadouts

[Source/coverage checkpoint](ORIGINAL_CODE1_ALL_CLASS_RACE_STARTING_EQUIPMENT_20261009.md) uses the
original immutable CODE.1 0988h starting-equipment
table and original OBJECTS.DAT (336 definitions).
All **90 family/race rows**, including **44 real
loadouts** and **46 source-empty rows**, are now
exposed through one shared source-backed model
used by normal fresh-party CHARS creation. All
21 runtime item slots and the asymmetrical
same-class/race second-actor category-1 duplicate
suppression are validated by expanded model
tests. A new Windows source-probe reports every
authentic named starting item and original slot.

**Source complete for starting equipment, not the
inventory interface.** Current HEAD is unbuilt,
expects 165 registered CTests and 12 Quick
tests; latest successful Windows #640 ran 160.
No new DOS screenshot parity is claimed and
no GitHub Actions build was triggered.
The bounded 32-task demo engineering source baseline
remains 56.25%, tracked separately from per-subsystem
source completeness and compile/DOS verification.

## Current engineering progress: 56.25% (32 fixed task units)

[Canonical auditable engineering checklist](ENGINEERING_PROGRESS_AND_FAST_BUILD_20261009.md)
counts **12 narrowly defined source-implemented tasks, 12
partly reconstructed tasks and 8 open tasks**, for
`(12 + 12/2)/32 = 56.25%`. This is the first
transparent source-engineering baseline; it is NOT
20% rising to 56% on the same metric, nor a claim
that the entire game is 56% faithful. Future
updates increase or decrease by a known 1.5625
percentage points per partial milestone transition
after a real bounded engineering outcome.

**Last full Windows verification:** manual #640 at
160/160; new branch has 164 registered CTests that
are not yet compiled. The user-supplied original
DOS screenshots still demonstrate substantial
mismatches. Legacy `1/5 = 20%` statements in
historical entries below refer **only to coarse
acceptance gates**, not development completion.

[Manual Windows build workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/demo-v1-build.yml)
offers new default `quick` executable + 9 focused
test build and explicit `full` ALL_BUILD+164 tests.
MSBuild is parallelized and can restore experimental
incremental cache. No workflow is auto-triggered.

**First source-backed LIVE original forest floor integration
(2026-10-08):** Original CODE.2 16E90 copies
`INIT+0x0E` to resource base 258F8; 1F388
adds signed Level22 ground selector +55 and loads
RES4 type1. Native now resolves that exact original
tile and conditionally executes original 34A74
textured lower-surface raster **in the live first-person
framebuffer**. It requires all 256 ray records to be
open-field 0x93, original C026 textured mode and
verified SINTAB perspective, matching CODE.2 335C8
sentinel and full 128-row FF50 calculation.
Any unverified/mixed geometry uses flat fallback,
with explicit status in the preview title and TSV.
The source-backed pixel kernel writes 32,768
indexed palette entries, respecting original
four-plane/paired-plane split.

**Status is SOURCE-ONLY:** current HEAD has not
passed MSVC CI, and the actual RES4 texture has
not yet been captured from a user-run executable
nor compared with original DOS indexed frames.
Other clipped-ground, upper texture, NPC, inventory,
combat and portrait parity work remains open.
164 CTests are expected if Python is installed;
last green run #640 (160/160) used older code.
Official acceptance **remains 1/5 gates (20%)**.
[Complete checkpoint](LEVEL22_LIVE_ORIGINAL_34A74_OPENFIELD_GROUND_20261008.md).

**Original RES4 ground source base RESOLVED (source-only):**
Direct `CODE.2` 16E55..16E90 establishes
`INIT+0x0E` as the initial resource base stored at
`258F8`. The original `1F388` adds that base to
the signed level header +7 selector and calls
bank-4 type-1 resource resolution. The native
Level22 startup now combines actual original
`INIT`, loaded Level22 header and original RES4,
validates the exact 64×64 source and records the
selected entry or precise error in opt-in capture.
The bundled manual RES4 probe also reports the
source-selected tile, not merely plausible candidates.
[Evidence and technical boundary](ORIGINAL_LEVEL22_RES4_INIT_BASE_RESOLUTION_20261008.md).

**Still an open visual gate:** the exact selected
texture is **not yet drawn by live exploration**.
Original FF50 row-span and C026/C027 upper/lower
drawing mode must be modeled before enabling the
separate 34998/34A74 native raster. All changes
require a new **manual** Windows build; #640
(160 tests) was at older source, while 164
CTest registrations are expected now. No formal
accepted gate has advanced above 1/5 (20%).

**Original Level 22 RES4 ground identification checkpoint:**
A dedicated original-media inspector now enumerates decoded
64×64 EH type-1 resources in real RES4 without exporting or
substituting their pixel data. It records valid/invalid
entries, raw/LZSS compression, exact 4096-byte pixel
counts and FNV-1a diagnostic hashes. Its standalone
Windows console tool and `Probe-Level22-Ground.cmd`
ship in the **next manually built** executable artifact.
The source-backed test includes both raw and LZSS
synthetic RES4 entries and exact indexed drawing checks.

Original `CODE.2 1F388..1F398` selects relative Level22
ground ID **55 plus mutable resource base 258F8**.
The runtime base is still unverified; no absolute tile
is assumed and the original live 34A74 floor remains
unconnected. Current HEAD has **164 expected tests**
but has not passed MSVC CI; #640 (160 tests) is the
last verified manual run. **No acceptance gate passed.**
[Full source and diagnostic checkpoint](ORIGINAL_LEVEL22_RES4_GROUND_SOURCE_AUDIT_20261008.md).

**Source milestone: original 34998/34A74 textured terrain**
(2026-10-08): Disassembly recovered the source of the two
128-word perspective arrays: original CODE.2 startup copies
them verbatim from SINTAB projection A/B. A source-backed
FF58 generator and original 64×64 palette-index rasterizer
now support camera phase, sine quadrant folding, signed
fixed-point interpolation, FE15 shade tables, and the
original 80 four-plane / 48 paired-plane viewport writes.
Tests include unmodified SINTAB numeric checkpoints.
The runtime frame generates all 128 source steps, and
capture v2 exports seam rows 0/79/80/127.

**Not yet live visual fidelity:** the original selected
RES4 type-1 ground resource and dynamic bank selector
are not yet proven in the native renderer, which still
draws a flat lower fill. Correct samples in a synthetic
test are not DOS viewport parity. Last successful
Windows build was #640 (160 tests) at older source;
163 tests are now expected, current HEAD **unbuilt**.
Manual GitHub Actions only. Formal 1/5 accepted gate
count unchanged.
[Source trace and raster checkpoint](ORIGINAL_CODE2_34A74_TEXTURED_GROUND_PIPELINE_20261008.md).

**Binary-backed current fresh-game milestone (2026-10-08):**
The original `CODE.2` bootstrap `2060C..2076D`
sets fresh level22, cell (4.5,4.5), heading 270
independently of SAVE.DAT; default new-party startup
still passes full 0x920 CODE.1 CHARS into CODE.2.
Native preview SAVE.DAT character splicing has been
removed. Source now gates movement on the original
two-game-tick schedule rather than every VGA retrace,
and connects original `2C800` live NPC + `2CC88`
placed-object footprints to the walking integrator.
Original class-1 0x54 conditional transitions now
respect live state `&0x1C0==0x40` and levels
0x17/0x0A before triggering. Tests are registered
but the newer branch is UNBUILT (162 expected CTests).
The original 0x34A74 textured lower renderer
is a significant remaining visual acceptance gap,
not a completed flat-fill equivalence.
[Full original binary audit](ORIGINAL_FRESH_START_TIMING_COLLISION_AND_GROUND_AUDIT_20261008.md).

**Original DOS screenshots and critical user clarification (2026-10-08):**
The screenshots are the original *freshly created party* entering
the forest and using inventory, automap and combat—not the supplied
quick-start SAVE.DAT. Fidelity acceptance therefore requires the
entire original CODE.1 creation -> CHARS transfer -> CODE.2 new-party
Level 22 path and matching initial actor, inventory, world and
camera state. Any SAVE.DAT party substitution in the opt-in
walkaround was **removed**. The existing CODE.1 transfer path
is integrated into the Win32 default flow, but NOT certified
visually against those screenshots.

**Source-only gameplay improvements awaiting manual build:**
CODE.2 201A7 two-game-tick motion pacing replaces motion-on-every-VGA
retrace; 2D5D0 checks incorporate original 2C800 NPC and 2CC88
placed-object/tree footprints; a strengthened Release-enabled
character-entry regression checks lossless 0x920-byte CHARS.
Original 34A74 textured forest floor, NPC motion, combat/
equipment-hand animation and pixel-exact outdoor layering are
unresolved. No automatic workflow dispatch, no new Windows build,
and **no new accepted gate**.
[Fresh-party reference and gap register](ORIGINAL_DOS_FRESH_PARTY_FIDELITY_REFERENCE_20261008.md).

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
