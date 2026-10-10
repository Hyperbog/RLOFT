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
