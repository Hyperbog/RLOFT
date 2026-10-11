# NativeEngine contributor/agent continuity

**Branch owner:** `native-engine`; do not mistake the inherited `DemoV1/` implementation or its developer-only `--explore-level22` path for this product. Read [README.md](README.md), [Proof C](FRAMEWORK_PROOF_C_INSTALL_AND_MOD_TRANSACTION_20261010.md), [GPU acceptance spec](GPU_BACKEND_ACCEPTANCE_SPEC_20261010.md), and [original execution charter](../DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) before changing native gameplay. Consult the `Code1/` and `Code2/` original-source continuity and immutable original game bytes before any game-facing implementation.

## Windows-first platform directive (2026-10-10)

**Windows x64/MSVC is the sole active desktop build and release gate.** Do not spend new effort on Linux compatibility, Linux CI, Linux packaging, Linux port-specific bugs or Linux performance benchmarks. Existing portable C++ game logic, source readers and previously executed Linux tests remain useful historical evidence, not new deliverables.

The separate `rloft_native_bootstrap.exe` Win32 **media/viewport diagnostic** target and `rloft_install_probe.exe` Windows console inspector are registered, alongside the dedicated `NativeEngine/Windows/Build-NativeEngine-Windows.ps1` script and manual `.github/workflows/native-engine-windows.yml` workflow on default branch and `native-engine`. **No accepted Windows build/exe exists until actual MSVC compilation/CTest and artifact checks are observed**; the bootstrap is not the playable `rloft_native.exe`. See [Windows-first acceptance](WINDOWS_FIRST_BUILD_AND_ACCEPTANCE_20261010.md).

## Windows-first hardening continuation

