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

## Independent original OBJECTS.DAT record audit

The supplied untouched `OBJECTS.DAT` is **21,844 bytes** (SHA256 `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`). Its little-endian initial count is **336**, and its remaining **21,840 bytes** form exactly **336 records × 65 bytes**. Using zero-based record indices after the four-byte count:

- Object **37** is **MACE**, with original record byte `+0x2D = 37`.
- Object **8** is **BLUE ROBE**, with original record byte `+0x2D = 8`.

These two authentic item indices agree with the live Level22 equipment words (`37,8`), while the erroneous RES1 request `16724` is neither of the observed raw record selectors. **This does not yet prove** whether the fault originates in item-table initialization, an indirect guest pointer, the `1EA90` reconstruction, or a callback ABI. Preserve the original data and disambiguate these possibilities by tracing the source computed request before `2F29C`.

**Final verified CI for the fail-closed change:** [MSVC original-media #39875380876](https://github.com/Hyperbog/RLOFT/actions/runs/39875380876) PASS, [Linux #39875404226](https://github.com/Hyperbog/RLOFT/actions/runs/39875404226) PASS, [source-link #39875404122](https://github.com/Hyperbog/RLOFT/actions/runs/39875404122) PASS. These results validate normal startup and compilation, **not** opt-in HUD correctness.

## Fail-closed feature gate

Commit `bcf2b85e` keeps the diagnostic available only when **`RLOFT_RECOMP_EXPERIMENTAL_HUD=1`**. It no longer runs during ordinary original-media boot, preserving the preceding verified `1FC14` and retained VGA boundary until equipment source state is reconciled. The following source-only change corrects the diagnostic trace newline. Ordinary startup **does not imply** this HUD callback passed.

## Required next work

1. Trace the original `1EA90` equipment-to-RES1 request using authentic Level22 `OBJECTS.DAT` initialization and original CODE.2 callback ABI. Determine how equipped indices `37,8` yield requested `16724`; cross-check archive catalog and guest object descriptor pointers independently.
2. Make the explicit HUD diagnostic pass using unaltered actor/equipment data; require positive indexed HUD pixel changes and source dirty-bit verification, then gate it in original-media Windows CI.
3. Attach authentic source outdoor callbacks `33E28/33DE0/34DE8/1BDAC/2C294/274A4` through compiled `1F1C8`; only then enable `181B4.call_1f8e0` and the actual `1F8FC` compositor.
4. Validate full original `1A5B0` page presentation and multiple input-driven indexed VGA/DAC6 frames against independently captured DOS frames.

**Not accepted:** full party HUD, source world composition, mode-4 palette+screen, input-driven frame parity, DOS screenshot parity, gameplay-ready executable.

## Follow-up: original catalog loader repaired and retained 1F494 executed

The original retail `CODE.2` at `0x75F24 + 0x992` contains bytes `54 41` ("TA"), little-endian **16724**. With `[255D8]=0`, original `1EA90` evaluates equipped object 37 as `37*65+0x2D=0x992`, proving the anomalous resource number was a read from **uninitialized object-table DGROUP**. This was NOT an `298F0` six-argument ABI error.

Repaired `Recomp/src/code2_original_res_startup.cpp`:
- Preserved original `16D04 -> 16810` (`NPCS.DAT`) and `16D04 -> 168C8` (`OBJECTS.DAT`) via full `Application29/28/25/26` callback chain in the **retained second initializer**, not via host-inserted decoded table or artificial sprite IDs.
- Source allocates original 83 NPC records × 69 bytes and 336 object records × 65 bytes with the existing Watcom allocator, and populates `22230/255D8` in preserved original DGROUP via read-only file I/O.
- New independent gate compares all 5,727 NPC bytes and 21,840 object bytes with untouched original input. [Windows original-media #39876509515](https://github.com/Hyperbog/RLOFT/actions/runs/39876509515) PASSED normal startup, all 86 existing CTests, original level actors, and retained input; original-gate log emitted `ORIGINAL_CODE2_16810_168C8_RETAINED_CATALOGS_PASS npc_count=83 object_count=336 mace_graphic=37 blue_robe_graphic=8 ... original_bytes=INDEPENDENT_MATCH`.

Actual HUD exercise and VGA edge:
- Added explicit opt-in `RLOFT_RECOMP_EXPERIMENTAL_HUD=1` to original-media Windows CI, with mandatory source `1F494/1EA90` indexed-pixel acceptance.
- Retail terminal-party graphic at y=275, x=266, width=56 **crosses the scanline by two pixels**. Original `2A380` uses linear four-plane VGA addressing, without row-width clipping. The retained video bridge incorrectly required `x+width<=320`. Revision [11746c67](https://github.com/Hyperbog/RLOFT/commit/11746c670e3966192f0c76b1dbbee404a1efb991) checks the actual bounded linear 320×400 planar draw-page footprint instead.
- [Original-media Windows #39877377048](https://github.com/Hyperbog/RLOFT/actions/runs/39877377048) executed **retained compiled `1F494` / `1EA90`** with source `22F40,298F0,2975C`, two normal party panels, two terminal panels, and **12,966 changed indexed VGA pixels**. Exact log: `CODE2_ORIGINAL_LIVE_BOOT=1F494_1EA90_RETAINED_HUD_PROBE original_source=1F494_1EA90_22F40_298F0_2975C normal_party=2 terminal_party=2 changed_draw_page_pixels=12966 mode=3_PRETRANSITION_ONLY frame_status=HUD_ONLY_NOT_ORIGINAL_1F8FC`.

**Follow-on test failure, not evidence of incorrect resource bytes:** The same Windows run later failed the previous `code2_original_res_startup.cpp` assertion:

```text
INIT/RES source: raw_type==1u && target==59u && encoded_length==8 &&
s->guest->original_type1_decodes==4u &&
!s->guest->original_image_invalid && !s->guest->error
```

That test was written when only four original type-1 resources had been decoded. The now-enabled full HUD loads more original RES0/RES1 resources. Do not blindly change the equality to a weak lower bound. Record the actual additional type-1 IDs, verify independent originals and cache reuse, and **then** update the historical exact-count expectation with a new explicit resource-ID ledger. Current Windows original-media CI is **RED**; full retained HUD is a successful **pretransition pixel witness** but not yet CI-accepted. The other source branches remain independent; default gameplay startup still does not enable `1F8FC`.

**Next acceptance:** Update the genuine resource decode ledger and exact-count gates, then validate two actor portraits, four equipment draws, two terminal panels, palette tables, original HP bars, complete draw-page indexed pixels, and final strict Windows CI on the same retained guest. Mode-4 compositor, exterior `1F1C8`, real physical input, and DOS screenshot parity remain unconnected.

