# Part 31 — Windows Diagnostic Build and Regression Verification

Date: 2026-10-08

**Status: SUCCESS — .NET 10 Windows build, both synthetic regression suites, and preview artifact publication. Interactive WPF and controlled crash tests still pending.**

## Build and source identity

- GitHub-hosted Windows build: https://github.com/Hyperbog/RLOFT/actions/runs/37861762919
- Source tested: commit `383c636b47540f61e6d68e604154621c05f4fab8`. This is exactly the Part 30 application source plus a narrowly scoped one-time workflow trigger.
- All four .NET project builds (core, CLI, persistence and WPF desktop) succeeded with **zero warnings and zero errors**. Windows GUI compilation does not mean the GUI has been launched in CI.
- Core smoke output includes `PASS: bounded diagnostic events, privacy defaults, atomic export and non-overwriting reports` and `ALL SMOKE TESTS PASSED`.
- Additional smoke cases passed for raw file/EOF/offsets, candidate-only analysis report export, relocation-backed indirect references, CFG and synthetic LE structures.
- SQLite project persistence smoke passed for source identity, backups, metadata, snapshot round trip, annotations and integrity guards.
- Desktop artifact `DOSDis-Desktop-win-x64-preview`, GitHub artifact ID `11586830229`, downloaded as `DOSDis-Part31-Windows11-Desktop.zip` (ZIP verified).
- CLI artifact `DOSDis-CLI-win-x64-preview`, GitHub artifact ID `11585884667`, downloaded as `DOSDis-Part31-Windows-CLI.zip` (ZIP verified).
- After the successful job, the temporary scoped trigger marker was deleted and the workflow returned to manual-only `workflow_dispatch`. No Linux builds or automatic push jobs remain.

## Validated behaviors and boundaries

The diagnostic module has a strict event-type schema, monotonically increasing event sequence, bounded 512-event in-memory capacity (8–4096 configurable), a dropped-event count, source SHA-256 when available, and user opt-in non-overwriting JSON export.

The synthetic tests prove the writer rejects duplicate export names, preserves prior report bytes, cleans its staging file, and omits exception messages and source path strings from the exported JSON. This is not an exhaustive privacy penetration test.

The WPF desktop now includes `Diagnostics…`. Unhandled WPF/process/task exceptions have best-effort incident logging in `%LOCALAPPDATA%\\DOSDis\\Incidents` without uploading or suppressing fatal errors. **These hooks have not been triggered in a real interactive Windows run.** Some severe/native exceptions may prevent a report; avoid using this feature as a substitute for saving research projects.

## Owner validation

1. Extract the current Part 31 desktop ZIP and launch `DosDis.Desktop.exe` on Windows 11.
2. Load CODE.1 or CODE.2 and then a raw data file; run analysis where supported.
3. Click `Diagnostics…`, review the consent prompt, export JSON, and verify stage chronology plus source SHA-256 (if previously loaded).
4. Deliberately type an invalid Go-to address, close its expected error dialog, and export again. Expect `HandledError` with only an exception type, not the entered text or original file path.
5. Confirm normal Explorer tab navigation, original raw hex, project save/reopen and separate candidate analysis exports still work.
6. Submit a sanitized report and screenshots if misclassification or crashes are observed. Never attach original copyrighted executables or private project comments accidentally.

## Decision-level tracing roadmap

The diagnostic recorder is currently **operation-level** and does NOT explain individual decoder, relocation, CFG or table recovery decisions. The next milestone should add typed evidence-linked records inside bounded analyzer stages, with location/rule ID, evidence source and classification, then an inspection view. Do not capture unrestricted messages or per-instruction original bytes by default; apply optional bounded detailed trace sampling for performance. Interactive step/replay remains separate.

## Progress and constraints

**Estimated first-final-release completion: 43% overall (up from 41%).** Increase recognizes verified Windows compilation, synthetic diagnostic privacy/regression coverage and packaged executables, not full debugging, DOS runtime parity or UI acceptance.

Windows-only. GitHub Actions manual-only. No AI integration until after final release. Original RLOFT files were not modified or uploaded to CI.
