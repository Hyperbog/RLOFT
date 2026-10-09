# R2.1 — bounded x86-16 unsigned abstract-value domain

**Checkpoint:** 2026-10-09, `disassembler-foundation` branch.
**Evidence status:** SOURCE COMPLETE for isolated value-domain infrastructure.
**No new Windows compilation, synthetic test run, original-binary analysis,
GitHub Actions execution, executable packaging or UI verification occurred.**
G1 remains READY / NOT AUTHORIZED. This checkpoint does not pass G1 or G2.

## Added source contract

`src/DosDis.Core/Dos16AbstractValue.cs` is a title-agnostic, byte-free
value lattice for later conservative 16-bit register analysis:

| Form | Meaning |
| --- | --- |
| `Unreachable` | No path/value represented (bottom) |
| `Unknown` | Any unsigned word in 0..65535 (top) |
| `Constant` | Exactly one unsigned word |
| `FiniteSet` | A sorted, deduplicated, immutable set of at most eight words |
| `UnsignedInterval` | All unsigned words between the inclusive endpoints |

`Join` over-approximates union; two small finite sets remain exact, while
more than eight possible members form the inclusive interval hull.
`Covers` checks set inclusion and `Widen` collapses expansions to Unknown
rather than pretending bounded iteration has proved a stable state.
`RestrictUnsigned` is an **abstract filter only**. A future caller must
independently prove the instruction bytes, unsigned comparison direction,
register value and branch edge before applying a restriction.

`Add`/`Subtract` implement x86-16 modular arithmetic on exact finite
members (maximum 64 combinations). Intervals receive a finite hull only
when all possible results share the same 16-bit carry/borrow side.
Mixed carry/borrow intervals are Unknown, not an invented single interval.
No signed interpretation, carry flag, overflow flag or higher-width alias
semantics are claimed.

## Provenance boundary

`Dos16ValueOrigin.Create` requires a full 64-character source SHA-256,
nonnegative **original file** offset, typed 16-bit address and controlled
origin kind. `Dos16ValueFact` binds a value to one source hash, refuses
cross-hash merges, and caps immutable origin labels at eight with an
explicit `OriginsTruncated` status. Origins are observation **labels**,
not proof that the instruction was reached or that source bytes match
the declared instruction. Future flow integrations must attach origins
only after validating byte ownership, mappings and instruction effects.

This component deliberately does **not** modify the existing
`Dos16ControlFlowAnalysis`, dispatch/table recovery, candidate CFG,
automatic labels, reports, project database or desktop UI. It does not
read any proprietary originals. This isolation prevents unfinished
dataflow rules from silently promoting a jump target to established code.

## Regression fixtures (written, not executed)

`Dos16AbstractValueSmoke.Run` is called from the existing
`DosDis.Smoke` test entry point. Synthetic assertions cover:
- top/bottom, exact sets, stable ordering, copy isolation and safe joins;
- 8-member cap, interval widening and loop expansion;
- exact carry and borrow, mixed modular boundary becoming Unknown;
- empty unsigned filters and constraints on initially unknown values;
- origin cap visibility, deduplication and cross-source rejection.

The full Windows test gate must also compile the desktop, CLI, core,
project smoke suite and all existing x86-16/LE/MZ/COM/SQLite regressions.
Until approved execution, no PASS claims attach to these new fixtures.

## Next Phase 2 checkpoints

1. **R2.2**: source-backed *exactly enumerated* x86-16 transfer rules
   (MOV immediate, safe full-register copy, chosen ADD/SUB and CMP forms).
   On partial registers, aliasing, memory writes, unsupported instructions
   and unknown flag dependencies, kill affected knowledge to Unknown.
2. **R2.3**: predecessor merge and bounded worklist with monotone joins,
   widening, finite processing/memory budgets and explicit truncation.
3. **R2.4**: pass locally proven unsigned index ranges to the existing
   independent table validator; preserve indirect unresolved edges.
4. **R2.5–R2.6**: source identity and simulated segment context isolation,
   then synthetic joined-branch, loop, stale-edge and alias tests.
5. **G2**: Windows compilation/fixtures/original comparisons only when
   explicitly authorized, and only after a coherent Phase 2 batch.

**No change in release-completion percentage is asserted.** The previous
43% figure is a planning estimate, not verified feature or original
game coverage.
