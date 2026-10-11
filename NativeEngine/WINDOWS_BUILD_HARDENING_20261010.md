# Windows-first Build Hardening — independent NativeEngine
**Checkpoint date:** 2026-10-10  
**Source branch:** native-engine  
**Execution status:** committed Windows code and test specifications; **no Windows/MSVC build or GitHub Actions run performed** in this checkpoint.

## Directive

Windows x64 only for the current native-engine deliverable. Do not allocate new time to Linux compatibility, Linux-specific toolchains, Linux runtime packaging, or Linux CI. Existing portable original-file/source-faithful C++ remains available for reuse. This workstream is **separate from DemoV1** and does not authorize modern gameplay changes before full original game fidelity.

## Windows source improvements

- Replaced the legacy 260-character-limited shell folder picker with a native **IFileOpenDialog** configured for filesystem folders; obtains Unicode `SIGDN_FILESYSPATH` from the selected item and releases COM allocations. The application initializes STA COM first. A cancelled selection does not change original game media.
- Fails safely inside the User32 `WM_COMMAND` callback: catches exceptions raised by Windows shell selection and displays an error in the application instead of unwinding across the OS callback boundary.
- Reports the actual `GameSource.location().media_path` in the UI (important for GOG `game.gog` installs nested below the selected folder).
- Improved the Win32 `--self-test` to compare **all 512000 output BGRA bytes** against the shared 320×400 indexed + 6-bit DAC CPU golden reference and verify that presentation geometry does not change source pixel hashes. The frame is a **synthetic test grid**, never authentic source gameplay.
- Added the hidden noninteractive `--self-test-install <directory or game.gog>` path to the **GUI executable itself**. This calls the actual inherited `GameSource::open` and `inspect_original_install` on disposable synthetic media and returns fail-closed exit codes 0 (minimum signature found), 3 (signature incomplete), 2 (unreadable/invalid). No window is opened, no save/CHARS or source media modified.
- Extended the existing Python original-media fixture to exercise that GUI test path, including Unicode folder names, loose media, standard ISO2048, ISO2352 data offsets 16 and 24, nonexistent directories and empty required files.
- Added MSVC `/utf-8` to the Unicode-bearing Win32 translation unit so the release C++ source is decoded predictably.

## Windows package assurance

- `NativeEngine/Windows/Verify-NativeEngine-WindowsPackage.ps1` opens the **finished ZIP** and checks SHA-256 sidecar, only five allowed files, both extracted executable SHA-256s, exact original Git source SHA, branch/native build metadata, `game_playable=false`, `original_assets_included=false`, and a genuine `PASS` test result. Rejects foreign bundled assets, malformed records and accidental stale revisions.
- `NativeEngine/Windows/Test-NativeEngine-WindowsPackage.ps1` is a **Windows-only synthetic QA fixture**, verifying a valid sample ZIP passes, a tampered ZIP checksum fails, and a correctly re-checksummed ZIP that includes an unexpected `OBJECTS.DAT` file is still rejected. Dummy .exe bytes in this test are **fixtures**, not compiled Windows executables. This proves verifier logic only, not signature or actual executable authenticity.
- The shipping `Build-NativeEngine-Windows.ps1` rejects tracked dirty engine/media sources before stamping Git identity, requires all native CTests (no release skip switch), gives the headless GUI self-test a 20-second timeout, and reopens/verifies the actual finished ZIP before an artifact is considered ready.
- Windows CMake registers three new Windows-only gates: `native_win32_bootstrap_headless_contract`, `native_win32_bootstrap_original_media_contract` and `native_windows_package_integrity_contract`. With Python installed on the Windows runner, **13 tests are now authored/expected** (10 prior portable tests and 3 new Windows-specific tests), **NOT** 13 successfully executed Windows tests.
- CMake Win32 target continues to use VS2022 MSVC C++17 with a native Unicode desktop entrypoint, Windows GDI temporary indexed display, actual inherited `GameSource`, and standard Windows system libraries. The actual Windows framework/GPU SDK (SDL3/bgfx/SDL_GPU/Godot) remains undecided until comparable Windows backend tests.
- `.github/workflows/native-engine-windows.yml` exists on the default branch and native-engine, invokes only **windows-2022**, checks out `native-engine`, runs the VS2022 x64 script, and uploads the asset-free bootstrap ZIP and SHA-256 sidecar. **Manual workflow_dispatch only**, no Linux job, no push or pull-request jobs, no workflow run triggered or observed.

