# Demo v1 — Full Windows Build #649 (2026-10-09)

**Verification status:** SUCCESS. [GitHub Actions run #38003217534](https://github.com/Hyperbog/RLOFT/actions/runs/38003217534).

| Item | Verified value |
| --- | --- |
| Workflow | `.github/workflows/demo-v1-build.yml` |
| GitHub Actions run number | **649** |
| Run identifier | `38003217534` |
| Event | Explicitly user-authorized one-time marker-only push |
| Build checkout/source SHA | `6241a807baf53449256e5517687430283fdf2ca3` |
| Windows MSVC compilation | **SUCCESS** |
| Original CODE.2 C++17 syntax check | **SUCCESS** |
| Full CTest suite | **179/179 passed** (100%) |
| Specific new test | `dev_session_trace_tests` — PASSED |
| Linux | Not requested; skipped |
| Build verification mode | Full |
| Windows x64 artifact | `Ravenloft-Demo-v1-Windows-x64`, artifact ID `11650206738` |
| Published artifact ZIP size | 811,857 bytes compressed |
| Native EXE SHA-256 | `087a0573962275e8cac3e826f3767b266bea4fba7b184da97d09c8b7c8b991f4` |
| Release EXE in Git | `DemoV1/Windows/ravenloft_demo_v1.exe` |
| Release checksum in Git | `DemoV1/Windows/ravenloft_demo_v1.exe.sha256` |
| EXE-publishing Git commit | `6e8513c29f1eaf8621e27868430e8e563648ca6c` |
| Verified workflow completion | 2026-10-09 23:17:09 UTC |

## New Level22 diagnostics included in this release

Build #649 includes the opt-in native **`--dev-trace`** session recorder, diagnostic console launcher **`Diagnose-Level22.cmd`**, event and mouse/keyboard state traces, modal-screen 320×400 indexed frame+palette captures, render-source TSV, Type1 draw/fill/restore ordering and bounds, and F12 defect markers with a 256-event recent history. The launcher automatically attempts ZIP packaging after the game exits. [Recorder protocol and byte formats](LEVEL22_DEV_TRACE_CHECKPOINT_20261009.md).

**User validation needed:** Download the Windows-x64 artifact from [run #649](https://github.com/Hyperbog/RLOFT/actions/runs/38003217534). Extract all files together. Double-click **`Diagnose-Level22.cmd`**, explore inventory/automap/viewport/menu/quit and press F12 at visible defects, then exit. The launcher prints the ZIP path under `%TEMP%\\RLOFT-Demo-Diagnostics`; upload that session ZIP here. This is the first real on-machine validation of log production and packaging. Do not conclude it works merely from unit tests; issues found by the user should be fixed at the next committed checkpoint.

## Fidelity interpretation and build policy

This Full build proves the authored diagnostic code compiles and all 179 native regressions pass on the current source. It does **not** verify full game playability, exact DOS pixels/IRQ timing, user-machine ZIP packaging, or the complete original mist ending. The project remains **49.8% provisional engineering readiness (20 systems, 122 sub-systems)** and **0% original DOS gameplay parity acceptance**.

The single marker file `DemoV1/.full-windows-build-once-20261009-trace` was deleted, and `.github/workflows/demo-v1-build.yml` was restored byte-for-byte to its **manual-only** version after the run was queued. The checkout SHA includes only temporary workflow-trigger metadata beyond the source tree that produced the program; these CI-only changes have since been reverted. Ordinary source commits do not dispatch builds; builds require explicit user authorization.

**Scope:** fully playable source-faithful Level22 through its original mist sequence first; independent Level23 gameplay remains deferred.
