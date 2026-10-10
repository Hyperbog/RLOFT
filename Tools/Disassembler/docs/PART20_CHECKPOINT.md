# Part 20 — Cancellation, saved-project validation, and build-readiness checkpoint

Date: 2026-10-08

**Overall application progress estimate: 30% toward the full first final Windows release.** This percentage is a deliberately conservative **planning estimate**, not an empirical game coverage/fidelity metric or a release certification. It weights loader/disassembly, advanced analysis, persistence, Windows UI, File Intelligence, and release testing. The estimate will be updated at every subsequent checkpoint, with the percentage adjusted for verified results rather than code volume. A partly implemented but uncompiled WPF desktop does not count as a functioning Windows release.

## Changes

1. `ControlFlowAnalysis.Analyze` now accepts an optional `CancellationToken` and checks for cancellation during entry validation, recursive disassembly, leader/block and function construction, and large classification scans. Cancellation throws `OperationCanceledException` rather than silently returning a falsely completed analysis.
2. The WPF preview now has **Cancel analysis**, requests cooperative cancellation of the running worker, does not overwrite the previous analysis when cancelled, and prevents window shutdown while background file/project work is active. When analysis has already completed and a project snapshot is being written, cancellation is disabled to avoid suggesting that persisted results can be rolled back mid-transaction.
3. Cached SQLite analysis metadata now cross-checks the restored payload's artifact SHA-256, instruction count and truncation state. Null or inconsistent core collections are rejected with `InvalidDataException`. Existing `.ddproj` schema version 1 is unchanged; existing projects remain loadable.
4. Added synthetic C# regression cases for pre-cancelled analysis and inconsistent stored instruction counts. Those cases are **authored but have not been compiled/run on Windows yet**.

## Offline/source validation actually performed

- Parsed WPF XAML and confirmed 12 UI event handlers are implemented.
- Executed the SQLite STRICT schema using a local SQLite test database and confirmed the metadata-mismatch fixture can be created.
- Checked cancellation and metadata-guard source contracts.
- Verified `.github/workflows/disassembler-core.yml` is `workflow_dispatch`-only, Windows-only, and lacks Linux jobs.
- Confirmed that the preparation environment has **no .NET SDK**. Therefore **no C# build, new C# test execution, Windows app launch, visual verification or original-binary GUI regression has been completed during Part 20**.

## Suggested verified Windows acceptance path (no GitHub Actions minutes)

Run from `Tools/Disassembler` on Windows 11 with the .NET 10 SDK:

```powershell
.\build-windows.ps1 -OriginalCode1 'C:\Games\Ravenloft\CODE.1' -OriginalCode2 'C:\Games\Ravenloft\CODE.2'
.\build-windows.ps1 -Publish
```

Launch `src/DosDis.Desktop/bin/Release/net10.0-windows/DosDis.Desktop.exe` or the published Windows desktop output. Open CODE.2, run analysis, exercise Cancel during analysis and verify that a prior graph is not replaced, save/open/backup a project, test a metadata-corrupted project, browse File Intelligence evidence, test display scaling and very large listings.

The manual GitHub workflow is available but **should be invoked only with explicit approval to spend Actions minutes**.

## Progress accounting (estimate; intentionally rounded)

| Release area | Weight | Estimated completion | Rationale |
| --- | ---: | ---: | --- |
| DOS file loading, mapping, x86 decoding | 25% | 60% | Proven early Ravenloft CLI and synthetic cases; broader DOS formats remain |
| Functions, CFG, references, code/data | 20% | 40% | Candidate recursion and limited relocation-backed indirect recovery; substantial work remains |
| Persistent research projects | 15% | 15% | SQLite implementation and source tests; runtime certification pending |
| Windows 11 desktop experience | 20% | 10% | WPF source exists; no verified Windows build or UX testing |
| Software/File Intelligence | 10% | 20% | Initial evidence-linked fingerprints, not comprehensive recognition |
| Distribution, fidelity and certification | 10% | 3% | Earlier CLI CI; no GUI release testing or accepted final build |

Weighted estimate: **~30%** (rounded from the planning score). It is a project completion estimate, **not a claim of 30% original game behavior parity or code coverage**.

## Status and blockers

- **Implementation**: source ready for Windows build attempt; not yet a verified Windows desktop.
- **GitHub**: manual commits, `workflow_dispatch` only. No automatic workflow invocation, Linux work, or AI assistant implementation.
- **Post-final release**: optional LLM assistance remains deferred and excluded from the active development plan.
- **Next priority**: Obtain first successful local Windows 11 compile, fix compiler/runtime issues, then exercise project open/save, large listings and analysis cancellation before expanding UI features.
