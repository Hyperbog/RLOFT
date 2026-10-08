# Original Preservation Archive → Demo v1 integration map

## Provenance and boundary
## Project progress scoring clarification

The [demo progress scorecard](DEMO_PROGRESS_SCORECARD_20261008.md)
distinguishes discrete **1/5 formal release gates accepted**
from cumulative engineering implementation, which has made
substantial additional advances but has **no objectively
weighted percentage**. A historical “20%” label is an
acceptance score, not an engineering completion estimate.
Track source-backed model coverage, actual live-game integration,
current-Windows CTests and original-DOS comparison separately.


The authoritative original-technology archive was produced on `code1-decompilation`. This directory's parent preservation snapshot was copied into `demo-v1` using the **identical 24 Git blob object IDs**, not regenerated prose, from source commit:

`5e70d40957093fb85e555fbc5f4dbe87fa2fea67`

The certified archive documents 22/22 numbered *historical static* chapters 00–21; Chapter 22 adds a separate **91-strand forward-looking backlog**. See [the static certification](../PreservationArchive/FINAL_STATIC_DOCUMENTATION_CERTIFICATION.md) and [open research register](../PreservationArchive/22_REMAINING_RESEARCH_AND_FUTURE_EVIDENCE_ROADMAP.md). Neither the certification nor a successfully compiled C++ reimplementation proves full original-game runtime behavior.

**Evidence precedence:** immutable original DOS binary bytes → instruction-accounted certified reconstruction → corroborated archive analysis → native-port fixture. Pin any new behavioral finding to its original CODE.1/CODE.2 offset and archived evidence, not merely to a modern source name.

