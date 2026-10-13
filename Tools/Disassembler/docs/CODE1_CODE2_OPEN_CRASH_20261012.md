# CODE.1 / CODE.2 desktop open crash — field report and mitigation

**Reported by owner (2026-10-12 local):** Windows DOSDis #60
closed/crashed when attempting to open original `CODE.1` and `CODE.2`.
This is a **release-blocking original-file GUI acceptance failure**;
the earlier #60 benchmark package success only proved compilation,
packaging and synthetic LE scores, not actual original LE opening.

## Local original-byte investigation

The owner-supplied exact source files are locally available to the
private analysis environment. No original game bytes or private
file paths were committed or sent to hosted GitHub Actions.

- `CODE.1`: **277,787** bytes, SHA256
  `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
- `CODE.2`: **558,689** bytes, SHA256
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Both contain one MZ-bound LE header at file offset
  `0x27BDC`, nested MZ at `0x25D24`.
- Both describe two LE objects and page-size 4096.
- CODE.1: 24 LE pages, first object's virtual extent
  `0x11302` bytes, LE entry `1:0xCC1C`, original
  code-page store begins file `0x2C724`; a private
  independent bounded fixup-byte walk completed **2,617**
  internal fixup records.
- CODE.2: 78 LE pages, first object's virtual extent
  `0x3A852`, LE entry `1:0x353A0`, original
  stored pages begin file `0x3AF24`; the same
  independent fixup-byte walk completed **9,813**
  internal fixup records.
- This is **structural corroboration**, not C# execution
  of the original files. It does not establish the WPF
  crash's specific exception type or stage. Never
  report a root cause or original-file crash fix as proven
  until an owner Windows reproduction confirms it.

## Changes in the new Windows desktop

- Desktop open now explicitly brackets new-binary WPF
  presentation with `_bindingNewSource`: selecting
  or removing stale graph/function/references rows
  during `ItemsSource` replacement no longer attempts
  to map the old row through the newly opened
  file's LE/MZ address space.
- The assembler grid clears old selected-item and
  References-grid selection **before rebinding**.
- Added four privacy-preserving open-stage diagnostics:
  `FileOpenMappingComplete`, `FileOpenPreviewComplete`,
  `FileOpenPresentationStarted`,
  `FileOpenPresentationComplete`.
  Incident JSON has only controlled event types, SHA and
  exception-class names, not paths, original bytes or
  stack traces.
- Recoverable disassembly selection/view-binding exceptions
  now leave the original read-only source and desktop
  process available, clear only the invalid selection
  and write an owner-local incident record. File-open
  failures also preserve a local incident.
- Existing normal MZ direct-call navigation, LE projects,
  byte search, classification benchmark and privacy-safe
  report exports remain in place.

## CI regression coverage

`LargeLeOpenSmoke.cs` builds completely ORIGINAL-FREE
synthetic two-object LE containers matching the publicly
documented sizes, object counts, page mapping geometry,
entry offsets and **2,617 / 9,813** fixup counts.
Synthetic payload pages are NOP/RET and zero data,
not original proprietary instruction excerpts.
The suite checks the shared LE loader, validated
container detection, File Intelligence, 128 bounded
Iced x86-32 preview rows and exact original-file-backed
hex address navigation. It does **not** execute or
automate WPF GUI controls.

**Automatic GitHub Windows [run #65](https://github.com/Hyperbog/RLOFT/actions/runs/38344030692)**
**SUCCESS**, exact source/test compiled SHA
`6deb51cb3a1c7a3b8acc4068e1b6ad1d3587627d`.

- Three Windows .NET release builds **zero warnings/errors**.
- `DosDis.Smoke` **PASS**, including synthetic large-LE
  opener: `PASS: original-free two-object large LE open,
  2617/9813 fixups, bounded preview/intelligence and
  source-backed hex navigation`.
- `DosDis.ProjectSmoke` SQLite source-backed project
  persistence **PASS**.
- Windows self-contained desktop-only artifact
  `DOSDis-Desktop-win-x64-preview`, GitHub ID
  `11788226495`, archive **66,931,981 bytes**, ZIP
  SHA256 `325d1d7dcfe3ba7c9ccf5b23e0ebf32792ee145bddc33c5a9433ce70aa699be4`.
- [Download Windows #65 ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38344030692/artifacts/11788226495).
- No CLI end-user release. No copyrighted original file
  bytes included in code commits, manifest, tests or
  hosted CI artifacts.

## Mandatory next acceptance

1. Owner launches the NEW #65 Windows desktop and tries
   opening exact unchanged original `CODE.1`, then
   `CODE.2` **before clicking Analyze**.
2. If either crashes again, provide the **newest**
   private `%LOCALAPPDATA%\DOSDis\Incidents\incident-*.json`
   from immediately after the failure (or a screenshot
   of the Windows error dialog / Event Viewer fault).
   The incident file omits proprietary bytes and source
   file paths and will distinguish source-open stages
   from presentation failures. Some hard/native crashes
   may leave no incident; state that explicitly.
3. If both open, select an instruction and inspect
   Hexadecimal and File Intelligence, then Analyze
   each LE file under its normal instruction budget.
   CODE.1's analysis and benchmark dialog must be
   separately acceptance-tested before publishing
   any original precision/recall result.
4. Never certify actual original-code semantics, GUI
   acceptance or complete G2 source accuracy on the
   basis of synthetic build/test success alone.

**Current classification:** mitigated WPF file-open/selection
reentrancy; synthetic large-LE loader PASS; original
CODE.1/CODE.2 Windows GUI reopen and root-cause
identification **PENDING**.
