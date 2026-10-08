# Original CODE.2 DC7C level-up HP — source-backed checkpoint

**Date:** 2026-10-08  
**Branch:** `demo-v1`  
**Status:** original binary disassembled, immutable data audited,
read-only native C++ implementation and focused CTests registered.
**Not connected to live character mutation or a complete Windows build.**

## Original immutable evidence

- CODE.2 full-file SHA256:
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Original executable object-1 file start: `0x3AF24`.
- Original data object-2 file start: `0x75F24`.
- DC7C function extent: original object-1 logical `DC7C..DDA0`.
  CS six-way DWORD jump table at `DC64..DC7B`: targets
  `DD06,DD06,DD06,DD27,DD17,DD32`.
- Two exact signed Constitution-modifier rows of **19 bytes**
  each at data `67FA..681F`. The index is original unsigned
  Constitution **current** byte at actor `+0x37`;
  the row is chosen by **signed class code / 3**, times 19.
  Data SHA256: `5090628e6309047081b2fc48172d79d8b13ffc08025af478484f42932760d60a`.
- Original HP die-sides table at data `6821`, for
  class codes `0..5`: `10,10,10,8,4,6`.
  Its six-byte SHA256 is
  `f79a9a67b29974232d1b0008612c8caf2a3046bd4d39b30500248285e910764e`.
- Six-target CS jump-table SHA256:
  `21c4e5c680980250d2ad1ac886735cc7a9f6f3d9a1c3e9bdfdf24e6c908734fa`.

Both original tables are extracted from the user-owned executable
and checked by an FNV64 fingerprint in the source reader. The
original-code verification tool independently checks the **SHA256**
of both data spans and the CS dispatch span. No proprietary
table contents are committed as distributable asset files.

## Exact original HP algorithm

`DC7C(actor_index, class_lane_index, new_level)`:

1. `DC80..DCA7`: index actor at stride `0x248`, read
   signed class `+0x15+lane`, look up its die sides and call
   `1C328(1,sides,0,0)`. This consumes **one original 30CA2
   RNG draw even when the subsequent level cap discards the roll**.
2. `DCAB..DCD4`: look up the signed Constitution adjustment
   from `67FA+19*(class/3)+byte[actor+0x37]`.
3. `DCFE`: take the original class-specific CS dispatch:

   | Class code | When below threshold | On/after threshold |
   | --- | --- | --- |
   | 0, 1, 2 | `die + con_bonus`, level below 10 | `con_bonus + 3`, level 10+ |
   | 3 | `die + con_bonus`, level below 10 | `con_bonus + 2`, level 10+ |
   | 4 | `die + con_bonus`, level below 11 | `con_bonus + 1`, level 11+ |
   | 5 | `die + con_bonus`, level below 11 | `con_bonus + 2`, level 11+ |

4. `DD3F..DD49`: first clamp the value to minimum **one**.
5. `DD51..DD84`: if second signed class byte `+0x16 != -1`,
   divide by **three** if third signed class byte `+0x17 != -1`;
   otherwise divide by **two**, truncating toward zero.
   If second class byte is `-1`, do not divide, **even if
   the third slot is present**. A triple-class division of one
   is **zero**. There is **no second minimum-one clamp**.
6. `DD8E/ DD95`: add low 16 bits of that HP increase to
   both maximum HP `WORD+0x1E` and current HP `WORD+0x1C`,
   preserving 16-bit wrap. Character and RNG state must be
   observed at actual DC7C entry, after preceding synchronous
   `50DC`, optional `3EA4/3DE4`, `3210F`, `1C21C` effects.

The original reads memory without modern safe bounds handling.
The native model reports unsupported for class codes outside the
six proven jump-table entries, Constitution values outside the
two 19-byte rows, invalid actor/class-lane indexes and invalid
table sources. This is deliberate **fail-closed preservation**,
not a claim that the original DOS game gracefully rejected them.

## Added C++ files

- `include/rloft/combat_level_up_hp.hpp`
- `src/combat_level_up_hp.cpp`
- `tests/combat_level_up_hp_tests.cpp`
- CMake registration under `rloft_boot_flow` and CTest
  `combat_level_up_hp_tests`.

`CombatLevelUpHpModel::inspect` returns a **read-only projection**:
original single die, chosen Constitution modifier, class/level
branch, post-multiclass HP delta, current/maximum HP result and
post-die original RNG state. It does not change `PartyImage`,
the shared RNG, levels, world state, message queue or clocks.

## Tests and audit accuracy

- Exact committed source/header/test Git blob IDs were matched
  against local copies. The regression compiled and passed
  `-std=c++17 -Wall -Wextra -Werror -pedantic` in both
  Debug (`-O0`) and Release (`-O2 -DNDEBUG`) against the
  original CODE.2 at `RLOFT_TEST_CODE2`.
- The initial regression had a **fixture error**:
  Constitution 15 in the second row is **+1**, not +2.
  The original-file assertion exposed this, it was corrected
  in the committed test, and both build configurations then
  passed. Test expectations for classes 3/4/5 were updated.
- The original-byte ledger gained **29 distinct DC7C instruction
  addresses** and three independent code/data table hashes.
  It now has **407 named instruction-byte checks at 399
  distinct logical addresses**, **one** direct Strength data
  anchor, **four** hashed DGROUP tables and **one** hashed CS
  jump table. This is **not** equivalent to full original binary
  decompilation or full gameplay coverage.
- **No complete CMake, Windows EXE, or original-DOS frame/timing
  acceptance run has occurred since these commits**. The native
  private-repository Actions workflow remains manual-only.

## Next

Reconstruct and compare the original `3EA4` / `3DE4`
class-specific level-change callbacks and the `1C21C`
IRQ-timed message queue. Only then sequence DDA4 and DC7C
in live party state with shared original RNG, actor writes and
derived `DBD4` attack target. Continue to preserve original
E284 item drop / BF44 / death presentation / XP award ordering.

**Overall faithful demo: 20% (1 of 5 major gates accepted).**


## Extended independent differential regression

A retained original-asset-gated section of
`combat_level_up_hp_tests.cpp` now evaluates **16,416** cases:
all six supported classes, all 19 Constitution indexes, levels
`0,1,9,10,11,20`, one/two/three-class records, original RNG seeds
`1,42,0,0xFFFFFFFF`, and actors `0` and `3` to cross-check
the `0x248` record stride. The reference reconstructs dice from
the original LCG, reads signed modifiers directly from the original
DGROUP bytes and chooses the branch from the **original CS:DC64
six-way jump-table DWORD values**. It independently checks the die,
post-division increase, two 16-bit HP sums, and subsequent RNG state.

All **16,416/16,416** original-byte-driven comparisons passed.
The same committed source/test also passed a strict C++17
`-fsanitize=undefined -fno-sanitize-recover=all` run
against the original file, with all matrix assertions active.
The corrected exact committed test Git blob SHA is
`aa187a3b86ec37688693bbc289536d47e3ded8b8`,
and both strict C++17 Debug and optimized Release modes passed
with the supplied original `CODE.2`. The test runs its synthetic
subset without proprietary assets; the exhaustive matrix is
enabled by setting `RLOFT_TEST_CODE2` to an owned CODE.2 path.

These are function-model differential results, **not** DOS
in-game event-order, real-time IRQ, or compiled Windows-demo
equivalence tests. The model is not connected to live party state.
