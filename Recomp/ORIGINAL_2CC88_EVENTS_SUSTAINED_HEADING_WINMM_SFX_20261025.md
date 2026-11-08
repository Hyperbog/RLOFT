# Part 31 — Original 2CC88 collision, source event paths, sustained turning, and guarded WinMM SFX

**Repository**: `Hyperbog/RLOFT`, branch `Recomp`  
**Date**: 2026-10-25  
**Baseline**: Part 30 proved three authentic retained-original Level22 camera coordinate changes from host process stdin.  
**Retail DOS framebuffer and actual Miles/AIL32 device parity: OPEN.** No original copyrighted game samples, render frames, VGA palette, or DOS save-state are committed.

## Original movement and object collision

The compiled original graph is retained:

```text
host stdin → original CODE.2 30B40 source keyboard callback
  → original 1FC14 C000/C002/C004 velocity/turn setters
  → original 2E120 vector/heading integration
  → original 2DF2C (SINTAB smooth / 8-direction grid)
  → original 2DBAC writes 255D0 X, 255D4 Y
      ↳ original 2D5D0 primary tile and dynamic-entity checks
      ↳ original 2C800 actor-body check
      ↳ original 2CC88 SECONDARY object-record collision
          uses 258A6 count, 2223C/22238 world location,
          22246 object type, 255DC object metadata/radius
      ↳ original 2D3D8 portal transition → original 181B4 guard
      ↳ original 20C0C tile/event handler → 20A24 → original 28110 SFX queue
      ↳ original 1C0C0 heading / 1C1DC/1C124/1C160 event flags
      ↳ original 1EA78 page refresh → original 1A510 page callback
```

Changes are in `Recomp/src/original_code2_live_full_frame.inc`. Original movement code is invoked only with `RLOFT_RECOMP_EXPERIMENTAL_1FC14_MOVEMENT_SOURCE=1`. Its secondary `Application65Boundary` now references actual `2CC88`; no fabricated empty-object pass is returned. Actual original records and metadata in memory determine collision. The `0x2ACDC` classification and event bit operations call original reconstructed functions.

**Bounded source regression** `canonical_code2_original_2e120_2dbac_camera_source` covers:

- Source-generated genuine camera X/Y changes and no change on a blocked map cell.
- Collision with an explicit synthetic secondary object in `2CC88`, followed by permitted movement when the object moves away; the source, not the host, changes the camera.
- Original `2CC88` special type-`0x54` portal callback and the original level ID `0x13` recorded under its appropriate previous-level state.
- Source `20C0C` tile activation and **one** `20A24` SFX dispatch rather than double activation.
- Original `1EA78` consecutive callback/page flips.
- Five consecutive original displacement ticks and positive/negative grid-mode heading transitions.

Synthetic fixtures are strictly limited to CTest memory; **not retail DOS**.

## Sixteen authenticated source keyboard reentries

The host keyboard mapping now adds `J` → original scan `0x147`, `L` → original scan `0x149`, distinct from `A/D` lateral movement. Both source choices update the turn accumulator `C002` in original `1FC14`. Forward movement uses original `W=0x148`.

