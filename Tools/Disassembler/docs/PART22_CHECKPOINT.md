# Part 22 — Windows GUI compile blocker, reference navigation and manual build efficiency

**Status:** source-only, no Windows executable built in this checkpoint.
**Release completion estimate:** 30%, unchanged pending a Windows .NET 10 build and interactive UI exercise.
**Build policy:** Windows x64 only; GitHub Actions workflow runs on `workflow_dispatch` only; no Linux or AI work.

## Audited and changed

1. Fixed a **confirmed source/API mismatch** in the WPF reference grid: `FlowEdge` contains `Destination`, not `Target`. The previous desktop code accessed `x.Target` for `FlowEdge` and would fail C# compilation.
2. The reference grid is now backed by a cached object-relative incoming/outgoing/data index prepared once for each analyzed graph. Selecting an instruction no longer requires scanning every graph edge repeatedly. Double-clicking a resolved reference navigates to the appropriate address. Unresolved references remain visible without navigation.
3. When a binary/project is switched, stale reference indexes are cleared before new data is displayed. Unknown and linear-preview addresses remain unverified.
4. The **manually triggered** GitHub Windows workflow contained duplicate desktop build, persistence test, desktop publication and same-name upload steps. Removed the duplicates so manual verification consumes fewer runner minutes and avoids duplicate-name artifact conflicts.
5. Local `build-windows.ps1 -Publish` now creates a packaged `dist/DOSDis-Windows11-x64-preview.zip` with a `.sha256` manifest after passing the local build/tests and copying redistributable notices. This does not trigger GitHub Actions and is not a production release.
6. Confirmed NuGet Gallery contains the pinned package versions `Iced` 1.21.0 and `Microsoft.Data.Sqlite` 10.0.12. This is package existence verification, **not** actual project restore/build success.

## Manual Windows build verification

On an authorized Windows 11 machine with the .NET 10 SDK:

```powershell
cd Tools\Disassembler
.\build-windows.ps1 -OriginalCode1 'C:\Games\Ravenloft\CODE.1' -OriginalCode2 'C:\Games\Ravenloft\CODE.2' -LocalLogDirectory 'C:\DOSDisLogs' -Publish
```

The script should compile all projects, run synthetic smoke tests, check original CODE hashes and bounded restore tests, then publish the WPF GUI and CLI. On success, inspect `dist/DOSDis-Windows11-x64-preview.zip` and its checksum. The transcript can contain private paths; review it before sharing.

Acceptance checks still pending on Windows: compiler with warnings treated as errors; first WPF startup; open original CODE.1/CODE.2; analysis and cancellation; scroll and select references; double-click navigation including unresolved reference; save/open/backup `.ddproj`; crash/corrupt project handling; close while analysis runs; package launch on clean Windows machine.

## Static-only verification scope

The changed XAML and project/workflow XML/YAML can be structurally checked without .NET. This does *not* establish a successful Windows compile or GUI run. If Windows build fails, use its log as the precise source of the next fixes rather than assuming a passing static inspection means the program works.

**No release percentage increase** from this checkpoint until verifiable compilation and runtime use.
