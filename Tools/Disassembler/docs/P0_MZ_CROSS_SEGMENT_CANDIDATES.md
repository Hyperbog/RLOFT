# MZ cross-segment candidate traversal — source checkpoint

`DosMzCrossSegmentAnalysis` uses a caller-selected DOS MZ load segment,
an immutable file-backed MZ load module, and its relocation table.

It begins at the executable's declared initial CS:IP. The shared x86-16
analyzer traces near branches within a single segment. The recovery pass
may enqueue an additional **relative CS:IP** only for an unprefixed
`CALL FAR ptr16:16` or `JMP FAR ptr16:16` whose stored selector word
has exactly one MZ relocation at the immediate selector offset. The
simulated runtime selector must map without wrap into an actual original
file-backed load-module byte. The original unresolved far edge remains in
the per-entry graph; the additional connection is explicitly a
`source-backed-far-candidate`.

Each entry is analyzed independently and is bounded by a limit on entry
graphs and decoded instructions per entry. Source addresses, byte lengths,
full source hashes and byte-overlap conflicts are retained. Segment aliases
are never silently merged: disagreements across original physical bytes
appear as competing candidate interpretations. A graph limit or an
individual instruction budget marks the review truncated.

This is **not** an emulator and does not model the PSP, actual DOS-assigned
load base, A20 wraparound, post-load code modification, cross-segment
register values, dynamic/far-indirect calls, overlay manager fixup semantics,
or verified function boundaries. The results remain candidate-unverified.
No game names, per-title addresses or source hashes are hardcoded.

## R3.1–R3.2 provenance and alias hardening — source-only

A public FAR immediate target inspection now independently checks the
original MZ relative-CS code region against its input graph, including
mapped physical start/length, source hash, loader mapping evidence,
entry offset, and candidate-analysis label. Every retained 5-byte FAR
CALL/JMP must additionally match its exact source file bytes,
source-backed instruction address and original Iced 16-bit decode.
An unverified or missing unresolved FAR control-flow edge blocks
publication. Source-backed relocation ordinals must match the
original relocation entry's segment, offset, physical source byte
position, original selector word and value immediately before the
simulated write. These checks do not establish runtime execution.

An upstream instruction-budget-truncated graph deliberately yields
**zero FAR candidates** while remaining eligible for a truncated
cross-segment review, rather than causing a new exception. This
retains independent diagnostic visibility without implicitly
traversing a partially supported path.

`DosMzCrossSegmentReview.PhysicalAliases` is a new additive ledger
separate from the existing `PhysicalConflicts` list. Identical,
independently decoded original instruction spans reached via distinct
relative `CS:IP` addresses are recorded as **aliases**, capped at
4,096 unique source-backed alias instruction keys. Contradictory
instruction boundaries and bytes remain conflicting interpretations,
never verified code.

The synthetic regression constructs a single resident MZ load module
with a near call reaching relative `0000:0010` and a relocation-backed
far jump reaching `0001:0000`. Both are the same original file-backed
`RET` at original offset `0x30`. The expected report is two
independent entry graphs, one FAR candidate, one physical alias and
zero conflicting decodes. Forged IP, wrong segment window, invalid
mapping evidence, missing/rewritten unresolved FAR edges, and
truncated analysis all have fail-closed checks.

**Verification:** source and synthetic tests are committed but have
not been compiled or executed since G1 build #35. The alias record
does not prove real-mode runtime equivalence, function ownership,
cross-segment calling convention, A20 behavior, or overlay relocation
semantics. Do not use this source checkpoint to mark G3 accepted.
