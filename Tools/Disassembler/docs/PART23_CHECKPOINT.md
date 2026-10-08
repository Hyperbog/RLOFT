# Part 23 — saved instruction evidence revalidation (2026-10-08)

**State:** source implementation plus offline structural verification. **No Windows 11 / .NET 10 executable build or GUI launch has been performed.** User-requested GitHub Actions remain manual-only, so no hosted CI was triggered.

## Problem

A project can have valid SQLite schema, successful database `integrity_check`, matching artifact SHA-256, and consistent JSON/row metadata, yet still contain *incorrect cached instruction bytes*. That may occur after tool changes, imported damaged snapshots, or tampering. Showing such bytes as if they came from the opened original would mislead the disassembly and contaminate research.

## Implementation

- New `SnapshotEvidenceVerifier.Verify(FlowAnalysisResult, LeImage)` checks the opened original artifact hash and every restored instruction's object, executable attribute, range, byte count, unique start and original stored bytes.
- `ProjectStore.LoadLatestAnalysis(hash, verifiedOriginal)` optionally invokes this source-byte verification **after** SQLite snapshot metadata validation. Existing callers without an original LE image may still inspect a project, but do not gain source verification.
- Both WPF restore paths now supply the current `LeImage` to the verified overload. A mismatch raises `InvalidDataException`; a corrupted stored instruction is not silently presented as original evidence.
- Synthetic persistence tests include a tiny generated MZ/LE object with a valid `NOP; RET`, an unmodified analysis round trip, deliberately altered `OriginalBytes` (rejected), and a duplicated instruction start (rejected). Optional original CODE.1/CODE.2 project tests now use source-verified restore.
- The original executable is always read-only; the saved project is independent of the game data and must not contain the full original binary.

## Evidence semantics

This validator verifies that **cached machine bytes match the hash-identified original executable** at the recorded object-relative address. It does **not** establish that the address is executable at runtime, that the x86 instruction is the right semantic interpretation, or that all branch targets are known. Candidate, protected, and unknown classifications are unchanged.

## Local review performed

- WPF XAML XML parsing and handler-name presence.
- .NET project file XML parsing and Windows-only WPF target setting.
- SQLite schema creation/integrity using a local SQLite engine (this is **not** a C# smoke test).
- Exact source fingerprint audit of uploaded CODE.1 and CODE.2, and byte-level mapping to the known LE object data.
- GitHub workflow has `workflow_dispatch` only, no `push`, `pull_request`, `schedule`, or Linux matrix.
- Source-level assertions confirm verified original use in both desktop restore paths and the optional original-binary roundtrip test.

**Pending gates:** Compile the Windows WPF project and both C# synthetic smoke suites with the .NET 10 SDK; run `build-windows.ps1` locally; open original CODE.1 and CODE.2, save project, mutate a cached source byte only in a copy of the project, and confirm safe refusal; navigate and reopen in a real Windows 11 GUI.

## User constraints / progress

Windows x64 only; no Linux work. GitHub Actions are strictly manual and unused by this checkpoint. AI assistance remains deferred until after final release. The optional session export/crash/analyzer-debugger specification is still a planned backlog item, not implemented here. First-final-release progress remains **30%** pending verified Windows build and GUI acceptance.
