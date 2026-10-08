# Part 27 — Verified Windows Build of Improved Interface and Analysis Exports

Date: 2026-10-08

**Status: Windows build + automated regressions PASS. Interactive visual acceptance PENDING.**

**Overall first-release estimate: 39%** (37% at Part 26). The +2-point estimate reflects confirmed compilation and synthetic regression coverage of the Part 25 analysis export plus updated Part 26 WPF theme; no credit is claimed for visual approval or original-game parity.

## Verified build provenance

- Source snapshot built: `03b1f17acc87e8ec7d5823e87556326d88685a0b` — temporary build trigger on top of the Part 26 UI theme changes, without changing application code.
- Successful Windows run: https://github.com/Hyperbog/RLOFT/actions/runs/37855935875
- Build result: **success**, Windows hosted runner and .NET 10; all four sequential project builds succeeded with **zero warnings and zero errors**.
- Synthetic core test: **ALL SMOKE TESTS PASSED**, including `candidate-only JSON/text export, machine-byte opt-in, atomic report publication`.
- Synthetic SQLite test: **PASS** for identity, backup, schema guard, restore and annotation revision handling.
- Portable Windows x64 self-contained desktop artifact: `DOSDis-Desktop-win-x64-preview` (artifact id `11584570631`).
- Portable Windows x64 self-contained CLI artifact: `DOSDis-CLI-win-x64-preview` (artifact id `11583991847`).
- Both downloaded ZIP archives passed `unzip -tq` in the preparation environment. Executables `DosDis.Desktop.exe` and `DosDis.Cli.exe` were present.
- The workflow was immediately returned to **`workflow_dispatch` only**, and the temporary one-time build marker removed. Branch history contains build-trigger and cleanup commits, but the application tree corresponds to the tested source.
- This run involved **no original proprietary RLOFT files**.

## User-verified Part 24 GUI baseline vs this new build

The user's prior screenshot confirmed that the Part 24 desktop launches on Windows, opens CODE.1 and renders candidate functions/disassembly and File Intelligence. That screenshot exposed bright-white disabled controls and default-themed tabs/header surfaces.

The newly built Part 26 desktop includes explicit dark WPF templates, tab/header/selection styling and dynamically sized File Intelligence columns. **The new build has not yet been visually examined by the user**; compilation and packaging do not certify aesthetic correctness or runtime behavior.

## Required next tests on Windows 11

1. Extract the new desktop ZIP into a clean folder and launch `DosDis.Desktop.exe`. Do not run it directly from inside ZIP.
2. Open the same original CODE.1; compare the toolbar, disabled button appearance, tab/header controls, selected row and metadata columns against the Part 24 screenshot.
3. Navigate Hexadecimal and References tabs, function jump, address box and original-byte display. Test resizing and keyboard focus.
4. Save and reopen a `.ddproj` project and comment; create a backup; test error behavior and cancellation.
5. Use **Export…** to save JSON and text (byte excerpts disabled by default); confirm candidate classifications, source SHA-256, and no original executable or private paths embedded.
6. Optionally run original CODE.1/CODE.2 test suite on a trusted local Windows machine with `build-windows.ps1` and the two original paths; these files are not sent to CI.
7. Collect screenshots or reproducible error details to guide UI and logic fixes.

The crash-reporting, session inspector and analyzer-decision debugger remain planned, not implemented. Windows only. AI assistance remains postponed until after final release.
