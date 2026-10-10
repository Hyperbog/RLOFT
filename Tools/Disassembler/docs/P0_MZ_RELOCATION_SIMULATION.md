# P0 DOS MZ relocation simulation and far-pointer evidence

**Implementation status:** committed source, synthetic assertions added,
Windows compilation and tests **not executed**.

## Non-destructive loader simulation

`DosMzImage.SimulateRelocations(assumedLoadSegment)` creates a private copy
of the original DOS MZ **file-backed load image**. For every MZ relocation
entry in table order, it adds the explicitly assumed load segment to the
stored 16-bit word modulo `0x10000`. The original bytes are immutable.
The resulting `DosMzRebaseSummary` records source SHA-256, original size,
initial relocated CS/SS, entry IP/SP, each physical file location, original
word, value before the write, value after the write and whether fixup write
locations overlap. Duplicate entries are not silently deduplicated.

No PSP, BSS, DOS process relocation environment, overlay state, A20 line or
real runtime loader base is claimed. Segment wraps cannot be projected back
into known original file offsets without further evidence.

## Immediate FAR pointer candidates

`DosMzFarTargetRecovery.Inspect(image, graph, simulation)` observes
already-decoded candidate instructions. It only accepts unprefixed
`CALL FAR ptr16:16` (`9A`) and `JMP FAR ptr16:16` (`EA`) with an
exact **single MZ relocation** on the selector word, no other touching
instruction relocation, matching original instruction bytes, and a
nonwrapped runtime-selector/file-backed mapping. These candidate targets
are returned separately rather than marked as verified edges in the
original graph.

Relocation location lookup is indexed and bounded, not a repeated full
relocation scan for every far candidate.

## Cross-segment review

`DosMzCrossSegmentAnalysis` starts at MZ header CS:IP, runs bounded
candidate graph traversals within a module-relative segment window, then
enqueues only the exact-fixup far candidates. Each graph retains its own
original-source physical file offsets. Aliased decoded bytes are counted
once; differing instruction interpretations at the same stored byte are
reported separately as conflicts, with neither automatically selected as
verified. Entry graph and per-graph instruction budgets are explicit.

This is generalized DOS MZ mechanics, not game-specific code or a
claim of faithful runtime reconstruction. All results retain
`candidate-unverified` evidence statuses. The overlay fixup format remains
opaque. Synthetic fixtures cover one relocation, modular wrap, duplicate
relocation writes, valid/invalid FAR selector evidence, bounded traversal,
truncation, and conflicting physical byte interpretation; not yet run.
