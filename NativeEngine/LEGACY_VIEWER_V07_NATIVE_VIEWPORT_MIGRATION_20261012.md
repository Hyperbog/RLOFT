# Legacy v0.7 viewport integration and native Alpha 0.10 field repair

Date: 2026-10-12 Eastern. Source reference: owner's uploaded `ravenloft_viewer_v07.zip` (59 MB), inspected locally; **never commit its bundled original RLOFT.zip or Python venv**. User's Alpha 0.9 screenshot documents a horizontally striped, vertically cramped original-view strip, color-swapped upper/lower walls, collision-blocked status and mouse-look stutter around **level 1, cell (14,11)**. User specifically requested original placed texture correctness, stable collisions, smooth mouse controls, scaling and an automap like the older viewer.

## What we recovered from v0.7

The earlier viewer has a useful **ModernGL 3.3** polygon renderer (`gl_renderer.py`) with a depth buffer and genuine triangles for four wall faces, ceilings and floors, render-target rescaling and texture filtering. Its `world.py` reads original LEVELS.DAT 64-word material banks and cell bit `0x0200` for optional upper-wall tiers; `gl_renderer.build_mesh` maps **X-wall faces to bank 0 lower/bank 1 upper** and **Y-wall faces to bank 2 lower/bank 3 upper**, NOT four opposing compass faces. The upper texture uses only the first authored `level_data[38]` rows (96 in the owner screenshot level). It also distinguishes the original blank/no-draw wall reference **147** in certain levels. Exact door and special-frame placement remains its own provisional renderer path, not a justification for universal bank changes.

Its mouse loop (`opengl_viewer.py`) uses SDL/Pygame relative `MOUSEMOTION.rel`, continuous W/A/S/D sampled every frame, `clock.tick(144)`, capped `dt<=.06`, and collision substeps `<=.05`. The previous native Windows viewer did full software redraw in every `WM_MOUSEMOVE` event and recentred the cursor in that event, creating a self-feeding WM_MOUSEMOVE stream. Holding keys depended on OS `WM_KEYDOWN` repeats. For a modern native engine, **keep a fixed/timed input-simulation tick** even when Windows message rates and frame rendering vary.

The old viewer's `Automap` reveals cells per short visibility ray, stores explored cells per level only in RAM, and draws a centered zoomable map. `GLRenderer.set_filter` exposes nearest vs mipmapped linear and an optional edge-aware sampling mode; separate F2 rendering targets include 1280x720, 1920x1080, 2560x1440, 3840x2160. Those are GPU *presentation quality* options, not new native RES4 source texel data.

## Independent original-data verification — screenshot's material cell

We did NOT treat the old Python prototype as higher authority than the original assets. A fresh independent Python `struct` decode of the original `RLOFT/LEVELS.DAT` nested in the provided v0.7 archive re-established:

- original level **1** is 29×33 cells;
- screenshot camera at **(14,11)** stands south of a world wall at **(14,10)**;
- the authored wall cell's complete raw word is **0x022C**, i.e. slot `44` and **upper-tier flag `0x0200`**;
- original LEVELS four real authored RES4 IDs for this one cell are **[497 lower-X, 62 upper-X, 499 lower-Y, 62 upper-Y]**;
- its level header's upper source-row count is **96/128**, the old viewer's `upper_height=2*96/128`.

The old native 0.9 viewer incorrectly used the CODE.2 paired bank ordering `3/2` or `1/0` to choose a *single* wall texture depending on ray direction. That swapped vertical tiers for alternating viewer angles, explaining orange/grey discontinuities in the supplied screenshot. The pair itself was correctly reconstructed from CODE.2 wall emitter bytes; the **runtime decision to render both authored vertical tiers** was missing. Static original axis/tier placement is now source-correct in the bounded preview; complete original CODE.2 live ray/emission/door/sprite scheduling is still open.

## Changes in NativeEngine

