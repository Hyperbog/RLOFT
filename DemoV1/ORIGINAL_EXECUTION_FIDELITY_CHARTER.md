# Ravenloft: Strahd's Possession — Original Game Code Reconstruction Charter

**Non-negotiable project objective:** **Finish recreating the code necessary to run the COMPLETE ORIGINAL game first.** Reconstruct all required code, systems and original runtime behavior into one integrated, executable game. Only **after that entire recreation is finished** may work turn to modern-port design, bug fixes, enhancements, modding or QoL features.

**No separate preliminary "Phase I" deliverable is intended.** The full original-game code recreation **is the current, complete project objective**. Level 22 is a reusable development testbed within that work, not a separate product or completion milestone that permits modern development.

**Reference authority:** original shipped *Ravenloft: Strahd's Possession* code and game files, with version provenance. Original DOS execution is the behavioral oracle used to validate the **recreated code**. The reconstructed executable need not execute the DOS binary's literal machine instructions to fulfill this goal; it must reproduce the original game's complete behavior accurately.

## Required end state — a faithful complete-game code recreation

Recreate and integrate the actual functionality of the original game from startup to ending:

1. **Recreate the original code behavior, not a modern redesign.** Preserve the original mechanics, game rules, numerical calculations, ordering, persistence, bugs, quirks, limitations, edge cases, timing, random-number behavior and input semantics. Implement equivalents when the host platform demands different APIs, but keep the game-visible original behavior intact.
2. **Recreate every system and subsystem.** CODE.1 character creation and transfer; CODE.2 startup and game loop; levels, movements, collisions, combat, magic, inventory, NPCs, events, controls, UI, automap, sound, music, cinematics, story and endings; original SAVE/LOAD and all associated state. Do not omit a subsystem because the current test level does not exercise it.
3. **Use the original data, not fabricated replacements.** Levels, creatures, objects, events, messages, graphics, animation and music must be loaded/decoded and consumed according to the original EXE, BAT, DLL, CODE.1/CODE.2, DAT, RES, ANM and other game files. Retain original game assets unmodified; distinguish release/patch variants.
4. **A single complete original game.** Original launch → original character creation/load → all source-defined levels and event transitions → original ending; all changes to party/inventory, quest flags, world, timers, RNG and saves remain continuous. No per-level mock games or developer-only teleport paths may substitute for authored progression.
5. **Faithful presentation and execution.** Reconstruct the real original rasterization, palette, text, animation, audio, input/IRQ-dependent cadence and game-state updates. Use DOS original-runtime captures/traces and source evidence to detect and fix divergences. An approximation or unverified replacement is provisional.
6. **Implementation evidence is not acceptance.** Independently track disassembly/source mapping, reconstructed code, integration, authored regressions, actual compiled/build-tested results, and original-game comparison. Reproducing every original instruction byte is **not** the requirement: reconstructing all code needed to **run the original game faithfully** is. Do not confuse CTest success or source-only completion with an accepted, end-to-end original-game recreation.
7. **Finish the whole recreation before modern changes.** No intentional bug fixes, graphics refreshes, mechanics changes, new gameplay features, accessibility/QoL redesign, modding-facing game alterations or independent modern port in the original-game recreation effort. Diagnostics and extraction/decompilation tools supporting faithful reconstruction remain appropriate.
8. **Record all gaps honestly.** Explicitly identify the original function/offset/data source where known, remaining unknowns, incomplete wiring, missing transitions, render/timing mismatches and unavailable original comparisons. A subsystem cannot be declared complete merely because its basic behavior has been imitated.
9. **Commit every checkpoint.** All reconstruction, tests, evidence and documentation must be committed/pushed to Git. Builds and CI are manual, only with explicit user authorization; record precisely which source commit a verified build used.

## Required Level22-first sequence

**Before beginning independent Level23 gameplay work, first finish a complete and source-faithful playable Level22.** This includes all original gameplay systems needed there and the authentic end-of-level mist sequence. The original sequence calls the Level23 loader after frame 20; implementing/accepting that exact handoff as part of the Level22 ending does **not** authorize premature work on Level23 encounters, environments or progression. See [Level22 playability acceptance](LEVEL22_COMPLETION_AND_PICKUP_FEEDBACK_GATE_20261009.md).

## Current development method

Continue the existing `demo-v1` work as the **actual code-recreation effort** toward the complete original game. The Level 22 developer slice tests original systems together and must feed directly into normal startup/character creation, following cinematics and each original authored level. Keep the reconstructed shared engine and original assets/source references; avoid rewriting for modernization now.

The general in-memory `transition_to_level` helper is a provisional implementation step: it still needs source-proven authored exits, exact effects and full original save/state behavior. A green 172-test prior Windows build and 177 currently registered tests do not certify the incomplete whole-game recreation.

## After the full original-game code recreation is finished

**Then, and only then**, reconsider modern-port goals. At that later point, maintain the fully recreated original game as a separately preserved fidelity baseline and develop any modern version on an **independent branch**, including rewrites, intentional bug fixes, modern features, QoL, visuals and modding tools.

Do not begin those considerations or implementation during the original-game recreation. There is **no intermediate "Phase I complete" authorization** based on archival work, a vertical slice, or a percentage readiness score.

## Reporting

Describe the objective as **"complete faithful code recreation to run the original game"**. State implemented, wired, test-registered, built/tested, original-DOS-compared and remaining work separately. Continue percentage completion for every system and subsystem without presenting engineering readiness as evidence that the whole original game now runs correctly.

This charter supersedes the previous two-phase/literal-original-instruction-execution interpretation.
