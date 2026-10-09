# R2.3 — bounded x86-16 register worklist, source-only checkpoint

**Branch:** `disassembler-foundation` | **Status:** SOURCE COMPLETE,
STATIC REVIEW ONLY. **No Windows build, smoke execution, hosted CI,
original-game regression or GUI interaction has been run.**
G1 is READY / NOT AUTHORIZED. G2 remains NOT READY.

## Implemented

`Dos16DataflowAnalysis.Analyze(region, graph)` is an opt-in read-only
analysis pass over **candidate** 16-bit instructions already produced by
`Dos16ControlFlowAnalysis`. It neither discovers nor promotes new
functions, code bytes, dynamic targets, branch truth, table entries or
runtime registers. It publishes immutable before/after register snapshots
only when its entire observed candidate graph has passed validation and
the bounded worklist completes.

Source and graph preflight verifies:

- full original SHA-256, mapped physical extent, segment, graph entry,
  mapping evidence and candidate-only status;
- every unique instruction is within original file-backed bytes and
  independently decodes to the cached length, opcode and control-flow
  classification, with no overlap or duplicate starts;
- every recorded CFG edge matches the independently decoded instruction:
  exact direct CALL/JMP and both conditional branches, straight-line
  successor, speculative call/interrupt continuation, and unresolved or
  out-of-region conditions;
- no missing, duplicated, added, spoofed or orphan CFG edge, and no
  mapped destination whose decoded instruction is absent;
- null/malformed snapshots are rejected before dereferencing.

Any failure returns an **incomplete/rejected report with zero state
snapshots**. This protects against forged predecessor counts, stale
cache rows and accidental acceptance of incomplete original bytes.

## Abstract interpretation

- Every entry register starts Unknown, with no inferred DOS startup
  constants, flags, initial stack, segment or memory state.
- The worklist processes independently checked outgoing edges and
  merges 8 general-purpose word registers using the R2.1 conservative
  unsigned abstract-value union.
- After repeated expanding merges at a node, per-register widening to
  Unknown forces convergence without asserting the number of loop
  iterations or the absence of runtime paths.
- Explicit budgets: at most 20,000 retained candidate instructions,
  80,000 edges and 100,000 processed worklist steps by default.
  Bounds are independently validated and caller-adjustable within a
  capped range. An exceeded budget yields no published value states.
- Exact, prefix-free short/near16 conditional branches and relative JMP
  preserve general registers and flags (but do **not** decide branch
  direction). Calls, interrupt returns, indirect control flow, byte
  aliases, unknown operations and memory-affecting code retain the
  existing conservative kill-all transfer rule.
- An unresolved dynamic/out-of-region transfer is counted explicitly;
  candidate states of visited source nodes are not runtime-certainty
  claims. An in-region successor absent from the CFG invalidates the
  entire snapshot rather than silently deleting a predecessor.

Only an exact byte-backed direct Jcc/JMP can retain values across an
edge. Matching assembler display text or an analyst-provided edge does
not suffice. There is **no** flag-based narrowing yet, and incoming
candidate paths are not declared feasible or infeasible.

## Source-only synthetic regression cases

`Dos16DataflowSmoke.Run()` is registered with the existing
`DosDis.Smoke` suite. New unencumbered COM byte fixtures check:

1. Two unknown-condition paths assign different AX values before
   converging at MOV BX,AX. Both values must survive the join.
2. A backward ADD/JMP loop expands monotonically and widens AX to
   Unknown rather than making a fabricated finite iteration claim.
3. Step, graph-size and edge-count budget failures, as well as an
   externally truncated CFG, produce **empty** state inventories.
4. Removed, redirected and forged edges, stale instruction bytes,
   duplicate starts, missing decoded destinations and null graph rows
   are rejected without promoting the surviving path to certainty.
5. A different exact original source hash cannot borrow these values.

All test assertions are **written, not run**. The previous successfully
built Windows preview predates R2.1–R2.3.

## Still required

The next source batch should keep register, flag and memory-state
abstractions separated; add negative tests for joins across speculative
call/interrupt continuations, exact direct-branch prefix boundaries,
max-IP wrap and stale original graph metadata. Then R2.4 can consume
*proven bounded* indices only after independent source-byte and
table-storage validation, preserving the original unresolved CFG edge.
Do not attach speculative register values to verified code or add game
specific constants.

Owner interaction: **none yet**. Request one consolidated Windows build
and synthetic regression gate only when explicitly authorized; original
file-type or GUI checks will be requested separately if needed.
