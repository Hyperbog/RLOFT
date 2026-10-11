# RLOFT shared Research Infrastructure (foundation)

**Canonical owner:** \`code1-decompilation:ResearchInfra/\`. Use this data/tooling from each workstream without wholesale cross-branch merges. \`project_registry.json\` is the branch-ownership manifest; actual HEADs and acceptance reports must always be checked live.

The foundation is a **read-only, deterministic, standard-library-only Python indexer** of the canonical CODE.1/CODE.2 C++ source. It creates a machine-readable symbol and static direct-call index, detects missing reconstructed definitions, computes source identities, and generates short context packets for AI agents.

It **does not** assert that a parsed function is semantically correct, reachable in DOS, matched by historical Watcom, or accepted in gameplay. Call edges detected in C++ are **not** a disassembly/original-runtime CFG; they are source-derived dependencies, explicitly named as such. Original file checks are separate SHA-256 authentication gates.

## Usage (from repository root)

\`\`\`sh
python3 ResearchInfra/index.py build --root . --output build/research/index.json
python3 ResearchInfra/index.py query --output build/research/index.json --key code2:353a0
python3 ResearchInfra/index.py impact --output build/research/index.json --key code2:35418
python3 ResearchInfra/index.py packet --root . --output build/research/index.json --key code2:35418
python3 ResearchInfra/index.py verify-media --root /private/original/install --module code2
python3 -m unittest discover -s ResearchInfra -p 'test_*.py' -v
\`\`\`

For other branches, run the same tool with \`--root <checkout>\` only if that checkout includes both canonical CODE sources, and tag the output with the **real** branch/commit hash externally. The format is designed for later branch-specific adapters but does not currently index native engine methods or DOSDis .NET method symbols. Never silently substitute a different source. The default output directory is \`build/research/\`, which is *generated*, not committed; do not commit original code bytes/assets into the index.

## Workflow / audit boundaries

- Function records: source path, exact source SHA-256, original logical offset from explicit \`function_XXXXX\` name, inclusive source lines, content digest, direct source calls, evidence classification.
- Edges: only statically named calls with indexed target definitions; dangling targets are preserved separately.
- Impact: reverse direct-source-call reachability only; no indirect call, alias, runtime, or DOS reachability claim.
- Packets: small source excerpts with caller/callee graph; refuse to use a packet if the source file changed since indexing.
- Provenance: installed original CODE.1/CODE.2 size and SHA-256 verified as an **opt-in external gate**; no redistribution of original binaries.
- Neither an AI model nor an AST heuristic can promote source-only observations to authenticated original bytes or DOS parity. Continue using the existing original-byte oracles and exact-source Windows CI.

## Next integration layers

(1) Import **original instruction-level CFGs** and indirect-target xrefs from independent x86 LE analysis, with explicit ambiguity tracking; (2) ingest Watcom OMF fixups and normalized instruction diffs from \`Code2/tools/\`; (3) bridge DOSDis JSON and native/DOS field captures; (4) produce project-specific change-impact reports tied to exact Git SHA and original hash. Validate every layer independently before promoting evidence strength.

Branch inventory, retirement policy, and housekeeping live in \`docs/BRANCH_CLEANUP_AUDIT_20261011.md\`; no branch should be deleted merely because its name suggests a temporary build.

## Shared decompilation / native-parity workflow v1

The [shared original-bank/CFG/trace/iteration gates](../SharedDecomp/README.md) extend this source-only index without changing its proof authority. Run `python3 -m unittest discover -s SharedDecomp -p 'test_*.py' -v` and `python3 ResearchInfra/export_workflow_context.py --key code2:35418 --out build/research/workflow_context.json` to generate a hash-verified bounded AI packet. See [implementation and DOS runtime limitations](WORKFLOW_INTEGRATION.md). The tool will not claim DOS codegen or native gameplay parity without independently captured execution traces.


## Independently authenticated original-machine CFG and cross-project packets (2026-10-11)

The original x86-32 LE witness importer is now implemented. [Method and original-byte results](ORIGINAL_MACHINE_CFG_20261011.md).

- \`original_cfg.py\`: source-selected, SHA-pinned original LE instructions → candidate CFG/basic blocks, call/branch edges, external and unresolved indirect edges, and explicit DOS interrupt sites. Requires owner-local original CODE.1 and CODE.2 plus GNU objdump; binary content is never committed.
- \`original_witnesses.json\`: 15 original instruction spans with complete original-file and individual slice hashes; no executable bytes embedded.
- \`compare_call_evidence.py\`: original direct CALL sites versus canonical C++ static function-call references; mismatches are review candidates, **not** a presumed behavioral defect.
- \`original_context.py\`: regenerates authenticated original CFG and bundles code signatures with SHA-fresh source context and project-ownership review lanes; no original-DOS parity or compiler-output matching implied.
- \`test_original_cfg.py\`, \`test_original_context.py\`, \`test_compare_call_evidence.py\`: offline synthetic original-source-safe regression suites.

The existing canonical source index is still the source-only graph, not a decompiler. Original-machine CFG records form a **separate trust domain** and are only joined when original file identity is authenticated. CI tests the synthetic paths; original-file acceptance is an opt-in local operation.
