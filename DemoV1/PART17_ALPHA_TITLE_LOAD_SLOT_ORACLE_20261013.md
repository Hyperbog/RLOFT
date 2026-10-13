# Part 17 — original title Load Game source boundary

**Branch:** `demo-v1`  
**Date:** October 13, 2026  
**Classification:** Original-byte research + Python regression, **not a native Load Game implementation or Alpha acceptance**.

## Version-controlled original evidence

Audited the supplied, private `CODE.2` (558,689 bytes, SHA-256
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`).
Original object-1 logical `0x07B94..0x07D42` maps to physical
`0x3AF24 + logical`. Its 431-byte range through exclusive `0x07D43`
has SHA-256
`d5fdc0c86cfdf304db4927fc0cee394d2354b1284608ba2c044ad228e698ff9f`.

The original executable confirms:

- It waits for mouse release at `07BA7→1A1F4`, then new press
  at `07BB0→1A1F4`. Masked mouse bit `2` returns status `2` at `07BDA`.
- Valid selector positions require X in **[21,379)** and Y in
  **[20,92)**. The X calculation uses stride **37**, recognizes
  remainders **0..25**, and maps zero-based column to numbered slot
  **1..10**. The original outer X bound clips the last slot at X=378;
  it does **not** accept X=379, even though a uniform ten-column
  26-pixel box would. No guessed extra hitbox was added to native code.
- The original `SAVE??\\SAVE.DAT` template lives at DGROUP
  `0x668C`, file `0x7C5B0`, with decimal digits written at
  `0x6690/0x6691`. The slash at `0x6692` is set temporarily to
  zero and restored to `'\\'` at `07CDC`.
- The original load copies the **whole directory**: `07C99→33A5A`
  changes to `WORK`; `07CA9→07378` deletes current WORK files;
  `07CB3→33A5A` navigates back; `07CD7→073A0` wildcard-copies
  selected `SAVE##` to `WORK`; `07CEB→07938` invokes the
  common loader. Preserve source error/retry and return behavior.
- DGROUP also holds `WORK` at `0xC020`, `SAVE.DAT` at `0x039C`,
  wildcard `\\*.*` at `0x03BC`, and parent `..` at `0x03C4`.

The preexisting executable-certified
`DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md` documents the common loader,
quicksave slot `SAVE00`, and normal snapshot coupling of
`WORK/SAVE.DAT` with **all** `WORK/LEVnnn.DAT` level snapshots.
Restoring `SAVE.DAT` alone would destroy continuity.

## Committed guard and observed tests

`DemoV1/tools/verify_original_title_load_slot.py` now:

1. Runs without original assets in normal public CTest, asserting source
   Load Game remains clearly unavailable instead of falsely jumping
   directly into the native exploration runtime.
2. Exercises all ten grounded selector columns and boundary/right-click
   negative cases, including X=379 rejection.
3. With optional `--original /path/to/CODE.2`, validates **whole-file
   SHA**, function-span SHA, **14** opcode windows, **8** decoded
   relative call targets, and **7** DGROUP fixed strings. It rejects
   differing patched or unrelated executables before interpreting offsets.

The private original-file oracle was executed locally on the provided
`CODE.2` and returned **`TITLE_LOAD_SLOT_SOURCE_PASS ORIGINAL_CODE2_PASS`**
when invoked with `--original-only --original /mnt/data/CODE.2`.
The registered public CTest is source-only and was not independently
executed in a full Windows build at the time of writing.

## Release blocker and implementation ownership

The current native title action 1 deliberately displays **Load Game
is not yet available**. It has no working native `SAVE##→WORK`
transaction, complete `SAVE.DAT` global deserialize, synchronized
`LEVnnn.DAT` restoration, post-load original state or matching DOS
modal-frame semantics. Calling it implemented on the strength of slot
geometry would be false.

For issue #15, reconstruct the original slot modal and both success and
error exits, model the write/read rollback and path behavior on an
isolated game copy, then integrate global + world rehydration only after
matching original source and tests. There is deliberately **no gameplay
code change** in this checkpoint; all ten Alpha gates remain open.
