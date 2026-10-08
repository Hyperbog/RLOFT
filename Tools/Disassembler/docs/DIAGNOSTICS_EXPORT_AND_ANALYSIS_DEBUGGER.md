# Diagnostic Exports, Crash Reports, and Analysis Debugger

**Status:** approved design/backlog requirement; **NOT IMPLEMENTED**.
**Requested:** 2026-10-08 by project owner; implementation may be scheduled later, after the first functional Windows GUI is running. Maintain Windows-only releases, original-byte fidelity, and manual GitHub Actions. No AI feature is involved.

## Why this feature matters

Reverse engineering sometimes yields an incorrect instruction, function boundary, relocation, pointer-table interpretation, or code/data decision without crashing. Users and maintainers must be able to reconstruct **what a particular analysis run did and why**, not merely view its final output. Unhandled failures should produce useful, privacy-controlled diagnostic packages rather than lose the research session.

This is **an internal disassembler-analysis debugger**, distinct from a live DOS program debugger. Running or stepping through the original game in DOSBox/DPMI is a separate future integration, not a prerequisite for diagnostic recording.

## Proposed user-facing features

### A. Export analysis and research results

User-initiated File > Export, with explicit selection of artifact, objects, functions, ranges, annotation states and output format:

- Structured, schema-versioned **JSON**: File Intelligence with evidence, object/page mappings, original/relocated address mappings, fixups, instructions, blocks, functions, xrefs, candidates, conflicts, unresolved items, tool version, and relevant analysis settings.
- **CSV/TSV** for indexed functions, instructions, strings, data references, fixups and diagnosis tables; preserve hexadecimal addresses as strings and quote untrusted text.
- **Text/Markdown** human-readable disassembly and audit summaries; original bytes and instruction provenance optional.
- **Graphviz DOT** for selected function CFGs or call graphs, labeled as candidates unless independently verified.
- A compact portable **project report** without the original executable or private analysis history by default.

Do not conflate an exported candidate listing with proven original code. Preserve SHA-256, artifact version, module/object-specific addresses, evidence classification, and analyzer version on every relevant record. No high-level language-generation export.

### B. Session recording and replay

- Give every run a UUID and start/end timestamps, app/core version and source commit, original artifact SHA-256 and size, selected project ID, parser/decoder settings, budgets, manual seeds/protected ranges, relocation mode, and any active user annotations.
- Record structured, monotonic events: import -> nested LE probe -> address mapping -> fixup apply -> seed selection -> instruction decode -> block/function traversal -> indirect-target candidate -> code/data decision -> persistence/export, with per-event artifact address, confidence, dependencies, reason/source-evidence IDs and result.
- Maintain a **bounded ring buffer** of recent diagnostic events in normal mode. Extended trace mode is opt-in for a specific run, with adjustable time/file-size limits, log rotation, cancellation, and redaction.
- Expose a **Session Inspector**: timeline, event filters, warnings, instruction/byte views, "Why did it classify this as code?", side-by-side candidate evidence, and jumps into assembly/hex/CFG views.
- **Replay analysis deterministically** from the same binary, version, ruleset and captured settings where possible. On mismatch, show missing evidence or nondeterministic external input rather than pretending perfect replay. A recorded trace can always be inspected without replay. Replaying must not automatically overwrite verified analyst findings.
- On unexpected exceptions, retain a minimally sufficient incident file even when the main UI cannot open; offer "Recover previous session" next launch, using atomic/transactional writing.

### C. Crash reporting and support bundles

- Catch and log .NET UI dispatcher, background task, and process-level unhandled errors (recognizing not every fatal/native crash can be recovered in-process). Never conceal a fatal error and continue with inconsistent project state.
- Export an explicit **Diagnostic Bundle ZIP**: manifest, anonymized environment summary, source artifact SHA-256 (**not** source contents), session events, bounded stack traces, pass diagnostics, latest errors, version/build identifiers, project schema, repro steps (when supplied), checksums and attachment inventory.
- **Do not auto-upload** diagnostic bundles, raw EXE/game binary bytes, private projects, comments, usernames, machine paths, tokens, or API keys. Preview what will be exported and allow opt-in inclusion/redaction of sensitive fields. Crash memory dumps/minidumps are **off by default**, because they may contain copyrighted game bytes and sensitive user data.
- Detect and validate bundle format on later re-import into the Session Inspector. Treat user-supplied bundles as untrusted; enforce ZIP decompression/entry limits, reject traversal paths, and never execute imported data.
- Preserve an emergency recovery path for unsaved annotations and allow project restoration/rollback using the existing SQLite snapshot/backup design.

### D. Analyzer debugger (not emulator debugger)

- A Debug Analysis mode can pause or step through analysis **passes and decisions** (loader objects, fixups, decode, traverse edge, classify bytes, recover indirect branch). A breakpoint is an analyst-defined address, rule, decision type or diagnostic severity, not an instruction breakpoint in a running DOS process.
- Inspect state at each stop: current artifact/object address, original bytes, optional relocated memory, decoded instruction, incoming branch, work queue, function candidate membership, evidence IDs, table hypothesis and warning messages.
- Commands: Continue, Pause, Step decision, Step pass, Stop; bookmark an event, attach a local comment, compare two runs.
- Implement using bounded trace hooks and stable event schemas first; only enable interactive stepping after cancellation, thread ownership, database consistency and UI responsiveness are tested.

## Delivery gates / sequencing

1. **Before public beta (important):** minimum structured crash/event log, guarded incident save, user-initiated analysis JSON/text export, report redaction preview.
2. **Before 1.0 final acceptance (target):** session viewer with evidence-linked event timeline, portable diagnostic ZIP, explicit trace recording, original-binary retention protections, regression tests and valid/invalid bundle import.
3. **After functional WPF workspace and core stable:** interactive step-through *analysis* debugger, optional if reliability/performance certification requires postponement; its traces should already support debugging even before step UI exists.
4. **Separate future scope:** actual DOS process emulator/debugger bridge, if warranted; does not replace static disassembly evidence and should not run untrusted executables in the analysis process.

## Required acceptance tests

- Simulated bad LE fixup, decoder dispute and misclassified embedded data produce an inspectable event chain with file/object offsets and source evidence.
- Forced exception after annotations and during background analysis produces a bounded crash report; the original game file remains hash-identical; corrupt/partial project data is rejected or recovered safely.
- Offline session export/reopen works without the original executable (inspection only); deterministic replay requests the **same SHA-256 verified original** and refuses a mismatched file.
- Captured logs/sanitized bundle contain no embedded source bytes, secrets or private paths by default; opt-ins are explicit.
- Exported CSV/DOT/text cannot inject spreadsheet formulas or silently elevate an uncertain instruction to verified.
- Large traces stay bounded, are cancellable, and do not freeze WPF.
- All builds/tests run locally on Windows by default; GitHub Actions remains manual-only and never runs automatically.

## Progress accounting

This document is **requirements planning only**; it contributes **0 completed implementation percentage**. Overall first-release disassembler progress remains at the most recent 30% estimate until actual functionality and verification change it. No AI integration is required or permitted before final release.
