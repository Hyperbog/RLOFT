# Alpha 0.9: correct original upper/lower RES4 source selectors, DOS pixel aspect, and working optional mouse path

**Date:** October 12 Eastern / October 13, 2026 UTC. **Engine:** independent native C++ original-install-backed Windows replacement technology preview, not `DemoV1`. **Source checkpoint compiled:** `native-engine:f006532b78639ae7fcf9eaa23326767a03b133f2`. **Exact Windows compiled checkout:** `native-alpha09-authored-surfaces-mouse-20261012:ab3a0fc52cd625b86b52c08ea6ff852678927323` (differs only by isolated one-time Windows CI trigger; production workflow stays manual-only).

## Field regression

Owner tested previous Alpha 0.8 and reported **perspective stretched**, **flat/non-rendering floor and sky**, and **mouse look not working**. The previous code's portable CTests only verified archive decoding and provisional host camera output; these did not establish visual success or Win32 input delivery. This report makes no claim that subsequent source CI substitutes for a live owner Windows session.

## Original CODE.2 authentic upper/lower surface bindings

Alpha 0.8 incorrectly treated `33E28`/`33DE0` background-gradient fillers as complete sky/floor artwork. The original CODE.2 object 1 proves separate conditional textured passes at `1F2B1..1F3DA`:

- `1F317`: `MOVSX EAX, WORD PTR [0x2561D]` = level header `+0x05`, then `ADD EAX,DWORD PTR [0x258F8]` at `1F320`, `PUSH 4` RES4 load through `2F29C`, texture body -> `0xC040`, and upper render `34A74`.
- `1F388`: `MOVSX EAX, WORD PTR [0x2561F]` = header `+0x07`, then `ADD EAX,[0x258F8]` at `1F391`, another RES4 texture -> `34A74` for lower surface.
- `1F359` falls back to `33E28` using header `+0x298` for original upper palette fill; `1F3CA` falls back to `33DE0` and `+0x299` for lower fill. These were never a complete substitute for projected textured planes.
- `34A74..34BAC` uses original SINTAB-like projected surface generation and programmable texture sampling, **not** a modern world-space floor-caster. The native renderer still approximates geometric plane UVs until that function is reconstructed and frame-compared.

