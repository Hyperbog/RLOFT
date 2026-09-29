# Development checkpoint 0.2.21

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.20-dev-incremental.zip`. It preserves the verified normal player-hand -> assassin combat/death/drop path and adds the original opening ground-object pickup and quest-ownership trigger chain.

- 0.2.20 SHA-256: `25faecefdb75a45cf0b23982f861bf74f19b979cf7a0a9a986ff885da2ec4e8d`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **91%**.

The assassin's four quest drops now enter the original opening pickup/ownership path rather than being granted directly by the alpha helper:

`CODE.2 0x223F4 object branch -> 0x21704 -> held-object state -> 0xF390 / 0xF4B0 -> 0x21F70 completion condition`

## Newly assembly-mapped components

- `CODE.2 0xF390` -> `ObjectContainsF390::evaluate`
- `CODE.2 0xF4B0` -> `PartyOwnsObjectF4B0::evaluate`
- `CODE.2 0x21704` opening-relevant world-object pickup -> `GroundObjectPickup21704::execute`
- `CODE.2 0x224E6..0x22713` object branch in `0x223F4` -> `OpeningGroundPickup223F4::execute`

## Original ownership behavior verified

The Quick Start regression proves details that the earlier alpha abstraction did not model:

- held/cursor object is considered owned before party slots;
- each active character contributes 21 WORD object slots beginning at raw +0x40, not only the 14 carried slots;
- equipment-slot object 252 is found through the extended slot scan;
- recursive container ownership is live: shipped quiver object 321 contains object 57 and `0xF4B0(57)` succeeds;
- parchment 233 is not owned in the initial Quick Start state but is owned immediately when held.

## Production opening pickup verification

After the real `0xE284 -> 0x2195C` assassin death path creates the four runtime drops, the regression performs production pickups from the runtime 22-byte object array:

1. pick object 136; no mist trigger;
2. stow it through the temporary inventory-UI bridge;
3. pick object 135; no mist trigger;
4. stow it;
5. pick object 116; no mist trigger;
6. stow it;
7. pick object 233 last; converted `0xF4B0` proves possession of 135/136/116 and the `0x21F70` transition condition fires.

A separate regression verifies that clearing the original click bit leaves the ground-object array and held state unchanged.

## Intentional remaining boundary

`stowHeldObjectInFirstFreeCarriedSlot()` is explicitly a temporary inventory-UI bridge. It is not claimed as an assembly reconstruction. The final HUD/inventory-click routine will replace it while keeping the converted world pickup and ownership components unchanged.

The next mechanical target is exact `0xF540` ownership removal at mist completion so the transition removes 135/136/116 from held state, all 21 party slots, and nested containers exactly as the original does while retaining 233.

## Verification gate

This checkpoint must pass from a fresh build directory:

- project configure/build;
- CTest;
- full regression against the read-only original-data overlay;
- direct `game.gog` ISO validation;
- all shipped CINE00..CINE12 decode-to-end checks;
- Quick Start `0xF390/0xF4B0` direct/equipment/nested-container ownership tests;
- click-gated runtime pickup and four-object quest-completion path.

The source archive excludes transient build directories and original Ravenloft game data.
