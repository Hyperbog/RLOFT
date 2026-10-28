# Whole-game original-source lifecycle — master execution and evidence matrix

**Date:** 2026-10-28. **Branch:** Recomp. **Status:** audit / implementation roadmap, NOT a completion certificate.

## Non-negotiable definition of success

A user starts the native executable with authentic, unmodified original media, sees the original publisher/intro/cinematics/title sequence, enters the original main menu, starts or loads a game, creates and manages a party, moves through the complete campaign, interacts with characters and objects, fights, casts spells, hears music and effects, saves and loads through normal UI, reaches the original ending, and exits. All original CODE.1/CODE.2 gameplay decisions must come from compiled reconstructed source; the host supplies OS/DOS/4GW, video, sound, keyboard, mouse, clock and file services.

A test-only saved-game injection, private key command, scene screenshot, isolated source function, or green CI run is **not** a pass for a player-visible lifecycle stage.

## Lifecycle reference sheet

Legend: **U** = unverified / not demonstrated end-to-end; **P** = partial original-source engineering evidence, not playable acceptance; **A** = independently accepted whole stage. No stage below is A.

| Order | Player-visible stage | Original source / infrastructure to trace | Existing evidence and exact limitation | Status | Required player-visible proof |
|---|---|---|---|---|---|
| 00 | Installation, original files, launch | R.EXE supervisor, CODE.1/CODE.2 LE loading, DOS/4GW, memory, disk | Original LE and compiled initializers; native engineering package; ordinary full retail boot unverified | P | Clean install launches without debug flags, injected saves, or missing service |
| 01 | Publisher splash, logos, intro | CODE.1 startup sequence, resource/file/video/audio calls | Staged source boot is not evidence of full presentation | U | Capture every original screen in correct order and timing |
| 02 | Intro cinematic / animation | Original cinematic dispatch, frames, palette, input skip and sound | No verified full original cinematic playback | U | Compare complete scene sequence, timing, palette, soundtrack and skip |
| 03 | Title screen and main menu | CODE.1 title/menu event loop, original keyboard/mouse dispatch | No accepted normal native title/menu flow | U | Navigate title, new/load/options/quit using ordinary inputs |
| 04 | New game / character creation | CODE.1 508C, CHARS persistence, CODE.2 handoff | Reconstructed CODE.1 compiles; source character creation/handoff not accepted (R06) | U | Create full party through original UI, enter game, verify CHARS state |
| 05 | Load existing game via menu | Original DOS file services, CODE.2 07938, 181B4 | Original saved-state restoration tested via private entry, not load UI | P | Select saved slot through title/in-game UI; restored state matches |
| 06 | First normal playable world | CODE.2 1FC14, 181B4, original RES/VGA/palette | Original saved Level22/23 frame probes; first normal new-game transition not accepted | P | Start newly created game, load first world, render all HUD |
| 07 | Walking, turning, doors and transitions | 30B40, 308F8, 2E120, 2D5D0, 2D3D8, IRQ9/timer | Partial physical input and private 23→26→23 portal probes | P | Continuous ordinary keyboard/mouse traversal and doors |
| 08 | Graphics, animations, cutscenes during play | 1F8FC, 3263C, 2C294, 36CEC, RES loaders | Native source VGA frames for sampled states only | P | Real-time frames, palettes, sprites, animations across game |
| 09 | Music, sound effects and speech where shipped | 28110, 28168, 27F24, AIL/DOS device services | Partial queue and native audio bridge; audible original parity unverified | P | Original music/SFX events audible and synchronized |
| 10 | Party, inventory, equipment, items | Original HUD, item/actor callbacks, DOS mouse | Source HUD paths sampled; interactive inventory not accepted | U | Pick up, equip, transfer, use and drop items |
| 11 | NPCs, dialogue, shops/quests | 1C77C, 1C368, 002A8, 2D5D0 | Source dialogue-text/script preflight tests only | P | Full original conversation and quest state changes |
| 12 | Combat, AI, damage, death, spells | 0D550, 1BDAC, actor update and spell routines | Isolated original-source combat/actor tests, not full combat | P | Fight enemies, resolve AI, damage, spells, death/revival |
| 13 | Save game through normal UI | 171FC, DOS file create/write/seek/close | Source persistence probes; no accepted user-driven save | P | Save in UI, quit, restart, reload identical state |
| 14 | Campaign-wide progression | All LEVELS/INFO/NPCS/OBJECTS, 002A8 scripts | 32 level archive records indexed; no all-level playability evidence | U | Complete original campaign with unmodified content |
| 15 | Ending, credits, game over, exit | 0CBC8, 2E614, original termination/cleanup | Game-over source tests, no end-to-end original ending | U | Reach and display authentic ending/credits and clean exit |

