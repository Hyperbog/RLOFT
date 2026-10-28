# Part 36 — original IRQ9 keyboard source and native Windows acceptance (2026-10-27 EDT)

**Branch:** `Recomp`. **Latest verified executable source:** `8f673ada8423c0118ae97755d71fb137ec1995a4` (2026-10-28 UTC). **Focus:** original CODE.2 source input services, not simulated gameplay or per-level patches.

## Defect and repaired source boundary

Earlier real Win32 keyboard events were sent as BIOS INT16 ASCII/scan pairs to a retained `1FC14` callback, although original CODE.2 `function_30b40` reads its own `F590[hardware_set1_scan]` table. Part 35 repaired table translation with a private `0xE0ss` native event envelope but directly wrote `F572=scan`, bypassing the original IRQ9 handler's queue, control flags and break-event effects.

Commit [`0f74845`](https://github.com/Hyperbog/RLOFT/commit/0f74845cdfa31db7203443aef28a656c260ef145) now invokes **compiled original `code2::interrupt_308f8`** on the same retained source worker for the genuine PC set-1 *make* and *break* events. Its host callbacks implement bounded port 60h scan reads, 61h keyboard-latch reads/writes and PIC 20h EOI acknowledgments. It verifies the original `30BB4` installed vector ownership and each original `308F8` EOI, requires original `F572` to publish the make scan, and calls original `function_30b40` with original `2F868` before the source break. CODE.2, not the host, now owns `F572/F571/F588` navigation queue and `F573` modifier effects for this path.

**Scope caveat:** The host still chooses the delivery instant at the already suspended original `30B40` boundary. Physical DOS IRQ9 asynchronous arrival, actual hardware cadence/key repeat, simultaneous held movement modifiers, device vector chaining and retail DOS keyboard parity are **not** certified. Windows W/S/A/D/J/L sustained movement and directional aliases still run through the earlier host-translated source `1FC14` movement path; they must not be represented as complete physical IRQ9 support.

## New independent synthetic original-source regression

Commits [`18f7f0e`](https://github.com/Hyperbog/RLOFT/commit/18f7f0ee332b1decf11db05ce0d1645cf271d711) and [`8f673ad`](https://github.com/Hyperbog/RLOFT/commit/8f673ada8423c0118ae97755d71fb137ec1995a4) introduce `src/test_original_code2_irq9_physical_source.cpp` and register its CTest `canonical_code2_original_308f8_irq9_make_break_f590_keyboard`. Synthetic guest RAM only; it executes original compiled `30BB4` vector install, `308F8` R make/break, original `30B40` F590 translation, original 307F0/30748 held-navigation queue make/break, original CTRL `30ABC` flags and six original PIC EOI/twelve port 61h acknowledgments. No hardware or DOS parity is claimed.

[Canonical Windows source-link #1082](https://github.com/Hyperbog/RLOFT/actions/runs/40616583085) **SUCCESS** at exact executable source `8f673ad`: **108/108 CTests**, including the new source IRQ9 test #103.

## Native original-media acceptance and player package

[Windows Player Build #121](https://github.com/Hyperbog/RLOFT/actions/runs/40616583040) **SUCCESS** at exact source `8f673ad`, artifact **`RLOFT-Recomp-Player-Windows-121`**. The mandatory player-package workflow executed and required exit code 0 for all five original-media probes: retained Level22 original camera/scene; physical-key F590/308F8 original source map; saved original Level22 and Level23 full frames through 07938; and source Level23→26→23 native portal cycle. This is a bounded **engineering package**, not a certified fully playable Ravenloft or a retail DOS parity replacement.

[Original 07938 native saved/portal #100](https://github.com/Hyperbog/RLOFT/actions/runs/40616583134) **SUCCESS**; [Linux C++ regression #790](https://github.com/Hyperbog/RLOFT/actions/runs/40616583123) **SUCCESS**. [Comprehensive Windows milestone #734](https://github.com/Hyperbog/RLOFT/actions/runs/40616583058) was still running when this report was written. Do not infer its conclusion from these other workflows.

**Earlier contradictory evidence:** [Windows milestone #732](https://github.com/Hyperbog/RLOFT/actions/runs/40613081311) failed after 2 source console keyboard events, before event 3, while all prior compilation/source-link/Windows-package steps passed. This is an unclosed intermittent or state-dependent failure until the exact source path and cause are captured; merely passing a subsequent run is not a proof that it is fixed. Milestone #733 confirmed its first two console acceptance steps before being cancelled by the next source revision. Do not hide an engine failure or lower the 16-key sustained test requirements.

## Remaining highest-priority engine-wide gaps

- Asynchronous Win32-to-original INT33 000Ch mouse event delivery and verified original click-to-action paths; keyboard input on physical held keys; genuine original IRQ8/IRQ9 timing cadence.
- Original actor `1BDAC/0D550` combat, projectile record cleanup, state-changing `002A8` script branches, dialogue `1C77C`, full CODE.1 character creation and whole-party `0CBC8` game-over.
- Full DOS filesystem I/O and normal game save/load, genuine AIL audio, all 32 original worlds and game-completion trace, independent synchronized retail DOS audio/VGA comparisons.
- Original CODE.2 Windows milestone console-event #732 failure; diagnose without weakening source return, renderer or input assertions.

**Authoritative acceptance:** `Recomp/ACCEPTANCE_GATES.json` R01–R03 accepted, **3/12** coarse gates. Do not describe the game as a complete recompilation or as fully playable.
