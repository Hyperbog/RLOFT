# Part 21 — Windows build preparation and research safety

**Status:** source changes prepared; .NET 10/WPF compile and Windows runtime tests NOT YET VERIFIED. Overall first-final-release estimate: **30%**, unchanged.

## Resolved by source inspection

- SQLite project backups are now made in a unique file within the requested destination directory; `BackupDatabase` plus integrity/foreign-key checks finish before a non-overwriting atomic `File.Move` exposes the final `.ddproj`.
- A crash during the temporary backup cannot leave an apparently complete destination. A concurrently created destination is never overwritten or deleted by cleanup.
- Backups leave only bounded, unique temporary cleanup candidates; clean-up failure does not replace the initial exception.
- WPF candidate analysis is not published to the displayed graph until the project's `SaveAnalysis` succeeds. Cancellation still preserves prior findings.
- Persistence smoke tests explicitly check that backups do not leave `.part` artifacts, and pre-existing backups remain readable and unchanged after a refused overwrite.
- Optional local-only PowerShell transcript: `build-windows.ps1 -LocalLogDirectory 'C:\DOSDisLogs'`. Transcript may include original file paths; manually review before any sharing. No automatic upload.

## Mandatory next verification on Windows 11

```powershell
cd Tools\Disassembler
.\build-windows.ps1 -LocalLogDirectory 'C:\DOSDisLogs' -OriginalCode1 'C:\Games\Ravenloft\CODE.1' -OriginalCode2 'C:\Games\Ravenloft\CODE.2'
.\build-windows.ps1 -Publish
```

Follow `docs/WINDOWS_DESKTOP_VERIFICATION_GATE.md`, including desktop application startup, binary opening, evidence and annotation navigation, save/reopen/backup and cancelled analysis. Run no GitHub Actions by default (`workflow_dispatch` only). No Linux outputs, no AI integration before final release.

## Explicit uncertainty

XAML and SQLite source structure can be checked locally, but that is not proof of successful C# compilation or actual Windows WPF behavior. Do not distribute this source checkpoint as a verified Windows desktop binary. The diagnostic export, crash and analysis debugger roadmap remains planned rather than implemented.
