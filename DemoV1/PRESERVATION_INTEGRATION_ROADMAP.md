# Original Preservation Archive → Demo v1 integration map

## Provenance and boundary

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
4. **R-045/R-075 combat:** the original `C80C` hit-threshold arithmetic is reconstructed ([checkpoint](LEVEL22_COMBAT_HIT_CHECKPOINT.md)). New read-only `214B8/26018/23F50` hand-entry preflight and `DB94/DACC` group priority, ranked target selection and original `0x7ED8` projection cutoff are now in source with tests registered ([new checkpoint](LEVEL22_COMBAT_ENTRY_CHECKPOINT.md)). The hand ready dword was corrected to actor `+0x19E`, distinct from the action-state gate at `+0x10B`. The `24478` audit has now reconstructed its three independent signed-class/item predicates but **not** the complete pair decision ([eligibility checkpoint](LEVEL22_HAND_PAIR_ELIGIBILITY_CHECKPOINT.md)). Complete `240A8` and full `24478` dual-hand eligibility, then real `E5F0` orchestration, `30CA2` RNG, `C910` damage gate, actual NPC-hit effects/cooldown and accepted DOS traces **before enabling attack input**. The new source has not been rebuilt because private GitHub Actions is manual-only.
   - **`240A8` update:** complete nonnegative candidate logic now exists with original `33FC4/34060` exception predicates, class-5 level gates, OBJECTS byte `+0x35 & 3`, and 21 additional 240A8 original opcode checks; a further 18 original pair-arbitration and heap-loader signatures ([checkpoint](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md)). **Do not classify `-1` as a normal item** until the original loaded OBJECTS pre-base bytes are recovered. Full `24478` pair arbitration and C++ runtime verification remain pending.

   - **`24478` normal pair arbitration:** literal hand-classified candidates, OBJECTS `+0x35` qualifier and `+0x30` weight priority, signed class-group exceptions, and separate `+0x10B/+0x10F` busy-state checks are now modeled for two regular nonnegative item IDs ([checkpoint](LEVEL22_HAND_PAIR_ARBITRATION_CHECKPOINT.md)). The original OBJECTS loader's `4104C` wrapper adds ten bytes before returning the buffer pointer; `item[-1]` reads the previous low-level heap block's vicinity, **not OBJECTS.DAT**. Never assume an unarmed category or claim complete `24478` until the heap byte and pseudo-hand runtime behavior are evidenced. Preserve tick-accurate attack eligibility under [timing contract](TIMING_FIDELITY_CONTRACT.md).

   - **`24478` early-return completion:** original machine code `244CE..246E9` now independently modeled with tests: both-empty and both-pseudo pair shortcuts; single pseudo/empty candidate overrides; the `0x7240` actor-status gate; and the separate first-two-actor status threshold **`>=0x4000`**, which must not be confused with `240A8`'s earlier exact `0x8000` test. Two complete short-circuit pair cases now reach their own final busy checks ([checkpoint](LEVEL22_HAND_PAIR_EARLY_GATES_CHECKPOINT.md)). The consolidated offline signature count is **87**, no Actions builds. Remaining mixed pairs may read raw pre-OBJECTS memory.

   - **`24478` conditioned full source closure:** The pair model now handles ordinary/pseudo/empty combinations, including mixed `-1` hand cases by taking **real observed** original allocator-adjacent item category (`OBJECTS_PTR-0x12`), weight (`-0x11`) and qualifier (`-0x0C & 3`) fields ([checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)). No serialized OBJECTS.DAT content is invented to fill these bytes. Live DOS heap capture, source compilation, event/cooldown integration and time-series comparison are still mandatory before playable attacks.

   - **E5F0 attack target and maintenance-clock recovery:** [New checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md) reconstructs explicit-target vs `DB94(actor)` preference, special mode-FF rear-party target suppression, and weapon WORD `+0x35` bits 2..5 multiplied by 9 for authored action duration (unarmed/pseudo: 18); actor `+0x132 & 0x01000010` halves that duration. **Only successful hits** store `A7E4 + floor(interval/2)` into selected hand `+0x19E` and post-damage into `+0x1AA`. The next routine—original logical **`24B48`** (objdump virtual `34B48`)—presents the action, then if interval!=0 writes the separate **`+0x10B` action-state deadline even on MISS/no target**, using a potentially later `29CC4 → A7E4` observation + **full** interval. Both are **pauseable gameplay-maintenance clocks**, never VGA/IRQ or a fixed Windows frame cadence. The pure C++ models retain original `26018`-supplied item ID and have tests registered but **not compiled**. Offline CODE.2 opcode audit is **117 anchors**; 21 new `E5F0`/`29CC4` anchors byte-matched. Next: actual `1E578`, original post-C80C damage/RNG, timed animation and NPC mutation. No automatic CI.

5. **R-078 story transition:** compare CODE.2 Level 22 event sequence to the native last-relic state machine and run the original 22-frame transition with correct Level 23 load and consumed relics.

## Current acceptance boundary

- GitHub Actions [run #572](https://github.com/Hyperbog/RLOFT/actions/runs/37710634731) certified Linux and Windows build/test at the then-current source, including **99/99 Linux CTest** and successful Windows executable publication.
- Following the user's usage-saving instruction, Linux automatic runs are paused. The Windows workflow remains enabled for native code changes and cancels superseded builds. Documentation-only commits are excluded from that path filter.
- The updated native port is **not** called 100% complete until original-versus-native frame/gameplay equivalence and Level 22 finishing behavior are demonstrated.

## Updating the preserved snapshot

The archive files in `../PreservationArchive/` are identical Git blobs copied from the pinned `code1-decompilation` commit above. Make factual changes to the authoritative archive branch, establish supporting original-byte evidence there, then intentionally resynchronize the snapshot; do not independently edit its contents on `demo-v1`.
