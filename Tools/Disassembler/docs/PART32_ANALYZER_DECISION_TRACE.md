# Part 32 — Bounded Evidence-Linked Analyzer Decisions

**Status:** source checkpoint. Windows compilation and interactive GUI acceptance are pending.

The Part 31 operation-level diagnostic session cannot explain individual
decode, relocation and CFG traversal decisions. Part 32 introduces an optional,
single-use AnalysisDecisionTrace argument to ControlFlowAnalysis.Analyze.

- Versioned in-memory JSON, source SHA-256, monotonic sequence IDs, enumerated
  rule/outcome identifiers, object-relative addresses, nullable physical-file
  offsets, candidate destinations and speculative indicators.
- Bounded capacity: 512 decisions by default (range 8–4096); oldest entries
  evict first, with an explicit dropped-event count.
- Logs explicit entry seeds, protected data, rejected/overlapping/invalid
  instruction hypotheses, accepted candidate instructions, analysis budget
  stops, relocation-backed indirect targets, pointer tables and unresolved
  branch destinations.
- Never serializes original bytes, source paths, arbitrary provenance text,
  analyst comments or exception messages. No uploads or automatic files.
  Findings remain *candidate/unverified*, not proof of runtime execution.

Synthetic regression source verifies original graph equivalence, evidence
offsets, bounds, rejection reasons, speculative classification and privacy.
**Not yet verified:** Windows compilation, smoke test execution, CODE.1/CODE.2
original-file regression, GUI trace inspection, replay or stepping.

**Estimated first-final-release progress: 43%**, unchanged until a real Windows
build and the synthetic suites pass. Windows-only, manual Actions only.
No copyrighted originals are uploaded to CI. AI stays deferred.
