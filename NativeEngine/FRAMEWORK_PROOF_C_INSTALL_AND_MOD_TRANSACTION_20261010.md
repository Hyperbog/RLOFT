# Framework Proof C — actual original GameSource integration and transactional mod plans

**Date:** 2026-10-10. **Branch:** `native-engine`. **Status:** independent Linux engineering preflight **compiled and tested**; Windows GUI engine, GPU framework benchmarks, genuine original install playthrough and DOS parity **NOT accepted**. **Earlier:** [Proof A](FRAMEWORK_PROOF_A_CONTRACTS_20261010.md), [Proof B](FRAMEWORK_PROOF_B_DISK_MOD_MOUNTS_20261010.md), [GPU implementation acceptance spec](GPU_BACKEND_ACCEPTANCE_SPEC_20261010.md).

## Batch 1 — actual inherited original-media reader

Prior Proof A/B could not compile `native_game_source_adapter_tests` because the local environment lacked a full repository checkout. For this batch the exact original `demo-v1` source files were independently transferred from the GitHub `native-engine` checkout into an isolated local build workspace. The local **Git blob hashes exactly match** the live repository:

| Inherited source | Verified Git blob |
| --- | --- |
| `DemoV1/include/rloft/game_source.hpp` | `273fb5a6fc931eef4ba3d896b67066ed7d339982` |
| `DemoV1/src/game_source.cpp` | `35e02a25ec015f930c6e480c998d3eb2c2bb68e7` |

**Not rewritten or mocked.** The linked code is the actual `GameSource::open`, `contains`, `read`, `auto_locate`, `Iso9660Image` reader, including original loose-media/game.gog logic.

The new `rloft_install_probe` **separate console executable** (`NativeEngine/src/install_probe_main.cpp`) links the authentic inherited source, checks original-media presence via the pure `install_probe.hpp` contract, and records expected original resource names *without copying or exporting copyrighted asset data*. It supports `--install <path>`, `--auto` and `--help`; error statuses for invalid install, incomplete minimum signature and invalid arguments. Its source revision is explicitly **`local-unversioned`** unless the build injects `-DRLOFT_NATIVE_SOURCE_SHA=<exact-checkout-SHA>`. The presence check reads only **RES0, INIT, T** and inventories 15 other candidate assets by name; **this signature is necessary for current loose-media detection, not sufficient to boot the full game**. Other assets, actual file layouts and retail patch versions are not comprehensively validated.

**CRITICAL PRODUCT DISTINCTION:** `rloft_install_probe` is **not** the intended playable `rloft_native.exe`, does not render, execute CODE.1/2 logic, save or load campaigns, or start a game. It is a reproducible independent installation reader/provenance preflight for M-ENGINE-01.

### Real-reader end-to-end synthetic installation test

`NativeEngine/tools/test_install_probe_cli.py` builds **disposable synthetic** source directories and ISO-9660 images entirely from generated, non-copyrighted bytes. The actual compiled `rloft_install_probe` is invoked as a subprocess. Checks cover:

- loose-media installation with lowercase file name and genuine real `GameSource::open` case-insensitive reads;
- `RLOFT_GAME_DIR` explicit autodiscovery;
- `game.gog` ISO with **2048-byte logical sectors**, and **2352-byte raw sectors with payload offsets 16 and 24**;
- opening both the install folder and the `game.gog` file itself;
- resource presence inventory and explicit **UNVERIFIED** version and **NOT YET PLAYABLE** status;
- zero-length and missing `T`, nonexistent install path, unsupported command option, help output;
- exact SHA-256 proof that the original-style installed loose files and ISO images remain unchanged.

A separate `native_game_source_adapter_tests` checks an actual original reader plus a real disk mod-folder overlay, and verifies that the base original `RES4` bytes remain unchanged.

**Actual local result:** test `INSTALL_PROBE_SYNTHETIC_MEDIA_PASS`; no proprietary file was required. **No original GOG/Steam/DOS installation loaded in this run**.

## Batch 2 — modding state and precedence

- `mod_load_plan.hpp`: strict namespaced ASCII mod-ID canonicalization, version string bounds, API version ranges, duplicate/missing/self dependency guards, topological ordering with deterministic user order among eligible nodes, cycle rejection, 256-mod/128-dependency diagnostic limits. Dependencies mount earlier/lower than dependents under existing last-mounted-wins VFS.
- `mod_mount_plan.hpp`: takes manifests plus real mod folders, validates the entire graph, then copies the current VFS mount metadata, stages every directory index in dependency order, and **commits the new in-memory VFS mount state only after all source folders pass**. A later invalid mount or circular graph leaves the previous mount state intact.
- `mod_load_plan_tests.cpp` / `mod_mount_plan_tests.cpp`: synthetic dependency graph, source conflict precedence, priority, missing IDs, duplicate IDs, invalid characters, API mismatches, cycles, rollback after a later invalid/protected file, and original media byte preservation.

