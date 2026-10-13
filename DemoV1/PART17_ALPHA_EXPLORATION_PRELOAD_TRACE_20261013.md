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
- The existing `alpha_boot_stage_trace_contract_tests` now asserts pre-load trace placement, all six stage IDs, opt-in instrumentation, source order, pointer cleanup, and six additional deliberate negative mutations (ten total). This is **source-only** coverage.

## First actual frame construction and submission

The normal exploration loop now records **one-time** optional diagnostic
checkpoints on the first real build/render/presentation path, not on a
synthetic Level22 warp or on every ~70 Hz redraw:

- `exploration.frame.build.begin` → `build.ready` or `build.failed`
  around the existing `runtime.build_frame()`.
- `exploration.frame.raster.ready` or `raster.failed`
  around the original-asset native `presenter.present(...)`.
- `exploration.frame.present.submitted` **after**
  `host.present(...)` returns for a normally constructed gameplay view.

The existing exception handlers, source frame capture, indexed draw
audit and normal gameplay rendering are left in place. These stage
IDs do not contain actor names, Win32 raw scan records, runtime save
bytes or original media paths. The isolated PowerShell runner prints
only the last whitelisted stage, never raw diagnostic JSONL contents.

The source-only boot-trace contract now rejects **10** deliberate
negative cases; the sandbox field safety contract rejects **17**.
Those tests and the CMake declarations are not substitutes for verified
exact-checkout Windows execution. A reported `present.submitted`
means only that the native host returned from its presentation call;
it does not certify a visibly correct frame or original-DOS raster.

## What a first-field report now means

If the generated exploration ZIP ends with `exploration.load.failed`, investigate the original-media Level 22 loading boundary. If it reaches `exploration.load.ready` but then `exploration.world.failed`, investigate original mutable-world initialization. If it reaches `exploration.world.ready` but not `exploration.presenters.ready`, investigate the presenters/resource constructors. Reaching `exploration.presenters.ready` still **does not prove the first gameplay frame rendered**: require an exploration frame capture and owner observation. `session.finish` is not a gameplay pass.

The isolated PowerShell field runner now displays only the last recognized **whitelisted** exploration initialization stage from the new diagnostic ZIP, not unfiltered event records; this leaves typed names and scan events out of console summaries. A source-only sandbox negative mutation rejects removing that stage whitelist.

Original installation/game-source or window creation failures can still occur before either trace starts. For all runs use the protected-copy `Test-Alpha-Continuous-Game.cmd`; do not run the native prototype against the owner's original CHARS/SAVE.DAT.

## Alpha field package byte identity and existing-party eligibility

The next protected-copy field-runner hardening avoids two ways to generate
misleading evidence without changing the engine's original-game paths.

- Windows CI stages `ravenloft_demo_v1.exe.sha256` from the **actual built
  Release EXE** and ships it with **both** field and original-intro
  artifacts. `build-source-sha.txt` still names the compiled checkout; it
  is a distinct source-identity claim, **not** a cryptographic guarantee
  that any arbitrary EXE corresponds to that commit.
- Before opening the original-installation folder picker, the Alpha
  PowerShell field runner requires the checksum manifest, validates its
  exact 64-hex + double-space + filename format and refuses to launch
  if the packaged executable's SHA-256 differs. This guards accidental
  stale/manual mixing of artifact files.
- For the **existing-party** route, the runner verifies only the **copied**
  CHARS where `GameSource::runtime_file` actually searches: the copied
  install root or copied `RLOFT` child, in that priority. A CHARS file
  elsewhere does not make the route eligible. The selected file must have
  the original `0x920` (2336-byte) transfer length. No CHARS content is
  printed, transmitted, deleted or modified by this check.
- The cold route still deletes CHARS **only in the scratch copy**. The
  original installation remains read-only to the runner, and the chosen
  route is not silently relabeled after launch.
- The standalone source-only `alpha_sandbox_normal_launch_contract_tests`
  now rejects **17** negative variants including checksum tampering and
  absent/malformed existing-party CHARS preflight.

These checks do **not** prove a valid party can load, nor that the native
first frame matches DOS. The newly staged checksum file is required by
the new Alpha launcher; older manually assembled launch packages without
this file are intentionally rejected.

## Session provenance and owner-folder safety

Because `latest-boot-session.txt` and `latest-session.txt` are shared
TEMP pointers, a separate native process can replace them during a
field run. The launcher now accepts a session only when its directory
name ends in the **actual launched process ID** (`-boot-<PID>` for
startup; `-<PID>` for exploration) and the read-only diagnostic
`manifest.json` names the packaged `build-source-sha.txt` revision.
Checks occur before any ZIP or console stage summary. An unrelated
run cannot be packaged as a successful result merely by updating the
shared latest pointer; instead the runner fails with a clear error.

The selected owner source directory is now also rejected if its
**root itself** is a Windows reparse point (junction/symlink); the
earlier recursive check already rejected such descendants. This
protects the original-versus-TEMP overlap check from an alias at the
selected root. The launcher still does not modify the owner source.

The source-only sandbox contract now rejects **17** distinct unsafe
mutations, including five new root/session provenance violations.
**No new field observation is claimed.** Normal `--dev-trace`
instrumentation stays opt-in; none of these guards alter the DOS
world, original party bytes, or gameplay input.

## Validation still required

1. Windows compile and CTest execution against the **exact** `demo-v1` source SHA after this commit.
2. Protected-copy normal-game cold start and existing-party sessions; retain both diagnostic ZIPs and the first visible blocker.
3. Independent original DOS comparator for the same startup/frame/palette/input sequence.
4. No Alpha gate changes; all 10 Alpha release gates remain open.
