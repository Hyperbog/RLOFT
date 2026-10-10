# Part 26 — Windows 11 theme corrections from first original CODE.1 UI review

**Status: source-only UI checkpoint; Windows build and visual acceptance of these modifications PENDING.**

## Evidence observed in the user's Part 24 screenshot (2026-10-08)

- Windows WPF executable actually starts and successfully opens CODE.1 (archive SHA-256 and LE architecture/fixup fields appear in File Intelligence).
- Disassembly listing, function candidate list, metadata table and status panel are populated.
- Critical usability defects: multiple disabled command buttons are bright white on the dark top bar, dark tabs/table headers render with near-white Windows default themes, selection contrast and typography are poor, and File Intelligence metadata columns truncate content with fixed narrow widths.
- The screenshot **does not** verify Save Project, navigation actions, annotations, cancellation, hex/reference tabs or fidelity of code/data classifications. It should not be interpreted as 100% parity with the DOS game.

## Implementation changes

- All interactive WPF button states use explicit dark `ControlTemplate` backgrounds (hover, pressed, focus, disabled). Primary file-open and Analyze actions use a restrained accent treatment.
- Tab headers, table headers, row/cell selection and function-list hover/selected states have explicit dark-theme resources instead of the white platform-theme default.
- Text inputs retain dark background in focus/disabled states with a visible accent focus border; preserves `PART_ContentHost` for Windows text editing and scrolling.
- File Intelligence is a star-width, virtualized DataGrid instead of a fixed-column GridView, with full-value hover tooltip and the existing source-evidence detail area. This is UI-only: same `IntelligenceList` name and selection event API, same data binding.
- Slightly larger fonts and more legible row/header spacing throughout disassembly and navigation panes. Minimum desktop width increased to 1180 to accommodate the existing seven command buttons.
- No changes to DOS decoder, disassembly behavior, LE fixups, original binary handling, SQLite persistence, or reporting semantics.

## Acceptance gates

1. Build `DosDis.Desktop.csproj` on Windows 11 using the existing manual-only `build-windows.ps1` script.
2. Launch on Windows 11, check *enabled and disabled* button colors, tab switching, grid headers, hover/selection and keyboard tab/focus.
3. Verify hex view text editing host scrollbars, resized File Intelligence value column and complete tooltips; test focus and proper accessibility with 100%/150% DPI, 1280 px and 1920 px desktops.
4. Load the original CODE.1 and CODE.2, confirm unchanged original hashes, candidate counts and source-evidence semantics; compare results to Part 24.
5. Only after a successful build and visual acceptance issue a new WPF executable; **Part 24 binary predates these changes.**

**Progress estimate: 37% overall (up from 35%)** due to first real Windows GUI startup, original CODE.1 identification and listing shown by the user. The new UI theme modifications themselves do not count as tested. GitHub Actions stays strictly manual-only, with no automatic run or Linux build; AI remains post-final-release.
