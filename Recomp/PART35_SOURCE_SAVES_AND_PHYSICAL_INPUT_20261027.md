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
