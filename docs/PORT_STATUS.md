# Port status

This file tracks the source-port project separately from the completed reverse-engineering milestone.

## Project metrics

| Metric | Current state |
|---|---|
| Reverse engineering / evidence closure | **100% closed milestone** |
| C/C++ executable reconstruction | **In progress — baseline contains a partial address-mapped implementation; full denominator ledger is being established** |
| Behavioral verification | **In progress — reconstructed subsystems include focused and original-data regression coverage** |
| Full-game playable parity | **Not yet achieved** |
| Modern viewport parity | **Experimental / not a completion metric until the faithful runtime is complete** |

No percentage will be assigned to full C/C++ reconstruction until the complete executable routine/classification ledger supplies a defensible denominator.

## Imported baseline: 0.2.31

The baseline selected for Git import is the last known-good reconstruction checkpoint before conversion to the full source-port program.

It contains reconstructed or partially reconstructed native-data loading, movement/collision, cell interactions, NPC behavior, RNG/timing, combat, object handling, ground pickup, inventory interaction portions, save handling, rendering selection, automap, HUD scaffolding, and the opening vertical slice.

## Baseline verification

Clean baseline verification performed before the canonical Git import:

```text
cmake -S . -B build-linux -DCMAKE_BUILD_TYPE=Release
cmake --build build-linux
ctest --test-dir build-linux --output-on-failure

100% tests passed, 0 tests failed out of 1
```

## Immediate milestone

**M1 — Complete executable reconstruction ledger**

1. Enumerate CODE.1 and CODE.2 routine/function targets from the closed decomp evidence.
2. Reconcile indirect/jump-table/orphan candidates.
3. Match every existing C/C++ reconstruction to original address/range.
4. Classify remaining routines as game logic, runtime/library support, embedded data, or dead/unreachable.
5. Generate machine-readable and human-readable coverage reports.
6. Use that denominator for future reconstruction percentages.
