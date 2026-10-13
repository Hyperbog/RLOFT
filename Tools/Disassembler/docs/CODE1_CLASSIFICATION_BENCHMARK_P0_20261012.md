# P0 — Strahd CODE.1 translation-ledger classification benchmark

**Scope:** DOSDis LE object-1 *byte classification*, not full executable,
instruction-boundary or original gameplay correctness.

**Evidence status (2026-10-12):** code and original-free synthetic tests
authored. The publicly audited CODE.1 ranges have been converted into
a pinned manifest and installed in the Windows desktop product. The
original CODE.1 executable has **not** been scored in the project CI
or in this task, and no original-file precision/recall result is
claimed by this document.

## Why this benchmark comes first

A small GNU objdump x86-16 oracle check of 28 original Strahd
DOS MZ launcher instructions cannot measure broad code discovery,
especially when the CODE.1/CODE.2 outer launcher is shared. The
more important risk is over-decoding embedded data and missing
executable machine-code paths reached through indirect control
flow.

The separate `code1-decompilation` workstream independently
reconciled the original protected-mode **LE object 1**:

- Original `CODE.1` file size: **277,787** bytes, SHA256
  `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
- Original LE object 1: **70,402** bytes (`0x11302`),
  file-backed start `0x2C724`, SHA256
  `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203`.
- **69,361** bytes of audited translated machine instructions.
- **1,041** excluded-from-translation bytes: **204** table,
  **575** alignment/zero/padding and **262** generic
  excluded bytes without a uniquely established subtype.
- Audited residual range count: **282**, with **no overlap**.
- The 69,361 instruction bytes are inferred as the complement
  of the complete 1,041-byte exclusion ledger within the exact
  70,402-byte object. **The ledger alone is not a complete
  independently confirmed instruction-START address catalog.**
- `NOP`, `LEA` or self-MOV alignment bytes can be syntactically
  valid x86 instructions while still being **excluded** by this
  specific faithful translation-ledger convention. A candidate
  covering those bytes is scored as a **ledger disagreement**,
  not proven ISA decoding error.

Source of record:
- [CODE.1 residual ledger](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Code1/RESIDUAL_LEDGER.md),
  exact blob SHA `2ce282b0d9acd15620e8d852cf958461fe247ea6`.
- [CODE.1 final audit](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Code1/FINAL_BINARY_AUDIT.md),
  original source geometry and object SHA.
- [CODE.1 progress](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Code1/PROGRESS.md),
  notes and audit history.

## Source-bound manifest, without executable bytes

`benchmarks/strahd-code1-object1-classification.json` contains:

- Exact original full-file identity and extracted object identity.
- Expected object extent and first physical-file offset.
- The 282 explicitly excluded offset ranges, half-open
  `[begin, endExclusive)`, and their exact audited categories.
- An evidence provenance pointer to the pinned repository blob.
- `referenceStatus = audited-translation-ledger-not-execution-proof`.

**No original CODE.1 bytes, disassembly, user file path or
proprietary image is included.** The remaining 69,361 object bytes
are the reference's machine-code translation class by complement.
This is a *source research interpretation*, not a runtime trace
or a second independently compiled decompilation.

## Implementation

The pure `CodeClassificationBenchmark` core:

1. Parses bounded JSON references; checks schema, source and
   object hashes, byte counts, sorted nonoverlapping ranges,
   category whitelist, provenance metadata and integer bounds.
2. Requires exact matching original-file SHA/length,
   `LeImage` identity and `FlowAnalysisResult` hash.
3. Extracts only the referenced LE object using the validated
   loader's read-only source map, verifies the independent
   original **object SHA**, and validates the manifest's
   physical-contiguity assertion if supplied.
4. Rejects invalid/overlapping candidate instruction starts,
   lengths, bytes that contradict the original, or candidate
   intervals that extend beyond the reference object.
   Other objects are not scored in this reference.
5. Scores every object byte as **candidate-on-translated**,
   **candidate-on-excluded**, **unclassified-translated**,
   or **unclassified-excluded**. The last class is an abstention,
   NOT proven true-negative data classification. Unknown bytes
   cannot inflate precision.
6. Reports candidate-byte precision as
   `candidate-on-translated / (candidate-on-translated +
   candidate-on-excluded)`; translated-byte recall as
   `candidate-on-translated / 69361` for this exact reference.
   Precision without any predicted instruction bytes is
   explicitly **undefined**, never 100%.
7. Produces source-offset disagreements:
   `candidate-on-excluded` and
   `translated-not-discovered`, bounded to at most 128 spans
   of each class with explicit truncation.
   Scoring an instruction-budget-truncated graph retains its
   `AnalysisTruncated` flag.

The Windows WPF **Benchmark…** action is enabled only for
a file with an actual LE analysis graph. It imports an
independently provided JSON classification manifest from
the owner's local filesystem; the pinned CODE.1 manifest ships
in `benchmarks/` with Windows desktop builds. Exact source
and object identities must match. The Benchmark tab reports
precision/recall, the four byte-accounting categories,
uncertainty labels and an actionable disagreement list.
Double-clicking a disagreement jumps to the original physical
Hexadecimal byte without inventing a new code candidate.
The entire analysis and original source remain read-only.

## Synthetic acceptance (no original game bytes in hosted CI)

`CodeClassificationBenchmarkSmoke.Run` is included in the
existing `DosDis.Smoke` suite and uses an original-free
four-byte synthetic LE object with NOP/RET as translation
and a non-translated `CD 21` pair. Tests check:

- Original CODE.1 manifest size/count/282 ranges and subtype sums.
- Exact positive precision/recall and missing-code recall.
- False positives on excluded bytes reduce precision.
- Zero predictions produce undefined precision, zero recall.
- Excluded unselected bytes are abstentions, not true negatives.
- Full-file SHA, object SHA, physical offset, forged instruction
  bytes, duplicate/overlap, reversed exclusions, inconsistent
  cardinality and malformed/oversized reference rejection.

These tests intentionally **do not** include original CODE.1
bytes or score the real game in public GitHub Actions.

## Remaining work before a credible accuracy claim

- Owner-local, exact-source run of original CODE.1 through
  DOSDis on the built binary, import the pinned ledger manifest,
  and retain the source SHA, tested build SHA, counts and error
  classifications as a **private** local report.
- Review false positive candidates carefully: a ledger-excluded
  NOP may still be a valid instruction. Distinguish ISA decoder
  validity from reference translation decisions.
- Independently derive an **instruction-start and length**
  catalog from the original code audit (not from DOSDis
  candidate predictions), then report start-level precision,
  recall and wrong-length/overlap cases separately.
- Extend beyond object 1 only after independently bounded
  original inventories exist; do NOT imply whole CODE.1
  or all-title coverage from this one object.
- Add held-out DOS executables, trace evidence for indirect
  paths and compiler/runtime signatures after the baseline
  source-bound classification is measured.
- A byte-identical reassembly is a separate stronger
  preservation objective but not by itself proof of meaningful
  recovered source, especially if raw-byte directives
  substitute for decoded code.

**Classification:** NEW P0 classification workbench foundation,
audited ledger manifest and original-free scoring contract.
**NOT** a validated original-file score, recovered instruction
boundaries or completed general disassembler accuracy gate.