**Current Windows acceptance tracker:** [M-WIN-01 issue #12](https://github.com/Hyperbog/RLOFT/issues/12) and [Windows build hardening checkpoint](WINDOWS_BUILD_HARDENING_20261010.md). The modern Unicode `IFileOpenDialog` bootstrap path, RGB→BGRA source-invariant test, hidden `--self-test-install` real `GameSource` fixture, all critical PowerShell ZIP/package checks and Windows-only CTest registration are **authored/committed**. **NO actual Windows compilation or executable artifact is verified.**

The Windows build now **requires Python >=3.10**, must enumerate **at least 15 CTests**, including the actual original-format RES0 EH cursor/COLORS decoder fixture including five mandatory Windows/original-media/package checks, does not permit releasing with skipped tests, fails on tracked dirty source and verifies ZIP/archive source/EXE hashes after building. Do not reduce these gates to make CI green or treat synthetic window output as actual game graphics. [Manual Windows workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml) remains dispatched only with explicit owner approval. No Linux work shall be added.

## First source-backed Windows image boundary

[`original_resource_preview.hpp`](include/rloft_native/original_resource_preview.hpp) binds original `GameSource` RES0/INIT/COLORS to the existing certified `ResArchive::decode_type1`; it does **not** implement a new RES raster parser. It composes the authentic default cursor into a clearly marked diagnostic 320×400 output and feeds the existing BGRA presenter. The GUI retains a synthetic fallback for unsupported/malformed original asset banks. `native_original_resource_preview_tests` and Win32 `--self-test-source-cursor` exercise a disposable original-format RES bank with real parser code. These changes are **not MSVC compiled until Windows CI runs**; do not count them as original title/game rendering or full DOS accuracy.

## Original static CODE.2 menu preview (2026-10-11)

[The native menu preview interface](include/rloft_native/original_menu_preview.hpp) now invokes the **existing** inherited `StartupPresenter::menu_frame(-1)` after loading `RavenloftBootAssets` from a read-only original `GameSource`. This composites original RES0 IDs 97–101 with palette12 into the native 320×400 indexed frame and does **not** create an interactive menu or change game state. The new Windows preview selects this source-traced static composition if available, otherwise falls back to the original RES0 cursor or synthetic diagnostic. Its frame/palette FNV64s are displayed as diagnostic fingerprints only. The original presenter is linked as its existing file `DemoV1/src/startup_presenter.cpp`; no duplicate renderer code.

[`native_original_menu_preview_tests`](tests/original_menu_preview_tests.cpp) uses disposable synthetic original-format RES and required CODE.1-sized media; [headless GUI `--self-test-source-menu`](src/win32_bootstrap.cpp) is a strict pass/fail path exercised by the ISO/loose installation Python fixture. It is now one of the **15 mandatory registered Windows CTests**. All new source is authored/committed; **Windows/MSVC build and original-media field test remain unrun** until manual CI invocation. No new Linux compatibility work.

## Windows first-run blocker repaired (2026-10-11)

The Windows release verifier had inadvertently accumulated **three duplicated copies** and broken `-cmatch` expressions. `NativeEngine/Windows/Verify-NativeEngine-WindowsPackage.ps1` was completely rewritten to a **single ZIP SHA256/allowed-entry/source-provenance/two-EXE-hash checker**; `Test-NativeEngine-WindowsPackage.ps1` now exercises a synthetic valid ZIP plus **four malicious/tampered ZIP variants**. These are **source-authored**, structurally reviewed only — **PowerShell has not run**, MSVC/Windows SDK is absent here, and the connected GitHub app cannot dispatch new workflow runs. Do not repeat previous claims that this ZIP verification passed on Windows.

The owner **explicitly authorized one Windows build** on October 11, but no GitHub runner could be dispatched from available tools. The manual workflow [native-engine-windows.yml](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml) remains the only approved execution path; do **not** add unapproved push-trigger/issue-trigger CI workarounds. A future verified run must compile `native-engine` on VS2022 Windows x64, execute **15+ CTests**, produce the two real .exe files, pass the package verifier, and publish checksums. Never count authored CI steps as successful results.

## Exact work order

1. Check **live Git HEAD** and all files modified by parallel agents; use blob-SHA-guarded edits, bounded commits and no force updates. Record actual build revision and original-file patch identity. Do not overwrite parallel work.
2. **Faithful game reproduction first**, no arbitrary new gameplay, invented art/content, debug-only warp as normal progression, new gameplay feature or modern render replacement of the original simulation. Separate tools and architecture infrastructure from source-traced gameplay.
3. Independent Windows player application target is **planned `rloft_native.exe`**. Current **`rloft_install_probe` is a CLI media preflight**, not a game. It can operate on a user's installed loose-media folder or supported `game.gog`; it is not allowed to write to original installs. Retain separate writable profiles when authentic saves are eventually wired.
4. **Current local evidence:** Linux GNU C++ CMake/Ninja `NativeEngine` subproject **10/10 CTests in Debug and Release**, including authentic SHA-pinned inherited `GameSource` and a synthetic ISO/loose reader fixture. No Windows Full, purchased media, genuine DOS parity, compiled SDL3/bgfx/SDL_GPU/Godot or field playthrough acceptance. Earlier proof reports retain historical counts.
5. Keep **C++17 simulation, original GameSource and immutable data formats** independent of renderer, GPU shader SDK, Lua/plugin interpreter, mod loader and future editors. `IndexedFrame` 320×400 original bytes and DAC6 are authoritative *inputs*; separate host RGBA golden conversion is diagnostic and not original analog DAC fidelity.
6. Mod VFS/planner is architecturally available for synthetic diagnostics; **no modified gameplay enabled** before complete original-game recreation. In-memory transaction does not guarantee filesystem race safety. Versioned file schemas, native OS path isolation, mod save migration and scripting sandbox are pending.
7. For a framework backend decision, follow **GPU readback source/fence constraints**, exact CPU golden pixel matching and same-hardware measured metrics. Do not turn synthetic comparator inputs into published GPU results or claim a renderer is selected.
8. **Windows GitHub Actions Full builds are manually dispatched only with explicit owner approval per run.** Documentation changes, user "continue" or source commits do not themselves authorize CI. No proprietary original game files or private saves are committed.
9. Keep a reproducible acceptance ledger: source evidence / authored regression / actual local compiler+CTest / actual Windows host / actual owned-install gameplay / original DOS comparison are six separate statuses. Do not increase complete-game readiness for a source-only tooling checkpoint.

## Windows x64 build commands (full Git checkout)

```powershell
git switch native-engine
powershell -NoProfile -ExecutionPolicy Bypass -File NativeEngine\Windows\Build-NativeEngine-Windows.ps1
```

The [dedicated manual GitHub Actions workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml) builds `native-engine` using MSVC x64, executes all registered Windows CTests, verifies the GUI/headless smoke test and uploads an asset-free ZIP with exact Git source SHA and executable hashes. It never runs automatically. Do not report an artifact as existing unless the run has succeeded and its artifact identity was verified.

**Next two gates:** (1) isolated SDL3 Renderer / bgfx / SDL_GPU GPU proof builds with readback and shader acceptance, then genuine installed-asset normal-game launcher work; (2) independent source-faithful executable `rloft_native` player boot-to-first-blocker test, tracked by [M-ENGINE-01](https://github.com/Hyperbog/RLOFT/issues/9). Treat the fidelity charter's Level22 acceptance sequence as binding for game work.
