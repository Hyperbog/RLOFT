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

## Proof A follow-up: canonical CPU RGBA reference and Windows adapter linkage

Added `include/rloft_native/cpu_reference_presenter.hpp` and
`tests/cpu_reference_presenter_tests.cpp`. This is a **benchmark-only CPU
reference**, not DOS palette gamma or an enhanced gameplay renderer. It
converts 6-bit RGB through a documented host rule
`(channel*255+31)/63`, expands each original 320×400 indexed pixel into
RGBA8 (alpha 255) and provides a deterministic FNV-1a64 RGBA fingerprint.
This makes future SDL_GPU, bgfx and Godot GPU readbacks **comparable to an
identical defined output**, including palette-only changes that must not
alter the underlying indexed source buffer. It does not claim the original
VGA analog/display phosphor transfer function.

Local GNU C++14.2.0 direct exact-source builds:

| Contract | Default C++17 `-Werror -UNDEBUG` | Release `-O3 -Werror -UNDEBUG` | ASan/UBSan |
| --- | --- | --- | --- |
| `framework_contract_tests` | PASS | PASS | PASS |
| `cpu_reference_presenter_tests` | PASS | PASS | PASS |
| `game_source_adapter_tests` (real inherited source) | NOT BUILT locally: complete Git checkout unavailable | NOT BUILT | NOT BUILT |

There are now **three authored CTest targets** in the `NativeEngine`
subproject, **not three locally executed CTests**. The first frame/VFS
CMake/CTest ran 1/1 in local default and Release configurations before
registering the adapter and CPU-target additions. The two independent
CPU/reference executable regressions subsequently passed via direct
compiler invocations (2/2) in both configurations. Thus full
`NativeEngine` three-target CTest on its **current SHA is still unrun**.
Additional `Advapi32` linking was authored for the inherited Win32
`GameSource` GOG-registry probe; Windows compilation remains unrun.

New exact Git blob hashes matched locally:
- `cpu_reference_presenter.hpp`:
  `fc87d0afef8901ec86fc9447ef182c84c5894300`
- `cpu_reference_presenter_tests.cpp`:
  `d8c706c3e584b10e7f90a973345666710181efc0`

No SDL3, bgfx, SDL_GPU or Godot runtimes are currently installed in
the local container. Do not upgrade the renderer selection or engine
playability claim based on CPU-only proofs.
