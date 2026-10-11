# DOSDis continuous-improvement playbook

**Status:** operative project workflow, 2026-10-10. Applies to the
`disassembler-foundation` workstream. It is a repeatable research /
engineering procedure, **not** model retraining, an accuracy oracle,
permission to run CI, or an alternative to the normative validation policy.

**Order of authority:** owner instructions and branch ownership; the
[roadmap/build gate policy](ROADMAP_BUILD_GATES.txt); the
[independent validation policy](INDEPENDENT_VALIDATION_POLICY.md);
the current named gate's predeclared scope; then this efficiency playbook.
A process improvement can never lower acceptance requirements.

## Operating loop: every development continuation

1. **Read only the shortest authoritative handoff.** Check
   [CURRENT_CHECKPOINT.md](CURRENT_CHECKPOINT.md), canonical branch HEAD,
   the relevant named gate and any current source-file contract. Consult
   full historical work logs only when a conflict or provenance question
   requires them. Do not assume a previous chat's HEAD remains current.
2. **Frame a falsifiable next improvement.** State the observed gap,
   why it matters, the minimal feature/test that would reveal it and
   what would count as a failure. Prefer work that eliminates a
   correctness uncertainty, prevents a repeated defect, or advances a
   previously unsampled format/behavior.
3. **Gather evidence before code.** Search the authenticated original
   `DnD-Classics/MASTER` tree and its `SOURCE_INVENTORY.csv`; first
   verify title-qualified path, exact size, SHA256 and file-backed
   region. Reuse already available authorized originals; request
   uploads only when a specific source is inaccessible or unverifiable.
   Avoid treating same-named files or different whole-file hashes as
   independent instruction samples.
4. **Use two genuinely different reasoning paths.** For decoding,
   prepare externally decoded original-byte expectations before
   examining the new DOSDis output. For loaders, independently derive
   header/fixup arithmetic from original bytes and a published format
   basis. Record sampled/unsupported/not-observed categories. Neither
   DOSDis's own output nor its embedded Iced decoder supplies an
   independent ground truth.
5. **Change a bounded batch.** Prefer a coherent set of core change,
   positive/negative synthetic fixtures, and updated evidence record.
   When possible, use small typed helpers and pre-existing contracts
   instead of rewriting a large component. Reject ambiguous, truncated,
   forged and unsupported cases explicitly.
6. **Perform STATIC review without a build.** Check source-offset/CS:IP/
   LE-object namespace, integer overflow, file-bound reads, null and
   duplicate records, result status labels, negative fixtures and
   non-overwriting output. Examine changed-file list and preserve
   `workflow_dispatch`-only canonical CI. Never label static review
   `COMPILED` or `PASSED`.
7. **Execute only at an explicitly authorized gate.** Batch builds,
   synthetic tests and original-file comparisons. For expensive
   Windows-only builds, use an approved isolated temporary PR if
   needed; keep originals and external proprietary manifest byte
   windows out of hosted CI. Record the exact runner merge SHA,
   equivalent canonical code SHA, artifact ID/hash, platform and
   authorization. Close/reset temporary verification branches after
   results; don't merge temporary CI triggers.
8. **Adversarially challenge each success.** Deliberately contradict
   source SHA, offsets, flow/operand facts, edges and malformed input
   fields; preserve the expected conflict/missing/not-observed or
   fail-closed status. A passing happy-path comparison is not enough.
   Different original containers carrying duplicate code are ONE
   sampled code behavior, not extra accuracy coverage.
9. **Record one short hindsight lesson.** Include the precise defect
   or bottleneck, evidence that exposed it, the new preventive
   regression or procedural change, and the still-pending tests.
   Update the compact handoff and relevant gate document; avoid
   duplicating entire historical logs in each summary.
10. **Choose next work by information gain.** Prefer untouched
    features and genuinely distinct source bytes, known uncertain
    loader/fixup semantics, high-risk correctness bugs, or missing
    independent evidence. Defer cosmetic refactors and repeated
    identical source comparisons unless they serve a specific
    portability or provenance control.

## Proven lessons and resulting changes

