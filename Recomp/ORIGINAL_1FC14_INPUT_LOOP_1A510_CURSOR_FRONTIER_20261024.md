# Original 1FC14 input-loop follow-up: 1A510 cursor/page integration frontier

Branch `Recomp`. Date 2026-10-24. The accepted **single** Level 22 original-source frame and retail DOS DAC hash parity remain the certified baseline; this checkpoint does **not** claim two rendered input-driven frames.

## Source inspection and implementation

Original `function_20608` enters the authentic `function_1fc14` input loop with the persistent `Application45Boundary`. The loop obtains key data from the retained `30B40` keyboard service, updates movement accumulators and world state, then invokes `Application45Boundary::call_1a510(page)`, flips `C050`, and proceeds with actor/HUD/audio updates. This differs from the complete redraw diagnostic `1F8FC → 1A5B0`.

New experimental implementation:

- [84b93b59c691](https://github.com/Hyperbog/RLOFT/commit/84b93b59c691133b590408892e9001eeb504566f) connects the actual `1A510` cursor/page source routine to retained original mouse, VGA and page-selection callbacks. It checks source `EF4E` and the physical CRTC start, and tracks source handoffs without claiming a world-scene redraw.
- [f8b73a907484](https://github.com/Hyperbog/RLOFT/commit/f8b73a907484ff198eb19a215de95993bb164f0a) adds exact argument diagnostics for the encountered original `29804` cursor draw.
- [4da5c36779a7](https://github.com/Hyperbog/RLOFT/commit/4da5c36779a79442a9e4876924c7c1080e8d4fed) connects the authentic clipped `29414 → 2A6B0` cursor raster using original VGA port callbacks. The previous bridge implemented only the un-clipped `2A380` case.
- [777a308a4862](https://github.com/Hyperbog/RLOFT/commit/777a308a4862beb43a7e1b5593c62e68ec5acfd5) quarantines this **unaccepted** per-tick experiment behind `RLOFT_RECOMP_EXPERIMENTAL_1FC14_PAGE_HANDOFF=1`. With that flag unset, previously validated original-source behavior remains unchanged.

## Actual Windows original-media findings

Two controlled experimental Windows source runs failed closed:

- [40057232644](https://github.com/Hyperbog/RLOFT/actions/runs/40057232644): the new `1A510` path reached source `29804` but stopped at `ORIGINAL_1A5B0_29804_MOUSE_SPRITE_UNSUPPORTED`.
- [40057361299](https://github.com/Hyperbog/RLOFT/actions/runs/40057361299): after adding the original clipped `2A6B0` callback, the same source draw still returned without a counted sprite raster. Precise guest parameters: frame `0`, mouse position `(200,160)`, original image address `34829` (`0x880D`), decoded frame count `11`, VGA source page-stack depth `1`. No image or VGA memory was manufactured to bypass this result.

Those parameters satisfy the bridge's outer cursor ABI. The remaining uncertainty is whether original `29414` **legitimately rejects the entire sprite under the active `EF55..EF61` clip**, returns 0, or invokes a raster that sets `invalid_access`/VGA unsupported-read status. The current wrapper ignores `function_29804`'s return value and requires exactly one sprite draw; therefore it cannot distinguish an authentic fully culled cursor from a raster error. Do not loosen that invariant until the return code, active clip values, and source VGA flags are recorded in a controlled probe.

## Accepted default CI retained

After the experimental opt-in was added, all three default workflows passed:

- [Windows original-media 40057646123](https://github.com/Hyperbog/RLOFT/actions/runs/40057646123): **success**.
- [Linux portable C++ 40057646144](https://github.com/Hyperbog/RLOFT/actions/runs/40057646144): **success**.
- [Canonical CODE.1 / CODE.2 source-link 40057646136](https://github.com/Hyperbog/RLOFT/actions/runs/40057646136): **success**.

The validated baseline is one original `1F8FC` scene frame, one `1A5B0` presentation/page flip, full source RGB6 DAC SHA-256 `0af8ea393f1f266596af59c1a2013350b51000b7456d377ab284ebbc43db0812` matching independent DOSBox-X, and source Level 22 selected indexed frame SHA-256 `b9649e13444cbdef42406a5077177ff0e15bb89325269067df219fbfbd85ef01`.

## Next engineering sequence

1. In the experimental 1FC14 callback only, instrument the **actual `29804` return result** (0/1/2), `29414` clip bounds and resulting cropped dimensions, `Bridge::invalid_access`, and VGA unsupported-read state. Verify legitimate zero-visibility cases without faking a drawn cursor.
2. After the authentic source `1A510` page/CRTC handoff passes, retain live `Application43/44` scene callbacks across the original `1FC14` loop instead of the one-shot `1F8FC` diagnostic's stack locals. Count two consecutive scenes produced *after actual keyboard input* on the same guest, including original `1F1C8`, `2C294`, `36CEC`, and `274A4` source steps, and capture each VGA index/DAC hash.
3. For strict original-DOS indexed comparison, obtain a same-pause original **Level 22** DOSBox-X VGA state with the matching camera and animation tick, including four planes, CRTC register state and full 768-byte DAC. The existing independent DOS screenshot/state captures are not temporally synchronized and belong to a different scene; they cannot establish pixel-by-pixel parity. Run `Recomp/tools/compare_dos_indexed_frames.py` only on independently authenticated, aligned original and reconstructed raw 320×400 index buffers.

**Status:** original 1FC14 input loop remains authentic and virtual key dispatch is tested; new 1A510 callback is diagnostic-only; accepted complete original-source frames remain **1**; input-driven multi-frame acceptance and indexed DOS parity remain **OPEN**.
