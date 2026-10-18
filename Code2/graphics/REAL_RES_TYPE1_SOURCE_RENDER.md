# Actual Ravenloft RES raster → original CODE.2 cache → VGA frame

**Source evidence and test date:** 2026-10-17/18. **Status:** original `RES0:15` and `RES1:336` render through the **unloaded CODE.2 resource path** (`2ECB4` → `2FD9C` → `2975C` → `2A380`) with bounded read-only host file services. Not a complete DOS file manager, world renderer, DOS screenshot parity, or playable game.

## Original function lineage

Original owner-supplied `CODE.2`: 558,689 bytes; SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Its LE object-1 bytes are at physical file offset `0x3AF24`, and the [21-range SHA-256 verifier](verify_original_vga.py) authenticates these spans:

| Original object-1 span | Binary SHA-256 | Compiled implementation role |
| --- | --- | --- |
| `2EB60..2EC73` | `5c98f9e862c7cda25a0a050947ba9ef1dd486c461665aaaec170529892fe4ec5` | Original typed resource header seek/read into guest buffer, then size field and type-header allocation |
| `2ECB4..2EFB3` | `787b20bda33e47803ed2f13cb90e203835e9f55fdcd0416e428a87745ba17e9b` | Original unloaded resource dispatcher, allocates payload and populates nine-byte item cache |
| `2F96C..2F9AE` | `4b40b4aa87a161b4a568e12206f9f23eca59211f1663d50b6d9a26e41f525eb8` | Original buffered stream initialization and 1,024-byte aligned refill |
| `2F9E4..2FA2F` | `6b63596dc23c7949dc6c201c6025bb92fac290dd50b43a0524b71b1277d75586` | Original individual buffered-byte decode input |
| `2FB04..2FB66` | `8685f1bcf40a6f3adf36a45cf60f76dc768ed1a7bf25e6dcd64b52323e83c7dc` | Original LZSS decoder dispatch and layout context setup |
| `2FA30..2FB01` | `d7746363c0a980d7c84cc27ffd57febdae1b4b41b973f2b32689bb7ba75abf76` | Original output-byte collector and per-frame planar packing |
| `2A20C..2A256` | `3c469e26818f3c532e07ef15f5edf6f9895812a6c0df951392f19484357c1539` | Convert decoded interleaved image bytes into the original four-plane guest image layout |
| `2FD9C..30040` | `bd24fd53a88d3fd58bb34d2bcaf8c68f650d87f2099e8dc0d046bebf7a5297a2` | Original DOS-stream-backed image loader: **now compiled and invoked** through the unloaded-cache resource dispatch |
| `2F29C..2F2E0` | `2662ebb816f7d7e2ce1b541b7b416835a297c067662840bcb6843c70ec32547e` | Retrieve resource from original nine-byte item cache through `2ECB4` |
| `2975C..2978A` | `57133ad4a6f17943d795e39c4fbefe47e8d2f8260ddd5d42099c2dfba74e6c7d` | Dispatch image request through `2F29C`, then call `2A380` |
| `2A380..2A478` | `f5d3f72361f32920058ab1527607be381717c93abeac8b0d2e1ee72d0c0fea91` | Render original guest image record through planar VGA sequencer and memory callbacks |
| `37810..37958` | `3b4abcfdc9dad165e0aba16bc070a7863508c9a4d1ac70eeb12ae98b1e4ee156` | Optional image plane permutation when header field-7 is nonzero; source Scratch table sized for frame count |
| `37700..3780D` | `61c42dd14fd8db7a22e439ff433ed851343e160d38b1b0d88ac5548ecc7f9915` | Original LZSS semantics; host decoder follows its 4,096-byte ring with last 18 bytes initially uninitialized |

The outer RES archive is an entry count and **nondecreasing** absolute offset table; type-1 images have the typed `EH` envelope, 8-byte signed-header descriptor, and either raw (`method 0`) or 4KiB LZSS (`method 3`) encoded payload in the supplied corpus. Decoded size is `frames * rows * width_units * 8`. Detailed evidence: [Resources/RES/FORMAT.md](../../Resources/RES/FORMAT.md).

## Working interface

Read-only, bounds-checked [shared type-1 decoder](original_res_image_archive.hpp) reads the owner's **existing** RES archive, validates every offset and descriptor, and decodes method 0/3 bytes. It does not embed proprietary media, and rejects zero-length/sentinel/unsupported entries and undefined LZSS dictionary reads.

