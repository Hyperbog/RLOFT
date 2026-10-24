# Authentic 29804 clipping and persistent 1FC14 source scenes — 2026-10-24

## Current verified facts
- Windows original-media [run 40059968644](https://github.com/Hyperbog/RLOFT/actions/runs/40059968644) passed with original `29804` return 2 (one valid raster), return 0 (authentic wholly clipped sprite), no invalid VGA access or unsupported read.
- Persistent retained `1FC14` scene callback graph executed four scene returns and four alternating `1A510` CRTC/page handoffs, including keyboard scan codes 0x148 (forward) and 0x14B (turn left).
- Scenes contain eight original polygon quads, 212 polygon scanline draws, original 1BDAC update, 175 RES3 sprites, original compass, and 41,231, 41,231, 41,231 and 41,208 indexed sprite writes respectively.
- The next original source scene stops deliberately at `ORIGINAL_314B0_SOURCE_VIEW_OUTSIDE_ORIGINAL_TABLE`. Do not relax actor-view bounds or treat table pointers as fabricated geometry.
- [cac0555a9cf2](https://github.com/Hyperbog/RLOFT/commit/cac0555a9cf2e7c2249486d67ebd9dd0a83ade10) adds diagnostic detail on the invalid original actor-view call: guest 258A8 view value, current keyboard scan code, source heading, scene return count, and original 2B764 heap-table pointer. It does not alter rendering or accepted bounds.
- Current source-validation gates: [Windows 40063321429](https://github.com/Hyperbog/RLOFT/actions/runs/40063321429), [Linux 40063321576](https://github.com/Hyperbog/RLOFT/actions/runs/40063321576), [canonical link 40063321442](https://github.com/Hyperbog/RLOFT/actions/runs/40063321442) — all passed.

## Important acceptance distinctions
- Four `1FC14` **scene** returns are not four complete `1F8FC → 1A5B0` gameplay frame presentations. The source `1A510` hardware handoff is verified, but follow-on HUD and sound behavior is not yet certified.
- Accepted one-shot full original `1F8FC` frame count stays **one**.
- Recompiled selected 320×400 indexed scanout SHA-256 is `b9649e13444cbdef42406a5077177ff0e15bb89325269067df219fbfbd85ef01`. Original-source RGB6 DAC SHA-256 `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812` matches independently captured original DOSBox-X palette.
- Source raw `.idx` and `.pal` exports are supported with `RLOFT_RECOMP_PRIVATE_FRAME_STEM`. Windows CI compares them to live hashes and deletes bytes; no raw art is uploaded.
- Independent synchronized original **Level 22** indexed DOS snapshot for same camera, tick and CRTC scanout is not yet available; indexed pixel parity is **OPEN**. Use the existing `Recomp/tools/compare_dos_indexed_frames.py` only after obtaining matching genuine DOS and reconstructed raw indexed buffers.

## Next frontier
1. Review the newly recorded 314B0 view value and whether original `258A8` is a view code or table index on the next keyboard-driven tick. Trace original angle/turn source and the two-level original heap pointer `2B764` before deciding legal range. Preserve fail-closed behavior.
2. After legal source actor-view handling is established, verify original 1FC14 post-scene HUD/audio callbacks and page state across at least two input-driven cycles. Keep a distinct `scene_returns`, `1A510_handoffs`, and `complete_1F8FC_frames` ledger.
3. Obtain a temporally synchronized retail DOS Level 22 VGA indexed capture and compare actual selected 128000-byte VGA indices and 768 DAC bytes without fabricated art.
