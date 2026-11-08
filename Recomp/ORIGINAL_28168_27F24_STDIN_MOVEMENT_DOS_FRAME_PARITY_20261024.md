# Part 29 — Original CODE.2 28168/27F24, host stdin guest motion and DOS-frame oracle (2026-10-24)

**Branch:** `Recomp`. Preserve all proprietary original files, VGA indexed frames, palettes, and DOSBox-X screenshots/savestates outside Git and published CI artifacts. This is a bounded source/regression checkpoint, **not** full DOS gameplay, sound-device, or screenshot parity.

## Independent DOS Level 22 indexed-frame comparison: still OPEN

An independently synchronized retail DOS `320x400` indexed source and its matching keyboard/camera/tick receipt **were not provided**. The supplied original game binaries and DAT files do not substitute for an emulator screenshot or DOSBox-X VGA saved state. No legitimate DOS↔recompilation indexed comparison was conducted or claimed.

Previously implemented:
- `Recomp/tools/check_dosboxx_2026_10_frame_pair.py` accepts the exact versioned DOSBox-X 2026.10.01 SDL2 VGA state and independently exported screenshot **only if all 128,000 displayed RGB pixels agree**; otherwise the private DOS `.idx/.pal` export fails closed.
- `Recomp/tools/verify_synchronized_dos_frame_pair.py` requires both raw frames + RGB6 banks and independent receipts matching level 22, source frame boundary, 16.16 camera coordinates, heading, CRTC start, source tick, original keyboard-code trace hash, file digests, and the DOSBox-X screenshot-state attestation. It reports exact differences; it never independently claims DOS execution.
- `RLOFT_RECOMP_PRIVATE_1FC14_FRAME_DIR` enables source-side private five-tick `320x400` original CODE.2 VGA scanout and `256xRGB6` receipts after verified `1FC14` scene/1A510/HUD/audio return. [Windows #40074872545](https://github.com/Hyperbog/RLOFT/actions/runs/40074872545) passed five capture SHA checks and deleted all raw owner-media-derived buffers before artifacts were published.
- **No retail DOS pair accepted.** The one-shot `1F8FC` presentation remains separately counted from five `1FC14` source-loop cycles.

Real source-side capture and format verification cannot be passed off as independent original-DOS pixel parity. The remaining external evidence needed is a retail DOSBox-X screenshot, the **synchronous** VGA saved-state from that exact screen instant, and an independent debugger observation of original source CPU/frame/camera/keyboard timing. Do not fabricate that receipt from reconstructed output.

## Original sampled audio: compiled real source graph, explicit device boundary

Canonical original machine-code and C++ audits prove:
- `28110`: two 12-byte priority queue entries (`EE48/EE54`); signed priority replacement, controlled by `EE40`.
- `28168`: if enabled, chooses the lower **signed** priority across both requests; ties choose slot 1. Calls original `27F24` and clears pending entries after the call.
- `27F24`: requires nonzero `EE40` and `EE70`, locates source `2F29C` audio packet, reads original length prefix, stops a playing audio sample via `36803/36817` if needed, copies the actual source bytes to `EE60` destination, and invokes `367F9`, `367EF`, and `3680D` device callbacks in order. The callbacks require an **actual** Miles/AIL adapter for real sound output.

Current `Recomp/src/original_code2_live_full_frame.inc` connects the actual post-HUD `1FC14` `28168` call to the compiled source routine on guarded disabled/empty-queue branches. On any **active device + unmuted pending request**, it records the source arguments and fails closed **before consuming the original queue**, because there is no authenticated game-usable AIL driver. No fake hardware-status response is supplied.

[Windows original-media #40076994179](https://github.com/Hyperbog/RLOFT/actions/runs/40076994179) **PASSED** the mandatory five post-HUD `28168` source-return gate. Its actual live records had `enabled=0`, `audible=1` (game's enable byte only), and both queues empty (`FFFFFFFF`) on all five iterations: **not an active audio playback claim**.

The isolated media-free CTest `canonical_code2_original_28168_27f24_driver_abi` compiles and executes **original reconstructed** `28168 → 27F24` with an explicit synthetic eight-byte payload and clearly marked synthetic device-status/address/rate/start callbacks. [Commit bd9ebfc11600](https://github.com/Hyperbog/RLOFT/commit/bd9ebfc11600ac956d4620c4b4a21549e6025ff0) expanded the test to cover a playing device stopped before replacement, both simultaneous queue requests, signed negative priorities, and equal-priority tie. [Canonical source #40077039665](https://github.com/Hyperbog/RLOFT/actions/runs/40077039665) and [Linux #40077039697](https://github.com/Hyperbog/RLOFT/actions/runs/40077039697) **PASSED**. This tests the game-facing ABI **without** Miles hardware, audible playback, driver timing, sample completion, or original VOC-format provenance.

## Real host input into retained original 30B40: guest motion VERIFIED, position NOT

`rloft_code1_code2_staged_boot --root <original> --cd-root <original-cd> --console-input` enters the already authenticated resource+compositor original guest, then reads the human host terminal stream and forwards original arrow-key scan words to the **suspended original 30B40 source callback**. It observes the same guest `255D0/255D4` camera coordinates, `C658` heading, and `C000/C004` motion accumulators on both sides of each source keyboard poll. No host code assigns camera or velocity deltas.

[Windows original-media #40076790915](https://github.com/Hyperbog/RLOFT/actions/runs/40076790915) **PASSED** a CI **scripted stdin test**, which is a host terminal stream, **not** human-held IRQ9 hardware:

| Terminal | Source scan word | Guest evidence |
| --- | --- | --- |
| `s` | 336 (`0x150`) | speed `0 → 10`, turning `0 → 0` |
| `d` | 333 (`0x14D`) | speed `10 → 0`, turning `0 → 10` |
| `w` | 328 (`0x148`) | speed `0 → 65526` (signed `-10`), turning `10 → 0` |

All three returned to next original `30B40` poll on the same source guest; three of three changed genuine original motion state. **Camera X/Y stayed 294912/294912, and heading stayed 270 throughout**. Therefore **actual camera displacement is not certified**. The source scan codes and accumulator sign conventions are original binary behavior; the terminal bridge itself is host input translation, not hardware IRQ9, autorepeat, mouse or playable DOS-equivalent controls.

The original `20B40` logical routine at the retained source loop contains event-grid updates; it is **not itself proof of camera movement**. Do not mislabel velocity/turning accumulator updates as world-coordinate changes. More source execution/timer transitions may be needed for visible motion.

## Ledger and next executable frontier

| Claim | State |
| --- | --- |
| Five original `1FC14` source scene returns + five `1A510` handoffs | ACCEPTED |
| Five post-scene source-loop reentries, including original `28168` disabled-branch service | ACCEPTED |
| Five native-source private indexed frame captures, SHA receipts | ACCEPTED, RAW PRIVATE, DOS PARITY OPEN |
| Independent DOS-vs-recomp index/RGB6 equality at same camera/tick/CRTC | **OPEN: NO DOS INPUT** |
| Compiled source `28168 → 27F24` callback ABI with synthetic device | ACCEPTED AS SOURCE REGRESSION ONLY |
| Live Miles/AIL device output, scheduling, completion | **OPEN** |
| Three scripted terminal events cause original CODE.2 speed/turn state changes | ACCEPTED |
| Three host inputs cause real camera displacement | **NOT OBSERVED** |
| Physical IRQ9 and key-repeat parity | **OPEN** |
| Whole-game recompilation milestones R07/R08/R09/R12 | **NOT PROMOTED** |

Next: obtain genuinely synchronized retail DOS screenshot/save-state/debugger state, run private exact comparator without propagating original media, then certify positive camera movement with a longer source-driven movement sequence or diagnose the original movement integration delay. Implement a real sound-device backend only with proved `367F9/367EF/3680D` status and timing behavior, not synthetic answers.
