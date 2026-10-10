# Level 22 — field confirmation of original RES4[655] 34A74 native ground raster — 2026-10-10

**Status:** **NATIVE OPEN-FIELD RASTER CONFIRMED from the user's Windows build #658 field diagnostic.** Original DOS pixel/IRQ/RNG fidelity **NOT YET ACCEPTED**. This file contains aggregate metadata and hashes only; original proprietary indexed frames, original resources, account-local paths and actor pictures are deliberately excluded.

## Immutable capture provenance

- Supplied field diagnostic archive: `1791666186872-27596.zip`, ZIP SHA-256 **`dd0b6f157816caf11035ecd8504ae17fde0e1f90ab936ba27916e04e295f4bd5`**. ZIP integrity verified; 408 entries. No automatic extraction of arbitrary ZIP paths or checked-in original images.
- Manifest: `rloft-level22-diagnostic-v1`, `native-preview-not-original-DOS`, build source SHA **`ba3dc29e42d93e87a147b4b909c52167dbd3b362`**. This is exactly the successful authorized [Windows Full #658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) checkout (**214/214 CTests**), not later documentation branch HEAD.
- Events: **110 consecutive source-ordered events** (`seq=1..110`), including one normal `session.finish`; **46** primary `frame.capture` events, **six** F12 `frame.marker` snapshots, and **64** independent renderer `.tsv/.idx/.pal` frame triplets. Primary automatic frames: **46** with **six** F12 marker dumps; no diagnostic error event observed.
- All 64 renderer captures carry exactly **128,000 indexed bytes** (320×400) and **768 bytes** of original VGA-6bit palette output. Frame metadata identifies Level 22, original installed GOG `game.gog` CD media, source revision, exact scene and camera fields. No original DOS buffer is supplied.
- Source-facing field samples turn through changing headings; 54/64 renderer frames remain at world cell (4,4), while the final 10 distinct positions advance through **(5,5), (6,5), (7,5), (7,6), (8,6), (9,6), (10,6), (11,7), (12,7)**. Thus the field set tests both rotation and movement.
- All **64** captured lower original source aperture x32..287/y132..259 have **unique SHA-256 pixel hashes**. Full-frame displayed images show a green/red stippled ground layer, source tree sprites, mountain backdrop and the party HUD. This confirms real indexed texture writes change with pose; it does NOT establish DOS pixel equality or prove every sprite/occlusion mask exact.

## 64/64 machine-checkable ground pass

| Source-owned diagnostic field | All 64 frames |
| --- | --- |
| `source_revision` | `ba3dc29e...` |
| `original_C026_initial` | **1** |
| `ground_original_res4_verified` | **1** |
| `ground_original_res4_base_from_INIT` | **600** |
| `ground_original_res4_signed_level_selector` | **55** |
| `ground_original_res4_absolute_entry` | **655** |
| `ground_original_res4_error` | Empty |
| `ground_ff58_ready` | **1** |
| `ground_sweep_ray_count` | **256** |
| `ground_sweep_main_nonsentinel` | **0** |
| `ground_sweep_active_outer_nonsentinel` | **0** |
| `ground_sweep_inactive_outer_nonsentinel` | **256** |
| `ground_sweep_invalid_prepass` | **0** |
| `prepass_runs` | **0,0,0** |
| `native_lower_surface` | **`original-34A74-128row-open-field`** |
| `ground_raster_indexed_writes` | **32768** |
| `ground_raster_error` | Empty |

The 64 ordinary/main `run` groups account for **256 ray positions in every frame**; every rendered run uses source sentinel material decimal 147 (`0x93`), and none has an active prepass. The previously over-restrictive native field check refused every build #657 frame because an **inactive** alternate bank was non-sentinel. Build #658 correctly ignores that inactive field while preserving the fail-closed active-ray test and source Level22/perspective/texture mode requirements.

Exact **32,768** indexed writes means **256 × 128** centered floor positions in the original reconstructed 34A74 full lower scanline rectangle. All 64 frames use the selected original EH6→runtime-type1 64×64 image previously independently matched against original `RES4[655]` FNV1a32 **`c1e71881`**. This confirms source-selected native **load + perspective + raster invocation** in the entire field series, instead of the old native flat fill. More restrictive states where a real drawable wall/prepass changes 22220/FF50 remain unaccepted until original dynamic clipping behavior is reconstructed.

## Fidelity and future work boundary

**CLOSED for the observed open-field scenario:** correct `INIT` + level-header original bank selection; exact EH6 resource decompression; valid 4,098-byte runtime tile; native full 34A74 lower-raster activation; 32768 indexed writes; real world camera rotation/movement without a recorded diagnostic crash.

**OPEN:** independently captured **original DOS** indexed VGA framebuffer and 6-bit palette at precisely matched level, camera fixed coordinates, facing, cell/actor/scene, original IRQ/tick, original RNG and resource-state values. Compare x32..287/y132..259 plus full viewport, report indexed pixel mismatches, palette differences, sprite draw ordering and original C026/C027/FF50 wall-occluded/cropped cases. Native self-CTests and native screenshots are not an original DOS behavioral oracle. **No DOS-parity acceptance or provisional 49.8% engineering-readiness change** is justified merely by success in this native field.

**No further RES4 ground probe is necessary for this source-selected Level22 asset.** Follow-on game work should prioritize authentic active-material/partial-depth ground FF50 cases and original F790/modal inventory integration under the project charter; do not modernize gameplay or substitute asset art.
