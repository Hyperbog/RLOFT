# Agent rules for Ravenloft reconstruction / native recompilation

Use [SharedDecomp/AGENT_RULES.md](SharedDecomp/AGENT_RULES.md) and [ResearchInfra/WORKFLOW_INTEGRATION.md](ResearchInfra/WORKFLOW_INTEGRATION.md). Respect the existing [branch ownership](docs/BRANCH_OWNERSHIP.md): CODE.1/CODE.2 research, DOSDis, asset extraction and native engine are separately owned; do not trigger unauthorized Windows gameplay builds.

1. Verified CODE.1/CODE.2 original instruction accounting is **not** complete original DOS behavior or native Windows runtime equivalence.
2. Original DOS/4GW LE object/page/fixup layout is authoritative. Preserve source/object/function identities, original file hashes, relocation semantics and DGROUP state; do not silently adopt generic ELF/objdiff outcomes as DOS matches.
3. Watcom 10.0a is not proven to be the original compiler. Compiler-match/permutation work requires a normalized authenticated machine-function target and independently verified toolchain, not speculative compilation scores.
4. Reuse `ResearchInfra/index.py` for static C++ caller/callee packets and `ResearchInfra/export_workflow_context.py` for bounded AI context. Source calls are not observed DOS reachability.
5. Use independent original DOS capture and host Windows replay with shared JSONL semantic trace comparison when both actual capture adapters exist; never synthesize two matching traces from one implementation.
6. No original copyrighted game files in workflow artifacts or public commits. Report original-original image verification separately from source-link gate and native playable acceptance.
7. Optional independent REA/Ghidra static analysis for **all** owning workstreams uses [ResearchInfra/REA.md](ResearchInfra/REA.md) and the single canonical `ResearchInfra/rea_bridge.py` via absolute target paths. Authenticate complete CODE.1/CODE.2 originals before LE raw-byte export; keep all derived original bytes and decompiler output under ignored `.rloft-private/`. Do not silently install engines/MCP integrations or treat REA output as original DOS behavior, a completed recompilation milestone, or verified native gameplay.
