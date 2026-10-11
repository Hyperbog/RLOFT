# Live original-game runtime reuse — independent Windows native engine

**Date:** 2026-10-11. **Branch:** `native-engine`. **Status:** committed source integration, **NOT yet compiled on MSVC/Windows and NOT yet field-tested**. Historical [bootstrap build 15/15 Windows CTests](WINDOWS_VERIFIED_BUILD_20261011.md) and the [user's authentic GOG static menu field test](WINDOWS_GOG_FIELD_TEST_20261011.md) refer to earlier compiled source `1f8287f61346c1143b3e9348342a3691a487df4a`; **they do not verify this live-game target**.

## Why this change

The decomp DemoV1 already contains working **CINE00/CINE12 animation with frame audio, 70Hz/PIT-paced intro/logo fades, original menu cursor selection and animations, CODE.2 supervisor→CODE.1 character-creation flow, stats/portraits/music and original-format 0x920 CHARS transfer back into CODE.2**. The owner confirmed the demo runs through character creation with accurate timing and animations. Rewriting these systems in NativeEngine would introduce avoidable divergences.

The independent `rloft_native_bootstrap.exe` is a proven installed-asset *static viewer* only. This integration adds **`rloft_native.exe` as a new, separately compiled Windows game product entry**, which **uses exactly the same source-traced gameplay implementation as DemoV1** but changes its product entrypoint, source provenance, registry identity, developer switch policy and mutable runtime filesystem destination.

## Source arrangement: no duplicate game logic

- **176 reconstructed game-core source files**: originally hardcoded in `DemoV1/CMakeLists.txt`; now declared once in `DemoV1/cmake/shared_game_core_sources.cmake`, referenced by the unchanged `rloft_boot_flow` demo library and separately by `NativeEngine` library `rloft_native_game_core`. The original 176 members were transferred without intentionally dropping or adding any; source equality verified via source-text inventory but **demo/full MSVC rebuild pending**.
- **Live Win32 host**: `DemoV1/src/win32_main.cpp` (original large integrated animation/input/audio/character/gameplay flow), compiled into both products. Common external entry `rloft::run_windows_game(HINSTANCE)` lives in `DemoV1/include/rloft/win32_game_entry.hpp`; the demo still owns its original `wWinMain` when not compiled with `RLOFT_NATIVE_ENGINE`.
- **Native entrypoint**: `NativeEngine/src/win32_native_main.cpp` owns the *different* `rloft_native.exe` `wWinMain`, calls the shared runner, and exposes a headless `--self-test-product` for Windows acceptance only.
- **Same original sound and MIDI implementation**: the native target compiles `DemoV1/src/win32_audio.cpp` and `DemoV1/src/win32_midi.cpp`. It does not rewrite audio timing, palette fades, or the original animations.
- **Normal-play policy**: `RLOFT_NATIVE_ENGINE` compilation rejects DemoV1 developer options (including `--explore-level22`, inspection, trace and capture flags). Normal game source selection is the existing demo's installation-backed discovery or one supplied path.
- **Distinct identity**: different initial Windows window class/title, error dialog name, and `HKCU\Software\Hyperbog\RLOFT NativeEngine` preference; the demo retains `RLOFT Demo V1` and its own executable entrypoint. Neither application is renamed, copied, or overwritten.
- **Normal original flow**: source-driven CINE00/CINE12 → menu → select original CODE.2 option 2 → CODE.1 character creation → original `CHARS` transfer → CODE.2 exploration, without the explicit demo shortcut. A functional end-to-end run of this new target **has not yet been observed**.

## Source-media immutability — critical safety behavior

The existing demo's `GameSource::runtime_output_path("CHARS")` can select a writable file *inside the original game's install*. Directly reusing that code would endanger user-owned DOS/GOG/Steam saves. This change adds the **opt-in** `GameSource::set_runtime_root(path)`, which:

1. Requires an absolute path, canonicalizes it and rejects a profile nested inside (or enclosing) the installed game directory.
2. Changes *both* `runtime_file()/read_runtime()` and `runtime_output_path()` to resolve against the private directory only. A missing native `CHARS` does **not** fall back to original user `CHARS`.
3. Rejects traversal, separators and unsafe filenames at the runtime-file boundary.
4. Leaves `GameSource::read()` and installed original-media archives unchanged.
5. Keeps the existing DemoV1 semantics until explicitly opted into the sandbox path, so demo regression behavior should remain unchanged.

The native Win32 host installs the profile immediately **before** reading its startup `CHARS`. `NativeEngine/src/win32_native_profile.cpp` obtains `FOLDERID_LocalAppData`; default is:

`%LOCALAPPDATA%\Hyperbog\RLOFT\NativeEngine\Default\`

The original game directory is never the target for new native `CHARS` writes. Existing original parties are intentionally **not automatically imported**, and runtime `SOUND.CFG` defaults to native profile unless explicitly configured; exact original-user sound preferences require a later **read-only, opt-in import** (not unsafe fallback). This is an *application-level* source-protection contract, not an OS hostile-process sandbox or atomic NTFS ransomware guard.

## Current demo-branch parity follow-up

After the first source integration, an exact live `demo-v1` branch comparison identified **four newer C++ modules** that were absent from the `native-engine` snapshot: `original_dos_feedback_runtime.cpp`, `original_dos_keyboard_irq.cpp`, `original_dos_keyboard_session.cpp`, and `win32_scan_observation.cpp`. Their four headers and four sources were copied **byte-for-byte** from current `demo-v1` to `native-engine` (eight matching Git blob hashes verified), and the **authoritative game-core manifest now contains all 176 currently listed demo source files in their original order**. These modules are presently research/runtime-contract foundations, **not a fabricated Windows virtual-key-to-DOS-port60 adapter or a replacement for the existing working DemoV1 input path**. The upstream DemoV1 host also has additional developer-only scan-observation trace instrumentation that was not injected into the new native host; it must not alter original gameplay. Demo-v1 and native-engine remain different branches; exact continued parity needs later source review.

## Tests authored — NOT executed on Windows

- `native_profile_isolation_tests` opens synthetic original-like installation media with existing `CHARS` and `SOUND.CFG`; verifies profile override hides original runtime data, native writes land only under the new directory, installed `CHARS` bytes remain unmodified, original `RES0` remains readable, and original-overlap/traversal attempts fail.
- `native_live_game_product_self_test` runs real `rloft_native.exe --self-test-product` noninteractively, proving the separate entrypoint resolves an absolute application-owned Windows profile location. Does **not** play original game or verify sound/video.
- Full CTest plan now **at least 17 tests** (15 previously passed Windows bootstrap tests + 2 new native/product checks), subject to successful MSVC configure and build. `NativeEngine/Windows/Build-LiveNativeGame-Windows.ps1` is a **local, user-authorized** recipe to build the new EXE, rerun all tests, and package a no-assets Windows field-test preview with exact SHA-256/provenance. It **does not dispatch GitHub Actions** and has not run yet.
- The Windows source/entrypoint, two GameSource paths and source-manifest wiring passed **source-static assertions**. This cannot substitute for compiler, actual Windows GUI or user owned-media tests.

## Next milestone and gates

1. **Obtain per-run owner authorization** for new VS2022/MSVC x64 source revision. Build the independently named `rloft_native.exe`, run 17+ CTests, record compile/link warnings and verify original DemoV1 regression targets remain source-compatible.
2. Field test owned GOG install on Windows: original CINE00/CINE12 video/audio, fades, menu click option 2, CODE.1 creation animation, finish, native profile `CHARS` and first legitimate original CODE.2 world blocker. Compare **actual** behavior with already-working demo and supplied DOS original references; do not reimplement the systems.
3. Verify original `CHARS` and `SAVE.DAT` remain unchanged with before/after hashes on a protected copy, and native profile owns newly written `CHARS`.
4. Record feature-by-feature acceptance separately for source, compiler/tests, user field-play, and original DOS fidelity. Static boot image and synthetic unit tests are not gameplay acceptance.
5. Only after proven original execution, widen coverage of normal gameplay and integrate rendering/backend choice without disturbing original mechanics.

**Important limitation:** the source-isolation checks and new native executable are **not yet built or shipped**. Existing Windows-bootstrap artifact `RLOFT-NativeEngine-Windows-x64-BOOTSTRAP-20261011` is an older tested static diagnostic and must **not** be presented as this game preview.
