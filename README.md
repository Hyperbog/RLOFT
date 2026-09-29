# RLOFT

A clean-room C/C++ source-port project for **Ravenloft: Strahd's Possession**.

The project goal is to reconstruct the complete original executable behavior in maintainable C/C++ while loading the user's legally obtained original game data at runtime. The original DOS executable is a reference oracle during verification; it is not intended to be a runtime dependency of the finished port.

## Project goals

- Reconstruct all reachable `CODE.1` / `CODE.2` game logic in C/C++.
- Preserve original arithmetic, state ordering, RNG consumption, timing, save layout, quirks, and bugs in the faithful compatibility layer.
- Load graphics, audio, maps, text, and other game content from the user's original Ravenloft files. No original copyrighted game data is distributed here.
- Keep the faithful compatibility core separate from modern presentation, bug fixes, QoL changes, and mods.
- Support both a faithful presentation path and a modern high-resolution viewport over the same authoritative simulation.
- Build reusable SSI-era reverse-engineering/source-port infrastructure so later projects, beginning with the Ravenloft sequel, can reuse proven components.

## Current state

The reverse-engineering/decompilation evidence base has reached its closed milestone. The C/C++ reconstruction is **in progress** and must not be confused with decompilation closure.

The imported baseline is development checkpoint **0.2.31**. It contains validated native-data loaders and reconstructed portions of movement, collision, timing, NPC behavior, combat, object handling, inventory, saves, rendering selection, and the opening Forgotten Realms Forest vertical slice.

See:

- `docs/ROADMAP.md` — full source-port roadmap
- `docs/PORT_STATUS.md` — current implementation/verification status
- `docs/ASSEMBLY_RECONSTRUCTION_MAP.md` — original-address → C/C++ mapping
- `docs/ARCHITECTURE.md` — architectural rules
- `docs/DATA_POLICY.md` — original-data and repository policy

## Build

### Portable developer build

```sh
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

### Windows alpha

```text
BUILD_AND_RUN_ALPHA.bat
```

The Windows alpha asks for the user's original `RLOFT` installation/data location. Original Ravenloft data is not included in this repository or its source releases.

## Accuracy policy

The original executable, original shipped data, original saves, and observed original behavior are authoritative for the faithful reconstruction. External tabletop rules, lore sources, fan documentation, and community assumptions are not used to fill implementation gaps in the compatibility core.

A subsystem is tracked independently as:

1. **Reverse engineered** — original behavior is understood and evidenced.
2. **Reconstructed** — behavior has a C/C++ implementation.
3. **Verified** — the implementation has been compared against original behavior/data.
4. **Playable** — the reconstructed system works end-to-end in the source port.

## Legal / data requirement

This repository is source code and reverse-engineering documentation. It is not a distribution of Ravenloft game assets. Users must provide their own original game files.

Ravenloft and related trademarks/assets belong to their respective rights holders. This project is an independent source-port/reimplementation effort.
