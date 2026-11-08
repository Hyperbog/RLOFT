# Retained original 36CEC sprite raster and complete 2C294 scene return

## Scope and status

This gate concerns the canonical reconstructed CODE.2 source running inside the **same retained Level 22 guest and VGA host** as original `1F8FC`. It is not a substitute renderer, retail DOS pixel parity test, or a completed gameplay frame.

**Accepted integration commits**
- [0c6e0b3f5eea](https://github.com/Hyperbog/RLOFT/commit/0c6e0b3f5eea5b7a1bdc08d16ea953ed2268726d) — connect original RES3 36CEC/36B14/369D8 raster to retained source guest memory and VGA.
- [1730ea1df39e](https://github.com/Hyperbog/RLOFT/commit/1730ea1df39e050f7166bd387764ce31d911647f) — link the previously independently tested renderer into the original CODE.2 startup target.
- [951fb9c4da44](https://github.com/Hyperbog/RLOFT/commit/951fb9c4da44c01f3875a7938f290b5c09864e6f) — require actual complete `2C294` return, followed by the deliberate `274A4` frontier.
- [598c62ae7693](https://github.com/Hyperbog/RLOFT/commit/598c62ae7693b17915fc96fb6c54679175cbefb2) — tighten the Windows predicate to require *nonzero* source raster runs, VGA plane passes, and indexed sprite pixel writes; zero off-aperture writes. Verify the corresponding newer workflow independently.

**Accepted workflow evidence at commit 951fb9c4da44:** [Windows original-media run 40055360687](https://github.com/Hyperbog/RLOFT/actions/runs/40055360687) — success. Linux C++ portable regression and canonical CODE.1/CODE.2 source link also passed for the sprite-source integration.

## Original source call graph

`1F8FC → 1F1C8 → 34DE8 → 33E6C → 34110 → 1BDAC → 2C294 → 335DC → 36CEC → 36B14 → 369D8 → 2C294 returns → 274A4 (next deliberate stop)`

`2C294` passes six authentic cdecl arguments to `36CEC`: horizontal position, RES3 ID, projected size, lookup bias, vertical mode, and ray depth. The callback reads the decompressed *original* RES3 body using source `2F29C` and executes the pre-existing bounded SINTAB/planar VGA renderer with the live guest `258C4` lookup, `18FE0` generated scale table, `15A18` ray records, and source CRTC/draw page. Temporary generated SINTAB patches are staged and not written back as permanent code changes. The prior `335DC` ray/wall pass runs in the same scene.

The exception path is fail-closed and logs the actual RES3 ID, projected size, source sprite body bounds, SINTAB end and error. Neither a generated sprite shape nor a fabricated actor position is accepted.

## Windows original-media evidence

Exact `CODE2_ORIGINAL_LIVE_BOOT=1F8FC_COMPOSITOR_CHAIN` diagnostic in the successful Windows run:

| Measurement | Original guest observation |
| --- | ---: |
| Backgrounds / authentic releases | 2 / 2 |
| Original VGA captures | 3 |
| Exterior sprite draws | 37 |
| World cells / quad projections | 285 / 28 |
| Polygon quads / scanline draws | 8 / 212 |
| Original actor `1BDAC` returns | 1 |
| Original scene `35798` projection callbacks | 88 |
| Original RES3 `21BF0` extent requests | 1 |
| Original `335DC` wall passes | 1 |
| Original `36CEC` sprite requests | 175 |
| Completed source sprite raster callbacks | 175 |
| Sprite ray-visible raster runs | 1,919 |
| Sprite VGA plane passes | 3,036 |
| Indexed sprite pixel writes | 41,231 |
| Writes outside VGA aperture | 0 |
| Original `2C294` scene returns | 1 |
| Original `1A5B0` presentations | 0 |
| Completed full gameplay frames | 0 |

Acceptance marker: `next=274A4_COMPASS_OVERLAY`, `presentations=0`, `complete_frames=0`.

## Next frontier

1. Execute the original `274A4` compass/overlay path with authentic resource, clipping, row and VGA callbacks. Preserve the same guest and source execution order.
2. Return from `1F1C8`, then complete the original frame update `1F120`, palette switch `325B0`, and presentation `1A5B0` **in the same 1F8FC call**.
3. Verify page selection and multiple input-driven frames against independent retail DOS pixel captures. Do not assert DOS screenshot parity before it is measured.

Completed frames and presentations remain zero despite the successfully drawn original RES3 scene sprites.
