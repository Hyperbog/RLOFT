# Part 19 — Windows desktop and project integrity verification gate

Date: 2026-10-08

**Status: Source repairs completed; .NET 10/WPF compilation and application runtime test NOT performed in this session.** GitHub Actions remains `workflow_dispatch`-only and no hosted minutes were requested. This checkpoint is a static-and-source audit, **not** an executable release certification.

## Repaired functional issues

1. **Jump-to navigation preserved:** navigating to an address not already in the analyzed listing previously replaced all instruction rows with a temporary linear preview. The UI now appends missing preview instruction starts to the existing sorted listing, marks those rows `linear-preview-unverified`, and preserves the original analyzed rows/graph, functions and xrefs.
2. **Project switch resets stale analysis:** when opening another valid project for the same file that has no saved analysis, the previous project's graph/listing is cleared and a distinctly unverified entry preview appears. This avoids implying the old saved analysis belongs to the newly opened project.
3. **Annotation editor resets safely** when a different analysis/listing is displayed; the inactive Save Comment button does not present stale revision state.
4. **WAL-safe project backup:** `ProjectStore.CreateBackup` uses SQLite `BackupDatabase` and refuses existing target files or self-overwrite. An active WAL database must not be backed up via arbitrary `.ddproj` filesystem copy. The desktop exposes a **Backup project** action for committed data; unsaved text in the editor is not included.
5. **Expanded persistence regression:** synthetic tests now check committed analysis and updated annotations in a consistent backup, rejection of an existing backup destination, unknown-schema rejection, and project reopen. Optional CODE.1/CODE.2 arguments drive bounded candidate analysis plus save/reopen/hash-preservation tests without embedding game data.
6. **Manual Windows workflow extended:** it now builds the desktop and both smoke suites, then publishes separate self-contained Windows desktop and CLI preview artifacts **only if explicitly started**. No automatic push, pull-request, cron, Linux, or AI jobs.

## Required manual Windows command

From `Tools/Disassembler`, install the .NET 10 SDK on Windows 11 and execute:

```powershell
.\build-windows.ps1 -OriginalCode1 'C:\Games\Ravenloft\CODE.1' -OriginalCode2 'C:\Games\Ravenloft\CODE.2'
.\build-windows.ps1 -Publish
```

Alternatively, invoke **Windows DOS disassembler core smoke tests** using `Run workflow` in GitHub Actions **only if you elect to consume the minutes**. The manual workflow uses synthetic fixtures; the original game binaries remain private and local.

## Remaining mandatory runtime acceptance gates

- Build and run WPF .NET 10 on Windows 11, including package restoration, `DosDis.Desktop.exe` startup, tabs, fonts, keyboard navigation, scaling and file pickers.
- Open exact SHA-256 matching original CODE.1 and CODE.2. Verify File Intelligence, original-file hex and object offsets, stored-vs-relocated bytes, and function navigation.
- Save analysis to a new `.ddproj`, add a note, reopen and verify graph/evidence/note preservation.
- Navigate from a known function to an address outside the current listing, confirm that analyzed functions remain browsable and the appended row is visibly unverified.
- Open a project containing the right artifact but no analysis; confirm the old analyzed listing is not inherited.
- Back up a live project and reopen the new backup with correct annotations and analysis. Decline overwrite when the destination exists.
- Test corrupted/unknown-schema databases, stale concurrent edits, interrupts or cancellation during long analysis, and display of very large listings.
- Validate zero hosted Actions minutes used unless explicitly authorized. Verify package manifests, exact NuGet lock state and desktop binary provenance before any public release.

## Scope boundaries

This is an early WPF desktop preview, not the finalized Windows 11 visual design. The loader remains 32-bit LE-focused; unresolved instructions and jump tables remain candidates. No original executable bytes are changed. AI integration is deferred until **after** the final release, and no high-level language generator is being implemented.
