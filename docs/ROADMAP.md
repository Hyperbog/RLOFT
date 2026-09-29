# RLOFT source-port roadmap

## Target

Reconstruct the complete *Ravenloft: Strahd's Possession* executable behavior in maintainable C/C++, load the user's original game files externally, reproduce the original game faithfully, and establish a reusable framework for the sequel and later related decompilation/source-port projects.

## Phase 0 — Preserve verified baselines

- Preserve the final reverse-engineering closure archive separately from the distributable source tree.
- Preserve known-good C/C++ checkpoints and deterministic fixtures.
- Never overwrite a verified milestone; advance through Git commits/tags/releases.

**Gate:** existing evidence and known-good reconstruction checkpoints are reproducible.

## Phase 1 — Reusable source-port architecture

Separate:

- reusable engine/platform services,
- original-compatibility behavior,
- Strahd-specific game systems,
- render/audio/input backends,
- mod/extensions layer,
- verification infrastructure.

**Gate:** game-specific rules can be distinguished from reusable SSI/runtime infrastructure without changing verified behavior.

## Phase 2 — Complete CODE.1 / CODE.2 reconstruction

For every reachable original routine, maintain a permanent record of:

- original object/address/range,
- callers/callees,
- known state and data dependencies,
- C/C++ counterpart,
- evidence,
- implementation status,
- verification status.

Every routine must end in one of these classifications:

- reconstructed game logic,
- compiler/runtime/library support,
- embedded data,
- proven unreachable/dead code.

**Gate:** executable reconstruction ledger reaches 100% classified and all required game logic is implemented in C/C++.

## Phase 3 — Native game-file runtime

- Detect/validate a user-owned installation.
- Load all required original formats directly.
- Remove dependency on execution of the DOS executable.
- Preserve original save/data layouts where compatibility requires it.

**Gate:** new executable can initialize the game from original files alone.

## Phase 4 — Complete gameplay parity

Implement and integrate all original gameplay systems, including character creation/progression, movement, collision, NPC AI/scheduling, combat, inventory, equipment, items, magic, conditions, world objects, triggers, dialogue/events, level transitions, automap, resting/time, save/load, and ending progression.

**Gate:** all original gameplay interactions work end-to-end.

## Phase 5 — Complete presentation parity

Implement original animation state machines/triggers, HUD/UI, portraits, cursors, interaction feedback, text, palettes/effects, mist, transitions, music, ambient audio, and sound effects.

**Gate:** faithful frontend reproduces the original game's player-visible behavior.

## Phase 6 — Full-game verification

Validate original and reconstructed behavior using deterministic traces, original-data integration tests, state comparisons, save/load checks, and complete human playthrough coverage.

Known original bugs/quirks remain part of faithful mode.

**Gate:** game can be completed from start to ending using the C/C++ executable and original data without executing the original EXE.

## Phase 7 — Modern viewport

Run the same authoritative simulation through the modern high-resolution viewport. Presentation improvements must not fork or duplicate game simulation.

**Gate:** modern viewport is fully playable with no gameplay-state divergence from faithful mode.

## Phase 8 — Modding / enhancement layer

Expose stable extension points for renderer replacement, UI scaling, texture/asset replacement, QoL options, bug-fix toggles, balance changes, scripting hooks, editor tooling, and gameplay mods.

Faithful compatibility code remains separately testable.

**Gate:** modifications can be enabled/disabled without rewriting the compatibility core.

## Phase 9 — Sequel reuse

Use the framework for the Ravenloft sequel. Reuse identical systems, isolate changed behavior, and promote genuinely shared systems into the common runtime.

**Gate:** the second project starts from the common framework rather than a new one-off codebase.
