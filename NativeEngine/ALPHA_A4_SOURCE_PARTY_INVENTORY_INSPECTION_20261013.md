# Alpha A4 — original CHARS party inventory and equipment inspection

**Branch:** `native-engine`  
**Source checkpoint:** `ff5611ae9415860466e5efe3c02699eccc4b9d4f`  
**Date:** 2026-10-13  
**Classification:** Pre-Alpha. A real-data inventory **read-only inspector**, *not* moving equipment, game-use, pickup, a DOS-perfect inventory UI or save-game inventory changes.

## Native first-level input

The CODE.1 → CODE.2 normal-party handoff produces the authentic 0x920 CHARS file (four serialized 0x248 records). The initial two actors retain their exact CODE.1 transfer bytes. Original CODE.2's item array begins at character record `+0x40`, with **21 little-endian signed 16-bit object IDs** at slots `0..20`; empty item ID is **`0xFFFF`**, not zero. The independent item-identified mask is `+0x12B` as a little-endian 32-bit word with slot bit `i` controlling the identified status of item slot `i`.

This milestone adds the portable `NativePartyInventory` API (`native_party_inventory.hpp/.cpp`), consuming the exact original party CHARS and a caller-provided immutable original `OBJECTS.DAT`:

- Demands exact counted **336 × 65-byte OBJECTS definitions**. Validates every source name is ASCII and NUL-terminated within the original 40 bytes, all source categories are in 0..15 and all RES1 UI image offsets are in the original object icon bank `168..335`.
- Extracts the source ID, 40-byte display name (including genuine blank names), category, original absolute **RES1 UI icon ID `168 + OBJECTS[+0x2D]`**, and known/identified bit for each occupied actor item.
- Checks each occupied serialized object ID is a real in-range object definition. No fake starting equipment or silent out-of-range fallback is allowed. Name and actor identity must agree with the original decoded 0x920 CHARS.
- Emits the original equipment-slot names for source slots 12 Ring A, 13 Ring B, 14/15 Hands A/B, 16 Body, 17 Head, 18 Accessory, 19 Quiver and 20 Bracers. Slots 0..11 are ordinary inventory; equipment routing is not altered. Original CODE.2 `12180..1246E` is the controlling routing evidence; slot names are behavioral convenience labels.
- **F7** in the normal generated-party GPU Level22 window toggles an original-ID/name/known-flag inspector for **both actual created actors**. It draws source items in two columns and never equips, drops, consumes, identifies, spawns or creates an item. No click-to-select or item manipulation is claimed.

## Original owned binary evidence

Supplied original `OBJECTS.DAT` size **21,844 bytes**, 336 entries, SHA-256 `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`.

The new `rloft_native_startup_audit` checks every definition and a **separately marked synthetic-only audit CHARS** fixture against actual original OBJECTS bytes. This is not a manufactured runtime character:

| Source object ID | Name from actual OBJECTS | Category | Original absolute RES1 UI ID |
|---|---|---:|---:|
| 6 | QUIVER | 1 | 174 |
| 12 | LEATHER ARMOR | 4 | 180 |

It verifies the first actor has known slot0 and the second actor has a different occupied slot0 without a known mark. There are exactly two fixture item instances. The verification publishes `original_party_inventory_definitions=336`, `original_party_inventory_slots_per_actor=21`, `original_party_inventory_object6_12_verified=true` and `original_party_inventory_gameplay_verified=false`. Windows original-media CI fails if those source fields diverge.

`native_party_inventory_tests` uses a tiny self-contained **336-record test catalogue** and synthetic valid CODE.1 actor records to exercise empty `0xFFFF`, real object ID **0**, equipment slot/known-mask decoding, actor-level source identity, UI icons, category and name extraction, exact source-size rejection, invalid category/UI index/name, and out-of-range IDs. Packaging now requires this new test; full suite expected **42** CTests.

## What is still required for genuine gameplay

1. CODE.2 mutable inventory and original object-instance representation; equipment moves and category/class restrictions; original UI/item sprites and current visual material state.
2. Native source-authored **pickups, use effects, container inventory, object placement and quest state** in Level22 and beyond. Native A3's checkpoint deliberately preserves only original initial CHARS bytes; any future item mutation requires a *new explicit native-save schema version*, not secretly editing this immutable original transfer.
3. Correct live CODE.2 RES3 sprite-frame selection plus the first actual Level22 ASSASSIN encounter and world event script.
4. Real interactive native Windows GUI field-play acceptance. Unit/CI passing does not establish a playable Alpha.

Original game binaries are read only. No copyrighted data enters GitHub or the Windows media-free archive. 
