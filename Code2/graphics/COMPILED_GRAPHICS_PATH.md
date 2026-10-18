# Compiled original CODE.2 graphics → host VGA → indexed frame

**Source evidence and scope:** This diagnostic executes C++ functions reconstructed from original CODE.2, not a recreation of their drawing logic in a test. The host VGA adapter interprets original source port callbacks and planar video-memory accesses. **It does not yet start the original game or render a scene using its original RES artwork.** All automated image inputs are synthetic and clearly labeled.

## Executed CODE.2 functions

| Object-1 entry | Exact action in the compiled diagnostic |
| --- | --- |
| `29930` | Initialize 320×400 mode 3 or 320×200 mode 2; program VGA sequencer, graphics controller and CRTC through callbacks |
| `2AB98` | Clear selected video planes using the original CODE.2 A0000-memory loop |
| `2ABC4` | Poll port `03DAh` for vblank (host advances deterministic simulation, not wall-clock VGA timing) |
| `2AB38` | Compute 32-bit original planar address from row, column, active page and x modulo 4 |
| `2A380` | Draw an actual reconstructed planar image routine from a caller-provided resource record |
| `29AA4` / `29AF0` | Set planes, latch-read, write/read indexed pixels using the CODE.2 raster primitives |
| `2AB6C` / `2ABF4` | Select write and display pages separately, including the second video page |
| `2AC10` | Program CRTC line compare (07h, 09h, 18h) for split-screen display |
| `29CA0` | Stream RGB6 palette bytes through DAC ports `03C8h/03C9h`, in original 192-byte chunks |
| `32150` | Apply original brightness/rescaling to 256 RGB palette entries using the active profile's +6Ch intensity byte |

The original file-backed consumer **`325B0`** also establishes `COLORS` bank offset `1 + 768 * bank` and invokes `32150`; the diagnostic currently reproduces only the **file read in a host read-only adapter**, then calls compiled `32150` and `29CA0`. The original DOS file-opening and asset loader are **not** yet connected.

The `COLORS` resource format is 11,521 bytes: one count byte (15), followed by fifteen 768-byte RGB6 banks. Its independently recorded SHA-256 is `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5`; see [original resource classification](../../Resources/MISC/COLORS/README.md). Never embed or commit raw COLORS/RES files.

## Implementation, build, and frames

- **Recomp-only compiled-source entry:** [`Recomp/src/code2_compiled_graphics_frame.cpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_compiled_graphics_frame.cpp) directly includes and compiles the unchanged reconstructed `Code2/Code2.cpp` into a separate engineering executable.
- **VGA adapter:** [`Recomp/src/original_vga_host.hpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/original_vga_host.hpp) implements covered unchained four-plane memory, 0–3 VGA write modes and latches, DAC6, 200/400 vertical scan repetition, CRTC start and line compare, and deterministic `0x3DA` retrace transition. It records unsupported register reads/writes as errors in the compiled harness.
- **Frames:** `rloft_original_code2_graphics_frame <output-stem>` emits `<output-stem>.idx` (raw 320×400, 128,000 palette indices) and `<output-stem>.bmp` (8-bit palettized 320×400 BMP with 256 RGB6→RGB8 entries, no alpha). Colors and image content are synthetic by default. The function's output is a **real indexed host frame produced by compiled CODE.2 code**, not an original Ravenloft game screenshot.
- **Optional real palette only:** `rloft_original_code2_graphics_frame <output-stem> /path/to/COLORS 0 0` reads the owner's authentic `COLORS` file in read-only mode using palette bank `0..14` and intensity `0..63`; it still draws a synthetic image unless a future authored resource caller is wired in. The raw palette file is never included in the Windows package.
- **Portable tests:** [`Recomp/src/test_original_vga_host.cpp`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/test_original_vga_host.cpp) and the compiled image test validate VGA hardware and pixels; the media-free `Recomp/tools/test_compiled_graphics_artifact.py` tests bank parsing, compiled brightness rescaling, BMP dimensions/palette, and malformed-file rejection. Each gate is registered in `Recomp/CMakeLists.txt` and runs on Linux and Windows.

## Verified milestones (engineering only)

- [Windows MSVC #39085580864](https://github.com/Hyperbog/RLOFT/actions/runs/39085580864), [Linux GCC #39085580768](https://github.com/Hyperbog/RLOFT/actions/runs/39085580768): **66/66 CTests** passed, including direct execution of original reconstructed CODE.2 drawing routines.
- [Windows MSVC milestone #39085661346](https://github.com/Hyperbog/RLOFT/actions/runs/39085661346): passed all 66 tests, invoked the compiled frame diagnostic independently and uploaded [Windows graphics engineering package](https://github.com/Hyperbog/RLOFT/actions/runs/39085661346/artifacts/12120363240) containing `.idx` and `.bmp` frames, plus [test evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39085661346/artifacts/12120383231). Later palette bank tests require their own verified CI result before being counted.

## Authenticated original RES raster path

Real game files now render through **compiled CODE.2 2975C → 2F29C → 2ECB4 → 2EB60 → 2FD9C → 2A20C/37810 → 2A380** on the Windows milestone runner using bounded read-only archive callbacks. The unloaded item is loaded by source CODE.2, and a second lookup confirms the newly populated original cache. The verified original RES0:15 and RES1:336 frames contain 90 and 2,992 nonzero indexed pixels; their hashes are unchanged from the prior diagnostic. The RES image bytes and graphics are **not** uploaded. Method 3 uses a bounded compatible host callback at 37700; the original 37700 stop interface is still not recovered. See [original RES source execution/SHAs and limits](REAL_RES_TYPE1_SOURCE_RENDER.md). The earlier synthetic-frame samples remain separate diagnostic fixtures.

## Remaining acceptance gaps

1. **Original image and world content**: the diagnostic currently uses synthetic planar image records; actual decoded RES textures, sprites, scene/HUD assets, animation state and palette banks are not yet linked into full game runtime.
2. **Full hardware timing**: vblank `3DAh` is deterministic synthetic cycling. Exact CRTC timing, attribute-controller registers, VGA read mode 1 and alternate memory addressing remain incomplete.
3. **Full CODE.2 execution**: the compiled graphics functions are invoked through a bounded explicit callback session, not yet by following the complete application's DOS startup, main loop, world renderer and resource/file services.
4. **Reference parity**: no independent original DOS captured screenshot or palette/frame SHA comparison. Visual fidelity and a playable recompilation remain unverified.

This report is synced as a byte-identical technical reference between `recomp` and `code1-decompilation`. The compiled VGA host, build targets and Windows artifacts **remain on `recomp`**. Do not conflate a CTest passing with R04 accepted or a runnable enhanced game.
