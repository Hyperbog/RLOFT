# Level22 F790 live-inventory and source-event integration gate (2026-10-10)

**Authority:** original shipped `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; independent `OBJECTS.DAT` item records, original `T` text archive and existing native inventory source. **Status:** isolated runtime/event components implemented, NOT yet attached to live Windows input/redraw. Source evidence is not Windows or DOS acceptance.

## Integration completed inside source-only components

`OriginalF790RuntimeDescription` consumes an actual raw 0x41-byte OBJECTS.DAT item record, the already parsed native `ObjectCatalog`, and a const reference to the actual **mutable `InventoryRuntimeState`**. It requires matching original record count and source size, verifies the selected record's item category, weight, signed subtype and enchantment agree with the parsed catalog, and refuses invalid item IDs. The empty-hand `-1` sentinel returns empty text without record lookup. A category-1 item obtains its *current* nested half-unit weight from the existing `InventoryRuntimeState::container_weight_half_units`, including the original F5F8 subtype20 Bag-of-Holding special case, not the initialized ten-byte descriptor's stale slot array. The unchanged `OriginalF790ResolvedDescription` then renders all source field arithmetic with original CODE.2 punctuation, T labels and subtype lookup tables.

The new `OriginalF790RuntimeEvent` takes explicit **source-call arguments** `item_id` and `known`, pending byte `6DAC`, and original pending buffer `759C`. This is deliberate: `F790` is NOT uniformly invoked with the global cursor. `F7B4` checks pending feedback first, and pending events forward their existing `759C` text without any item lookup or normal-description formatting. Ordinary calls resolve current source text BEFORE mutating the saved VGA background or shared RNG, then invoke the existing `OriginalF790ModalSequence::source_event_f790` once per original event. The actor's `ExplorationRuntime` RNG must be the **same** `OriginalRandom&` in a real hookup. Redraws only invoke `replay_after_host_redraw`. The bridge does not start a new PRNG, event loop or invented ribbon.

## 17 exact source callsite origins

An independent scan of *original binary bytes* found precisely these 17 direct E8-rel32 calls to `F790`, divided by their two pushed argument sources. The new `verify_original_f790_runtime_event.py` pins the call target, all direct sites, argument-source opcode signatures and original F7B4/ F7DB precedence gates.

| Original direct caller(s) | Argument form | Source context/meaning |
| --- | --- | --- |
| `1134C` | global 8600/8602 | Initial modal 112EC, after 292BC background capture |
| `1173B 1178E` | global 8600/8602 | Scribe feedback pending, original 6DAC and 6DA8 assigned first |
| `1184A 11882 118FF` | explicit (-1,0) | Item consumed or delegated, restoration with empty arguments |
| `119FC` | stack-local | Ordinary item/cursor exchange, locally saved ID and known |
| `11B63` | global 8600/8602 | Actor/container pane is redrawn via 11268 |
| `11F47` | stack-local | Container transaction using local arguments |
| `127EC 128A3 12AFA 12BA4 12D50 12DFD` | global 8600/8602 | Equipment/character-panel redraw families |
| `130DE` | selected item ID, known=0 | Selected item feedback rather than held cursor |
| `131BB` | global 8600/8602 | Expiry check: only after unsigned 30700 tick exceeds 6DA8 and 6DAC clears |

**Verified grouping:** 11 global, 3 explicit empty, 2 stack-local, 1 selected index. Higher-level source contexts listed here describe direct disassembly neighborhoods; fully equivalent native event cadence and all indirect call paths remain separate proofs.

## Native input and redraw audit

Current `DemoV1/src/win32_main.cpp` invokes actual mutations in separate click branches: `swap_actor_container_slot` near line 1494, `swap_ordinary_slot` near 1515, `swap_equipment_slot` near 1571, `swap_hand_slot` near 1818, and `quick_stash_held` near 1861. These events do not map one-to-one to the original 17 immediate display callsites merely by being called. Source original 13184 unsigned feedback expiry is observed near host line 1383, **but its boolean change result is currently discarded**, whereas original 131BB calls F790 on expiry. The modal presenter is separately drawn in the two host paths near lines 1630 and 1902 at the native presentation frequency. Those repaints MUST NOT call the source-owned event/RNG method: it would consume far more `30CA2` draws than original DOS.

There is intentionally no live call from Win32 to `OriginalF790RuntimeEvent` yet. Finishing the mapping requires the exact event triggers and actor/held-known state at the seven click families and modal transitions; pending feedback message generation using the existing authentic T source; a 112EC one-shot page capture; and a native/DOS original-RES2:3 pixel/cadence comparison. Preserve the source's unconditional background restore even for empty-hand F790 events, and never substitute an approximate repaint cadence.

## Added test coverage

One new C++ CTest `original_f790_runtime_description_tests` now checks both the full source-text arithmetic on synthetic 90-object data and **mutations of real native container state**, verifying live changing pouch weight, subtype20 Bag of Holding invariant, known-only enchantment/charges, damage ranges and rejection of mismatched raw/catalog records. It also verifies source-argument ownership when the global cursor intentionally holds a different item, the 6DAC-priority path with a deliberately invalid item ID, and dispatch into the real indexed framebuffer via `OriginalF790ModalSequence` with **one shared OriginalRandom**. The event test asserts exactly one post-draw IRQ sampling for the ordinary branch, none for pending/empty, and unchanged RNG through 100 VGA-only redraws.

The new optional-original-Python `original_f790_runtime_event_contract_tests` pins the 17 binary caller sites and prevents premature `win32_main.cpp` live hookup. **201 CTests registered when Python is present**, increased from 199. Source/binary identities and original E8 positions were checked locally; the two latest registered CTests, and the native inventory/game library at this HEAD, are **NOT Windows-built or executed**. Last actually completed Full Windows **#651 passed 182/182** at an earlier revision. Original DOS parity remains unaccepted and readiness/acceptance percentages must not change without user/DOS evidence.

## Next bounded actions

Continue mapping individual mutation statuses to original direct F790 calls while distinguishing redraw-only, swap, empty-hand and scribe expiry paths. When event causality is proved, bind original T and raw OBJECTS data once per session, call through the ONE existing ExplorationRuntime RNG, and reuse prepared raster on native present frames. Then request permission for a manual Windows build and user-assisted native/DOS screenshots, click sequences and IRQ deadline comparison. No independent Level23 or modernization work.
