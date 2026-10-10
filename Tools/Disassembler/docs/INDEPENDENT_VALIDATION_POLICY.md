# DOSDis independent validation policy

**Status:** NORMATIVE / REQUIRED, effective 2026-10-10.  
**Applies to:** all DOSDis accuracy claims, future format acceptance, owner-authorized validation gates G2–G8, and final-release acceptance.  
**Authority:** `ROADMAP_BUILD_GATES.txt`; this document defines its independent-evidence contract.  
**Execution authorization:** unchanged. A request to "continue" authorizes source/docs/fixtures/commits, **not** builds, smoke execution, GitHub Actions, local original-file testing, previews or publication.

## 1. Required rule

**No DOSDis decoder, loader, relocation handler, CFG recovery, function finder, code/data classifier or supported file-format family may be marked independently validated, correctness-certified, or release-ready solely because DOSDis-generated outputs agree with DOSDis-generated tests.** At least one independently established external or prior source-backed expectation must be compared against the precise shipped implementation and original input where applicable.

Compilation, unit/synthetic tests, reproducible output, SHA agreement and GUI acceptance remain necessary evidence for their respective claims. They are **not substitutes** for independent semantic accuracy testing. A candidate instruction or control-flow edge is **not** proved executed solely by having a valid encoding and incoming static candidate path.

Any missing, failing or unreviewed required independent comparison blocks the relevant **accuracy/format acceptance** and final-release gate. It does not retroactively revoke accurately recorded compilation or synthetic-only evidence.

## 2. What qualifies as independent evidence

A usable oracle must have an identifiable origin and expected interpretation **established without using the DOSDis output being evaluated**. Document its producer, tool/version or original research record, creation date, exact input identity, assumptions and limitations.

Acceptable *complementary* classes of oracles:

1. **Independent instruction decoders.** Compare selected byte-identical instruction windows against a separately implemented decoder (for example, Capstone or Ghidra) with explicit x86-16/x86-32 mode, instruction offsets, prefixes, lengths, operands and branch targets. Iced.Intel is DOSDis's embedded decoder; comparing DOSDis against its own Iced.Intel invocation alone is **not** an independent engine-level oracle.
2. **Executable-format standards and separately derived loader maps.** Check MZ/COM, LE and, when implemented, NE/LX/overlays against published format references plus original byte/offset arithmetic. For Strahd LE use the documented IBM/OS2 LE specification and Open Watcom references. A standard is a semantics reference, not proof that every executable complies.
3. **Prior exact-byte research and manually independently checked known answers.** Use the original-game research stored in `Runtime/CODE_OBJECT_MAP.md`, `Code1/FINAL_BINARY_AUDIT.md`, `Code2/FINAL_CERTIFICATION.md` **and their correction/superseding records**. Recheck the expectation against the immutable original bytes, and do not treat a later DOSDis-generated export as independent ground truth. A prior C++ reconstruction alone is not a complete oracle for every assembly address without supporting byte-level evidence.
4. **Dynamic traces or controlled instrumented execution**, only where legally available and reproducible, for runtime reachability, loader effects or mutable state. Static agreement alone cannot establish runtime execution; absence of a trace must remain an explicit uncertainty, not a fabricated failure or success.
5. **Adversarial independently constructed cases**, with expected results derived from specifications or a second implementation, for edge situations not present in sampled originals. Their construction and expectation must not be inferred from DOSDis's current output.

At least two *different* evidence classes must support a claim of verified **format-level semantic accuracy**: one instruction/format/behavior oracle independent of DOSDis and one exact-source or separately constructed case with a pinned expected output. Two views of the same DOSDis result are one source, not two.

## 3. Required comparison units

Record findings at these independently attributable units, as supported by the format:

