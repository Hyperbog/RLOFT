# Development checkpoint 0.2.27

## Continuity base

This checkpoint continues directly from `Ravenloft_Alpha_0.2.26-dev-incremental.zip`. No renderer/viewer rollback or alternate source snapshot was used.

- 0.2.26 SHA-256: `4053160297ebf8f5b8682eb7adfecd6d4e952d9152aba8eab53dfc3f2fb27016`
- `Ravenloft_Decomp_Final.zip` SHA-256: `331c85756a36ba359c1f0823f612bd24e759965b361f73dbfc5910546f177729`
- original `game.gog` SHA-256: `a915b600a59b7512cc3894d154a6225a312a74526cc9c5965542107eddc37e6a`

## Mechanical-demo milestone

Verified mechanical-demo completion: **97%**.

## Exact SAVE.DAT writer / live-session snapshot closure

The original `CODE.2 0x67D4..0x6C55` save writer order is represented by `SaveGame::encode` / `SaveGame::write`. The live opening session exports its current reconstructed state through `OpeningDemoSession::snapshotSaveDatState`, `encodeSaveDat`, and `writeSaveDat`.

The reconstructed writer preserves, in original order:

- the fixed 50-byte description and 32-byte quest flag field;
- level, 16.16 party X/Y and facing;
- F03C/F03E/FD08/6DAE fixed fields;
- the F58C timer value read through the reconstructed `0x30700` boundary;
- world clock, held object ID/state;
- four raw 0x248-byte character records;
- DS:C038 dynamic-record count and exactly `count * 0x16` dynamic bytes;
- D8A8 and A7E4 state;
- all 62 variable-capacity DS:7330 container arrays plus their descriptor state bytes;
- F920/F91C lighting state;
- the exact-size address-named DS:67EC, DS:A808 and DS:6FA0 tail blocks.

## New regressions

The shipped Quick Start `SAVE.DAT` is decoded and immediately re-encoded and must match the original bytes exactly.

A mutation round-trip test changes fields spread across every proven writer region, adds one dynamic record, writes the result, reloads it through the production loader, and verifies the values without normalizing raw character/container state. The encoded file grows by exactly 0x16 bytes for the added dynamic record.

A live-session test advances the original timer, moves the party, creates the assassin drops through the reconstructed death path, picks up object 136, snapshots the current session, and verifies that F58C/A7E4, party position/facing, held object/state, party records and container state come from the live reconstructed state rather than the immutable boot save.

## Verification gate

This checkpoint passes:

- incremental compile of the new regression against the unchanged 0.2.26 implementation;
- ordinary CTest;
- exact untouched Quick Start SAVE.DAT byte-for-byte round trip;
- mutated SAVE.DAT write/read round trip;
- live session SAVE.DAT snapshot regression;
- full read-only original-data regression;
- direct original `game.gog` validation;
- all shipped CINE00..CINE12 decode checks;
- all prior movement, combat, death, drop, pickup, ownership, mist and timing regressions.
