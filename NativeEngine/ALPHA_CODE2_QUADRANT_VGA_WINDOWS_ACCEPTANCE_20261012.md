# Alpha 0.7: original CODE.2 quadrant + 338C8 planar VGA in the interactive native engine

**Date:** 2026-10-12 Eastern / 2026-10-13 UTC.  
**Product:** independent C++ `rloft_native.exe` replacement-engine preview (never `DemoV1` or original DOS x86 execution).  
**Canonical gameplay/engine source:** `native-engine:d818b6424f9c5f30ed79072f2d72fd539e2a6938`.  
**Actual Windows compiled SHA:** `native-alpha-code2-vga-acceptance-20261012:2f5937c9c6a5d60f6264e52ca17e4662b1c84ecc`. The latter differs from native-engine source only by its one-off authorized Windows acceptance workflow. Normal `native-engine` Windows workflow remains manual-only.

## Functional advancement in the user-facing executable

Alpha 0.6 directly interpolated vertical original RES4 textures with a **host formula**. Alpha 0.7 removes that synthetic sampler from the actual keyboard-controlled Windows world preview.

New source-backed pipeline in `NativeEngine/src/native_probe_original_textures.cpp`:

1. User's immutable original `LEVELS.DAT` -> typed per-level cell/code/material candidate tables, original collision and mutable runtime.
2. **PROVISIONAL** host DDA and user camera generate rays and expected view column/distance; original CODE.2 traversal/phase calculation remains to be reconstructed.
3. Source-verified CODE.2 `32954` **four quadrants** classify the incoming approximate ray angle and derive original signed 16.16 step words at `0x25610/0x25604` and exact x/y wall mirror decisions. Native UI still converts host radians to approximate original tenth-degree ray angles; this is *not* a claim of original CODE.2 `0xC714+0xC73C` live angle provenance. The live choice of **which** of the original dual wall texture words to display remains a host face guess.
4. Authentically decompressed original 64×128 `RES4` EH6 wall source columns are cached; original wall source bytes remain readonly.
5. CODE.2 `338C8` original **integer 16.16** sampler uses `0x800000 / projected_height`, indexed color-to-VGA write and authentic four-plane 80-byte-per-scanline, sequencer-masked storage. Host-supplied projected height is bounded to **1..256**. Actual original CODE.2 projected heights and shading are still upstream work. The source sampler is `draw_native_type6_columns`, not an invented nearest-neighbor approximation.
6. Original CODE.2 32-level `COLORS` bank mapping and `scanout_native_vga_rgb6` convert exact 6-bit palette values to a true **320×400** indexed VGA-derived window. Host display expands DAC6 values to 8-bit Windows BGRA without altering source indexed pixels.
7. Flat background palette indices `1/2` remain a host design choice, **NOT original ceiling/floor rasterization**. The GUI explicitly warns **ORIGINAL EH6 + CODE.2 VGA / PROVISIONAL CAMERA — NOT DOS FRAME**.

All 32 original maps remain selectable; W/S/Up/Down movement, A/D strafing, Left/Right turning, R restart, PageUp/PageDown level select and Esc close remain integrated. This feature adds no original copyrighted data to the output EXE.

## Exact source byte provenance

Owner-uploaded `CODE.2`: SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Source object 1 at physical offset `0x3AF24`, length `0x3A852`, SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

| Original object 1 half-open x86 range | Bytes | SHA-256 |
| --- | ---: | --- |
| `32954..32B80`: source ray quadrant setup + original per-ray origin preparation | 556 | `25bce99ea0f964592782884e51f715d1cb6f1c777baea90abe4e6c34633c46aa` |
| `32A19..32B75`: signed ray quadrant fields and original sine-index branch setup | 348 | `a9499c782074b67ee76a3e9cd8fe949a296ec7f79dbba78892797cca23577d4b` |
| `32F8C..33122`: original x-wall pair emission and mirroring | 406 | `57fe0d8c39e1b19e52b567db39df43c04131f3d3ab50ef8d0c0fb72a684825ae` |
| `33124..332B3`: original y-wall pair emission and mirroring | 399 | `a3f6583594908a90514a3f33ccd54f72b9660a464b7e13d2a2f202432b438a7f` |

New pure C++ helper `native_code2_ray_quadrant.{hpp,cpp}` reproduces the original immediate threshold boundaries `0x384=900`, `0x708=1800`, `0xA8C=2700`, `0xE10=3600`. A **single targeted 3,600-case regression** verifies every source-angle choice without adding unrelated reconstruction test suites.

