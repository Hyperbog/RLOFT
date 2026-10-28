# Part 35 — source-selected saved worlds and native Windows gameplay input

**Branch:** `Recomp`, after `113cdf07326a6de2c09aea0ce30587b0110f6129` (2026-10-27 EDT / 2026-10-28 UTC).
**Priority:** actual original CODE.1/CODE.2 runtime and interactive Windows gameplay, **not** `Decomp`, isolated emulation, a new game engine, or per-level implementations.

## Compiled original source — implemented

- `07938` restoration no longer assumes the source-selected level is 23. The owner's original `SAVE.DAT` must have one of the authentic supported lengths (9288 or 9310), and its 32-bit source level at offset `0x52` must index `0..31`. The host reads only matching `LEV%03u.DAT`, validates the saved world, installs a session-private read-only snapshot, and requires compiled `07938 → 181B4 → 1FC14 → 30B40` source ownership. It does **not** invent a destination, party, camera, or save format.
- The legacy `--load-save23` / `--smoke-save23` Level23 portal fixture remains intact. New `--load-save SLOT_DIR` and `--smoke-save SLOT_DIR` accept a source-selected level. The Windows PowerShell launcher exposes `-SavedLevelFolder` and `-SavedFirstFrameSmoke`; `-SavedLevel23Folder` continues to work.
- Live `answer_original_keyboard_event` now services source-requested `Original20440MousePointer` / INT33 function 3 synchronously on the **same** original guest worker, bounded to 256 reads per source frame. The sample contains coherent physical Win32 mouse X (DOS `0..638`), Y (`0..399`) and two button bits. Original mouse service and source event logic remain authoritative; no synthetic mouse click callback is generated.
- The native Windows player queues bounded one-shot BIOS-style keyboard commands for the original `30B40` poll while preserving held W/S/A/D/J/L and arrow-key movement. Escape now goes to the original game; use Alt+F4 to close the host. Queued input is cleared on focus loss; guest processing stays on the separate native worker.

## Exact Windows evidence

