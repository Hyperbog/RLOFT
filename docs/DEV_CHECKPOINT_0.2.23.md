# Development checkpoint 0.2.23

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.22-dev-incremental.zip`. It preserves the verified combat, death/drop, pickup, quest-ownership, mist-transition, and ownership-removal paths while replacing the alpha's direct quest-item stow mutation with the original opening-relevant inventory-slot exchange semantics.

- 0.2.22 SHA-256: `58919ffbcbf6c5c0fb089939eb34f38543cfee5bfbea75bdc9d1411c1b855b71`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **93%**.

The temporary UI bridge still chooses the first empty carried slot because the final HUD is not interactive, but it no longer performs the transfer itself. The selected slot now executes the reconstructed ordinary empty-slot branch of `CODE.2 0x117A0`.

## Newly assembly-mapped component

- `CODE.2 0x117A0..0x11AD7` opening ordinary empty-slot branch -> `InventorySlotExchange117A0::executeOpeningEmptySlot`

## Exact opening-path behavior preserved

For an empty selected slot, the reconstructed branch now:

1. captures the prior slot object from raw character +0x40 + slot*2;
2. captures the prior per-slot state from the original bit dword at raw +0x12B;
3. writes the held object into the selected raw slot;
4. mirrors slots 0..13 into `CharacterRecord::carriedItems`;
5. executes exact `0xF238` bit arithmetic after the slot write;
6. preserves the `0xF790(oldObject,oldState)` presentation boundary;
7. updates held state to the old slot object/state at the `0x1A5B0` boundary.

The special existing-slot branches for object families `0x7E..0x86`, `0xA6`, and `0x35`, plus the nonempty-container insertion path through `0x11404`, are outside the opening empty-slot route and remain explicit reconstruction boundaries rather than approximations.

## Verification gate

This checkpoint passes:

- a fresh configure/build;
- CTest;
- full read-only original-data regression;
- direct `game.gog` validation;
- all shipped cinematic decode checks;
- focused 0x117A0/F238 state-1 and state-0 tests;
- exact `F238 -> F790 -> 1A5B0` boundary ordering test;
- full assassin death/drop -> pickup/stow -> completing pickup -> mist cleanup -> level-23 regression.

The source archive excludes transient build directories and original Ravenloft game data.
