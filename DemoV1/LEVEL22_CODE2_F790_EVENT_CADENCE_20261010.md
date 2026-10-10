# Original CODE.2 F790 modal invocation cadence and native ribbon epoch (2026-10-10)

**Scope:** source-verified, isolated source-only presentation adapter. No gameplay changes or Windows build; the new header is NOT wired into the live native game. All original CODE.2/RES resources are immutable. [Prior F66C source model](LEVEL22_F66C_FEEDBACK_RIBBON_SOURCE_MODEL_20261010.md) remains authoritative for tile and font geometry.

## Original CODE.2 evidence

User-provided shipped `CODE.2` length **558,689**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 physical base `0x3AF24`. Direct object-1 disassembly establishes source execution boundaries, and `tools/test_original_inventory_f790_cadence.py --original-code2 /mnt/data/CODE.2 --binary-only` passed **six exact instruction windows** plus the complete rel32 direct-call inventory.

| Logical | Physical | Original control-flow fact |
| --- | --- | --- |
| `21492..214B1` | `5C3B6` | Actor-action flag predicate enters `13118` from `214AC` |
| `13118..13138` | `4E03C` | Modal entry writes DWORD `6DA8=0`, byte `6DAC=0`; calls `19CCC(0)` then `112EC` once |
| `1133C..11351` | `4C260` | `112EC` invokes `F790` once after four actor columns |
| `11717..11740` | `4C63B` | Original scribe sets `6DAC=1`, deadline `6DA8=30700()+0x36`, and immediately invokes `F790` |
| `130CB..130E6` | `4DFEF` | Pointer/index-change path calls `F790` conditionally, not every idle poll |
| `13184..131C3` | `4E0A8` | Only if pending, original `30700() > 6DA8` **unsigned**, clear deadline/flag then call `F790` once; equality does not expire |

Exhaustive, five-byte E8-rel32 target scan of original object-1 finds exactly **17 direct F790 callsites**: `1134C 1173B 1178E 1184A 11882 118FF 119FC 11B63 11F47 127EC 128A3 12AFA 12BA4 12D50 12DFD 130DE 131BB`. Their instruction boundaries were independently inspected. The sole direct `112EC` caller is `13133`, and the sole direct `13118` caller is `214AC`. This is an inventory of **direct** rel32 instructions, not a claim excluding dynamically dispatched or indirect calls.

## Native implementation cross-check

Existing `DemoV1/src/win32_main.cpp` already calls `InventoryRuntimeState::reset_scribe_feedback_13118()` when opening actor panels, and `expire_scribe_feedback_13184(code1_tick_count(exploration_epoch))` once per native modal loop. Existing `inventory_runtime.cpp` uses an unsigned strict `>` comparison and clears its pending message. Those behaviors should NOT be duplicated. The host draws the entire `PartyModalPresenter` at ~70 VGA frames/s, including twice via distinct initial/loop paths; original `F790` calls are input/state-event owned and must not be equated to those host draws.

`OriginalFeedbackRibbonEpoch` is a **host-only, currently unwired** adapter: `enter_modal_13118` clears cached art; `source_f790_call` runs the prior proved `OriginalFeedbackRibbonText::prepare` only for a real original F790 event with `6DAC` pending; `present_vga_frame` exposes previously generated tiles without advancing RNG; ordinary F790 / expiry clears only pending-feedback artwork. It uses a caller-provided shared `30CA2` callback, never a second generator. The new synthetic C++17 regression passed 1,000 repeated VGA reads without extra RNG, verified **18 RNG draws** for a 160-pixel text message, verified two distinct pending F790 calls consume **36**, and checked event clearing, state reset and failure isolation. No shipped art is embedded.

The separate Python source/binary contract is registered in CTest, as is the C++ epoch test. **186 total registered CTests when Python is available**, up from 184; both new local tests passed in isolation, while the last completed Full Windows run **#651 passed 182/182 at an older source revision**. Do not claim a new Windows build, DOS pixel equivalence, or gameplay milestone. Remaining live integration blockers are exact per-interaction F790 event routing across all original callsites, using the one ExplorationRuntime RNG, original F790 ordinary-description raster branch, resource RES2:3 tile blits and cross-check against DOS captures. Building requires explicit user authorization. Level22-first and complete-game-before-modernization remain binding.
