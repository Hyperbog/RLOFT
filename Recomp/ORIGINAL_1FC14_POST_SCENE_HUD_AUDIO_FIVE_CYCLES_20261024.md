# Part 29 — Five original 1FC14 post-scene HUD/audio cycles (2026-10-24)

**Branch:** `Recomp`. **Source of truth:** preserved original `CODE.2` (LE object logical offsets), compiled reconstructed CODE.2, and Windows original-media validation. This is a *bounded native-source execution* certificate, not original DOS indexed-pixel equivalence, playable keyboard acceptance, or full sound-driver parity. Original assets remain uncommitted.

## Original executable control-flow evidence

An independent disassembly of the uploaded original `CODE.2` proves the following strict straight-line call order inside `1FC14`, after the input/movement branches join:

| Source site | Original operation |
| --- | --- |
| `201C0` | call `20B40` (world update) |
| `201C5` | call `1F1C8` (scene) |
| `201D0` | call `1A510` (CRTC/page presentation handoff) |
| `201E1` | call `1F494` (party HUD) |
| `201E9` | call `28168` (subsystem update) |
| `201EE` | call `1F120` (audio/state) |
| `201F7` | call `2E614` (post-audio routine) |
| `2021C..20268` | additional event/game-state operations |
| `20268` | call `1F86C`; eventual branch to next `30B40` input poll |

Within original `1F120`, the first source call at `1F121` is `2B1BC`, followed by conditional audio-mode/volume/tempo updates and a `30CA2` random call at `1F1AA` before the routine returns. Only actual executed branches are counted; callbacks not reached remain unverified.

## New trace instrumentation

[3d7079c0](https://github.com/Hyperbog/RLOFT/commit/3d7079c0a6617d8ecfe492a3213b1d73fbbbecf9) adds per-source-tick counters to the existing guest-thread-owned `OriginalFullFrameTrace`, distinguishing:

1. successful original `1A510` return after the source-scene and VGA-page check;
2. actual `1F120 -> 2B1BC` actor-count callback *after* the handoff, which proves that the preceding `1F494` and `28168` direct calls returned;
3. actual post-query `30CA2` callback, which occurs near the tail of original `1F120`;
4. the *next* original `30B40` keyboard-loop entry, which proves the preceding post-audio work returned to the source loop.

[85ae9a7c](https://github.com/Hyperbog/RLOFT/commit/85ae9a7c8ad3363d4ebf4bd108658a0348f5f725) attaches the checkpoint to the retained `1FC14` guest and counts complete post-scene source cycles separately from scene returns, page handoffs and `1F8FC` compositor presentations. A private one-shot compositor handshake cannot become an extra retained source-tick counter. Trace presence and source order are checked; counters are not used to invent guest behavior.

[87427120](https://github.com/Hyperbog/RLOFT/commit/8742712025ad89636291d6f5bacda48a6f6c81f4) prints the diagnostic in Windows original-media CI. At [Windows #40071992518](https://github.com/Hyperbog/RLOFT/actions/runs/40071992518), five independent ordered trace records each contained:

```text
observed_page_return=1
hud_return_before_audio=1
audio_actor_queries=1
audio_random_callbacks=1
keyboard_loop_reentry=SOURCE
acceptance=BOUNDED_POST_SCENE
```

The records had `scenes=1..5` and `pages=1..5` consecutively. [Windows #40072166729](https://github.com/Hyperbog/RLOFT/actions/runs/40072166729) **PASSED** after [5411c98b](https://github.com/Hyperbog/RLOFT/commit/5411c98b515babd1c851c610e2fc5d7dfafdbc9f) upgraded these observations to a required regression gate:

```text
ORIGINAL_1FC14_POST_SCENE_SOURCE_LOOP_GATE_PASS
scenes=5 page_handoffs=5 source_loop_cycles=5
source_hud_audio=BOUNDED_COMPLETE
1F8FC_presentations=SEPARATE indexed_dos_parity=OPEN
```

On the exact preceding code commit [85ae9a7c](https://github.com/Hyperbog/RLOFT/commit/85ae9a7c8ad3363d4ebf4bd108658a0348f5f725), [Linux #40071970541](https://github.com/Hyperbog/RLOFT/actions/runs/40071970541) and [canonical source-link #40071970539](https://github.com/Hyperbog/RLOFT/actions/runs/40071970539) both passed. Workflow-only changes after this commit did not modify native C++ source.

## Separate, non-interchangeable acceptance counts

| Counter | Accepted | Meaning |
| --- | ---: | --- |
| Original retained `1FC14` source scene returns | 5 | World polygon/RES3/compass source rendering returned |
| Original retained `1A510` page handoffs | 5 | Original page selection and cursor call returned |
| Original retained `1FC14` post-scene loop reentries | **5** | HUD/28168 and bounded audio branch executed and input poll re-entered |
| Independent full one-shot `1F8FC` compositor presentations | **1** | Separate source test; **not** counted as five `1FC14` full `1F8FC` presentations |
| Independently matched DOS Level 22 indexed framebuffer pairs | **0** | Retail VGA indexed parity still unverified |
| Physical-input / complete sound-device parity cases | **0 certified** | No claim of audible output or physical keyboard control |

The `1F120` callback trace proves bounded executed audio **logic**, not audible output, full `1EF7C` rare-sound handling, or timing equivalence. Reaching the subsequent input poll is source-loop completion, not a claim that all optional game-state/event branches were taken. The existing original-DOS RGB6 DAC palette parity evidence is separate from absent indexed-pixel comparison.

## Next fidelity frontier

Acquire an independently synchronized retail DOS Level 22 `320×400` indexed framebuffer from a matching save/camera/tick/CRTC page. Compare it with the original-source VGA guest before certifying pixel parity. Exercise physical input and active audio-driver paths separately, and retain fail-closed source branches for rare sounds, dialogue, and nearby `0D550` AI. Do not mark coarse milestones R08/R09/R12 accepted merely on these bounded source-loop receipts.