Original `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Original `LEVELS.DAT` 32-level archive was inspected as raw bytes independently of C++.

The authentic source archive has **600 wide (64×128) EH6 wall textures plus 67 narrow (32×128) EH6 surface entries**; source `0x258F8` is a dynamically initialized RES4 class base. The native ordinary-install profile maps these to `RES4[600+header_selector]` and validates the resulting 32×128 original source shape. The base is not a generalized invariant for unverified game variations.

Across 32 original level headers:

- **27/32** upper selectors are nonnegative; **5** are signed `-1`.
- **32/32** lower selectors are nonnegative.
- Combined authored selectors use **20 distinct narrow source entries**, all in `0..55`.
- Exact 128-byte binary of ordered little-endian upper/lower signed pairs SHA-256: `f878415dec54d6676e21a3872cfae5de99420070cf5189d6e7e0ab16193afaca`.
- The native engine preserves missing upper sources as CODE.2 palette background and **no synthetic sky image is invented**. Which source profile/feature toggle enables each original textured pass is not fully runtime-proven; this is a consistent original-file-backed graphical preview, not final parity.

`NativeProbeOriginalTextures` now loads these source-authored narrow RES4 textures and applies bounded host-projection UVs on ceiling and floor within source viewport. The wall pipeline retains original 64×128 candidate textures, level-conditional source banks, CODE.2 338C8 sampler, and planar RGB6 palette. The original `34A74` surface polygon raster and live `32954` ray scheduler remain unimplemented.

## Perspective/presentation fix

Previous Alpha 0.8 displayed the whole indexed `320×400` VGA source as **square host pixels**, missing the original horizontal 2× presentation used by 640×400 DOS screenshot presentation. Alpha 0.9 displays original-pixel visual proportions (`x2` horizontal) and **zooms the actual authored x32..287/y4..259 first-person window by default** instead of shrinking it within an otherwise blank unimplemented original HUD. Press `V` to show the full 320×400 indexed frame in its DOS display ratio.

The prior host wall height `55/distance` was an arbitrary distortion correction. It is now **focal-length/positive ray depth** using the 256-ray viewport and host camera horizontal FOV `90°`. Wall height is bounded 1..256 as required by original `338C8` and existing swept 0.26-cell player clearance. **This host projection is NOT the original CODE.2 35730 per-ray height.** A close wall still fills the view at genuine contact; repeated fully blocked movement does not change the camera or frame. This is perspective geometry stabilization, not DOS camera parity.

## Optional mouse-look rework

Previous Win32 implementation relied on `WM_INPUT` raw device registration and an overly strict foreground equality condition. In real field use the owner reported no mouse-look response, which CI could not detect. Alpha 0.9 replaces that path with **opt-in `WM_MOUSEMOVE` relative-to-window-center capture**:

- `M` or right-click explicitly toggles capture; **OFF by default**.
- Captured mouse motion updates yaw and bounded independent vertical pitch; relative mouse deltas recenter without cursor-edge stall.
- `M` key autorepeat cannot accidentally immediately toggle mouse mode off.
- Loss of keyboard focus releases capture and cursor visibility and restores original keyboard-only fixed pitch.
- `Q/E` retain native-style keyboard turn; `W/S` move forward/back and `A/D` strafe; no arrow-key turning. `PageUp/PageDown` choose level, `R` resets preview spawn and `Esc` closes window.
- Actual `WM_MOUSEMOVE` physical responsiveness is **not certified by Windows CI**; it needs a user session. The headless original-media audit checks **that a nonzero host pitch changes a rendered original-source image**, a narrower and appropriately reported software gate.

## Real original files, source/tests, and release verification

[Linux portable source run #38330350850](https://github.com/Hyperbog/RLOFT/actions/runs/38330350850) passed **32/32** native CTests for canonical source.

[Owner-authorized isolated Windows x64/MSVC acceptance run #38330375936](https://github.com/Hyperbog/RLOFT/actions/runs/38330375936) built `rloft_native.exe`, passed **33/33 native CTests**, and executed the read-only real-original game-data audit over all 32 maps with `CODE.2`, `LEVELS.DAT`, `RES4`, `COLORS`, `INIT`, `RES0`, `T` sourced from the original DnD-Classics branch.

| Verifiable Windows result | Value |
| --- | ---: |
| Original levels loaded | **32/32** |
| Original maps producing authentic textured walls | **32/32** |
| Authentic RES4 wall columns | **8,072** |
| Original EH6 wall texel samples | **779,551** |
| Original source-authored floor plane texels | **987,256** |
| Original source-authored upper plane texels | **842,872** |
| Unique original RES4 texture records decoded (wide + narrow) | **137** |
| Windows CTests | **33/33 PASS** |
| Linux CTests | **32/32 PASS** |
| Original checked game files modified | **0** |

The 32-source-frame 320×400 BGRA output SHA-256 was `63287c4dde9ff02a3dc3c4ece539051a7ad0af6c377c764c98dd1579167e6b31`.

[Original-media-free Windows Actions artifact #11782501579](https://github.com/Hyperbog/RLOFT/actions/runs/38330375936/artifacts/11782501579), downloaded and independently rehashed.

| Reproducible file | SHA-256 |
| --- | --- |
| Actual compiled Windows AMD64 `rloft_native.exe` | `bc91760d56253f452b4183d1ac392c20f0dd5e96b8cc743c681661e0c9dbf0be` |
| CI inner 6-file distribution ZIP | `4f2db67c9b868cc23a0dbfe50038d460f53f03274b94c0d7e6638244b82a6f72` |
| Outer GitHub Actions ZIP | `0a4042b2a41f59f06e989b35a01f2640af58d9dafb8d2dab089148ede5d05dc7` |
| User-friendly 7-file Alpha 0.9 ZIP (added README only) | `0e52c18c3daf7874ee9bcdd1993160111212f232bf539baaee97939fe132fe3e` |

No original game resources, screenshots or saves were packaged into the distribution.

## Acceptance still open

**User field-test required:** confirm M/right-click actually activates and moves yaw AND pitch in their Windows game window; compare screenshots of the original game and current first-person surface output in affected levels. No claim of original DOS 34A74 sprite/ceiling/floor geometry, automatic wall-face dispatch, DOS pixel-frame equality, original HUD or normal gameplay is made by this checkpoint. Next meaningful reconstruction is original CODE.2 live camera, wall material selector and 34A74 projected polygon raster to replace host plane UVs, followed by first-frame visual parity against real DOS captures. Keep first playable issue M-ENGINE-01 open.