- **Bytes and instruction boundaries:** original SHA-256 and length; location namespace (physical offset, 16-bit segment:offset, or LE object:offset); mode; stored bytes; decoded start/length/opcode/operands; explicit original-vs-relocated view.
- **Control flow:** independently established direct branch/call targets, fallthrough and return conventions; count each named expected edge as matched, missing, conflicting or not observed. Indirect targets remain candidate-only without runtime evidence.
- **Executable structure:** header origin, object/page mapping, file offsets, file-backed versus virtual-only bytes, fixup source/destination and overlapping-source ambiguity.
- **Code/data classification:** separately verified non-code ranges and code islands; no candidate instruction may silently consume a proven data range. Unknown bytes are not automatically scored as known code or known data.
- **Error paths:** unsupported, malformed, ambiguous, truncated, packed or unbounded cases must fail closed. Do not score an invented target as correct merely because it is plausible.

Separate **known-answer comparisons** from **whole-image coverage**. A partial manifest is a sampled benchmark; it cannot justify a statement such as "100% disassembly accuracy". Report numerator and denominator only over externally verified expectations, plus the number of unbenchmarked candidates.

## 4. Required artifact identity and independence controls

Every comparison record MUST include:

- DOSDis branch, commit SHA, executable build SHA/identifier, analysis options, relocation mode and toolchain/platform;
- binary game/title **and version if known**, internal path, full original SHA-256, byte length, loader/module origin, analysis address mapping and source view;
- oracle class, external source/document/research revision, tool and version where applicable, provenance method, reviewer, sample-selection method and complete expectation set;
- exact matched / missing / conflicting / not-observed / not-classified counts, unbenchmarked candidate count, negative-case results and retained unknowns;
- adjudication for **every** discrepancy, with an evidence link or reproducible counterexample, and a documented disposition.

The filenames `CODE.1` and `CODE.2` are shared by Strahd's Possession, Stone Prophet and Menzoberranzan. The `Code1.json`/`Code2.json` handoff dated 2026-10-10 refers **only** to *Strahd's Possession*; other games require separate hashes, oracles and comparisons. Do not copy object offsets, functions, names, test counts or byte maps between titles by basename.

Known-answer manifests are **read-only comparison inputs**, not traversal seeds and not automatically authoritative because they have `Verified` in a field name. Each expected instruction/edge/data/fixup must have a specific `EvidenceId` grounded in preexisting evidence independent of DOSDis. See `KnownAnswerValidation.cs`. Manifest files and public logs must not embed or publish proprietary original game bytes.

## 5. Required approval criteria

A validation gate can be labeled **INDEPENDENT VERIFIED** for a stated scope only when:

1. The **exact candidate source SHA** has successfully compiled and its relevant synthetic regressions have executed, under explicit owner authorization. The report cannot borrow a previous run's SHA.
2. The gate's sampled real-binary and/or independently generated examples have **documented outside-oracle expectations** sufficient to test the feature being accepted. For x86 decoding include a second independent decoder; for loader/fixups include a published format basis and original mapped bytes; for code/data/CFG include an independently researched known answer.
3. All mandatory sample expectations have been compared and results include **zero unexplained conflicts and zero unexplained missing expected results**. A documented supported-format bug means **BLOCKED** until corrected and retested. An unsupported or unobserved case must remain explicitly outside the supported claim, not be silently removed from the denominator.
4. Known negative cases show no falsely promoted code/relocations/control targets. Unsupported format assumptions and ambiguous provenance remain unknown or rejected, never silently "validated."
5. The comparison record states its **sample coverage, unbenchmarked count, blind spots and residual runtime uncertainties**. No statement of complete coverage follows from passing a sample.
6. A human reviewer accepts the source/expectation independence and discrepancy resolution. The developer who authored an expected fixture does not alone establish an independent oracle by declaring it correct.

If a gate lacks an available independent source, mark it **INDEPENDENT VALIDATION PENDING/BLOCKED** with the missing item; source development can continue, but correctness acceptance and release cannot.

## 6. Applicability by gate and format

