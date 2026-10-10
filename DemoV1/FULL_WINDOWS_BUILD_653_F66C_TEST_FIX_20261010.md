# Windows Full #653 — source-message dependent F66C test fixture correction — October 10, 2026

**Run:** [Full #653](https://github.com/Hyperbog/RLOFT/actions/runs/38079341409), source checkout `81a66262e5f8a298ce2788d8d797c939201ac070`. Canonical CODE.2 syntax gate and **all** C++ targets compiled successfully after project-wide C++17 repair.

**CTest:** **209/210 passed**. The single failure was `original_f790_runtime_description_tests` at line 232, asserting source ribbon pixel x=`0x44` had frame-zero color 10. All other source contracts, library tests, F790 oracle, source-only band/ribbon, and feedback planner tests passed.

## Root cause: incorrect test coordinate, not source behavior

Earlier `original_f790_modal_sequence_tests` deliberately used synthetic message `AA` with font `A` width 80, giving original F681/F66C ribbon cap x=`0x44`. The newer runtime-description bridge test passes real category-specific formatted source text (the POUCH item with live mutable weight), with **one-pixel fixture font glyph widths**. That entirely different message width changes the original source F681 computed left cap position. The bridge's renderer correctly computes a new location, while the regression had incorrectly copied the `0x44` coordinate from the unrelated AA fixture.

Repair the integration test to resolve actual F790 source message using `OriginalF790RuntimeEvent`, prepare a **test-only deterministic** F66C layout through the already source-verified `OriginalFeedbackRibbonText` model, assert frame-zero tile color at its actual first tile column, and assert empty-hand F790 background restoration at **the same modified pixel**. This test-only expected-location computation does NOT touch the gameplay `OriginalRandom` and maintains all existing live RNG/tick/order assertions.

## Verification limitations

No original-game implementation, PNG/RES content, native timing, or source event handler changed. The test source alone is corrected; **new Full Windows compile + CTest acceptance is required**. The 209/210 prior success must not be mislabeled a clean build, and the pipeline did not publish a verified user artifact or commit the Windows executable because CTest failed. Original DOS pixel/timing acceptance remains zero.

CI was restored byte-for-byte to the manually dispatched workflow after #653 trigger, without a second queued build.
