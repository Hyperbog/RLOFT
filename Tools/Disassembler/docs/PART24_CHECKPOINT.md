# Part 24 — Windows desktop build certification checkpoint (2026-10-08)

**Build/automated-test gate: PASSED. Runtime GUI interaction: NOT YET TESTED.**
**Overall first-final-release planning estimate: 35%** (previously 30%). This increase recognizes actual .NET 10 WPF compilation, both synthetic suites passing, and reproducible Windows build artifacts. It is NOT game accuracy coverage or a complete-feature percentage.

## Provenance
- Branch: `disassembler-foundation`
- Successful GitHub Windows build: https://github.com/Hyperbog/RLOFT/actions/runs/37841978786
- Source build commit: `60871b883f16041b42c8acc681d6e4088956c473`
- The four project builds succeeded: `DosDis.Core`, `DosDis.Cli`, `DosDis.Persistence`, `DosDis.Desktop`, plus both smoke test executables.
- Synthetic core test output: `ALL SMOKE TESTS PASSED`.
- Persistence test output: `PASS: SQLite identity, WAL-safe backup, schema guard, snapshot metadata, analysis round-trip, annotation conflict, integrity and reopen`.
- Both the desktop Windows x64 preview and CLI Windows x64 preview published and were downloaded and checked.
- The earlier diagnostic builds (all Windows, only after explicit user build authorization) exposed and resolved:
  1. Missing `using System.IO;` in the WPF `MainWindow.xaml.cs`.
  2. Nonexistent `Require` assertion helper in `DosDis.Smoke/Program.cs`.
  3. Nullable restored analysis in `DosDis.ProjectSmoke/Program.cs`.
- Current branch restores `.github/workflows/disassembler-core.yml` to `workflow_dispatch` **only**, and removes the one-time trigger marker. **Do not re-enable automatic CI.**

## Not yet established

A GitHub-hosted Windows CI runner builds and runs command-line regression tests; it **does not** automatically verify that the WPF window launches or that users can navigate it interactively.

Still required before any readiness claim:
1. Extract the `DOSDis-Desktop-win-x64-preview` artifact and run `DosDis.Desktop.exe` on a real Windows 11 PC.
2. Check startup, dark/light styling legibility, window resizing, File Intelligence evidence details, assembly and hex navigation, function/xref browsing, source/relocated bytes, analysis cancellation, error feedback.
3. Test a local `.ddproj` save/reopen cycle and transactional backup in the GUI; test failure/recovery flows without overwriting game binaries.
4. Run authorized original `CODE.1` / `CODE.2` regression with `build-windows.ps1 -OriginalCode1 ... -OriginalCode2 ...`, separately from public CI. Confirm archived object geometry and hashes, investigate false-positive code/data candidates.
5. Assess full exception handling, diagnostic trace/export backlog, performance under original input, licensing notices, and install/distribution requirements.

## Next steps

Continue on Windows only. Keep AI assistance postponed until **after the final release**, preserve original binary evidence and unknown classifications, and do not introduce high-level-language generation. Use manually launched builds/tests and the original-binary validation gate before adding major new UI features.
