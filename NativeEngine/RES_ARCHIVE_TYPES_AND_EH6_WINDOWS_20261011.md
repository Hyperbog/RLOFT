# Native image-browser scope correction — RES archive type mix and EH6 renderer

**Date:** 2026-10-11. **Owner field report:** independent native source browser works for some RES files, but RES3/RES4/RES5/RES6/RES10 display "no additional original EH type-1 record". Screenshot verifies GOG ISO2048, 69 source files indexed, 19/19 old structural checks. The screenshot is not a RES corruption proof.

## Original archive classifications — exact shipped corpus

[Reconstructed canonical RES inventory](https://github.com/Hyperbog/RLOFT/blob/native-engine/Resources/RES/INVENTORY.md) and [format research](https://github.com/Hyperbog/RLOFT/blob/native-engine/Resources/RES/FORMAT.md) prove:

| Archive | Count | Actual records | Browser interpretation |
| --- | ---: | --- | --- |
| RES0 | 141 | 126 EH1 indexed rasters + 15 EH2 fonts | EH1 viewer |
| RES1 | 428 | 405 EH1 rasters, 21 empty, 2 non-EH | EH1 viewer |
| RES2 | 38 | 38 EH1 raster graphics | EH1 viewer |
| RES3 | 4,234 | **4,234 EH7** compiled scene shapes/display lists | Not an EH1 image; requires SINTAB-aware geometry renderer |
| RES4 | 667 | **667 EH6** 32/64-column first-person textures | New independent native EH6 texture preview |
| RES5 | 98 | **98 EH3** XMIDI/XDIR music | Audio; never EH1 |
| RES6 | 266 | **266 EH4** Creative VOC sampled SFX | Audio; never EH1 |
| RES7 | 62 | 62 EH1 raster graphics | EH1 viewer |
| RES8 | 73 | 73 EH1 raster graphics | EH1 viewer |
| RES9 | 555 | **517 EH1 raster images**, 38 non-EH interaction scripts | New RES9 selection; first 517 image candidates |
| RES10 | 219 | **219 EH4** VOC voice/sample resources | Audio; never EH1 |

The previous browser only sought EH1. Its "no additional type-1" message was expected for RES3/4/5/6/10, but unhelpfully obscured the actual data. **No files are missing or corrupted as a result of this unsupported view.**

## Independent native-engine source change

- New `native_lzss.hpp/.cpp` centralizes native DOS 4-KiB LZSS decompression shared by EH1, EH6 and future EH7 decoders; original demo engine/runtime remains separate.
- New `native_type6_texture.hpp/.cpp` recognizes EH6 (no header body, two-byte decompressed size, LZSS), accepts the two actual 4096/8192-byte source body sizes and transposes the original column-major 128-texel source columns into 32x128 or 64x128 indexed preview rasters. It does not write original data and does not claim to implement the original per-draw palette-index shading subtraction or renderer geometry.
- Native image browser now opens RES4 EH6 textures by record index, including previous/next texture navigation, and RES9 EH1 records. `NativeRasterRecord.resource_type` identifies EH1 versus EH6. Win32 archive selector includes RES9, GUI labels RES4 textures explicitly. RES3 reports that EH7 geometry and SINTAB are needed, while RES5/6/10 explicitly say XMIDI/VOC audio and do not pretend these are bad images.
- CMake now links only new native codec sources, keeps the full independent original-game data VFS and requires `native_type6_texture_tests` as the 14th CTest. Synthetic tests cover both original texture widths, 128-column transpose, truncated/corrupted LZSS, archive browser navigation across EH6 and RES9 art.
- The decomp demo is not linked and no owned DOS/GOG resources ship in the build.

## Actual Windows MSVC acceptance

[Windows Actions build #38172310617](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617) compiled exact independent native source Git SHA `744036693e84619d136d69dab157e9de25a2086e`. **14/14 Windows CTests PASSED**. Full asset-free Windows GUI executable and ZIP packaging completed successfully. [Artifact #11714760864](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617/artifacts/11714760864) was downloaded and inspected bytewise. Exact SHA256:

- Distribution ZIP: `8f69d550992a1b7c170a98a158adc940c383346a69631c9fc837c1cab9102b03`
- Executable `rloft_native.exe`: `e4cd5b61f7a380d9269e413fc2a3f25b6b4d5a0bcd6ffb06ef6cad7910890e97`

**Not yet user-field-verified:** RES4 image appearance and RES9 image list against actual original GOG. The source-format synthetic tests do not prove original gameplay/renderer visual parity; RES4's source indexed pixels undergo additional per-draw index subtraction/shading, scene-level palette selection and scaling in original CODE.2. RES3 EH7 geometry remains a separate substantial SINTAB interpreter/replacement renderer task, not a normal sprite image.

**Next:** owner opens the new ZIP, selects original GOG directory, selects RES4 record 0/1 and Next texture, then selects RES9 record 0 and checks a sprite. Use screenshot to flag width, orientation, color context/shading; audio archives should now explain their format rather than show a generic EH1 error. Resume independent EH7 geometry and typed VOC/XMIDI inspection after real source texture field validation.
