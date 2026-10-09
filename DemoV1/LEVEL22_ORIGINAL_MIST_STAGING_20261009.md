# Part 9 — original CODE.2 21F70 staged camera reconstruction

**Goal:** recreate the complete original game code. This is source fidelity work, not modernization.  
**Status:** source trace model and authored CTest, not built or presented in the live Windows game.

## Original byte-level evidence

Control evidence: original CODE.2 object-1 logical addresses, original physical object-1 starts at file `0x3AF24`. The independent raw disassembly covers `21F70..222A6`. The original routine prepares the camera **before** the Level22 relic/mist effect begins.

- If source column `255D2` is greater than **23**, turn in 10-degree steps to **90**, then recenter the column fraction to `0x8000`, decrement the column to 23, calling `1F1C8` followed by `1EA78` after every update (`21FA6..22048`).
- Otherwise, **including column exactly 23**, turn to **270**, then increment column as required, still recentering the fraction and emitting the original draw/present pairs (`2204A..220F9`).
- If source row `255D6` is greater than **7**, turn to **180** and decrement row to 7 (`22111..221A5`).
- Otherwise, **including row exactly 7**, turn to **0** and increment row to 7 (`221A7..2222B`).
- Finally, turn to **213 degrees** with the original source comparisons `0xD2..0xDC`, asymmetric ten-degree stepping, snap windows and 360-degree wrap (`2222D..222A6`). This is the staging pose for the original effect; do **not** replace the actual turning with a teleport or shortest-arc interpolation.

**Important original quirk:** even from column 23, row 7, heading 213, CODE.2 stages by turning first to 270, then to 0, then to 213. Equal-axis cases are not skipped.

## Code and regression

The new header-only `OriginalMistStagingModel::trace` returns every intermediate original pose, its phase and the two original call-site offsets for the corresponding drawing/presentation cycle. It emits no artificial images, sounds or time advances. The caller will ultimately use the original reconstructed display and IRQ engine for those calls.

The new `original_mist_staging_tests` tests original fresh Level22 (4.5,4.5,270) yielding **63** per-step draw/present pairs, centered staging yielding **48**, alternate decrementing-axis paths, immediate 275-degree snap from heading 1, numerous headings at both map extremes, and invalid-camera rejection.

This is a distinct source-order model that fits ahead of [the 22-frame original sequence](LEVEL22_ORIGINAL_MIST_FRAME_SEQUENCE_20261009.md). Both models remain **unwired to live cinematic execution**, and no fabricated event is enabled.

**Registered CTests:** 175 total, 19 Quick. **Verified Full Windows build:** still #647, older SHA `fb4a0145`, 172/172 tests passed. These 175 current-source tests are unbuilt. **Scorecard:** unchanged 49.8% provisional readiness, 20 major systems/122 individually scored subsystems, current-head DOS acceptance 0%.

## Remaining faithful code reconstruction

The original staged camera calls must be integrated into one live runtime session; effect `1CD48` and `CF4C/CF4E` wait logic, all 22 original RES sprite frame calls, original level-23 load only after frame index 20, post-frame-21 F540 relic removal and final teardown still require full original graphics/audio/timing and DOS comparison. Do not declare the event complete on the strength of a trace model.
