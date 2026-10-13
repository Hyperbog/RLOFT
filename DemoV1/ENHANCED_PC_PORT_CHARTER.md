# Ravenloft: Strahd's Possession — Enhanced Native PC Port Charter

**Effective:** October 13, 2026. **Owner decision:** Replace the original-DOS-fidelity-first release objective with **one modern, playable, enhanced native PC port**. **Active branch:** `demo-v1`. This charter supersedes the previous project policy in [ORIGINAL_EXECUTION_FIDELITY_CHARTER.md](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md). That document remains available as a historical research/fidelity reference, not as the controlling engineering or release gate.

## Product

Deliver a **self-contained native Windows PC game executable plus legitimate access to user-provided original game assets**; do not require DOSBox, an emulated DOS guest, or a static recompiler to play the port. Preserve the recognizable game—art direction, opening, campaign levels, geography, story, puzzles, characters, monsters, items, D&D systems and ending—while replacing obsolete hardware and host interfaces with contemporary PC services.

DOSBox running the shipped game remains an **optional historical/reference experience** for people who want the original. Original `CODE.1`, `CODE.2`, files, offsets and existing decompilation research are **research inputs and compatibility specifications**, **not constraints that every DOS instruction, interrupt, renderer side effect, or timing quirk must be reproduced**. Keep all original archival material and branch `DnD-Classics` intact.

## Priority order

1. **Never regress already functional work.** Preserve and test established DreamForge/SSI/TSR screens, CINE00/CINE12, soundtrack, sampled sound and speech, the opening lady/cat sequence and correct card-flip direction, title menu and complete character creation. Repair player-visible audiovisual failures before further esoteric hardware research.
2. **One continuous player experience.** Launch one game—not a selector, disassembler, asset editor, DOS guest or set of unrelated demos—from opening presentation through two-person party creation into authored Level 22, then through the whole campaign. Keep separate developer tooling out of the player path. A first-time asset setup is allowed; no recurring mode/folder chooser.
3. **Make Level 22 actually playable and visually complete.** Restore the identifiable sky, ground, distance/background, scenery/trees, assassin NPC, world placement, animations, HUD, movement and encounter pacing from shipped data. Visible output and normal gameplay matter more than passing isolated source audits.
4. **Finish the entire campaign.** Restore all authored levels and transitions, event scripts, NPC interactions and combat, magic, inventory, items, puzzles, quests, ending, and reliable save/load. Reuse verified parsers, assets, data models and rules already in the repository rather than redoing their reverse engineering.
5. **Then enhance deliberately.** Modern graphics presentation (aspect-correct windowed/fullscreen/resizable scaling and optional filters), smooth vsync/frame pacing, robust audio and volume controls, remappable keyboard/mouse/gamepad where feasible, accessibility, usability, stability, and optional quality-of-life features. Maintain an unaltered core rules/content profile by default unless explicitly agreed otherwise. No arbitrary gameplay or story rewrites disguised as fidelity.
6. **Ship with responsible distribution.** Neither public source nor release artifacts may contain unlicensed original game assets. Use the owner's legitimately acquired files or another explicitly authorized asset path. Make setup simple and do not modify the owner’s original saves.

## Scope cuts — no longer mandatory release blockers

- Byte-identical `CODE.1`/`CODE.2` decompilation, recompilation or ABI layout; exact DOS `R.EXE` module transitions and full `CHARS`/`SAVE##` byte compatibility are **optional compatibility projects**, not release prerequisites. Continue using these materials for information where useful.
- Exact VGA planar writes, source-addressed drawing instruction order, 70.0863 Hz/BIOS/PIT/IRQ emulation, port `0x60` keyboard packets, DOS mouse device services, audio driver and channel timing, original memory corruption or DOS-specific error paths.
- Bit-exact frame/palette/audio captures or matched DOSBox playback as **mandatory evidence** for Alpha/Beta/Final.
- Exact reproduction of glitches, unsafe directory clearing, string mutations, platform constraints or defective asset/error handling. Fix bugs safely when required for modern usability and record material differences.
- A DOS executable, DOSBox bundling, a static recompiler, original-executable-distribution or a separate level-warp preview as the primary game.

