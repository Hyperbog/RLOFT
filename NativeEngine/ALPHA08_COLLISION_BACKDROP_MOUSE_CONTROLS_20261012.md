# Alpha 0.8: collision clearance, original CODE.2 backdrop, optional mouse-look

**Date:** October 12, 2026 (US Eastern; Windows CI October 13 UTC).  
**Canonical source:** `native-engine:2b54a176c3a60b0458a06fb11cbd9f6b35357522`.  
**Exact Windows compilation:** `native-alpha08-environment-controls-20261012:f80fcc2202ff33cc88c1feedb6c8b8dbba7c79dc`. The branch differs from canonical engine source only by an isolated one-time Windows acceptance workflow, whose normal `native-engine` manual-only trigger remains unchanged.  
**Linux:** [source-only Actions #38328956553](https://github.com/Hyperbog/RLOFT/actions/runs/38328956553), **32/32 CTests passed**.  
**Windows:** [MSVC x64 + authentic game-data Actions #38328986042](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042), **33/33 CTests passed and 32/32 original level render checks passed**.

## Reported visual failure and native fix

Alpha 0.7 rendered with a **point player**: `move_native_probe_camera` only checked the destination cell, so a center point could move until almost intersecting the wall. Simultaneously the host renderer mapped perspective height as `400 / max(.05,distance)` and clamped at **256** pixels. The result appeared as an increasingly stretched, maximum-height wall when players pushed against collision. A rejected movement remained camera-stationary, but the UI still redrew.

Alpha 0.8 adds `move_native_probe_camera_clearance`:

- Nonpenetrating circular host player footprint **radius 0.26 world cells**; subdivided swept step (at most 0.04 world cells each).
- Separately bounded tangent-axis slide for diagonal wall contact. No teleport through corners; no movement into a blocked original `LEVELS.DAT` cell. Collision still delegates to original `world.normal_collision` for cell state.
- Partial approach is permitted until clearance contact, then further forward pushes yield **unchanged camera pose and unchanged frame**. The user-facing UI does not rerender on a blocked **zero-movement** result.
- Perspective in the host camera is explicitly corrected to `55 / max(.22,distance)`, clipped to **1..240** pixels inside the 256-line original viewport, avoiding premature full-height wall scaling. This is a **host-only intermediate formula**. It does **not** claim reconstruction of original DOS `35730` live camera depth.
- Extended existing targeted `native_scene_view_tests` to cover partial approach, terminal repeated collision/frozen frame, tangent slide and invalid clearance. The first test incorrectly assumed the very first large blocked sweep meant full terminal contact; Linux and Windows each caught it. The test was corrected to stop only at verified steady contact; **no collision check or guard was removed**.

## Original backdrop replaces placeholder floor and sky

Actual source bytes are from original owner `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` and original `LEVELS.DAT` fixed headers.

Original CODE.2 object1 `33E28` draws the **top 128 rows** through `REP STOSB` 64 bytes per plane per row, from VGA page byte `0x148`, scanning forward 80 bytes per scanline. `33DE0` draws the **bottom 128 rows** from byte `0x50F8` scanning backward 80 bytes per scanline. Together they fill precisely **x=32..287; y=4..259** of the original 320×400 four-plane VGA page.

For each background row, original instruction `SUB AL,BYTE PTR [EBX+FE94]` subtracts the 129-entry CODE.2 object2 `FE94` table indexed **128 down to 1**. The full exact 129-byte source table at physical `0x75F24+0xFE94` has SHA-256 `dcd62d3a40b5cbd2e60aa49900c90e097090efc583b4aa7eb5eb1c2f385bfe84`. That identity is checked when the original media is opened. Original level header `+0x298` supplies upper renderer fill base; `+0x299` supplies lower fill base. Both are byte indices, not an invented one-word color field.

New `NativeCode2Backdrop` writes these **source-defined palette indices** into the actual VGA-plane storage and then through existing original COLORS DAC6. Mouse look applies an explicitly host-only bounded vertical presentation shift to these original rows. **This recreates source-authored gradient/fill background; it is not a textured skybox, simulated outdoor sky, original gameplay render-profile selection, or a proof of DOS-frame parity.** The bottom user-interface area outside the original 256×256 first-person region is not yet populated by original full gameplay GUI art.

The wall render now casts **256 host view rays** aligned to the original viewport x32..287 instead of filling the entire 320-pixel screen with rays, while preserving authentic per-level `LEVELS.DAT` material candidate IDs, original RES4 EH6 texels, CODE.2 source quadrant/reflection states, exact `338C8` fixed-point column sampler and four-plane DAC6 scanout. **Full original wall-face/dual-word choice remains provisional** until live CODE.2 `32954` traversal and renderer dispatch are integrated. Do not label all walls as exact original DOS surfaces based only on original texture bytes.

## Native-control preservation and optional mouse-look

Original keyboard-oriented preview is the default. Alpha 0.8 changes:

| Control | Result |
| --- | --- |
| `W` / `S` | Move forward/backward |
| `A` / `D` | Strafe left/right |
| `Q` / `E` | Turn left/right (not Left/Right arrow keys) |
| `M` | Toggle **optional** raw Windows mouse-look |
| Mouse move while enabled | Relative yaw (horizontal) + **pitch** (vertical), bounded |
| `PageUp` / `PageDown` | Choose other authentic original level |
| `R` | Reset host preview spawn |
| `Esc` | Close preview only |

Optional `WM_INPUT` uses relative raw mouse deltas, captures and hides cursor only while mouse-look is on; returns to keyboard-only horizon and releases/hides no cursor on toggling off or focus loss. In **default keyboard mode**, pitch remains zero and camera turns only in yaw. New sampler `host_pitch_pixels` is strictly bounded `[-96,+96]`, clips wall columns to the original 256-line working region and retains exact source CODE.2 vertical texel stepping for all visible rows. Host pitch is expressly a non-original optional extension.

User-owned original DOS/GOG files and saves remain **read-only**.

## Actual Windows owner-original evidence — PASS

The Windows/MSVC runner compiled the current user-facing `rloft_native.exe`, ran **33/33** native CTests (including the corrected stable-wall-contact regression), and executed the read-only original-media preview auditor against all **32** maps from `DnD-Classics` original `LEVELS.DAT`, `RES4`, `COLORS`, `INIT`, `RES0`, `T`, and `CODE.2`. All source-file SHA-256 digests before/after matched.

| Original source-backed output | Verified |
| --- | ---: |
| Level records loaded | **32/32** |
| Level views with authentic RES4 wall pixels | **32/32** |
| Source-textured wall columns | **8,077** |
| Original EH6 texel reads | **226,589** |
| Unique source-wide RES4 EH6 textures | **97** |
| Windows native CTests | **33/33** |
| Portable Linux native CTests | **32/32** |
| Original media byte modifications | **0** |

Aggregate 32 original-source 320×400 BGRA frame SHA-256:  
`2b232795b9650be8013b1d0dce312bde6fb974bfb3a87edeb7603035d94daaf2`.

The 32-view audit also exercises native camera turning/moving; the Windows UI raw-mouse event capture is compiled but **not personally field-tested by a GUI user**, and **the audit is not DOS screenshot parity**. The reduced texel count versus Alpha 0.7 reflects deliberately bounded near-wall heights and alignment to the authentic 256-wide view region.

## Download and exact hashes

[Isolated authorized Windows build and logs](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042) · [GitHub download artifact #11781988551](https://github.com/Hyperbog/RLOFT/actions/runs/38328986042/artifacts/11781988551).

| Source-identified asset-free package item | SHA-256 |
| --- | --- |
| `rloft_native.exe` Windows x64 binary | `cabf280706f9bc3cf16c24073573e444ca46b0bc5bf70fe3f660cf4bd409726a` |
| CI six-file distribution ZIP | `e9908e1bca339c59e32d0d4d4c022ee98e41ba0311178953f309b7e04f02b7e3` |
| Outer GitHub Actions ZIP | `f88280098c64b5c0fac9dcd447d3fc40e6717626aa6d4ecb59ca87bc366ab098` |
| User-friendly seven-entry Alpha 0.8 ZIP (extra README only) | `a7af00428bc3c86699ef086dc0170bac2a92b91ca588ee1f1c5d93099485f10c` |

All three official compiled Windows AMD64 executable hashes matched their CI logs, and the user-friendly ZIP was independently checked for exact payload contents. The package contains no licensed original Ravenloft media.

## Open work

Actual CODE.2 ray/face/depth generation, complete wall material scheduling, view animation/sprites/EH7 actor projection, color fade and special render profiles, original UI art, mouse-look **visual user field acceptance**, DOS snapshot parity, party/events/combat/audio/save systems and a first playable game. This remains **Alpha 0.8 technical preview**, **NOT** a fully playable or screenshot-faithful engine. Further reconstruction tests should be created only when needed to reproduce a concrete issue or certify a game-facing subsystem.
