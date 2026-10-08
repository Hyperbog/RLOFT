# Part 18 — Local persistence and initial Windows 11 workspace

Date: 2026-10-08

**IMPLEMENTED IN SOURCE, NOT YET COMPILED OR EXECUTED.** This checkpoint was prepared without a local .NET SDK or Windows runtime. GitHub Actions is **manual-only** to avoid consuming Actions minutes. The code must pass a real Windows build and synthetic/original-game tests before describing it as functional or shipping a binary.

## Code delivered

- `src/DosDis.Persistence`: SQLite `.ddproj` single-user project, schema v1 with immutable SHA-256 artifact references, compressed snapshots of candidate CFG results (Brotli), user comments/labels/classification notes, optimistic annotation revisions, integrity/foreign-key checks, and rejection of unknown schema versions. Projects do not embed the complete original executable or original full paths. Cached instruction records **do retain the original bytes of recovered instructions**; project files can therefore contain copyrighted binary excerpts and must not be redistributed without appropriate rights.
- `src/DosDis.Desktop`: WPF .NET 10 Windows-only prototype. Open original file, display File Intelligence with field confidence, linear (unverified) entry preview, explicitly trigger recursive analysis, browse candidate functions, jump to addresses, inspect original-file hex and xrefs, open/save project and add comment annotations with revision checks.
- `tests/DosDis.ProjectSmoke`: synthetic project round-trip, exact hash identity, compressed analysis, persisted comments/revision conflict, reopen and SQLite integrity tests (pending actual compilation).
- `build-windows.ps1`: manual build/test/publish script, with optional user-supplied CODE.1/CODE.2 paths. Nothing launches hosted Actions.

## Manual Windows build

Install .NET 10 SDK on Windows 11. From `Tools/Disassembler`:

```powershell
.\build-windows.ps1
.\build-windows.ps1 -OriginalCode1 'C:\Games\Ravenloft\CODE.1' -OriginalCode2 'C:\Games\Ravenloft\CODE.2'
.\build-windows.ps1 -Publish
```

The last command creates `dist/windows-desktop/DosDis.Desktop.exe` and `dist/windows-cli/DosDis.Cli.exe` if the build succeeds. These paths are **expected build outputs, not confirmed available downloads**.

## Evidence and limitations

- Stored analyses are marked candidate, not certified. WPF's initial linear preview is explicitly unverified. Data gaps and indirect transfers retain their earlier uncertainty.
- Original files stay read-only. The `.ddproj` caches recovered instruction-byte excerpts, but not the complete game executable. Reopening a project requires choosing an original binary with matching SHA-256 and length.
- Project format v1 is versioned; unknown schema versions fail rather than migrate silently. WAL is intended only for local project storage, not network share collaboration.
- Manual notes can be changed only when the currently stored revision matches the UI's revision. Stale updates fail instead of clobbering the analyst's text.
- Initial WPF shell is **not the finished Fluent/WinUI-level interface**. It uses native WPF without docking, the purpose-built virtualized hex viewer, or advanced control-flow graph layout. Hex panes show a bounded on-demand window, with `??` for bytes lacking original backing. The assembly DataGrid enables row virtualization.
- Existing analysis engine remains 32-bit LE specific. 16-bit MZ code, uncommon LE pages/fixups, robust register-based jump tables, project migration/backups, saved window layouts, multi-binary linked projects, keyboard navigation polish, and full QA remain outstanding.
- No AI integration, chat, account login, LLM packages, or high-level-language generation is implemented. AI is postponed until after final release.

## Manual release gate

1. Compile all desktop/core/persistence projects on Windows 11 using pinned packages.
2. Run `DosDis.Smoke` and `DosDis.ProjectSmoke` with zero failures.
3. Re-run original-file fixtures using the user's authorized CODE.1/CODE.2 files, never commit them.
4. Open CODE.2 in WPF, run analysis, save `.ddproj`, reopen and confirm SHA-256, annotation and instruction results.
5. Verify the project contains no original full file paths or complete executable files. Inspect cached instruction-byte excerpts before any redistribution.
6. Validate project crash/recovery, slow UI cases, stale updates, missing binary and malformed projects.
7. Manually trigger GitHub Actions only if the owner wishes to consume minutes; otherwise build locally.

This is a source checkpoint, **not** a release certificate.