The recomp diagnostic [code2_compiled_graphics_frame.cpp](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp) now creates an **unloaded** original nine-byte resource cache item. Compiled CODE.2 `2975C` calls `2F29C` which dispatches `2ECB4` → `2EB60` (source seek/read header) → `2FD9C` (source image allocation/decoding/layout) → `2A380` (source VGA draw). Source `2A20C` performs image planar packing, with `37810` permutation where needed. Host services supply read-only archive seek, read, position and a bounded guest heap. A second `2F29C` lookup confirms the original cache returns the loaded pointer without reopening the resource. `2FD9C` is no longer bypassed for the `--res` CLI.

Example, on a Windows PC with legitimately owned originals and the Windows milestone engineering executable:

```powershell
.\rloft_original_code2_graphics_frame.exe --res "C:\Games\RLOFT\RES1" 336 1 0 "C:\Temp\RLOFT-portrait" "C:\Games\RLOFT\COLORS" 0 0
```

`RES0 15 0 0` is another certified source-backed fixture. The output stem produces a **raw 320×400 indexed framebuffer** (`.idx`) and a palettized 8-bit Windows BMP (`.bmp`). The output contains actual copyrighted source art: keep these local, do not commit the pictures or distribute them. Without the optional `COLORS` file the visualizer uses a synthetic diagnostic palette, not the game's authentic one.

## Executed loader and file-service proof

The compiled original CODE.2 `2EB60` reads the typed `EH` header using `31334` seek and `312EC` read callbacks. `2FD9C` obtains the type-1 descriptor and payload from the original `2F96C` buffered stream. Read-only callbacks serve bytes from the authenticated archive and expose cursor queries through `3131C`, preserving the source's 1,024-byte refill windows; the callback prevents guest heap overflow and retains original file bytes. Source `2ECB4` updates cache entry state and timestamps and a subsequent source lookup confirms a cache hit with **zero additional archive seeks**.

Raw method 0 executes the source byte-stream and image packing path. For method 3, original `2FD9C` still dispatches `2FB04` and output packing via `2FA30`; the [instruction-derived corrected `37700` control loop](original_37700_control.hpp) checks the **live guest byte `2B6C2`** at the original four instruction offsets, calls original source `2F9E4` for compressed bytes and source `2FA30` for decompressed output, and terminates when `2FA30` sets the guest stop byte. Host bounds/undefined-dictionary checks remain defensive. The legacy static-stop-argument function in `Code2.cpp` remains uncorrected and must not be used. Host preflight performs strict RES envelope and size validation; it does **not** install an already-loaded guest cache record.

**Post-integration validation:** [Linux GCC #39089790975](https://github.com/Hyperbog/RLOFT/actions/runs/39089790975) and [Windows MSVC #39089791043](https://github.com/Hyperbog/RLOFT/actions/runs/39089791043) passed **68/68 CTests**. The Windows milestone additionally passed the real-media validation using original RES0, RES1 and COLORS. Both original source indexed image SHA-256 values and nonzero-pixel counts matched the previously verified preloaded-cache implementation exactly. The Windows milestone reports R04 **unaccepted**.

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

- **Not the complete DOS runtime**: `2ECB4` and `2FD9C` **execute** through original source callbacks, but the archive file is opened by a bounded read-only host service rather than the original DOS open/close API. Host preflight validates type-1 structure, dimensions and compression before source execution; it does **not** preload the guest image/cache. The full game main loop, archive-opening initializer, other asset types and scene/HUD compositor remain unconnected.
- **Other resource types**: RES4's type-6 column textures now have a separately verified read-only importer, original `2ECB4` dispatch and compiled `338C8` column sampler ([source audit](RES4_FIRST_PERSON_SOURCE.md)). The exact compiled `37590` body, RES3 type-7 display-list rendering, and world/HUD composition remain missing. Never feed type-6 into type-1 `2A380`.
- **No original screenshot/scene parity**: Original frame composition, placement, state transitions, inventory/automap and DOS VRAM timing are unverified. Rendered source art in a single frame is not the same as the game's composed HUD/world.
- **Graphics compatibility**: VGA adapter still has documented gaps in read-mode-1, attribute and certain sequencer/CRT timing modes.

Next source milestone: consolidate corrected 37700 control into canonical `Code2.cpp`, recover original INIT-backed multi-archive startup and original type-6 loader 37590, then connect world camera/walls and composed HUD/screen state. The [RES4 source results](RES4_FIRST_PERSON_SOURCE.md) cover authentic column textures, not full projection. Treat all results as source-specific until compared with original DOS captures.