- **Strict source-selected save and portal acceptance:** [run 40610693547](https://github.com/Hyperbog/RLOFT/actions/runs/40610693547), revision `113cdf073`, **success**. Real unmodified original installation `RLOFT/SAVE01`: `level=22 restored=True frame=True exited=True code=0`. Real preserved cloud `SAVE01`: `level=23 restored=True frame=True exited=True code=0`. Original saved Level23 → Level26 → Level23 trace: `transitions=3 audited_frames=3`, completed native source loop, exit code 0.
- **Canonical Windows source-link:** [run 40610431758](https://github.com/Hyperbog/RLOFT/actions/runs/40610431758), revision `3a5a31b47`, **107/107 CTests pass**. The workflow file changed after this revision; no executable source changed.
- **Windows player package:** [Build #108](https://github.com/Hyperbog/RLOFT/actions/runs/40610431778) at `3a5a31b47`, **success**, artifact `RLOFT-Recomp-Player-Windows-108`.
- **Linux regression:** [run 40610431682](https://github.com/Hyperbog/RLOFT/actions/runs/40610431682) at `3a5a31b47`, **success**.
- **Windows comprehensive milestone:** [run 40610431770](https://github.com/Hyperbog/RLOFT/actions/runs/40610431770) at `3a5a31b47`. Check its live conclusion separately; do not infer it from other jobs.

## Acceptance limits and next source work

The `ACCEPTANCE_GATES.json` ledger remains **R01–R03 accepted, 3/12**. This is a coarse engineering acceptance count, not a 25% game-completion estimate. The Level22/Level23 saved-restoration tests prove **two original saved states**, not all 32 level presentations or user save-menu roundtrips.

Physical Windows mouse coordinates and buttons now reach genuine INT33 *polls* when source asks; original DOS IRQ9, INT33 callback event delivery and completed mouse-driven interaction are not certified. The new queued BIOS scan codes have compiled and passed existing regression, but individual retail menu/inventory/combat/dialogue commands require actual original-source behavior verification. The original CODE.1 character creation, active Miles AIL music/driver, complete combat/spells/inventory/dialogue/scripts, whole-party game over, level breadth, source timer cadence and synchronized retail DOS framebuffer/audio parity remain open.

**Next implementation priority:** continuously playable, owner-tested Windows gameplay with actual source movement, button actions, actors, persistent audio and normal transitions, followed by new generalized original-source callback closures and independent retail DOS comparison. Keep diagnostic `0xFFF*` commands in strict CI only and original files read-only.


## Unmasked unsaved-start portal renderer fix — 2026-10-28 UTC

The previously nonblocking Windows milestone source-only `--smoke-portal` check stopped after an otherwise complete final Level23 scene with `ORIGINAL_1FC14_SCENE_SOURCE_RASTER_INCOMPLETE`. Original `34DE8` produced **two real quads**, but original clipping emitted **zero visible raster scanlines**. The guest had completed actor, wall, sprite, compass and polygon traversal. The old host assertion `polygon_quads > 0 ⇒ drawn_scanlines > 0` was false for valid original-source frames; it is not a game-level invariant.

[Fix `31cc1b9`](https://github.com/Hyperbog/RLOFT/commit/31cc1b9ec885fe0a3fef47f233f907fa8e5b2246) tracks `polygon_quad_returns` only **after** the genuine reconstructed `33E6C` returns and its caller-owned scratch bytes are restored. Completion now requires `polygon_quad_returns == polygon_quads` and `drawn_scanlines <= scanlines`, plus the existing real source scene/actor/wall/compass/sprite-return checks. It does not draw missing pixels, change geometry, or manufacture a successful original callback.

In [Windows milestone #722](https://github.com/Hyperbog/RLOFT/actions/runs/40611218059), the exact independent step **“Probe genuine original CODE2 23-26-23 portal and resumed 1FC14 VGA”** completed with **success**, after previously failing on [milestone #721](https://github.com/Hyperbog/RLOFT/actions/runs/40610431770). Canonical source-link, Linux, Windows Build #109 and the strict two-save/native-portal validation at revision `31cc1b9` passed independently. The broader milestone #722 aggregate result should be read from its final GitHub run, not assumed from this individual step.

[Gate-hardening commit `34774d58`](https://github.com/Hyperbog/RLOFT/commit/34774d58f209317e31d1b2b395d5a9e96e48415e) removes `continue-on-error` from **this specific native unsaved-start portal step**, making future regressions fail the comprehensive milestone. This narrows the remaining gap between CI success and actual original-game execution; it still cannot certify full Windows GUI playability, combat, user-driven portal traversal or retail DOS synchronized parity.

## Source physical-key and cursor closure — Windows Build #118 (2026-10-28 UTC)

### Verified source behavior and implementation

- Original CODE.2 `function_30b40` reads the IRQ9 scan state `F572` and applies original `F590[scan]` as a 32-bit keyboard mapping before `1FC14` interprets commands. The earlier Windows input queue emitted BIOS `AH=scan/AL=ASCII` pairs, which were **not** the value 1FC14 expected. The native host now passes a private 0xE0ss make-scan envelope (not a guest DOS key), and the retained guest executes compiled `function_30b40` with the authentic F590 table and original `2F868` callback. Original IRQ9 hardware and modifier parity are **not** claimed.
- A new `--smoke-key-map` native real-media probe performs R, F1 and Esc source reads in sequence, checking the actual guest `F590` table against the completed `1FC14` source-key result and requiring a fresh 320×400 original-VGA scene after each input. This is accepted only when the native process exits successfully and prints `RLOFT_30B40_F590_PHYSICAL_KEY_SOURCE_PASS`.
- Original `1A288 → 2AE30` cursor hit classification no longer stops simply because source returned hover category 1 or 2. The retained owner preflights the authentic two-page, 400-record-per-page, 22-byte hit table, bounds-checks referenced type records, invokes compiled original `2AE30`, and connects original `1C1DC` quest-bit checks. This preserves source-owned hover logic; a hover is **not** a fabricated click or completed inventory action.
- Original `1A288` can request both cursor backing image `0x880D` and the held-item RES1 image staged at `0x8603`. The existing source VGA adapter now admits either authenticated guest buffer with actual sprite frame-count bounds in original `29804 → 2A6B0/2A380`, instead of unconditionally rejecting held-item drawings. An owner-tested physical held-item interaction has **not** been demonstrated.

### Real-media Windows acceptance at current source revision

- [Windows Player #118](https://github.com/Hyperbog/RLOFT/actions/runs/40613081477) on source [`0986e368d`](https://github.com/Hyperbog/RLOFT/commit/0986e368d272912a785c8f9347a102fffe713b57) **SUCCESS**. This packaging workflow now requires **five** actual native original-media checks before uploading `RLOFT-Recomp-Player-Windows-118`: Level22 movement, F590 keyboard map, original saved Level22 frame, original saved Level23 frame, and full native Level23→26→23 unsaved-start portal. Every source process exited **0** and all required markers were found. The package contains no original media.
- [Strict original 07938 retained Level23/portal #98](https://github.com/Hyperbog/RLOFT/actions/runs/40613081215) at same source SHA: **SUCCESS**, including 07938 saved Level22/23 acceptance and 23→26→23 restored portal.
- [Canonical CODE.1 + CODE.2 source link #1078](https://github.com/Hyperbog/RLOFT/actions/runs/40613081274) at same source SHA: **SUCCESS, 107/107 CTests**.
- The full [Windows milestone #732](https://github.com/Hyperbog/RLOFT/actions/runs/40613081311) and Linux portable C++ regression #787 were underway when this checkpoint was recorded. Their final result must be read separately. Passing the five player probes alone does **not** certify normal game menus, mouse callback events, whole-game completion or retail DOS frame parity.

### Open engine-wide correctness boundaries

Original physical IRQ9 make/break/modifier timing and INT33 callback event delivery; original `2F868` modal prompts; ordinary `1A288` click actions and user-driven inventory; near-actor/projectile/record-removal `0D550/1BDAC`; unsatisfied `002A8` script/dialogue callbacks; actual game-over and CODE.1 character-creation lifecycles; real AIL audio; uninterrupted game-long world progress; and synchronized original DOS VGA/audio comparisons. These remain source-backed implementation objectives rather than per-level special cases. The authoritative R01–R12 ledger remains **3/12 accepted**, not a whole-game playability percentage.
