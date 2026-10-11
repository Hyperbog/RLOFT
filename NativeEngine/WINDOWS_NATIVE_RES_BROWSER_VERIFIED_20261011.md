# Windows native RES image browser — independently compiled and verified

**Date:** 2026-10-11. **Product:** independent OpenMW/KEX-style original-game asset runtime. No game source decomp execution and no required bulk unpack.

## Actual MSVC/Windows evidence

**Build A: first native EH1 graphics decoder**

- [GitHub Actions #38168263623](https://github.com/Hyperbog/RLOFT/actions/runs/38168263623), Windows Server 2022 / VS2022 x64.
- Initial job FAILED due to an uncaptured sample pixel vector in a C++ **test lambda**. The native asset library and `rloft_native.exe` both compiled; the test target did not.
- Fix committed; second attempt compiled all targets, passed **10/10 CTests**, verified distribution ZIP and uploaded [artifact #11713200511](https://github.com/Hyperbog/RLOFT/actions/runs/38168263623/artifacts/11713200511).
- Compiled source commit: `cfa3fe23602270026ab7a154d5504ee1f1f0360e`.
- EXE SHA-256: `e2aaf6229a651f3c320b4290271af3a98d91a25556d6989e3f0f8aafdaeb6d7d`.
- User-distribution ZIP SHA-256: `3ea5078560c876d2ec8cd2a7a599bf2c59f443ac886b52cc380ac18b87fde02c`.
- Native EH1 parser newly decodes original raw and 4KiB LZSS type-1 RES images, handles source `INIT` default cursor record, transparency and bounded sizing, and can display indexed pixels via native Win32 GDI.

**Build B: original RES browser across source archives**

- [GitHub Actions #38168898735](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735), Windows Server 2022 / VS2022 x64, completed **SUCCESS**.
- Exact compiled Git SHA: **`d29914503fb6d6bb3a65954257d0ac145f0ceaf0`**.
- **11/11 CTests passed** in 1.14 seconds, including new `native_raster_browser_tests`, previous EH1 raw/LZSS tests, native ISO image fixtures, rendering contracts, mods/VFS and the independent Windows executable gate.
- [GitHub artifact #11712864857](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735/artifacts/11712864857), independently downloaded and inspected.
- GitHub outer artifact ZIP SHA-256: `ecd067c4181d32edd19d98fe1097371781eab91a45198c1cc0f4abaaae460de4`.
- Inner user ZIP SHA-256: **`8e578ac9acfefb0ceae498b389003314b5f2e632694133ec05b0988710033cca`** (matches ZIP sidecar).
- Embedded `rloft_native.exe` SHA-256: **`9abc292c71b05e2a19f38b78386914e442c95bb0fca01e26501f03e7b5047f76`** (matches inner SHA256SUMS).
- Original Windows PE machine **AMD64 0x8664**, subsystem **Windows GUI 2**. Original source assets are NOT included: the inner package has exactly four files (EXE, README, checksums, provenance) with `original_assets_included=false`, `gameplay_verified=false`.

## User-visible functions added beyond previous original-asset inspector

The independent native Windows GUI can now select source archives `RES0`–`RES8` and `RES10`, specify a **real numeric original entry index**, and request the exact EH type-1 raster. **Previous / Next image** navigation skips other original resource types and preserves actual source record numbers. Malformed EH type-1 records fail closed rather than being silently skipped. Raster pixels are colored through the user-owned source palette 12 and displayed via GDI. **All original data is read-only; no bulk extraction or repack.**

The original cursor selected by `INIT` remains the default image if available; optional browsing errors do not falsify the 19-file source structural inventory. The browser uses independent `NativeResArchive`, `NativeMedia`, and the independent `decode_native_type1_raster`—it does not compile or link a DemoV1 codec, presenter, game core or executable.

## Verification scope and the next useful field test

- **ACCEPTED:** actual Windows compiler/link/test/package execution for both new source revisions, verified release bytes and original-game-media-free distribution.
- **ACCEPTED FROM PREVIOUS USER SCREENSHOT ONLY:** original owned GOG `game.gog` ISO2048 opened, 69 files indexed, 19 known resources structurally valid and COLORS palette12 visible. That was source `fd314b89`, **not** this compiled browser build.
- **NOT YET USER-VERIFIED:** the new `d2991450` EXE displaying actual owned-GOG EH1 raster pixels and browsing forward/back through real multi-frame original source records.
- **NOT CLAIMED:** original DOS raster parity, full texture/scene decoding, all EH types, ANM, DAT, audio, gameplay or original save handling.
- **Next:** user extracts this ZIP, runs `rloft_native.exe`, opens same original GOG folder, and confirms both the default EH1 image and navigation (e.g., `RES0`, Next image). Capture browser result or decoding message, then implement other typed native resource decoders as proven by real data.

A one-time Windows-only trigger branch was used for each expressly owner-authorized build. The normal development/default branch workflows remain manual-only; no permanent push triggers were added. Documentation changes after source SHA `d2991450` are **not** part of that compiled binary.
