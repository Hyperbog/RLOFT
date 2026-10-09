# Ravenloft Demo v1 — Complete System/Sub-system Completion Scorecard

**Baseline date:** 2026-10-09  
**Branch:** `demo-v1`  
**Scope:** Level 22 reusable development testbed and systems needed for the full original game.  
**Source of truth:** [editable scoring data](PROGRESS_SUBSYSTEMS.csv).  
**Current revision acceptance:** Full Windows build [#648](https://github.com/Hyperbog/RLOFT/actions/runs/37998467852) **passed all 178 CTests** at source `fb0912346a831dad21d2960e99863cc8f446173a`, including the Part 9 continuity, mist staging, original pickup-message and IRQ ribbon reconstruction. The [exact build/EXE provenance](FULL_WINDOWS_BUILD_CHECKPOINT_20261009_RUN648.md) is archived. **No current-source original-DOS visual, state, timing or complete Level22 gameplay acceptance has passed.**  
**Separate historical metric:** the pre-existing 32-task engineering ledger remains **56.25% (18/32)**. It is NOT numerically comparable to these weighted readiness estimates.

**Post-build trace checkpoint (unverified source):** the opt-in Level22 session [flight recorder](LEVEL22_DEV_TRACE_CHECKPOINT_20261009.md) now captures indexed VGA pixels/palettes from viewport and modal screens, event/state hashes, original RES3/raster manifests and F12 defect markers, packaged as one ZIP by the new launcher. Source CTests now number **179**, with `dev_session_trace_tests` unbuilt. **Last accepted Full Windows build remains #648 (178/178)**; no original DOS pixel/gameplay acceptance and no score changes.

## Scoring rules — fixed for future checkpoints

**Weights:** original behavior mapped **10%**; native source implementation **20%**; runtime integration **20%**; regression assertions present **10%**; current-revision original DOS behavioral acceptance **40%**.

Each gate in the CSV is scored **0** (not established), **10** (partial / bounded / limited), or **20** (substantially source-backed for this named scope). One subsystem's readiness percentage is `10×source/20 + 20×implementation/20 + 20×live/20 + 10×regression/20 + 40×DOS_acceptance/20`. Scores are **evidence-weighted provisional readiness indicators**, not percent of hours spent, code lines implemented, an objectively quantified feature test coverage metric, or percent of the entire Ravenloft campaign.

In particular, 20 points for “regression” means test cases are written and registered, **not that they passed**. The DOS acceptance gate must remain **zero** until **this exact source revision** has been built, executed, compared and accepted against original DOS behavior. Historical successful builds do not substitute for that. Full DOS acceptance requires source-equivalent pixels/timing/state when the feature involves them. The `0/10/20` confidence assessments and the equal-per-subsystem roll-up can be refined when original acceptance evidence arrives. Never mark a feature 100% solely because the code is written.

## Current quantified baseline

**122 named subsystems across 20 major systems. Provisional weighted readiness: 49.8%.** This is deliberately distinct from the **56.25%** original 32-task engineering completion ledger. The 40%-weighted DOS acceptance component is **0% at this HEAD**; the weighted baseline reflects source archaeology, code, wiring and authored (but unexecuted) assertions. It should not be advertised as a finished game percentage.

| Assessment gate | Average completion within gate | Contribution to overall score |
| --- | ---: | ---: |
| Source mapping | 96.3% | 9.6% |
| Native implementation | 87.7% | 17.5% |
| Live runtime wiring | 70.9% | 14.2% |
| Authored regression coverage | 84.8% | 8.5% |
| Current-head DOS acceptance | 0.0% | 0.0% |
| **Total weighted readiness** | | **49.8%** |

**Level22-first binding gate:** No independent Level23 content/gameplay work before the complete original Level22 is playable from startup through its original mist ending. The original frame-20 Level23 loader is only a required end-of-Level22 boundary. The latest source also reconstructs original pickup BF44 message assignments and the Assassin death BF44=1 mutation. These are not yet a complete original ribbon renderer or accepted original-DOS behavior. See [Level22 acceptance gate](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md) and [BF44 runtime integration](LEVEL22_BF44_LIVE_GLOBAL_20261009.md). **177 authored/registered CTests** (Quick 19), last compiled Full 178/178 (#648).

**Next Level22 work:** authentic picked-object message source bytes are now assembled at the original actual click path and IRQ `30700+0x5A` queue state is retained, using the previously verified source service. No VGA ribbon rendering or original DOS pixel/time comparison is accepted. [Code/evidence](LEVEL22_ORIGINAL_PICKUP_IRQ_RIBBON_20261009.md). The project continues to block independent Level23 work until the full playable Level22 ending.

## Major-system completion percentages

| Major system | Completion | Tracked subsystems | Lowest-scoring next gate |
| --- | ---: | ---: | --- |
| **Startup and supervisor** | **59.0%** | 5 | Full menu sound and timing acceptance |
| **Character creation** | **59.2%** | 6 | Original equipment byte comparison |
| **Party stats and progression** | **50.0%** | 6 | Original rest and progression modal |
| **Level lifecycle and continuity** | **47.9%** | 7 | Live original event-driven transitions and DOS acceptance |
| **Movement and collision** | **52.5%** | 6 | Complete authored door event chain |
| **Mutable world and objects** | **53.3%** | 6 | Accept retained level mutations against original DOS |
| **Perspective and visibility** | **50.8%** | 6 | DOS indexed capture comparison |
| **VGA and graphics resources** | **49.2%** | 6 | Original composite and teardown |
| **HUD menus and overlays** | **51.7%** | 6 | Bind original game actions |
| **Inventory and equipment** | **56.9%** | 8 | Remaining use classes and charge semantics |
| **Magic and spellcasting** | **35.8%** | 6 | Faithful rest and active duration handling |
| **Combat and damage** | **48.1%** | 8 | End-to-end player attack pipeline |
| **NPC encounters and AI** | **51.0%** | 5 | Full AI decision and tick integration |
| **Story events and puzzles** | **48.3%** | 6 | Implement authored event graph |
| **Cinematics and ANM** | **48.0%** | 5 | Bind all authored event IDs |
| **Audio and music** | **48.0%** | 5 | Original live sound routing |
| **Input timing and clock** | **49.2%** | 6 | Independent source clocks integration |
| **Automap and navigation** | **51.0%** | 5 | Compare saved/reentered original DOS automap state |
| **SAVE LOAD and persistence** | **38.6%** | 7 | Multi-level backing across exits |
| **Verification and developer testbed** | **50.0%** | 7 | Full original campaign milestone |

**Part 9 continuation:** General in-memory level-transition and visited-world reentry are source-implemented. No original event exit, disk SAVE/LOAD or DOS fidelity has been accepted. Current registry **178 CTests** (Quick 19), not yet compiled. See [Part 9 checkpoint](LEVEL_TRANSITION_STATE_CHECKPOINT_20261009.md).

**Part 9 source audit update:** F540 duplicate-item removal now preserves the original F5C6→F5D9 continuation; the 21F70 camera staging and 22-frame Level22 mist story timeline are encoded in testable source models. The original LEVELS.DAT Level23 target is 64×64, but there is NO live mist playback or original-DOS acceptance yet. Registered total **178** CTests, 19 Quick, last green compiled **178** at source SHA `fb091234`. Engineering readiness unchanged at 49.8% pending live integration.

## Every subsystem — individual readiness percentages

Each row's five gate marks are out of 20; the weighted readiness column is an individual **percentage**. `S` = source mapping; `N` = native implementation; `L` = live runtime; `T` = authored tests; `D` = current-head DOS acceptance. **0/20 DOS scores in this baseline are intentional, not an assertion that historical work never passed earlier builds.**

### Startup and supervisor — 59.0% (5 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| CODE2 startup and title flow | **60.0%** | 20/20/20/20/0 | `boot_flow.cpp and win32_main.cpp` | Current-head DOS end-to-end playback |
| CINE00 CINE12 intro sequencing | **60.0%** | 20/20/20/20/0 | `anm_cinematic.cpp and win32_main.cpp` | Reference skip and frame chronology |
| Existing CHARS gameplay entry | **60.0%** | 20/20/20/20/0 | `exploration_session.cpp` | Live original-party replay |
| Title menu choices and sound | **55.0%** | 20/20/20/10/0 | `startup_presenter.cpp` | Full menu sound and timing acceptance |
| CODE1 to CODE2 supervisor return | **60.0%** | 20/20/20/20/0 | `boot_flow.cpp and character_transfer.cpp` | Real original full-flow trace |

### Character creation — 59.2% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Opening scene and palette | **60.0%** | 20/20/20/20/0 | `character_creation_presenter.cpp` | DOS frame comparison |
| Race class alignment and restrictions | **60.0%** | 20/20/20/20/0 | `character_creation_flow.cpp` | Full original decision matrix |
| Attributes rolls edits and modifiers | **60.0%** | 20/20/20/20/0 | `character_stats.cpp` | Original RNG replay |
| Portrait names and character visuals | **60.0%** | 20/20/20/20/0 | `character_creation_presenter.cpp` | Exact original UI comparison |
| Initial gear and resources | **55.0%** | 20/20/20/10/0 | `character_transfer.cpp` | Original equipment byte comparison |
| CHARS binary serialization | **60.0%** | 20/20/20/20/0 | `character_transfer.cpp` | Full party DOS runtime roundtrip |

### Party stats and progression — 50.0% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Actor record and status flags | **60.0%** | 20/20/20/20/0 | `exploration_state.cpp` | Original live actor comparison |
| Character stats sheet and XP display | **60.0%** | 20/20/20/20/0 | `character_sheet.cpp` | HUD pixel acceptance |
| Reordering actor lanes | **60.0%** | 20/20/20/20/0 | `inventory_party_order.cpp` | Source mouse drag timing |
| HP injury death and conditions | **50.0%** | 20/20/10/20/0 | `combat_npc_injury_prelude.cpp` | Full character life-cycle wiring |
| Experience and level thresholds | **50.0%** | 20/20/10/20/0 | `combat_xp_distribution.cpp` | Complete XP award runtime chain |
| Rest recovery and level-up UI | **20.0%** | 10/10/0/10/0 | `combat_level_change_sequence.cpp` | Original rest and progression modal |

### Level lifecycle and continuity — 37.9% (7 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Original fresh Level22 entry | **60.0%** | 20/20/20/20/0 | `exploration_session.cpp` | DOS startup to level A B |
| LEVELS and LEVnnn source loading | **60.0%** | 20/20/20/20/0 | `exploration_session.cpp` | All original level variants acceptance |
| Original NPC and object expansion | **60.0%** | 20/20/20/20/0 | `level_world_state.cpp` | All authored level types |
| Authored exit and portal discovery | **25.0%** | 20/10/0/10/0 | `level22_story.cpp` | Wire source exit transitions |
| In-session level-switch lifecycle | **40.0%** | 10/20/10/10/0 | `level_continuity.cpp and exploration_runtime.cpp` | Connect original event exits and verify DOS transition |
| Level reentry and persistent worlds | **40.0%** | 10/20/10/10/0 | `level_continuity.cpp and level_continuity_tests.cpp` | Native original side effects and return visit acceptance |
| Multi-level source-authenticated RES4 ground selection | **50.0%** | 20/20/10/20/0 | `original_ground_res4_audit.cpp and exploration_runtime.cpp` | Validate live Level22 to Level23 transition and ground pixels against original DOS |

### Movement and collision — 52.5% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Q16 movement and turning | **60.0%** | 20/20/20/20/0 | `exploration_motion.cpp` | Original DOS movement timings |
| Grid walking and facing | **60.0%** | 20/20/20/20/0 | `exploration_locomotion.cpp` | Original directional input traces |
| Wall and door collision geometry | **60.0%** | 20/20/20/20/0 | `exploration_collision.cpp` | DOS corridor comparisons |
| Entity occupancy and blocking | **50.0%** | 20/20/10/20/0 | `original_entity_collision.cpp` | Moving NPC collision acceptance |
| Movement-triggered events and doors | **35.0%** | 20/10/10/10/0 | `exploration_world.cpp` | Complete authored door event chain |
| Movement and game IRQ synchronization | **50.0%** | 20/20/10/20/0 | `exploration_gameplay_cadence.cpp` | Real source IRQ timing |

### Mutable world and objects — 49.2% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Cell flags and control records | **60.0%** | 20/20/20/20/0 | `exploration_world.cpp` | Original door and cell mutation trace |
| NPC record expansion and HP | **60.0%** | 20/20/20/20/0 | `level_world_state.cpp` | NPC spawn and death comparison |
| Placed map objects and state | **50.0%** | 20/20/10/20/0 | `placed_scene_catalog.cpp` | General object use script wiring |
| Loose item drop pickup and location | **60.0%** | 20/20/20/20/0 | `inventory_world_drop.cpp` | Whole-game item action acceptance |
| NPC and object respawn logic | **50.0%** | 20/20/10/20/0 | `combat_npc_respawn.cpp` | All original respawn cases |
| World carryover across level transitions | **40.0%** | 10/20/10/10/0 | `level_continuity.cpp and exploration_runtime.cpp` | Original NPC and world item reentry comparison |

### Perspective and visibility — 50.8% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| SINTAB fixed-point perspective | **60.0%** | 20/20/20/20/0 | `fixed_projection.cpp` | Full DOS perspective trace |
| Wall traversal and visibility | **60.0%** | 20/20/20/20/0 | `view_sweep_traversal.cpp` | Reference occlusion coverage |
| Ground RES4 raster identity | **60.0%** | 20/20/20/20/0 | `original_ground_res4_audit.cpp` | Ground pixel DOS alignment |
| Main wall raster composition | **40.0%** | 20/10/10/20/0 | `surface_sintab_main_executor.cpp` | Complete original wall pixels |
| Sprite depth occlusion and sorting | **50.0%** | 20/20/10/20/0 | `world_scene_selection.cpp` | Visible world ordering acceptance |
| Full framebuffer equivalence | **35.0%** | 20/10/10/10/0 | `exploration_presenter.cpp` | DOS indexed capture comparison |

### VGA and graphics resources — 49.2% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| RES archive decode and images | **60.0%** | 20/20/20/20/0 | `resource_assets.cpp` | Complete original source asset acceptance |
| VGA planar masks and sequencing | **50.0%** | 20/20/10/20/0 | `vga_planar_memory.cpp` | DOS plane trace integration |
| Sprite transparency clipping and blits | **50.0%** | 20/20/10/20/0 | `combat_original_sprite_raster.cpp` | Real scene overlay timing |
| Palette shading and fades | **50.0%** | 20/20/10/20/0 | `startup_presenter.cpp` | Source 6-bit DAC chronology |
| RES3 type7 raster machine | **50.0%** | 20/20/10/20/0 | `type7_scene_pixel_executor.cpp` | DOS sprite raster acceptance |
| Page stacking and final composition | **35.0%** | 20/10/10/10/0 | `combat_original_overlay.cpp` | Original composite and teardown |

### HUD menus and overlays — 51.7% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| In-game HUD and portraits | **60.0%** | 20/20/20/20/0 | `party_hud.cpp` | Original HUD framebuffer comparison |
| Top-edge menu and actions | **50.0%** | 20/20/10/20/0 | `top_edge_menu.cpp` | Complete rest and save menus |
| Party and inventory modal | **60.0%** | 20/20/20/20/0 | `party_modal_presenter.cpp` | Full DOS modal pixel and timing |
| Mouse keyboard and hit regions | **60.0%** | 20/20/20/20/0 | `party_panel_controls.cpp` | Original mouse event mapping |
| Quit confirmation and menus | **60.0%** | 20/20/20/20/0 | `quit_confirm.cpp` | Original modal exit acceptance |
| Options rest pray memorize load save | **20.0%** | 10/10/0/10/0 | `options_menu.cpp` | Bind original game actions |

### Inventory and equipment — 56.9% (8 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Object definitions and item metadata | **60.0%** | 20/20/20/20/0 | `object_catalog.cpp` | Full original object category actions |
| Cursor and slot identified state | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp` | Original rare inventory edge cases |
| Equipment hand and class gates | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp` | Original all-class matrix |
| Nested containers and capacity | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp` | Timing and DOS nested UI |
| Four actor container persistence | **60.0%** | 20/20/20/20/0 | `inventory_save_codec.cpp` | Multiple actor source runtime replay |
| Encumbrance and party order | **60.0%** | 20/20/20/20/0 | `inventory_encumbrance.cpp` | Reference weight item cases |
| World drop pickup routing | **60.0%** | 20/20/20/20/0 | `inventory_world_drop.cpp and level22_encounter.cpp and original_pickup_message.hpp and original_pickup_ribbon_text.cpp` | Connect original 1C258 VGA ribbon rendering and DOS timing parity |
| Special item use and charges | **35.0%** | 20/10/10/10/0 | `inventory_runtime.cpp` | Remaining use classes and charge semantics |

### Magic and spellcasting — 35.8% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Mage spell book matrix and IDs | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp` | Original full spell bank roundtrip |
| Mage scroll scribing intelligence gating | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp` | DOS original message and success case |
| Cleric memorization and preparation | **25.0%** | 20/10/0/10/0 | `combat_level_spell_tables.cpp` | Original interactive prayer UI |
| Spell target selection and range | **25.0%** | 20/10/0/10/0 | `combat_hand_dispatch.cpp` | Source target and LOS action wiring |
| Active spells and magic combat effects | **25.0%** | 20/10/0/10/0 | `combat_original_text_runtime.cpp` | Implement verified effect dispatcher |
| Spell recovery and timer advancement | **20.0%** | 10/10/0/10/0 | `spell and time source notes` | Faithful rest and active duration handling |

### Combat and damage — 48.1% (8 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Combat hit and miss calculation | **50.0%** | 20/20/10/20/0 | `combat_hit_check.cpp` | Wire original gameplay hand action |
| Weapon and class eligibility | **50.0%** | 20/20/10/20/0 | `combat_hand_dispatch.cpp` | Input hand resolution integration |
| Damage dice and bonuses | **50.0%** | 20/20/10/20/0 | `combat_damage_dice.cpp` | Source combat timing and live RNG |
| Attack timing and cooldown | **50.0%** | 20/20/10/20/0 | `combat_attack_timing.cpp` | Source IRQ synchronization |
| NPC health injury and death | **60.0%** | 20/20/20/20/0 | `combat_npc_death_followon.cpp` | DOS casualty state comparison |
| Kill rewards XP and loot | **50.0%** | 20/20/10/20/0 | `combat_xp_distribution.cpp` | Live source-ordered award chain |
| Level-up progression and HP | **50.0%** | 20/20/10/20/0 | `combat_level_up_hp.cpp` | Connect level-up to party modal |
| Full input attack to reward loop | **25.0%** | 20/10/0/10/0 | `combat_* model workstream` | End-to-end player attack pipeline |

### NPC encounters and AI — 51.0% (5 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| NPC catalog and original records | **60.0%** | 20/20/20/20/0 | `npc_catalog.cpp` | Original full level archive comparison |
| NPC actions movement and pursuit | **35.0%** | 20/10/10/10/0 | `level22_encounter.cpp` | Full AI decision and tick integration |
| Encounter target selection and priority | **50.0%** | 20/20/10/20/0 | `combat_target_priority.cpp` | Source live combat replay |
| Assassin Level22 behavior | **60.0%** | 20/20/20/20/0 | `level22_encounter.cpp` | Original assassin full interaction |
| Respawn and scripted return | **50.0%** | 20/20/10/20/0 | `combat_npc_respawn.cpp` | All level authored spawn events |

### Story events and puzzles — 48.3% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Event flag bitset read and write | **60.0%** | 20/20/20/20/0 | `inventory_runtime.cpp + exploration_session.cpp` | Original source story sequences |
| Level22 relic and mist script | **50.0%** | 20/20/10/20/0 | `level22_story.cpp and level22_mist_timeline.cpp and original_mist_staging.hpp and original_numbered_effect_flags.hpp and exploration_runtime.cpp` | Finish Level22 player combat and original mist ending before Level23 |
| Source-triggered doors traps and switches | **35.0%** | 20/10/10/10/0 | `exploration_world.cpp` | Complete authored event set |
| Level16 four-Seed ritual condition | **60.0%** | 20/20/20/20/0 | `inventory_world_drop.cpp` | Source playback DOS trace |
| Level16 Holy Symbol reward | **60.0%** | 20/20/20/20/0 | `inventory_world_drop.cpp` | Cinematic teardown and accepted reward |
| Quest event graph all levels | **25.0%** | 20/10/0/10/0 | `CODE.2 research archive` | Implement authored event graph |

### Cinematics and ANM — 48.0% (5 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Original ANM packet and frame decode | **60.0%** | 20/20/20/20/0 | `anm_cinematic.cpp` | All event resource acceptance |
| Intro event 0 and 12 playback | **60.0%** | 20/20/20/20/0 | `win32_main.cpp` | Original frame music and skip timings |
| Event8 in-game live scene handoff | **60.0%** | 20/20/20/20/0 | `inventory_world_drop.cpp + win32_main.cpp` | DOS event cancellation comparison |
| Event audio palette and graphics teardown | **35.0%** | 20/10/10/10/0 | `CODE.2 13C88 audit` | Exact original 13C88 state restore |
| Other story cinematics and transitions | **25.0%** | 20/10/0/10/0 | `CINE data archive and CODE.2 graph` | Bind all authored event IDs |

### Audio and music — 48.0% (5 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| VOC sample decode and play | **60.0%** | 20/20/20/20/0 | `voc_decoder.cpp + win32_audio.cpp` | Original SFX timing and channel check |
| XMIDI sequence playback | **60.0%** | 20/20/20/20/0 | `xmidi.cpp + win32_midi.cpp` | Original tick and output comparison |
| Startup and creation audio routing | **60.0%** | 20/20/20/20/0 | `win32_main.cpp` | Source fade stop and loop verification |
| Music transitions and scene restoration | **35.0%** | 20/10/10/10/0 | `win32_midi.cpp + CODE.2 13C88 audit` | Full event audio and volume rollback |
| Positional in-world SFX and prioritization | **25.0%** | 20/10/0/10/0 | `sound_config.cpp + source sound research` | Original live sound routing |

### Input timing and clock — 49.2% (6 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Mouse and keyboard input decoding | **60.0%** | 20/20/20/20/0 | `exploration_legacy_input.cpp` | DOS physical control traces |
| Party inventory modal event sequencing | **60.0%** | 20/20/20/20/0 | `win32_main.cpp + party_panel_controls.cpp` | Original press hold and release mapping |
| Movement IRQ cadence | **50.0%** | 20/20/10/20/0 | `exploration_gameplay_cadence.cpp` | Source real PIT frequency trace |
| Cinematic IRQ and frame waits | **55.0%** | 20/20/20/10/0 | `anm_cinematic.cpp` | Original event pause and frame pacing |
| Timed spell combat and event effects | **35.0%** | 20/10/10/10/0 | `combat_irq_display_runtime.cpp` | Independent source clocks integration |
| State restoration after suspend and resume | **35.0%** | 20/10/10/10/0 | `win32_main.cpp` | CINE08 DOS mode exit acceptance |

### Automap and navigation — 47.0% (5 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Level cell discovery and visited cache | **60.0%** | 20/20/20/20/0 | `exploration_world.cpp` | Full source exploration trace |
| Map labels markers and geometry | **60.0%** | 20/20/20/20/0 | `automap_annotation_presenter.cpp` | Original on-screen DOS comparison |
| Automap modal input and pan controls | **60.0%** | 20/20/20/20/0 | `automap_modal.cpp + automap_controls.cpp` | Source mouse exit and pointer timing |
| Map reset and state across levels | **40.0%** | 10/20/10/10/0 | `level_continuity.cpp and level_continuity_tests.cpp` | Original DOS level-local automap comparison |
| Special map overlays and triggers | **35.0%** | 20/10/10/10/0 | `automap_presentation.cpp` | All authored special map indicators |

### SAVE LOAD and persistence — 38.6% (7 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| CHARS preservation and startup load | **60.0%** | 20/20/20/20/0 | `character_transfer.cpp + GameSource` | Original end-to-end file behavior |
| Inventory SAVE codec 62 containers | **60.0%** | 20/20/20/20/0 | `inventory_save_codec.cpp` | Full DOS byte-range roundtrip |
| Four 6FA0 container actor SAVE records | **60.0%** | 20/20/20/20/0 | `inventory_save_codec.cpp` | Source active animated-save acceptance |
| Story event flag and clock state SAVE | **50.0%** | 20/20/10/20/0 | `inventory_save_codec.cpp` | Full source world timeline restore |
| World NPC effects loose items SAVE | **25.0%** | 20/10/0/10/0 | `level_world_state.cpp` | Implement global persistent full SAVE |
| In-game SAVE LOAD menus and slots | **10.0%** | 20/0/0/0/0 | `options_menu.cpp` | Original serialized file I O and menu wiring |
| Level re-entry and multiple-world saves | **5.0%** | 10/0/0/0/0 | `CONTINUOUS_GAME_INTEGRATION_CONTRACT` | Multi-level backing across exits |

### Verification and developer testbed — 50.0% (7 subsystems)

| Subsystem | Readiness | S/N/L/T/D | Source implementation evidence | Next acceptance blocker |
| --- | ---: | --- | --- | --- |
| Original file provenance and byte audits | **60.0%** | 20/20/20/20/0 | `source archive docs + DAT CODE audits` | Complete all remaining original unknowns |
| Automated regression test catalog | **60.0%** | 20/20/20/20/0 | `CMakeLists.txt 179 CTests (trace one unbuilt)` | Original DOS cross-comparison and current-head build |
| Opt-in Level22 state diagnostics | **60.0%** | 20/20/20/20/0 | `dev_level_diagnostics.cpp and dev_session_trace.cpp and win32_main.cpp` | Full Windows build and user session ZIP validation |
| Opt-in Level22 world pickup and hand exchange | **60.0%** | 20/20/20/20/0 | `dev_level_interaction_policy.hpp and win32_main.cpp` | Current-head manual interaction trace and DOS source behavior |
| Visual DOS captures and comparisons | **45.0%** | 20/20/10/10/0 | `exploration_frame_capture.cpp` | Complete DOS/native pixel comparison |
| Manual Windows build reproducibility | **60.0%** | 20/20/20/20/0 | `github workflow and CMakeLists.txt` | Full Windows build 648 passed 178 of 178; trace code now needs next manual build |
| Original start to game-end acceptance | **5.0%** | 10/0/0/0/0 | `CONTINUOUS_GAME_INTEGRATION_CONTRACT` | Full original campaign milestone |

## Mandatory reporting format for ALL future checkpoints

1. Report **the latest committed Git branch and SHA** and **test/build execution status**, without claiming a compiled binary when none was built.
2. Report the **overall weighted readiness**, all **20 major-system percentages**, and the **changed subsystem percentage(s)** with concrete source evidence and remaining blockers. Refer to this complete table for every other subsystem; provide the full subsystem view on request or at major audit checkpoints.
3. When a subsystem is investigated or changed, update its five CSV gate scores and blocker, then regenerate the major-system and overall roll-ups **in the same Git checkpoint**. Unchanged areas keep their previous scores; create a new named row for an entirely new subsystem.
4. Keep the **32-task 56.25% engineering score** separate until that ledger changes according to its own defined task-state criteria. Never average these two measures.
5. **Git commits happen continuously. Windows builds, CI runs and DOS acceptance runs remain manual or user-authorized.** Compiled/accepted statuses cannot advance on source-only work.

## Highest-risk work regardless of individual averages

The development slice cannot become a faithful integrated game without completing the original **full-world SAVE/LOAD**, general **in-session level lifecycle and persisted worlds**, **combat input→damage→death→XP live sequencing**, remaining **spell uses and original rest/pray flows**, and **source-equivalent VGA/frame/audio/IRQ timing**, verified with current-head Windows and DOS acceptance. All level-specific scripts and transitions must enter via the shared original startup/character session, not separate prototypes.

[Continuous-game contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md) · [Level22 developer testbed](DEV_LEVEL_TESTBED_20261009.md) · [existing engineering ledger](DEMO_PROGRESS_SCORECARD_20261008.md)
