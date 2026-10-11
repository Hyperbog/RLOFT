# Framework proof A — renderer-neutral native contracts (2026-10-10)

**Owner:** `native-engine`, NativeEngine standalone workstream. **Scope:** platform-free, **diagnostic** contracts; NOT DOS renderer, real gameplay or definitive SDL3/bgfx selection.

## Landed code

- `include/rloft_native/frame_contract.hpp`: immutable-by-interface 320×400 indexed frame and 768-byte 6-bit DAC, fail-closed sizes/channel checks, separate FNV-1a64 diagnostic checksums of source pixels and palette, original 4:3 presentation rectangle, `IndexedPresenter` interface. It does **not** expand 6-bit DAC values or alter source render pixels.
- `include/rloft_native/vfs_contract.hpp`: read-only base media callbacks, deterministic last-mounted-wins overlay for **in-memory synthetic mod files**, uppercase/normalized virtual paths, reject traversal/duplicates, reject overlay writes to original runtime `CHARS`/`SAVE.DAT`/`GAME.OPT`, provenance listings. **NOT** file-backed mod loading, archive loading, production manifest or Lua.
- `include/rloft_native/game_source_adapter.hpp`: adapter from the existing original `rloft::GameSource::contains/read` APIs into the read-only base VFS. The caller must retain its `GameSource` object for the VFS lifetime. The existing original media reader already supports loose game directories and `game.gog` image; it was **not copied, edited or replaced**.
- `tests/framework_contract_tests.cpp`: synthetic indexed frame fingerprint determinism, palette-only mutation not altering pixel identity, 4:3 fit, invalid-frame/DAC rejection; overlapping mod order, provenance, protected runtime state, path traversal and case-alias collision failures, original byte preservation.
- `tests/game_source_adapter_tests.cpp`: authored, **NOT locally compiled yet**, fixture that builds a disposable synthetic loose-media directory, opens it using the actual existing `GameSource::open` and validates the adapter's reads. It is intentionally **not a gameplay installation**.
- `CMakeLists.txt`: separate native contract test project, independent of the DemoV1 executable and SDL/bgfx/Godot. Two Python-free C++17 targets and two CTests; the adapter target directly links the inherited `../DemoV1/src/game_source.cpp` and `../DemoV1/include`. No shipping executable and no demo binary relabeling.

## Actual validation, carefully scoped

Local GNU C++ 14.2.0, CMake/Ninja:
- `cmake -S NativeEngine -B build/native-contracts && cmake --build build/native-contracts && ctest --test-dir build/native-contracts` — **not** run against an entire local repo, because the source checkout is GitHub-connector-only; the local test sub-tree contains exactly the renderer-neutral contract and test files.
- The standalone `native_framework_contract_tests` was **built and passed 1/1 CTests** in local Debug/default and separately local `Release`, with assertions explicitly enabled via `-UNDEBUG`.
- A separate direct `g++ -std=c++17 -Wall -Wextra -Wpedantic -Werror -UNDEBUG -fsanitize=address,undefined` build of the exact same standalone contract test ran **ASAN_UBSAN_PASS**.
- **Git blob identities match byte-for-byte** for the locally compiled four files and remote `native-engine` repository source:
  - `CMakeLists.txt`: `243a53d5dc5c8370bddfd609d8400661a18b3b93`
  - `frame_contract.hpp`: `c04566babcfbe7eb61a4b76a18c2c834148ba187`
  - `vfs_contract.hpp`: `299cd2ddb042570ae8abaed7139e9a539e33fdf8`
  - `framework_contract_tests.cpp`: `8e4b10de00122d8e244632fa97ca6a9ac6655cb3`
- The adapter header and synthetic reader adapter test files also exactly match local source Git blobs, but the **actual `GameSource` integration target remains authored-only** because the original Git checkout is unavailable locally.
- **No SDL3/bgfx/SDL_GPU/Godot GPU backend was built or benchmarked**: those development libraries aren't installed in this environment. No actual original `RES4[655]` was GPU-presented. No Windows Full CI authorized/dispatched. No DOS runtime framebuffer/palette parity.

## Next bounded verification

1. On a full Git checkout, build **both** registered targets with actual inherited `GameSource`; fix any true portability issues without rewriting game logic. Run synthetic loose-media and ISO-9660 cases.
2. Select pinned dependencies and build **small separate, disposable** SDL3+bgfx and SDL3+SDL_GPU proof hosts against the same exact `IndexedFrame` identity and palette. Check GPU readback/visual and several window/aspect/fullscreen sizes. Build an independent Godot GDExtension comparison if tooling available. Measure GPU and CPU frame pacing, init time, code/runtime size and upgrade friction; select renderer only from evidence.
3. Extend VFS from in-memory proof to safe read-only file mounts, dependency/manifest validation, stable virtual data IDs and hermetic profiles. **Do not** activate new modded gameplay until the entire original execution baseline is accepted under the charter.
4. Progress M-ENGINE-01: a genuinely separate original-install-backed desktop engine executable and normal-path user playthrough. Present test coverage must not count as that milestone.

This is an **architecture integration checkpoint**, not the standalone playable engine, a released mod SDK, enhanced renderer or completion score.
