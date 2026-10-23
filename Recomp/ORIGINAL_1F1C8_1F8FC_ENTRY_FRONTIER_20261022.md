# Part 28 — Original 1F1C8 exterior and bounded 1F8FC frame entry

Branch `Recomp`. Scope is original, retained CODE.2 source, original RES archives, DOS/4GW guest memory and planar VGA. **Do not label any result a completed gameplay frame.**

## Existing original exterior source frontier

`Recomp/src/original_code2_live_exterior_prefix.inc` executes the actual compiled `1F1C8` and source `1EDAC`, `334C4 → 32954` ray setup, `34998` four angle patch callbacks, and `314B0` active source environment through the first RES8:50 subtractive sprite raster `2A478`. It is opt-in `RLOFT_RECOMP_EXPERIMENTAL_EXTERIOR=1`; original-media Windows CI was green [#39883670234](https://github.com/Hyperbog/RLOFT/actions/runs/39883670234). The source intentionally stops before the remaining 31 sprite draws and `34DE8` polygon raster, the `1BDAC/2C294/274A4` integration and full presentation. One authentic RES8:50 type-1 decoder event extends the former ten-resource ledger to eleven.

## Newly connected original 1F8FC source entry

Added `Recomp/src/original_code2_live_1f8fc_entry.inc` and an opt-in `RLOFT_RECOMP_EXPERIMENTAL_FRAME_ENTRY=1` hook in `Recomp/src/code2_startup_gate.cpp`. A fresh `Application45Boundary` invokes actual **compiled `code2::function_1f8fc`** on the SAME retained Level 22 guest. Its first two `2E510` calls decode and blit genuine original source **RES0:25 and RES0:26** at (row,x) = (0,0) and (200,0). The first `2EFD0` release callback validates its expected original argument and deliberately stops at a separate scoped guest source frontier, before HUD/world/presentation callbacks. It does not fake a screenshot, suppress callbacks, invent frames, or connect the user-facing continuous loop.

Original-media Windows probe [#39884342807](https://github.com/Hyperbog/RLOFT/actions/runs/39884342807) confirmed:

```text
CODE2_ORIGINAL_LIVE_BOOT=1F8FC_SOURCE_FRAME_ENTRY original_source=1F8FC_2E510_2F29C_2939C_2A380 backgrounds=2 first_RES0_id=25 second_RES0_id=26 original_release_boundary=2EFD0 original_guest=PRESERVED scene=NOT_YET_CONNECTED completed_frames=0
```

That run failed a previous exact eleven-decoder expectation because authentic RES0:25/26 added **two further 2FD9C decodes**. The exact ordered ledger is now 13 events:
RES0:59, RES0:62, RES1:338, RES1:362, RES0:54, RES0:49, RES1:37, RES1:34, RES0:60, RES0:34, RES8:50, RES0:25, RES0:26.

Updated `Recomp/src/code2_original_res_startup.cpp` to require those 13 exact original events if BOTH diagnostic flags are enabled, independently decode every IMAGE from its original archive, compare all retained IMAGE bytes, and prove `2F29C` cache pointer reuse. Earlier ten- and eleven-resource configurations remain independently supported. The strict Windows gate also checks the two source background draws and the original first resource-release frontier.

Commits:
- [`5de09326`](https://github.com/Hyperbog/RLOFT/commit/5de09326831e103939aab5c80472a973e98b58d4): bounded 1F8FC source entry
- [`37c32720`](https://github.com/Hyperbog/RLOFT/commit/37c327203370bbb72108d6a5e8dde8822c130f17): experimental retained hookup
- [`ec09e7fb`](https://github.com/Hyperbog/RLOFT/commit/ec09e7fb5961d161cd39d27825a85689a5f15e92): Windows entry acceptance
- [`4e6aaaa2`](https://github.com/Hyperbog/RLOFT/commit/4e6aaaa21c5c4e87c4121f7cd94e51bda72e3160): strict 13-resource original parity
- [`f6552f97`](https://github.com/Hyperbog/RLOFT/commit/f6552f97e36af529ecb80becc231d9a6526c6c95): CI exact source parity

## Unfinished work

For full 1F8FC connect original `2EFD0` cache release, `30070` capture and source party `1F494` in the SAME compositor call (rather than separate checkpoints), then `1F1C8` exterior polygon pipeline and `1F120`, `325B0` palette switching, `1A5B0` presentation, original page toggle and event callbacks. Prove consecutive, input-driven indexed VGA frame hashes against actual DOS behavior. No claim of a playable recomp until this exists.
