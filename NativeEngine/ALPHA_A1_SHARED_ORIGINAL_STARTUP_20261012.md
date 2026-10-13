# A1 checkpoint — neutral original supervisor and character creation source

**Date:** 2026-10-12 (US Eastern), 2026-10-13 UTC CI.
**Canonical branch:** `native-engine`. The Alpha → Beta → Final gates remain in [README.md](README.md). This checkpoint is an **incremental Alpha A1 dependency**, not a playable game or a new versioned Windows release.

## Implemented

- `Shared/BootFlow/include/rloft/boot_flow.hpp` and `src/boot_flow.cpp` are the **unchanged original-source-traced** `rloft::BootFlow` supervisor model formerly in `DemoV1`. They preserve the CODE.2 source startup split, original menu index 2 → CODE.1, supervisor continuation status 10 → CODE.2, existing CHARS branch, presentation skip/latch and exit status 0. No startup logic was rewritten.
- `Shared/CharacterCreation/include/rloft/character_creation_flow.hpp` and `src/character_creation_flow.cpp` are the unchanged existing CODE.1 original choice machine: actor selection, sex/race/class combination rules, optional multiclass, portrait, existing actor and completion state. `Shared/CharacterCreation/tests/character_flow_tests.cpp` is also byte-identical to the former demo regression.
- The five **Git blob SHAs match exactly** before/after extraction: BootFlow source `26f1c29ff27aa8355b7ba70184820147c023ed8d`, BootFlow header `d2ab1f3bf1814a06a479f5854fb2460b5d79c143`, character logic `f395419c839a0c6809366cdc6d3239c3370ef7a0`, character header `181acc69c6e04ca2c78a79465e7cb5afe18a4c07`, character test `9e4078972239811614018cf5fb25ea0439da6809`.
- The `DemoV1` CMake project on **this branch** consumes both neutral libraries. Its former public headers are forwarding compatibility headers; redundant compiled source files and the duplicate character test source are removed. The **separate** `demo-v1` branch has not been altered or silently merged.
- `NativeEngine` CMake links the same two neutral libraries to the independent `rloft_native.exe` target. Native production still does **not** compile `../DemoV1/src`, link `rloft_boot_flow` or relabel/repackage the demo executable.
- The native installation picker in `src/win32_native_assets.cpp` now calls the actual shared `rloft::BootFlow` after validating `CODE.1`, `CODE.2` and `INIT`, using read-only `NativeMedia.contains("CHARS")` to classify the original startup branch. The diagnostic explicitly states that original party import, ANM presentation and character UI integration are still pending. It performs no write to original media.

## Validation

- [Native source-only CTest run #38343564098](https://github.com/Hyperbog/RLOFT/actions/runs/38343564098): **35/35 PASS**, including `native_shared_boot_tests` for ordinary startup/creation/transfer/existing party transitions and `native_shared_character_flow_tests` for full CODE.1 choice rules.
- [Separate DemoV1 source compatibility run #38343815405](https://github.com/Hyperbog/RLOFT/actions/runs/38343815405): **2/2 PASS** compiling and running the unchanged demo `boot_flow_tests` and shared `character_flow_tests` against the neutral source. Compiled source remains owned by `Shared`. Neither run contained copyrighted game media or claimed full game parity.
- The native Win32 installation-loader integration requires an **MSVC build and desktop run** before it is marked Windows-verified. No new Windows EXE or asset-bearing bundle was created by this checkpoint.

## Next gameplay work — must not regress into standalone diagnostics

A1 is **partially implemented**, not complete. The existing `DemoV1` interactive cinematics, original menu visuals/70Hz cadence, CODE.1 character-creation presenter and original `CHARS` 0x920-byte transfer must enter `rloft_native.exe` through neutral interfaces; do **not** compile `DemoV1/src/win32_main.cpp` into the native target or create a second asset decoder.

The next design/implementation seam is the existing `RavenloftBootAssets` constructor taking the demo-only `GameSource` plus `StartupPresenter` / `AnmCinematic` / `CharacterCreationPresenter`. The native app already has `NativeMedia` and must retain it as the one original-file loader. Extract a loader-neutral indexed asset provider (or equivalent owned original-byte interface) for the existing presenters and adapt original-file reads from `NativeMedia`; preserve exact indexed palette/timing and make `DemoV1` pass its own acceptance. Later A0/A2–A4 gates include real party/world state, source-authored RES3 scenery and doors, event/inventory/combat and profile-only save/reload. **Neither this checkpoint nor technical Alpha 0.12 satisfies playable-Alpha exit.**