1. **Correct material axis/tier binding:** `native_probe_original_textures.cpp` uses bank0/base X and bank2/base Y, plus bank1/upper X or bank3/upper Y only for source cell `0x0200` flag and header[9] authored upper source rows. Upper geometry is a **bounded host approximation** that uses original texel data and source vertical range; still not the original 34A74/35730 source placement. Source-tested original 338C8 128-row sampler remains default for lower walls.
2. **Deterministic view crop:** `win32_native_exploration.cpp` explicitly copies original VGA `x32..287/y4..259` into a tightly packed 256x256 top-down DIB prior to scaling. Previous direct StretchDIBits subrect sampling could present as a cramped top strip/black elsewhere depending on GDI path; no original map/art bytes are altered. `V` still shows full 320x400 page. `N` switches nearest-neighbor vs HALFTONE GDI **presentation filtering**; it does not invent high-resolution original textures.
3. **Timer input, not WM event-rate input:** `WM_TIMER` runs approximately 16ms where available, caps `dt` to 0.06s, polls held W/S/A/D/Q/E, normalizes diagonal speed, and renders **at most once per event-loop tick**. Mouse motion is sampled using `GetCursorPos` relative to captured window center at most once per tick, then recentred once. `WM_MOUSEMOVE` no longer calls SetCursorPos or redraw. M or right-click toggles mouse-look; native keyboard look remains default; focus loss releases capture. Movement retains original-backed collision and swept host clearance, so a **fully blocked step does not change camera or image**.
4. **Session-only automap:** `Tab` toggles a centered minimap; + / - adjust zoom. Uses already-computed native host ray `reached` cells, original-world collision categories and level dimensions; exploration is in memory only. It is not the original DOS automap semantics or a save writer.
5. **Original-data acceptance:** the existing 32-map smoke CLI also verifies the level-1 (14,10) original 0x022C cell, authored [497,62,499,62] bank lookup, and its 96-row upper tier. Collects the new source-authored upper-tier rendered column count so a future single-bank regression is caught. Source-only CTests continue to cover normal collision and original wall sampler behavior. Windows GUI mouse feel still requires actual player testing; headless acceptance does not exercise real Windows mouse motion.

## Verified Alpha 0.10 Windows acceptance and package (2026-10-13 UTC)

[Owner-authorized Windows/MSVC x64 CI #38335193771](https://github.com/Hyperbog/RLOFT/actions/runs/38335193771) compiled the exact preview source from `native-engine:93cd779754e8d091b3ac20fb2f75949c17907cd0` (the isolated one-off workflow checkout commit `bbe2d8be4931ec2a7677d48a111a081096f3ef71` differs only by workflow trigger). **33/33 Windows CTests pass**, with original-media **32/32 maps pass** and source file hashes identical before/after. Source-only portable Linux CTests also pass on this canonical revision: [Actions #38335159001](https://github.com/Hyperbog/RLOFT/actions/runs/38335159001).

The exact Windows original-media audit reported **8,072** wall-hit columns, **609,056** source wall texel samples, **2,029** genuine upper-tier columns, **987,256** floor and **842,872** ceiling source-preview texel samples, across **160** distinct original EH6 resources. The original level-1 cell `(14,10)=0x022C`, exact authored table values `[497,62,499,62]` and level upper source-row count `96` were explicitly checked.

Exact original Windows GUI `rloft_native.exe` SHA-256:
`3f44388387a3c1457b179c208cfc13fc4af12b41531c80b9fcee8a62f2d9c4a6`.
CI-created 6-file distribution ZIP SHA:
`75ff9b5a5d15edda6826067aa83a4f00a173fcc97c9dcd66593fa63eaf5677bf`.
Downloaded Actions ZIP SHA:
`4f8726a2bb97c7bc70b6947f8c6167b500a566c6e75937819a2ce906721f1413`.
User-friendly 7-file Alpha 0.10 ZIP (extra README only) SHA:
`25702ff10fb30102bb6a97e3c934f34faed54ee789b4c70ebd783a4017c6188e`.
[GitHub artifact #11785130189](https://github.com/Hyperbog/RLOFT/actions/runs/38335193771/artifacts/11785130189), original-data-free.

**Live field testing still pending**: original mouse feel, actual wall texture placement in a player's Windows screenshot, door/actor occlusion, DOS source 34A74 floor/ceiling UV parity and full original game. Source-backed static material placement is improved; automatic original DOS ray/camera remains incomplete. Do not close first-playable issue M-ENGINE-01 or label this as DOS frame parity.

## Recommended future viewport architecture

**Adopt SDL3 + an actual accelerated renderer**, either SDL_GPU or bgfx with a well-defined CPU pixel reference fallback. Use **separate source-preservation and modern-presentation modes**. The existing native CODE.2 fixed-point VGA compositor remains the original research oracle, while a modern GPU depth-buffered mesh viewport handles smooth arbitrary camera pitch/yaw, frame rates, higher internal resolutions, filtering and multisampling. Never resample original low-res data into invented new "original" texels or treat an OpenGL 3.3 prototype's inferred material/door choices as established CODE.2 binary facts. Retain original-level resource identities, collision, state, animation, official saves and automap until formally verified. The old prototype's SDL event model, mesh topology, map overlay and texture filtering are useful design references; **do not merge its Python runtime, dependencies, old saves, bundled original game or speculative asset patches into the production C++ engine**.

Status remains a technical alpha, not DOS frame parity or first playable. All original assets stay user-owned/read-only.
