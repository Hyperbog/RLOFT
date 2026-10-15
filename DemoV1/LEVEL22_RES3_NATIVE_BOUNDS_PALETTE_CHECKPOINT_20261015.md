# Level 22 — Native RES3 placement and palette diagnostics

**Date:** October 15, 2026  
**Branch:** `demo-v1`  
**Native source commit:** [`c7099fe41a4cef96a0ad8d4fad976b70c790a972`](https://github.com/Hyperbog/RLOFT/commit/c7099fe41a4cef96a0ad8d4fad976b70c790a972)  
**Windows CI:** [#877, run 38722251801](https://github.com/Hyperbog/RLOFT/actions/runs/38722251801) — native Windows build, source-contract preflight, launcher validation, publishing and **38/38 focused CTests passed**.  
**Published continuous Alpha artifact ID:** `11965758222` (`Ravenloft-Continuous-Alpha-Windows-x64`).  
**Checked-in executable SHA-256:** `0e31a3e16b7cd71894828a3df0fe7fcd51b7459e479b9deb58163a30478eba57`. The checked-in `build-source-sha.txt` identifies the exact compiled source `c7099fe4...`.

## Source-based assessment

The supplied original `SINTAB` is SHA-256 `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87` and contains 127 scale blocks and a bounded 508-entry maximum runtime lookup index. Its actual source-row scale runs vary with projected distance. A single guessed offset for every Level 22 tree would therefore be a risky workaround. The original `CODE.2` is SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; its FE15 shading table spans subtract values 0..8. Neither finding by itself establishes that the current terrain or trees have the correct visual palette on Windows.

## What is implemented

- `EnhancedType7Compositor::draw` optionally returns `EnhancedType7DrawMetrics`: expected opaque output *before* viewport clipping, actual indexed writes, occupied pixel bounds and min/max source palette indices.
- Its row-coordinate arithmetic now uses signed 64-bit accumulation, avoiding overflow for extreme (including mod-authored) offscreen `source_top` values.
- `ExplorationPresenter` retains per-draw original resource ID, source class/index, projected Y origin, visible column count and measured indexed bounds. This metadata is internal provenance, not a stable public mod API.
- The opt-in `ExplorationFrameCapture` TSV writes one `scene_native_draw` row per successfully invoked native scene draw:
  `class, source-index, resource-id, source-top, columns, potential-opaque, writes, left, top, right, bottom, min-index, max-index`.
- Earlier `scene_zero_pixel` and `scene_outcome` records remain, so zero-output draws are distinguishable from a missing sprite source, decode exception and genuine occlusion.
- Synthetic regressions verify bounded/visible sprite pixels, wholly transparent masks, vertically offscreen sprite projections, extreme signed coordinates, bounds/color range and exported TSV rows.

**Not implemented:** a new palette remapping rule, guessed changes to authored tree placement, a player-facing mod registry, or a released mod package loader. The current drawing result is intentionally unchanged.

## Field reproduction boundary

For diagnostics on a locally configured copy of the game, `DemoV1/Windows/Capture-Level22.cmd` launches an opt-in Level 22 preview with bounded frame capture. The `*.tsv` files are under `%TEMP%\\RLOFT-Demo-Captures\\<session-id>`; `scene_native_draw` entries provide the drawn bounds and palette-index ranges. This shortcut uses the **developer Level 22 path**, not a normal end-to-end character creation and save-state verification.

Interpret the telemetry conservatively:

1. `potential-opaque > 0` with `writes = 0` indicates opaque scaled input but no pixel survived the clipped viewport, a strong vertical-placement clue when the existing visible X columns are valid.
2. `potential-opaque = 0` identifies input rows/columns with no opaque output at that scale; this is not evidence of a wrong tree coordinate.
3. `writes > 0` records actual sprite drawing, but it is **not** proof that its tree/NPC is at an artistically correct position, palette ramp or depth.
4. Min/max sprite index does not prove how palette lookup, RES8 shading, original RES4 ground shading or on-screen RGB combine. Use the matching indexed frame and native screenshot before modifying colors.

## Next source-verifiable action

Capture consistent Level 22 pose/facing on the **exact-source** Windows build; compare native `scene_native_draw` bounds and final indexed image with original authored positions and the user's current screenshot. Correct the first confirmed root cause in the geometry projection, palette/texture lookup or compositing order. Avoid world-specific hardcoded offsets; new resources should eventually pass through typed stable-ID resolution under [the mod-forward policy](../MOD_FORWARD_METHODOLOGY.md).

**Release:** still **Pre-Alpha**. Existing `ALPHA-BUILD` evidence remains met; Level 22 visuals, inventory, automap and menu acceptance remain open. Neither passing CI nor internal telemetry establishes owner field acceptance.
