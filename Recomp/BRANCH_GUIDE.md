# Recompilation branch — working guide

**Owner:** `recomp` branch. **Canonical source:** `Recomp/`. **Baseline:** `85327ed7f25eccbb6184f28360913402d1f5cb3d` on `code1-decompilation` before the 2026-10-17 split.

This branch deliberately retains `Code1/`, `Code2/` and common original-source research because `Recomp/CMakeLists.txt` compiles the existing reconstructed C++ directly. Do not relocate or duplicate those inputs without updating includes, build scripts, tests, and source-location contracts.

## Build and test

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

The Windows workflow `.github/workflows/recomp-source-link-gate.yml` runs for this branch and publishes **engineering test executables, not a playable game**. No original proprietary media belongs in Git commits or CI artifacts.

## Workstream boundaries

- **Recompilation changes:** `Recomp/` and explicitly required source/ABI integration in `Code1/`, `Code2/` on this branch.
- **Decompilation changes:** `code1-decompilation`, with entry point `Decomp/README.md`, and reconstructive sources `Code1/`, `Code2/`.
- **Player game:** `demo-v1`; do not wholesale merge this branch into the game.
- **Original files:** immutable `DnD-Classics/MASTER/`; local originals are user-supplied and excluded from source control.

For cross-branch code sharing, port bounded, verified changes with exact source attribution and rerun each workstream's tests. A successful recompilation unit test does not establish native gameplay acceptance or complete original DOS execution.
