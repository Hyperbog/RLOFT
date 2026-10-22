# Part 25 — original CODE.2 20440 SINTAB, CHARS and Level 22 integration

**Owner:** `Recomp` branch. **Status:** IMPLEMENTED AND SOURCE-EXECUTED ON ORIGINAL MEDIA FOR THE PRESENT-CHARS/LEVEL-22 BRANCH. **NOT** full `20440` callback closure, a player-facing boot, a rendered frame, or original DOS parity.

## Original source order exercised in one guest

The canonical compiled original `353A0 → 20608 → 20440` worker remains stackful. Previous source callbacks initialize DOS/4GW heap, mouse, DPMI, 11 RES tables, VGA 290F4, dual-page 20294, 5E38 absent GAME.OPT, CHARS probe, 312A0 DOS file service and second original 16D04 INIT. The following three original source bodies now execute **on that same worker and original RAM** in their actual `20440` order:

1. `03838` (source callback name `3838`): reads the original `SINTAB` file through the same read-only DOS handle table, uses original source `3104C` Watcom heap allocations, and verifies source table dimensions and pointers.
2. `16CD0 → 16ADC`: the *existing-CHARS* branch selected by original `31218` reading the owner-original 0x920-byte `CHARS`. Uses reconstructed original `0DBD4`, `03EA4` and `03DE4` character-source callbacks. Absent-CHARS `19888` creation remains separate and unimplemented in this continuous runtime.
3. `181B4` (default `20608` level argument `0x16`, Level 22): original source first attempts the constructed `work\\lev022.dat` file, detects its absence in the original installation, then executes the indexed read-only `LEVELS.DAT` archive fallback using original source heap/file wrappers. The original filename buffer `0x170A0` is not initialized by the reconstructed startup; the adapter restores **only** the attested `levels.dat\0` literal at original `CODE.2` file offset `0x51FC4` after authentic CODE.2 identity verification. That filename adapter is host-derived, not a verified original guest startup instruction.

Existing original `1FC14 → 30B40` host suspension is still reached after these source operations; two explicit no-key replies re-enter the real source loop with unchanged guest memory.

## Independent source-vs-media verification

Source `181B4` writes guest `0x25618` Level 22 header, `0x198E0` packed grid, `0xC6C8` active level and `0xCF44` event-script pointer. `Recomp/src/code2_original_res_startup.cpp` independently parses the read-only original archive with `Code2/graphics/original_levels_grid.hpp` and compares the **actual guest results** byte-for-byte (header except the verified original 0x29A scale normalization; all 450 packed 16-bit cells). It does not prefill the guest using the parser. Expected dimensions 30 × 15.

**Windows original-media evidence:** [run #39702311445](https://github.com/Hyperbog/RLOFT/actions/runs/39702311445), **85/85 CTests** and `ORIGINAL_CODE2_20440_LEVEL22_SOURCE_GRID_ACCEPTED`, together with `ORIGINAL_CODE2_1FC14_TWO_SOURCE_LOOP_ITERATIONS_PASS`. Linux/Windows C++ source-link regression is tracked separately. All owner originals and pixel outputs remain private.

## Remaining work — do not mislabel as completed

Other `20440` optional callbacks remain unconnected or host-staged: `325B0` original palette reload, `291B0/291C8` page handling, `2ECB4/2F184` game-picture resource operations, `1A0B0/1A288` mouse/cursor initialization, and full initialization dependencies of `181B4` (`171FC`, audio, `1F8E0` scene transition and actor/event callbacks). The source code currently skips unset callback slots; passing the Level 22 binary grid comparison does **not** establish all of these branches or live actor behavior.

**Next exact step:** preserve current source-media grid gate, then hook/validate the remaining `20440` calls in their original sequence, including actual scene palette and transition callbacks. Only afterward advance to frame renderer, keyboard-controlled movement, and independent retail DOS comparison. No invented DOS success or original content bytes may be committed.
