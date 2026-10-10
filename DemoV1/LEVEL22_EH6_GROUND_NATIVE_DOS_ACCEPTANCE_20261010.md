# Level 22 RES4[655] EH6 ground — native field acceptance and DOS parity gate

**Date:** 2026-10-10. **Workstream:** `demo-v1` native reconstruction. **Status:** exact original-game resource identity independently established, native EH6 decoder source-corrected, **original-DOS pixel acceptance not yet established**. Preserve original quirks. No original binary data or original art is embedded here.

## Original evidence already closed

- Original `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Nine EH6/load/37700 disassembly windows, seven direct call targets, and EH6 switch to `2EEC5` independently verified byte-for-byte.
- Original `INIT+0x0E` yields **600**; authentic `LEV022.DAT` header+7 yields signed **+55**. Original `CODE.2 1F388` therefore selects **bank RES4 entry 655**.
- User #655 source-media probe reports `RES4[655]` **on-disk `EH 06`**, **2,515 bytes**, prefix `45 48 06 00 00 00 10 FF 9A 9A A0 9E 3B`. Archive count 667, no EH1 on-disk records. The disk header's decompressed payload byte count is exactly 4,096. Source decompression is via `37590 → 37700`; loaded cache record becomes runtime type1 at `2EF38`; the original `34A74` sampler uses payload pointer+2.
- The separately recorded independent original-media EH6 LZSS oracle produces **4,096** indexed palette pixels, SHA-256 `2266dc05fa40037116bb7e6914624ee1d1deb0d636d4d404f58e3da3be94a0a0`, FNV1a32 `c1e71881`. This confirms the decoded *resource*, not the *rendered DOS frame*.

## Build gate

Authorized Windows Full [#656](https://github.com/Hyperbog/RLOFT/actions/runs/38083481440) passed all native C++ compilations and **213/214** CTests; the sole failure was the final Python `print` of a Unicode arrow under default Windows CP1252, after all source oracle assertions. Test-only ASCII fix `3dc37d0` is retried in [#657](https://github.com/Hyperbog/RLOFT/actions/runs/38084050726). **Do not call #656 a successful build** or attribute a future #657 pass to any subsequent commits. Any release candidate must have unambiguous artifact build-source SHA, complete CTest pass and executable hash.

## Source-media probe gate

Run `Probe-Level22-Ground.cmd` from the future successful Windows artifact with the exact same original installed game media. Compare its TXT report to:

| Field | Required observation |
| --- | --- |
| Original INIT base | 600 |
| Signed Level22 relative selector | 55 |
| Original RES4 archive entries | 667 |
| Source-selected archive entry | **655** |
| On-disk header kind | **EH6** |
| Selected decoded verification | **1** |
| Selected decompressed pixels | **4096** |
| Candidate 655 original EH type | **6** |
| Candidate 655 FNV1a32 | **c1e71881** |

If any mismatch arises, stop and inspect original resource selection/decompression; do **not** silently choose another similar 64×64 tile or relabel a missing original tile as accepted.

## Native renderer field gate

From the same successfully built artifact run `Diagnose-Level22.cmd` in Level22, stay in an unobstructed outdoor area where the original open-field 256-ray sentinel condition applies, press F12 at a representative view, walk/turn enough to change indexed pixels, press F12 again, then close the game and preserve the generated diagnostic ZIP. A source-driven successful capture should establish all of:

- `ground_original_res4_verified=1`, `ground_original_res4_base_from_INIT=600`, `ground_original_res4_absolute_entry=655`, and no resource-decode error.
- When all **256** original sweep rays report `0x93`, the source-guarded original `34A74` lower-raster path is eligible with **128** rows. Confirm that actual native lower-surface telemetry reports textured, **not** `flat-fill-fallback`.
- Correct original size semantics: 4,096 palette texels, 4,098-byte prefixed raster block, no fabricated green/procedural grass. Verify displayed indexed pixels and 6-bit DAC palette survive render/capture and sprite/collision layers remain ordered.
- Legitimate closed-field or non-source-verified dynamic `C026/C027`/`FF50` configurations must retain explicitly bounded fallback, never be treated as a general original-ground equivalence pass.

## Field gate outcome — Windows Full #658 source 34A74 raster now active

The user's build #658 diagnostic ZIP (full SHA-256 `dd0b6f157816caf11035ecd8504ae17fde0e1f90ab936ba27916e04e295f4bd5`) **passes the native open-field rendering field gate**: 64/64 renderer captures report original `RES4[655]` verified, 256 main rays with non-drawable `0x93`, no active prepass conflicts, original `34A74` full lower raster, exactly **32,768 indexed writes**, no raster error and 64 unique lower-aperture pixel hashes. See [complete field validation](LEVEL22_658_FIELD_TEXTURED_GROUND_PASS_20261010.md). These findings apply only to the observed open-field cases; independent original DOS pixels/ticks and dynamic FF50/cropped-wall states are still outstanding.

## Independent DOS parity gate (separate)

At matched original DOS and native Level22 **cell, camera subposition, facing, actor/scene state, palette and original game time**, compare source-authoritative indexed screen pixels (320×400), lower raster region x32..287/y132..259, original raster row/clipping, foreground-sprite occlusion, palette table, RNG side effects and IRQ-derived event cadence. Report pixel-diff counts plus bounded accepted discrepancies, with screenshots, hashes and exact build/source IDs. Native-to-native CTests or reference images generated by the same native renderer do **not** count as independent DOS validation. Keep this gate **pending** absent such paired evidence.

## Priority after field capture

If original EH6 texture resolves but the lower floor remains flat, investigate `ExplorationPresenter::original_full_ground_eligible`: it requires level 22, source-backed 4,098-byte tile, SINTAB perspective ready and exactly 256 original rays with `resource_outer==resource_inner==0x93`. Do not weaken this original-code guard speculatively; inspect diagnostic sweep and source `22220`/`FF50`/`C026` state before connecting more branch paths.

If accepted in field but not DOS comparison, only the native-resource decoding and source-guarded texture branch may be reported working; do not increase the overall 49.8% provisional engineering readiness or claim finished Level22. CI remains manual-only and no additional build is requested by this document.
