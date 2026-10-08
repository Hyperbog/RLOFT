# Original Preservation Archive → Demo v1 integration map

## Provenance and boundary

The authoritative original-technology archive was produced on `code1-decompilation`. This directory's parent preservation snapshot was copied into `demo-v1` using the **identical 24 Git blob object IDs**, not regenerated prose, from source commit:

`5e70d40957093fb85e555fbc5f4dbe87fa2fea67`

The certified archive documents 22/22 numbered *historical static* chapters 00–21; Chapter 22 adds a separate **91-strand forward-looking backlog**. See [the static certification](../PreservationArchive/FINAL_STATIC_DOCUMENTATION_CERTIFICATION.md) and [open research register](../PreservationArchive/22_REMAINING_RESEARCH_AND_FUTURE_EVIDENCE_ROADMAP.md). Neither the certification nor a successfully compiled C++ reimplementation proves full original-game runtime behavior.

**Evidence precedence:** immutable original DOS binary bytes → instruction-accounted certified reconstruction → corroborated archive analysis → native-port fixture. Pin any new behavioral finding to its original CODE.1/CODE.2 offset and archived evidence, not merely to a modern source name.

**Important branch divergence:** `Code1/Code1.cpp` already shares the authoritative Git blob `226c563007ac4daef8f732505341175914ba2b94` between both branches. The certified CODE.2 C++ blob on `code1-decompilation` is `c3f16122e79c3c464382e4511d9d932f6fcd694a`, but the demo branch's `Code2/Code2.cpp` is `87f6638921c53a32bde59735515fbc175e2c96ee`. **Do not silently replace either**: reconcile the byte/instruction-level difference against the certified audit and original binary before deciding which source revision should be synchronized. This difference does *not* mean the original CODE.2 file changed.

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

## Implementation order, using the archive to avoid rediscovery

1. **R-072 traceability baseline:** reconcile CODE.2 C++ branch divergence, cite source offsets for the existing Level 22 native routines and isolate any remaining inferred behavior.
2. **R-076 visible fidelity:** use Ch.04 and certified rasterizer evidence to finish the type-6 walls/prepass and compare exact indexed pixels against original DOS reference scenes. Current synthetic SINTAB tests do not replace this acceptance criterion.
3. **R-061/R-079 pointer and item behavior:** use Ch.07 §15–24 and §31–55 to implement correctly ordered hand/cursor swap and inventory panel inputs. The current host click→loose-item bridge intentionally refuses pickup while holding another item.
4. **R-045/R-075 combat:** implement the original attack hit/damage model from Ch.12 and actor state from Ch.11 before enabling arbitrary viewport attack clicks. Never synthesize a damage roll merely because an NPC is clicked.
5. **R-078 story transition:** compare CODE.2 Level 22 event sequence to the native last-relic state machine and run the original 22-frame transition with correct Level 23 load and consumed relics.

## Current acceptance boundary

- GitHub Actions [run #572](https://github.com/Hyperbog/RLOFT/actions/runs/37710634731) certified Linux and Windows build/test at the then-current source, including **99/99 Linux CTest** and successful Windows executable publication.
- Following the user's usage-saving instruction, Linux automatic runs are paused. The Windows workflow remains enabled for native code changes and cancels superseded builds. Documentation-only commits are excluded from that path filter.
- The updated native port is **not** called 100% complete until original-versus-native frame/gameplay equivalence and Level 22 finishing behavior are demonstrated.

## Updating the preserved snapshot

The archive files in `../PreservationArchive/` are identical Git blobs copied from the pinned `code1-decompilation` commit above. Make factual changes to the authoritative archive branch, establish supporting original-byte evidence there, then intentionally resynchronize the snapshot; do not independently edit its contents on `demo-v1`.
