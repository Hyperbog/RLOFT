# Part 28 — Source HUD/world entry and original VGA 1A5B0 presentation (2026-10-22)

**Owner:** `Recomp` branch. **Last tested source:** [`3dc4a8be702849b3b18a56f3a6b830c8579131e9`](https://github.com/Hyperbog/RLOFT/commit/3dc4a8be702849b3b18a56f3a6b830c8579131e9). This is source/raster **partial integration**, **not** a playable frame or DOS screenshot parity. Do not wire `Application45Boundary.app44` into the retained `1FC14` gameplay loop until the complete `32954` ray, RES1 HUD and overlay callback graphs execute without skipped branches.

## Verified original VGA frame presentation

- [`7935f0be`](https://github.com/Hyperbog/RLOFT/commit/7935f0be6a3aa7447e8962c1bb737b28d4745a42) adds the original source `2A020` VGA graphics-controller mode-1 latch copying of all four VGA planes from the selected original page to its opposite; and `2932C -> 2A178` planar cursor-background restoration. The original mode-1 copy is **not** a host RGBA blit.
- [`3d683e0e`](https://github.com/Hyperbog/RLOFT/commit/3d683e0e5d718a7c31fae1819c037007f7aec396) tests the **compiled original `1A5B0` fallback (mouse-absent)** on a synthetic 320×400 original VGA surface: original `291C8` CRTC scanout, `2E590` page push, `2A020` four-plane copy, `2E5D4` page pop; indexed pixels match a second independently generated pattern on the alternate page.
- [`c1bb5a29`](https://github.com/Hyperbog/RLOFT/commit/c1bb5a2957dd615775dc491113c803a1b7356897) wires a **mouse-active `1A5B0` adapter on the retained original CODE.2 session**, reusing the existing INT33 state, original `1A288/29804`, source `292BC/2A114` capture, `2A020` VGA plane copy, source `2932C/2A178` restore, `30CD4` cursor background copy, `291C8` CRTC and page stack. It is ready as `presentation.call_1a5b0`; it is **not invoked by the live full scene** because the `1F8E0/1F8FC` mode transition is still gated.
- [`cd34289b`](https://github.com/Hyperbog/RLOFT/commit/cd34289ba78295b38cc9c28cfb7c2d55f90ecca1) verifies **mouse-active source `1A5B0`** with an *explicitly synthetic* original-format 11-frame cursor image: original source capture, 29804 raster, mode-1 page copy, background restore on other page, 3 matched push/pops and exact readback `77` versus original background pixel `19`. No original RES0:62 image or DOS screenshot parity is claimed by this test.

## Original 1F494 party HUD

- [`61ebeb2f`](https://github.com/Hyperbog/RLOFT/commit/61ebeb2f50ae27f0382a149b53acb361cc3de592) executes `1F494` in an isolated source probe for all 4 high-bit party statuses. Verifies actual actor-table strides, `C034=4` image selection, original `CF5A..CF5D` dirty flag clearing, and requests at `113h` with the source-computed resource ID, not hardcoded simulated frame writes.
- [`413bfcc7`](https://github.com/Hyperbog/RLOFT/commit/413bfcc7f29d578a0f5ccbb4bb1f762e4b6c00c8) also executes the `1F494` **normal party** and `1EA90` equipment branches with source model actor/HP/item fields, verifying original `2975C` frame, `298F0` portrait/equipment translation, health-bar rectangle, original status enumeration, and both equipment slots. Callback observations are **not** pixels.
- [`c61664fb`](https://github.com/Hyperbog/RLOFT/commit/c61664fbd5d5ad36fa38e003353736711f94c766) adds original `2975C -> 2A380` planar HUD frame adapter and `298F0 -> 2A584` guest palette-table translated portrait adapter. [`4ec2aca9`](https://github.com/Hyperbog/RLOFT/commit/4ec2aca984dd338ca0d20607eb79164aab19e1c3) tests synthetic decoded image `77` and same image translated by original guest table to VGA indexed `78`. This is real source raster but **not** yet live RES1 portrait rendering.

## Original 1F1C8 world entry and 334C4 camera

- The same [`61ebeb2f`](https://github.com/Hyperbog/RLOFT/commit/61ebeb2f50ae27f0382a149b53acb361cc3de592) test enters original `1F1C8` from a source `Application44Boundary`. It verifies original `1EDAC` world state changes, `FDBC` VGA aperture, `C044/C048` camera high-word, `FF50` scene vertical value, and ordered source requests `334C4`, `33E28`, `33DE0`, `34DE8`, `1BDAC`, `2C294`, `274A4`, `2F868`. The ray and background callbacks in this test **observe requests; they do not render**.
- [`3dc4a8be`](https://github.com/Hyperbog/RLOFT/commit/3dc4a8be702849b3b18a56f3a6b830c8579131e9) executes the **compiled original `334C4`** including source `35680` signed fixed-point camera-vector products on a bounded synthetic grid. Verifies original `198E0` cell flag clearing, trig/camera globals, `FD0C` cell count, `C714` heading, `255FC/255F8` trig registers, `255F4/255EC` camera offsets, `22220` reset, and precisely one source request for **`32954`**. Actual source `32954` ray traversal, original `3263C` textured wall columns, and SINTAB-generated VGA raster are **not attached to the retained runtime**.

## Authentic Level 22 resource and branch witness

Read-only original-media instrumentation at [`07d2b90b`](https://github.com/Hyperbog/RLOFT/commit/07d2b90b1d342deb5af50d4e830e4957eab34b8c) reports after real `181B4/17A68`, from **original guest bytes** (not synthetic tests):

```text
mode=4, retained_mode=3
1F8FC initial RES0 frame IDs 28 and 29
heading=270
exterior_flags=1,1
world_kind=58, active_world_cell=1
party slot0 flags=0, source RES1 portrait ID=338, normal=1
party slot1 flags=0, source RES1 portrait ID=362, normal=1
party slot2 flags=32768, terminal HUD branch
party slot3 flags=32768, terminal HUD branch
portrait_slots=2, terminal_slots=2
```

The source-derived portrait IDs in slots 2/3 are **not requested** while the terminal bit is set. The real level exercises the outdoor `1F1C8` path and is not covered by a wall-only synthetic renderer.

## CI proof

- [Original-media MSVC Windows #39864605306](https://github.com/Hyperbog/RLOFT/actions/runs/39864605306): **PASS, 86/86 CTests**; original Level22 111 actor records, 17A68 source execution, 1FC14 virtual input; above authentic HUD/world branch trace.
- [Original-media MSVC Windows #39865045256](https://github.com/Hyperbog/RLOFT/actions/runs/39865045256): **PASS** including new bridge compiled source.
- [Source-link CI #39865513544](https://github.com/Hyperbog/RLOFT/actions/runs/39865513544): **PASS 86/86** on `3dc4a8be`, including all new isolated source/pixel/control probes.
- [Portable Linux regression #39865513586](https://github.com/Hyperbog/RLOFT/actions/runs/39865513586): **PASS** on `3dc4a8be`.

## Required next implementation (do not skip)

1. Attach original RES1:338/362 decoding to the same retained `2F29C` cache and VGA translated portrait source, alongside original RES0:28/29 frame base and terminal HUD sprite, then connect `Application43Boundary` and **real `1F494`** (with `22F40`, `1EA90` and source palette/transparency overlays).
2. Connect original `334C4 -> 32954 -> 335DC -> 3263C -> 32620` and `SINTAB` generated scaler to the same CODE.2 guest and retained OriginalVgaHost; use **real heading 270, outdoor flags 1,1, cell1**, and complete `34998/34A74/34DE8` and environment branches.
3. Only after `Application44Boundary` and all mode-4 palette/presentation callbacks are source-backed, **enable original `181B4.call_1f8e0`**; accept one full actual composite VGA frame, then two input-driven frames with source world-position changes and byte-exact palette/indexed-pixel original DOS reference captures.
4. Retain `1FC14` input and source actor postload/AI completeness as separate acceptance gates.

**No original copyrighted binaries committed. No false playable, active game-frame, per-pixel DOS screenshot, original HUD portrait, or full original raycaster parity claims.**
