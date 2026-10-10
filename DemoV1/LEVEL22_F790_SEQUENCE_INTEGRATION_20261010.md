# Level22 F790 modal sequence integration checkpoint (2026-10-10)

**Git branch:** `demo-v1`. **Live status:** verified source-only integration component; NOT connected to `win32_main.cpp` or `PartyModalPresenter::draw`, so not a field fix.

## Original immutable proof

Authoritative original `CODE.2` has 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical base `0x3AF24`. This batch rechecked the disassembly of `112EC` (`112EC..11351`) and `13118` (`13118..131C8`) against these original bytes. Exactly one original direct E8-rel32 call reaches `112EC`, at `13133`. The existing original-binary regressions independently cover ten background save/restore windows, nine event/RNG windows and all 17 direct `F790` callsites. Original data and detailed byte proofs are in `LEVEL22_F790_BACKGROUND_2932C_20261010.md`, `LEVEL22_F790_SHARED_RNG_AND_REPLAY_20261010.md`, and `LEVEL22_CODE2_F790_EVENT_CADENCE_20261010.md`.

The verified native sequence is: original modal `13118` resets feedback; `112EC` renders four columns, captures the **only** clean ribbon backing image by `292BC` at row `0x62` (320×16), then invokes `F790`. Every actual `F790` invocation restores that original saved region by `2932C`, before resolving `6DAC` feedback or item input. A pending feedback message advances the one shared `30CA2` state on interior tiles; ordinary nonempty items seed the shared state with held ID before the `F66C` draw, then reseed it from `30700` AFTER the draw; empty hand skips ribbon/RNG. Original `F66C` uses `RES2:3` frames and original font one at y=`0x65`.

## Composed native implementation (isolated)

New `OriginalF790ModalSequence` combines **the existing audited building blocks**, not a second renderer or RNG. `enter_modal_13118` clears stale page, glyph message and cached tile frames; `capture_after_actor_columns_112ec` saves the 320×16 indexed original rectangle **exactly once** and now refuses a second capture; `source_event_f790` restores the same snapshot on a real original event, passes through the caller's **existing** `OriginalRandom&` and tick observer to the source-verified tile selector, and rasterizes the result. `replay_after_host_redraw` only draws preselected frames/text on newly reconstructed native framebuffer pages and never samples `30700`, mutates RNG or recaptures the original background; `leave_modal` disposes of both page and message.

**Validation executed here:** C++17 `-Wall -Wextra -Werror -pedantic` test passed with the actual prior source ribbon/RNG headers and `original_random.cpp`, plus a test-only native-shaped indexed framebuffer substitute. It asserted rejection of F790 before capture, rejection of duplicate capture, empty-hand restore, pending F66C pixels and signed text centering, ordinary held-item post-draw IRQ reseed, 1,000 host-frame replays with no extra RNG, and safe exit/re-entry. The Git CMake test is registered and links to the actual `rloft_boot_flow` library but has **not** been executed on Windows in this batch.

## Explicit next gates

Do NOT call `source_event_f790` during the host's 70-Hz presentation loop. Live binding requires mapping source F790's 17 original argument-bearing callsites to each native click/state transition; obtaining original `759C` feedback and item-description text, including known/variant state; sharing `ExplorationRuntime::original_random_` without a second UI generator; preserving one-shot original page capture; replaying unchanged authentic `RES2[3]` and original `RES0` font one; and original DOS indexed pixel, IRQ and event-cadence comparison. The current provisional flat held-item strip remains live until then.

**Counts:** 190 registered CTests with Python; last actually compiled Windows Full #651 (182/182 passing at older source); current source unbuilt. Original-DOS acceptance, engineering readiness and Level22 completion percentages unchanged. No manual Windows build dispatched and no independent Level23 gameplay.
