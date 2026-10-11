# Independent native EH1 raster decoding — source checkpoint

**Date:** 2026-10-11. **Branch:** `native-engine`. **Status:** source/authored tests committed, **Windows MSVC build and original-image field test not run** for this decoder.

## Inputs and design

The owner field-tested the **previous compiled native asset loader** against their own GOG `game.gog`; the screenshot reports ISO2048, 69 source files indexed, **19/19 candidate resources validated, 0 missing/invalid**, and actual COLORS palette 12 visible. That remains valid evidence for original-media indexing and structural data validation. [Real GOG evidence record](WINDOWS_REAL_GOG_ASSET_FIELD_TEST_20261011.md).

This milestone builds the next **typed, independent native graphics codec**: EH type-1 raster records from an original RES archive, **without compiling, linking or calling DemoV1**.

- `NativeEngine/include/rloft_native/native_assets.hpp` declares `NativeType1Raster` (count, width, height, transparency key, palette-indexed pixel buffer), `decode_native_type1_raster`, and `load_native_default_cursor`.
- `NativeEngine/src/native_raster.cpp` implements original EH type-1 header validation (signed count/height, width in 8-pixel units), **uncompressed payload** and **4 KiB LZSS compressed payload** decoding, original 0xFEE dictionary start/initial FE region, fail-closed truncated literals/references and undefined history, 32 Mi-pixel decoded safety cap, and one-row CODE.1 descriptor transform semantics. It does not share a source translation unit or library with the decomp demo.
- `load_native_default_cursor` reads the **original INIT first word** and selects `RES0[INIT word + 0x2F]`, through the existing independent `NativeMedia` and `NativeResArchive` interfaces. No extraction/repack or installed-game write.
- `NativeEngine/src/win32_native_assets.cpp` now optionally attempts that exact original default cursor after source validation, creates a BGRA8 buffer using palette12 and shows a zoomed native GDI diagnostic preview below the source palette. An image's original transparent key is honored; key 0xFF **disables** transparency testing. Optional raster failures are reported separately from the original 19-file structural inventory; they do not relabel structurally valid installation files as invalid.
- Windows headless `rloft_native.exe --verify-raster "<original-install>"` requires valid primary INIT/RES0/T/COLORS plus the strict cursor decode and display-compatible bounds. Result 0 = success, 3 = primary signature incomplete, 4 = raster/palette/cursor decoding problem, 2 = input cannot be opened. `--verify-assets` retains its established structural semantics.
- `NativeEngine/tests/native_raster_tests.cpp` exercises copyright-free original-format RAW and LZSS literals/backrefs/dictionary seeds, corrupt compressed references/headers, sprite frame transform/transparent-key handling and **native loose-media INIT→RES0[47]** resource selection, including out-of-range ID rejection and unchanged source media.
- `NativeEngine/CMakeLists.txt` now compiles `src/native_raster.cpp` into the **native assets library** and registers the new `native_raster_tests` CTest. The Windows build script now requires **at least 10 native CTests**, including the new decoder test (versus 9/9 actually passed on the earlier compiled revision).

## Explicit verification hierarchy

| Gate | State |
| --- | --- |
| Native GOG ISO2048 reader, RES count/palette12, user-owned media | **PASS user-observed** on earlier compiled `fd314b89` |
| New typed independent raw/LZSS raster codec | **SOURCE COMMITTED**, not MSVC built |
| Synthetic corruption, LZSS and cursor-index tests | **AUTHORED**, not Windows executed |
| GUI independent source raster preview | **SOURCE COMMITTED**, not field observed |
| Native `--verify-raster` real GOG acceptance | **NOT YET RUN** |
| Original content hash/diff before-after | Not measured |
| 2D/3D graphics, cinematic, DAT world/gameplay/audio/save runtime | Future independent engine milestones |

**Next acceptance:** one explicitly authorized Windows MSVC compile/CTest/repackage run, then user selects the same original GOG directory and captures the palette plus authentic decoded cursor preview; optional headless `--verify-raster` check confirms the source cursor is structurally decoded. Do **not** substitute DemoV1 decompiled gameplay runtime or repack user assets. Separate, disposable native decoded-resource caches remain optional.

No new Windows Actions workflow has been launched for this unverified raster revision.
