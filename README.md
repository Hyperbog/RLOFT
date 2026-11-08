# Ravenloft Enhanced

**One game. One development priority.** A native, playable and moddable port
of *Ravenloft: Strahd's Possession*, built around the original game's assets,
characters, levels, rules, scripts, sound and story.

**Development branch:** `Ravenloft-Enhanced`. This is a recovered **pre-alpha**,
not a complete campaign or production game. The repository default branch is
still named `Decomp` until GitHub repository settings are changed.

## Start here

- **[Enhanced/](Enhanced/README.md)** — the only player-facing native game
  application. Restored from the verified October 16, 2026 native-game source.
- **[Enhanced project charter](Enhanced/ENHANCED_PC_PORT_CHARTER.md)** — what
  we are building, and what is no longer an arbitrary release blocker.
- **[Enhanced release gates](Enhanced/RELEASE_GATES.json)** — Alpha, Beta and
  final game acceptance, measured by actual player-visible features.
- **[Research/](Research/README.md)** — original CODE.1/CODE.2 reconstruction,
  data-format research and authenticity comparisons.
- **[Recomp/](Recomp/README.md)** — preserved, tested reconstructed-source host
  infrastructure, retained as an *engineering reference*, not a competing
  player product.

## Build the game

**Windows x64 (Visual Studio C++ and CMake):**

```powershell
cmake -S Enhanced -B build/enhanced -A x64
cmake --build build/enhanced --config Release --target ravenloft_demo_v1
```

The existing executable target name is retained until the native build and
launcher contracts are migrated safely. The player binary is staged as
`Ravenloft.exe` by CI. Original game media must be provided by the owner; do
not commit or distribute it with binaries.

## Repository rules

1. `Enhanced/` is the authoritative game. Do not create another independent
   implementation of character creation, inventory, world state or gameplay.
2. Reuse independently verified `Code1/`, `Code2/`, `Recomp/` and research
   routines *within* the enhanced game when they solve a real game problem.
   Exact DOS instruction, port and IRQ equivalence are valuable research but
   not mandatory milestones for the native player.
3. Keep original commercial assets and executables unmodified on the
   `DnD-Classics` preservation branch. Normal builds contain **no game media**.
4. Never confuse a successful CTest/source reconstruction with a playable
   feature. Validate title → character creation → Level 22 interaction →
   save/restore → Level 23, then the full campaign, using a real Windows player.
5. Preserve historical CMake targets and sources until an equivalent new
   target runs and passes. Migrate consumers before deleting compatibility code.
6. Keep dated diagnostic reports in Git history rather than copying hundreds of
   old checkpoint documents into the active player directory.

**Out of scope for this Ravenloft workstream:** DOSDis. It is independent and
must not be imported, built or used as a release dependency.

## Historical source recovery

The October 16 native game source was recovered from
[`8982fcc`](https://github.com/Hyperbog/RLOFT/tree/8982fcc1f0ca68781cf0a55c4af98f4f15f0fa1c/DemoV1).
Its original build is preserved at
[GitHub Actions #38982609266](https://github.com/Hyperbog/RLOFT/actions/runs/38982609266).
This recovery retains engine code, tests, tools and source-safe launchers while
excluding a historically committed Windows EXE and redundant date-stamped logs.
