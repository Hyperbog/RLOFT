# NativeEngine — Active developer/agent instructions

## First verified independent Windows asset build — new accepted state

[WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md) is the exact execution evidence. **MSVC x64 compiled the independent `rloft_native.exe` from SHA `fd314b89e96db07c739176d25f1fc9be94ecaa19`**, **9/9 native Windows tests passed**, source-identified no-assets ZIP uploaded and its actual bytes downloaded/verified. [Windows Actions run #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). This replaces earlier **no native asset Windows compilation** claims; those were historical. The **real user-owned GOG installation has NOT yet been opened by this independent asset engine**, so field compatibility is still pending. Historical decomp bootstrap's original-menu screenshot does not verify this parser. Do not call any documentation-only later HEAD compiled. No new automated push triggers on main development branches; one authorized isolated build was used.


**Source of truth:** [ASSET_RUNTIME_ARCHITECTURE_20261011.md](ASSET_RUNTIME_ARCHITECTURE_20261011.md). Owner explicitly corrected architectural confusion on October 11, 2026.

## Two different products — DO NOT MERGE

- **`demo-v1` branch:** complete source-traced **decomp/recomp** workstream. Its intro cinematics, menu, character creation, DOS timing/animations and game controllers remain its own executable/project. It is allowed as **research and DOS-fidelity evidence**, not as a production dependency of `NativeEngine`.
- **`native-engine` branch:** wholly new, standalone replacement engine akin to **OpenMW/KEX**. Its executable must load original user-owned file data and independently implement rendering, gameplay, physics/world, audio, scripting and saves. **Do not link `DemoV1/src/win32_main.cpp`, 176-file game core, BootFlow/CharacterCreationFlow or any original DOS module** into `rloft_native.exe`. Do not copy/rename a decomp demo EXE or construct another old-code wrapper.

**A previous attempt to compile the decomp demo as native was mistaken and retired.** The modified DemoV1 CMake/host/media files were restored to previously verified blobs. Old docs `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` are historical, **superseded**. The previous Windows bootstrap (15/15 actual CTests; original GOG screenshot) is valid archival evidence but used decomp resource parsers; **its results do not certify the new independent native asset reader**.

## Active platform and file I/O decisions

- **Windows x64/MSVC first**. Do not spend new effort on Linux porting, Linux CI or Linux packages. Keep C++17 renderer/gameplay boundaries portable by design only where that does not delay Windows.
- **Read original media directly, lazily, read-only.** Mount original DOS loose files or GOG `game.gog` ISO9660; never modify installed files, never bundle game assets, never run CODE.1/CODE.2 executables or game.gog under DOSBox. Full installation unpack/repack is NOT a runtime requirement.
- **Native original asset path:** `native_media.hpp/.cpp` (ISO sector indexing/loose media/unique DOS paths), `native_assets.hpp/.cpp` (RES index/entry and COLORS 6-bit palette), `win32_native_assets.cpp` (independent Windows UI and user-owned GOG validation). `NativeEngine/CMakeLists.txt` contains **no DemoV1 source paths** and builds `rloft_native.exe` from these NEW sources.
- **Future asset cache** (optional): separate user-owned profile, disposable, keyed by source-byte hash + independent codec revision + resource ID; never replace the original ISO and never commit derived copyrighted resources. Mod VFS must layer above the immutable base, no silent save-file fallback to purchased installation.
- **Type/format validation**: bounded reads, case-insensitive DOS names, no path traversal, duplicate resolution fails closed. Tests must use synthetic copyrighted-free ISO2048 and raw 2352/offset16/24 as portability baselines, and a **REAL original owned GOG installation on Windows** for acceptance. Tests that only run old decomp source do not count toward native asset validation.
- Decode original assets as independent typed data: RES graphics/texture/scene/audio, `COLORS` palette, ANM video, DAT world/objects/NPC, music; add original gameplay semantics in new runtime once data is proven. Do not fake data, convert original game code into game loop, or overwrite retail saves.

## Builds, progress and release honesty

Current **only active Windows native-engine workflow**: [native-engine-assets-windows.yml](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml), **manual-only** on default and native-engine branches. [Build-NativeAssets-Windows.ps1](Windows/Build-NativeAssets-Windows.ps1) builds with VS2022/MSVC x64, requires **at least 9** source-independent native CTests, verifies ZIP and EXE SHA256 and packages only four engine-owned files. Source output marks `engine_stage=NATIVE_ASSET_LOADING`, `gameplay_verified=false`.

**Current validation state:** independent source implementation and tests committed; **NO new Windows/MSVC compile or user-owned original installation field test has run for this new engine.** The old 15/15 Windows result is for the retired bootstrap. Do not increment gameplay completion, represent original ANM/DAT decode as implemented, or reuse old binary/artifact. Windows CI runs require an explicit new user instruction; `continue` does not automatically authorize a new run.

For any substantial task: inspect live HEAD/branch differences, original Code1/Code2 archaeology, applicable continuity, and source data; use separate self-contained native source/CTest improvements, commit with `[skip ci]`, report actual evidence and remaining blockers; preserve decomp/demo changes on its own branch.

**Next exact gate:** build native asset engine under Windows; have owner select real `game.gog`, validate 19 named original sources/RES entry tables/palette12; then implement genuine independent EH sprite/ANM/DAT decoders and rendering. Focus on original-data fidelity before graphical modernization or mod extensions.
