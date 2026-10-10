# R2.4 — correlated guarded dispatch and exact finite BX offsets

**Checkpoint:** source committed to `disassembler-foundation`; all
changes in this checkpoint are **SOURCE-ONLY, NOT YET COMPILED OR TESTED**.
The prior G1 Windows Release/synthetic suite succeeded at
`9f605e36581b2c6db72a6a47248ea5af31611506`, GitHub Actions
run [37987465368](https://github.com/Hyperbog/RLOFT/actions/runs/37987465368).
That successful build does **not** verify the newer scaling or correlation
source. The owner also accepted the build #35 desktop preview manually.

## What changed

The unsigned modular lattice now supplies
`Dos16AbstractValue.ShiftLeft(value, count)`. It is a **unary** operator,
not a cartesian addition: finite-set members double themselves, so
`{1,3} << 1` yields `{2,6}` rather than invented combinations.
Wrapped 16-bit finite values stay exact. Interval shifts yield an
unsigned hull only if all possible results remain in one modulo quotient;
otherwise they conservatively become Unknown. Out-of-range shift counts
are rejected.

`Dos16RegisterTransfer.Apply` now models only the following additional
**exact, prefix-free, original-byte-decoded** scaling instructions:

| Bytes | Transfer |
| --- | --- |
| `D1 E3` | `SHL BX,1` |
| `C1 E3 01` | `SHL BX,1` |
| `C1 E3 02` | `SHL BX,2` |
| `01 DB`, `03 DB` | `ADD BX,BX` |

These changes invalidate flags and carry typed source offsets/origins.
No general-purpose shift/memory semantics or flags model is implied.
Unknown remains Unknown and unmodeled prefixes/aliases still kill all
tracked values.

`Dos16CorrelatedDispatchRecovery.Inspect` is a new **read-only,
opt-in** cross-check that runs, in order:

1. The complete, independently original-byte- and edge-validated
   bounded candidate CFG/register worklist; any incomplete graph,
   worklist budget or mismatch produces an empty *incomplete* report.
2. The existing automatic guarded dispatch analysis, which requires a
   uniquely backed unsigned CMP/Jcc control path, exact BX register
   copy (if present), and exact one- or two-stage table stride. More
   than the maximum indirect site count fails closed.
3. An independently revalidated `Dos16TableManifest` built solely
   from accepted **automatic** source-backed guarded table counts.
   This is **not** an external analyst guess and is neither inferred
   from a filename nor accepted only because BX looks small.
4. The R2.4 finite-value table index validator. Only a small exact
   singleton/finite BX **byte offset** set aligned to the near/far
   pointer width and within every source-backed stored entry is
   returned as a candidate selected set.

The result retains per-site guard reasons, optional source-backed
pointer candidates, source SHA/mapping evidence, explicit unresolved
counts and code/data overlap count. There are **no newly decoded
targets, CFG edges, runtime memory values, function seeds, pointer
writes, or modifications of game bytes**. A consistent guard + finite
index remains a static hypothesis, **not** evidence that a runtime
branch is taken or that writable pointer bytes remain unchanged.

## Source-only synthetic regressions

`Dos16CorrelatedDispatchSmoke.Run()` was added to the manual
`DosDis.Smoke` Windows gate, along with lattice and register-transfer
scaling cases. These tests are **written, not run**.

- `MOV BX,1; CMP BX,2; JA reject; SHL BX,1; JMP CS:[BX+table]`
  independently confirms three stored table entries but selects only
  index 1 as a candidate source-backed pointer.
- Source-backed `ADD BX,BX` encodings and `C1 E3 01` use the same
  correlated unary transfer semantics.
- An unsigned guard with unknown BX preserves stored-table discovery
  but yields **no selected index**.
- An out-of-bound candidate BX offset is unresolved even when the
  automatic guard counts three stored entries.
- Truncated and forged-edge CFGs publish **zero** indexed candidates.
- Pointer bytes overlapping decoded candidate instructions are
  reported as code/data conflicts and never selected.
- The value domain tests exact finite correlation, modular wrapping,
  interval hulls and mixed-quotient Unknown; transfer smoke confirms
  the source-backed origin and flag invalidation for all scaling forms.

## Outstanding work

G2 remains open for **new** code: run an owner-authorized Windows
Release build and complete synthetic regression suite before claiming
compilation or tested semantics. Validate negative far-pointer fixup,
two-step FAR scaling, prefix/alias, mixed-source windows, malicious
index bounds, changing runtime pointer contents and graph budget
behavior. Original DOS game executions and GUI/manual checks remain
distinct from synthetic evidence. Do not convert speculative table
entries into executable CFG paths without an independently accepted
runtime/code provenance model.

## R2.5 / R2.6 adversarial safety extension (source-only)

Further synthetic cases exercise both state flow and independent evidence
boundaries, before any G2 build:

- An unsupported `MOV [BX],AL` between the known BX source and the
  compare causes the register transfer layer to invalidate BX. The
  independently source-backed guard and stored table remain detectable,
  but the finite-index cross-check **must not select any entry**.
- A memory write between Jcc and the exact BX scaling operation
  breaks the unique transparent guard-to-stride proof. The output must
  not claim a guarded storage table even if earlier BX was constant.
- The same source hash loaded at a different explicit COM origin, or
  a different source SHA at the same origin, is rejected before joining
  graph/register state.
- A DS-prefixed indirect table cannot masquerade as `CS:[BX+disp16]`
  source storage. Literal displacement alone does not establish the
  segment selector.
- A FAR indexed table with exact `SHL BX,2` but no corresponding
  original MZ selector-fixup and rebased mapping remains unresolved.
  No intersegment target can be manufactured from raw pointer bytes.
- The original indirect-jump edge remains unresolved after a positive
  correlation; repeated analysis does not mutate the graph.

These cases are committed as **unexecuted** Windows smoke fixtures.
They do not validate true CPU flag behavior, volatile memory contents,
runtime segment registers, original game binaries or desktop workflows.
The existing Windows build #35 passed at a *previous SHA* and must not be
represented as testing these additions.

## G2 verification matrix

Run the existing manual-only `.github/workflows/disassembler-core.yml`
on the current branch when separately authorized. Confirm the Windows
Core/CLI/Desktop, DosDis.Smoke and DosDis.ProjectSmoke builds, both
synthetic test suites, and both artifact uploads succeed. Audit output
for the newly added correlated-dispatch PASS line as well as the existing
R2.1–R2.4 PASS lines. Any compiler diagnostic, unhandled synthetic case,
unexpected published target or missing artifact fails the gate.

Only the exact workflow SHA can be marked verified. An artifact compiled
before these additional commits cannot certify the new source.
