# Canonical CODE.1 / CODE.2 whole-source compilation and linking gate

This is the first **actual C++ compilation, linker and executable runtime**
gate on the `code1-decompilation` branch. The target includes both canonical
`Code1/Code1.cpp` and `Code2/Code2.cpp`, unchanged, in separate compilation
units and links them into one executable. Its three lightweight probes call
**compiled canonical reconstruction functions**, not synthetic replacements:

- CODE.1 `7F50`: real DGROUP 32-bit tick read.
- CODE.2 `3A80A`: real DGROUP 32-bit value read.
- CODE.2 `353A0`: the real original entry stub, checked against its original
  `tail_35418` boundary contract (the downstream callback is deliberately an
  observation witness, **not** a fabricated implementation of DOS startup).

Run on Windows with MSVC and CMake:

```powershell
cmake -S Recomp -B build/recomp -A x64
cmake --build build/recomp --config Debug --parallel 2
ctest --test-dir build/recomp -C Debug --output-on-failure
```

**What a pass proves:** the two current source files successfully compile as
code (not `-fsyntax-only`), their symbols can link into an executable, and
three restricted source-backed entry/leaf operations execute successfully.

**What a pass does NOT prove:** original LE loading, correct loaded data,
DOS services/interrupts/graphics, the entry-to-35418 *implementation*, a
working character creation screen, gameplay, or DOS pixel/timing parity.
The whole-game recompilation remains **unbuilt** until those gates pass.

Next milestone after this build gate is the first original entry-point execution
with authenticated data-object initialization and a source-backed DOS service
boundary (no null or fabricated callback counted as progress). Explicitly
record the first actual boot blocker and fix it, instead of more compiler
fingerprinting research.

This project is strictly separate from `demo-v1` and `native-engine`. Do not
invoke their full Windows build workflows from this target.

## Whole original LE image integration (2026-10-11)

The current source tree now also builds `rloft_original_image_gate`, which reconstructs **the entire original LE code/data memory image** for both modules and validates all original object/page/fixup records against the original executable bytes. Its synthetic, media-independent CTest is registered beside the existing canonical source-link CTest.

After building, to run against personally installed original media (not included in this repository):

```powershell
.\build\recomp\Debug\rloft_original_image_gate.exe "C:\RLOFT\CODE.1" "C:\RLOFT\CODE.2"
.\build\recomp\Debug\rloft_recomp_link_gate.exe "C:\RLOFT\CODE.1" "C:\RLOFT\CODE.2"
```

The latter additionally calls the **compiled canonical CODE.1/CODE.2 memory getter functions** with the actual loaded data objects. Without those two arguments, CI explicitly reports `CANONICAL_SOURCE_WITH_ORIGINAL_DGROUP=NOT_RUN_ASSETS_ABSENT`; it never silently substitutes synthetic data for a claimed original-file acceptance.