| Original angle (tenths of degree) | x signed 16.16 `25610` | y signed 16.16 `25604` | X-path mirror | Y-path mirror |
| --- | --- | --- | --- | --- |
| 0–899 | `+0x10000` | `-0x10000` | No | No |
| 900–1799 | `-0x10000` | `-0x10000` | Yes | No |
| 1800–2699 | `-0x10000` | `+0x10000` | Yes | Yes |
| 2700–3599 | `+0x10000` | `+0x10000` | No | Yes |

Source `32FA5` reflects the x-wall column when signed x step is negative; source `3313D` reflects y-wall column when signed y step is positive. This is *only the verified quadrant/sentinel stage*; the rest of `32954..32F4B` traverses original live VGA/world state and is **not** represented as implemented.

## Actual Windows source-media acceptance — PASS

[Successful owner-authorized Windows/MSVC x64 build #38327641005](https://github.com/Hyperbog/RLOFT/actions/runs/38327641005) compiled all NativeEngine Windows binaries, ran **33/33 Windows CTests PASS**, checked authentic owner-controlled original files from `DnD-Classics` via sparse checkout (six files: `LEVELS.DAT`, `RES4`, `COLORS`, `INIT`, `RES0`, `T`), and ran `rloft_native_original_preview_audit.exe` against all 32 original maps with 320×400 frames, source quartet wall reflection and real code-derived EH6 fixed-point VGA drawing. Every source file's SHA-256 was checked before/after and **unchanged**.

| Real original-media result | Count |
| --- | ---: |
| Source maps loaded | **32 / 32** |
| Levels producing authentic-textured walls | **32 / 32** |
| Source-textured wall columns | **10,097** |
| Actual source original RES4 texel reads | **1,167,450** |
| Distinct original EH6 64×128 texture records decoded | **97** |
| Native CTests on Windows | **33 / 33** |
| Native CTests on Linux [#38327608651](https://github.com/Hyperbog/RLOFT/actions/runs/38327608651) | **32 / 32** |
| Original files modified | **0** |

All 32 authentic 320×400 BGRA preview frames together have SHA-256:  
`613097b3f688314682de29b30d804697d4af794b131feb93b21c8e04bd6d2520`.

A new end-to-end audit assertion requires that both the original CODE.2 sampler and original quadrant helper are actually enabled in every rendered original view. The native host also tested bounded camera turn and movement for every original level. This is a proper real-media functional smoke, **not a visual DOS reference-frame comparison**.

## Verified Windows distribution

[Source-identified Windows GitHub Actions artifact #11781826386](https://github.com/Hyperbog/RLOFT/actions/runs/38327641005/artifacts/11781826386), expires per GitHub retention. Downloaded artifact ZIP opened and rehashed locally; its inner six-file distribution ZIP, three Win64 PE binaries, SHA256SUMS and manifest were independently checked. No game resources or saves bundled.

| Exact item | SHA-256 |
| --- | --- |
| Windows `rloft_native.exe`, actual compiled code | `1c3879b8463a0b504d1b593af2105482fca8831b31f565df734b106dd0f77747` |
| Official inner six-file Windows ZIP | `be082105f22db734eebedab13e14ce80a83c8c769e742bb95e0bf11ffeb306be` |
| Outer GitHub artifact ZIP | `73237e9af1f34798961c273e0b5ed17bc9155e6503f37a9ccf84c5cb37d25d49` |
| User-friendly repackaged seven-file Alpha 0.7 ZIP (extra README only) | `d243597e6cf9fd217f218a422c8777dca5b2843835ca9dcb8362690e12a9c578` |

## Still open: original camera and actual game

The original `32954..32F4B` DDA traversal, live per-ray source angle, actor/special-cell handling and original shading/depth/channel update logic are **not** fully implemented. The GUI uses a host DDA to supply original sampler calls, makes an unverified first/second wall material choice and generates host heights/background. The original DOS 320×400 visual reference, CODE.2 draw scheduling, live EH7 sprite placement/occlusion, UI/party/events/AI/combat/audio/saves and first-playable game completion remain open. Do **not** rename this product to a DOS-faithful engine or close M-ENGINE-01 on the strength of these source-based render pixels alone.

**Next implementation:** original `32954..32F4B` real per-ray cell-crossing, plane selection, view-state/DGROUP preparation, then original CODE.2 wall scheduling, EH7 integration and DOS frame-by-frame comparisons. Full C++17 core remains independent of host renderer; no proprietary DOS executable code is executed.
