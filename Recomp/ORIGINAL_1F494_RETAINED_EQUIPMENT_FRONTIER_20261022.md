# Part 27 — retained original 1F494 HUD/equipment frontier (2026-10-22)

**Owning branch:** `Recomp`. **Status:** source-backed live HUD callback bridge implemented, compile-checked; **complete 1F494 retained HUD NOT accepted**, full `1F8FC` and exterior `1F1C8` still gated. No playable build, original DOS scene comparison or continuous frame acceptance.

## New retained-guest bridge

`Recomp/src/original_code2_live_party_hud.inc` is included by the retained `code2_startup_gate.cpp` translation unit. The diagnostic invokes compiled `code2::function_1f494` on the **same retained guest and VGA device** used for real `181B4`, the RES1:338/362 raster and `334C4/32954` rays, through the exact existing `Application43Boundary` and `Application44Boundary` callback signatures:

- `19CC4`, `24478`, `0EF14` source guest game clock, item predicate and HP arithmetic.
- `2F29C` original RES0/RES1 archive cache and `2975C/298F0` genuine `2A380/2A584` planar VGA raster.
- `29B40` retained original rectangle raster, `2E374` with original `291E0/29208` line callbacks, `2A258` animation, and original `2E510/2E4A4` RES0 frame operations.
- `22F40` original state overlay. `23024` group UI and `2E6F0/30508` numeral/font rendering **fail closed** and may not be silently suppressed.
- The diagnostic explicitly saves and forces original `CF5A..CF5D` dirty redraw flags (what `1F8FC` would do on its mode transition), checks source clearing, restores the flags, and compares indexed draw-page pixels. This is a **pre-transition diagnostic only**, never a replacement for mode-4 compositor entry.

## Authenticated failure, not a visual success

[Windows original-media run #39874792066](https://github.com/Hyperbog/RLOFT/actions/runs/39874792066) compiled all sources and passed **86/86 existing CTests** before failing in the newly invoked live HUD callback. The second [source diagnostic run #39875057730](https://github.com/Hyperbog/RLOFT/actions/runs/39875057730) reported:

```text
1F494_RES1_LOOKUP_OUT_OF_BOUNDS requested_id=16724
slot=1 entries=428 saved_mode=3
actor0_hands=37,8 actor1_hands=37,8
LIVE_CODE2_SOURCE_STOP=ORIGINAL_1F494_RES1_RESOURCE_RECORD_BOUNDS
```

Authentic RES1:338/362 decoded/drew successfully earlier in the same guest, as did authentic Level22 `32954` rays and the separate `37590` type-6 decoder. **An ID outside the RES1 catalog is not a rendering result**. It indicates an unresolved pre-transition equipment/item-source dependency or ABI/state mismatch; its precise cause is not yet established. Do not clamp the ID, fabricate sprites, skip the callback or assert normal-party HUD success.

## Fail-closed feature gate

Commit `bcf2b85e` keeps the diagnostic available only when **`RLOFT_RECOMP_EXPERIMENTAL_HUD=1`**. It no longer runs during ordinary original-media boot, preserving the preceding verified `1FC14` and retained VGA boundary until equipment source state is reconciled. The following source-only change corrects the diagnostic trace newline. Ordinary startup **does not imply** this HUD callback passed.

## Required next work

1. Trace the original `1EA90` equipment-to-RES1 request using authentic Level22 `OBJECTS.DAT` initialization and original CODE.2 callback ABI. Determine how equipped indices `37,8` yield requested `16724`; cross-check archive catalog and guest object descriptor pointers independently.
2. Make the explicit HUD diagnostic pass using unaltered actor/equipment data; require positive indexed HUD pixel changes and source dirty-bit verification, then gate it in original-media Windows CI.
3. Attach authentic source outdoor callbacks `33E28/33DE0/34DE8/1BDAC/2C294/274A4` through compiled `1F1C8`; only then enable `181B4.call_1f8e0` and the actual `1F8FC` compositor.
4. Validate full original `1A5B0` page presentation and multiple input-driven indexed VGA/DAC6 frames against independently captured DOS frames.

**Not accepted:** full party HUD, source world composition, mode-4 palette+screen, input-driven frame parity, DOS screenshot parity, gameplay-ready executable.
