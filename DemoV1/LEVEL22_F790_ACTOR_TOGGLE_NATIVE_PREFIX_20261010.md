# CODE.2 11AD8 native actor-toggle prefix and input return boundary — 2026-10-10

**Workstream:** B, faithful native reimplementation. **Branch:** `demo-v1`. **Status:** original-source-verified isolated native C++ code and local regression; **not** connected to live Win32 input, original VGA/F790 drawing or source item-action follow-on.

## Original identity and direct binary verification

Immutable original `CODE.2`: **558,689 bytes**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object-1 executable file base `0x3AF24`; logical offsets below are relative to that base. Before this code batch, original `11AD8..11C1F` and `12E70..1309E` were disassembled directly with i386 Intel syntax, and existing actor-mode/resource audits were consulted. No candidate disassembler labels are treated as proof.

The expanded `tools/verify_original_f790_actor_toggle.py` independently matches **16** exact original instruction windows and **11** direct E8-rel32 targets, including the source input queue `22D49/22DC1/22E20`, mode branches, actor redraws, F790, and the generic cleanup path. The original Windows workflow has **not** run against this newer source.

## Exact bounded implementation: 11AD8 through 11B68

New `include/rloft/original_actor_modal_toggle_prefix.hpp` exposes four **DWORD** `6F90` contexts, original `C050` full word and the mutable `7F6C` lane offset, not an invented independent boolean-only game state. The adapter's callbacks correspond to original functions `291B0`, `11268` and the **one** source `F790` call at `11B63`.

- **Flags without bit 2:** `11B30` XORs the selected actor's **low BYTE** at `6F90+4*actor`; `11B37..11B4B` selects current `C050` **BYTE** via `291B0` and redraws only that actor via `11268`. The existing `7F6C` lane remains unchanged.
- **Flags with bit 2:** `11AF0` captures the selected actor's **entire DWORD** before any callback; `11B02` XORs only bit zero of its low WORD **once**, then `291B0` runs. `11B0E..11B2E` copies the resulting DWORD to each of four actor contexts, changes `7F6C` to `0,80,160,240`, and calls `11268` in that order. The back edge `11B2E: EB DE` returns to `11B0E`, not the XOR.
- **Both paths:** `11B53` first loads known BYTE `8602` unsigned and pushes it, then loads held-ID WORD `8600` signed and pushes it, *after* repaint operations; `11B63` invokes `F790` exactly once. The adapter provides a deferred callback rather than caching held/known before `291B0` or `11268`. The callback must use the existing **single game RNG** and original 112EC saved page; host VGA frame refreshes cannot call it.

The adapter has no fabricated rollback or host timing. If a source callback throws, already-applied prefix writes remain; invalid actor indices are rejected before mutation.

## Important 12F94 versus 1307C control-flow distinction

Source `12F8A` pushes the original flags and actor index, `12F8F` calls `11AD8`, and `12F94..12F9A` **returns directly** after that call. Therefore an actor-toggle input with flag bit 2 does **not** fall through to the generic `12E70` flag-2 cleanup at `1307C`. That later cleanup calls `310B0` (free backing page), `1F8FC` (modal cleanup) and `1AE30` only on paths that actually reach it.

The current Windows host consumes its secondary-click queue by closing the modal *before* actor hit-testing. It therefore cannot implement the original bit-2 actor operation at the toggle hotspot if that Windows click is intended to represent source bit 2. The raw DOS mouse-button-to-source-flags mapping is not yet independently established; **do not infer that right-button always means bit 2 or patch it speculatively**. Preserve the existing host input while tracing the raw event producer and remaining callbacks.

## Current tests and blocked integration

New `tests/original_actor_modal_toggle_prefix_tests.cpp` asserts source ordering, exact DWORD and BYTE mutations, every actor-offset redraw, C050 byte select, single late F790 call, callback mutation visibility, source-preserving exception behavior and invalid actor rejection. It passed locally under strict C++17 GCC, optimized GCC and Clang (all `-Wall -Wextra -Werror -pedantic`). The new CTest target `original_actor_modal_toggle_prefix_tests` brings the registry to **212 with Python available**, counting the previous 211 source registry. The Python original-byte oracle now guards the new C++ source and deliberately unconnected host.

**Known remaining callback gate:** `11B6B..11C19` dispatches different original item actions: the bit-2 branch conditionally writes `BF44=0x3A` for held IDs `0xA7..0xC9`, calls `1A5B0`, and toggles the low bit of `C050`; the other branch conditionally writes the same BF44, calls `1A724` with the current actor offset/other original values, and toggles `C050`. Do not claim the full actor toggle is native-integrated until those callbacks, original flags, resource-page lifecycle, and the real F790 renderer are connected.

**Build gates:** last explicitly authorized Full Windows #654 passed **210/210** on its earlier checkout; the new 212-test source has **not** been built in Windows CI. No fresh CI build was requested or dispatched. No DOS pixel/IRQ/RNG/state parity, subsystem completion increase, or independent Level23 gameplay is asserted.

**Next independent module:** prove `11B6B..11C19` action arguments and their exact input/call ordering against the original bytes and reconstructed `Code2.cpp`; confirm original source input-to-button flags, then connect the complete callback lifecycle to Win32. Maintain F12 read-only source events while original saved-page/text/RNG parity remains unaccepted.
