# Ravenloft CODE.1/CODE.2 — recompilation workstream

**Owning branch:** `recomp`. Original-source recompilation, DOS/4GW compatibility, native link/startup experiments and independent original-image tests. **Not** the canonical enhanced Windows game: that is [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1/DemoV1).

## Build and test (Windows x64 / MSVC)

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

The owning [.github/workflows/recomp-source-link-gate.yml](../.github/workflows/recomp-source-link-gate.yml) is scoped to `recomp` and packages **engineering executables, not a playable game**. The reconstructed `Code1/` and `Code2/` directories remain at their established paths because these are compile inputs. [Workstream/branch guide](BRANCH_GUIDE.md) documents ownership.

## Acceptance boundary

Historical research reached R01–R03 of 12 planned gates, with R04 DOS/DPMI memory/startup investigation incomplete; the older milestone count is **not a current measure of enhanced-game completeness**. The last historical README claimed Windows source run #38325531978 with 63/63 CTests at its stated revision. Retest current HEAD before updating any verification claim. Neither a successful C++ linker nor bounded source entry tests proves original DOS boot, complete original-machine equivalence or player gameplay.

## Evidence and historical notes

The previous extensive milestone log remains in [README_HISTORY_20261017.md](README_HISTORY_20261017.md), in its original directory to preserve relative links to source-address audits. Original-file identity checks, DOS memory selectors and initializers must remain independently attributable to CODE.1/CODE.2, with no proprietary originals in build artifacts.

[Current GitHub branch/CI observation](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md) is automated, but does not establish recompilation semantic acceptance.