**Not cut:** original authored campaign content, system logic, stable persistence, major intended visual/audio identity, character creation, user controls, and end-to-end game completion. An enhanced port is not permission to substitute placeholders for a missing game.

## Engineering method

1. **Reuse first.** Inspect existing `DemoV1` front-end/engine, `native-engine`, `Code1/Code1.cpp`, `Code2/Code2.cpp`, asset tools and source reports. Repair and integrate functional subsystems instead of making competing versions.
2. **Distinguish intent from incidental DOS behavior.** Reconstruct core game mechanics with original data and source as needed; choose modern Windows/SDL-style portability services for input, renderer, scheduler and audio where they improve stability. Do not silently claim precise original behavior where it is not known; document intentional deviations.
3. **Prioritize playable vertical slices.** Build a single normal startup → visible Level 22 → controllable encounter → save/reload proof; then expand level-by-level to the ending. Test on the Windows player build with screenshots/video and real-world timings, not just unit/source-byte checks.
4. **Run automated tests and user tests independently.** Source audits and existing CTests remain valuable, but a strict DOS provenance contract may be retired/reclassified when it blocks legitimate native improvements. Record actual compiled SHA, regression counts, visual/sound observations and unresolved defects separately.
5. **Preserve the baseline.** The currently established ANM/music/SFX/title/character code is protected as an initial regression snapshot. Changes are permitted only with a replacement that demonstrably works as well or better (tests and actual Windows observations), not a blanket prohibition on modernizing these files.
6. **Keep portability and source separation in mind.** Prefer reusable portable engine subsystems under `DemoV1`, with a thin platform host. Initial target is Windows x64; other platforms can follow after a stable playable port. Never make a platform rewrite a prerequisite to completing gameplay.
7. **Risk policy.** Never overwrite owned game installs or saves during automated tests. Favor explicit import and safe/atomic new save files over intentionally reproducing the original unsafe `WORK` directory clearing. Define versioned port save format and migration only when tested. Original-format saves may be imported/exported later if achievable and legal.

## Acceptance, without redefining current progress

**Alpha:** single click through opening/title/creation to a visually correct, audible, controllable Level 22 start; no extra chooser; crash-free first gameplay; verified native Windows package and owner field acceptance. Incomplete title load can be deferred if clearly unavailable rather than misleading users.

**Beta:** a start-to-ending campaign can be completed on the native port with all required original levels, story, mechanics, encounters, transitions and **port-native** save/load. Convenience features and design polish may still be in progress.

**Final:** stable, polished enhanced PC experience: whole game, audio, graphics, display/input options, save security, installation guidance, supported hardware, package integrity and owner acceptance. Original DOS parity or byte-for-byte save compatibility is *not* a final gate.

The previous Alpha/Beta/Final gate **IDs remain stable** to keep evidence history, but their acceptance wording and proof requirements change in [RELEASE_GATES.json](RELEASE_GATES.json). **Do not mark old open gates as met by merely relaxing the policy.** Previously completed source-verification audits remain true historical findings; they are no longer on the critical path unless they resolve player-visible gameplay problems.

## Current first execution order

1. Build and observe the real **continuous opening → Level 22** Windows artifact; validate audiovisual intro and card directions.
2. Correct blank/flickering gameplay viewport and missing sky/ground/trees/assassin; check frame pacing, sound and input.
3. Enable Level 22 interaction, encounter, world state, and safe port saves.
4. Expand to authored level-to-level continuity and the full campaign.
5. Deliver modern window/fullscreen scaling, audio mixing, accessible controls and polish in parallel where they help gameplay rather than block it.

**Controlling policy precedence:** this enhanced-port charter > new release ledger / milestones > historical DemoV1 fidelity charter and 2026-10-09 continuous-game audit. The older documents remain preserved and may be cited for evidence, not enforced as prohibitions on enhancement.