**Boundaries:** This is an **in-memory transaction**, not an atomic snapshot of disk files, OS-level security sandbox or a production mod SDK. The backing filesystem can still mutate after mounting. No modded gameplay or Lua execution is enabled. Versioned manifest parser, profile persistence, save compatibility and hardened Windows reparse-point protections remain future tasks, **after faithful original-game reconstruction**.

## Compiled verification, exact source evidence

**Local toolchain:** GNU C++ 14.2.0, CMake 3.31.6, Ninja, Python 3.13, Linux execution. A compact local workspace contained source files whose Git blob identities matched live repository file refs, including the inherited actual `GameSource`. This does **not** mean a full repository clone, compiled Windows build or original DOS test.

| Test | Release CTest | Debug CTest |
| --- | --- | --- |
| Renderer-neutral indexed frame + virtual precedence | PASS | PASS |
| **Actual GameSource + disk mod adapter** | **PASS** | **PASS** |
| Independent installation-inspector help | PASS | PASS |
| CPU RGBA benchmark reference | PASS | PASS |
| Disk mod-folder safety/precedence | PASS | PASS |
| Strict synthetic GPU metadata comparator | PASS | PASS |
| Mod dependency planner | PASS | PASS |
| Read-only installation file inventory contract | PASS | PASS |
| **End-to-end actual-reader synthetic ISO/loose CLI fixture** | **PASS** | **PASS** |
| Transactional mod directory mount | PASS | PASS |
| **TOTAL** | **10/10 PASS** | **10/10 PASS** |

CTest wall times: Release **1.52s**, Debug **1.38s** on the local runner. Earlier eight- and nine-test intermediate reports have been superseded by this final ten-test state. The original `GameSource` + independent `rloft_install_probe` pair was separately compiled under strict `-Werror` and AddressSanitizer + UndefinedBehaviorSanitizer; its synthetic loose/ISO subprocess suite reported **ORIGINAL_GAMESOURCE_ASAN_UBSAN_PASS**. Transactional mod-plan test separately passed both sanitizers. Compiler warnings from incomplete test manifest initializers were corrected before the final builds.

Key new exact Git blobs checked in local workspace:
- `NativeEngine/CMakeLists.txt`: `55973800c5e66374b70e612d7e8c16bdcc15d3ee`
- `NativeEngine/src/install_probe_main.cpp`: `99795a96d2d65348b1b60bbafe294c10e49a07fa`
- `NativeEngine/tools/test_install_probe_cli.py`: `3bc80d33c74ab47f37308bcdab8378bd48658c73`
- `NativeEngine/include/rloft_native/install_probe.hpp`: `d4467379900991b11ac1949a7bc337f5c89278c7`
- `NativeEngine/include/rloft_native/mod_load_plan.hpp`: `3053d76c354ad2bc7f3eae3df603be9dfe9feda6`
- `NativeEngine/tests/mod_load_plan_tests.cpp`: `c818695628cceba0645d907ef7dc6acdafabf846`
- `NativeEngine/include/rloft_native/mod_mount_plan.hpp`: `59f6c052de83b22b141dd70927aaea0b0baeaef8`
- `NativeEngine/tests/mod_mount_plan_tests.cpp`: `87ae34570de4840f845e7628861d0937d986aa73`

### Reproducible checkout commands

```sh
cmake -S NativeEngine -B build/native-engine-preflight -DCMAKE_BUILD_TYPE=Release \
  -DRLOFT_NATIVE_SOURCE_SHA="$(git rev-parse HEAD)"
cmake --build build/native-engine-preflight --parallel
ctest --test-dir build/native-engine-preflight --output-on-failure
./build/native-engine-preflight/rloft_install_probe --help
./build/native-engine-preflight/rloft_install_probe --install "/path/to/owned/Ravenloft"
```

For Windows, use the generated `rloft_install_probe.exe` from the same target after a separately owner-authorized Windows build; Windows Unicode installation paths and junction/reparse behavior still require direct field tests. **Do not confuse the inspector with the game's executable or claim it can play.** The runtime game install and writable user profile must ultimately be isolated.

### Remaining hard gates

1. **M-ENGINE-01:** independently built `rloft_native.exe` that runs actual original startup/gameplay to first player blocker; current inspector does not meet this gate.
2. **GPU RFC-001:** actual SDL3 Renderer faithful-fallback, SDL3/bgfx and SDL_GPU GPU capture hosts on identical hardware, shaders, async readback and same indexed-palette source. No GPU measurements have been made.
3. **Original game media**: validate installed file sets/patch identity, unmodified assets and real saved-state behavior. Synthetic ISO fixture is not a retail installation test.
4. **Windows**: independent build and live host testing; manual GitHub Actions dispatch must be explicitly authorized per run. No Windows workflow dispatched in this checkpoint.
5. **DOS fidelity**: indexed pixels, palette/IRQ/timing/RNG and gameplay acceptance remain separate and unaccepted. This architectural tooling changes no original gameplay code, **no engineering-readiness score increase**.
