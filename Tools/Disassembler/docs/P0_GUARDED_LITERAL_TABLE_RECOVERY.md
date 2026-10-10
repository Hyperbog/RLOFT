# P0 control-flow milestone: unsigned-guarded literal jump tables

Status: source implementation, **not compiled or executed**.

The previous `ReferenceRecovery` implementation required at least two
contiguous LE internal relocation records for an indexed jump table. Some
32-bit LE binaries contain literal linear dword pointers without per-slot
fixups; these cases were always left unresolved.

The additional deliberately narrow candidate rule recognizes only:

1. `CMP register, unsigned constant N` with `1 <= N < 128`
2. Immediately following `JA` branching away from the fallthrough
3. Immediately following `JMP dword ptr [same-index-register*4 + constant]`
4. A uniquely mapped, backed LE table base; all `N+1` literal dwords map
   unambiguously into stored executable objects under preferred bases
5. No fixups at the literal slots, at least two distinct candidate targets

All conditions must hold. Neither an incidental literal nor just a bounds
check promotes bytes to verified executable code. The jump-table result,
data cross-reference and candidate indirect flow edges retain
`candidate-guarded-literal-table;unsigned-ja-bound;no-relocation-proof`
while the existing `unresolved-indirect-jump` edge is retained.

Synthetic regression source covers seven valid candidates, failure with a
non-JA guard, one invalid pointer, and a guessed object offset. The exact
reported Menzo object/address `0x008D` remains **unverified**, as the relevant
original executable/report was not available in this conversation's files.
Other indirect forms (register loads then jumps, signed bounds, switch index
normalization, noncontiguous tables, 16-bit addressing) remain unresolved.

No Windows build or CI invocation was made. Game-specific source identities
and annotation namespace work are separate P0 gates.
