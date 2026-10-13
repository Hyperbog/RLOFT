# DOSDis original CODE.1/CODE.2 LE open crash — WPF binding root-cause investigation

**Date:** 2026-10-13. **Owner supplied** ten privacy-filtered
`%LOCALAPPDATA%\\DOSDis\\Incidents\\incident-*.json` files
representing **five distinct process sessions**. All incident JSON
stays private in the conversation; no original bytes, user paths,
incident report contents or session IDs are committed here.

## Incident evidence and scope

- 2026-10-09 earlier session: opened and analyzed original
  CODE.1 (17,923 candidate instructions) and CODE.2 (66,321
  candidate instructions) successfully before a DIFFERENT
  `System.IO.FileNotFoundException` on `AppClosed`.
  This is not evidence for the new file-open crash's root cause.
- 2026-10-13 older two sessions: CODE.2 and CODE.1 each recorded
  `FileOpenStarted` -> `FileOpenedLe` ->
  `UnhandledUiException` `System.InvalidOperationException`
  (paired `UnhandledProcessException` also captured).
- 2026-10-13 NEW #65 sessions: CODE.1 and CODE.2 both recorded
  `FileOpenStarted`, `FileOpenMappingComplete`,
  `FileOpenPreviewComplete`, `FileOpenPresentationStarted`,
  `FileOpenedLe`, `FileOpenPresentationComplete`,
  then **about 100 ms later** an unhandled WPF
  `System.InvalidOperationException`, followed by
  process failure. Neither user clicked Analyze.
- Both SHA-bound original identities match the audited originals.
  Thus **core LE loader, decoding preview and the synchronous
  MainWindow presentation path completed** before WPF raised
  the exception from its deferred dispatcher work.
  The #65 stale-selection guard did not resolve this exception.

## High-confidence source-level root-cause hypothesis

Three `<Run Text="{Binding ...}"/>` instances were added to
`Tools/Disassembler/src/DosDis.Desktop/MainWindow.xaml` for
byte-exact disassembly instruction highlighting. Their source
properties `AssemblyRow.BytesBefore`,
`AssemblyRow.BytesMatched` and
`AssemblyRow.BytesAfter` have **getters only**.

**WPF `Run.TextProperty` has
`FrameworkPropertyMetadataOptions.BindsTwoWayByDefault`.**
An implicit TwoWay binding to a read-only source property
produces a deferred `InvalidOperationException` when
WPF realizes a row after DataGrid ItemsSource changes.
Documented framework source:
https://www.dotnetframework.org/default.aspx/4%400/4%400/DEVDIV_TFS/Dev10/Releases/RTMRel/wpf/src/Framework/System/Windows/Documents/Run%40cs/1305600/Run%40cs
and the representative issue
https://github.com/dotnet/wpf/issues/1379 .

This precisely explains exception **class**, time AFTER
`FileOpenPresentationComplete`, and triggering on
displaying original LE instruction rows.
The private incident format deliberately omits exception
messages and stack traces, so the exact throwing frame
has **not** been directly established from user logs.
Classification: **high-confidence framework/source diagnosis
pending owner Windows reproduction**; not independently
proven beyond doubt.

## Targeted production fix

All three XAML bindings now specify:
`{Binding BytesBefore, Mode=OneWay}`,
`{Binding BytesMatched, Mode=OneWay}` and
`{Binding BytesAfter, Mode=OneWay}`.
No original executable bytes, decoder behavior, original
virtual/physical address mapping or SQLite schema changed.

## Focused regression gate

New internal **Windows-only `DosDis.DesktopSmoke`** harness
references the actual WPF desktop, instantiates real
`MainWindow` and Disassembly grid, supplies 128
**synthetic read-only row properties**, checks the
real original-byte `DataGridTemplateColumn` has explicit
`BindingMode.OneWay` for all three source bindings,
then exercises at least 600 ms of deferred WPF layout
and item realization. It cannot check actual CODE.1
or CODE.2, which are prohibited from hosted CI.
GitHub canonical `disassembler-core.yml` now compiles
and runs this platform-specific synthetic smoke BEFORE
publishing a Windows desktop ZIP, in addition to
core and SQLite regression suites.

**Initial CI integration issue:** First smoke build
failed on `Run` method/class naming collision
and a nullable binding warning. After correcting this,
a Windows smoke execution failed during test harness
setup on an invalid null `StartupUri` assignment
rather than the production UI. The harness was
corrected and augmented with CI-only synthetic
exception stack reporting. A successful run must
be evidenced by Windows CI before distributing
a new preview.

## Confirmed Windows CI verification — run #71

- **Canonical source built:** `4aae4380ec32383e7e5a21daa84b34f883a2f6b9`.
- **GitHub Windows Actions:** [run #71](https://github.com/Hyperbog/RLOFT/actions/runs/38344982613)
  **SUCCESS**; all four .NET release builds compiled with
  **zero warnings and zero errors**.
- Pure DOSDis.Smoke (including original-free large CODE.1/CODE.2
  synthetic LE) and DosDis.ProjectSmoke SQLite suite **PASS**.
- **Real WPF desktop smoke PASS**, exact Windows CI output:
  `PASS: real Windows WPF MainWindow, 128 read-only instruction rows, one-way Run.Text bindings and deferred DataGrid layout (no originals)`.
  This tests the previously missing *deferred WPF binding realization*,
  not just LE loader compilation.
- Desktop package integrity check for the pinned original-free
  CODE.1 reference **PASS**. One `win-x64`, self-contained,
  desktop-only artifact **11788283077**;
  ZIP **66,932,416 bytes**, SHA-256
  `a820d9164691d05158864a85a744d23e07de384a31f191a34242b33c3cb54b27`.
  [Download #71](https://github.com/Hyperbog/RLOFT/actions/runs/38344982613/artifacts/11788283077).
  No original proprietary binaries or user incident JSON
  uploaded to public repository or GitHub CI.

The automated WPF smoke initially needed its own corrections
(name collision at compile and invalid null StartupUri during
harness initialization). Both were fixed before #71, which
ran **the actual WPF deferred DataGrid** successfully.

This strengthens the root-cause assessment, but actual
user-observed CODE.1/CODE.2 open acceptance remains **PENDING**.

## Owner acceptance

1. Download the eventual **new successful Windows preview**
   and start in a fresh extracted directory, not #65.
2. Open original CODE.1 and CODE.2 separately; wait
   at least a second for the deferred WPF data grid.
   Both must remain open in Disassembly.
3. Verify address rows/byte-highlights, Hex byte click,
   References and Analyze. Confirm CODE.1 Benchmark…
   can load shipped SHA-bound ledger reference.
4. If any new failure, owner supplies incident(s).
   Files are private and should not be placed in
   the public GitHub repository.

**Scope:** a WPF read-only Run.Text binding fix and
new WPF rendering gate, not a game-code/decompiler
accuracy certificate.
