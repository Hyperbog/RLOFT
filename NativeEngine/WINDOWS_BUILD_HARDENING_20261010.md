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
