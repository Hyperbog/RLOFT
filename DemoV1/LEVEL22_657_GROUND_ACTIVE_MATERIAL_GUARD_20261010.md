# Level 22 #657 field audit: dormant alternate wall bank blocked 34A74 — 2026-10-10

**Branch:** `demo-v1`. **State:** source correction and regressions committed, NOT YET compiled in Windows CI; DOS pixel/timing acceptance remains pending. The user's Build #657 diagnostic ZIP contains proprietary indexed frame captures, so only bounded metadata and provenance appear in Git.

## Uploaded original-media and native build evidence

- `RLOFT-Level22-RES4-Ground-31223.txt`: original `INIT+0x0E` base **600**, signed Level 22 header+7 **+55**, selected authentic **RES4[655]**, compressed on-disk `EH6`, raw **2,515 bytes**. The new native loader returns **4,096** indexed texels, `selected_decoded_64x64_verified=1`, `selection_error=none`; selected tile FNV1a32 **`c1e71881`**. Archive contains 667 EH6 entries; 67 eligible 64×64 entries, 0 invalid EH6 entries. This closes the former EH-type loader mismatch.
- `1791664739233-21804.zip`: manifest gives **Build #657 source `68fd2ccbcb8db46f53bc224dabb7a22a996ebadc`**, 320×400 indexed pixels, native preview. **76 contiguous events**, **29** automatic indexed captures, **7 F12** markers, **54** separate renderer frame `.tsv/.idx/.pal` triplets. There is no error event or game crash in this bounded trace.
- All **54/54** renderer `.tsv` frames report: `ground_original_res4_verified=1`, selected entry `655`, `ground_ff58_ready=1`, `original_C026_initial=1`, but `native_lower_surface=flat-fill-fallback;other-modes-pending`, `ground_raster_indexed_writes=0` and diagnostic refusal `original full 128-row 34A74 requires all 256 sweep rays to carry 0x93`.
- All **54** frames are at map cell **(4,4)**; the camera rotates through most headings without advancing. The `run main` diagnostic uses source-selected resource **147 (0x93) on every recorded main run**; `prepass_runs=0,0,0` throughout. Other RES3 scenes and terrain backdrop are rasterized, but the lower ground is visibly a flat band in the indexed F12 images. It would be incorrect to label this as corrected merely because the ground resource was decoded.
- ZIP SHA-256 (provenance only): `9c8ba4ebe8849ad418cb176c8f539b92ea313b7e5ab968b4689f39dde1cf6adf`.

## Independently source-grounded gating distinction

Original CODE.2 `335C8` initializes the nearest wall-depth sentinel 0x800. The existing source-backed `ViewSweepRunModel::build`, corresponding to `335DC` source prepass and `336F7` main pass, establishes two *different* active values:

- **Ordinary/main pass (every ray):** `resource_inner`. If equal to 0x93, it is non-drawable, so the original 0x800 wall-depth sentinel remains unaffected.
- **Optional prepass (only rays with `prepass==1` at sweep-record +0C):** `resource_outer`. If active and non-0x93, the full-open-field 128-row case cannot be proven, and native falls back pending dynamic FF50/cropping reconstruction.
- **Inactive alternate:** when `prepass==0`, even a non-0x93 `resource_outer` is **not read by the original prepass** and cannot affect whether the 0x93-only main wall scan drew anything. An unconditional check of both fields for 0x93 is stricter than original execution.
- **Unknown prepass values:** fail closed rather than guessing their source behavior.

The existing native `ExplorationPresenter::original_full_ground_eligible` erroneously rejected non-0x93 `resource_outer` even when `prepass==0`. This is the bounded second blocker exposed only after fixing original RES4 EH6 loading. This source correction changes that predicate to **`main==0x93 && (prepass==0 || (prepass==1 && outer==0x93))`** while retaining Level22, exact 4,098-byte original texel payload, full 256 ray count, original FF58, initial C026 and all other raster safeguards.

The existing `exploration_presenter_tests` now explicitly tests a dormant drawable alternate as *eligible*, an active drawable alternate as *ineligible*, an active original sentinel as eligible, unsupported prepass values as ineligible, drawable main material as ineligible, and previously existing truncated/missing level/texture guards. The CTest registry is unchanged.

New read-only indexed renderer `.tsv` fields count **`ground_sweep_ray_count`**, **`ground_sweep_main_nonsentinel`**, **`ground_sweep_active_outer_nonsentinel`**, **`ground_sweep_inactive_outer_nonsentinel`**, **`ground_sweep_invalid_prepass`**. These permit a later original DOS-paired comparison to prove *which* field is active without giving incorrect 'all 256 rays' failure reasons.

## Acceptance boundary and next step

The original EH6 resource identity and decode are **verified**. The source-backed conditional guard is **not yet Windows-built or field-accepted**. If it compiles and main+active prepass counts are zero, the native `34A74` raster should be attempted in this witnessed Level22 open-field orientation. It may still fail for separate original C026/FF50/22220 mode, source viewport, or render-order reasons. Do not force an unconditional texture or substitute arbitrary source art.

After explicit user build authorization, run Windows Full, validate every CTest, and obtain one fresh `Diagnose-Level22.cmd` field capture; verify `ground_raster_indexed_writes=32768` when the complete original 128-row path applies and the actual lower surface says `original-34A74-128row-open-field`. Then compare original DOS framebuffer/palette/tick data independently. Do not increase provisional 49.8% readiness or DOS acceptance from source-only evidence.
