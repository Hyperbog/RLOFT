# Part 16 — Conservative recursive control flow and candidate code/data classification

**Status:** Implementation checkpoint, not whole-image disassembly certification.

## Scope

`ControlFlowAnalysis.Analyze()` takes the immutable loaded `LeImage`, one or more entry seeds, optional analyst-supplied protected-data spans, an instruction budget, and an optional relocated-memory decoding view. It returns structured, object-relative:

- decoded instruction starts, exact **original** bytes, formatted assembly, and decoder flow kind;
- direct near calls, branches, conditional taken/fallthrough edges, continuation edges, and explicitly unresolved indirect/far transfers;
- basic blocks, candidate function entries and candidate function-to-block membership;
- maximal spans of `candidate-code`, `protected-data`, `unknown-file-backed`, or `unbacked-virtual`;
- conflicts, unsupported flows and analysis-budget diagnostics.

The default seed is the original LE entry. `--seed 1:0x2975C` analyzes a researcher-selected additional entry **instead**. Add `--include-entry` to retain the module entry. `--data 1:0x3A7F8:0x3A7FE` marks an *analyst-specified* protected-data interval. It is not automatically certified historical data solely because the caller supplied it.

## Evidence and non-goals

- **Candidate reachable is not verified executable.** Decodability and reachability based on static control-flow assumptions do not constitute behavioral proof.
- No guess that all gaps are code or data. Unknown bytes stay unknown.
- No jump-table discovery, indirect-call target enumeration, exception-handler recovery, or exhaustive function discovery. Indirect edges remain unresolved.
- No dynamic code inference from relocation targets. Fixups may target data.
- No x86-16 real-mode control flow. The LE loader and 32-bit executable-object constraints remain in effect.
- No automatic handoff of a tail-jump block to a uniquely proven function; function membership may be many-to-many.
- No source-language generation.
- Analyzer size limit: 16 MiB total executable-object virtual bytes per run. Default instruction budget: 100,000, configurable up to 1,000,000. Diagnostics are capped at 2,000 reports; truncated analysis reports `Truncated=true`.
- A protected-data range can prevent speculative decoding, but it is **not** a serialized annotation database yet. SQLite evidence storage remains a future milestone.

## Execution policy

Seed addresses are object-relative, never bare physical-file offsets or preferred-base linear addresses. The recursion follows direct call/branch destinations only when target offsets remain inside the same validated, executable, original-file-backed LE object. Runtime selector transitions, far calls/jumps, out-of-object near edges and indirect transfers require new evidence and therefore remain unresolved.

The decoder uses the original stored bytes by default. `--relocated` decodes a separately reconstructed relocated view but still records the original encoded bytes in instruction records. It **never** decodes unbacked virtual zero-fill as original instructions.

An external interrupt may return or terminate the program, depending on the service. The prototype records an unresolved interrupt transfer and an **assumed** return-continuation candidate; no blanket assertion is made that interrupts return.

The CFG builder splits blocks at branch/call destinations and return continuations. Every direct call target becomes a *candidate function entry*. Blocks reached by more than one candidate are retained in both memberships, covering shared returns such as CODE.2 `0x2975A`. A conflict is surfaced when an alternate seed points inside an already-decoded instruction.

## Reproducible invocation

```sh
dotnet run --project src/DosDis.Cli -- analyze CODE.2 --seed 1:0x2975C --max-instructions 3000 --max-output 80
dotnet run --project src/DosDis.Cli -- analyze CODE.2 --max-instructions 100000 --max-output 200
dotnet run --project src/DosDis.Cli -- analyze CODE.2 --seed 1:0x2975C --data 1:0x3A7F8:0x3A7FE
dotnet run --project tests/DosDis.Smoke -- CODE.1 CODE.2
```

The CLI reports totals separately from its bounded JSON listing. `--max-output` limits displayed records, **not** analysis scope. Results include the original artifact SHA-256 and explicit candidate/unknown statuses.

## Synthetic regression obligations

1. Direct 32-bit near call discovers its target.
2. A conditional backward branch to an instruction before the selected function entry is retained.
3. A shared-return basic block can be associated with multiple candidate functions.
4. Register-indirect jump remains an unresolved edge.
5. Explicitly protected data prevents decoding.
6. Undiscovered file-backed gaps remain unknown.
7. A seed pointing inside a different decoded instruction is reported as a conflict.
8. Seed and protected-range bounds are enforced.
9. A hard instruction budget is disclosed, not silently treated as coverage completion.
10. Original fixture remains byte-identical during analysis.

The optional original-game smoke test uses CODE.2 `0x2975C` and its independently documented branch into the preceding shared `RET` at `0x2975A`.

## Future prerequisites for preservation certification

- Reconcile all verified Ravenloft instruction starts and data islands against a machine-readable oracle.
- Add relocatable pointer and jump-table resolution, far transfers, and code/data reference recovery.
- Import RLOFT annotations with provenance and conflict-resolution workflows.
- Measure precision and recall on independent test corpora, including unknown DOS games.
- Add persistent SQLite projects and GUI cross-reference browsing.