**Branch reconciliation closed (2026-10-07):** `Code1/Code1.cpp` already matched across both branches (Git blob `226c563007ac4daef8f732505341175914ba2b94`). The two CODE.2 C++ revisions differed in **exactly two source regions**, both justified by direct original `CODE.2` x86 disassembly: `0139C` automap offsets and `2975C` graphics argument forwarding. The original-certified snapshot `c3f16122e79c3c464382e4511d9d932f6fcd694a` is retained in Git history, and `code1-decompilation` now uses the same corrected `Code2/Code2.cpp` blob as `demo-v1`: `87f6638921c53a32bde59735515fbc175e2c96ee`. See [the demo errata](../Code2/POST_CERTIFICATION_ERRATA.md), the [pinned original-byte audit](https://github.com/Hyperbog/RLOFT/blob/33c109add901459150011009bf88a90c1428185d/Code2/POST_CERTIFICATION_GRAPHICS_CORRECTIONS_2026-10-07.md), and [main-branch reconciliation commit](https://github.com/Hyperbog/RLOFT/commit/c952e9adb152007208951da3b0030c7439fd72b9). No original executable data or byte-accounting totals were changed. New strict full-file syntax and original-frame acceptance gates remain separate.

## Direct original-code-to-port crosswalk

| Archive primary evidence | Native demo implementation | Practical next acceptance test |
| --- | --- | --- |
| [Ch.04 — planar VGA, SINTAB, type-6/type-7](../PreservationArchive/04_VGA_RENDERING_PIPELINE.md), `Resources/RES/TYPE7_RENDERER.md`, certified CODE.2 | `src/type7_scene_pixel_executor.cpp`, `src/type7_vga_trace_validation.cpp`, `src/surface_wall_presenter.cpp`, `src/surface_prepass.cpp` | R-076: exact frame/plane masks, wall texture columns, sprite overdraw and occlusion from deterministic original captured frames |
| [Ch.05 — world state and events](../PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md), `DatFiles/LEV022.DAT/` | `src/level_world_state.cpp`, `src/exploration_runtime.cpp`, `src/exploration_world.cpp` | R-036: compare actual Level 22 door/wall state, mutable actor indices, conditions and visibility under repeated turns |
| [Ch.07 — mouse/UI/inventory](../PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md) | `src/win32_main.cpp`, `src/exploration_hit_regions.cpp`, `src/level22_viewport_target.cpp`, `src/exploration_legacy_input.cpp` | R-061/R-079: original event ordering, viewport click rank, cursor-held item policy, hand-slot use vs swap, non-invented attack dispatch |
| [Ch.10 — items and containers](../PreservationArchive/10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md) | `src/inventory_runtime.cpp`, `src/level22_encounter.cpp` | R-045: preserve known/identified bits, item cursor ownership, slot swaps, recursive containers and world drops |
| [Ch.11 — NPC/AI](../PreservationArchive/11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md), [Ch.12 — combat](../PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md) | `src/level22_encounter.cpp`, `src/npc_scene.cpp`, `src/exploration_runtime.cpp` | R-045/R-075: original player-to-Assassin attack roll/THAC0, weapon/effect damage, death-state changes and four drop records |
| [Ch.14 — event/wordcode/story state](../PreservationArchive/14_EVENT_WORDCODE_DIALOGUE_AND_STORY_STATE.md) | `src/level22_story.cpp`, `src/assassin_relic_transition.cpp` | R-078: verify final-relic possession predicate and state change; do not substitute an arbitrary Level 23 transition |
| [Ch.16 — ANM/cinematics](../PreservationArchive/16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md), [Ch.17 — timing](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md) | `src/anm_cinematic.cpp`, `src/win32_main.cpp` | R-062/R-078: 22-frame Level 22 mist sequence, exact handoff frame/coordinates, pacing and authorized media availability |
| [Ch.08 — SAVE/party persistence](../PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md) | `src/inventory_runtime.cpp`, `src/exploration_runtime.cpp` | R-053/R-077: preserve original scene flags, party belongings and module handoff in complete game-state transitions |
| [Ch.20 — programmer reference](../PreservationArchive/20_MODDER_PROGRAMMER_REFERENCE.md), [Ch.21 — unknown/correction audit](../PreservationArchive/21_FINAL_CONTRADICTION_UNKNOWN_AND_EVIDENCE_AUDIT.md) | All `DemoV1/` modules | R-072: each reconstructed function/module must cite controlling CODE offset, format provenance, test and regression result |

## Cross-cutting timing acceptance (applies to every task below)

**Timing is a correctness gate, not an end-stage visual improvement.**
Follow the [native timing fidelity contract](TIMING_FIDELITY_CONTRACT.md)
and original [Chapter 17](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md)
throughout movement, turning, animations, card flips, combat, UI/modal waits,
world time and story transitions. The original `2E608/2E614(2,0)`
main-loop cadence is separate from the VGA retrace; the native host currently
integrates exploration motion at nominal `70.0863`-Hz presentation cadence.
**Investigate this mismatch before accepting movement speed or combat action
frequency as faithful.** Preserve original input/event service ordering,
separate `F58C` IRQ ticks, `A7E4` gameplay maintenance ticks, `CF4C`
world-time state, animation packet waits and driver-dependent clock behavior.
Never turn authored multiframe sequences into instantaneous changes or assign
uniform animation FPS without evidence.

Required acceptance: original-vs-native time-series traces (coordinates,
heading, frame IDs/dwell, cooldown/hand flags, palette/sound and pause/resume),
including actual DOS capture under a documented runtime configuration. Render
smoothing on modern monitors may be presentation-only; it must not speed up
the game simulation. Keep GitHub Actions manual-only while private.

## Implementation order, using the archive to avoid rediscovery

1. **R-072 follow-through:** CODE.2 branch divergence is reconciled (above). Continue original-offset traceability for Level 22 native routines; isolate remaining inferred behavior and run a fresh whole-file CODE.2 strict syntax check when a compiler is available.
2. **R-076 visible fidelity:** transactional original-SINTAB RES4 prepass pixel execution is integrated for modeled 1..7-column runs, with strict original-plane masks and diagnostics ([checkpoint](SURFACE_PREPASS_CHECKPOINT.md)). Finish ordinary type-6 main-wall SINTAB rendering and compare exact indexed pixels against original DOS reference scenes. Synthetic tests alone do not replace this acceptance criterion.
3. **R-061/R-079 pointer and item behavior:** the exact CODE.2 `12E70` ordinary 3×4 inventory tiles and a bounded `117A0/0F238` cursor/slot identity+known swap are now connected to full-party modal clicks. The original empty-slot identified-bit no-op is preserved after direct opcode audit ([checkpoint](LEVEL22_INVENTORY_INTERACTION_CHECKPOINT.md)). The host still refuses a second loose-item pickup while holding an item; the ordinary inventory grid can now safely stash that item and free the cursor. Implemented bounded native `214B8` live hand/cursor swap, original inclusive D684 hand tiles, and `211D8` first-empty actor-panel stash ([hand checkpoint](LEVEL22_HAND_INTERACTION_CHECKPOINT.md)); next: full DOS input flag fidelity and `214B8` *use/attack* controller, `21404` container insertion, remaining `12180` equipment legality and original item cursor sprites; do not treat unsupported special objects as generic swaps.
4. **R-045/R-075 combat:** Original hit arithmetic `C80C`, target selection `DB94/DACC`, conditional complete hand approval `24478`, `E578` pending actor-effect service, attack targeting, dual `A7E4` recovery clocks `E5F0/24B48`, the new [E5F0 damage dice/exceptional-strength model](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md), and the bounded original `C910` creature/weapon gate are all **source models with registered but uncompiled regression tests**. A successful input-to-NPC attack is **not yet connected**; capture the real `OBJECTS[-1]` heap-adjacent bytes (the original `DGROUP:684B` signed Strength table for scores 0–25 has now been recovered), reuse the newly reconstructed `22E40` Vampiric class-level helper and reachable Rod branch, integrate the reconstructed HARM staging and [E284 death-follow-on source model](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md), then complete original `38110` presentation, `BF44` story, `E0F8` runtime respawn, `1DDA4` awards, broader actor/NPC mutations and exact timed action traces before enabling real player attacks. The original `+0x19E` hit-ready write and `+0x10B` miss-inclusive action lockout have **different conditions and potentially different clock snapshots**. Build via manually requested consolidated CI only while private.
   - **`240A8` update:** complete nonnegative candidate logic now exists with original `33FC4/34060` exception predicates, class-5 level gates, OBJECTS byte `+0x35 & 3`, and 21 additional 240A8 original opcode checks; a further 18 original pair-arbitration and heap-loader signatures ([checkpoint](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md)). **Do not classify `-1` as a normal item** until the original loaded OBJECTS pre-base bytes are recovered. Full `24478` pair arbitration and C++ runtime verification remain pending.

   - **`24478` normal pair arbitration:** literal hand-classified candidates, OBJECTS `+0x35` qualifier and `+0x30` weight priority, signed class-group exceptions, and separate `+0x10B/+0x10F` busy-state checks are now modeled for two regular nonnegative item IDs ([checkpoint](LEVEL22_HAND_PAIR_ARBITRATION_CHECKPOINT.md)). The original OBJECTS loader's `4104C` wrapper adds ten bytes before returning the buffer pointer; `item[-1]` reads the previous low-level heap block's vicinity, **not OBJECTS.DAT**. Never assume an unarmed category or claim complete `24478` until the heap byte and pseudo-hand runtime behavior are evidenced. Preserve tick-accurate attack eligibility under [timing contract](TIMING_FIDELITY_CONTRACT.md).

   - **`24478` early-return completion:** original machine code `244CE..246E9` now independently modeled with tests: both-empty and both-pseudo pair shortcuts; single pseudo/empty candidate overrides; the `0x7240` actor-status gate; and the separate first-two-actor status threshold **`>=0x4000`**, which must not be confused with `240A8`'s earlier exact `0x8000` test. Two complete short-circuit pair cases now reach their own final busy checks ([checkpoint](LEVEL22_HAND_PAIR_EARLY_GATES_CHECKPOINT.md)). The consolidated offline signature count is **87**, no Actions builds. Remaining mixed pairs may read raw pre-OBJECTS memory.

   - **`24478` conditioned full source closure:** The pair model now handles ordinary/pseudo/empty combinations, including mixed `-1` hand cases by taking **real observed** original allocator-adjacent item category (`OBJECTS_PTR-0x12`), weight (`-0x11`) and qualifier (`-0x0C & 3`) fields ([checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)). No serialized OBJECTS.DAT content is invented to fill these bytes. Live DOS heap capture, source compilation, event/cooldown integration and time-series comparison are still mandatory before playable attacks.

   - **E5F0 attack target and maintenance-clock recovery:** [New checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md) reconstructs explicit-target vs `DB94(actor)` preference, special mode-FF rear-party target suppression, and weapon WORD `+0x35` bits 2..5 multiplied by 9 for authored action duration (unarmed/pseudo: 18); actor `+0x132 & 0x01000010` halves that duration. **Only successful hits** store `A7E4 + floor(interval/2)` into selected hand `+0x19E` and post-damage into `+0x1AA`. The next routine—original logical **`24B48`** (objdump virtual `34B48`)—presents the action, then if interval!=0 writes the separate **`+0x10B` action-state deadline even on MISS/no target**, using a potentially later `29CC4 → A7E4` observation + **full** interval. Both are **pauseable gameplay-maintenance clocks**, never VGA/IRQ or a fixed Windows frame cadence. The pure C++ models retain original `26018`-supplied item ID and have tests registered but **not compiled**. Offline CODE.2 opcode audit is **126 anchors**; 21 new `E5F0`/`29CC4` anchors byte-matched. Next: actual `1E578`, original post-C80C damage/RNG, timed animation and NPC mutation. No automatic CI.

   - **E578 prehit actor effect servicing:** the original CODE.2 logical `E578` (objdump virtual `1E578`) runs after initial DB94 target choice and before target validation/C80C accuracy. Native `CombatAttackStatusService` now clears `actor+0x132 & 0x20` for **all four characters**, then clears `actor+0x133 & 0x02` and `actor+0x15A` dword for the attacker only, marking original `CF5A` redraw bytes as `1`. Exactly nine new original instructions passed byte verification; no live mutation path is enabled until original action ordering is integrated ([checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md)).

   - **E5F0 dice/stat bonus:** the original `E5CC` table's nine pseudo-hand cases now have exact `1C328` dice-count/sides/bonus parameters, and ordinary OBJECTS record bytes `+0x38/+0x39/+0x3A` are exposed ([checkpoint](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md)). Stat byte `actor+0x33==18` plus eligible class `1/5/0/2` uses exceptional percentile boundaries 75/90/99; the original signed `DGROUP:684B` modifier table is now recovered for all authored Strength scores 0–25; scores above 25 still require the actually loaded adjacent data byte. Vampiric Touch now computes `22E40` directly from signed class-4 party record lanes (first match, fallback 1), then schedules `Nd6` **only after hit success**; no guessed RNG calls.
   - **C910 weapon-vs-creature gate:** NPC definition `DWORD+0x31`, item category `+0x2F`, enchantment `+0x37` and original immunity mask bits 14/15/17/18 are now modeled, including a **real MOVZX/signed -2 comparison quirk** and minimum damage one. **Rod of Smiting `0x4A` bypasses this routine**; source audit proves its `C80C` Boolean result never equals the later Rod comparison 20, leaving normal charged-crit/decrement code unreachable and types `0x17/0x1F` to reroll an extra `2d8+6 per die`; HARM lowers NPC HP to a roll, clears pre-gate injury, and can still receive C910's one-damage minimum. These are source-only selection/gating models, not NPC mutation. Offline CODE.2 byte verifier: **173 signatures** (47 new this checkpoint). Do not enable attack input until `E284` NPC injury and post-roll effect cadence are faithfully integrated.

   - **22E40/Rod/E284 follow-on:** [new bounded checkpoint](LEVEL22_CLASS_LEVEL_ROD_INJURY_CHECKPOINT.md) now reconstructs signed three-lane `22E40` first-match class level, shows that `C80C` returns only 0/1 so the Rod `0x4A` comparison to 20 makes its charge/critical path dead, and models `E284..E306` NPC injury's creature types `0x08/0x0F` exceptions, reaction flags and signed 16-bit HP arithmetic. The Level 22 Assassin type9 damage path now reuses this exact prelude before existing drops/death logic; no live *player attack* is enabled. Original byte verification: **212 anchors** (39 new), no CI build. Later E284 death/respawn, actual DOS timing/RNG and the historically pending Strength/HARM stage are covered by the [new follow-up](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md): the table, HARM prelude and initial 3ACDC NPC-class state transition are now reconstructed.

   - **CODE.2 DGROUP Strength, HARM and E284 classification:** [Current source checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md) byte-verifies and embeds the **26 original signed Strength modifiers** at physical `CODE.2:0x7C76F` (`DGROUP:684B`), preserving the separate Strength-18 exceptional branch and requiring observed adjacent bytes for scores >25. New `CombatHarmPreludeModel` explicitly stages actual `1d4` → signed HP clamp → outgoing zero → `C910` minimum-one → `E284` second HP subtraction. Original `3ACDC(type)` was decoded as `type<0x22 || type==0x50` and its NPC byte `+0x0B` state mutation is integrated into the Level 22 Assassin injury handler for both surviving and lethal hits. Newly audited executable evidence: **226 code anchors + one 26-byte data-table anchor**, 14/14 new x86 matches and 1/1 data match. **Later E284 death variants, actual attack RNG/input and clock-matched presentation remain unimplemented; C++ source remains uncompiled**, no GitHub Actions minutes spent.

   - **E284 death and presentation follow-on:** [E284 source checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md) now distinguishes HP-positive `38110(DEF+41 + global258C0)` from corpse `38110(DEF+43 + global258BC)`, dispatches the ordered type `0x08/09/12/18/21/50` scripts, and exactly models four type9 `3195C` item/angle arguments. The native Level22 Assassin drop loop **uses** the audited type9 source plan. Original `E50F..E539` scans **three signed type words** at `258AA` and may call `E0F8` before deciding whether to mark `NPC+0x0B` bit02; actual tracker/respawn input is still missing. Global story flag `BF44`, audio resource presentation `38110`, `1DDA4` award and type18 conditional early return remain unported. New **32/32** original byte anchors bring the verifier to **258 opcodes + one Strength data anchor**. No private Actions build; source-only tests require compilation.

   - **E0F8 native source model:** [Read-only exact E0F8 candidate checkpoint](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md) now implements original low16 IRQ-tick/BIOS fallback reseed, exactly two 30CA2 coordinate draws per attempt (up to three), low-byte tile flags, independent 15-cell axis thresholds, NPC slot occupancy, HP dice and field-by-field candidate patch. Original Level 22 30x15 geometry rules out a successful candidate for an in-bounds party, but not E0F8's RNG side effects; real 258AA tracker/tick caller inputs are still unobserved. Local isolated source test passed, registered Windows test has not run. Cumulative original executable signature audit 293 opcodes +1 Strength data table; live event integration remains blocked on evidence.

   - **E0F8 temporal respawn audit:** [Updated death checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md) now records `E0F8`'s independent `30700` IRQ-clock read, `290D0` service, **two 30CA2 RNG draws per attempted respawn coordinate**, original tile-obstruction and signed distance tests, **maximum three attempts**, and additional NPC HP dice roll on successful placement. Because E0F8 may change **both** RNG stream and NPC world state *between* `38110` death presentation and `E539` marking the corpse, its current source model requires the **actual external callback outcome**, never a guessed `false`. +15 original E0F8 signatures matched, cumulative offline verifier **273 opcode + 1 data-table anchor**. Native E0F8/world integration and observed 258AA type slots remain open; current C++ uncompiled, no CI minutes.

5. **R-078 story transition:** compare CODE.2 Level 22 event sequence to the native last-relic state machine and run the original 22-frame transition with correct Level 23 load and consumed relics.

## Current acceptance boundary

- GitHub Actions [run #572](https://github.com/Hyperbog/RLOFT/actions/runs/37710634731) certified Linux and Windows build/test at the then-current source, including **99/99 Linux CTest** and successful Windows executable publication.
- Following the user's usage-saving instruction, Linux automatic runs are paused. The Windows workflow remains enabled for native code changes and cancels superseded builds. Documentation-only commits are excluded from that path filter.
- The updated native port is **not** called 100% complete until original-versus-native frame/gameplay equivalence and Level 22 finishing behavior are demonstrated.

## Updating the preserved snapshot

The archive files in `../PreservationArchive/` are identical Git blobs copied from the pinned `code1-decompilation` commit above. Make factual changes to the authoritative archive branch, establish supporting original-byte evidence there, then intentionally resynchronize the snapshot; do not independently edit its contents on `demo-v1`.


## Current source-only callback verification — 2026-10-08

Original CODE.2 DDA4 callback modeling now includes the bounded
3DE4 and 3EA4 spell-state byte loops, their separately verified
level tables and group counts, and the original 1C21C IRQ
message queue/deadline. The latest [checkpoint](LEVEL22_DDA4_SPELL_IRQ_CHECKPOINT.md)
contains original SHA256 provenance, actor byte offsets, all
50 new opcode signatures, 960 table-driven local C++17
differential cases, and the remaining live-integration gates.

Before actual XP or level-up writes are enabled:
1. Implement/validate 3210F exact formatted text and
   1C258..1C326 message display/expiry side effects.
2. Execute each callback with the *observed post-prior-call*
   character state; never use a stale preplanned spell/HP
   state or reseed the shared RNG.
3. Preserve full 30700 IRQ time, 1C21C +0x5A unsigned
   deadline behavior, and original DC7C dice consumption.
4. Complete a deliberate full CMake/Windows regression and
   reference-DOS state/time trace. No automatic Actions
   build is authorized merely by this checkpoint.

The latest audited ledger has 457 named instruction-byte
checks at 449 unique logical offsets, one direct data table,
and eight hashed DGROUP/CS tables. Acceptance remains
**20% (1/5 major demo gates)**.


## New message-format and display evidence (2026-10-08)

The specific DDA4 level-up string and message lifecycle are
now source-modeled:
`DF54->3210F(original format+548)->DGROUP 759C
->1C21C/IRQ enqueue->1C258 display->1C2CC IRQ expiry`.
See [the authoritative source checkpoint](LEVEL22_DDA4_MESSAGE_FORMAT_DISPLAY_CHECKPOINT.md).

This closes the original **format identity** and **display
call ordering**, not the broader general-purpose 385B0
formatter and not original VGA/IRQ runtime integration.
Original 1C258 uses 291B0, 2939C, 3060C and 1A8C8
as deferred renderer services; those must run in sequence
against live globals and with a fresh IRQ observation
AFTER drawing. Do not treat the read-only event trace as
a rendered HUD update, or a source regression as a
complete Windows/demo build. Overall release acceptance
is still **20% (1/5 major gates)**.


## Windows build gate closure and active renderer dependency

The real Windows CI build finally **passed 130/130 CTests**
on [run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281).
The runnable demo executable was refreshed and committed.
The earlier source-only checkpoints that stated the entire
repository had never been built are historically correct
for their time but **superseded as of this exact source SHA**.
This pass certifies MSVC compile and tests, not original DOS
fidelity, timing or complete native gameplay.

Following that green run, a new `CombatIrqDisplayRuntimeModel`
can synchronously call original message-rendering services
with the true order and caller-supplied 30700 tick. It also
models original 291B0/29FEC/2AB6C VGA page state from
observed F005 page stride. **These new sources remain
outside that Windows build;** only strict C++17 isolated
Debug/Release/UBSan tests passed. The next integration
must connect underlying 2939C->2A380/2E758 and
3060C->303C8 plus 1A8C8 framebuffer effects to
the original planar VGA model and validate DOS reference
image/timer traces, without inventing missing resources.

Details: [2026-10-08 checkpoint](WINDOWS_BUILD_AND_IRQ_RENDERER_CHECKPOINT_20261008.md).
Overall **20% (1/5 demo acceptance gates)** is unchanged.


## 303C8/2939C faithful glyph and sprite layer (2026-10-08)

The [original glyph/sprite continuity checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
extends the DDA4 message renderer from 1C258 through
3060C/303C8 to 2939C/293EC and deferred 2E758/F0C8.
It preserves signed DOS glyph indexes, the live mutable
font record/width lookup, shaded-vs-normal sprite selection,
zero-resource early return and post-raster refresh timing.
The verified original executable ledger now contains
**557 named byte checks / 549 distinct code locations**.

The native adapters can execute original callbacks in order,
but the injected **sprite rasterizers** 2A380/2A584 and
**compositor** 1A8C8 still require real source-backed
VGA-plane implementations and DOS pixel comparisons.
The currently available IndexedFontPresenter is not
proof of exact DOS CP437 signed-width behavior.
Do not merge any approximate glyph renderer as the final
original gameplay path. The original Windows #634 PASS
at a previous source commit is unaffected and does not
test these two newly registered CTests. A new explicit
Windows build will be needed to claim full integrated
CMake/CTest acceptance of this newer source.


## 2AB38 exact sprite destination and planar write prerequisites

The original `2A380/2A584` sprite primitives use
`2AB38` to calculate the destination video byte and
plane from the current row/column and mutable `EFED`,
`EFF1` and `F001` globals. The
[checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
has a tested, source-backed C++ implementation preserving
both x86 variable-SHL counts, unsigned column SHR,
byte-plane low bits and wrapping physical address.
The executable audit has 577 named/569 distinct sites.

A subsequent exact sprite implementation must now reproduce
the 2A380/2A584 raster formats, VGA sequencer mask OUTs,
transparent/packed sprite byte behavior, shade self-patches,
and source-driven refreshes **before** feeding verified
planes into the native framebuffer/compositor. It must not
equate the 2AB38 address projection or the existing
VgaPlanarMemory with completed DOS sprite rasterization.
No new full Windows CI run has been triggered by this
source-only addition.


## 2A380/2A584 raster pixel path closed at bounded component level

The [2026-10-08 source checkpoint](LEVEL22_CODE2_SPRITE_RASTER_CHECKPOINT.md)
adds a reusable native sprite raster that writes original
opaque/transparent bytes to the existing four-plane
VGA memory. Both normal 2A380 and shaded 2A584
paths are covered, with exact original resource-header
fields, four rotating sequencer mask planes, transparent
sentinel, DWORD frame addressing and EFF9 row pitch.
Shaded output uses **original byte lookup memory**
via the actual 32-bit patched shader pointer, not an
invented palette or brightness adjustment.

This closes source-level sprite *copy/lookup arithmetic*
but **not** live DOS asset binding, actual CODE.2
self-modifying instruction operand writes, persistent
sequencer-register state, 1A8C8 compositing, 2E758
refresh implementation, or reference-DOS pixel/frame
comparison. Existing native sprite and font dispatchers
should call this reusable raster on observed original
memory views, rather than reimplementing incompatible
Level 22-specific drawing.

The combat byte verifier has **648 named/640 unique**
original CODE.2 instruction checks; exact implementation
source passed isolated Debug/Release/UBSan tests.
No new private Windows runner build was triggered.
Faithful demo acceptance remains **20% (1/5)**.


## Source-ordered original sprite/VGA state and pointer binding

The [current machine binding checkpoint](ORIGINAL_SPRITE_MACHINE_BINDING_CHECKPOINT_20261008.md)
closes the bounded normal/shaded sprite raster's omitted
DOS state: the 2A584 two-DWORD self-modifying code operands,
VGA sequencer 03C4h index-02 persistent mask, and
transparent global F009. A reusable native
`CombatOriginalSpriteRasterBinder` drives the existing
2939C/293EC->2A380/2A584->2E758 chain through
authentic original DOS sprite and shader pointer readers.
Source checks: **653 named / 645 distinct original x86
instruction sites**.

The binding intentionally rejects absent resource bytes;
no dummy art, default lookup palette, fixed VGA pitch or
fabricated timing was introduced. The next integration
needs an actual source-backed DOS memory/resource provider,
original 1A8C8 compositor and 2E758 refresh implementation,
and DOS VGA-plane/IRQ reference comparison. Local strict
C++17 builds passed; a new full Windows CTest and EXE
build are **still required** for this newer codebase.
No private Actions runner was triggered here.


## Native 1A8C8 signed overlay compositor wrapper — 2026-10-08

The [latest source checkpoint](LEVEL22_CODE2_COMPOSITE_1A8C8_CHECKPOINT.md)
reconstructs the original 1A8C8 **direct** and
**buffered overlay** paths, distinguishing native
row/column order, inclusive signed intersection with
32×16 original overlay extents, A6BA busy-state
writes, 9E37 reference counting, active-page switching
and exact 0x400 backing copy through 30CD4.
A new `bind_irq_display_1a8c8` adapter runs it
under the earlier 1C258 timed-message callback
without advancing 30700 ahead of rendering.

31 additional CODE.2 x86 instructions were verified
(684 named / 676 distinct). Isolated C++17 tests
passed; consolidated current-HEAD Windows tests and
DOS reference pixels are **not** claimed. The next
core-graphics shared-engine item is exact 2A078
VGA sequencer/graphics-controller rectangle-copy,
then nested 1A288/1AB20 overlay effects and DOS
A/B frame/IRQ testing. Formal demo acceptance stays
1/5 gates; engineering percentage is unmeasured.


## Actual native 2A078 VGA latch and 30CD4 overlay backing copy

The [2A078/30CD4 original-code checkpoint](LEVEL22_CODE2_2A078_30CD4_COPY_CHECKPOINT.md)
closes two further **shared engine** source paths:
original graphics-controller write-mode-1 four-plane
VGA rectangle copying, including original EFE9/F001
page restoration and forward overlapping-source behavior;
and original `30CD4` unsigned-overlap-aware DGROUP
buffer transfer for the `1A8C8` 1024-byte overlay
backing block.

The `CombatOriginalVgaCopyBinder` now supplies these
to the original `1A8C8` renderer callback, and native
code can transfer real planar pixel and backing bytes.
**70** further CODE.2 original opcode signatures
passed, for **754 named / 746 unique addresses**.
Strict source-equivalent local Debug/Release/UBSan
checks passed, but no fresh Windows EXE or full
CTests are claimed for this HEAD.

Remaining overlay-specific work is reconstructing
`1A288` and `1AB20` display/restore services,
integrating actual original framebuffer and DGROUP
captures into the live runtime, and DOS frame/state/
IRQ equivalence tests. Do not mark the formal demo
acceptance gates passed solely because reusable engine
components pass isolated unit tests.


## Original 1A288/1AB20 shared-overlay lifecycle reconstructed

[Source checkpoint](LEVEL22_CODE2_OVERLAY_1A288_1AB20_CHECKPOINT.md):
the reusable overlay model now reconstructs the
original 85FC/9E37 first-activation, nested, and
last-reference gates, 2E590/2E5D4 page callbacks,
1A3B8 position update, 292BC/2932C 32×16
backing capture/restore, 2AE30 probe and conditional
29804 sprite modes. It binds to the actual 1A8C8
compositor and preserves authentic 2A078/30CD4
collaborators instead of replacing them with
imaginary pixels.

49 newly verified CODE.2 anchors increase the
executable ledger to **803 named checks at 795
distinct logical instruction offsets**. The
isolated strict C++17 model tests passed;
the new full 1A8C8 integration test is CMake-
registered but not yet Windows-built. The
individual original page-stack, overlay backing
capture/restore and mouse/graphic services still
require source-backed native implementations and
DOS reference screen/IRQ comparison.


## Original 2E590/2E5D4 page stack connected to overlay renderer

The [VGA page-stack checkpoint](LEVEL22_CODE2_VGA_PAGE_STACK_CHECKPOINT.md)
reconstructs original `F0D0` byte stack depth,
`EF4D` active-page savings in DGROUP 2B644,
true `291B0/2AB6C` selection, live F005 page
stride and original depth-10/depth-0 diagnostics.
The reversible page-stack callbacks now bind
to the earlier 1A288/1AB20 overlay model.
A shared-video binding also propagates its
EFE9/F001 changes into the live 2A078
VGA-copy globals rather than leaving two
stale independent video snapshots.

23 newly checked source instructions raise the
ledger to **826 named / 818 unique logical
CODE.2 addresses**. Exact committed source
passed isolated strict Debug/Release/UBSan
tests, with two more assertion-enabled CTests
registered. No complete Windows build at this
current source HEAD is claimed.

Next is original `292BC/2932C` clip and
four-plane backing capture/restore, including
`2A114/2A178` VGA read map and sequencer
registers, then DOS reference framebuffer
comparison and other gameplay gates.
