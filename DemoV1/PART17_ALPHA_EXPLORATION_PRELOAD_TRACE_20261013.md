# Part 17 — Normal-game first-level preload diagnostic boundary

**Branch:** `demo-v1`  
**Date:** October 13, 2026  
**Classification:** Pre-Alpha; source change only, **no exact-HEAD Windows/MSVC build or original DOS field acceptance recorded here**.

## Confirmed gap after Part 16

Before this change, the source-pinned optional boot trace finished at each of the three calls to `startup_handoff()`, **before** `run_exploration()` began. The separate exploration trace was created only after `ExplorationRuntime::load_fresh`, `initialize_gameplay_state`, and the exploration presenters were constructed. A Level 22 load failure therefore left no current exploration session even though the previous boot trace showed its normal handoff.

This is a missing *diagnostic* boundary. It is not evidence that any of those constructors fails in an owner Windows field run, nor that normal gameplay passes once they return.

## Changes

- The opt-in exploration `DevSessionTrace` and `latest-session.txt` now initialize **before** `ExplorationRuntime::load_fresh`; the original boot and exploration session files remain separate.
- The new symbolic, privacy-safe stage IDs are `exploration.load.begin`, `exploration.load.ready`, `exploration.load.failed`, `exploration.world.ready`, `exploration.world.failed`, and `exploration.presenters.ready`. The `failed` stages rethrow the original exception without copying exception strings, user names, game data or absolute paths into the event.
- A pre-load host-pointer RAII guard clears the optional session pointer if an exception occurs before presenters exist. Once created, the existing presenter/framebuffer teardown retains its own draw-audit detach.
- A trace-event write failure clears optional session instrumentation instead of blocking original program flow. Without `--dev-trace`, no trace file is created and all original world loading, initialization order, presentation, and mutable character/game state paths remain in place.
- The existing `alpha_boot_stage_trace_contract_tests` now asserts pre-load trace placement, all six stage IDs, opt-in instrumentation, source order, pointer cleanup, and three additional deliberate negative mutations (seven total). This is **source-only** coverage.

## What a first-field report now means

If the generated exploration ZIP ends with `exploration.load.failed`, investigate the original-media Level 22 loading boundary. If it reaches `exploration.load.ready` but then `exploration.world.failed`, investigate original mutable-world initialization. If it reaches `exploration.world.ready` but not `exploration.presenters.ready`, investigate the presenters/resource constructors. Reaching `exploration.presenters.ready` still **does not prove the first gameplay frame rendered**: require an exploration frame capture and owner observation. `session.finish` is not a gameplay pass.

The isolated PowerShell field runner now displays only the last recognized **whitelisted** exploration initialization stage from the new diagnostic ZIP, not unfiltered event records; this leaves typed names and scan events out of console summaries. A source-only seventh sandbox negative mutation rejects removing that stage whitelist.

Original installation/game-source or window creation failures can still occur before either trace starts. For all runs use the protected-copy `Test-Alpha-Continuous-Game.cmd`; do not run the native prototype against the owner's original CHARS/SAVE.DAT.

## Validation still required

1. Windows compile and CTest execution against the **exact** `demo-v1` source SHA after this commit.
2. Protected-copy normal-game cold start and existing-party sessions; retain both diagnostic ZIPs and the first visible blocker.
3. Independent original DOS comparator for the same startup/frame/palette/input sequence.
4. No Alpha gate changes; all 10 Alpha release gates remain open.
