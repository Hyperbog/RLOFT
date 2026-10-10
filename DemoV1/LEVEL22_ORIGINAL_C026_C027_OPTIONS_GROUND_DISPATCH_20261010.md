# CODE.2 C026/C027 original GAME.OPT toggles to staged ground draw — 2026-10-10

**Status:** Independently original-binary verified, native options-to-isolated-dispatch **test coverage**, **not live game-mode integration**, Windows Full build pending. Authoritative `CODE.2` length 558,689 bytes, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 at `0x3AF24`, original DGROUP object-2 at `0x75F24`.

## Exact original mutable mode authority

Original DGROUP source bytes `C026=C027=1`. The 14-byte persisted `GAME.OPT` record begins at `C025`: `C026` is **Floors**, `C027` is **Ceiling**, and `C028` is **Sky**. The source option menu dispatch at `6520` has action 1→`65C3` and action 2→`65DB`. Both arms require mouse row **strictly less than `0xA8`** then execute original `65CF: XOR BYTE [C026],1` and `65E7: XOR BYTE [C027],1`. This is mutable user option state. A check of only immutable default CODE.2 bytes would be invalid after toggling.

Exact original code-object address census (five literal references each, scanned across the complete `0x3A852` object):

| Variable | Direct operand offsets | Source operations |
| --- | --- | --- |
| `C026` Floors | `05F5C,065D1,1F229,1F36B,34D48` | menu display read, menu toggle XOR, perspective trigger check, lower fill/texture branch, surface-record masked path |
| `C027` Ceiling | `05F6D,065E9,1F232,1F307,34D97` | menu display read, menu toggle XOR, perspective trigger check, upper fill/texture branch, surface-record high-flag path |

The native `OptionsState` already carries the exact 14 serialized original bytes and `OptionsMenuModel::apply` already uses the original guarded XOR semantics. Native `OriginalGroundSurfaceDispatch` independently reconstructs original source `1F227`, `1F305`, `1F369`. Existing runtime frame render still does **not** use mutable original GAME.OPT mode state end-to-end, nor complete original C026/C027/C028 active overlay callbacks; do not claim live mode rendering is completed.

## Cross-component source regression

Extended `options_menu_tests` to carry `OptionsState::defaults().raw[1/2]` to `OriginalUpperSurfaceInput::original_c026/c027`. Test the following source behaviors in an otherwise no-overlay 128-row fixture:

- Both ON: source 34998 heading recalculation eligible, upper texture `34A74`, lower texture `34A74`.
- Floors OFF: upper remains textured; lower source selects `33DE0` fill.
- Ceiling OFF too: upper `33E28` fill, lower `33DE0`, and original `34998` perspective recalculation skips because both modes are zero.
- Floors ON, ceiling still OFF: upper remains `33E28`, lower becomes `34A74`, and 34998 eligibility returns.
- Menu row `0xA8` does **not** toggle. Both plan calls are read-only over the original serialized options record.

The new source oracle `DemoV1/tools/verify_original_ground_mode_option_bytes.py` checks original CODE.2 SHA, the ten exact instruction windows, ten unique literal address references, original menu jump entries and initial DGROUP bytes, plus the native two-system C++ contract. Without user original media the registered Python CTest checks only native source registration. No private original bytes were introduced into Git. **Independent local original binary probe PASSED.**

## Fidelity boundary

The observed Level22 open-field `RES4[655]` full ground raster remains verified from Windows Full #658, 64/64 F12 captured renderer frames and 32768 indexed writes. This audit does not enable new modes: live `C026/C027` must be owned as exact original mutable game options and must govern 34998, 1F1C8 stages and 34D46/34D95 surface records. Source `FF50` is still only projected conditionally on unknown upper callback alias writes; original DOS 320×400 indexed pixels, original 6-bit DAC, IRQ/ticks and RNG comparisons remain separate acceptance gates. Current source Windows Full build is not run; CI remains manual-only.
