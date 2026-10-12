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
