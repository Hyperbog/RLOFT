# DemoV1 agent continuity rules

Applies to every new assistant, coding agent, or resumed session working under `DemoV1/`.

1. Read `ENHANCED_PC_PORT_CHARTER.md`, `MOD_FORWARD_PROFILE.md` and repository-root `MOD_FORWARD_METHODOLOGY.md` first. They govern all new player-facing work. `ORIGINAL_EXECUTION_FIDELITY_CHARTER.md` and `CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md` are historical/source references, not mandatory DOS-parity release gates.
2. Re-read live `demo-v1` HEAD, latest commit and touched-file source audits; assume other project threads can commit concurrently. Use a compare-and-swap expected HEAD when updating Git. On collision, refetch, reconcile and re-test, never overwrite work.
3. Use existing `Code1/Code1.cpp`, `Code2/Code2.cpp`, original byte proofs, and `Tools/DISASSEMBLY_CACHE.md` before generating any new disassembly or gameplay code. Linear disassembly is a lead, not an instruction-boundary proof.
4. Preserve authenticated original hashes/patch provenance and relevant source evidence when using original data or code. Use port-native, data-driven contracts for new content and graphics. Never silently substitute invented authored assets, geometry, game rules or unsupported parity claims.
5. Run targeted native tests and source verifiers for changed code. Use `tools/run_original_ground_proof_batch.py` only where original ground-source research is material. A source-only pass, Windows CI pass and actual field acceptance remain distinct gates; DOS parity is optional.
6. The owner authorized automatic Windows Full CI for the public repository on 2026-10-11. Relevant `demo-v1` source pushes now run Full automatically; manual quick/full remains available. CI tests and original DOS acceptance remain different gates. Do not revert to manual-only without owner instructions.
7. For user field feedback, identify exact exe/manifest commit, provenance, original media and consistent camera/view, compare diagnostics, and commit a compact repro and the surviving limitations. Keep game assets/saved states and copyrighted full disassembly out of Git.
8. At every checkpoint, report exact branch HEAD, concrete changed paths/tests, newly closed blockers, still-open dependencies and the next smallest **source-verifiable** integration step. Do not raise completion or DOS-parity claims for native-only success.

For continuous improvement, consult `RECONSTRUCTION_IMPROVEMENT_LOOP_20261010.md` and add new failure patterns to its guardrail ledger rather than rediscovering them.