The original-file loader passed GCC C++17 validation using authenticated `CODE.1` and `CODE.2` files: **12,430/12,430 original fixups, seven signed backward-page cases**. Windows [run #38173341351](https://github.com/Hyperbog/RLOFT/actions/runs/38173341351) passed the expanded two-test source-only CMake project at commit `7d16425d014a6afd20a907ae0569cec85bc4b113`. See [whole-image evidence and limitations](ORIGINAL_LE_IMAGE_INTEGRATION_20261011.md).

**Next gate is real startup:** `CODE.2:353A0→35418` with verified DOS/4GW services and original selector/PSP/environment state. This loader does not by itself execute or certify DOS game startup.

## 2026-10-11 original CODE.2 startup to game-entry boundary

The extended Windows CTest now **executes the canonical reconstructed original `353A0→35418→395E2` startup, all four actual initializer callbacks `3A16E,3A2AD,3A1B3,3968E`, the original core allocator request path `37B42`, and post initializer `39589`**, then stops at the actual game-entry callback `20608`. The original selector environment adapter and arena memory allocator remain **test-only bounded fixtures**, so this is not DOS/Watcom heap parity and does not boot the game. The earlier first-initializer gate remains a separate CTest; the expanded CMake project has **4 tests**. [Implementation, original span oracle and acceptance](INITIALIZER_20608_INTEGRATION_20261011.md).

[Windows source-only Actions run #38186500373](https://github.com/Hyperbog/RLOFT/actions/runs/38186500373) passed **4/4** tests at executable source commit `7af91b0efe974ff781c1eb4b6642e92ee9b001b5`. The owner-uploaded CODE.2 original is checked separately with `Recomp/tools/verify_code2_startup_oracle.py`; no original game files are in the Windows runner.

## Original termination callback source execution — 2026-10-11

The CODE.2 static LE table resolver independently authenticated an otherwise easily missed **termination callback at `0x3A228`**. Original DGROUP record `0x12738..0x1273D` has **five file-backed bytes and one LE zero-fill byte**, producing target `0x3A228`, priority `0x20`, initial flag `0`. The terminating dispatcher executes the real original `function_3962d` callback scanning routine, which calls the reconstructed original `function_3a228` (stream close then exit) when its source callback is selected.

`Recomp/src/code2_startup_gate.cpp` now exposes a separate `--termination-probe` test path, using the original static table values with a zeroed source stream list and a **fail-closed synthetic OS exit handler**. It checks the selected original `0x3A228` callback and the original scheduler's argument `0xFF00` before stopping at the unimplemented DOS exit `0x3A63E`. A full original stream-close path and actual DOS process termination are **not executed**.

[Windows canonical source CI run #38186890574](https://github.com/Hyperbog/RLOFT/actions/runs/38186890574), source commit `07e70d64e493dc1fd0a4a74547aca00706828e2a`, passed **all 5/5 CTests**, including the existing four-initializer game-entry test and new original shutdown scheduler probe. The original media were **not included** in Windows CI. The owner-uploaded `CODE.2` was separately authenticated locally with its full SHA-256, original scheduler instruction-span SHA-256s, exact indirect `FF D0` call opcodes, the four initialization records, and the six-byte shutdown record (five original file bytes plus one virtual zero-fill byte). Source analysis does not establish dynamic DOS target immutability.

## 2026-10-11 actual original allocator arena source integration

The canonical CODE.2 startup and stand-alone arena/rollback probes now execute reconstructed **`37B42 → 39838`** block selection, splitting and linked-list metadata, plus **`37C37 → 398E0`** free/coalescing. The original-byte verified `37BC2 MOV EAX,EDI` request-ABI discrepancy was fixed (raw seven-byte requests produce 12-byte arena blocks rather than 16). Test-only memory still seeds one bounded guest arena and explicitly injects one failure; neither test models DOS/DPMI allocation. [Detailed evidence and exact branch sync](CODE2_ORIGINAL_ARENA_CLOSURE_20261011.md).

[Canonical Windows source compile/link CTest #38190078813](https://github.com/Hyperbog/RLOFT/actions/runs/38190078813) passed **7/7** on `16a2ca0f93adb2e90403e1e45995edccbc22bd72`. The original `20608` game application code is still **not executed**; DOS runtime parity and native game boot are not established.

## 2026-10-11 canonical original game-main execution (new integration)

The existing source gate previously stopped *before* the original application
`CODE.2:20608`. Three new Windows CTests execute the actual compiled
`353A0→35418→395E2→39589→20608→20440→2F2EC` path, using the four
original callback initializers and the existing bounded DOS service/allocator
fixture. The original 20608 game-state writes, actual 352EE getenv lookup,
39526 case comparator, 35349 decimal parser and first original 2F2EC
audio/resource initialization writes are all exercised. The source stops at
the **real original DOS/4GW 375FE far callback installation boundary**;
it does not fabricate a successful interrupt or continue pretending to
boot gameplay. A default path, configured original environment and
negative/overflow variant are separately asserted.

Run:
```powershell
ctest --test-dir build/recomp -C Debug --output-on-failure
.\build\recomp\Debug\rloft_code2_startup_gate.exe --enter-game "C:\RLOFT\CODE.2"
python Recomp/tools/verify_original_game_entry_oracle.py "C:\RLOFT\CODE.2"
```

Original source spans and actual owner-provided binary settings were
independently hashed and authenticated. See
[the original game-entry integration audit](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md).
**Build verification:** [canonical Windows CI run #38196507014](https://github.com/Hyperbog/RLOFT/actions/runs/38196507014)
compiled and passed **10/10 CTests** on exact commit
`56139409213a0bf4af19dc0adb060b477a5baa16`, including all three
new original game-entry runtime executions.

**Further source integration:** The actual canonical `function_375fe` now
runs from the original `2F2EC` callback, preserving its original DOS
extender-dependent interrupt-vector choice; the source stops **inside
375FE at the DOS vector-install service** rather than at a preceding
callback. A fourth independent integration CTest exercises all 256 mode
values against the original selector/vector/handler contract. The
[original game-entry source audit](GAME_ENTRY_20608_SOURCE_EXECUTION_20261011.md)
documents this change. A fresh Windows build is required for this extension.

**No full DOS game boot, visual gameplay or native-engine milestone is
claimed.** Passing CTest proves the named source-execution boundaries only,
not original-DOS hardware, audio or frame equivalence.
