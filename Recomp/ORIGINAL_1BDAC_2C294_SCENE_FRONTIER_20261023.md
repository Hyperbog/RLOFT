# Retained original 1BDAC actor update and 2C294 scene projection checkpoint

**Scope:** exact original CODE.2 runtime callbacks within the retained level-22 1F8FC compositor, not a substitute renderer or a playable release.

## Source integration and acceptance

- Accepted retained actor-update source: [8ec17bd31015](https://github.com/Hyperbog/RLOFT/commit/8ec17bd31015debba771932884607ddcc9bb7f86).
- Accepted original scene projection/wall source: [9bdc33329d07](https://github.com/Hyperbog/RLOFT/commit/9bdc33329d07e2d96b7048fa96ab8878d1bf38a7).
- Windows original-media actor gate: [run 40054708619](https://github.com/Hyperbog/RLOFT/actions/runs/40054708619), success.
- Windows original-media scene gate: [run 40054938868](https://github.com/Hyperbog/RLOFT/actions/runs/40054938868), success.
- The stricter level-22 first-sprite acceptance gate is committed separately as [a567d4d80d95](https://github.com/Hyperbog/RLOFT/commit/a567d4d80d95b941e4645f10a8391d89a572dfd5); its subsequent run must be checked independently.

## Accepted original source path

`1F8FC → 1F1C8 → 34DE8 → 34BB0 → 33E6C → 34110 → 1BDAC → 2C294 → 335DC → 36CEC (frontier)`

1. `1BDAC` runs authentic actor timer, orientation, and movement-branch source with retained actor records. `311E0` reads the guest timer; `290D0` invokes the source RNG seed update; `2DF2C` computes the original movement vector. `30D30 → 379D0` performs the original scratch fill. Unsupported collision/actor effects remain explicit fail-closed callbacks. Level 22 returns from `1BDAC`.
2. `2C294` runs original primary/secondary/transient sprite-scene projection using original `35798`, cached `2F29C` RES3 access, `21BF0` sprite extents, `30CA2` random state, `30CC4` source seed, `30700` clock, and the original `335DC` ray/world-wall pass.
3. The first original `36CEC` visible-sprite callback is reached and deliberately stops. No host-generated sprite pixels or fabricated geometry are counted as original output.

## Exact retained-frame observations

Observed in Windows original-media job log for [run 40054938868](https://github.com/Hyperbog/RLOFT/actions/runs/40054938868):

| Source measurement | Observed |
| --- | ---: |
| Background draws and releases | 2 each |
| VGA captures | 3 |
| Exterior sprite draws | 37 |
| RES4 resource requests | 9 |
| World map cells examined | 285 |
| Projected polygon quads | 8 |
| Polygon scanline draws | 212 |
| Original 1BDAC update returns | 1 |
| Original 2C294 35798 projections | 88 |
| Original RES3 21BF0 extent callbacks | 1 |
| Original 335DC wall passes | 1 |
| Original 36CEC sprite draw requests | 1 |
| 2C294 full returns | 0 |
| Full-frame 1A5B0 presentations | 0 |
| Completed original gameplay frames | 0 |

The scan/raster pipeline reaches a real sprite request but **not** a complete frame. The expected diagnostic stop marker is `next=36CEC_SPRITE_RASTER`.

## Next frontier

Integrate the original `36CEC` scene sprite pixel callback with original RES3 resource data, SINTAB, and retained VGA, reusing separately tested original raster/occlusion logic only after validating its ABI and pixel effects in this exact guest. Then let `2C294` return and continue through `274A4` overlay, `1F120`, `325B0`, and `1A5B0`. A full-frame acceptance gate must demand nonzero source presentations and reference-DOS pixel parity; a scene/frontier gate does not satisfy it.
