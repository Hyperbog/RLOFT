# R2.6 — deterministic automated feedback and pattern-family regression

**Status:** source-only; smoke suite registered but NOT compiled or executed
since the accepted Windows G1 build #35. The most recent independently
verified Windows workflow is [run 37987465368](https://github.com/Hyperbog/RLOFT/actions/runs/37987465368)
at source SHA `9f605e36581b2c6db72a6a47248ea5af31611506`.

## Mechanism

`tests/DosDis.Smoke/Dos16MetamorphicFeedbackSmoke.cs` is now an additional
bounded and deterministic test stage in `DosDis.Smoke`. It runs **only**
when the owner explicitly authorizes running the manual local smoke
suite or `workflow_dispatch` Windows build. This is not an autonomous
background job, scheduled CI trigger, source-generating agent, or
internet-dependent service.

The stage uses a fixed `0x16D05D15` Xorshift32 seed for
repeatable generated inputs. When an assertion fails, the exception
includes the pattern family, original seed, case number, inputs or
encoding, expected property and observed value. The failing family
can therefore be reproduced and reduced to an isolated synthetic
fixture before changing the decoder.

| Family | Cases | Independent oracle and safety property |
| --- | ---: | --- |
| Finite modular shifts | 192 | Compare abstract finite-set results to independent native 16-bit shift arithmetic on each input |
| Interval shifts | 192 | Enumerate every concrete short-range input; verify containment, plus exact hull when no modulo boundary is crossed and Unknown when one is crossed |
| Guarded table encodings | 24 | Four byte-exact x86-16 stride encodings crossed with BX values 0–5; a separately backed CMP BX,3/JA table can select only valid indices 0–3 |
| Inventory permutations | 24 | Reverse stored instruction order, reorder outgoing-edge groups, and reverse each group's edge records; graph validation must compare exact edge pairs independently of row order |
| Coverage mutations | 24 | Forge one decoded-byte total; correlation must be incomplete and publish no selected pointers |
| Exhaustive node/edge mutation | Inventory-dependent | Tamper with each original decoded row and each outgoing edge in a small known-good graph, one at a time; all mutations must fail closed |

## Independent lattice and ALU feedback

`Dos16AlgebraicFeedbackSmoke` adds a second, independently seeded
`0xA16E2026` feedback family of **256 bounded domain pairs**. It
combines constants, small finite sets, and short unsigned intervals,
including boundary-focused samples such as `0x0000`, `0x7FFF`,
`0x8000`, and `0xFFFF`.

For each pair, concrete enumeration checks that abstract `Join` and
`Widen` contain every possible source value; modular `Add` and
`Subtract` contain every independent 16-bit arithmetic outcome; and
unsigned `RestrictUnsigned` retains exactly the values satisfying
the supplied inclusive bounds. A separate directed case requires
exact finite-set behavior across word carry and borrow. The suite
prints its seed, domain pair count, concrete arithmetic comparisons,
and unsigned restriction count.

This checks abstract-domain invariants through independent concrete
enumeration, without making the abstract `Covers` implementation its
own test oracle. Both generators are fixed-seed and bounded, not
statistical correctness claims. The full binary space is not exhaustively
covered.

## Graph edge serialization invariant

The dataflow preflight now independently derives the exact
`(kind,destination)` multiset per decoded instruction and requires
**one** match for each expected pair, with exact total edge counts.
Outgoing edge list order is intentionally ignored; missing, duplicate,
or forged pairs are still rejected and report no states. The worklist
continues using the independently decoded successor ordering.

Both feedback suites reverse instruction records and outgoing branch
edge order as metamorphic inputs; dedicated negative smoke cases
attempt to duplicate a conditional edge while losing the other.
This is a serializer-independence property, not permission to change
actual machine-target bytes or edge semantics.

The patterns are synthetic and deliberately small; they make source-
identity and decision invariants measurable without exposing original
D&D executable bytes. They **do not** prove CPU runtime state,
selector values, the immutability of writable memory, or that a
candidate jump target actually executed.

## Human-supervised agentic feedback procedure

1. **Observe** a specifically authorized build/test run or a reproduced
   offline original-file analysis. Record its **exact tested SHA**, run
   ID, completed steps and relevant failure output. Do not conflate a
   successful old SHA with later source commits.
2. **Cluster** the failure by family:
   `finite-shift`, `interval-soundness`, `interval-precision`,
   `interval-wrap`, `guarded-pattern`, `guarded-index`,
   `source-inventory-permutation`, `coverage-invalidation`,
   `original-byte-mutation`, `control-edge-mutation`, compiler,
   persistence, publication or Windows GUI.
3. **Reproduce** the reported fixed seed, case number and exact
   synthetic encoding; minimize the contradiction by removing
   instructions or candidate values. Never repair by hardcoding a
   game/title-specific behavior or by silently promoting unknown.
4. **Repair** the smallest independently substantiated logic error;
   add an exact non-generated regression to preserve the minimized
   counterexample. Re-run **only after authorization**. If CI cannot
   be dispatched from the connected tool, ask the owner to invoke
   the manual workflow instead of pretending a build ran.
5. **Compare** against the previous verified baseline (tests that
   passed, failures, new candidates and intentionally rejected cases).
   A change in count is not by itself proof of correctness.
6. **Commit** code, minimized regression, review findings, and checkpoint
   documentation. Verify remote `disassembler-foundation` HEAD SHA.
   Preserve evidence state: source-only / compiled / synthetic /
   original-file / owner GUI acceptance separately.

## Safety and resource caps

The feedback module contains no new original binaries, writes no
source data or project databases, performs no network calls and adds
no automatic GitHub trigger. Tests use a 0x50-byte synthetic COM
window and short bounded iteration counts. Any case that cannot be
independently validated must fail or report unknown, not authorize a
new CFG edge or function seed.

Existing workflow `.github/workflows/disassembler-core.yml` remains
`workflow_dispatch` only. On a future owner-authorized G2 execution,
the final smoke log must include the deterministic feedback PASS line
with family counts, alongside the earlier R2 regressions, or G2 fails.

**Open verification:** this new generated test source and the post-G1
R2.4–R2.6 core changes remain uncompiled and unexecuted until a
newer Windows run at their exact SHA is inspected.
