# Part 33 — Controlled CLI Decision Trace Export

**Status:** source-only, not built or executed. 2026-10-08.

Part 32 provided in-memory, opt-in analyzer decision records. Part 33 exposes
those records for offline inspection using `analyze --trace-out decisions.json`
with optional `--trace-capacity N` (512 default; allowed range 8–4096).

The new output is a separate local JSON evidence record. Existing analyze
JSON on stdout is unchanged; export confirmation goes to stderr.
The trace file records source SHA-256, object-relative and source-file offsets,
typed outcomes, speculation markers, event/drop counts, and explicit
`analysisCompleted` / `analysisTruncated` metadata. Cancelled and failed
analyses are never mislabeled as successfully completed. No original bytes,
source paths, private analyst comments or arbitrary exception messages are
written into the trace. It is a bounded sample, **not** a complete execution
history or a proven original-game disassembly.

Export policy: explicitly requested only; `.json` path required, 8 MiB
maximum serialized size, target directory must exist, same-directory staging,
flush to disk, non-overwriting rename, and temporary cleanup. Requests to
overwrite are rejected even if the destination appeared during writing.
`--trace-capacity` without `--trace-out` and trace options on `report`
are rejected; invalid numeric capacity is reported as an argument error.

Synthetic source tests cover export content equality, privacy defaults,
existing-report preservation, invalid extension, unstarted traces, temporary
file cleanup and unchanged source SHA-256. **These tests have not been run.**

Future work: source-authenticated import and read-only Session Inspector,
fine-grained analyzer pass tracing, deterministic replay, desktop UX
integration and optional decision stepping. No CLI step debugger or game
execution emulator has been added. Windows-only, manual builds only, no AI
integration before final release. Progress remains **43%**.
