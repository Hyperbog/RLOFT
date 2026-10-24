# Retained original 33E6C/34110 polygon source execution — Windows CI acceptance

**Scope:** exact source reconstruction of *Ravenloft: Strahd's Possession*, not a native replacement renderer.

## Accepted implementation

- Source integration commit: [40d6e69af81f](https://github.com/Hyperbog/RLOFT/commit/40d6e69af81f73d2a030fa6182788f868ff6e702).
- Initial Windows original-media acceptance gate commit: [b6c29c16681a](https://github.com/Hyperbog/RLOFT/commit/b6c29c16681a0c78b57c1b53dcbbcf33d22abe9b).
- Windows original-media validation: [run 40053542133](https://github.com/Hyperbog/RLOFT/actions/runs/40053542133), **success** (verified job logs at commit b6c29c16681a).
- Linux C++ regression: [run 40053526754](https://github.com/Hyperbog/RLOFT/actions/runs/40053526754), **success**.
- Canonical CODE.1/CODE.2 Windows link gate: [run 40053526860](https://github.com/Hyperbog/RLOFT/actions/runs/40053526860), **success**.
- Hardened follow-up acceptance requires nonzero `polygon_drawn_scanlines` in commit [239673573c25](https://github.com/Hyperbog/RLOFT/commit/239673573c25e3767dab0b117716542ddfa40314). Verify the subsequent run independently; do not transfer the original run's acceptance status to the later commit.

## Original source call chain

`1F8FC → 1F1C8 → 34DE8 → 34BB0 → 33E6C`

The retained original guest owns the level, resources, projected vertices, source polygon edges and scanline raster. `Runtime103Boundary` now supplies:

- `rep_movsd_ds_es`: eight original guest dwords, bounds checked;
- `call_3402c`: source edge setup;
- `call_33fb4`: source polygon step;
- `call_34110`: source scanline through the previously integrated real VGA aperture bridge.

Each source-created quad is copied into a 32-byte bounded guest scratch argument for synchronous `function_33e6c`, and the scratch area is restored upon normal return. No host texture sampler or fabricated terrain polygons were introduced.

## Observed Windows retained-compositor counters

Exact `CODE2_ORIGINAL_LIVE_BOOT=1F8FC_COMPOSITOR_CHAIN` line in Windows job 120751317908, run 40053542133:

| Measurement | Observed |
| --- | ---: |
| RES0 background draws | 2 |
| Resource releases | 2 |
| Captures | 3 |
| Exterior world sprites drawn | 37 |
| Clipped sprite rasters | 5 |
| RES4 original resource requests | 9 |
| Oblique source rasters | 1 |
| 34DE8 world cells examined | 285 |
| 35798 vertex projections | 28 |
| 33E6C quads | 8 |
| Completed 34DE8 map scans | 1 |
| REP MOVSD vertex copies | 8 |
| 3402C polygon edge callbacks | 14 |
| 33FB4 polygon stepping callbacks | 283 |
| 34110 scanline callbacks | 212 |
| Scanlines with source raster draws | 212 |
| Presentations | 0 |
| Completed original gameplay frames | 0 |

End-of-probe marker: `next=1BDAC_ACTOR_UPDATE`. The source polygon pipeline returned. The next `1BDAC` callback still deliberately stops the integrated diagnostic.

## Important non-claims

The game does **not** yet display a complete original gameplay frame. This gate proves retained original polygon source traversal/raster execution and safe return to the next dependency boundary. It does **not** prove raster parity against a retail DOS screenshot, nor input-driven consecutive frames.

## Next source frontier

1. Connect original `1BDAC` actor update, then `2C294` and `274A4` scene composition with explicit original source ABI and resource callbacks.
2. Continue through `1F120` frame update, `325B0` palette switch, and `1A5B0` presentation **in the same `1F8FC` call**.
3. Validate page selection, source VGA pixel output against DOS evidence, and at least two input-driven consecutive completed frames.
4. Require `presentations>0` and `completed_frames>0` before any full-frame claim.
