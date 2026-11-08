# Part 29 — Original sampled-audio service and live host-input guest motion

**Date:** 2026-10-24. **Owning branch:** `Recomp`. This is a source-backed *bounded integration checkpoint*, not a whole-game milestone acceptance. No original `CODE.2`, RES media, indexed frames, VGA planes or DOSBox state bytes are committed.

## Source-authentic audio: 28168 → 27F24 → AIL

Independently disassembled owner-uploaded original `CODE.2` (`SHA-256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`) and exposed canonical CODE2 source in Windows original-media workflow. Original logical offsets:

- `28168`: checks the software sampled-audio enable dword `EE40`; examines two pending request handles `EE48` and `EE54`; if both exist compares **signed** priorities `EE50` and `EE5C`; calls `27F24` with the selected handle. Source clears **both** requests when both occupied, or just the present request when one is occupied. This is a single-dispatch queue service, not proof of two independent playback calls.
- `27F24`: returns immediately if `EE40==0` or the original digital-sound enable byte `EE70==0`; otherwise retrieves a sound packet using original resource lookup `2F29C(id,EE44)`, reads the length, copies the payload to the original `EE60` sound buffer, queries/stops prior playback via `36803/36817` when necessary, and invokes game-owned AIL device edges `367F9`, `367EF`, `3680D`. Exact source signatures use `Application55Boundary`; no host sound-generator semantics were substituted.

### New independent synthetic ABI test

`Recomp/src/test_original_code2_28168_audio_service.cpp` compiles the canonical `Code2.cpp` as original source and tests these precise operations, including a *synthetic* 8-byte sound packet, a synthetic device handle and callbacks, actual original guest-RAM packet copy, original resource selection, muted queue consumption and an instrumented driver callback sequence. It does **not** instantiate an AIL driver, produce PCM output to a real speaker, load owner-only sounds, or attest playback timing. The first CTest had a synthetic buffer destination outside the original object-2 memory range (segfault); the test-owned destination was corrected and guarded before acceptance at [a31826a7](https://github.com/Hyperbog/RLOFT/commit/a31826a756344f76e007dc143a9ab7ec99bc58e7). [Canonical Windows #40076285075](https://github.com/Hyperbog/RLOFT/actions/runs/40076285075) then passed **88/88** CTests, including `canonical_code2_original_28168_27f24_driver_abi`. The prior `28110` queue test remains included.

### Retained Level 22 guest bridge

`original_full_28168_audio_service()` is wired to the actual original `1FC14` post-HUD `Application45Boundary.call_28168` when experimental source-scene execution is enabled. It executes the **compiled original `function_28168`** on proven inactive/muted/no-request branches. If `EE40`, `EE70`, and a queued request all indicate an active device-dependent playback call, it stops **before** consuming queued game state; no fake driver result or silent successful playback is emitted.

[Windows original-media #40076994179](https://github.com/Hyperbog/RLOFT/actions/runs/40076994179) **passed** a mandatory source receipt gate and logged **five** separate actual `28168_SOURCE_SERVICE_RETURN` calls on the retained five-scene Level 22 loop. The original guest observed `EE40=0`, `EE70=1`, and both queue slots empty on all five: these are **source software-service returns**, **zero active audio-driver receipts**.

## Real host stdin through original keyboard service

The opt-in `rloft_code1_code2_staged_boot --root ... --cd-root ... --console-input` mode consumes actual terminal lines, maps S/D/W to proven original scan words `0x150/0x14D/0x148`, and resumes the same suspended original `1FC14 → 30B40` source worker. It captures **only source-owned** pre/post `C000` movement accumulator, `C004` turn accumulator, `255D0/255D4` camera 16.16 and `C658` heading; it never sets any guest motion field on the host.

The owner-original Windows gate pipes three known stdin lines and requires the original guest to return to its keyboard boundary after each one, with at least one genuine motion-state change. [Windows #40076790915](https://github.com/Hyperbog/RLOFT/actions/runs/40076790915), reconfirmed at [#40076994179](https://github.com/Hyperbog/RLOFT/actions/runs/40076994179), reports:

| Host stdin | Original 16-bit scan word | Original movement accumulator | Original turn accumulator |
| --- | --- | --- | --- |
| S | 336 (`0x150`) | `0 → 10` | `0 → 0` |
| D | 333 (`0x14D`) | `10 → 0` | `0 → 10` |
| W | 328 (`0x148`) | `0 → 65526` (signed −10) | `10 → 0` |

All three source keyboard polls and velocity/turn state changes were observed without host writes to the guest. **Camera X/Y remained `294912/294912` and heading `270` at every observed poll**; no player translation, turning the world camera, input IRQ9 timing or player-controlled GUI is yet certified. The original `20B40` visible assembly chiefly advances event/cell timers via `CF50` and `CD3C` record lists; do not assume it alone advances the camera. The position integrator/controller edge must be located from actual original code and executed under authentic guest state before upgrading the movement acceptance.

A separate unsynchronized stdin fixture missing the required source-rendering flags failed during original RES decode-count validation **before** stdin processing. That duplicate was removed at [7f5d4e16](https://github.com/Hyperbog/RLOFT/commit/7f5d4e1665238bba691f28f836674751820fab37), preserving the successful correctly configured scene/input test.

## Independent DOS screenshot / original saved-state parity

The existing [private capture procedure](PRIVATE_DOS_FRAME_SYNC_AND_AUDIO_INPUT_BRIDGE_20261024.md) records five source-owned `320×400` indexed scanouts, DAC6 palettes and matching camera/heading/keyboard-trace receipts, with no media payloads in CI. The Python `verify_synchronized_dos_frame_pair.py` refuses unmatched camera, CRTC page, input sequence, tick or presentation boundary, and `check_dosboxx_2026_10_frame_pair.py` independently checks the DOSBox-X screenshot against its own VGA save state before exporting original files.

**No independently synchronized original DOS screenshot/save-state/debugger observation was available in this task. Zero cross-execution retail DOS indexed frame pairs are accepted.** Source captures and synthetic comparator tests do not substitute for original DOS gameplay execution. Retail DOS RGB6 DAC parity verified in the prior checkpoint remains a *separate* result.

## Checkpoint and next source boundaries

- **Accepted:** compiled original sampled-sound queue/dispatcher semantics, original `27F24` synthetic-game-packet/AIL-call ABI; five original-source `28168` retained Level 22 post-HUD service returns; three real stdin events provoking exact original 16-bit movement/turn accumulator updates on the same guest.
- **Not accepted:** real AIL playback or completion callbacks; DOS audio timing/device equivalence; guest camera displacement; DOS IRQ9 physical input timing; synchronized independent DOS indexed pixels; fully playable native game. Whole-game R07/R08/R09/R12 gate statuses unchanged.
- **Next:** instrument and source-integrate the original game-camera movement consumer beyond the `1FC14` input accumulators; implement an evidence-backed host AIL device playback/status sink while preserving `27F24` semantics; run the strict independent DOS frame-pair tool when an actual matching DOSBox-X Level 22 screenshot, save-state and debugger observation are captured.
