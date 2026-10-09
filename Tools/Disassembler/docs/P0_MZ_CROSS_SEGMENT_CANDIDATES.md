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
