# Development checkpoint 0.2.22

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.21-dev-incremental.zip` and preserves the verified opening pickup/ownership chain while replacing the mist transition's raw ownership-cleanup gap with converted original routines.

- 0.2.21 SHA-256: `2a6216e3a702062dfbacabde324c09d21052dec3668f42d41cbdabd0a9956810`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **92%**.

The end of the opening mist-transfer sequence now performs the original ownership removal semantics instead of updating only the alpha `PartyState::ownedObjects` mirror.

`CODE.2 0xF540 -> 0xF40C` is live against the held object, raw four-character party records, and mutable container descriptor state.

## Newly assembly-mapped components

- `CODE.2 0xF40C..0xF4AC` -> `ContainerRemoveF40C::execute`
- `CODE.2 0xF540..0xF5F6` -> `ObjectRemoveF540::execute`

## Exact removal behavior preserved

`0xF540` now preserves the original ordering and early-return behavior:

1. inspect held/cursor object first;
2. clear it immediately if it directly matches the target;
3. otherwise recursively remove the first target found inside a held container;
4. scan four active character records;
5. skip status-`0x8000` records;
6. scan exactly 21 WORD object slots from raw +0x40;
7. clear a direct slot to `0xFFFF`, retaining the original `0xF238` boundary;
8. otherwise recurse into category-1 containers via `0xF40C`;
9. return after the first successful removal.

`0xF40C` clears the matching container descriptor entry and the corresponding DS:7339 occupancy bit, and likewise returns after the first match.

## Mist-transition integration

At completion of the converted `0x21F70` state machine, the opening session invokes `0xF540` in original order for:

1. object 135
2. object 136
3. object 116

Object 233 is deliberately not removed.

The end-to-end regression picks/stows 136, 135, and 116, picks 233 last to trigger the mist sequence, completes the transfer, and verifies:

- level 23 is reached;
- raw party ownership no longer contains 135/136/116;
- held object 233 remains;
- the alpha ownership mirror likewise retains 233 only among the four quest objects.

## Verification gate

This checkpoint must pass from a fresh build directory:

- project configure/build;
- CTest;
- full regression against the read-only original-data overlay;
- direct `game.gog` validation;
- all shipped CINE00..CINE12 decode-to-end checks;
- standalone `0xF40C` direct/nested removal tests;
- standalone `0xF540` held, equipment-slot, and nested-container removal tests;
- full opening pickup -> mist transfer -> raw ownership cleanup integration.

The source archive excludes transient build directories and original Ravenloft game data.
