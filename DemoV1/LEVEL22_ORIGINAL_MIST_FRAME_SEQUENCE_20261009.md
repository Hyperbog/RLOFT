# Part 9 — original CODE.2 Level 22 mist loop reconstruction

**Goal:** faithfully recreate complete original game code. No modernization or invented cinematic.  
**Source:** immutable CODE.2, object-1 original physical file base `0x3AF24`, machine-code disassembly of logical `21F70..223F0`.

## Exact original execution order

The source routine `21F70` begins after the final-relic pickup branch `223F4:226CF`.

| Original logical offset | Operation |
| --- | --- |
| 21F78 | Calls `19CCC(0)`, reset motion/input state |
| 21F82, 21F87 | Calls `1F1C8`, then `1EA78` for initial presentation |
| 21F93 | Sets `BF30=1` event lock |
| 21F9D..222A6 | Moves and rotates the real camera incrementally toward stage column 23, row 7, facing 213 degrees, using original 16.16 sub-cell coordinates and repeated `1F1C8/1EA78` calls |
| 222A6..222BD | Calls `1CD48(2,0x21,7,-1,-1,-1,-1,-1,-1)` |
| 222C0..222E5 | Waits until high word of `CF4C` equals 2; each wait step increments `CF4E` and calls `31F94`, `1F1C8`, `1EA78`. **Not a fixed timeout.** |
| 222F0..22361 | For frame `i=0..21`, call `2E608`, `291B0(C050)`, `2E510(4,0x20, *(DWORD*)258DC + 0x67 + i,0)`, `1EA04`, `1EA78`, `28168`, `2E614(9,0)` in exact order |
| 22361..223A5 | **Only when i=20**, set column=9 + half-cell, row=58 + half-cell, facing 180, then call `181B4(level 23)`. The next iteration `i=21` uses the new level world. |
| 223B5..223D6 | After the **last** frame call, invoke `F540(0x87)`, then `F540(0x88)`, then `F540(0x74)`. Parchment `0xE9` survives. |
| 223D9,223DE | Calls `19CF4`, sets `BF30=0` and returns |

The transition uses the normal original loader `181B4`, not a restart of CODE.2 or a new character party. Its level/pose constants match the existing `AssassinRelicTransition` and `Level22Story` source-authenticated definitions.

## Implemented in this checkpoint

A dedicated `Level22MistTimeline` generates a typed, source-offset-indexed command trace, preserving **seven per-frame calls ×22**, the level load **after** the frame-20 action, original dynamic RES resource-base-relative frame offsets `0x67..0x7C`, and final relic removals. It preserves source routine identities where complete side effects are not yet reconstructed, rather than inventing timing or images.

`level22_mist_timeline_tests` verifies all 167 original trace actions, order, source offsets, frame IDs, level-change boundary, dynamic resource-offset calculation and relic-removal epilogue. This test is synthetic/source-order verification; it **does not** claim native playback or original DOS fidelity.

## Critical outstanding execution gaps

1. Recreate the **individual** 21F70 movement/turning loops and exact 16.16 step timing, render/presentation feedback; the model currently denotes these as a single opaque source routine.
2. Recreate original `1CD48` effect state progression and `CF4C` wait, and connect frame calls `2E608/291B0/2E510/1EA04/1EA78/28168/2E614` to the actual original RES/palette/VGA/audio/clock machinery.
3. Wire actual quest-triggered gameplay `pickup` to the faithful timeline; invoke `ExplorationRuntime::transition_to_level` **only after frame index 20**, then run post-frame `F540` removals using the corrected original duplicate-item behavior.
4. Cross-level mutable state, pickups, triggers, frame-21 new-world raster, original input/event-lock/interrupt ordering and final teardown need DOS reference acceptance.

Do **not** install a synthetic mist effect or fast-forward Level 23 for the sake of a playable demo.

**Source status:** 174 registered CTests after this checkpoint (19 Quick). Current HEAD source is **not compiled nor executed**; last verified Windows build #647 compiled 172 tests at older commit `fb4a0145`. **Scorecard:** unchanged provisional **49.8%**, 20 systems / 122 individually scored subsystems / current-head original DOS acceptance **0%**, because this code is a source-order reconstruction with no new live gameplay or verified parity.

## Reconstructed stage movement and destination archive

[Original 21F70 stage camera trace](LEVEL22_ORIGINAL_MIST_STAGING_20261009.md) reproduces the source heading and grid-step branches (including 63 draw/present pairs from the original fresh Level22 start). [The original archive audit](ORIGINAL_LEVEL23_DESTINATION_AUDIT_20261009.md) confirms destination Level23's 64×64 geometry and signed +17 RES4 selector. Both remain unbuilt and not wired to playable story presentation. Registry now contains **175** CTests.

## Original effect history gate

The [original 1CD48 flag audit](ORIGINAL_NUMBERED_EFFECT_1CD48_AUDIT_20261009.md) proves that the numbered effect 2 is associated with persisted bit **0x82** in DGROUP 22200; the source checks that bit to suppress repeated audiovisual effect execution and sets it afterward. The code model is separate from the still unimplemented live original effect dispatcher. **176** CTests now registered, unbuilt.
