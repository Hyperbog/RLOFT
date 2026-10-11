# Independently Identified Trace Comparison (v1)

Canonical owner: \`SharedDecomp/\` on \`code1-decompilation\`. This is a reusable foundation for DOS game recompilation, independent native engines, DOSDis and other game ports. It does not access or redistribute retail game files.

The existing \`workflow.py diff\` compares trace events but has no provenance contract. The \`certify_trace.py\` gate binds each JSONL trace to its declared capture SHA-256, emulator/host source kind, binary SHA-256, capture adapter SHA-256, session ID and a common scenario. It enforces contiguous logical steps and refuses to compare a file against itself. It compares *all* execution state, except \`wall_time\` and \`capture_note\` excluded by the existing deterministic comparator.

Run:

\`\`\`sh
python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v
python3 SharedDecomp/certify_trace.py \
  --reference /private/original.jsonl \
  --reference-manifest /private/original.manifest.json \
  --candidate /private/native.jsonl \
  --candidate-manifest /private/native.manifest.json \
  --report build/research/trace_comparison.json
\`\`\`

Each manifest has this shape (replace the illustrative fingerprints with *actual* SHA-256 values):

\`\`\`json
{
  "schema": "rloft-trace-capture-v1",
  "role": "reference",
  "scenario_id": "level22-move-01",
  "capture_id": "independently-recorded-original-run-001",
  "source_kind": "original_dos",
  "trace_sha256": "<sha256-of-jsonl-file>",
  "binary_sha256": "<sha256-of-the-actual-executed-binary>",
  "adapter_sha256": "<sha256-of-capture-adapter>"
}
\`\`\`

For the second manifest set \`role=candidate\`, a distinct \`source_kind\` (for example \`native_windows\`) and \`capture_id\`, and its own hashes. Run the same scenario with compatible logical-step boundaries. Identity fields are validated as nonempty names and lowercase 64-digit SHA-256 values; the trace content hash is checked against actual bytes. The manifest's binary and adapter hashes are **declarations, not verified measurements of binaries**. Reference and candidate traces must be collected independently.

Exit codes: \`0\` means \`EQUAL_UNATTESTED\`, \`1\` means \`DIFFERENT\`, and \`2\` means invalid input. A missing, duplicate or out-of-order step is invalid rather than a match. The report includes the first differing step and field when comparable.

## Critical acceptance boundary

An equal result is only a **byte-bound, declared-provenance trace comparison**. It does **not** prove the declared source was executed, original CODE bytes were loaded, the capture adapters observed equivalent steps, DOS timing or hardware emulation was correct, or native game parity. The report therefore always emits:

- \`capture_origin_attested=false\`
- \`original_binary_authenticated=false\`
- \`dos_runtime_parity_verified=false\`
- \`native_gameplay_parity_verified=false\`

A separate verified original-binary hash/original runtime collection gate and independently reviewed step-alignment adapter are still required before escalating evidence. Never convert this into a gameplay certificate by changing labels or fixtures. Tests use only synthetic traces; no real original-versus-native run is asserted.
