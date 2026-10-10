# Source-bound x86-16 recursive control-flow candidates

**Source-only; not compiled or executed.** This is one shared analyzer for
supported DOS MZ, explicitly mapped COM and hash-bound overlay code regions.
No game-specific byte patterns or known-function answers enter the analyzer.

`Dos16CodeRegion` contains the complete immutable original-file SHA-256,
file-backed start/length, declared 16-bit load segment/origin, and provenance
of that mapping. `DosMzImage.CreateEntryCodeRegion()`,
`DosComImage.CreateCodeRegion()`, and
`DosOverlayImage.CreateCodeRegion(manifest,id)` construct these safely from
their own loader assumptions; protected overlay data is excluded.

`Dos16ControlFlowAnalysis.Analyze(region,entry,maxInstructions)` recursively
walks only candidate near control transfers within one mapped segment.
It records original file offsets, decodes at most the requested budget,
preserves unknown bytes, rejects overlapping decoded instruction ownership,
and emits explicit unresolved/out-of-region diagnostics for far, indirect,
and DOS interrupt transfers. Candidate post-CALL and post-interrupt
continuations are marked as hypotheses. No x86 runtime selector is invented,
no 16-bit wraparound is silently accepted, and no overlay fixup blob is
decoded as instructions. Disassembly remains `candidate-unverified`.

Limitations: no instruction symbolic execution, 8086 segment alias analysis,
far-call target reconstruction, DOS interrupt emulation, 16-bit register flow,
cross-segment CFG, overlay fixup interpretation or full function boundaries.
This is a foundation for generalized later passes, not proof of executable
coverage. Synthetic positive and adversarial smoke assertions are present
but not run. Release builds are manual-only by user request.

## CLI surfaces

`mz-analyze EXE` starts from the validated initial MZ CS:IP.
`com-analyze COM --origin 100` uses an explicit analyst-assumed COM
load origin. `overlay-analyze OVR --regions manifest.json --region ID`
requires a separately source-authenticated, non-data code-region map.
The optional `--entry` is a hexadecimal 16-bit IP within that region,
and `--max-instructions` is a bounded candidate instruction budget.
Results carry original source SHA-256, mapping evidence, candidate bytes,
unclassified file-backed bytes, unresolved flow edges and diagnostics.

The overlay region map's assumed `segment` is a *model parameter* and
does not establish the original overlay manager's runtime selector.

## Far immediate branch review

The MZ source now has a separate `DosMzRebasedView` and
`DosMzFarTargetRecovery`, enabled only with an explicit load-segment
assumption. An unprefixed `9A` or `EA` far immediate pointer is eligible
for annotation only when the selector operand has one exact MZ loader
relocation and the resulting runtime selector maps without alias wrap into
the original file-backed MZ image. The original unresolved graph edge remains.

`DosMzCrossSegmentAnalysis` can follow those **candidate** links into
separately decoded segment windows while retaining competing byte ownership
evidence. This is neither general far-pointer symbolic execution nor
a complete DOS memory model. No source-specific addresses are embedded.

## Opt-in CS indirect-pointer and indexed-table review

When analyzing a mapped 16-bit segment, `--indirect` can add separate
candidate evidence and per-site unresolved statuses for exact explicit-CS
memory-pointer transfers. `--table-map` adds a hash-bound, analyst-sized
CS-indexed dispatch table review. Neither review modifies recursive
reachability. Near pointers must remain file-backed within the declared
segment; far pointers must have unique MZ selector fixups and an explicitly
assumed load segment. See [indirect recovery](P0_X86_16_INDIRECT_RECOVERY.md).