Windows preserved-media probe [run #40227035410](https://github.com/Hyperbog/RLOFT/actions/runs/40227035410) **PASS** on 16 host-supplied terminal lines to the retained source game:
- **16** original `30B40` keyboard polls returned
- **9** original camera displacements
- **7** actual heading rotations
- Original 16.16 coordinates and heading came from native reconstructed source, not a host edit
- No claim for DOS physical IRQ9 key-down/hold/repeat/timing

The separate 16-event sustained camera + heading probe was promoted to a **required, non-`continue-on-error` Windows CI gate**. The three-key movement gate additionally requires the original `2CC88` callback registration logs; **that registration is different from having encountered an actual collision with a retail object**.

## Bounded event behavior, not invented completeness

Cross-level original `181B4` world loads require independently prepared preserved media and their loader callbacks; the retained original-media adapter supports the present Level22 initialization and **fails closed on other target levels**. The original `002A8` arbitrary script dispatch, `0E0F8` random actor respawn, direct `28164` audio, and certain game-over and transition paths remain unconnected. Those callbacks stop execution rather than continue after an invented result.

Do NOT mark all scripted object interactions, all-level transitions, battle collisions, or gameplay as fully certified. The source movement and ordinary secondary object collision graph is connected; the complete event universe is not.

## Original game SFX queue to native sound device

A distinct opt-in flag `RLOFT_RECOMP_EXPERIMENTAL_SOURCE_SFX_WINMM=1` permits a **host-backed WinMM adaptation** when, and ONLY when, the original guest already has `EE40!=0` (original digital sound enabled), `EE70!=0` (unmuted), and a pending original `28110` request in `EE48/EE54`.

Path, still inside compiled source `28168 → 27F24`:

1. Preflight selects the original request by **signed priority**; equal priorities select slot 1, matching original `28168`. Guest queue slots are not altered here.
2. Resolve original resource via compiled `2F29C` using sound bank `EE44` and original archive directory `F0B8`. Reject absent/out-of-range catalogue records, invalid pointer, unbounded/zero LE32 packet length, guest copy-destination `EE60` overflow and source/destination overlaps.
3. Decode the exact source packet `LE32 size || Creative VOC Type1 unsigned PCM8`. Compressed/unsupported samples fail closed. Preflight **real** WinMM `waveOutOpen` and `waveOutPrepareHeader` **before** the original `28168` would consume any queue entry. Missing device or codec → preserve the queued SFX.
4. Let compiled original `function_28168` select the request, call compiled original `function_27f24`, copy the original source sample into the original guest destination, and invoke the source callback order `36803 → (36817 when playing) → 367F9 → 367EF → 3680D`.
5. Adapter callbacks verify exact source device/DS selector, original copied sample bytes and format; `3680D` submits actual `waveOutWrite` to the already prepared native Windows device.
6. Adapter status reports `2` only while a **real submitted WinMM** header is pending. `waveOutReset` stops the previous active sample. Real `WHDR_DONE` on a subsequent source tick is required to increment host completion receipts; neither request selection nor `waveOutWrite` is called successful original AIL completion.

This is an **AIL-shaped WinMM emulation**, **not a reconstructed Miles/AIL32 runtime or Sound Blaster**. Original AIL32 device installation, native 367F9/367EF/3680D hardware callback equivalence, IRQ/DMA scheduling and native AIL completion **remain unverified**. No live game-triggered audible SFX is attested by Windows CI, which runs headless and does not initialize source sound driver to claim active audio.

Implementation: `Recomp/src/original_source_winmm_bridge.hpp`, `Recomp/src/original_code2_live_full_frame.inc`, `Recomp/src/code2_startup_gate.cpp`, Windows `winmm` linking for all four original-source host targets. Source PCM packet parser: `Recomp/src/original_code2_pcm_packet.hpp`. New `canonical_code2_original_28168_host_sfx_status_boundary` regression checks that an invalid packet or unprepared write never fabricates playing or completion. Previous `canonical_code2_original_28168_27f24_driver_abi` checks synthetic original dispatch order, signed priorities, stop/restart and source copying.

## Acceptance states

| Gate | Status |
|---|---|
| Reconstructed original source + Level22 keyboard → actual camera/heading | **PASS, bounded** |
| 2CC88 object and 20C0C animation/SFX event, original-source fixture | **PASS, synthetic-world source CTest** |
| 16 source keyboard entries / nine displacements / seven heading changes | **PASS, preserved-media native source run** |
| All scripts / cross-level transitions / 0E0F8 actor respawn | **OPEN; fail closed** |
| Original 28168→27F24 queue and adapter ABI / invalid status rejection | **Bounded source regression** |
| Audio actually audible from original in-game request | **NOT VERIFIED** |
| Native Miles/AIL32 completion and DOS IRQ/DMA parity | **OPEN** |
| Synchronized independent retail DOS framebuffer equality | **OPEN** |

Do not overwrite these classifications with synthetic fixture results. The new default game build retains the original guard on active digital sound and does NOT silently claim an AIL device.
