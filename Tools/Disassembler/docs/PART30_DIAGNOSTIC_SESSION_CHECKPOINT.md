# Part 30 — Bounded Diagnostic Sessions and Private Crash Records

**Date:** 2026-10-08.
**Status:** source implementation; **NOT YET COMPILED on Windows or run in the GUI**. No new GitHub Actions run requested.

## Owner confirmation / progress

The user confirmed the Part 29 build works for the tab-state and raw-file inspection checks. This is interactive acceptance for those two reported issues only. Overall first-final-release **planning estimate: 41%** (previously 40%). Part 30's new unverified code does not add a percentage until Windows build and tests pass.

## Newly implemented functionality (source only)

- `DosDis.Core.DiagnosticSession`: per-process UUID, ISO UTC timestamps, strictly enumerated diagnostic stages, current artifact SHA-256, optional LE object/offset and instruction count, numeric monotonic event sequence.
- **Bounded ring buffer:** 512 events by default; capacity configurable from 8 to 4096. Explicit dropped-event count prevents implying the log contains the entire session.
- Desktop records startup, file-open attempt/result, LE/raw classification, analysis start/completion/cancellation/failure, navigation, project open/save/backup, analyst-comment save (not content), analysis report export, handled error type, diagnostic export and window close.
- WPF **Diagnostics…** button: shows explicit privacy information and requires user confirmation plus a Save dialog before exporting a schema-v1 JSON report. Output is staged to a unique same-folder temp file and atomically moved; an existing report is never overwritten.
- WPF `App` hooks Dispatcher unhandled exceptions, AppDomain process-level unhandled exceptions and unobserved task exceptions. It attempts a bounded **local-only** incident file in `%LOCALAPPDATA%\DOSDis\Incidents`. An incident write failing does not prevent original exception handling. The dispatcher exception is **not marked handled**; the app cannot guarantee recovery after a fatal/native crash.
- The core smoke suite has new synthetic source tests for buffer eviction and monotonic sequence, schema/privacy values, refusal of arbitrary hash-like text, exception type capture but absence of exception message, atomic report creation, rejected overwrite, and cleanup.

## Privacy and limits

By design, no file paths, project notes, usernames, original executable bytes, memory images, arbitrary messages or stack traces enter this event structure. **SHA-256 identities and addresses are included** and may reveal the known source title to someone with a fingerprint database. Export is local-only and the app does not upload anything.

This is **coarse operational diagnostics**. It is not yet a fine-grained decoder/fixup/CFG decision log; it cannot replay the analyzer, reconstruct the state of an arbitrary prior crash, or resume a corrupted research session. Native crashes, forced termination or out-of-memory conditions may prevent incident capture. Default incident files on disk should be reviewed before sharing.

The separate JSON/text **analysis report** from Part 25 contains rich final evidence, but its report format is not a step-by-step execution log.

## Windows acceptance gates

1. Compile desktop/CLI/core/persistence and run both synthetic smoke suites on a manually initiated Windows build; verify no warnings or errors.
2. Launch the desktop, open original CODE.1 and CODE.2 and a raw DAT file; use analysis and project save/export controls; then export diagnostics. Verify stage order, source SHA-256, addresses and counts, and dropped-event indicators.
3. Confirm the report contains **no original bytes, original path, private note bodies or exception messages**. The test suite only uses synthetic data; check the real exported JSON before sharing.
4. Trigger a controlled nonfatal error (invalid Go-to address), then export diagnostics; verify error type appears but the invalid input string does not.
5. On an isolated test Windows installation, exercise unhandled .NET exceptions to validate best-effort local incident creation. Do not trigger fatal crash tests on valuable unsaved research sessions.
6. Verify existing project persistence, Explorer tab retention, raw hex, exports and original hashes remain unchanged after this feature.

## Future work

- Detailed structured analyzer-pass decisions with source-evidence IDs and strict bounded trace mode.
- Session timeline inspector, filtering, portable incident ZIP import, diff/replay across matching file identities.
- Interactive pass/decision stepping and breakpoints (the *analyzer* debugger, not DOS-emulated game debugging).
- Local report retention/rotation and privacy review before sharing.

GitHub Actions remains `workflow_dispatch` only, Windows-only; the original CODE.1/CODE.2 binary files were not uploaded or modified. No AI assistant integration before final release.
