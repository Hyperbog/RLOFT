# Part 29 — original 1FC14 private-compositor handshake and actor dependency frontier (2026-10-24)

**Branch:** `Recomp`. **Validated code commit:** [cfab7f05f9db](https://github.com/Hyperbog/RLOFT/commit/cfab7f05f9db99a5c534c9acb0233e3e691520ee). This note is a continuity checkpoint, not original-DOS indexed-pixel parity or native playable-game acceptance.

## Baseline entering this checkpoint

[Part 36 frontier](ORIGINAL_1FC14_29804_CLIP_AND_FOUR_SCENE_FRONTIER_20261024.md) accepted four original `1FC14` scene returns and four source `1A510` page handoffs, while a fifth scene stopped at `ORIGINAL_314B0_SOURCE_VIEW_OUTSIDE_ORIGINAL_TABLE`. The distinct one-shot complete `1F8FC` frame count remained one.

## Independently verified original data and ABI

- The user-supplied original `LEVELS.DAT` SHA-256 is `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`. Archive index 22 (Level 22) and the independent `LEV022.DAT` both store environment-profile byte `01` at header `+0x290` (`DGROUP 0x258A8`).
- From original `CODE.2` (SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`), `1F2B1` begins with `movzx edx, byte ptr [0x258A8]`, branches over the environment draw when the value is `FFh`, and otherwise pushes that byte-extended value to `314B0` on the active `C028` path. Original `314B0` begins by indexing the word table at `2B750` and the pointer table through `*(2B764) + 4*view`. Source addresses above use the original CODE.2 logical-offset mapping; the on-disk disassembly uses file offset = logical offset + `0x3AF24`.
- The repo's independent [INIT environment-profile research](../Resources/MISC/INIT/ENVIRONMENT_PROFILES.md) identifies only profiles 0 and 1 for the supplied retail content. The callback's older `actor-view` naming is not the semantic identity of this profile selector.

## New diagnostic and actual root cause

[6c939c33f3b8](https://github.com/Hyperbog/RLOFT/commit/6c939c33f3b860041891f94119ef3af28ceac0b4) adds `314B0_SOURCE_VIEW_BOUNDARY` to the Windows persistent scene probe's printed log filter. [Windows run 40069405712](https://github.com/Hyperbog/RLOFT/actions/runs/40069405712) passed and revealed:

```text
view=1 camera_heading=270 view_state_258A8=4294901761
source_keyboard_code=331 scene_returns=4 table_pointer=16927412
```

The guard in `original_full_exterior_actor` is `if (!exterior_trace || view >= 10u)`. With actual `view=1`, the branch necessarily arose from the missing **host trace pointer**, not view-table bounds. The apparent 32-bit state value `0xFFFF0001` includes adjacent header bytes; it is not the 8-bit argument.

The synthetic keyboard response `0xFFFF` invokes a private one-shot `1F8FC` compositor on the same guest. Its exit clears the thread-local `original_full_frame_trace` and `exterior_trace` pointers and returns a synthetic zero key, without taking the regular non-handshake keyboard branch that initializes the next persistent `1FC14` tick.

[cfab7f05f9db](https://github.com/Hyperbog/RLOFT/commit/cfab7f05f9db99a5c534c9acb0233e3e691520ee) rearms per-tick source trace state **only when the experimental persistent-scene mode is enabled**, immediately after the private `1F8FC` diagnostic and before returning the synthetic zero key. No guest input, level bytes, rendering output, original table pointer, camera state or guard was fabricated or broadened.

## Verified regression and new frontier

- [Windows original-media 40069660903](https://github.com/Hyperbog/RLOFT/actions/runs/40069660903): **PASS**.
- [Linux C++ regression 40069660909](https://github.com/Hyperbog/RLOFT/actions/runs/40069660909): **PASS**.
- [Canonical reconstructed source-link 40069660998](https://github.com/Hyperbog/RLOFT/actions/runs/40069660998): **PASS**.
- The false `314B0` view-bounds stop **does not recur**. The fifth source iteration advances into `1BDAC` and fails closed at `ORIGINAL_1BDAC_EXTERNAL_3_ARGUMENT_DEPENDENCY`. Four completed original `1FC14` scene returns remain accepted; a fifth completed scene is **not** accepted.
- Complete original `1F8FC` presented frame count remains **one**. Retail RGB6 DAC parity remains verified; independently synchronized original-DOS Level 22 VGA indexed framebuffer parity remains **OPEN**.

## Next source dependency

Determine whether the fifth tick's `1BDAC` three-argument callback is original `0D550` or `30CD4`: the current `original_full_actor_missing3` stops on both and hides the callee identity. Record its actual arguments and original source callsite, then integrate only evidence-backed behavior with bounded guest memory, without relaxing the existing fail-closed architecture. Subsequently verify post-scene HUD and sound and obtain an independently aligned retail DOS index/DAC capture.
