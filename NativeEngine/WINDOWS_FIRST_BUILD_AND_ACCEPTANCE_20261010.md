# NativeEngine Windows-first x64 build and acceptance checkpoint

**Date:** 2026-10-10. **Owner:** `native-engine`. **Status:** Windows GUI bootstrap source + Windows-only manually triggered VS2022/MSVC workflow committed; **no verified Windows compilation, Windows GUI field test or released playable engine yet**.

## New direction from owner

Target **Windows x64 first**. Stop allocating active development/CI time to Linux portability, Linux-only packages, Linux performance benchmarking, or Linux-specific defects. Keep existing platform-neutral original-game behavior and file parsers as useful implementation boundaries, not as a competing shipping target.

## Delivered engineering changes

1. **Independent Win32 bootstrap** `NativeEngine/src/win32_bootstrap.cpp` → Windows target `rloft_native_bootstrap.exe`. It owns its own Unicode Win32 window, Windows folder selector, actual inherited `GameSource::open` and `auto_locate`, read-only `inspect_original_install`, error/status reporting, and **clearly identified synthetic** 320×400 `IndexedFrame`/6-bit DAC diagnostic. It expands through `CpuReferencePresenter` to BGRA32 and displays with Win32 GDI `StretchDIBits`, 4:3 viewport aspect and nearest-like display mode. GDI is a **temporary Windows bootstrap mechanism**, not the final SDL3/bgfx/SDL_GPU framework choice. The source game pixels, original saves and gameplay are never changed.
2. **Dedicated Windows CMake target** `rloft_native_bootstrap` (WIN32 only), MSVC C++17, packaged system Win32 libraries `User32/Gdi32/Shell32/Ole32/Advapi32`, runtime static link configuration, exact Git source-revision definition. Windows CTest `native_win32_bootstrap_headless_contract` uses `--self-test` to exercise the renderer-neutral indexed→BGRA conversion without opening a window.
3. **Actual install console inspector** `rloft_install_probe.exe`: preserved dedicated native media-reader target. Windows-specific `wmain(wchar_t**)` now supports Unicode installation paths without CP_ACP argv conversion. The synthetic loose/ISO integration fixture adds a Unicode directory path. **No original game files are embedded.**
4. **Single-step user build/packager** `NativeEngine/Windows/Build-NativeEngine-Windows.ps1`: VS CMake `-A x64` Release, full registered native CTest suite, GUI/headless smoke, snapshot exact source SHA, SHA256 of the two EXEs, full build manifest with explicit `game_playable=false`, and ZIP named `RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip`. Does **not** package proprietary assets or change Git automatically.
5. **Dedicated manual Windows-only workflow** [`.github/workflows/native-engine-windows.yml`](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/.github/workflows/native-engine-windows.yml) exists on **both** default `code1-decompilation` (for GitHub Actions workflow visibility) and `native-engine` (for source ownership). `workflow_dispatch` only, `windows-2022`, actions checkout fixed to the latest `native-engine` branch source, setup Python for synthetic fixture CTests, runs MSVC build+tests+ZIP and publishes an artifact. **No Linux job; no push/PR/release trigger; no bot binary commits; read-only contents permission.**

## Build it on a Windows developer machine

From an actual Git checkout of `native-engine` with VS2022/MSVC x64, CMake and Python installed:

```powershell
git switch native-engine
powershell -NoProfile -ExecutionPolicy Bypass -File NativeEngine\Windows\Build-NativeEngine-Windows.ps1
```

The package is written to `NativeEngine\Windows\dist\RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip` with a sibling SHA256 checksum. The script **must fail** on MSVC compile, CTest, GUI smoke, missing binary or failed ZIP. CMake test count may vary if Python is unavailable; GitHub workflow pins Python and should register all original GameSource tests plus the Windows smoke.

### Manual GitHub Actions build

Navigate to: https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml

Select **Run workflow** on the default branch (`code1-decompilation` workflow registration) and approve one manual build. Its job intentionally checks out **`native-engine`**, not DemoV1, and captures the exact commit SHA from that checkout. Once green, download artifact `RLOFT-NativeEngine-Windows-x64-BOOTSTRAP`. Do not assume that creating the workflow dispatched it. This project session's GitHub connector **does not expose workflow_dispatch**, so no run may be claimed without an independently observed run.

## What this Windows app can and cannot do

**Can (source wiring; needs Windows execution validation):** open normal own installation path, autodiscover supported GOG/loose sources, read presence of RES0/INIT/T and inventory additional resource names, present a labeled original-format indexed framebuffer diagnostic through Win32 GDI, show missing-file errors, and leave game directory untouched.

**Cannot:** boot the original title scene, create party, load Level22 world, render authentic RES sprites/world, run combat/quests/doors, or save/load original gameplay. In particular the explicit `game_playable=false` manifest ensures the bootstrap is **never** misrepresented as accepted `rloft_native.exe`. It does not repackage `ravenloft_demo_v1.exe` and should not replace the demonstration application.

## Gate status

| Gate | Evidence / status |
| --- | --- |
| Portable diagnostic original-format/media core | Earlier **10/10 CTests** Linux verified; retained as historical evidence, no new Linux work |
| Windows-specific CMake/GUI/bootstrap | **AUTHORED/COMMITTED** — no Windows SDK/MSVC available in this session to compile |
| Windows Actions build/workflow | **MANUAL WORKFLOW REGISTERED; NOT DISPATCHED** |
| New Windows CTest/UTF-16 path handling | **AUTHORED; NOT EXECUTED ON WINDOWS** |
| Windows ZIP and executable hashes | **PACKAGING SCRIPT AUTHORED; no actual ZIP or EXE until successful Windows run** |
| User original install field test | **NOT RUN** |
| True `rloft_native.exe` authentic game launcher/playability | **NOT IMPLEMENTED/NOT ACCEPTED** |
| Original DOS parity and complete-game original source fidelity | **NOT ACCEPTED**, mandatory existing charter remains unchanged |

**Next action:** run exactly one authorized manual MSVC Windows workflow. If it fails, retrieve the exact compiler/test log, fix Windows issues on the independent `native-engine` branch and retry only with explicit user approval. On a green build, verify the asset-free ZIP on a real Windows x64 machine against the user's original installation (prefer protected copy), then integrate source-faithful original resource rendering and authentic startup into the distinct native game engine. No Linux reimplementation effort is required for this gate.
