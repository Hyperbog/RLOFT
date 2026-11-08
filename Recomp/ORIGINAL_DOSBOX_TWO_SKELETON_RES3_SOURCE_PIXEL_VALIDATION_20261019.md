# Part 23 — authentic two-skeleton source sprite validation (2026-10-19)

**Branch:** `Recomp`. **Parent checkpoint:** `c714ba815a6362e6f894b6c5b1ab8da36ee35c5c`.
This records a successful *native reconstruction* test of the user's unchanged DOSBox
Level 5 save; it is **not** independent DOS VGA pixel parity, a full world
frame, or continuous compiled CODE.1 gameplay.

## Owner-local provenance

The owner reran the read-only `run_chatgpt_save09_native_probe.ps1` with Windows
engineering Build #152 (`39206621665`), unchanged `SAVE09(2).zip`, and local
original game media. The inner `SAVE.DAT` SHA-256 reported in the previous
checkpoint is `28a4b28473997d2682d0a4671e104b13fbad078eea49b0369205ed8ed3669542`.
The two returned logs were `source_actor_projection.log` and
`original_2c294_36cec_sprite.log`. Neither raw owner log nor any private
save, resource, image or VGA capture is included in this commit. This
log-based observation does not independently reauthenticate the executable,
save bytes, or retail DOS runtime.

Source camera: **level 5**, heading **198**, x16 **1419090**, y16 **1436586**.
Source actor candidates:

| Actor | Type | Cell | Signed depth | Projected scale | Screen projection |
| --- | ---: | ---: | ---: | ---: | ---: |
| #38 | 4 | 1302 | 111760 | 411 | 45 |
| #70 | 4 | 1365 | 59190 | 775 | 176 |

Both actors pass the near plane, source initial `2B6D0` draw eligibility
and projection checks. The source ray census reports **two** ray-visible
primary cells. The original compiled `2B6D0` renderer reports **two** matching
primary actor callbacks at depths 59190–111760 and reaches `2C294` twice.
Native genuine scene raster counters:

| Metric | Result |
| --- | ---: |
| Authentic `36CEC` source draw calls | **2** |
| Preflight source columns | 78 |
| Preflight sprite descriptors | 223 |
| `36CEC` visible sprite runs | 78 |
| `369D8` VGA plane passes | 287 |
| `369D8` indexed sprite writes | **5515** |
| Source ray occlusion | YES |
| Full frame | NO |
| Original DOS VGA pixel parity | NOT VERIFIED |
| Continuous CODE.1 game boot | NOT ACCEPTED |

**Narrowly accepted:** the two-skeleton source-camera test that was blocked
before the original `32954` cell index and `369D8` signed effective-address
corrections now produces genuine RES3 indexed sprite pixels without inserting
actors or moving saved cameras. The renderer's scene-level counters and the
matched two primary depth extrema do not yet identify pixels by actor. They
do not prove visual parity with DOSBox, including animation phase, clipping,
color, output page or palette.

## Next boundary

1. Obtain per-draw native attribution (actor record, selected RES3 shape,
   draw call, contributing indexed writes), without modifying source actor
   or ray state; keep raw outputs owner-local.
2. Render a full 320×400 native indexed frame and 256-entry RGB6 palette
   with the same unchanged save, inspect it privately, and distinguish
   full-frame writes from the current isolated sprite-only probe.
3. Independently capture the actual retail DOS/4GW display: 4 VGA planes,
   CRTC start/page, viewport/panning, 320×400 row-major palette indices,
   all 256 DAC RGB6 values, and enough timing/frame identity to select the
   matching animation frame. **Never construct a DOS reference using native output.**
4. Use `Recomp/tools/compare_dos_indexed_frames.py` to compare separate
   authenticated DOS/native indexed frames and palettes, report first
   divergence. Leave formal R08/R09/R12 gates open until their own full
   subsystem acceptance is demonstrated.

**No new save or screenshot is required** for the next host-side work.
Related source-rooted correction and first 10-save regression:
[ORIGINAL_RES3_369D8_GENUINE_SPRITE_ACCEPTANCE_20261019.md](ORIGINAL_RES3_369D8_GENUINE_SPRITE_ACCEPTANCE_20261019.md).
