# Field P0 — Windows owner GUI QA: timing, flicker, VGA partial frame

**Date:** 2026-10-13  
**Branch:** `native-engine`  
**Implementation checkpoint before this note:** `09fe842579924f506071d6a513a717644c3c6ee5`  
**Reported blocking issue:** [#23](https://github.com/Hyperbog/RLOFT/issues/23)  
**Classification:** **Pre-Alpha**. Original CODE.1 → CODE.2 Level22 game launch is actually observed on the owner's Windows display, but scene composition and audible playback are incomplete.

## Original owner field observations

The supplied screenshot is an actual native first-party **Level22** HUD/3D world view after the original two-character workflow completes. It shows a brown/banded sky and speckled flat ground; real NPC9/placed source positions appear only as colored dots in the minimap. The owner reports no audio, visible flicker, excessively rapid animations, missing rendered trees/NPCs and a still-broken separate **CODE.2 VGA Reference View**.

**Minimap:** owner likes current look and requests postponing all minimap work. This batch does not change the minimap.

## P0 corrections implemented in the source

1. **GPU has only one present owner.** `win32_native_gpu_exploration.cpp` previously called `s->paint()` (including `SwapBuffers`) directly inside every focused 16ms `WM_TIMER`, *and* independently in `WM_PAINT`. It now executes only `step(dt)` plus `InvalidateRect(hwnd, nullptr, FALSE)` from the timer and performs all GL drawing and `SwapBuffers` once through the Windows `WM_PAINT` path. `WM_ERASEBKGND` was already suppressed. This eliminates a specific concurrent presentation flaw; it does not certify GPU driver/v-sync functionality.
2. **Original CODE.1 opening / CODE.2 menu no longer step a 70-frame source animation at 14ms per frame.** The prior host schedule ran 70 CODE.1 frames in about **980ms**. The new *provisional* presentation fallback is `kNativeLegacyAnimationFrameMs=55` (~18.2 frames/sec, **3.85 seconds** for 70 frames), independently polled on a `16ms` Win32 timer. The positive original `ANM` `frame.wait_ticks` schedule continues to be honored as before; only its zero/default fallback changes to the conservative 55ms.
3. **GDI screen compositing is atomic.** `win32_native_startup.cpp` paints the full original 320×400 graphics/palette and controls onto a compatible offscreen bitmap then `BitBlt`s once. `win32_native_exploration.cpp` independently paints the 320×400 source CPU reference DIB, map, controls and backdrop to an offscreen bitmap then performs one GDI `BitBlt`. **The reference image source itself now uses a true top-down 32-bit `CreateDIBSection` bitmap plus `StretchBlt` between memory DCs**, avoiding device-dependent `StretchDIBits` source cropping that had previously left only a thin visible strip. If DIB-section allocation fails, `StretchDIBits` remains the fallback and a visible error appears when even that fails. This targets incomplete visible frame publication, but **cannot certify original DOS VGA reference scene parity**.
4. **Regression:** `native_presentation_timing_tests` proves monotonic interval gating (including backward clock rejection), that a 16ms host polling callback cannot advance a source animation prematurely, and that 70 presented frames take a deliberately bounded 3–5 sec on the provisional cadence. The native Windows package must include this test, bringing CTest count to **44**. Windows CI triggers on all modified Win32 playback files.

### Actual MSVC acceptance gate

PR [#24](https://github.com/Hyperbog/RLOFT/pull/24), first Windows [run #124](https://github.com/Hyperbog/RLOFT/actions/runs/38481438055), **failed MSVC compilation**: the new offscreen Win32 startup paint measured two `LONG` client extents with `std::max(int,LONG)`. The failure is isolated to source `win32_native_startup.cpp`; the original archive was never altered and no CTests were run by that failed job. Commit `390020182679bfd422e96c487746b9adcb6f70b0` and QA mirror `0dc4292cd46e6bc439911d7f8172afc0ae86f084` explicitly convert both Win32 extents to `int`.

A later QA patch `c3c78e10d1e2ff67a0021125828f2c8b25c4b5d2` adds the DIB-section fallback. It requires **fresh exact-head CI #128** ([run 38481934975](https://github.com/Hyperbog/RLOFT/actions/runs/38481934975)) with all **44 native CTests** and immutable original-media audit before accepting the source. Do not confuse a successful compile of one source file with a passing original-data audit.

## Non-claims and remaining blockers

- **Timing parity not established.** The historical timer research (`PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md`) confirms independent IRQ/gameplay/PIT wait layers. A DOS game frame is not inherently one BIOS tick. The 55ms timing here is a *controlled usability baseline*, pending visual comparison and source-real CODE.1 scheduling evidence. ANM positive original wait packets remain explicit source timing.
- **No sound is implemented.** `rloft::AnmCinematic` parses genuine `MusicSequence`, `QueuedSfx` and `DirectSfx` commands. Native startup only increments `cinematic_audio_events`; no corresponding WinMM/WASAPI audio stream or original XMIDI/VOC playback is connected. An audible original-source music and SFX pipeline requires its own decoder, buffer lifetime/audio-device handling, timing gate, CI, and owner listening acceptance.
- **No actual Level22 RES3 authored entity draw.** The 111 original source scene placements exist and the map markers derive from them, but CODE.2 live view-frame selection/projected EH7 image composition and NPC9 script AI remain open. Do not patch with invented geometry or off-source enemy motion.
- **VGA Reference View correctness open.** Double-buffering addresses partial paint publication only. Its independently projected CPU content still requires owner screenshot comparison/reproduction, and original DOS VGA parity is not certified.
- **No on-device confirmation yet.** Even if MSVC + CTests + immutable original-media audits pass, repeated field play with a Windows GPU, actual frame rate and screenshots/video is required before resolving issue #23. A screenshot alone cannot prove flicker/frame pacing.

## Required field acceptance sequence

Run source-selected executable from the Windows CI artifact with the owned original files; allow CINE00/CINE12 and CODE.2 menu, then enter original CODE.1, observe the complete 70-frame animation, finish two actors and enter Level22. Compare before/after flicker and frame pace, and open the separate CPU VGA Reference View. Test full-screen and native/HD renderer modes only after normal mode stabilizes; verify source original files remain unmodified. Collect video or repeated-screenshot evidence if any flicker persists. Test original audible playback *after* actual audio plumbing lands.

**Hold Level23 and Beta** until a normal playable Level22 loop exists with real original scene objects and encounters; keep minimap refinement postponed.
