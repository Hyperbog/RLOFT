# Original DDA4 / 50DC level-change gate and service-order checkpoint

**2026-10-08. Status: source-only, read-only original-code model.**
The previously reconstructed DDA4 XP and level-threshold models still
**do not award experience or mutate live character levels**.

## Immutable reference

Supplied original CODE.2 SHA-256:
`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
Object-1 file base: `0x3AF24`; logical offsets below exclude the
disassembler's `0x10000` virtual base.

Direct instruction audit established:

- `50DC:50E5..50F7` tests the original character status
  `WORD+0x10 & 0xF240`. A nonzero result returns **-1**.
- `50FD..514E`, for zero mode, checks signed class bytes
  `+0x15/+0x16/+0x17` for **class code 3, in slot order**.
  When none match, first class code **1** with signed
  `WORD+0x20 >= 9` returns track 0. Otherwise -1.
- `514F..518B`, for **any nonzero mode**, checks all
  three class bytes for code **4**, returning the first matching
  track or -1. This is a distinct original selector mode, not
  a refactoring of the zero-mode predicate.
- `DDA4:DF18` first writes the newly computed level word.
  `DF1F` then calls `50DC(0,actor)`. Only a nonnegative
  return permits the **optional** class-specific service:
  if the character's *first class code* is 1, call
  `3EA4(actor,new_level)`; otherwise call
  `3DE4(actor,new_level)`. Do not use the selected 50DC
  track to choose this branch.
- Regardless of the 50DC return, `DF54..DF86` then
  invokes `3210F` (formatted presentation), `1C21C`
  (timed message queue), and `DC7C` (level-related HP
  calculation), **in that order**. `1C21C` observes
  original `30700` IRQ time, sets a `+0x5A` deadline
  at `CF54`, copies a queued message and sets flag
  `CF59=1`. It is not the pauseable A7E4 clock.
- The `DC7C` call unconditionally starts with original
  `1C328` dice (sides from a signed class-indexed byte
  at data `0x6821`) and ends by adding a signed
  16-bit HP increment to **both** current HP
  `+0x1C` and maximum HP `+0x1E`. Its complete
  class/race/Constitution conditional calculation is not
  yet implemented and must not be replaced with a
  simplified HP rule.

## C++ additions

`CombatLevelChangeGateModel::inspect` is a pure implementation
of the 50DC return classification, including the original
signed level-9 exception and both modes.

`CombatLevelChangeSequenceModel::inspect` accepts a verified
party snapshot **after the level WORD is written, before 50DC**.
It requires an actual old/new level difference, a matching
new level in the observed class lane and a valid actor.
It reports the ordered service list, with or without optional
3EA4/3DE4. It does not execute any presentation, IRQ,
RNG or HP mutation.

These models must be invoked **per level transition** after
preceding original callbacks have completed. Preplanning all
character transitions from one stale party snapshot would
be semantically wrong: earlier callbacks can mutate character
data before later reads and before final DBD4 recomputation.

## Verification and limits

- 18 new `50DC` instruction signatures and 13 new
  `DDA4/DC7C` instruction signatures matched immutable
  CODE.2 bytes: **31 new, 378 cumulative original opcode
  signatures**. There is still one direct Strength data
  signature plus two hashed original class data tables.
- Both committed model/source and test files were copied
  verbatim into a standalone C++17 harness, and each
  Git blob identity was checked against GitHub. Both
  `-O0` and `-O2 -DNDEBUG` **passed** with
  `-Wall -Wextra -Werror -pedantic`, using a minimal
  `PartyImage` record declaration with the same size and
  offsets as the repository.
- Both CTest targets are CMake-registered, but **the full
  native repository and Windows executable have not been
  rebuilt or executed**, and original DOS playthrough
  timing/character-state comparison has not been completed.

## Next dependency

Reconstruct `DC7C` completely from the original small
data tables, signed class/race/Constitution arithmetic, 1C328
dice ordering, and the existing party record. Then audit the
3EA4/3DE4 callbacks and timed message behavior before
performing live level changes. Any native mutation must preserve
the original DDA4 sequence, `DC7C` RNG consumption and
final DBD4 recomputation.

**Five-major-gate demo acceptance remains 20% (1/5).**
No automatic private GitHub Actions builds were triggered.
