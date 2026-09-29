# RLOFT source-port roadmap

## Target

Reconstruct the complete *Ravenloft: Strahd's Possession* executable behavior in maintainable C/C++, load the user's original game files externally, reproduce the original game faithfully, and establish a reusable framework for the sequel and later related decompilation/source-port projects.

## Phase 0 — Preserve verified baselines

Preserve final reverse-engineering evidence, known-good C/C++ checkpoints, and deterministic fixtures. Never overwrite a verified milestone; advance through Git commits/tags/releases.

## Phase 1 — Reusable source-port architecture

Separate reusable engine/platform services, original-compatibility behavior, Strahd-specific systems, presentation backends, mod/extensions, and verification infrastructure.

## Phase 2 — Complete CODE.1 / CODE.2 reconstruction

Maintain a permanent original-address → C/C++ mapping. Every reachable routine must be classified as reconstructed game logic, compiler/runtime/library support, embedded data, or proven unreachable/dead code.

**Gate:** executable reconstruction ledger reaches 100% classified and all required game logic is implemented in C/C++.

## Phase 3 — Native game-file runtime

Detect and validate a user-owned installation, load all required original formats directly, and eliminate runtime dependence on execution of the DOS executable.

## Phase 4 — Complete gameplay parity

Integrate character creation/progression, movement, collision, NPC AI/scheduling, combat, inventory/equipment, items, magic, conditions, world objects, triggers, dialogue/events, level transitions, automap, resting/time, save/load, and ending progression.

## Phase 5 — Complete presentation parity

Implement original animation state machines/triggers, HUD/UI, portraits, cursors, interaction feedback, text, palettes/effects, mist, transitions, music, ambient audio, and sound effects.

## Phase 6 — Full-game verification

Validate original and reconstructed behavior with deterministic traces, original-data integration tests, state comparisons, save/load checks, and complete playthrough coverage.

**Gate:** the game can be completed from start to ending using the C/C++ executable and original data without executing the original EXE.

## Phase 7 — Modern viewport

Run the same authoritative simulation through the modern high-resolution viewport. Presentation improvements must not fork or duplicate game simulation.

## Phase 8 — Modding / enhancement layer

Expose stable extension points for renderer replacement, UI scaling, asset replacement, QoL options, bug-fix toggles, balance changes, scripting hooks, editor tooling, and gameplay mods while keeping faithful compatibility separately testable.

## Phase 9 — Sequel reuse

Use the framework for the Ravenloft sequel. Reuse identical systems, isolate changed behavior, and promote genuinely shared systems into the common runtime.
