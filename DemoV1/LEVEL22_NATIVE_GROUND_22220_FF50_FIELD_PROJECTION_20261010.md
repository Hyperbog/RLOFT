# Level22 source-ordered nearest depth and FF50 native field projection — 2026-10-10

**Branch:** `demo-v1`. **Classification:** *read-only field diagnostics* plus original-code provenance and regressions. **No source-game rendering, movement, clock, RNG, palette or inventory behavior was changed.** This is not a DOS RAM dump nor DOS pixel parity acceptance. No original game binary/resource data was committed.

## Established before the batch

Authorized [Windows Full #658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) passed **214/214** CTests on checkout `ba3dc29e42d93e87a147b4b909c52167dbd3b362`. Its user-supplied field capture contained **64/64** native Level22 indexed full-open `RES4[655]` ground rasters with **32768** indexed writes. The *separate* original DOS 320x400 framebuffer, original palette, IRQ, RNG and partially wall-clipped ground remain unaccepted.

The isolated source models added afterward established:
- `OriginalGroundNearest22220SourceModel`: original `335C8` initializes `22220=0x800`; `330E7/33278/3337F/33487` materializer tails can reset it to **zero** for source inner material `0x93`, then signed-min compare against record depth.
- **Distinct** `32F4C` source sentinel writer fills one 28-byte record with depth 0, inner/outer 0x93 but **does not update 22220**. Therefore a materialized 0x93 and an original sentinel-filled 0x93 must never be conflated.
- `OriginalGroundFF50SourceModel`: genuine original source `15618` dynamically generated table, `C738=127` scale blocks from original supplied SINTAB, signed `22220/8` indexing; initial upper FF50 `128-EBX/2+EBX/8` and lower `256-(EBX+upper)`.
- `OriginalGroundSurfaceDispatch`: actual `1F1C8` later reloads **mutable FF50 after upper callbacks**. The derived pure first-pass lower span is a *projection* unless all upper callbacks and mutable source mode branches are faithfully executed.

## New read-only native projection

A new `OriginalGroundSourceProjectionModel::evaluate` takes the ordered `ViewSweepRecord` vector, original loaded SINTAB **C6D8 ray count** and **C738 scale count**. It maps each source provenance pair (`original_materializer_executed`, `original_sentinel_initializer_executed`) to the already verified isolated nearest-depth replay; no inference is made from textures, source resource IDs or image pixels. It converts the original ray depth DWORD to signed without relying on implementation-defined unsigned narrowing. The source replay fails closed for an unknown source producer, contradictory sentinel fields, incorrect count or unsupported depth; FF50 calculation independently fails closed when `22220/8` is outside the original C738 table.

`ExplorationRuntimeFrame` now additionally carries **two read-only counters** sourced directly from `RendererTables::c6d8()` and `RendererTables::scale_block_count` (these are read from the installed original SINTAB; they are not host constant 256/127). They never affect the renderer, movement or game state.

The existing optional `ExplorationFrameCapture::write` F12 TSV has new fields:
```text
ground_projection_original_c6d8
ground_projection_original_c738
ground_projection_available
ground_projection_22220
ground_projection_zero_resets
ground_projection_sentinel_fills
ground_projection_min_updates
ground_projection_upper_ff50
ground_projection_lower_ff50
ground_projection_lower_four_plane_rows
ground_projection_lower_paired_plane_rows
```
All are labeled *native source-order projections*, **not original DOS values**. Unknown/unverified cases write the literal **`unverified`** rather than numeric zero or a fabricated clipping height. This is a diagnostic extension of the existing native `rloft-native-indexed-capture-v2` format; original indexed VGA and palette buffers are written exactly as before.

A key bounded fixture: source materializers with depths 800 then 408 and one genuine 32F4C sentinel writer produce projected `22220=408`, first upper `FF50=51`, lower `FF50=1`, source 4-plane count 1 and paired-plane count 0. An unknown producer in the same three-slot fixture yields unavailable projection with `unverified` values. Both scenarios are asserted in the **actual exported TSV** via extended `exploration_frame_capture_tests`, in addition to new `original_ground_source_projection_tests` for 256 source slots, signed negative depths, 0x93 materializer zero-reset vs sentinel-fill distinction, out-of-range C738 and invalid flags.

## Immutable-original evidence and test boundary

Original `CODE.2`, 558689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object1 at physical offset `0x3AF24`. New `verify_original_ground_source_projection.py` checks **six independent original byte windows**, covering the source `335C8` 0x800 initialization, `32F4C` 28-byte sentinel writer's depth/main/alternate/tail, and `330E7` actual source materializer material test. All six source windows were independently checked against the original local binary. Original `SINTAB` (333757 bytes, SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`) verifies **C738=127** at +0x1B38. Those original media bytes are *not* included in Git.

The new source-only Python contract is registered as `original_ground_source_projection_binary_contract_tests`, along with the new C++ `original_ground_source_projection_tests`. The existing exported-frame C++ regression has been extended; it still runs under its previous registered CTest. The present CMake source contains **223** `add_test` declarations when Python is available, including other concurrent legitimate source work. These new changes have not yet been compiled/accepted in authorized Windows CI. The last approved Full #658 remains **214/214** on its earlier SHA. Do not quote new source regressions as passing Windows tests.

To replay the immutable-original signature independently:
```sh
python DemoV1/tools/verify_original_ground_source_projection.py \
  --original-code2 /path/to/CODE.2 \
  --original-sintab /path/to/SINTAB
```

## Next integration gates and known limitations

1. Reconcile original `32954` **cache-hit branch**: when camera/heading are unchanged, original may reuse its prior 256 ray records and nearest-depth global. Native traversal currently reruns full tracing on each frame; thus a projection from regenerated rays is not accepted DOS RAM.
2. Verify the original `335C8` → `32954` lifecycle and every original frame-changing producer including dynamic world/visibility branches. Preserve any original caller-order/exception/partial-update state.
3. Only after exact source nearest-depth execution is integrated may native use projected `22220→FF50` to select a *partial* `34A74` row span; preserve source `C026/C027/C028`, overlay `258A8`, `34DE8` render ordering, original 70 Hz pacing and raster state.
4. Compare real DOS VGA indexed buffers/palette/ticks/camera to native at matched poses. Native field frames, authored CTests and source agreement alone are not that acceptance.
5. **No new Windows build, CI workflow edit, Level23 work, completion score or original DOS parity increase** in this batch. Workstream B weighted engineering readiness remains the recorded **49.8%** pending independent DOS acceptance.