| Observed event | Durable preventive action | Present evidence status |
| --- | --- | --- |
| CI attempts #36/#37 uncovered a C# `Decoder` name collision and a shadowed fixture variable before Windows run #38 passed | Review qualified symbol resolution, local variable scope and synthetic-project build at each approved compiled gate; preserve failed run IDs in the record | Both issues fixed and compiled/synthetic checked by #38 |
| The early external-oracle JSON reader accepted duplicate/unknown keys, potentially dropping assertions | Reject duplicate/case-colliding and unknown fields, including nested operand/memory objects; add malformed negative fixtures | Compiled/synthetic checked by #40 |
| A malformed source/oracle error initially aborted the Linux CLI instead of returning a controlled diagnostic | Catch expected `InvalidDataException` at CLI boundary; test exit status, no output report and absence of process abort | Original-source negative controls and cross-platform tests checked by #40 |
| A successful 11-instruction Strahd entry sample did not cover branches | Add predeclared independent branch/call sample; report each instruction/operand/edge denominator separately | 28/28, 15/15, 5/5 narrowly original-compared at #40; G2-wide pending |
| Strahd `CODE.1` and `CODE.2` have different file hashes but identical 16-bit outer launchers | Compare source-mapped region and explicitly benchmarked instruction byte identities; don't double-count samples | Newly implemented reporting/fixtures are SOURCE ONLY; uncompiled |
| Pool of Radiance and Curse of the Azure Bonds have identical first 283 *bytes* at separate mapped entries | Predeclare a shared-byte correlation control, not 283 instructions or two independent decoders; choose distinct original code for coverage | External source SHA and header audit only; new original DOSDis comparison pending |
| Repo has a known inventory-integrity backlog, but selected originals may still be exact | Compare candidate-specific `SOURCE_INVENTORY.csv` SHA+size first; don't demand large archive reuploads for already verified inputs | Four original DOS executables individually source-verified; broader collection audit remains separate |
| Independent Python preflight checked source SHA only in its CLI file reader; direct `describe(data, claimed_sha, ...)` could have labeled unchecked bytes as verified | Validate identity at **each public evidence boundary**, not only the outer CLI; add direct-helper wrong-SHA regression | SHA fail-closed source change and synthetic cases authored; **not executed** |
| Only one DOSDis relocation implementation previously provided relocated words, making original MZ comparison less reproducible | Maintain a separately implemented Python loader-word-addition model; compare only source-bound copied-image SHA/count/overlap at the **same assumed load segment** | Separate Python/C# SHA interfaces and adversarial source-only fixtures added; no original comparison or build since #40 |
| Fixed literal SHA256 expectations in newly authored fixtures were opaque and could create avoidable first-build failures | Construct independently specified expected byte images and serialize exact IP/length/original-byte samples **inside synthetic fixtures**, then hash those buffers; never copy a digest from DOSDis candidate output | C# and independent Python source-only 480-/464-byte reference cases authored, not executed |
| Cross-implementation digest equality alone could hide different MZ extents or assumptions | Reconcile source SHA, assumed load segment, stored image length, relocation count, overlap and digest; reject ambiguous or self-contradictory statuses and JSON | Source-only strict five-field comparator and negative-control fixtures authored; no original-file execution |
| G2 build #41 compiled on both platforms but failed the x86-16 oracle digest fixture: changing one oracle instruction list from two windows to one also changed the true sampled-original window | Treat sampled membership/order/IP/length as part of benchmark identity; adversarially mutate oracle claim **without silently changing the sampled denominator**; add a negative fixture retaining both original windows | Fixed at `3956f036`, full cross-platform C#/Python synthetic suite **PASSED** at owner-authorized #42 |
| Windows desktop #42 opened a valid MZ `CHECKSYS.EXE` only as raw hex, disabling Analyze/Export, even though .NET core MZ/x86-16 and CLI synthetic tests passed | **Test user-facing capability wiring, not just core feature implementations**. Give the desktop an explicit supported-format route, typed x86-16 navigation, file-backed preview/graph/export and a post-build owner GUI checklist; avoid leaking LE object-address semantics into MZ | WPF MZ integration, source-bound export and synthetic core tests authored, **UNBUILT**; GUI acceptance pending |
| Original CHECKSYS MZ JSON showed working analysis/export, but did not verify visual WPF formatting or navigation | Treat **real owner GUI screenshot** as a distinct acceptance class; cross-check displayed file identity, headers and status against private source-bound JSON, but leave actual Go/References/Hex interactions and LE regressions unverified until exercised | Original screenshot confirms MZ entry, 43 fixups, x86-16 listing and 336/394/28 result; clipped Explorer/Evidence/Intelligence values and inactive LE-only panels captured for later UX work |
| CHECKSYS owner's first navigation test found disassembly double-click inert, Hex Go visually stationary and References scoped to only the initially selected instruction | Add explicit WPF interaction handlers and visible position confirmation; keep source-backed control-flow target selection independent of assembly text; provide a **default complete list** with optional contextual filter. Reposition Hex even when selecting the already selected row | Direct-target helper with negative controls, Hex original-file readout/reset, global references and selected-only filter SOURCE-AUTHORED; **not yet built or GUI verified** |
| Owner confirmed #44 navigation works, but raw TextBox hex still lacked per-byte click selection and Enter-to-search | Keep original-file mapping explicit; use a separately tested pure hex-rendered-text mapper that rejects ASCII, whitespace and LE unbacked `??` cells; preserve read-only drag-copy and the existing MZ/LE navigation controls | New `HexByteNavigation`, synthetic token/pattern/wrap cases, Enter Go/Find, click selection and source-bound cross-tab row/byte highlights SOURCE-AUTHORED, **not compiled yet** |
| Dynamic byte highlights on WPF record rows would change compiler-generated record equality while the DataGrid was tracking selected objects | Use stable reference-identity UI row classes with `INotifyPropertyChanged` for mutable highlights; never mutate value equality used for selection/key lookups | `AssemblyRow` refactored to a class; same-row byte highlight updates bind without replacing/restamping candidate graph rows; WPF behavior awaits Windows build |

