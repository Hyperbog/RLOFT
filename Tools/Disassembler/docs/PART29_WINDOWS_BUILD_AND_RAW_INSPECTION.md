# Part 29 — Windows Build and Synthetic Verification of Raw Hex and Tab Navigation

**Date:** 2026-10-08

**Result: Windows .NET 10 project builds PASSED. Both synthetic regression suites PASSED. Self-contained Windows x64 Desktop and CLI artifacts PUBLISHED. GUI interaction in this new build is not yet user-tested.**

## Build provenance

- Successful Windows GitHub run: https://github.com/Hyperbog/RLOFT/actions/runs/37858020058
- Built source commit: `1bbc7a69a20780314daa377e0fa7309a52955d7d`. Application files identical to the Part 28 implementation; temporary build-only trigger file/workflow were subsequently removed.
- All four builds succeeded: Core, CLI, Persistence, Desktop (the latter WPF).
- Core synthetic output includes **PASS: raw bytes, EOF, physical-offset hex navigation and unsupported-format capability label**, **PASS: candidate-only JSON/text export, machine-byte opt-in, atomic report publication**, and **ALL SMOKE TESTS PASSED**.
- SQLite persistence synthetic suite PASSED.
- Downloaded ZIPs passed integrity check and contained `DosDis.Desktop.exe` and `DosDis.Cli.exe`.
- Desktop Windows x64 artifact ID `11584757949`; CLI Windows x64 artifact ID `11584847501`.
- The temporary scoped push trigger was removed. The authoritative workflow is **manual-only `workflow_dispatch`**; no recurring builds or Linux work.

## Scope verified vs not verified

- Verified via compiler and regression suite: source byte-preserving `RawHexPreview.Render` including EOF, ASCII and file-offset bounds, original-byte immutability, unsupported-format File Intelligence label, previous core analysis and export behavior.
- Verified via code inspection and compilation only: selecting Explorer function no longer forcibly calls `MainTabs.SelectedIndex = 0`, while address/hex/xref context updates. User interaction must test it.
- Not yet verified: WPF user interface after Part 28, actual data/resource file (OBJECTS.DAT, INFO.DAT, etc.) drag/open workflow, tab retention during selection, native GUI exception handling, original CODE.1/CODE.2 regression against private originals.
- Not yet implemented: structural parsing for non-LE resources, 16-bit MZ/COM/NE, general purpose format recognition, analysis debug tracing/crash report backlog.

## Acceptance test for owner

1. Extract fresh Part 29 desktop package on Windows 11, launch `DosDis.Desktop.exe`.
2. Open CODE.1 or CODE.2 and Analyze. Select **Hexadecimal** tab. Click several function candidates in Explorer; verify the selected tab remains Hexadecimal while hex contents and selected address change.
3. Repeat on References; tab should remain References, while incoming/outgoing references correspond to the currently selected candidate.
4. Open arbitrary data (e.g. `OBJECTS.DAT`), expect automatic Hexadecimal view, **RAW FILE — NO FUNCTION ANALYSIS** and honest `analysis_support` metadata. Seek with `file:0x100` and check displayed original bytes. Do not expect fabricated assembly or exported game assets yet.
5. Report screenshots, output, and crash details for remaining runtime issues.

## Completion score

**Estimated 40% overall first-final Windows release**, up from Part 28's 39%, since Windows compilation and synthetic verification established the raw-file inspection checkpoint. The change is deliberately modest because GUI interactivity, broad binary loaders, crash/session debugger, project UI end-to-end validation and game fidelity remain unverified.

Original binaries untouched. Windows-only. GitHub Actions manual-only. AI integration after final release only.
