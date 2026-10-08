# Original CODE.2 E5F0 attack target and recovery timing

**State:** original executable byte audit verified; read-only native source models and deterministic regression targets committed. **These source changes have NOT been compiled or executed in the Windows CTest suite.** No live attack, no RNG/damage integration, no new EXE and no GitHub Actions runs (manual-only while private).

## Reconstructed original entry ordering

The original `26018` hand dispatch calls `E5F0(actor, hand, item_id, target)` for item categories `9, 10, 11`. This attack orchestrator has essential observable ordering and time-state behavior independent of the already reconstructed `C80C` hit arithmetic:

1. **`E5F0:E5FB..E614`:** if target argument is exactly `-1`, call `DB94(actor)` to find a preferred projected target; if caller supplied any nonnegative target, preserve it and **do not call DB94**. `DB94` selects group `actor & 1` first and scans the opposite group only if the first returned `-1`. The existing `DACC` ranked projected-NPC selector is the underlying visibility scan.
2. **`E618`:** calls original CODE.2 logical **`E578(actor)`** (objdump virtual `1E578`) *before* target/hit evaluation. The bounded E578 mutation and CF5A redraw flags now have their own source model and tests; real action orchestration must run it in this exact order.
3. **`E621..E669`:** if global party mode `0x258A8 == 0xFF`, a target was found, and `actor & 2 != 0` (actors 2/3), clear the selected target to `-1` when `item_id == -1` or `OBJECTS[item].WORD(+0x35) & 3 != 2`. Importantly this is **target suppression, not input-event rejection**. Actors 0/1 and qualified item code 2 retain their target. Original `E5F0` can still execute its later no-target animation/feedback path.
4. **`E671..E6C3`:** only when a target remains, resolve original projected NPC data and call `C80C` for accuracy. The original hit result is separately checked before post-hit damage resolution; no target means no genuine hit. Existing `CombatHitCheckModel` covers only that arithmetic primitive, not the complete original `E5F0` side effects.
5. **`E6C9..E9D0`:** on a hit, original weapon spell/ability damage and object-specific cases feed subsequent `C910` gating and NPC injury calls. The bounded [damage dice/stat bonus and C910 source checkpoint](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md) now models these dice/gating arguments, **not** RNG consumption, live HARM/Rod effects, `E284` NPC injury, or complete action ordering.
6. **`E9D0..EA0D`:** compute an authored action animation interval from item ID and the full original 16-bit OBJECTS word at offset `+0x35`; see exact timing below.
7. **`EA15..EA2E`:** optionally halve the interval based on actor effects at `character+0x132` (global `0x21A12`), independent of monitor refresh.
8. **`EA2E..EA5C`:** only for the original nonzero hit result, read pauseable maintenance clock `A7E4` via `29CC4`, set the selected hand's ready deadline at `character+0x19E + 4*hand` to `A7E4 + floor(interval/2)` (32-bit arithmetic), and write the damage result at `character+0x1AA + 4*hand`. A miss/no-target path **does not perform either write**.
9. **`EA63..EA75`:** call original CODE.2 logical **`24B48`** (objdump virtual `34B48`) to present the hand action, then return the hit result. `24B48:24BC9..24BEC` checks **whether the interval is nonzero**. If nonzero, **after presentation**, it reads `A7E4` again through `29CC4` and writes the OTHER selected-hand action-state dword at `character+0x10B + 4*hand` to **that later A7E4 observation + full interval**. **This action-busy write happens even when C80C misses or no NPC target exists**, unlike the hit-only ready/damage writes. If interval is zero, this second deadline is not written. The action interval and both clock snapshots must be preserved separately.

## Exact verified interval arithmetic

At `E9D0`, a hand item of `-1` or a temporary spell-hand `0x7E..0x86` receives an action interval of **18 game-maintenance ticks**. Other valid item IDs read the **complete little-endian WORD at original OBJECTS record `+0x35`**. The x86 sequence `MOV AX,[+35]; SHL EAX,10; SHR AX,12; MOVZX EAX,AX; LEA 8*EAX; ADD EDI,EAX` produces:

`interval = 9 * ((OBJECTS_WORD_35 >> 2) & 0x0F)`

Then `DWORD(actor+0x132) & 0x01000010` **halves** the interval, with integer truncation toward zero. This is not the low two qualifier bits used by `240A8/24478`; the relevant interval bits are **2..5**. The parser now retains both `raw_word_35` and its existing `raw_byte_35_low2`.

| Actual original object | WORD +0x35 | Initial action interval |
| --- | ---: | ---: |
| SPEAR `0x2E` | `0x0019` | 54 original maintenance ticks |
| THROWING KNIFE `0x2F` | `0x0008` | 18 ticks |
| COMPOSITE BOW `0x30` | `0x001E` | 63 ticks |
| SLING `0x31` | `0x0018` | 54 ticks |
| DAGGER OF THROWING `0x59` | `0x0008` | 18 ticks |
| No item or temporary spell-hand `0x7E..0x86` | Special code path | 18 ticks |

**Clock fidelity:** `29CC4` returns DWORD `A7E4`, which `29C80:29CBC` increments only when its enabling flag `A7E8` is active. This is **not** an assumed milliseconds value, not the independent `F58C` IRQ count, and not the native Win32 `70.0863`-Hz display cadence. Use actual original tick evolution and input/order traces to calibrate timing; do not convert these counts into arbitrary fixed wall time.

