# Part 9 — original CODE.2 F540 direct item-removal fidelity correction

**Current work:** complete original-game code recreation, not modernization. **Build status:** source and tests committed, not yet compiled; CI builds remain manually authorized.

## Controlling binary evidence (original supplied CODE.2, object-1)

Original CODE.2 object-1 is at physical file offset `0x3AF24`. These offsets are **object-1 logical offsets**, confirmed by direct `objdump -D -b binary -m i386 -M intel` disassembly of immutable original bytes.

- `F540..F55F`: if a held cursor item equals the target, call `1A344(-1,0)` and jump to `F5EF` (return).
- `F56C..F57B`: if the target is nested within the held item's container via `F40C`, terminate the search immediately.
- `F581..F5A1`: scan four `0x248` character records; do not inspect an inactive actor (actor status high-bit 0x8000).
- `F5A1..F5AB`: for each actor scan up to **21 slots** at actor `+0x40`, skipping absent `-1` entries.
- `F5AD..F5C6`: direct target match writes `0xFFFF` to the slot and calls `F238(actor,slot,0)`. **At `F5C6`, the branch target is `F5D9`: it increments the slot and continues the same actor loop.** It does *not* break that actor's loop.
- `F5C8..F5D7`: if `F40C` found a nested instance, jump to `F5EF` instead of continuing.
- `F5D9..F5ED`: advance every slot and actor; direct duplicate target items can therefore be removed more than once in a single `F540` invocation.

## Native discrepancy and repair

`InventoryRuntimeState::remove_object` already honors held-item-first, nested recursive return, inactive actor skip, 21-slot extent, and unchanged stale actor-known bits. However its direct party-item match incorrectly used `break`, which prematurely skipped remaining slots within the current actor. Changed it to `continue` with original branch evidence, **preserving the original duplicated-item quirk** rather than "fixing" the DOS behavior.

The existing `inventory_runtime_tests` now verifies three matching copies in two actors, all cleared by one original `F540` call, stale item-known flags unchanged, and no remaining target in the party. No separate new CTest was registered. Existing total remains **173 registered / Quick 19**, none of the Part 9 modifications built or tested by Windows CI. Historical Full Windows run #647 passed only the older 172-test source.

## Other original Level 22 source evidence found while auditing

`223F4:226CF` invokes `21F70` after a valid final relic pickup. `21F70..223F0` performs original position/heading staging, initiates original effect `1CD48(2,0x21,7,...)`, waits for the effect state, runs **22 iterations** of the native RES frame/action sequence, calls level loader `181B4(0x17)` after rendering **frame index 20**, then invokes `F540` in the **specified order** for item IDs `0x87`, `0x88`, `0x74`. The document/test-only level transition constants already match those original instructions. Full faithful graphics/audio/clock sequencing and live event dispatch still need implementation; do not invoke incomplete cinematic frames as substitutes.

### Next fidelity gates

- Original Level 22 movement-to-stage `21F70` stepping order and 22-frame graphic calls must be matched with a source-derived native choreography model.
- Transition callbacks may change level **only at the original frame-20 point**, using original authored coordinates and facing, then consume three items **after** the sequence; parchment ID `0xE9` remains.
- End-to-end Windows verification of a real source-initiated level switch, actual RES frame raster and effect/audio teardown, and current-head DOS comparison remain open.

**Completion score:** this correction removes a verified error in an already scored inventory subsystem, so the existing **49.8% provisional engineering readiness** does not change; original-DOS acceptance is still **0%**.
