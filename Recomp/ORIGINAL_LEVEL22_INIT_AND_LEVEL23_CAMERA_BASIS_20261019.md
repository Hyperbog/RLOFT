# Part 27 — original Level 22 16990 INIT restoration and Level 23 334C4/35798 basis parity

Branch: `Recomp`. Original CODE.2: 558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.

## 1. Authenticated Level 22 view-descriptor initialization — complete

**Original source proof:** CODE.2 `16D04` calls `16810`,
`168C8`, `1686C`, `16934`, and `16990` before the save
`07938/181B4` path. The `16990` routine reads a 16-bit view count
and nested entry lists from original `INIT`, allocating the table
recorded at guest `2B764` and entry pointers via original `3104C`.

`Recomp/src/code2_original_save_game.cpp` now constructs the authentic
`Application25/26/28/29/30Boundary` callback graph and executes the
actual compiled `function_16d04`. Its read-only inputs are the
unchanged owner `INIT`, `NPCS.DAT`, `OBJECTS.DAT` files. The
critical host callback forwards **both** stack-local `312EC_local`
reads and guest-memory `312EC` reads using handle `0x49`; it never
fills `2B764` with synthetic metadata. NPC and object handles `0x4A`
and `0x4B` retain separate byte cursors.

Observed verified full-source reads:
- `INIT`: **1,879/1,879 bytes**.
- `NPCS.DAT`: **5,731/5,731 bytes**, **83 definitions**.
- `OBJECTS.DAT`: **21,844/21,844 bytes**, **336 definitions**.
- `2B764`: original allocator-returned view table pointer
  **1,408,868**, original `3104C` heap top after loading **1,409,396**.
- `RLOFT/SAVE01` authentic Level 22: **1** primary actor, **0**
  ray-marked cells; `07938 -> 181B4 -> 334C4 -> 32954` passed.

[Cross-level 3/3 success](https://github.com/Hyperbog/RLOFT/actions/runs/39196153464):
Level 23, Level 21, Level 22 all source-loaded and raycast.
[Real Level 22 ASan/UBSan](https://github.com/Hyperbog/RLOFT/actions/runs/39196153446):
process exit **0**, exact authenticated scene-INIT marker and source
ray census present; no original guest segfault. The ASan workflow has
been tightened to reject missing original INIT startup, not silently
label it memory-safety success.

## 2. Level 23 camera basis — reconciled, not fabricated

The original `334C4` raycaster derives:
- `[255FC]` and `[255F8]` from the original SINTAB table
  `[C65C] + heading*4` and `[C65C] + (heading+90)*4`.
- `[255EC] = [255D4] - 35680([2560C],[255F8])`.
- `[255F4] = [255D0] + 35680([2560C],[255FC])`.

The native world frame copied `[C65C..]` but omitted the preceding
heading `[C658]`, so original `ray.guest[C658]=252` was presented as
`gfx.memory[C658]=0` in SAVE10. It now copies the actual saved
heading *before* original textured world rendering.

`project_original_scene_res3` cross-checks both guests' camera
coordinates, scale/basis parameters, heading, and the exact
source-computed trig-derived offsets, without modifying actors or rays.

Observed real Level 23 `cloud_saves/RLOFT/SAVE01` after executing
authentic INIT **before** original `07938`:
- heading **50°**, sine **50,203**, cosine **42,125**,
  `[255EC]` **2,912,253**, `[255F4]` **755,268**;
- all ten checked ray/projector source registers equal,
  source trig identities passed;
- primary actor records **34**, source ray-marked cells **3**,
  initial `2B6D0` flag-02 eligible **3**;
- all **9/9** source `35798` projection callbacks rejected at signed
  near plane `0x5800`; primary depths span **-2,815,566** to
  **-246,671**; `2C294 -> 36CEC` emitted **0** calls.

These are original saved data and reconstructed original code, **not**
an independent DOS observation. A source ray mark does not guarantee
the scene actor is in front of the camera.

## 3. Native framebuffer fingerprint changed — intentionally tracked

Copying the original camera heading into the world renderer changed
the reconstructed SAVE10 native 320x400 index digest:

- Previously `d4831707916391c12290afc488318e65b9325d6e178ebb35fce4c794e9cbb8b7`
  (incorrect projector/world heading zero).
- Now `6537547c11da4502a1b3cd93be1c36171e737a972a1824c112f52da7a9b805a7`
  (correct original saved heading copied).

The current original-scene validation still passes after this change,
and generated private `.idx` remains 128,000 bytes, palette `.pal`
remains 768 RGB6 bytes. Do not reinterpret either digest as DOS parity
or silently keep using the previous digest as the accepted current
native baseline. Public CI never uploads commercial frame bytes.

## 4. CI verification

- [Windows MSVC milestone](https://github.com/Hyperbog/RLOFT/actions/runs/39196268302):
  **74/74 CTests passed** on the final source-init/camera-basis revision.
- [Linux portable C++](https://github.com/Hyperbog/RLOFT/actions/runs/39196268261):
  **74/74 CTests passed**.
- [Genuine SAVE10 + Level23 sprite diagnostic](https://github.com/Hyperbog/RLOFT/actions/runs/39196268282):
  passed and logs ray/camera identity; no original-scene sprite draw
  requests are emitted.
- [Owner-original independent RES4 world gate](https://github.com/Hyperbog/RLOFT/actions/runs/39196268263):
  passed.
- [Cross-level owner-save census](https://github.com/Hyperbog/RLOFT/actions/runs/39196153464):
  **3/3**, including previously crashing Level 22.
- [Instrumented Level22](https://github.com/Hyperbog/RLOFT/actions/runs/39196153446):
  original metadata initialized and ASan source execution exited zero.

## Remaining milestones

- No genuine saved camera has yet produced a source `2C294 -> 36CEC`
  sprite draw. The Level23 save is ray-marked but its actors are behind
  the source near plane at this pose. Identify another untouched
  original camera with actors in front; do not edit actor/ray data.
- Independent original DOS VGA 320×400 indices and RGB6 DAC palette
  are **still not captured**. Pixel parity and full original game
  boot remain unaccepted.
- Alternate saved Level5 camera SAVE04 still triggers an existing
  zero-scanline RES4-quad expectation in the diagnostic world harness;
  this is **separate** from the resolved Level22 INIT and Level23
  camera-basis issues.