## Dependency-first implementation order

1. **Boot and presentation vertical slice:** trace R.EXE → CODE.1 initializers → intro/title/menu → New/Load selection → CODE.2 transition. Build a source-call/service reference sheet for every blocking branch. Do not start from an injected saved world.
2. **Universal host services:** implement DOS file APIs (including writes), heap/selector lifetime, VGA mode/palette/page changes, original sound driver service boundaries, timer/IRQ, keyboard and mouse callback scheduling. Each service must be generic and bounded.
3. **Character creation to first normal frame:** execute original 508C and CHARS handoff into original 1FC14. This is the first meaningful playable milestone.
4. **Continuous gameplay:** original movement and mouse interactions, inventory, NPC dialogue, combat, scripts, audio and animations on the same retained guest, not separate harnesses.
5. **Normal persistence and full campaign:** menu save/load and roundtrip, all level transitions, quests, ending and shutdown.
6. **Original DOS parity:** reproducible side-by-side captures of screen/audio/input/file state from matching scenarios; record mismatches explicitly.

## Source and test reference sheet

| Domain | Existing entry / reference | Work product needed |
|---|---|---|
| Executable entry and handoff | `Recomp/src/code1_code2_staged_boot.cpp`, `Recomp/src/code1_startup_gate.cpp`, `Recomp/src/code2_startup_gate.cpp`, `Code1/Code1.cpp`, `Code2/Code2.cpp` | Exact startup call tree and first unsupported host service |
| Canonical code/guest memory | `Recomp/src/canonical_code2_runtime_owner.hpp`, `Recomp/src/original_guest_continuation.hpp` | Retained-guest service contract and error ledger |
| Title / character creation | Original CODE.1 508C; `Recomp/src/original_code2_chars_file_access.hpp` | Original intro/menu/508C route and CHARS handoff receipts |
| Original media / DOS I/O | `Recomp/src/dos_readonly_files.hpp`, `Recomp/src/code2_original_save_game.cpp` | Full DOS filesystem read/write API inventory and persistent save contract |
| Frame/input | `Recomp/src/original_code2_level22_window.cpp`, `Recomp/src/original_dpmi_real_mode_mouse.hpp` | Complete IRQ9 and INT33 event callback route, real-time source worker |
| Renderer and cinematics | `Code2/graphics/`, `Recomp/src/original_code2_video_boot.hpp` | Original source graphics-mode/palette/animation/intro call graph |
| Audio | `Recomp/src/original_source_winmm_bridge.hpp`, `Recomp/src/host_original_voc_waveout.cpp` | AIL call contract, device scheduling and synchronized sound receipt |
| Mechanics | `Recomp/src/test_original_code2_1bdac_0d550_actor_source.cpp`, `Recomp/src/test_original_code2_002a8_script_source.cpp` | Complete interactive actions and end-to-end gameplay receipts |
| Gates | `Recomp/ACCEPTANCE_GATES.json` | Preserve 3/12 accepted; do not relabel partial work as gameplay completion |

## Reporting contract

Every future checkpoint must state: (a) the **player-visible lifecycle stage** it advances, (b) exact original-source path, (c) the missing dependency removed, (d) evidence from an unmodified original-media run, (e) what still fails, and (f) reproducible commit and CI run. Distinguish **code committed**, **CI passed**, **original-media integration passed**, and **retail DOS parity passed**. If no player-visible stage advanced, say so explicitly. No progress percentages based on test counts, levels visited or code size.

**At creation:** no whole lifecycle stage has been independently accepted. Existing acceptance ledger remains 3/12 infrastructure-level gates; those are not three completed lifecycle stages.
