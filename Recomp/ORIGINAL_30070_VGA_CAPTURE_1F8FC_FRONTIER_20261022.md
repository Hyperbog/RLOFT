# Part 27 — Original 30070 VGA capture and 1F8FC callback frontier (2026-10-22)

**Branch:** `Recomp`. **Validated implementation:** [`ed4da647cbe2893e7987aaeb683cc1c7dce57250`](https://github.com/Hyperbog/RLOFT/commit/ed4da647cbe2893e7987aaeb683cc1c7dce57250). **Status:** Original `30070 -> 292BC -> 2A114` VGA capture built and source-tested; original `1F8FC` compositor **NOT YET connected end-to-end**.

## Verified source implementation

- `Recomp/src/original_code2_video_boot.hpp` now executes the actual reconstructed `30070` (image metadata, guest allocation), original `292BC` (screen-bound rectangle), original `2A114` (planar VGA plane-major capture) against the retained VGA device. No host-authored art or resource image is substituted.
- Validates requested VGA rectangle, memory range, record byte count, image header (frame count, row count, byte units, format) and original source capture callback count. Original captures are designed for (row, x, rows, pixels, key), not an arbitrary RGBA buffer.
- Source `0x30040` image release calls original `310B0` guest heap release. Original guest allocation itself remains the original `3104C` callback, not an independent host allocator, in the live adapter.
- The original RES0 image base `2E510 -> 2E4A4 -> 2939C -> 2A380`, `2EFD0` cache cleanup, `30070` capture, `30040` image release, original level-mode `325B0` palette and `29234` DAC, source `22D70`, `2F868` and `30B2C` finalizers are prepared on the retained `Application45Boundary`. They are **not** evidence of a completed live `1F8FC` scene.
- Level 22's original source requests scene mode **4** after retained mode **3**. The bootstrap still intentionally leaves `Application32Boundary.call_1f8e0` absent to avoid executing an incomplete HUD/world graph, which would silently skip original routines. The real `1FC14` loop remains executable.

## 30070 all-plane pixel parity regression

The synthetic, media-free `Recomp/src/test_original_code2_1f8fc_17a68.cpp` directly executes reconstructed `30070` and verified `292BC/2A114` on the original video device for the **three original 1F8FC capture rectangle geometries**:

| Rectangle | row | x | rows | pixels | Expected 30070 image body |
|---|---:|---:|---:|---:|---:|
| First | `0` | `0x20` | `4` | `256` | `1024` bytes |
| Second | `0x104` | `0x20` | `4` | `256` | `1024` bytes |
| Third | `0x106` | `0x20` | `11` | `256` | `2816` bytes |

Each captured guest buffer byte for all 4 VGA planes is compared to a separate deterministic VGA pixel generator. This demonstrates source capture pixel parity with **synthetic known images**, **not** independent original DOS scene parity. The capture image header is written by original CODE.2.

The first test incorrectly expected the accumulated per-Bridge counter to equal 1 after every image and failed; diagnosis showed **2 valid callbacks** after the second capture with all header fields and VGA state correct. Commit [`1e6e57b8`](https://github.com/Hyperbog/RLOFT/commit/1e6e57b83d278010b7a99454d534fb18b8896057) fixes the assertion to check one increment **per capture**, maintaining sequential capture provenance. This regression now passes across three rectangles.

## Reproducible CI evidence

- [Windows original-media milestone #39861662162](https://github.com/Hyperbog/RLOFT/actions/runs/39861662162): **PASS, 86/86 CTests**; retained original Level 22 and original `1FC14` input checks pass.
- [Linux regression #39861662119](https://github.com/Hyperbog/RLOFT/actions/runs/39861662119): PASS.
- [Canonical CODE.1/CODE.2 source-link #39861662170](https://github.com/Hyperbog/RLOFT/actions/runs/39861662170): PASS.
- Windows original-media log confirms `requested=4 retained=3 original_30070_capture=SOURCE_VGA_BRIDGE_AVAILABLE scene_rendering=NOT_CONNECTED`.

## Exact remaining blockers to a complete 1F8FC

1. **Source `Application44Boundary` HUD and world:** real `1F494` party/HUD, `1F1C8` world view (including raycaster `334C4` and RES3/RES4 column draw), and `1F120` side effects. Currently `presentation.app44` is null, so turning on `1F8FC` now would silently omit these branches. Full source-backed callback graph must be attached before enabling the live mode-change callback.
2. **Source `1A5B0` cursor/game page presentation:** original dual-page mouse/cursor state, `2A020` page copy and `1AB20` background handling, with source `291C8` scanout, `2E590` push/`2E5D4` pop. The already-proven boot cursor draw alone does **not** prove this update path.
3. **Actual scene palette bank 4:** `325B0` source loader/DAC callbacks are attached with checked mode transition; the arbitrary *second* bank load has not yet been executed as part of the source compositor, so mode-4 image and palette parity are still pending.
4. **Continuous frames:** wire `1FC14 -> 20B40` world update and `1A510` source presentation, and verify at least two distinct, input-driven original VGA frame buffers against separately captured original DOS pixels and DAC6 palettes. Currently there is only original input accumulator motion and source page state, not world-coordinate changes.

Do not claim playable native executable, complete scene renderer, independent DOS pixel parity, original actor AI/combat, or R04 acceptance. Commercial original media is not committed.
