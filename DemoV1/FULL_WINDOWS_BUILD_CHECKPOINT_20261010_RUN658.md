# Windows Full #658 — Level22 active-ray ground gating compiled and tested (2026-10-10)

**Result: SUCCESS.** [Authorized GitHub Actions run #658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) (run ID `38085278540`) finished successfully on 2026-10-10 at **20:58:03 UTC**.

| Acceptance item | Independent CI/source evidence |
| --- | --- |
| Build checkout / embedded source revision | `ba3dc29e42d93e87a147b4b909c52167dbd3b362` |
| Prior source-bearing native gameplay commit | `b4c46bc8a4435d85f648a46900ec6e46f33a6cdf` |
| Full Windows C++ target compilation | **PASS** |
| Original `CODE.2` C++17 syntax preflight | **PASS** |
| Windows CTest | **214/214 PASS (100%)**, 4.26 seconds |
| `exploration_presenter_tests` | **PASS**, Test #105, includes dormant/active original material checks |
| Original-source EH6 selection/regression oracles | **PASS** |
| Published artifact | `Ravenloft-Demo-v1-Windows-x64`, artifact #11682436651 |
| Artifact ZIP bytes | **826,748** |
| Artifact ZIP SHA-256 | `13f64cf8a50017f7cf8ddda2eebb54b0c36fa51c949374589c9c6d79b771d6c9` |
| Native EXE SHA-256 | `7e2ad0c2e23e2a4a922b654ea3e0f28e0ce76764540f544f3bf076af8b35b466` |
| Workflow-published EXE commit | `b60684537f7d68fab8a195824d92d0108615b75c` |
| GitHub Actions workflow after run | Original manual-only blob `7c8e5720f38c0690d01457ea3daf556124725ef2`, one-shot trigger removed |
| Linux job | Skipped according to user-authorized workflow configuration |

## Source fix accepted by native Windows compilation/tests

The build contains the [source-verified active-ray gate](LEVEL22_657_GROUND_ACTIVE_MATERIAL_GUARD_20261010.md) from commit `b4c46bc`. Prior build #657 proved original RES4[655] EH6 decoding succeeds but 54/54 user-supplied Level22 renderer captures still displayed flat floor. Field ZIP analysis independently established all 54 had **256 source MAIN rays with resource=147 (0x93)**, zero PREPASS runs, original ground source verified, original `C026=1` and original perspective ready. The old native `original_full_ground_eligible` wrongly required *both* `resource_inner` and **inactive** `resource_outer` to be 0x93. Original CODE.2 `335DC` selects outer only on `prepass==1`; original `336F7` always selects inner for the main draw.

The corrected code preserves the 256-ray proof, per-frame source resource, original perspective, original Level22 gating, original C026, and valid 4098-byte prefixed texel payload. It accepts only `inner==0x93 && (prepass==0 || (prepass==1 && outer==0x93))`; active material remains disqualifying, invalid prepass values fail closed. The native CTest explicitly checks dormant outer non-sentinel passes, active outer non-sentinel fails, active outer sentinel passes, invalid prepass flag fails, main non-sentinel fails, and original payload/level/ray-count guards remain.

Additional **read-only** renderer capture fields are compiled: `ground_sweep_ray_count`, `ground_sweep_main_nonsentinel`, `ground_sweep_active_outer_nonsentinel`, `ground_sweep_inactive_outer_nonsentinel` and `ground_sweep_invalid_prepass`. No source-game RNG, level assets, actor events or IRQ handling was changed.

## Field verification: open, NOT a Windows CI pass

Build #658 does not have the user's private original GOG media in the runner and cannot validate original DOS pixel equivalence. On identical installed media, a successful open-field Level22 capture is expected to report:

```text
ground_original_res4_verified\t1
ground_original_res4_base_from_INIT\t600
ground_original_res4_absolute_entry\t655
ground_ff58_ready\t1
ground_sweep_ray_count\t256
ground_sweep_main_nonsentinel\t0
ground_sweep_active_outer_nonsentinel\t0
ground_sweep_invalid_prepass\t0
native_lower_surface\toriginal-34A74-128row-open-field
ground_raster_indexed_writes\t32768
```

The expected `ground_sweep_inactive_outer_nonsentinel` can be **nonzero**, and this is no longer a reason to withhold source-owned 34A74 drawing. If any alternate active wall material appears, do not force full span or guess FF50 cropping. Record exact per-frame output without calling the visual complete.

To verify: download [#658 Windows ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540), extract it into a **fresh directory**, run `Diagnose-Level22.cmd`, capture a few F12 markers in the unobstructed forest view, exit and upload the resulting diagnostic ZIP from `%TEMP%\\RLOFT-Demo-Diagnostics`. Probe need not be rerun: build #657 already independently decoded source `RES4[655]` with **4,096** pixels and FNV1a32 `c1e71881`; no further original-resource file upload needed.

Even with source-ground textured pixels appearing, original DOS 320×400 indexed framebuffer/palette and original original IRQ/RNG timing parity are separately **UNACCEPTED**, and no provisional **49.8%** weighted engineering-readiness or DOS-acceptance score is increased by this CI-only checkpoint. Finish the original complete game faithfully and keep all future CI executions manually authorized.
