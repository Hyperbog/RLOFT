# Original LEVELS.DAT grid into canonical CODE.2 334C4

**Status:** Source-executed real-grid view *preparation* for all 32 owner-preserved `LEVELS.DAT` records, on Linux and Windows. **Stop boundary:** original `32954` ray/grid traversal, *not executed* in this checkpoint. The test uses a synthetic fixed-point multiplier and trig-table context so it does **not** claim original camera position, visibility/occlusion parity, first-person projection, or a playable original CODE.1-to-CODE.2 game boot.

## Original bytes and source mapping

- Immutable `LEVELS.DAT`: 499,878 bytes, SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`. It contains 32 indexed level records, 115,584 total 16-bit packed cell words.
- Original CODE.2 `181B4` copies **0x2A4 fixed level-header bytes** to guest `0x25618` and **width × height × 2 packed grid bytes** to guest `0x198E0`.
- Original `334C4..335DB` (SHA-256 `66a8a82aeffae8ff115ae4e314d766582c159ec629d6dce222dd319d03a2586d`, covered by [the 32-span CODE.2 verifier](verify_original_vga.py)) clears bit `0x0040` in every loaded grid cell, preserving all other bits. It then initializes view-angle/position scratch state and calls `32954`.
- The original grid format: a 32-element absolute-offset table, 0x1D-byte level preamble with seven relative section boundaries, 0x2A4-byte header (width/height signed words at offsets +1/+3), then row-major cell words. See [original format classification](../../DatFiles/LEVELS.DAT/FORMAT.md) and [bit semantics](../../DatFiles/LEVELS.DAT/CELL_WORD.md).

## Implementation

[Shared read-only parser](original_levels_grid.hpp) validates original archive bounds, offsets, per-record relative section boundaries, signed dimensions and exact grid length. It neither mutates source media nor decodes the unrelated dynamic records, scripts or family structures. It is synchronized to Decomp; the runnable executable stays in Recomp.

[`rloft_original_code2_level_view`](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/src/code2_original_level_view.cpp) includes the compiled canonical `Code2/Code2.cpp`, transfers each real source header and cell grid into the original guest locations, runs **source `function_334c4`**, and stops via an explicit `32954` callback after verifying that the sole cell-word mutation was clearing `0x0040`. The synthetic fixture includes three set visibility marks to test this transition; the original archived levels contain zero preset marks, so the authentic-file test checks no other bits change. A zeroed trig lookup and synthetic multiplication boundary are deliberate **test-only** values.

On the owner's original data:

```powershell
.\rloft_original_code2_level_view.exe --levels "C:\RLOFT\LEVELS.DAT"
```

Without arguments, the executable runs a media-free 32-record synthetic fixture. It outputs source checkpoint metadata (level, dimensions, cell count and cleared-mark count), not original raster art.

## Verified result

- [Linux source CTests #39097755657](https://github.com/Hyperbog/RLOFT/actions/runs/39097755657): build and test gate passed after adding the level target.
- [Windows/MSVC milestone #39097776940](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940): **72/72 CTests** passed, plus SHA-256 validation of the original `LEVELS.DAT` and source `334C4` preparation against **all 32 authentic level records**. The visible printout includes `CODE2_ALL_32_ORIGINAL_LEVELS_VIEW_PREP_PASS` and expressly `CODE2_32954_REAL_WORLD_RAYCAST_EXECUTED=NO`.
- [Windows engineering executable ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940/artifacts/12125820072) and [JUnit evidence](https://github.com/Hyperbog/RLOFT/actions/runs/39097776940/artifacts/12124973924) expire December 17, 2026. No original resource/cell binary data is included in these artifacts.

## Next integration

1. Extend the bounded read-only CODE.2 `181B4` loader itself through the existing resource-startup file services, rather than supplying original serialized header/grid directly. Preserve variable records and level script sections without fabrication.
2. Supply authentic view position/direction and CODE.2 trig lookup inputs to `334C4`, then let original `32954` traverse the **same authenticated cell grid**. Test against independent DOS ray/visibility traces rather than mirroring a synthetic result.
3. Join actual produced wall/depth records into already-compiled `335DC` → `3263C` → `338C8`. Verify full first-person source frame, occlusion and HUD only with real DOS reference capture.
4. Connect the resource subsystem checkpoint to the real CODE.1 → CODE.2 handoff. **R04 and playable original game boot remain unaccepted**; 72 passing tests do not change that.
