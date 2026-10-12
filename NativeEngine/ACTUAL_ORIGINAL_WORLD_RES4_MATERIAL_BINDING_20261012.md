# Original 32-level surface material → RES4 native rendering gate

**Date:** 2026-10-12. **Scope:** strict source candidate-resource binding and one original-data indexed texture column. This is not DOS raycast, camera, wall-orientation branch, or full-frame parity.

## Verified committed source

NativeEngine commit [`06c821e2570b6300fb8b894fe992c84a3ccf3377`](https://github.com/Hyperbog/RLOFT/commit/06c821e2570b6300fb8b894fe992c84a3ccf3377) introduces:

- `native_world_materials.hpp/.cpp`: reads **all five original level-header banks of 64 words per level** from `LEVELS.DAT`. Four banks are original wall/surface RES4 candidate IDs. Fifth bank is retained verbatim as the original 16-bit marker and **is not treated as RES4 texture material**.
- Exhaustive catalogue validates every raw candidate across all 32 maps against the true RES4 wide texture bank from `INIT[0x0e]`; independently decodes every distinct referenced RES4 EH6 texture and validates 64×128 dimensions.
- `draw_native_world_surface_column` composes `NativeWorldRuntime` original/mutable tile code, already-researched special-frame/neighbor and state variant substitution, original map candidate table lookup, EH6 texture decoder, original CODE.2 338C8 integer fixed-point sampler and safe four-plane VGA output.
- Which of the **four** possible candidate banks is selected, ray viewpoint, texture column, projection height, shade and display destination are **caller-provided explicitly**. There is no false inferred original DOS camera/wall orientation or screen-frame pixel parity.
- New `native_world_materials_tests.cpp` uses asset-free synthetic 32-level and EH6 fixtures, validates true RES4 resource ID zero (not absent), page/plane pixel values and all-or-nothing failures for invalid bank, source column, height, archive boundary and tampered fifth-bank data. New read-only `rloft_native_world_materials_audit` checks actual-original source and writes metadata only.

[Native Linux Actions #38206444852](https://github.com/Hyperbog/RLOFT/actions/runs/38206444852) compiled that exact source with **25/25 CTests PASS**. Original copyrighted files were **not** included in Git or CI.

## Executed against authentic original LEVELS.DAT/RES4/INIT

The exact Linux executable from successful source-only Actions was downloaded and run **locally** against user-owned original `LEVELS.DAT` and `RES4` plus original `INIT` (with `RES0`/`T` present for original-folder recognition).

Pinned SHA256:
- LEVELS.DAT `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- RES4 `d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46`
- INIT `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276`

| Authentic original source proof | Validated |
|---|---:|
| Source level maps | **32/32** |
| Original map code/texture-bank candidate slots | **8,192/8,192** (32×64×4) |
| Nonzero candidates / zero resource IDs | **2,894 / 5,298** |
| Distinct original 64×128 wide EH6 resources actually decoded | **452** |
| Original INIT wide RES4 bank | **600 entries** |
| Fifth preserved level-header field | **0xB5B1** uniformly, 2,048/2,048 cells in bank 4 |
| Invalid candidate material IDs or EH6 decoder failures | **0** |
| Original sample | Level 0 cell (6,0), source code 1, candidate bank 0, RES4 ID **62** |
| Original sample raster | 128 source index reads / 128 individual VGA-plane writes |

A **separately written Python original-byte parser and LZSS EH6 decoder** independently checked every original level table, decoded all **452** unique wide EH6 resources, and recreated the authentic source sample with source column 3, viewport x=9, height 128, shade byte 5 and page 0. Exactly **32/32 per-level SHA-256 fingerprints** and the original aggregate source hash matched compiled C++; the independently generated full four-plane pixel hash matched too. Zero divergences.

Source-domain fingerprints:
```
RLOFT-LEVELS-RES4-v1
565ab170002aa1fcbbf706e6fb3c64429e0d6778f8651f72769639cf6df0a8a5

Original RES4 selected-cell VGA four-plane diagnostic:
b1f1b0d4cfc39b4c78c4121bef0b2dd39ef783700a07abc3a6a06292b9f6e028
```

Local-only metadata comparison `rloft_world_res4_independent_comparison.json`, original compiled report `rloft_world_res4_source_audit_06c821e2.json`, and local independently written source verifier `rloft_world_materials_independent.py`. No original game data copied to source, reports or CI.

## Boundary and next gameplay work

**VERIFIED:** LEVELS.DAT original header/conditioned cell code → one of four source candidate IDs → actual original RES4 EH6 compressed texture → safe 16.16 texture column → original indexed-color four-plane output under specified draw inputs.

**NOT VERIFIED:** original CODE.2 camera-generated ray/viewpoint/side selection, active bank choice among the four candidates, projected wall height, shade and clip state, overlay/event conditions, wall-to-sprite depth/occlusion/order, whole DOS frame color/palette equivalence, playable gameplay, actor/VM/save state and current-source Windows release. These must be obtained from CODE.2 instruction semantics and original DOS traces, not inferred from the host debug DDA view.

**Next implementation:** recover original CODE.2 32F93 / 33647 wall candidate-bank selection, tile orientation, distance/height and shade/clip call arguments, then feed authenticated runtime ray results through this new integrated material renderer and the existing EH7/SINTAB/VGA compositor. Windows release workflow stays manual-only and DemoV1 gameplay stays unlinked.
