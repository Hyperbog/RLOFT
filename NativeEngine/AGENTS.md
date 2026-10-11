# NativeEngine contributor/agent continuity

**Branch owner:** `native-engine`; do not mistake the inherited `DemoV1/` implementation or its developer-only `--explore-level22` path for this product. Read [README.md](README.md), [Proof C](FRAMEWORK_PROOF_C_INSTALL_AND_MOD_TRANSACTION_20261010.md), [GPU acceptance spec](GPU_BACKEND_ACCEPTANCE_SPEC_20261010.md), and [original execution charter](../DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) before changing native gameplay. Consult the `Code1/` and `Code2/` original-source continuity and immutable original game bytes before any game-facing implementation.

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

## Quick local commands (full Git checkout)

```sh
cmake -S NativeEngine -B build/native-preflight -DCMAKE_BUILD_TYPE=Release \
  -DRLOFT_NATIVE_SOURCE_SHA="$(git rev-parse HEAD)"
cmake --build build/native-preflight --parallel
ctest --test-dir build/native-preflight --output-on-failure
./build/native-preflight/rloft_install_probe --help
```

**Next two gates:** (1) isolated SDL3 Renderer / bgfx / SDL_GPU GPU proof builds with readback and shader acceptance, then genuine installed-asset normal-game launcher work; (2) independent source-faithful executable `rloft_native` player boot-to-first-blocker test, tracked by [M-ENGINE-01](https://github.com/Hyperbog/RLOFT/issues/9). Treat the fidelity charter's Level22 acceptance sequence as binding for game work.
