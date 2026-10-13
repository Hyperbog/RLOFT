# Enhanced Ravenloft native PC port — Alpha / Beta / Final

**Active owner policy:** [Enhanced PC Port Charter](ENHANCED_PC_PORT_CHARTER.md).
**Machine-readable acceptance:** [RELEASE_GATES.json](RELEASE_GATES.json).
**Branch:** `demo-v1`. **Status:** **Pre-Alpha** (all 10 Alpha, 10 Beta and 10 Final gates remain open). Previous DOS-fidelity-first milestone policy was superseded October 13, 2026, and is available in Git history.

## Goal and evidence

Build a real Windows x64 **enhanced native PC port** of the complete *Ravenloft: Strahd's Possession* game. People seeking the unmodified DOS experience can use their original game with DOSBox. The port reuses shipped original game assets supplied by the player and the existing extensive reverse engineering; it need not reproduce the original x86/DOS/VGA/IRQ/mixer machinery.

Release evidence is **playability-first**, not count-of-reconstructed-routines-first:

| Evidence | Required use | Not evidence of |
| --- | --- | --- |
| `native_field` | Observe the real Windows game: visuals, audio, frame pacing, input, story and save/reload | Other unvisited gameplay branches |
| `windows_ci` | Exact-source native Windows build and automated regression tests | Actual screen/audio quality |
| `package_audit` | Verified EXE, supported original asset layouts, no unlicensed asset redistribution or original-save writes | Gameplay readiness |
| `owner_acceptance` | Recorded approval of a reproducible milestone field test | A missing build or renderer |
| `source_oracle` | Optional original-code/asset research for intended systems and tricky data | Necessity of DOS-perfect execution |
| `dos_comparison` | Optional differential reference when useful to settle a gameplay/content question | A mandatory native release condition |

Any gate marked `met` needs concrete evidence references pinned to a source SHA; the [automated validator](tools/check_release_gates.py) continues to reject fake promotion, missing evidence, stage errors and accidental reinsertion of mandatory DOS comparisons. No historical CTest count or successful old build closes a current player acceptance gate.

## Alpha — single-click opening to visible and controllable Level 22

1. A normal Windows launch runs the intro cinematics, DreamForge/SSI/TSR logos and animated title, with music, voice and sound effects. Preserve existing verified lady/cat animation pacing, character-creation card direction, and original look; fix observed bugs rather than rewrite from scratch.
2. Character creation builds both actual party members and enters the real authored Level 22 *in the same native process*, not a synthetic preview or developer warp.
3. First gameplay view visibly includes proper ground/sky/background/trees, assassin NPC placement, HUD and stable composition. **An empty or flickering scene is a failing Alpha**, even if internal scene data exists or a trace says frame submitted.
4. Camera movement, turning, collision, input responsiveness, basic encounter/UI transitions, sound and smooth frame pacing work at Windows-native timings. Original 70 Hz/VGA re-creation is optional; choose modern stable pacing.
5. Publish a safe one-click Windows artifact usable with the player's owned original assets. Original installation and saves must not be overwritten. The current continuous launcher copies source media to a temporary test installation; packaging can improve later.
6. Title intro replay, create-party and exit are usable. A not-yet-supported Load Game option may clearly say so for Alpha; unsafe or false restore is prohibited. Full save/load is a Beta release requirement.

**Alpha acceptance:** every `ALPHA-*` gate passes real Windows verification, source-matched CI and owner field acceptance. DOSBox comparison is optional. **Current known blocker:** owner previously reported missing exploration visuals, flicker and audio, and the fresh exact-source continuous build has not yet passed a player test.

**Not Alpha blockers:** exact original `CHARS` bytes, x86 supervisor emulation, original `SAVE##` destructive copy quirks, x86 instruction-address proofs, DOS timer/keyboard IRQ parity, unneeded VGA micro-operations, the complete campaign, or exact DOS-video capture equality.

## Beta — entire authored campaign is playable

1. Starting from normal title/party setup, reach the ending through all original levels, authored story paths, NPCs, triggers, puzzles, monsters, spells, combat, inventory and real level transitions.
2. Implement **port-native save/load** that reliably round-trips character/party/world/script/level state across exit/relaunch, even after changes to encounters and quest state. A versioned, safe format is preferred; original DOS-save binary compatibility is optional.
3. Finish essential original-content graphics, environmental animation, sprites and music/voice/SFX across the campaign. Fix gameplay-blocking rendering, pathfinding, collision and event bugs before polish.
4. Make in-game options and failure modes usable on contemporary PCs. No title-only proof, simulated campaign, developer skip or synthetic end screen qualifies.
5. Publish a coherent full-game Windows build and reproducible playtest evidence; track and fix all known save corruption or story-blocking defects.

**Beta acceptance:** every `BETA-*` gate passes field evidence and required build/package proofs. DOS binary parity is not required.

## Final — polished enhanced modern PC release

1. Complete verified campaign and reliable saves; no major crashes, state loss or broken authored content.
2. Modern rendering: resizable/windowed/fullscreen, aspect-correct scaling, smooth vsync/frame pacing, legible HUD, stable compositing and configurable rendering where useful.
3. Reliable speech/music/SFX and practical master/music/SFX volume settings; contemporary device handling.
4. Responsive modern keyboard and mouse, sensible remapping/options, accessibility and optional comfort features as feasible, without unintended rewrite of original story or encounter design.
5. Safe user-data ownership and versioned portable save format; clean installation/asset importing, documentation, verified release archive and known-issue list.
6. Document intentional gameplay/visual improvements and modern safety fixes. Preserve original content, not harmful memory/file quirks.

**Final acceptance:** all `FINAL-*` gates and an owner-approved native Windows end-to-end experience. **No full DOSBox gameplay/VGA/audio/IRQ parity gate.**

## Engineering focus in order

**Now:** test the latest continuous Alpha launcher and repair the first player-visible defects (cinematics/audio, first Level 22 scene, frame pacing, controls).

**Next:** complete Level 22 quest/encounter and persistent native world/party saves, using reconstructed game data and rules.

**Then:** all authored levels/quests and campaign ending with integration tests, followed by display/audio/input enhancements and release polish.

### Relationship to historical research

The [2026-10-09 continuous integration contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md), [original DOS charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md), `Code1` / `Code2` decompilation reports, DAT audits and original-file provenance are **preserved historical/technical resources**. Any old instructions requiring exact original interrupt behavior or withholding modern renderer/input improvements are superseded by the enhanced-port charter. Do not delete research; do not make its uncompleted parity tasks a critical path for the enhanced PC port.
