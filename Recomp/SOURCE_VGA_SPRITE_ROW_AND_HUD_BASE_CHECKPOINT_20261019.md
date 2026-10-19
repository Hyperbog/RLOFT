# Recomp — source-native VGA sprite row and genuine gameplay HUD base checkpoint
**Date:** 2026-10-19 · **Branch:** `Recomp` · **Status:** Source-derived fixes and archived-world tests passed; private two-skeleton re-render and independent retail DOS parity remain open.

## Retail CODE.2 evidence and correction

Original `CODE.2` SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object-1 code is read from the owner-supplied binary; only addresses, behavior and hashes (no bytes/art) are stored here.

### Sprite vertical row, source 36E6C and 36B58

- `36EC9..36ECF` assigns the genuine **source raster row** at guest `0x10450`. For ordinary RES3 ID below 0x117, it is `clamped_scale >> 2`. The special range `0x117..0x13A` computes `2*clamped_scale + (2*clamped_scale >> 3)`.
- `36F3E..36F88` derives a **different** scalar using the SINTAB scale index and clips code instructions against 256 physical world rows.
- `36B78..36B95` consumes `0x10450 * 80 + C6E0 + (column >> 2)` to form the actual VGA CPU destination; this is **not** the clipping scalar.
- `36FD1` updates `0x10450` for nonzero vertical-mode corrections.
- Previous `raster_original_36cec_scene` reused the clipping scalar as a raster destination row, placing genuine skeletons hundreds of rows too low and triggering 4,499 B000+ CPU stores.
- Replaced that scalar by separate `raster_row` vs `phase`, both derived from original instructions; `Original369d8Context.renderer_phase` now uses `raster_row`.
- A synthetic test with projected size 8 checks actual row 0 against the old erroneous row 130, not an invented source position.

### Gameplay HUD background, original 1F8FC

- Retail `1F8FC..1F962` selects draw page via `291B0(C050)`, then calls `2E510` with two original RES0 raster IDs: `INIT RES0 base (15) + 1 + 3 * HUD mode` at row 0, and `15 + 2 + 3*HUD mode` at row 200.
- `2E510` calls `2F29C` for decoded original archive 0 image, then `2E4A4`; the latter loops image `word+2` frame count with row step `word+4`, dispatching `2939C -> 2A380`.
- This is distinct from previously integrated `1EA04`, which only issued 12 comparatively small UI overlay image blits and left the lower screen black.
- The native code now invokes this original resource/iteration sequence **before the outdoor/wall rendering pass**, then re-opens RES4 for the original type-6 sampler. On the archived Level 5 camera this produces **two RES0 images and 400 authentic frame blits**, with the original source world frame nonzero indexed pixel census rising to **121,820**.
- Native HUD acceptance now checks rows 260..399: nonzero pixels must exceed 20,000 and no entirely black row may remain. It logs `CODE2_ORIGINAL_GAMEPLAY_HUD_BASE_LOWER_FRAME`. These gates passed using the archive, not the private save.

### Active VGA page sprite visibility

- The original sprite caller runs **before** `2ABF4` flips the CRTC visible page to the completed draw page. Testing the live visible CRTC directly at that point would falsely report no pixels.
- A read-only host copy now temporarily sets its CRTC start to the active guest draw page, compares before/after indexed scanout for each genuine `36CEC` call, and logs `CODE2_SOURCE_RES3_SPRITE_ACTIVE_SCANOUT` with source ID, source X, viewport row delta, count, bbox, page, genuine generated CPU writes and off-aperture count. Source media are not copied to logs.
- The archived native SAVE09 scene (a *different camera* from the owner's private `SAVE09(2).zip`) produced a genuine source sprite at `bbox=32,211,42,222`, **107 active-page changed pixels, all 107 inside the first 256 rows and zero off-aperture writes**.
- The owner-local runner now demands two independent active-page sprite footprints inside the original 256-row viewport, plus the two RES0 frame images / 400 authentic frames. Failing frames get an honest failure rather than a source-callback-only approval.

## Remaining work — do NOT call full HUD completed

The retail party panel routine `1F494` renders four character slots (offsets 0, 53, 211, 264), original RES1 portraits and equipment/hand overlays:
- `1F581..1F5EE` selects a portrait by `RES1:336 + sex*23 + face_index + (status&0x100 ? 46 : 0)`.
- It uses `298F0 -> 2A584` (masked image compositor), not ordinary `2939C -> 2A380`.
- The original hand widgets use `1EA90` twice for each party member; HP and status bars are also state-derived.
- Those masked RES1 portrait, HP, status, hand and control-icon routines **are not yet wired to the native complete frame**. Do not substitute a guessed portrait blit or fabricated UI geometry. Remaining work is to carry the original `1F494` and `298F0/2A584` side effects through the canonical original-source boundaries.
- Independent physical/or DOSBox VGA plane, CRTC and DAC registers for the exact frame are still missing. Synthetic tests, saved-world frame hashes, and public screenshots are not a pixel-parity oracle.

**Native private save:** `SAVE09(2).zip` was verified in the preceding audit, but the new source/build has **not yet been executed on that exact two-skeleton save**. The owner must run the packaged `run_two_skeleton_complete_frame.ps1` with the matching Windows binary, then return receipt/log/BMP or raw .idx/.pal. The next run's active-page per-skeleton footprints and lower-HUD pixel census provide positive observable acceptance gates, but independent retail DOSBox pixel/DAC parity remains a separate requirement.

**Data policy:** No original game binaries, saves, portraits, screenshots, RES bytes or palettes are committed.


## Verified build and archived regression links (same source revision)

- [Windows MSVC Milestone #171](https://github.com/Hyperbog/RLOFT/actions/runs/39216776172) at commit `0f1e9d614c66ee62c3ff0de3458c63865c22590a`: **74/74 CTests passed**, package upload successful, and packaged private owner runner passed PowerShell syntax validation. Package `RLOFT-Recomp-Windows-Milestone-171`, GitHub artifact ID `12170385541`, archive SHA-256 `60a7add78aa0555264baad9d1e8c8ad22a0e3b09ef0332c649b3466ccb8dd6df`.
- [Linux 74/74 regression suite](https://github.com/Hyperbog/RLOFT/actions/runs/39216776174): success.
- [Authentic archived Level 5 world compositor](https://github.com/Hyperbog/RLOFT/actions/runs/39216776118): success, including the new no-fully-black-HUD-row acceptance.
- [Source-world native view/texture gate](https://github.com/Hyperbog/RLOFT/actions/runs/39216776254): success.
- [Strict archived original save UI and sprite acceptance](https://github.com/Hyperbog/RLOFT/actions/runs/39216646786): success, including 400 original frame image blits, one real sprite's active-page image footprint, and the complete 320x400 indexed / RGB6 native outputs.

**Do not infer that the exact owner-uploaded two-skeleton `SAVE09(2).zip` has passed this new build:** that save is not on the GitHub CI runner and no owner-result frame from Milestone #171 has yet been received. The earlier owner result from Milestone #165 with 5,515 source sprite writes and offscreen/missing HUD was a different, older executable; it must not be used to claim new presentation success.
