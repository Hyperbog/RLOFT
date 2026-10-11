# Native-engine reset — independent original-asset runtime (October 11, 2026)

**Authoritative project correction from owner:** There are TWO SEPARATE projects. The `demo-v1` branch is a faithful original-source **decompilation/recompilation** with its already-working cinematic, menu, and character creation. The `native-engine` branch must be a NEW replacement game engine **akin to OpenMW or the KEX model**: load ORIGINAL user-owned data, reconstruct the game's behavior independently, permit future renderer/engine upgrades, and NEVER make the old executable or demo game loop a runtime dependency.

## Selected data strategy

**Default: index original game media and read assets lazily in place. NO mandatory full unpack or repack.** This works with user-owned DOS loose directories and GOG `game.gog`. Disks stay read-only. Reasons:

- Original media remains byte-exact and independently verifiable.
- Initial launch need not copy gigabytes of files; indexing directory entries is fast, resource payloads load only when requested.
- RES archive offsets, palette bytes, ANM cinematics, DAT worlds, sound and music can each be exposed through independent typed decoders and cached after decoding.
- Mod folder/virtual overlay resolution can be layered above an immutable original volume. No need to rewrite copyrighted original bundles.
- Later, **optionally** add an engine-owned, versioned *derived decode cache* under the native profile, keyed by original-resource hash, codec revision and output format. The cache must be disposable/rebuildable, never trusted as authoritative game data and never written to the install.
- A separate asset export/import/repack utility may be useful for mod creators and archival verification. It must not be a runtime requirement.

### Actual source changes

1. `NativeEngine/include/rloft_native/native_media.hpp` + `src/native_media.cpp`: independent native media backend (**no DemoV1 includes or links**). Case-insensitive DOS key normalization, path traversal/collision guards, native loose-directory indexing, lazy indexed ISO9660 original-data access, 2048-byte sectors and raw 2352-byte sectors with offset 16 or 24, nested ISO directory support, strict unique basename lookup for DOS files found under `RLOFT/`. Bounds-checks extent/range and per-resource maximum 256 MiB. No extraction or writes.
2. `NativeEngine/include/rloft_native/native_assets.hpp` + `src/native_assets.cpp`: original `RES` count + monotonic offset table + entry extraction and original 15-bank `COLORS` six-bit palette decoder. Read-only inventory/structural validation of **19 known resource filenames** with file sizes, valid/missing/corrupt status and RES entry counts. This is structural inspection, **not yet decoding every art, cinematics or world command type**.
3. `NativeEngine/src/win32_native_assets.cpp`: a new independent `rloft_native.exe` Windows desktop asset inspector. Select the original installation folder and examine real asset inventory; display the actual **256-color palette from original COLORS bank 12**, not synthetic source art. The headless `--verify-assets <path>` mode validates INIT, RES0 archive and COLORS/T from user-owned installed media, without original asset export.
4. `NativeEngine/tests/native_asset_media_tests.cpp`: independently generated, copyright-free structural tests covering loose DOS files, ISO2048, ISO2352+offset16/24, nested `RLOFT/` original directory, RES entry offsets, palette DAC6 range, malformed archive and unsafe-path rejection, and unchanged input media.
5. `NativeEngine/CMakeLists.txt`: fully independent native target/test build. **No `../DemoV1` CMake source, no linked 176-file decomp library, no original DOS binaries, no old `win32_main.cpp`, no demo controller reuse.** Existing native VFS, render and mod-planning contracts retained.
6. `NativeEngine/Windows/Build-NativeAssets-Windows.ps1` + `README-NATIVE-ASSETS.txt`: Windows-only release recipe. Require **nine** independent CTests, build independent target, use SHA256/provenance, package only four allowlisted files (EXE, README, checksums, manifest), reopen final ZIP and confirm executable bytes. `engine_stage=NATIVE_ASSET_LOADING`, `gameplay_verified=false` and `original_assets_included=false`.
7. [New manual-only Windows asset workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml) is registered on default and `native-engine`. Previous live-demo and old-static-bootstrap workflows were removed from current branches to prevent the wrong Windows product build.

### Unwinding the prior architectural mistake

The **wrong** step compiled the decomp demo's `win32_main.cpp`, 176 source files and original CODE.1/CODE.2 execution controllers into `rloft_native.exe`. That would produce an altered recompilation, **not** the replacement engine requested by the owner.

That native target has been replaced. The four DemoV1 source/build files modified by that integration (`CMakeLists.txt`, `win32_main.cpp`, `game_source.cpp`, and `game_source.hpp`) were restored exactly to their previously verified Git blobs at `1f8287f61346c1143b3e9348342a3691a487df4a`; the experimental native demo wrapper, duplicate shared source manifest and added demo-branch research mirrors were retired. The real decomp demo continues evolving separately on `demo-v1`. Its file-format archaeology can guide independent native decoders, but its gameplay binary/library is NOT part of the native runtime.

The older [LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md](LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md) plan is **SUPERSEDED and not an active implementation path**. The earlier successful 15/15 Windows bootstrap and user's actual original GOG static-menu screenshot remain useful archival evidence, but that old bootstrap linked inherited decomp codecs/presenters and does not certify this independent asset engine.

## Acceptance plan — real original asset loading first

1. **Independent MSVC x64 build** of `rloft_native.exe` via the new manual workflow, with native CTests green and asset-only ZIP verified. **No new Windows Actions run has yet been dispatched** for this target; compilation is NOT certified merely by committing sources.
2. **Owner original GOG test**: choose installed `game.gog`, verify source ISO sector mode, inventory of actual files, RES record counts, `COLORS` bank 12 displaying the original palette and no invalid required asset. The user should report the first validation error. CI synthetic ISO tests do not replace this owned-media test.
3. Record original file hashes before/after if proving immutability is needed. Default engine opens media **read-only** and never writes `game.gog` or installed loose assets.
4. Implement independent typed native decoders incrementally: RES EH raster types/texture/geometry, ANM video and audio, DAT level/world/objects/NPC tables, CODE.1/CODE.2 source-derived gameplay *semantics* **without running the old modules**.
5. Render source-decoded art and cinematic frames through a new Windows graphics host, integrate original world simulation, transitions, party gameplay and saves in an engine-owned writable profile. Compare against DOS/decomp reference evidence when necessary, but do not make either game runtime a dependency.
6. Once working, benchmark and select Windows SDL3 Renderer/bgfx vs SDL_GPU using actual frame/palette readbacks. Mod profiles and editor extensions remain layers above original faithful behavior.

**No decision to bulk unpack/repack original game media has been made:** lazy read-only source mounting is the default and should change only if a measured loading or modding requirement justifies it.
