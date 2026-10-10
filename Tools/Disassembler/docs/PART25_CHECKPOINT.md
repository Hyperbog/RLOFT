# Part 25 — Offline Candidate Analysis Export

**Date:** 2026-10-08  
**Overall first-final-release progress estimate:** **35% (unchanged)**.

## Scope

Implements the first tangible slice of the diagnostic/export roadmap. It does **not** implement full session recording/replay, a crash reporter, or a step-through analyzer debugger. Those remain separate milestones. The original Ravenloft executables and archives are **never modified**.

### Included

- Core, schema-versioned `dosdis-analysis-report` JSON with artifact SHA-256; generated UTC time; analyzer assembly version; explicit `candidate-unverified` classification; counts/truncation; seeds; analyst-protected ranges and provenance; decoded candidate instructions; control-flow edges; blocks; function candidates; file-backed/unknown regions; diagnostics; data references; and jump-table candidates.
- Human-readable text report with analogous sections and object-relative addresses; all results remain candidates, and unresolved edges remain unresolved.
- **Default privacy:** no complete source executable, original input path, private SQLite project comments, or machine-byte excerpts embedded. **Machine bytes require explicit user consent** in the GUI (`Yes`) or CLI (`--include-bytes`). Analyst-specified provenance/diagnostic strings may still be private; review exports before sharing.
- Save as a unique file via a *same-directory temporary file and non-overwriting rename*, avoiding incomplete published exports and destructive overwrites. JSON/text UTF-8 output is capped at 128 MiB.
- WPF `Export…` toolbar action enabled only with analyzed/restored graph; file-type chooser, explicit machine-byte consent, background export, error feedback, and busy-state UI disabling.
- CLI `report` command with `--out` and optional `--seed`, `--data`, `--max-instructions`, `--relocated`, `--include-entry`, and `--include-bytes`.
- Synthetic regression source checks for candidate labeling, omission/explicit inclusion of original instruction byte excerpts, object-relative addressing, report field completeness, file publication without overwrite, and unchanged artifact identity.

### CLI example (on Windows)

```powershell
cd Tools\Disassembler
# Conservative report; machine bytes omitted
.\dist\windows-cli\DosDis.Cli.exe report "C:\Games\Ravenloft\CODE.2" --max-instructions 5000 --out "C:\Research\candidate-analysis.json"

# Explicitly include excerpts of original machine bytes (opt-in)
.\dist\windows-cli\DosDis.Cli.exe report "C:\Games\Ravenloft\CODE.2" --out "C:\Research\analysis-with-bytes.txt" --include-bytes
```

The output path must not already exist. Reports are **not** verified source decompilations, and the application makes no claim of complete executable behavior reproduction.

## Verification status and Actions-minute policy

The new C# export and WPF integration were locally checked for structural consistency, WPF handler wiring, versioned JSON schema shape, and preservation of original artifact byte loading. **The Part 25 additions have not been built on a Windows runner or interactively run.** The previous Part 24 build remains verified only for the earlier baseline (Windows Actions run `37841978786`). The existing GitHub Actions workflow remains `workflow_dispatch` **only**; no new hosted runs were requested.

Before counting this as certified GUI functionality, run the new core/test/desktop sources on Windows 11 using `build-windows.ps1`, open the UI and export reports from CODE.1/CODE.2, inspect resulting privacy/metadata, and validate overwrite refusal. Progress remains **35%** until a meaningful Windows runtime checkpoint.

## Next research steps

1. Add structured local crash/error events and redacted incident package export with explicit consent; do not auto-upload original bytes.
2. Build a session inspector for decision provenance and candidate disputes.
3. Only after a stable, tested trace model, consider interactive analyzer pass/decision stepping.
4. Preserve Windows-only targets and manual-only GitHub Actions. AI integration is postponed until **after** final release.
