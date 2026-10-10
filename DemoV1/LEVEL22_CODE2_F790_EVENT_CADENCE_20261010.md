# Original CODE.2 F790 modal invocation cadence and native ribbon epoch (2026-10-10)

**Scope:** source-verified, isolated source-only presentation adapter. No gameplay changes or Windows build; the new header is NOT wired into the live native game. All original CODE.2/RES resources are immutable. [Prior F66C source model](LEVEL22_F66C_FEEDBACK_RIBBON_SOURCE_MODEL_20261010.md) remains authoritative for tile and font geometry.

## Original CODE.2 evidence

User-provided shipped `CODE.2` length **558,689**, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 physical base `0x3AF24`. Direct object-1 disassembly establishes source execution boundaries, and `tools/test_original_inventory_f790_cadence.py --original-code2 /mnt/data/CODE.2 --binary-only` passed **six original event windows, subsequently expanded to nine by the shared-RNG correction** plus the complete rel32 direct-call inventory.

| Logical | Physical | Original control-flow fact |
| --- | --- | --- |
| `21492..214B1` | `5C3B6` | Actor-action flag predicate enters `13118` from `214AC` |
| `13118..13138` | `4E03C` | Modal entry writes DWORD `6DA8=0`, byte `6DAC=0`; calls `19CCC(0)` then `112EC` once |
| `1133C..11351` | `4C260` | `112EC` invokes `F790` once after four actor columns |
| `11717..11740` | `4C63B` | Original scribe sets `6DAC=1`, deadline `6DA8=30700()+0x36`, and immediately invokes `F790` |
| `130CB..130E6` | `4DFEF` | Pointer/index-change path calls `F790` conditionally, not every idle poll |
| `13184..131C3` | `4E0A8` | Only if pending, original `30700() > 6DA8` **unsigned**, clear deadline/flag then call `F790` once; equality does not expire |

Exhaustive, five-byte E8-rel32 target scan of original object-1 finds exactly **17 direct F790 callsites**: `1134C 1173B 1178E 1184A 11882 118FF 119FC 11B63 11F47 127EC 128A3 12AFA 12BA4 12D50 12DFD 130DE 131BB`. Their instruction boundaries were independently inspected. The sole direct `112EC` caller is `13133`, and the sole direct `13118` caller is `214AC`. This is an inventory of **direct** rel32 instructions, not a claim excluding dynamically dispatched or indirect calls.

## Follow-on saved VGA page and 17-source-argument audit

Original 112EC captures a 0x1400-byte 320x16 VGA background via 292BC; original F790 restores it on every invocation via 2932C before deciding its feedback path. Ten byte-exact original instruction windows passed and the 17 E8-rel32 calls are now classified 11 global, 3 empty, 2 local, 1 selected-index; native event routing remains unresolved. The isolated native background-page component and regression are registered, with 189 CTests pending a user-authorized Windows Full build. See [source proof](LEVEL22_F790_BACKGROUND_2932C_20261010.md).

## Native implementation cross-check

Existing `DemoV1/src/win32_main.cpp` already calls `InventoryRuntimeState::reset_scribe_feedback_13118()` when opening actor panels, and `expire_scribe_feedback_13184(code1_tick_count(exploration_epoch))` once per native modal loop. Existing `inventory_runtime.cpp` uses an unsigned strict `>` comparison and clears its pending message. Those behaviors should NOT be duplicated. The host draws the entire `PartyModalPresenter` at ~70 VGA frames/s, including twice via distinct initial/loop paths; original `F790` calls are input/state-event owned and must not be equated to those host draws.

**Correction after additional original-binary audit:** The earlier epoch adapter omitted F66C for ordinary nonempty held items. The original F790 seeds 30CC4(held item ID), draws F66C, then reseeds from original 30700 tick; only the empty hand skips F66C. Pending 6DAC takes the unreseeded F66C path. The corrected source-only epoch now accepts the native OriginalRandom object and deferred tick callback, and the new raster adapter replays cached RES2:3 art without extra RNG. See [shared RNG and raster replay audit](LEVEL22_F790_SHARED_RNG_AND_REPLAY_20261010.md).

The separate Python source/binary contract is registered in CTest, as is the C++ epoch test. **187 total registered CTests when Python is available after the follow-on corrected epoch and raster regression**, up from 184; both new local tests passed in isolation, while the last completed Full Windows run **#651 passed 182/182 at an older source revision**. Do not claim a new Windows build, DOS pixel equivalence, or gameplay milestone. Remaining live integration blockers are exact per-interaction F790 event routing across all original callsites, using the one ExplorationRuntime RNG, original F790 ordinary-description raster branch, resource RES2:3 tile blits and cross-check against DOS captures. Building requires explicit user authorization. Level22-first and complete-game-before-modernization remain binding.
