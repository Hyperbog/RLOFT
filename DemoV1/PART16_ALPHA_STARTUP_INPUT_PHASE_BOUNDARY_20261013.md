# Part 16 — Alpha startup Win32 input ownership/focus cleanup

**Branch:** `demo-v1`. **Scope:** Win32 host behavior only;
this is not an original DOS keyboard/IRQ emulation claim.

## Reproducible native defect

The host stores Escape in one mutable `escape_pressed_` flag.
`WM_KEYDOWN VK_ESCAPE` sets it without distinguishing title
menu, CODE.1 character creation and exploration owners.
Previously, `set_menu_mode`, `set_character_mode` and
`set_exploration_input` never cleared it. A title-menu Escape
could therefore immediately skip the *next* player-triggered
CINE00 replay, and a character-creation Escape could terminate
the **first Level22 gameplay loop** before a fresh user press.

The other native buffers also survived focus loss:
title `clicked_`, character `character_click_`/`key_events_`,
and exploration mouse event queues. Only held exploration
movement keys were cleared on `WM_KILLFOCUS`.

## Source boundaries

- Original CODE.2 `19464` polls `1A1F4` for pointer-modal
  actions, with selection 0 running original `13EF8(event=0)`.
- Original CODE.1 `043C` / `2DFC` input and CODE.2
  exploration have different input consumers.
- The native Windows Escape flag and `WM_KILLFOCUS` buffers
  **do not exist as equivalent original guest state**.
  Clearing them on owner change is a host adapter safety boundary,
  not a synthesized guest DOS keyboard/IRQ routine.

## Native fix

Mode ownership changes discard stale Escape and previous-stage
mouse/keyboard input before exposing the new owner.
`WM_KILLFOCUS` clears keyboard/mouse clicks, hover, Escape
and held exploration movement; `WM_SETFOCUS` samples the
current pointer to restore accurate original resource hitboxes.
The 19464/043C hitbox and original gameplay input mappings
are untouched. No game assets or CHARS/SAVE.DAT are modified.

`alpha_input_phase_isolation_contract_tests` is a source-only
CI regression with five deliberately unsafe mutations,
**not** a native UI field test. The first actual Windows startup
through the Level22 frame, original DOS input ordering and
full original-menu LOAD path remain open Alpha gates.
