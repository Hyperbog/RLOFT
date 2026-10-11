# Original palette context and CODE.2 cursor colors — Windows verification

**Date:** 2026-10-11. **Project:** independently written native replacement engine; no decomp-demo gameplay is linked.  
**User observation:** native RES image browsing functions but sprite colors look wrong, almost inverted. The owner reports a visually similar color problem in the **separate** decomp demo. The new native fix is **not yet field-verified** on original GOG graphics.

## Source-backed diagnosis (do not presume a universal RGB inversion)

The previously compiled native RES browser selected **`COLORS` bank 12 for all raster records**, without considering the original CODE.1/CODE.2 presentation context. The original game uses bank 12 for the later startup menu, bank 8 for CODE.1 character creation, banks 14 and 9 during the opening logo transitions, and other banks for gameplay/world contexts. Original indexed EH1 sprites themselves hold **palette indices rather than independent RGB palettes**. Recoloring them with the wrong bank can make otherwise correct pixels look inverted or dramatically wrong.

Second issue: the original CODE.2 title menu applies a **19-entry color-index translation** to the default `INIT`-selected RES0 cursor, translating **only frame 0** (transparent pixels are excluded, and the transparent index itself remains unchanged). The previous native preview showed the untransformed cursor directly with bank 12, which is not the title-menu presentation.

The RGB→BGRA conversion in the previous native inspector matched the corresponding software host logic in the decomp demo. A naive global channel swap or `255 - channel` inversion is therefore **not justified by this evidence**; introducing either would falsify original DAC6 data. A shared visual symptom does not establish a shared single root cause across two separately developed executables. The separate demo has not been modified by this native project correction.

## Actual native source changes

- New `NativeEngine/include/rloft_native/native_raster_colors.hpp` and `src/native_raster_colors.cpp` implement typed RGB/DAC6-index-to-Windows-BGRA mapping, with **explicit palette bank as an input**. No hardcoded bank or invented 8-bit inversion is part of the colorizer. Original DOS palette channel formula is `(DAC6*255+31)/63`.
- Typed `NativeRasterColorMode::RawIndices` versus `Code2TitleCursor`. The latter implements the 19 original source-evidenced index substitutions, with a zero-default lookup table and frame-0-only transformation. Transparency is preserved. No automatic cursor remapping happens to ordinary RES browser records.
- The independent Win32 asset GUI now offers a **bank 0–14 selector**. The source palette swatch and decoded sprite rerender together after selection. Bank 12 is labeled menu, bank 8 CODE.1, banks 9 and 14 logo. The **Title cursor** control restores the original `INIT + 0x2F` selected cursor with the CODE.2 menu remap; browsing any other RES entry uses raw pixels.
- New `native_raster_colors_tests` validates three asymmetric RGB primary colors and BGRA output bytes, changing palette banks while keeping original pixels unchanged, original CODE.2 translation, untouched later frames, transparent-key behavior, and invalid source bounds.
- CMake builds the independent native colorizer into `rloft_native_assets`. The Windows package script raises the mandatory test floor from 11 to **12** and requires the color regression. The asset-free Windows README explains why a single bank is not appropriate for all RES images.

## Real Windows build and artifact acceptance

**[GitHub Windows Actions run #38169819411](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411)** — Windows Server 2022, Visual Studio 2022/MSVC x64 Release: **SUCCESS**, complete **12/12 actual Windows CTests passed** (0.80 s), release ZIP integrity and upload passed.

**Exact compiled source SHA:** `1c9440303a5c83ff6834a3f87cfdf9afc8dbfa19`. Later documentation commits are not part of the binary.

**[Artifact #11713202926](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411/artifacts/11713202926)** was downloaded and independently inspected. Exact hashes:

| Item | SHA-256 |
| --- | --- |
| GitHub outer artifact ZIP | `bf7af5ad94f56185e4e6dd28626105d8471748a928fcb7c9f6a414cf3d93ec06` |
| Embedded asset-free user ZIP | `2e63c7218a37f2bf0f850c8d307c3dcfb39018c8db25fc3647d3d143a15bcf92` |
| `rloft_native.exe` x64 Win32 GUI | `056410da6e53f26b04eb83b14fe66f6acf638a35be303f19912e6f6e16dc591b` |

Binary was confirmed to be a Windows **PE AMD64 `0x8664`**, GUI subsystem 2. ZIP had the exact four expected files: `rloft_native.exe`, `README-NATIVE-ASSETS.txt`, `SHA256SUMS.txt`, `build-provenance.json`. The manifest records `source_commit=1c9440...`, `gameplay_verified=false`, `original_assets_included=false`, `full_extraction_required=false`.

User's previous original GOG media indexing (ISO2048, 69 files, 19/19 structural assets, palette12) was verified on a **different earlier binary**. This new palette-aware sprite rendering has not been directly observed on actual user-owned GOG media.

## User field test: isolate remaining mismatch

1. Run this new `rloft_native.exe` from the asset-free ZIP and open the same original GOG installation.
2. Observe **Title cursor** under **COLORS 12** (menu remap) and confirm title cursor colors, and compare with the cursor selected by browsing RES0 index manually (raw indices).
3. Switch to **COLORS 8** for RES sprites associated with original CODE.1 character creation; verify the bank selector changes both swatch grid and sprite coherently.
4. For any remaining wrong image, record **RES archive, record index, palette bank, whether Title cursor is active and a screenshot**. Do not mark all records as intrinsically palette12 or blindly swap/invert all colors.
5. Future support: independently determine additional DOS per-scene palette assignments/index remaps, based on original CODE and research, without calling into the decomp demo's executable runtime. Keep DOS image parity a separate gate.

**Status:** original-media/RES browser builds and Windows color regression **PASS**; visual issue resolution on real original art **PENDING**.
