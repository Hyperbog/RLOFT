# Part 25 — original CODE.2 20440 palette and VGA page source continuation

**Branch:** `Recomp`. **Validated source commit:** [`d244896`](https://github.com/Hyperbog/RLOFT/commit/d244896556c53430afbc91bd9f941b7e973371a3).
**Original-media Windows gate:** [#39705282061](https://github.com/Hyperbog/RLOFT/actions/runs/39705282061) — **success; 85/85 Windows CTests**, plus mandatory `ORIGINAL_CODE2_20440_COLORS_PRESENTATION_ACCEPTED`.

## Retained CODE.2 original-source execution, in 20440 order

After source `16D04`, `03838` SINTAB, present-CHARS `16CD0 → 16ADC` and the second original `20294` source two-page clear:

1. `325B0`: now invoked as original compiled `function_325b0` from `20440`, using original `32588 → 312A0`, `3131C`, `312EC`, `32150` and `312DC` through the retained read-only DOS file callbacks. Authentic `COLORS` palette **bank 3**, 256 RGB6 triplets, is compared against an independent read-only file read; no file bytes are committed. The already-authenticated initialization sets zero brightness, so `32150` does not modify RGB6 values on this validated path. The original `29234` follows and streams the resulting guest palette to the existing VGA DAC.
2. `2F844`: previously connected compiled original source body, retained in source order.
3. `291B0`: newly connected original output-page function through `source_video_boot::Bridge`, selecting **page 1**.
4. `291C8`: newly connected original source scanout-page function via original `29FFC → 2ABF4`, selecting **page 0** and programming **CRTC start 0**.

The original `181B4` Level 22 archive loader and subsequent two `1FC14 → 30B40` source keyboard-loop iterations still execute, without guest restart or synthetic image output. `original_325b0_completed`, boot output-page and boot scanout-page counters must each equal 1 before the keyboard-loop entry is accepted.

## Acceptance evidence

The original-media gate compares all **768 original COLORS bytes** with the post-source guest palette, all **256 VGA RGB6 entries** against the guest palette, both original page registers (`EF4D` and `EF4E`), the page-stack depth, and the hardware CRTC start against the exact original `29FFC` formula derived from guest `F005`. Source callback/witness chain:

```
CODE2_ORIGINAL_LIVE_BOOT=325B0_SOURCE_COLORS_COMPLETED bank=3 RGB6=768 original_guest=PRESERVED
CODE2_ORIGINAL_LIVE_BOOT=291B0_OUTPUT_PAGE_SOURCE_COMPLETED page=1
CODE2_ORIGINAL_LIVE_BOOT=291C8_SCANOUT_PAGE_SOURCE_COMPLETED page=0 CRTC_start=0
ORIGINAL_CODE2_20440_COLORS_PRESENTATION_SOURCE_PASS compiled_palette=325B0 ... original_vga_palette=256_RGB6_MATCH guest_memory=PRESERVED rendering=NOT_CONNECTED
ORIGINAL_CODE2_20440_COLORS_PRESENTATION_ACCEPTED palette=AUTHENTIC_RGB6 pages=SOURCE
```

**Important remaining authenticity qualifier:** at initial resource selection, `source_palette_from_scene` still host-stages original scene bank/brightness and seeds the first VGA palette. That staging has not been removed or replaced with complete `1F8E0` original source initialization. The later `325B0` call above itself *is* original compiled source and is independently validated.

## Next source dependency: 2ECB4 → 2F184

Immediately after 291C8, `20440` requests `2ECB4(state,0)`, where `state=DGROUP[0x258DC]+0x2C`; `2F184(state,0)` follows. The existing original `Application68Boundary` provides archive opens/indexed reads but **does not yet connect the type-specific resource decode callbacks** (`2FD9C`, `301A8`, `app55` branches, etc.). Current reconstructed source `2ECB4` marks a record loaded after invoking a missing decoder, yielding an invalid success. Do not wire or accept this until the target original RES record's type and corresponding canonical decoder are verified. Next, finish `1A0B0 / 1A288` mouse/cursor callbacks, `181B4` world transitions, and the `1FC14` renderer. None is certified by this checkpoint.

**Not claimed:** native playable game, fully initialized scene/actors, actual DOS VGA pixel parity or input-driven frame rendering. Original game media remain private/read-only.