| User requires the Windows desktop as sole distributed DOSDis application; CLI previews add unnecessary products and feedback loops | Publish only Windows WPF desktop, retain CLI only internally for independent research or validation, and keep the canonical workflow manual-only | Local build script and Windows CI publishing revised to desktop-only, **not yet executed** |

These are **project engineering lessons**, not claims that the assistant
has modified its underlying model training. Their value comes from
being reproducible and read again on future turns.

## Efficiency without weakening proof

- **Batch independent reads**, cache source/branch/commit identifiers in
  the work context, and only re-fetch mutated files. **Serialize
  writes to the same file** against its current blob SHA; re-read
  before retrying a failed compare-and-swap.
- **Keep a small active surface**: compact checkpoint and exact gate
  record first; 90K+ historical roadmap only for necessary details.
  Work on the highest-risk source contract before adding broad text.
- **Select originals strategically**: COM vs MZ, relocation-bearing vs
  relocation-free, shared startup vs genuinely distinct instruction
  streams, 16-bit vs 32-bit, expected-valid vs expected-unsupported.
- **Prefer reuse over reimplementation** when it doesn't compromise
  independent-oracle separation: use the core parser for DOSDis, but
  another implementation/format standard for the expected behavior.
- **Create the regression at discovery time.** Every significant
  defect should yield a minimal synthetic case that fails under
  the old behavior and passes under the corrected behavior when
  next authorized. Source-only fixtures are not test results.
- **Make uncertainty visible**. Report exact counts, sampled
  denominator, unbenchmarked candidates, source hashes, build SHA,
  explicit missing controls and human-review state. Never produce
  an unmeasured global completion percentage.
- **Avoid adding tools for appearance or novelty.** Adopt a script,
  library or framework only if it gives a concrete correctness,
  reproducibility, speed or maintenance advantage; reject additional
  mechanisms that risk stale parallel truth sources.

## Next-cycle process measurement

When a build gate actually executes, optionally record:
`source_commits`, `authorized_runs`, `compile_fixes_after_first_run`,
`new_independently_sampled_features`, `source_identified_samples`,
`reused_sample_byte_windows`, `unexplained_discrepancies`,
`new_negative_controls` and `gate_status`. Use recorded evidence,
not estimated productivity improvements. Objective is **less repeated
work and fewer escaped defects**, not maximum commit count or a
misleading apparent pass rate.

## Non-negotiable boundaries

- `Continue` and a request for increased efficiency do **not**
  authorize builds, smoke tests, CI, original-game executable testing,
  publication, or editing originals. Request explicit owner consent
  per normative gate.
- No game bytes, raw proprietary manifests, source paths, or original
  executable archives in Git/CI/published artifacts.
- Cross-workstream references are read-only until the correct
  owner/branch update is explicitly authorized.
- Never claim private model retraining, self-modifying model weights,
  background autonomous work, or independent human reviewer approval.
