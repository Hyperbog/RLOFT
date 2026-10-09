# P0 executable gap ledger and coverage uncertainty

Source checkpoint — Windows build and regression tests **not run**.

`ExecutableGapAnalysis.Analyze(graph, image, minimumGapBytes, maximumGaps)`
requires the exact source SHA-256 and closes executable-object byte accounting
across candidate code, protected data, unknown file-backed data/code and
unbacked virtual address space. It ranks unclassified file-backed spans by
length with stable object-relative addresses and source-file offsets.

For each ranked span, source-linked reference counts identify relocation
sources and destinations, candidate incoming control-flow edges, data
cross-references and analyst seeds. None of these counts are proof of code.
A truncated analysis is explicitly marked. Virtual-only unbacked bytes are
counted separately; gaps are **not** linearly decoded or automatically
reclassified. The `WriteNew` export is local, no overwrite and 8 MiB capped.

The Stone Prophet reported unknown span in object 1 near
`0x0463..0x6133` will be a useful real-world check once the original
Stone binary/export is available. The reported length and endpoints remain
unconfirmed. Synthetic source tests cover byte accounting, exclusion of
protected bytes, SHA-256 mismatch rejection, and export no-overwrite.

## CLI and candidate report integration

CLI analyze and report now compute the source-authenticated gap ledger.
`analyze` exposes unknown-file-backed byte totals and the largest
candidate gaps in JSON output; `report` embeds `gapAnalysis` in its
machine-readable JSON or annotated text section.
`--gap-out gaps.json` additionally creates an independent non-overwriting
gap evidence file. `--min-gap-bytes` (default 256) and `--max-gaps`
(default 100) bound detailed output; aggregate byte accounting is never
truncated by these display controls. No actual binary was processed here.