## Distinct verification hierarchy

| Gate | Evidence at this checkpoint |
| --- | --- |
| Source implementation and committed documentation | **Completed; Git SHA verified** |
| Cross-platform unit logic (historic) | **10/10 local Linux CTests from previous batch**; no new Linux work |
| New Windows-only CTest registration | **Three tests authored**, not executed |
| CMake VS2022 configure and MSVC x64 compiler | **Not run** |
| Win32 GUI, long-path picker, keyboard/window interaction | **Not field tested** |
| GUI original-install synthetics on Windows | **Authored, not run** |
| ZIP integrity positive and negative cases under Windows PowerShell | **Authored, not run** |
| GitHub Windows Actions artifact / ZIP / executable SHA256 | **Not built/published** |
| User-owned GOG/Steam/DOS retail installation validation | **Not field tested** |
| Original gameplay, CODE.1/2 startup, full DOS parity | **Not reached** |

## Exact next acceptance

1. One explicitly user-authorized manual GitHub workflow run at https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml. It must build `native-engine` from a verified source SHA and compile a Windows desktop `rloft_native_bootstrap.exe`.
2. Collect actual MSVC compiler, CTest, PowerShell package QA and artifact SHA evidence. Triage the earliest actual Windows compiler/fixture blocker instead of reverting to Linux-only workaround development.
3. Extract the asset-free bootstrap and inspect a protected owned original installation. Record explicit missing/versioned assets, folder picker functionality and independent install integrity.
4. Use the validated Windows host to integrate **source-faithful original graphics and runtime**, not synthetic screens or developer skip flags, toward full `rloft_native.exe`. The bootstrap is not gameplay and **does not close** [M-ENGINE-01 #9](https://github.com/Hyperbog/RLOFT/issues/9).

**Tracking:** [M-WIN-01 #12](https://github.com/Hyperbog/RLOFT/issues/12), [framework benchmarking #10](https://github.com/Hyperbog/RLOFT/issues/10), [original engine playability #9](https://github.com/Hyperbog/RLOFT/issues/9). No new Windows run was dispatched and no copyrighted assets were committed.

## Release fail-closed gate refinements

After the initial source audit, Windows CMake was tightened to require **Python 3.10 or later** (rather than silently skipping Python-dependent CTests). The Windows build script now enumerates CTests and refuses to release an artifact unless **at least 13 tests** are registered and all five named critical tests are present: the Win32 indexed-frame GUI self-test, the Win32 actual-media synthetic fixture, the Windows PowerShell ZIP integrity test, the real `GameSource` ISO/loose media CLI fixture, and the real reader/mod overlay adapter. **Only then** does it run the complete native CTest suite and create the archive.

The release script also requires the relevant tracked `NativeEngine` and inherited `GameSource` files to be clean against Git HEAD before embedding a source-revision label. The ZIP verification is performed against the **final compressed archive and extracted file bytes**, not just the build directory.

These are **source-level gates only** until Windows/MSVC compiles and executes them. The existing Linux CTest count cannot substitute for a green Windows build. [M-WIN-01 #12](https://github.com/Hyperbog/RLOFT/issues/12) remains open.

## First authentic original-asset presentation proof (follow-on, not gameplay)

The independent Win32 `rloft_native_bootstrap.exe` now links the *existing inherited, original-source-traced* `DemoV1/src/resource_assets.cpp` EH resource decoder as well as the `GameSource` install reader. [`NativeEngine/include/rloft_native/original_resource_preview.hpp`](include/rloft_native/original_resource_preview.hpp) implements one narrowly bounded source-backed diagnostic:

- Read original `INIT` bytes 0–1 to select the original CODE.1/CODE.2 default cursor `RES0[INIT word + 0x2F]`.
- Decode **EH type-1 raster** with the **existing certified** `ResArchive::decode_type1` implementation, not a second speculative decoder.
- Read the actual 15-bank original `COLORS` file and palette index 12 used by the original CODE.2 startup/menu path.
- Compose the decoded original cursor as a **centered diagnostic sample** in the immutable-format 320×400 indexed frame, skipping its original transparent index, then upload the shared CPU golden RGBA expansion to the Win32 GDI BGRA presenter. Source files are **read only**.
- Show explicit UI caption "**Original RES0 cursor + palette12 — DIAGNOSTIC ONLY**". If absent/invalid, revert to the clearly labeled synthetic reference. Neither path is falsely called the original title/menu, normal game boot, or gameplay.

[`NativeEngine/tests/original_resource_preview_tests.cpp`](tests/original_resource_preview_tests.cpp) generates a synthetic *original-format* 48-entry RES archive whose entry 47 is a valid 8×2 EH type-1 cursor. It opens this via the real inherited `GameSource`, decodes it through actual `resource_assets.cpp`, compares selected indexed pixels and palette-driven RGBA output, repeats to check deterministic pixel identity, and rejects a truncated COLORS bank. No copyrighted asset is embedded.

The Windows GUI headless `--self-test-source-cursor <original-media>` now **requires** a successfully displayed original resource rather than merely a positive RES0/INIT/T installation signature. The existing Python end-to-end fixture builds disposable RES0/INIT/T/COLORS assets and checks the GUI strict mode succeeds, detects corruption with exit code 4, and leaves media SHA snapshots unchanged. The CMake fixture target is `native_original_resource_preview_tests`; the Windows packager now requires **at least 14** registered CTests and explicitly checks that this original decoder fixture is present.

**Evidence classification:** Windows GUI/source integration **committed, not MSVC-compiled/tested**. Existing historical portable tests do not prove new Windows linker correctness or genuine Windows presentation. A genuine game installation needs its own protected-copy field test. Windows runner build remains undispatched because the connected GitHub integration does not expose workflow dispatch. The user authorized one Windows build; the direct workflow link remains https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml .

## Original static menu integration (October 11 continuation)

- [`original_menu_preview.hpp`](include/rloft_native/original_menu_preview.hpp) opens an independent **read-only** `GameSource` at the already identified original media path. It loads the **existing** `RavenloftBootAssets` and invokes `StartupPresenter::menu_frame(-1)` from the original-source-traced inherited `DemoV1/src/startup_presenter.cpp`. CODE.2 0x192CC composes RES0 **97..101** at exact startup-menu coordinates; the output retains the 320×400 palette-indexed buffer and original **COLORS palette12**. No interactive selection, runtime timer, CODE.1 state, saved game, or DOS execution is emulated by this static witness.
- Independent Windows `rloft_native_bootstrap` now **prefers the original static CODE.2 menu** when a complete verified-size media set loads, falls back to the original RES0 cursor diagnostic, and otherwise shows a distinctly labeled synthetic image. All three cases are explicitly marked non-playable. The window reports source-frame and DAC FNV64 fingerprints for patch/asset diagnosis. These hashes are **not** an original-DOS screenshot parity certificate.
- [`original_menu_preview_tests.cpp`](tests/original_menu_preview_tests.cpp) generates synthetic but authentic-format RES archives and a dummy *length-correct* CODE.1 (never executable content); it verifies original resource IDs 97..101, original draw coordinates, palette response, deterministic frame identity, and fail-closed missing required archive behavior.
- The GUI's strict `--self-test-source-menu` path is exercised by the Python original-media fixture against disposable RES/COLORS/INIT/T/CODE.1 images, including a missing-required-bank failure. `--self-test-source-cursor` was fixed to test cursor decoding **independently** even if the UI would prefer a valid full menu.
- The Windows release build now requires **at least 15 registered CTests**, including `native_original_menu_preview_tests`. No Windows build/run has yet been observed. The current environment has neither MSVC/Windows SDK nor a GitHub Actions dispatch capability, so the **owner-authorized single Windows build could not be executed here**; the manual-only Windows workflow remains available at https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml .
- A previously identified fidelity error was corrected: in inherited EH type-1 rasters, transparency key `0xFF` **disables** key-based transparency. The original cursor preview now treats indexed pixel `0xFF` as drawable in that case, matching `IndexedFramebuffer::draw_frame`, and its dedicated C++ regression tests that case. Windows autodiscovery errors also now remain in the desktop UI instead of aborting the application; the user can choose another install.

**Verification state:** changes are committed and source-wired, **not compiled/tested by MSVC or an actual Windows UI runner**. No original media, COPYRIGHT assets, playable-game claim, binary artifact or Linux-specific compatibility work was added. The normal-game menu and party creation remain distinct future execution goals; the static menu preview alone is **not game progress**.
