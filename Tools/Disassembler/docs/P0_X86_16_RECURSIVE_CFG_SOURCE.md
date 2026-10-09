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