The read-only `CombatAttackTimingModel::inspect` consumes the **captured item ID passed by `26018` to E5F0**, not a new hand-slot read, and **two separately observed original gameplay-clock values**. It returns the authored interval, optional **hit-only** `+0x19E` ready deadline, optional **hit-only** `+0x1AA` damage result, and the independently optional **interval-nonzero, miss-inclusive** `+0x10B` action-busy deadline after presentation. It does not mutate game state, apply clocks or present frames. Tests cover hit-vs-miss disagreement, split clock observations, zero-duration omission, odd halving, deadline rollover, and nonmutation.

## The two original hand deadlines are not interchangeable

| Source branch | Authoritative clock observation | Original actor field | Write condition |
| --- | --- | --- | --- |
| `E5F0:EA35..EA5C` after successful hit calculation | First read of pauseable `29CC4 → A7E4` | `+0x19E + hand*4` | **Hit only:** first clock + `floor(interval/2)`; also store post-damage at `+0x1AA + hand*4` |
| `24B48:24BD0..24BE5` after presenting the action graphics | **Second**, potentially later `29CC4 → A7E4` read | `+0x10B + hand*4` | **Any outcome with interval != 0:** second clock + **full interval**, including misses/no target |

The earlier `24478/26018` input gate reads the `+0x10B` action state, **not** the hit-only `+0x19E` ready field. The item swap at `214B8:215E9` clears the latter. Neither should be mapped to a fixed Windows frame count. This distinction was verified independently against **nine** additional exact original x86 instructions at logical `24B48` and is a correction to earlier documentation that described misses as having no deadline whatsoever.

## Separate target-selection model

`CombatAttackTargetModel::choose` now preserves original explicit-vs-`DB94` selection, preferred group by actor parity, fallback scanning, and the special rear-party `+0x35 & 3 == 2` target restriction. Its tests cover actor 0..3, preserved explicit target/no scan, search ordering, exact mode `0xFF`, item `-1`/ordinary/qualified, no target, input bounds and absent target.

This model does **not** check whether a caller-provided explicit NPC index maps to an existing live projected record; that remains the original combat state layer's responsibility. The model reports whether original rear-party rules suppressed a target, rather than falsely interpreting target suppression as a rejected user click.

## Original E578 actor-status service — before hit resolution

At `E5F0:E618` the executable calls logical **`E578`** (not `1E578`; the latter is an adjusted disassembler address). The source-level `CombatAttackStatusService::apply` now models its bounded mutation in exact original order:

1. For **all four actors**, inspect byte `actor+0x132` bit `0x20`. When set, mark that actor's original `CF5A` HUD-dirty byte `1` and clear **only** bit `0x20`.
2. For the **attacking actor alone**, inspect byte `actor+0x133` bit `0x02`. When set, clear the 32-bit field at `actor+0x15A`, clear only that `0x02` effect bit, and set the attacking actor's `CF5A` redraw byte `1`.
3. Do not change the independent `+0x19E` hit-ready or `+0x10B` action-state timing fields in this helper. Those are changed later at the two clock-dependent stages.

The code is deliberately a **separate callable state operation**, not yet connected to live attacks. Its new C++ test fixture asserts four-actor iteration, preserved unrelated effect bits, selected-actor-only cleanup, original literal redraw `1`, idempotence and invalid-actor nonmutation. **Nine additional original x86 opcode signatures matched**, bringing the offline verifier to 126. These tests have not run on MSVC yet.

## Provenance and acceptance

- The supplied immutable CODE.2 SHA-256 is `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, with object-1 file base `0x3AF24`.
- **21 additional executable instruction-byte anchors** for `E5F0`, `29CC4`, DB94 target handling, `+0x35` recovery, effect mask, hit-gated deadline/damage writes, and animation dispatch were independently checked byte-for-byte against the supplied original and passed.
- This historical target/timing stage reached **126 original machine-code anchors** (87 previous + 21 E5F0/29CC4 + 9 original 24B48 scheduler + 9 E578 service). Subsequent [damage/C910 research](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md) added **47 verified anchors**, bringing the current offline verifier to **173**. Static byte checks do not establish compiled or runtime fidelity.
- Native sources: `combat_attack_timing.hpp/.cpp`, `combat_attack_target.hpp/.cpp`; updated `ObjectCatalog` parser retains full `WORD(+0x35)`; two new C++ test programs are registered in CMake for later manual CI.
- **No new compiled-test claim**: previous 102/102 Windows pass from Actions #631 predates the full CODE.2 hand-pair, E5F0 timing and target source.

## Remaining work

1. Connect complete conditional `24478` eligibility and `26018` category dispatch to the real native hand input without losing the existing safety gates. For `-1` and mixed hands, observe the original DOS heap-adjacent bytes rather than invent values.
2. Bounded **logical `E578`** pre-hit cleanup, original [E5F0 dice/stat calculations and C910 gate](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md) are now reconstructed separately. Connect them in the correct order with the original `30CA2` RNG sequence, Rod/HARM side effects, projected NPC state and **`E284` injury** before mutating a live NPC.
3. Reconstruct original logical **`24B48`** (objdump virtual `34B48`) frame presentation and audio/input-service ordering; the current timing plan already preserves its **second**, miss-inclusive `A7E4` action deadline, but the actual frame/UI work, cooperative ticks and pause/resume remain to be ported. Compare the two original timestamp reads separately.
4. Batch a consolidated current-HEAD C++ build/test only on explicit request or when the repository moves public.

**Bounded E5F0 target/timing and E578 service source checkpoint: 67% (4/6 gates: original instruction audit, object/timing decoding, staged source reconstruction, and regression registration complete; compiled/DOS-run verification and live orchestration remain open).**

**Overall faithful-demo end-to-end acceptance stays 20% (1/5 major gates)**. A high source-level percentage does not imply a playable Assassin encounter.
