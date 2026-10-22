# Part 26 — retained original actor, virtual-key movement, and 1F8FC scene frontier (2026-10-22)

**Branch:** `Recomp`. **Controlling validated source:** [`fc0b86426035d781cf963b0a3c9d96e9c02aef4b`](https://github.com/Hyperbog/RLOFT/commit/fc0b86426035d781cf963b0a3c9d96e9c02aef4b). **Windows original-media gate:** [#39858329878](https://github.com/Hyperbog/RLOFT/actions/runs/39858329878), **PASS 86/86 CTests** and mandatory live actor/input callbacks. **Linux regression:** [#39858252216](https://github.com/Hyperbog/RLOFT/actions/runs/39858252216), PASS at preceding code commit `ab637b5e`. **Source-link gate:** [#39858252200](https://github.com/Hyperbog/RLOFT/actions/runs/39858252200), PASS.

This is a verified **source/runtime checkpoint**, **not** a playable build, DOS pixel-parity acceptance or fully rendered original Level 22.

## Original actor post-load path is now executed live

Commit [`497828b0`](https://github.com/Hyperbog/RLOFT/commit/497828b09bf23087650193be070e6b4d17ed68bf) attaches a real `Application31Boundary` inside the retained `181B4` level-loader call, so the original `17A68` routine and `17838` party pass run **on the same original CODE.2 guest** after deserializing Level 22:

- `1C1DC` quest-state checks and `2ACDC` actor-type predicates use compiled original source and actual retained DGROUP.
- `30CA2` random and `1C328` roll callbacks are installed; the verified **initial Level 22 load triggered zero RNG requests**. Do not assert random behavior was exercised on that path.
- Unsupported `1C77C` dialogue explicitly fails closed; original actor AI and scheduled world activity are not integrated.
- The independent original-media oracle still matched all **111** original Level 22 actor records (1 moving, 110 primary, zero alternate, 10 column objects).

Required live log: `CODE2_ORIGINAL_LIVE_BOOT=17A68_ACTOR_POSTLOAD_SOURCE_EXECUTED` with `quest_predicate=1C1DC actor_type=2ACDC`.

## Original virtual keyboard control path now executes nonzero scan codes

Commit [`77f0914b`](https://github.com/Hyperbog/RLOFT/commit/77f0914b36d1ad470543843bc43a7582d7da4e8e) drives the actual suspended `1FC14 -> 30B40` runtime with `0x0148` (forward) and `0x014B` (turn left) **after** the two existing zero-key iterations, on the same guest memory and source call stack.

In the Windows original-media trace:
- Original forward accumulator at `C000`: **0 -> 65526**, interpreted as signed **-10**; gain **10**, limit **160**.
- Original turn accumulator at `C004`: **0 -> 65526**, signed **-10**; gain **10**, limit **160**.
- Each resumed source iteration returns to `30B40`, observes one original `C050` page-state toggle and retains the guest.
- Camera/party pose at `C658/255D2/255D6` stays unchanged, **not** verified input-controlled world motion. The original `20B40` world-update and `1A510` scene refresh callback graph is still absent.
- These are **explicit virtual-host test keyboard replies**, not physical keyboard events or player interaction.

Commit [`fc0b8642`](https://github.com/Hyperbog/RLOFT/commit/fc0b86426035d781cf963b0a3c9d96e9c02aef4b) makes both live actor and keyboard evidence **mandatory** in Windows original-media CI.

## Original compositor frontier narrowed with the retained guest

Commit [`ce5e69b3`](https://github.com/Hyperbog/RLOFT/commit/ce5e69b3697c42a97982acd37509383156ec726d) prepares the direct **source** RES image drawing bridge `2E510 -> 2E4A4 -> 2939C -> 2A380`, using original unloaded resource decoding, source video blit, and the retained original VGA device. Original source `2EFD0` release is also prepared. These are **prepared adapters**, not a completed gameplay scene or evidence the bridge has been executed by the real `1F8FC` branch. The isolated `1F8FC/17A68` synthetic control-flow test still passes.

The first attempt to set the live `181B4` `call_1f8e0` immediately hit the **genuine changed-scene** boundary, and Windows run [#39857888651](https://github.com/Hyperbog/RLOFT/actions/runs/39857888651) **failed** with `ORIGINAL_181B4_1F8FC_SCENE_COMPOSITOR_NOT_CONNECTED`. This failure was not hidden or marked as success.

Commit [`ab637b5e`](https://github.com/Hyperbog/RLOFT/commit/ab637b5e8d94f4416e8443b805f108afd91c9039) retains the new source VGA adapter but **leaves the live changed-scene callback disabled until all required original callbacks exist**, preserving the previously validated uninterrupted guest runtime. Windows run #39858329878 **passed**.

Original-media mode selection was observed precisely:

```text
CODE2_ORIGINAL_LIVE_BOOT=1F8FC_SCENE_MODE_FRONTIER requested=4 retained=3 original_30070_capture=NOT_CONNECTED scene_rendering=NOT_CONNECTED
```

Thus original Level 22 requests **mode 4** while the prior retained scene palette/art mode is **3**. Do not use a fake mode-3 unchanged-scene return or replace the original frame with diagnostic artwork. `1F8FC` needs original `30070` captures, captured-image lifecycle, `1F494` party HUD, `1F1C8/1F120` world, source `325B0/29234` mode-4 COLORS palette and `1A5B0` cursor/page update in **the same guest**.

## Immediate next source integration

1. Implement original `30070 -> 292BC -> 2A114` generic page capture through the retained planar VGA host, with authenticated format/size and original `30040/310B0` image handle lifecycle, **before enabling `call_1f8e0`**.
2. Connect source `1F8FC` world/HUD and mode-4 palette callbacks on the same resource/VGA session; fail closed at any unknown callback or ABI. Validate indexed scanout pages against independent DOS capture where available.
3. Connect source `1FC14 -> 20B40` camera/actor update and `1A510` frame presenter, and virtual input/physical OS keyboard bridge. Verify successive nonzero-input frames with changed source camera positions.
4. Compare genuinely produced indexed VGA frames plus DAC6 palette with independent original DOS frames captured from matching unchanged original saves, poses, tick and palette state. Neither input control state nor two page toggles constitutes pixel parity.

**Do not declare:** original scene completed; actual movement; physical input; playable executable; complete actor AI; cross-level transitions; DOS screenshot parity; or R04 acceptance. Do not commit copyrighted original assets or private screenshots.
