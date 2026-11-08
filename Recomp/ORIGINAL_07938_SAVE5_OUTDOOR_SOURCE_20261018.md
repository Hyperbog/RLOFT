# Original CODE.2 saved Level 5 game restoration, wall VGA, and outdoor dispatch

**Owner:** `recomp` · **Date:** 2026-10-18 · **Status:** compiled original SAVE/load and Level 5 wall raster verified; **outdoor textured sky/ground/tree and complete HUD pixels not yet generated**. This is an engineering diagnostic, not a player build or original DOS frame comparison.

## Source and owner-original inputs

- CODE.2 retail executable: 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Original initialized CODE.2 object-2 DGROUP starts at file offset `0x75F24`; the test loads its bytes into the native guest, including all **62 original inventory-container descriptors** at `0x7330` and their pointer tables.
- `SAVE10/SAVE.DAT`: 9,288 bytes; source `07938/06CAD` reads the file through allowlisted read-only DOS callbacks, loads four party records and original game state, including the Level 5 camera `x=0x00065960`, `y=0x0006EB68`, `heading=252`, fixed-point clock, source lighting `F920=10, F91C=3` and saved container state.
- `SAVE10/LEV005.DAT`: directly loaded by compiled original `181B4` with the literal `WORK\\LEV005.DAT` guest filename mapped into read-only original-media CI. Source-installed 64×64 cell grid, scripts and mutable objects remain in guest memory.
- Source `03838` reads authentic `SINTAB`, original `334C4` prepares view state, `32954` generates 256 original ray records. `335DC → 3263C` uses original `2F29C → 2ECB4 → 37590 → 37700` RES4 type-six archive loader, decoded guest texture allocations and the SINTAB scale interpreter plus real `338C8` fallback.
- Real owner media are read by GitHub CI only with **`DnD-Classics` read-only sparse checkout**, hashed where appropriate, and all private framebuffer outputs and save data are removed before any artifact packaging.

## Verified first-person wall result

[Successful source-authentic saved-world CI #39135916368](https://github.com/Hyperbog/RLOFT/actions/runs/39135916368) executes:

```text
CODE.2 initialized static data + original SAVE10/SAVE.DAT
  -> compiled original 07938 / 06CAD deserializer
  -> compiled original 181B4 loads LEV005.DAT
  -> compiled original 334C4 -> 32954 (256 rays)
  -> compiled original 335DC -> 3263C -> 338C8/SINTAB
  -> indexed VGA (walls only)
```

Observed: 4 RES4 wall materials, 77 SINTAB scaling callbacks, 49 authentic `338C8` fallback invocations, 4,696 scaled indexed writes, 20,407 nonzero indexed pixels in 320×400 shadow VGA, 121 wall compositor flushes. Source-generated native indexed output SHA-256: `c6a1297e5705d73a025798c4a76bda7193171f15e31453efc64251eb6ca8ba39`. Hash is a **native fingerprint**, not a real DOS oracle or full-scene screenshot. The old source-mapped 14,917-pixel result was superseded by executing the authentic full source `07938` deserializer and proper initialized state.

Standalone original deserialization [Linux gate #39135675030](https://github.com/Hyperbog/RLOFT/actions/runs/39135675030) independently verifies `07938` source and `181B4` with 62 container descriptors, four party records, saved game clock/light and all 4,096 saved Level 5 cells. [`rloft_original_code2_save_game`](src/code2_original_save_game.cpp) and [`rloft_original_code2_world_frame`](src/code2_original_world_frame.cpp) are separate reproducible engineering targets. No full CODE.1→CODE.2 boot has occurred.

## Original outdoor dispatch: newly executed source

[Dedicated original saved Level 5 world CI #39136699731](https://github.com/Hyperbog/RLOFT/actions/runs/39136699731) executed compiled source **`function_1f1c8`** scene dispatcher against the exact saved Level 5 guest state, after previously completing original `334C4/32954` rays on that state. On real owner data:

```text
level header +0x290   0xFF (alternate/default environment profile)
C026                 1 (textured ground branch enabled)
C027                 1 (textured sky branch enabled)
C028                 1
level sky-kind byte   152
level ground-kind     206
real 1F1C8 -> 34A74 textured sky callback       1
real 1F1C8 -> 34A74 textured ground callback    1
original RES4 texture lookups resolved            2 distinct source items
other RES0 HUD sprite requests                    12 (NOT RENDERED)
flat 33E28 / 33DE0 sky/ground calls              0 (correct for saved state)
34A74 textured raster execution                  NOT IMPLEMENTED
```

The environment profile `0xFF` bypasses the two INIT `314B0` panorama-placement tables. The original `1F1C8` itself proves the branch: when `C027=1` or `C026=1`, it loads an original RES4 texture derived from `0x258F8` and the level's source-header texture selectors, assigns `FF4C/FF44/FF48/C040`, and invokes original `34A74`. Source `33E28/33DE0` are **flat-color fallbacks** when the respective flag is clear; running those on this save would be incorrect and is disallowed in the test. The two authentic original `2F29C` RES4 texture lookups have bounded EH6 decoded allocations. The 34A74 callback currently records and validates resource identity, **but does not rasterize or invent backdrop pixels**. The optional `1EA04` RES0 HUD requests are observed without drawing because their resource archive is not joined here.

## Precise next integration: real textured sky/ground and scene objects

1. Execute compiled original `34998` perspective-control setup from authentic current SINTAB/camera state for guest `FF58` rows. Source `1F1C8` already calculates offset fields `C044/C048`.
2. Integrate compiled original `34A74` for both texture passes, with original 342E4 / 34954 raster scans. Bridge its self-patched immediates through a safe bounded interpreter/operation callback; maintain exact `FF4C,FF44,FF48,FF40,FF54,C040` values and **use both genuine RES4 images**, never a fabricated horizon.
3. Restore palette/profile shading, preserve occlusion by composing sky/ground **before** the recorded original `335DC` wall passes. Assert actual shaded sky/ground pixel output independently of wall-only records and include nonzero draw counts.
4. Execute original `34DE8` scene-object projection/quad handling through RES3 and other source sprite/HUD drawing once the texture backgrounds are right. Trees, hills, scene object animation and viewport framing still need independent original DOS observation.
5. Capture an independently observed original DOS indexed frame + 256×RGB6 palette at the **same Level 5 save and camera**, use [`compare_dos_indexed_frames.py`](tools/compare_dos_indexed_frames.py) for exact byte comparison. The earlier user screenshots are reference images only, not original raw VGA/CRTC/DAC captures.

**Acceptance:** authentic `07938` source save loader and 181B4 guest level restoration PASS; saved Level5 SINTAB ray/wall pass PASS; `1F1C8` textured sky/ground **dispatch and resource selection** PASS; *34A74 textured sky/ground pixels, trees, full HUD, continuous boot, R04 and DOS parity remain OPEN*. Do not use the term complete outdoor renderer until all missing layers are executed and visually checked.
