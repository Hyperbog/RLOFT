# Level 22 — original 22510 pickup ribbon, 1C21C IRQ queue

**Scope:** faithful and fully playable Level22 first. No independent Level23 development.

## Direct original CODE.2 disassembly

The supplied immutable CODE.2 SHA256 is `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. Object-1 physical base `0x3AF24`, object-2 physical base `0x75F24`.

The original `223F4` viewport item action at `224FA..2250D` calls `21704`, transferring the item to DGROUP `8600` and compacting family C. Immediately afterward:

1. `22510..22534` copies the NUL-terminated prefix at the original **DGROUP D67C pointer** to the original scratch buffer `759C`.
2. The shipped CODE.2 has `DWORD[D67C]=0x11B4` in object-2, and `0x11B4` holds the actual bytes `You pick up \\0`. The native code obtains this prefix by following the original source pointer, **not** hardcoding English text.
3. `22535..22566` appends the original serialized OBJECTS 0x41-byte record name at byte +0. Native `ObjectDefinition::name` comes from the same source record.
4. `22567..225D9` appends an extra prefix at DGROUP `11C4` plus `2E6F0` result only if the item's current picked known-bit `8602` is nonzero **and** OBJECTS byte `+0x37` is nonzero. This charged/enchanted name suffix is **not reconstructed**; the new source model does not invent it. In the original LEVEL22 quest relics 0x74, 0x87, 0x88 and 0xE9 the actual OBJECTS +37 byte is **zero**, so the simple case is fully source-validated for all four.
5. `225DA..225E4` calls original **1C21C** with the assembled C-string, *before* examining which fourth relic triggers mist in `225E7..226CF`.
6. `1C21C..1C257` copies the C-string to source memory `14580`, sets `CF54 = IRQ30700 + 0x5A` modulo unsigned DWORD and `CF59=1`. The separate already reconstructed `CombatIrqDisplayRuntimeModel` carries the eventual source draw/erase sequence via `1C258/1C2C4`.

## Native integration

- `OriginalPickupRibbonTextModel` extracts original D67C-prefix bytes directly from source CODE.2 and composes original OBJECTS names, preserving the NUL and refusing any unsupported enchanted/known suffix.
- `ExplorationRuntime::pickup_level22_loose_item` and `pickup_level22_loose_item_at` now accept an **optional observed original IRQ tick**. The native Win32 Level22 click supplies its existing `code1_tick_count(exploration_epoch)` source-compatible `30700` cadence; test callers can omit the tick when no authentic observation is available.
- After a successful actual family-C pickup, the runtime owns the pending `CombatIrqMessageProjection` with the exact original string bytes, original IRQ deadline and pending flag. It retains the most recent actual source message, separate from story selector `BF44`.
- This is not a modern toast, debug-text substitution or fake in-game banner. The existing original `1C258 / 3060C` VGA draw binding must still be completed and manually compared against DOS before the ribbon can be declared player-visible and authentic.

## New regression

`original_pickup_ribbon_text_tests` checks source pointer dereference from an in-memory synthetic CODE.2 object-2 fixture, exact prefix/name composition, byte NUL, original IRQ `+0x5A` scheduling, strict unsigned deadline behavior, known/unenchanted path, unsupported enchantment suffix and invalid data rejection.

**Build policy:** source and tests committed, not built. Now **178 registered CTests** (19 Quick). Last accepted full Windows build still #647, 172/172 at old SHA `fb4a0145`. Engineering readiness remains 49.8% provisional; DOS acceptance 0%.
