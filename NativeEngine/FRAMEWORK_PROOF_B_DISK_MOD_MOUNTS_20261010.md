# Framework Proof B — lazy read-only mod directories (2026-10-10)

**Branch:** \`native-engine\`. **Scope:** native framework diagnostics only; no authentic DOS gameplay, standalone GUI, SDL3 GPU backend, release mod SDK or Windows build. **Prior:** [Framework Proof A](FRAMEWORK_PROOF_A_CONTRACTS_20261010.md).

## Source change

The same \`ReadOnlyVfs\` from Proof A now accepts **mixed, ordered, read-only mounts**:

- \`mount(Overlay)\` continues to support in-memory test assets, preserving existing API.
- \`mount_directory(id, directory)\` recursively indexes filesystem files by uppercase, validated virtual path. The index records **relative paths, not content bytes**. Files are read lazily on demand, with a 256 MiB **diagnostic per-file maximum**, preserving original source reads unless a higher-priority mod owns the same path.
- Last mounted provider wins whether the layer is in-memory or on disk. \`providers(path)\` explicitly lists the ordered conflict chain through the unmodified original source.
- Rejects \`..\`, rooted/nonsensical paths, duplicate normalized names, duplicate mod IDs, \`CHARS\`/\`SAVE.DAT\`/\`GAME.OPT\` overlays, detected mount-time symlinks, and symlink replacements detected at read time. Deleted files fail **without silently falling back** to lower mounts. Only files under a selected mod directory are indexed; the original installation is never enumerated/written by this mod adapter.
- \`GameSource::contains/read\` remains the existing original-source entrypoint. Original image/loose file readers were not changed. The authored \`game_source_adapter_tests.cpp\` now also checks installed-original \`RES4\` untouched after a real directory override.
- This proof **does not** claim a hostile-mod security sandbox. Symbolic-link detection + opening are not atomic (TOCTOU); Windows reparse/junction behavior and Unicode/non-ASCII filenames need explicit native platform tests before enabling untrusted mods.

## Tests actually run

A local native-contract workspace contains the same committed \`NativeEngine\` header/test/CMake blobs, but **does not contain the repository's \`DemoV1/src/game_source.cpp\`**. The CMake project now explicitly checks its presence; in partial workspaces it emits a visible NOT REGISTERED message for the \`native_game_source_adapter_tests\` target rather than failing configuration or implying pass. **In a full repository checkout** that test is registered and links the inherited \`GameSource\` source (plus \`Advapi32\` on Windows).

Compiled and executed with **GNU C++ 14.2.0 / CMake Ninja**:

| Executed suite | Local Debug CTest | Local Release CTest |
| --- | --- | --- |
| \`native_framework_contract_tests\` | PASS | PASS |
| \`native_cpu_reference_presenter_tests\` | PASS | PASS |
| \`native_disk_mod_mount_tests\` | PASS | PASS |
| **Total executed** | **3/3 PASS** | **3/3 PASS** |

The file-mount test additionally passed \`-Wall -Wextra -Wpedantic -Werror -UNDEBUG -fsanitize=address,undefined\`. The test exercises two different folder priority layers plus memory mod and original fallback; no eager source reads; nested original-style paths; empty files; case-insensitive conflict, protected-save and traversal rejection; deleted-file fail-closed; symlink insertion and symlink replacement detection where the host permits symlink creation; and original source bytes unchanged.

**Source identity:**
- \`NativeEngine/include/rloft_native/vfs_contract.hpp\` blob \`0a02b1d7aa0e0f0c8332da3c152d0fa8ab610c22\`;
- \`NativeEngine/tests/disk_mod_mount_tests.cpp\` blob \`2238d5fbc0dbda4e7649536d9f60e3fa0e5cd7b0\`;
- \`NativeEngine/CMakeLists.txt\` blob \`a0d63a149e7b785453f32b39ae3902e92984fc58\`;
- \`NativeEngine/tests/game_source_adapter_tests.cpp\` authored updated \`d8ce0c78f466174b00d93ec57a701b6920f8f3b0\`.

The first three hashes were independently checked with local \`git hash-object\` and match remote exactly; the edited original-reader adapter test **is authored but not compiled in this partial workspace**. A full checkout should show four registered CTests, including the real inherited original-media adapter fixture; local partial checkout shows three, unambiguously. No original game binaries/media were bundled into Git.

## Next acceptance gates

1. On a complete checked-out repository, **compile and run all four** CTests using the actual \`GameSource\` implementation; independently test the ISO-9660 \`game.gog\` path and the user-provided installation (do not upload assets).
2. Build minimal SDL3+bgfx and SDL3+SDL_GPU GPU hosts feeding the already pinned \`IndexedFrame\` and \`CpuReferencePresenter\`. Gather actual GPU readback equality, window scaling, first-frame time, memory and backend coverage; do not decide renderer based on CPU tests.
3. Harden mod provenance: stable manifest IDs/load-order profiles, archive mount formats, limits/streaming, atomic file-open protection for untrusted mounts, signed/unsigned path differences, dependency checks, session and save isolation. Modded gameplay remains a later target subordinate to finishing the faithful original engine.
4. Continue [M-ENGINE-01](https://github.com/Hyperbog/RLOFT/issues/9): the actual separate GUI executable reading original install assets through verified core, with a **real player boot-to-first-blocker attempt**. Nothing in this proof is claimed to be a playable build.

**No Windows CI workflow triggered.** Existing DemoV1 unaffected, original-source/gameplay fidelity unchanged.
