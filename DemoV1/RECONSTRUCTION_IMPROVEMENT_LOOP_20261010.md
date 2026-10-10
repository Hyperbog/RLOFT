# Ravenloft original-game reconstruction — continuous improvement loop (2026-10-10)

## Objective and limits

This document makes review and feedback improvements **durable across sessions**. It does not train or alter the assistant's underlying model weights. The original-game reconstruction charter, executable/data evidence, and independent DOS runtime remain authoritative. The goal is the **entire original game**; Level 22 is a reusable testbed, not the final deliverable.

## Fast, strict per-change cycle

1. **Orient once:** obtain current `demo-v1` HEAD, last commit, relevant continuity subsection, existing canonical `Code1.cpp/Code2.cpp` routines, original immutable file hash, existing tests, original disassembly cache. Avoid repeating completed full-object disassembly.
2. **State a falsifiable claim:** identify original routine, entry and output, mutable state, call-order/timing/callback dependencies, scope and unresolved cases. If uncertain, do not connect it to native gameplay.
3. **Verify a bounded source proof:** original instruction bytes or exact data records, relevant call targets and required branches; cite original logical addresses and file identity. Fail closed on wrong version, truncated data, unknown provenance or indirect alias.
4. **Implement a small source-correct change:** prefer isolated source model or read-only diagnostics until original state is proven. Add edge/boundary tests before touching live pixels.
5. **Run focused local checks:** compile or validate the changed test where available; batch the existing source oracles with the runner below. Preserve logs and mark exactly which level of verification actually ran.
6. **Commit atomically:** update code, test and small audit note together. Use expected-HEAD lease; on concurrent commit, rebase/merge intellectually before reattempting. No silent overwrite.
7. **Validate independently:** compile/CTest, on-device F12 diagnostics, and original DOS indexed framebuffer/palette/tick comparison are *different* gates. Full Windows GitHub Actions is manual and user-authorized only.
8. **Learn from the failure:** record the assumption that failed, add one regression/byte oracle, amend the continuity contract, then choose the next highest-value source-correct dependency. Prefer removing blockers over increasing document volume.

## One-command reproducible ground-source audits

The pinned `DemoV1/tools/run_original_ground_proof_batch.py` runs **10 existing** isolated ground-renderer oracles. It validates the original immutable CODE.2 and SINTAB hashes **before** the first test, then reports each result and optional JSON. No copyrighted game binaries or local paths are committed.

Original byte **and** source contracts (preferred on a machine with legally installed originals):

```sh
python DemoV1/tools/run_original_ground_proof_batch.py --original-code2 /local/CODE.2 --original-sintab /local/SINTAB --json-out /local/ground-source-proof.json
```

Native/source-structure contracts **without** original binary proof (appropriate as a source-only CTest):

```sh
python DemoV1/tools/run_original_ground_proof_batch.py --source-only
```

Focused check: `--only upper_callbacks` (repeat `--only` for multiple). `--list` shows pinned names. A focused result prints `coverage=subset`; it is never treated as full suite. Any failed, missing or timed-out checker gives a nonzero exit code. Results are labeled **`ORIGINAL_PASS`** versus **`SOURCE_ONLY_PASS`** so absence of originals can never be disguised as original-game verification. CI registers one new source-only CTest; it does **not** launch a Windows build or certify machine-byte behavior.

The manifest is explicit, not discovered via a permissive filename glob. When adding an original ground source oracle, review and deliberately add it to the catalog; a new script isn't counted until then.

## Mistake-to-regression ledger

| Observed lesson | Permanent guardrail |
| --- | --- |
| Decoded original texture but lower image stayed flat (#657) | Separate resource verification, active-source-ray guard, actual raster indexed writes, and end-user capture |
| Inactive `resource_outer` incorrectly blocked 34A74 | Read source prepass predicate; test active vs inactive materials independently |
| `22220` initial `0x800` did not tell the complete story | Verify all four materializer tails; track explicit `0x93 → 0` depth resets |
| Original `32F4C` sentinel looked like a materializer | Record provenance classes; sentinel writer must not update global 22220 |
| An isolated regression asserted values overwritten earlier in its fixture | Validate test fixtures at assertion time and retain independent original-byte oracles |
| Original full raster but variable `FF50` remains incomplete | Confirm upper-callback side effects, partial plane transition at 80/81, live C026/C027 and VGA ES aliases before drawing |
| RNG seemed disconnected from graphics | Preserve original `314B0 → 30CA2/30CC4 → 30700` RNG/tick ordering |
| Concurrent commits moved branch HEAD | Compare-and-swap update ref; read, reconcile and rerun affected checks |
| Tests registered but latest Windows build older | Report authored/local, Windows compiled, field-tested and DOS-matched statuses separately |
| Repeated ten single-script invocations waste time | Use explicit, SHA-pinned source proof batch with JSON report; do not downgrade original evidence |

## Metrics to report, never guess

Record **exact HEAD/source SHA, verifier selection and pass/fail, CTest registered versus actually run, native exe hash, field capture source/media, original indexed-pixel mismatch counts, still-open source aliases**, and any accepted readiness metric only from the project's source-backed progress register. Zero failed tests is not equivalent to source completeness or DOS equivalence.

Update this document only for a demonstrated new mistake or validated acceleration, not for routine status updates.
