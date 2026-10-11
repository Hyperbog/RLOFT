# Shared evidence adapters — NativeEngine / DOSDis / DemoV1

The canonical \`SharedDecomp/adapters.py\` normalizes **actual published export contracts** from the three workstreams without changing the binaries, linking DemoV1 into NativeEngine, or exposing installed original bytes.

The adapters are **source-report-only**; they do not authenticate original DOS execution, confirm disassembler candidate instructions, establish matching compiler code generation, or certify native gameplay. Adapters are built on \`code1-decompilation\`; the other independent project branches remain untouched.

## Supported source formats

| Input producer | Command kind | Contract | Output |
|---|---|---|---|
| \`rloft_native_bank_audit.exe\` | \`native-bank\` | \`rloft-native-bank-audit/v1\` | RES per-bank counts, type summaries, anomalies and total statuses |
| \`rloft_native_world_audit.exe\` | \`native-world\` | \`rloft-native-world-system/v1\` | 32-level geometry, control and event counts; SINTAB metadata |
| DOSDis C# analysis exporter | \`dosdis\` | \`dosdis-analysis-report\` | Candidate graph counts only; **not original proven CFG** |
| DOSDis MZ desktop exporter | \`dosdis\` | \`dosdis-mz-16-candidate-analysis\` | Candidate MZ code count and unknown bytes; **not executed** |
| DemoV1 diagnostic logger | \`demo-events\` | \`rloft-level22-diagnostic-v1\`, \`events.jsonl\` | Normalized, consecutively numbered **native-only** event JSONL |

The structural adapter does not export original instruction bytes, arbitrary DOSDis assembly strings, event frame pixels, palette buffers, installed paths, or executable contents. It preserves a SHA-256 digest of the **input metadata report**, not of original media. DOSDis's exporter-declared source SHA-256 is retained but never treated as independently attested. The native bank/world audit reports presently lack original-byte hashes: their original identity is explicitly **unattributed**.

## Commands

\`\`\`sh
python3 SharedDecomp/adapters.py adapt native-bank /private/bank-audit.json --output build/research/bank-observations.json
python3 SharedDecomp/adapters.py adapt native-world /private/world-audit.json --output build/research/world-observations.json
python3 SharedDecomp/adapters.py adapt dosdis /private/dosdis-analysis.json --output build/research/dosdis-observations.json
python3 SharedDecomp/adapters.py demo-events /private/session/events.jsonl --manifest /private/session/manifest.json --output build/research/demo-events-normalized.jsonl --report build/research/demo-events-source.json
python3 SharedDecomp/adapters.py compare build/research/bank-baseline.json build/research/bank-candidate.json --output build/research/bank-first-diff.json
python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v
\`\`\`

\`compare\` accepts **only the same observation domain** and the same declared original identity. It returns \`EQUAL_METADATA_UNATTESTED\` or \`DIFFERENT\` with the first thirty differing keyed fields; it deliberately rejects comparing different types of reports. This is for version/regression comparison, not claiming one engine matches a DOS reference.

The DemoV1 adapter preserves original order, event name and two signed numeric arguments from the existing user-opt-in developer logger; its \`step\` begins at zero for compatibility with \`SharedDecomp/workflow.py diff\`. It cannot infer dropped events when the underlying logger hits its fixed cap. It must **not** be used as the original/reference input to a DOS parity claim. For real gameplay parity, independent original DOS trace collection and agreed event semantics are still required; the provenance-bound capture contract is in [TRACE_CERTIFICATION.md](TRACE_CERTIFICATION.md).

All tests use synthetic original-free fixtures modeled on the current published C++/C# export fields. Source-only CI tests the schema adapter, rejection cases and deterministic comparisons. It does not run the Windows applications or a real owned GOG game.
