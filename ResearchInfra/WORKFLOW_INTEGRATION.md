# RLOFT AI-assisted matching / native parity: shared deterministic gates

**Owner:** `code1-decompilation:ResearchInfra/`, matching the repository branch-ownership policy. This reuses, and does not replace, the existing `ResearchInfra/index.py` CODE.1/CODE.2 source index, compiler probes, original DOS/4GW LE loader, or canonical Windows source compilation.

The common `SharedDecomp/workflow.py` adds original-vs-native JSONL trace comparison, evidence/CFG bounds and a hypothesis improvement guard. The bridge `ResearchInfra/export_workflow_context.py` wraps the existing **hash-verified** caller/callee/source packet, with `STATIC_SOURCE_ONLY` evidence identity and **false** original-binary, DOS runtime and native parity flags. It rejects stale source hashes exactly as the existing index does.

```sh
python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
python3 ResearchInfra/index.py build --root . --output build/research/index.json
python3 ResearchInfra/export_workflow_context.py --index build/research/index.json --root . --key code2:35418 --out build/research/workflow_context.json
python3 SharedDecomp/workflow.py diff /private/original_dos_steps.jsonl /private/native_windows_steps.jsonl --report build/research/dos_native_first_divergence.json
```

**Source-only CI cannot claim DOS/native parity**: the two `/private/*.jsonl` inputs must be independently captured under the same replay scenario, with a physical LE object+offset code identity, x86 register/flags, DOS service/fixup state, DGROUP writes and relevant timer/VGA/RNG effects. It is forbidden to fabricate or copy both trace inputs from reconstructed C++ output. The common compare only checks aligned logical steps after the source and native capture adapters agree on the event schema. Original DOS media remains strictly user-supplied and uncommitted.

Watcom 10.0a remains a codegen **hypothesis**; existing Watcom OMF probe gates should remain separate from whole-source native compilation and observed DOS runtime parity. `objdiff` may be useful after authentic OMF/LE relocation normalization; do not substitute generic ELF32 similarity for an exact historical DOS object match.

Progress is judged by executable entry/ABI coverage, original versus native replay cases, authenticated LE fixup/loaded-image correctness, and actual native game-state parity, not number of reconstructed C++ functions alone. Three non-improvements stop only the current hypothesis (record blocker), never the complete CODE.1/CODE.2 subsystem workstream.


## Provenance-bound trace gate (2026-10-11)

The stronger `SharedDecomp/certify_trace.py` accepts two *separate* scenario-aligned
trace files and SHA256-bound capture manifests, enforcing distinct declared
reference/host capture identities, contiguous steps and explicit first-divergence
diagnostics. See [the trace certificate contract](../SharedDecomp/TRACE_CERTIFICATION.md)
for required fields and the source-origin proof boundary. A successful comparison
is intentionally labeled `EQUAL_UNATTESTED`: manifest declarations are not
external attestation that DOS reference execution actually took place.
`workflow.py diff` now refuses to ignore live execution-state fields, and a
missing JSON key is reported with separate field-presence indicators instead of
an unserializable sentinel object. These protections are covered by ROM-free
regressions; no original DOS/native gameplay parity has been certified.
