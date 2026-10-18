# DOSDis — multi-platform video game disassembler

**Owning branch:** [`DosDis`](https://github.com/Hyperbog/RLOFT/tree/DosDis). The Windows WPF desktop application is the user-facing product. The CLI is a developer/independent-validation harness, not a second player application.

## Scope

- **DOS:** conservative MZ/LE module loaders, x86-16/x86-32 decoding, address/CFG candidates, source bytes and data references.
- **NES:** initial iNES NROM/UxROM and documented 6502 instruction support.
- **Sega Master System:** initial Sega/Codemasters mapper and partially implemented Z80 decoding.
- Larger game/console support is an **objective**, not an established accuracy guarantee.

A candidate instruction, branch, mapper or game-level function is **not** a verified executable behavior claim. Preserve original instruction bytes privately; do not bundle the original Ravenloft game in public artifacts.

## Build and use

Windows source and CI entry points: [desktop project](src/DosDis.Desktop/), [build script](build-windows.ps1), [synthetic test suites](tests/) and [accuracy policy](docs/INDEPENDENT_VALIDATION_POLICY.md). [Multi-platform support limits](docs/MULTIPLATFORM_CORE_PLAN.md) document partial 6502/Z80 functionality. Use the desktop's original-game file loading, analysis and export tools to cross-check original source research.

A prior [Windows run #38947104950](https://github.com/Hyperbog/RLOFT/actions/runs/38947104950) passed its C# builds and WPF smoke for a specific source revision; actual latest workstream HEAD and source correctness require separate verification.

## Historical reference

The former 63 KB chronological README is preserved as [README_HISTORY_20261017.md](README_HISTORY_20261017.md), maintaining its relative links to individual proof records. Existing `docs/` P0, platform and source-classification evidence remains in place. Do not append a new top-level status headline for every change; update a focused subsystem report and tests.

[Live branch/CI view](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/LIVE_STATUS.md) reflects GitHub state, not independent disassembly accuracy.
