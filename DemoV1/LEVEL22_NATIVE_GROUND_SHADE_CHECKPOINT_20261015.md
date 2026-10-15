# Level 22 — Enhanced PC ground palette underflow

**Date:** October 15, 2026. **Active branch:** `demo-v1`.  
**Reference files:** Owner-provided `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; [DnD-Classics original COLORS/INIT/RES4](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER/Ravenloft%20-%20Strahd's%20Possession/RLOFT). Original proprietary file bytes are not committed to this branch.

## Defect class, evidence and intent

The original CODE.2 ground shade table at DGROUP FE15..FF14 has
subtract values 0..8. The native Level 22 lower-floor renderer follows
the original `34A74` sampler: each RES4 indexed texel is reduced by the
FE15 shade for its perspective row. With 8-bit unsigned subtraction,
a texel less than the decrement wraps to indices 248..255, not to
the neighboring dark colors. The original `COLORS` Level 22 palette
(index 4 from `LevelDisplayModel`) maps some of these high indices to
bright cream, orange or other unrelated hues.

For the **enhanced Windows PC rendering only**, this change caps that
specific underflow to palette zero. This is an **intentional native
visual enhancement**, not a claim that DOS performed saturation.
It can affect output only where actual source texels are below the
original per-row shading decrement. The number of such pixels in a
real Level 22 session is **not yet measured or field accepted**.

## Architectural boundary

- `GroundTextureRasterSpec::shade_policy` provides two explicit typed
  policies: `VgaWrap` (default, faithful to the original 8-bit SUB)
  and `ClampToBlack` (enhanced native presentation).
- `ExplorationPresenter` opts into clamping for its native RES4 ground.
  It does not change world geometry, collisions, depth, floor sampling,
  source media, SINTAB perspective or RES3 sprites.
- `GroundTextureRasterReport` separately counts source samples and
  destination pixels that **would** underflow even in VGA reference mode.
- `ExplorationFrameCapture` emits `ground_shade_policy` and
  `ground_shade_underflow_pixels` in the existing TSV. The latter is
  zero if the active source texture never reaches an underflowing shade.
- C++ tests retain original-wrap checkpoints while testing the native
  64-texel horizontal repeat, the row-80 transition to 2-pixel paired
  VGA lanes, clamped output, out-of-viewport isolation and rejection of
  unknown policy values before framebuffer writes.
- The automated enhanced Level 22 source contract now rejects dropping
  the native clamp explicitly.

This is an internal replaceable render policy, **not** a public stable
mod-package schema or shipped registry. Future palettes or mods should
select a validated policy through the canonical content resolver without
redefining original source file offsets.

## Verification and remaining work

The CI source-contract proof, Windows build and focused tests remain
separate from source-matched real-game field verification. Previous CI
`#877` had passed 38/38 before this change. The new 15-mutation
source verifier and changed CTests should be reported against the **new
source commit** when GitHub Actions has completed; no previous CI success
is presented as evidence for this new change.

A Level 22 field report needs a matching indexed capture, palette file
and native `ground_shade_underflow_pixels` value. If it is zero, this
change will not visibly affect that captured frame and the remaining
palette discrepancy is elsewhere (RES8, palette selection, wall shading,
RES3 artwork or mixing). **Tree positions, sprite appearance, inventory,
automap and top-menu field acceptance remain open**; do not close
release gates from this change alone.
