# Ravenloft: Strahd's Possession — Independent Native Engine

## VERIFIED Windows x64 independent native asset build — October 11

**[Download and verification evidence](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md):** `rloft_native.exe` was compiled on Windows Server 2022 using **MSVC 19.44**, all **9/9 Windows CTests passed**, and the asset-free ZIP was downloaded and independently checked. [Actual GitHub build #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). **Exact compiled Git revision:** `fd314b89e96db07c739176d25f1fc9be94ecaa19` (later README/docs HEADs are not compiled revisions). Native engine is not the demo recomp, and this build does **not** implement gameplay. First **real GOG asset-loader field test remains pending**, although an older diagnostic bootstrap did display real GOG menu artwork. Primary output: `RLOFT-NativeEngine-Windows-x64-ASSET-PREVIEW.zip` (no original files). ZIP SHA-256: `5ee2cdb58f3aa0ea10c715a2841b6be3eb0f411abb957132c4aa1150a857713c`.


**Authoritative direction (October 11, 2026):** [Native asset runtime architecture](ASSET_RUNTIME_ARCHITECTURE_20261011.md).

This is a NEW Windows replacement game engine, conceptually like OpenMW/KEX. It reads the player's **original game assets** and will eventually implement its own graphics, audio, game-state, world and gameplay systems. It is **not** the original-source decompilation/recompilation and does **not** link or execute the `demo-v1` game runtime. The working decomp demo continues as an independent project and can supply research/evidence about original formats and behavior.

## Active implementation — asset loading

- Native, read-only original media backend [`native_media.hpp`](include/rloft_native/native_media.hpp), [`native_media.cpp`](src/native_media.cpp): index DOS loose files or GOG `game.gog` ISO9660 (2048-byte data sectors, 2352-byte raw sectors with 16/24 byte user offsets), including nested asset folders, and **read original resources lazily** without unpacking or rewriting the disc image.
- Independent original format decoders [`native_assets.hpp`](include/rloft_native/native_assets.hpp), [`native_assets.cpp`](src/native_assets.cpp): RES offset tables/entry lookup and original `COLORS` 6-bit DAC palette banks. Report the presence, byte size, archive entry count and structural validity of 19 key assets.
- Windows desktop entry [`win32_native_assets.cpp`](src/win32_native_assets.cpp): **independent `rloft_native.exe`**, original-install folder picker, read-only inventory and original COLORS palette12 display. Optional headless `--verify-assets <owned install>` for actual original-media validation.
- Existing native, renderer-neutral indexed-frame, VFS mod mount/precedence and proof-contract foundations remain independent of the original runtime. No original game media or private saves are included in distributed outputs.
- **No full unpack/repack is required at runtime.** A disposable, versioned derived-resource cache can be evaluated later if profiling justifies it. Do not repack the game.gog or change installed originals.

### Windows-first build

The next **manual-only** Windows build is [Native Engine Original Assets — Windows x64](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml). It runs [`Build-NativeAssets-Windows.ps1`](Windows/Build-NativeAssets-Windows.ps1), builds the independent native reader, runs at least **nine** isolated native CTests (loose files, ISO sector variants, nested original directories, RES archive/COLORS decoding, native VFS/renderer contracts), and prepares an asset-free source-identified ZIP.

**Status:** this current independent asset engine source has been **committed but NOT yet compiled or executed under Windows MSVC**, and its real GOG asset validation is **NOT yet field-tested**. It must be built and checked against the user's original `game.gog` before acceptance. No Windows CI run has been dispatched for this revision. The previous Windows bootstrap's genuine **15/15 CTests** and the user's screenshot of its original static menu are real historical results, but that older bootstrap linked inherited decomp codecs and is **not proof that this new native engine loads assets**.

### Intended acceptance sequence

1. Compile and test the *independent* Windows `rloft_native.exe` on MSVC x64.
2. Open the user's original GOG/DOS install; validate actual `RES0`–`RES10`, `INIT`, `COLORS`, DAT and ANM assets and inspect original palette output. Keep the install read-only.
3. Add native, independently implemented RES EH sprite/texture/geometry, ANM cinematic, music/SFX and DAT world decoding; build a typed asset manager with resource-ID/version provenance and optional derived-data cache.
4. Implement original-source-faithful game semantics in **new** simulation code, original boot/world/input/save flows, and Windows rendering, comparing against evidence from DOS and the separate decomp demo without using either as a runtime dependency.
5. Establish gameplay fidelity/normal-world acceptance; then add renderer, modding and editor capabilities without changing original rules.

## Continuity and archival evidence

- [Current design and decision](ASSET_RUNTIME_ARCHITECTURE_20261011.md)
- [Native-engine developer instructions](AGENTS.md)
- [Windows native bootstrap build 15/15 (historical)](WINDOWS_VERIFIED_BUILD_20261011.md)
- [Real GOG installation static artwork test (historical bootstrap)](WINDOWS_GOG_FIELD_TEST_20261011.md)
- [Original CODE.1/CODE.2 decomp continuity](../Code1/DECOMPILATION_CONTINUITY.md) — **reference evidence, not native production dependency**
- [Native engine playable/runtime objective — Issue #9](https://github.com/Hyperbog/RLOFT/issues/9)
- [Native GPU comparison — Issue #10](https://github.com/Hyperbog/RLOFT/issues/10)

**Superseded:** `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` document an abandoned recompilation-based native integration and MUST NOT guide current implementation.