- **G1 (historical):** preserve the previously recorded Windows compile/synthetic status at its exact SHA; do not retroactively advertise it as independent verification.
- **G2 (next):** source-precise Windows compile and synthetic suites, *plus* at least a focused independently checked x86-16 decoding/control-flow/dataflow sample before marking Phase 2's correctness accepted. If external comparison cannot be performed in the authorized G2 session, record build/synthetic success separately and leave independent acceptance PENDING.
- **G3:** original exact-hash samples spanning implemented COM/MZ/overlay variants, independently checked loader/fixup maps and failure controls; unsupported overlay behavior stays unverified.
- **G4:** independently checked LE/x86-32 decoding windows, fixups, guarded tables, code/data boundaries and known-answer diffs from **Strahd, Stone Prophet and Menzoberranzan**, with separate identities; a game's unavailable sample is disclosed rather than substituted.
- **G5/G6:** each newly claimed executable/resource/VM family requires its own independently sourced format/behavior oracle and explicit malformed/unsupported controls before that family is considered supported.
- **G7:** UI/persistence acceptance also requires exact source-to-export/reopen fidelity, not merely attractive rendering; no unverified candidate may be silently relabeled verified.
- **G8 (release):** fail closed unless all **advertised supported families** have passing documented independent comparisons at the exact release commit/build, no unresolved mandatory contradictions, a reproducible evidence ledger, and explicit support limitations. Owner acceptance is separate and still required.

This is a **stage-gated standard**, not permission to execute unattended comparisons. All original-game testing is opt-in and conducted without sending proprietary binaries to hosted CI or public source.

## 7. Evidence record workflow

1. Choose independent oracle(s) and scope **before** examining a new DOSDis export. Preserve pre-existing originals/research revisions; log any later oracle corrections with a separate revision.
2. Hash the authorized original file locally, record build SHA and exact CLI/GUI command options, and obtain reference decode/maps separately.
3. Run the owner-authorized comparison with `KnownAnswerValidator` where the schema covers the evidence; for other units use a clearly specified external comparison procedure. Keep unknown/unsupported results explicitly visible.
4. Produce a record using [the validation record template](INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md). Retain command recipes, raw count diffs, oracle tool versions, reviewer and discrepancy dispositions. Do not commit the original binaries or automatically upload them.
5. Mark the gate's **independent** evidence state separately from COMPILED, SYNTHETIC VERIFIED, ORIGINAL VERIFIED and GUI VERIFIED in the roadmap. If any required evidence is missing or contradictory, mark PENDING/BLOCKED and retain the unresolved issue.

## 8. Baseline on adoption

As of adoption (2026-10-10), last compiled Windows G1 run #35 tested SHA `9f605e36581b2c6db72a6a47248ea5af31611506`. Many subsequent changes are **source-only**. Prior source-backed Strahd decompilation audits and original-file smoke runs are useful starting evidence, **not** proof that the current DOSDis HEAD satisfies this policy. **No G2 or G3/G4 independent comparison is claimed by adopting this document.**

## 9. First source implementation checkpoint (2026-10-10)

The existing LE `KnownAnswerValidator` has been hardened to check each
**observed** claimed instruction against three distinct internal facts:
the externally supplied reference bytes, the opened immutable LE
object's original file-backed byte window, and the decoded graph row.
Instruction length must also match. A reference may optionally carry
`ExpectedFlowControl` as an exact canonical named Iced flow category
(e.g., `Call` or `ConditionalBranch`); this tests category agreement
without requiring assembler-specific formatted text. It does **not**
check the complete instruction semantics, operand values or runtime
reachability, and the reference category is only as independent as
the evidence that produced it.

Machine-readable `KnownAnswerFinding.DetailCode` values distinguish
unbacked source bytes, reference/source disagreement, decoded length,
decoded original-row bytes, and flow-class mismatch. Duplicate
expected instruction starts, duplicate directed edge/fixup
expectations, overlapping non-code spans and source-expectation
contradictions are rejected instead of inflating the match rate.
These additions are designed to preserve comparison integrity;
the comparator **cannot itself establish that a human-authored
reference is independent**.

The predeclared [G2 comparison matrix](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md)
lays out external x86-16 decoder/ISA/original-file obligations
before next validation execution. The current known-answer mechanism
is LE-object-relative only and is **not** a 16-bit external-decoder
runner or a substitute for G2's outside-oracle checks.

This checkpoint is **SOURCE ONLY**: reference-control regressions
have been committed, not compiled or executed, and no independent
third-party comparison or human discrepancy review was performed.
G2 independent acceptance remains **PENDING**, and G8's
independent release requirement remains BLOCKING.
