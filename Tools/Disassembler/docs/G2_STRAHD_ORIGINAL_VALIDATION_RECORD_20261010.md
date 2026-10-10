# G2 Strahd outer-MZ original-source comparison record — 2026-10-10

**Evidence state:** COMPILED / SYNTHETIC VERIFIED / ORIGINAL SOURCE
COMPARED (named narrow samples only). **G2 independent accuracy
acceptance: PENDING**. No human independent reviewer approval,
whole-file correctness or runtime execution proof is asserted.

## Exact build and authorization

- Owner explicitly authorized the G2 build and continuation on
  2026-10-10. Temporary private PR #6 was for builds only; **not merged**.
- Final Actions run **#40 / 38091963405**, SUCCESS:
  https://github.com/Hyperbog/RLOFT/actions/runs/38091963405
- Compiled Windows desktop, Windows CLI, Linux CLI, DosDis.Smoke and
  DosDis.ProjectSmoke; Windows **and** Linux synthetic regressions PASS.
  Both self-contained platform CLIs and Windows desktop were published.
- Exact runner merge checkout:
  `5eaa1fb0798007dfca73e648f91bc1add42409e9`.
  Only temporary workflow trigger/Linux packaging differed from
  canonical code/test SHA:
  `7fa76f822e0aa56a4bf1c611076bc50e5e0a5389`.
- Linux executable SHA256:
  `9a2ee2c8fd7d61aacfbd0702dbdd7f3cbb13fbf2554c60b2de43777577f0a036`.
- Proprietary game originals, original-byte oracle manifests, and
  private comparison reports were **never sent to hosted CI or committed**.

## Original Strahd source

- Title: *Ravenloft: Strahd's Possession*; original `CODE.2`,
  558,689 bytes, SHA256
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Owner-local independent read: MZ outer DOS launcher, declared MZ
  extent 62,532 bytes, 512-byte header, relative CS:IP `01BD:2382`,
  file-backed entry-region base `0x1DD0`, physical entry `0x4152`.
  The protected-mode LE object is **not** this comparison's scope.
- Reference: GNU binutils `objdump` **2.44**, x86-16
  `-D -b binary -mi8086 -M intel --adjust-vma=0x2382`.
  Its independent instruction bytes and source offsets were checked
  against original file bytes, not constructed from DOSDis output.
- Local execution options:
  `mz-analyze <PRIVATE>/CODE.2 --dos-stub --max-instructions 20000
  --external-oracle <PRIVATE>/oracle.json
  --external-report <PRIVATE>/new-comparison.json`.
  No source relocation or runtime segment base was inferred.

## Two bounded, nonoverlapping original-source samples

| Sample | Instructions | Explicit operand facts | Expected direct edges | Missing/conflicting/not observed |
| --- | ---: | ---: | ---: | ---: |
| MZ entry `01BD:2382–239B` | **11/11** | **9/9** | 0 untested | 0 |
| Branch/call `01BD:23D2–23FC` | **17/17** | **6/6** | **5/5** | 0 |
| **Combined** | **28/28** | **15/15** | **5/5** | **0** |

The branch/call window was frozen from independently decoded
original bytes before comparing that sample with DOSDis. Both
comparisons returned `truncated=false` and source hash/mapping
agreement. There were 6,540 distinct DOSDis candidate instruction
starts in this analysis, **6,512 not benchmarked by these two samples**.
The sample denominator does not measure executable coverage and no
candidate is declared runtime-executed on this evidence alone.

The JSON reports deliberately retain `independenceStatus =
claimed-external-not-reviewed` and `analysisStatus =
candidate-comparison-not-certification`; these labels are correct.

## Owner-local negative controls, exact compiled CLI

Eight original-file-preserving negative comparisons were executed:
- Original-hex contradiction -> one `conflict`.
- Incorrect direct target -> one `missing` expected edge.
- Incorrect canonical register -> one `conflict`.
- Foreign original SHA, physical MZ mapping or file byte length ->
  rejected with CLI exit status 1, no report.
- Misspelled expected direct-edge key and duplicate case-variant
  source-SHA JSON key -> rejected with CLI exit status 1, no report.

**8/8 expected outcomes.** The earlier CLI emitted an unhandled Linux
process abort on invalid `InvalidDataException`; canonical commit
`7fa76f8` corrected it. Final Linux CLI rerun established controlled
exit 1 and ordinary diagnostic messages for all five invalid manifests.
Three contradictory-but-well-formed manifests instead produce a
comparison report (exit 0), appropriately separating a finding from
an import failure. These are negative controls, not accuracy oracles.

## Unresolved acceptance requirements

- No independent human reviewer/adjudication or CPU-runtime trace.
- Still lacking comprehensive external signed/unsigned guard,
  indirect pointer/table, far/relocation and unsupported-code/data
  original-source checks and title-qualified cross-game samples.
- No conclusion about unbenchmarked decoder starts, LE x86-32,
  Stone Prophet, Menzoberranzan or whole game correctness.
- The original file is read-only; no original bytes are tracked by Git.
- The comparison is **INDEPENDENT SOURCE-SAMPLE AGREEMENT**, not a
  passed whole-G2 gate. Reviewers must use
  `INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md` and the normative
  `INDEPENDENT_VALIDATION_POLICY.md` before changing acceptance state.

**Final decision: G2 INDEPENDENT VALIDATION PENDING.**
