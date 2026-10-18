# Actual Ravenloft RES raster → original CODE.2 cache → VGA frame

**Source evidence and test date:** 2026-10-17/18. **Status:** real source-backed image records `RES0:15` and `RES1:336` successfully rendered into indexed host frames on a Windows runner. Not an authored scene, complete resource/file manager, DOS screenshot parity, or playable game.

## Original function lineage

Original owner-supplied `CODE.2`: 558,689 bytes; SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Its LE object-1 bytes are at physical file offset `0x3AF24`, and the [15-range SHA-256 verifier](verify_original_vga.py) authenticates these spans:

| Original object-1 span | Binary SHA-256 | Compiled implementation role |
| --- | --- | --- |
| `2A20C..2A256` | `3c469e26818f3c532e07ef15f5edf6f9895812a6c0df951392f19484357c1539` | Convert decoded interleaved image bytes into the original four-plane guest image layout |
| `2FD9C..30040` | `bd24fd53a88d3fd58bb34d2bcaf8c68f650d87f2099e8dc0d046bebf7a5297a2` | DOS-stream-backed image loader; **not yet called** by the new read-only host file adapter |
| `2F29C..2F2E0` | `2662ebb816f7d7e2ce1b541b7b416835a297c067662840bcb6843c70ec32547e` | Retrieve resource from original nine-byte item cache through `2ECB4` |
| `2975C..2978A` | `57133ad4a6f17943d795e39c4fbefe47e8d2f8260ddd5d42099c2dfba74e6c7d` | Dispatch image request through `2F29C`, then call `2A380` |
| `2A380..2A478` | `f5d3f72361f32920058ab1527607be381717c93abeac8b0d2e1ee72d0c0fea91` | Render original guest image record through planar VGA sequencer and memory callbacks |
| `37810..37958` | `3b4abcfdc9dad165e0aba16bc070a7863508c9a4d1ac70eeb12ae98b1e4ee156` | Optional image plane permutation when header field-7 is nonzero; source Scratch table sized for frame count |
| `37700..3780D` | `61c42dd14fd8db7a22e439ff433ed851343e160d38b1b0d88ac5548ecc7f9915` | Original LZSS semantics; host decoder follows its 4,096-byte ring with last 18 bytes initially uninitialized |

The outer RES archive is an entry count and **nondecreasing** absolute offset table; type-1 images have the typed `EH` envelope, 8-byte signed-header descriptor, and either raw (`method 0`) or 4KiB LZSS (`method 3`) encoded payload in the supplied corpus. Decoded size is `frames * rows * width_units * 8`. Detailed evidence: [Resources/RES/FORMAT.md](../../Resources/RES/FORMAT.md).

## Working interface

Read-only, bounds-checked [shared type-1 decoder](original_res_image_archive.hpp) reads the owner's **existing** RES archive, validates every offset and descriptor, and decodes method 0/3 bytes. It does not embed proprietary media, and rejects zero-length/sentinel/unsupported entries and undefined LZSS dictionary reads.

The recomp diagnostic [code2_compiled_graphics_frame.cpp](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp) copies decoded bytes to bounded guest memory, calls **compiled original `2A20C`** to build the guest image, optionally calls **compiled `37810`** to permute planes, writes an original-format *already-loaded* nine-byte cache item, then enters **compiled `2975C` → `2F29C` → `2A380`**, with actual VGA port/memory callbacks. This is the original resource *lookup and rendering* path, not the full original DOS resource open/seek/read path.

Example, on a Windows PC with legitimately owned originals and the Windows milestone engineering executable:

```powershell
.\rloft_original_code2_graphics_frame.exe --res "C:\Games\RLOFT\RES1" 336 1 0 "C:\Temp\RLOFT-portrait" "C:\Games\RLOFT\COLORS" 0 0
```

`RES0 15 0 0` is another certified source-backed fixture. The output stem produces a **raw 320×400 indexed framebuffer** (`.idx`) and a palettized 8-bit Windows BMP (`.bmp`). The output contains actual copyrighted source art: keep these local, do not commit the pictures or distribute them. Without the optional `COLORS` file the visualizer uses a synthetic diagnostic palette, not the game's authentic one.

## Verified original-media run

Read-only sparse checkout from `DnD-Classics/MASTER/Ravenloft - Strahd's Possession/RLOFT/`, hashes compared to archived verified originals. [Windows/MSVC milestone #39087826736](https://github.com/Hyperbog/RLOFT/actions/runs/39087826736) **passed 68/68 CTests and original media validation**:

| Original file | Original SHA-256 | Record | Observed original-render raster |
| --- | --- | --- | --- |
| `RES0` | `5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d` | entry 15 (RES0 raster base) | 90 nonzero pixels, indexed SHA-256 `cb8c6c57278a10fb5a3351d0e0bb1a8d14280180f2a73f78b71588ac21070977` |
| `RES1` | `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19` | entry 336 (first portrait) | 2,992 nonzero pixels, indexed SHA-256 `aba3866977b67c01b161fb2573d8a6cb3ddd808a3092168dc883d8f41b4e8271` |
| `COLORS` | `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5` | bank 0, intensity 0 | Real RGB6 DAC palette applied in both captures |

The original-media validation script [verify_owned_res_render.py](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/tools/verify_owned_res_render.py) creates frames **only in an ephemeral Windows runner temp directory** and prints harmless hashes/pixel counts. Neither original files nor their decoded raster/BMP output are uploaded in CI artifacts. The downloadable engineering package contains executable/tests and a separate clearly marked **synthetic-input** graphics capture.

The first original-file test run #39087613509 timed out on the RES1 portrait because the `37810` permutation scratch-table was undersized. Fixing the table to `4 * max(frames,rows)` enabled the verified original portrait render on the next run. The failure was real; do not count the first test as passed.

## Current limitations

- **No complete runtime graph**: Source `2FD9C` and the original `2ECB4` *unloaded* resource dispatch, DOS file IO, global game asset tables and runtime world/HUD loops are not yet executed. The host file decoder preloads the cache and the actual CODE.2 draw wrapper dispatches the indexed resource.
- **Only type 1**: RES4's type-6 first-person column textures and RES3's other typed content need different original loaders and drawing routines; do not feed type-6 payloads into type-1 `2A380`.
- **No original screenshot/scene parity**: Original frame composition, placement, state transitions, inventory/automap and DOS VRAM timing are unverified. Rendered source art in a single frame is not the same as the game's composed HUD/world.
- **Graphics compatibility**: VGA adapter still has documented gaps in read-mode-1, attribute and certain sequencer/CRT timing modes.

Next source milestone: wire the original unloaded resource-loader branch (`2ECB4` → `2FD9C`) to a bounded read-only host DOS file interface, then connect RES4 type-6 textured 3D column rendering and game-composed screen state. Treat all results as source-specific tests until compared with original DOS captures.
