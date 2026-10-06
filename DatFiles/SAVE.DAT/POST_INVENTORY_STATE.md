# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **fixture shape established; one executable anchor certified; remaining semantics intentionally unresolved.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. It exists to prevent sparse quick-start data from being mistaken for padding and to provide reproducible offsets for later CODE.2 consumer tracing.

## Hard executable anchor

The character runtime base is DGROUP `0x218D8`. The audited CODE.2 reconstruction directly tests and clears byte `0x21A12`, therefore:

```text
0x21A12 - 0x218D8 = 0x13A
```

Record byte `+0x13A` is consequently live character state/flags, not padding. The final semantic audit specifically identifies exact bit tests/byte clears in this field as behavior that had to be preserved. Individual bit names remain unresolved until the relevant consumers are isolated.

## Reference-fixture survey

A byte-for-byte scan of the supplied 9,288-byte quick-start SAVE.DAT was performed across `+0x072..+0x247` for all four active-party records and all ten saved-roster records. The survey reports bytes other than the two dominant fill values `0x00` and `0xFF`; this is an observation tool, not an occupancy or semantic predicate.

The two populated active characters are sparse:

```text
EHLIANNA
  +0x10A = 0x0F
  +0x131 = 0x02
  +0x13A = 0x00

ANVIL DAWN
  +0x0C2 = 0x11
  +0x10A = 0x0F
  +0x131 = 0x01
  +0x13A = 0x00
```

All ten saved-roster records in this fixture contain only `0x00`/`0xFF` values throughout the same post-inventory range. The two unused active-party records likewise contain no byte outside `0x00`/`0xFF`; their `+0x13A` byte is `0xFF`, while populated records observed here use `0x00`.

These facts do **not** establish meanings for `+0x0C2`, `+0x10A`, or `+0x131`. They are retained as exact candidate anchors for executable cross-reference work.

## Candidate runtime addresses

Using runtime record base `0x218D8`, the fixture-varying offsets correspond to:

```text
record +0x0C2 -> runtime base 0x2199A
record +0x10A -> runtime base 0x219E2
record +0x131 -> runtime base 0x21A09
record +0x13A -> runtime base 0x21A12  [executable-certified live state]
```

The first three addresses are search targets, not yet semantic claims.

## Reconstruction rule

Do not collapse `+0x072..+0x247` into reserved padding. The certified live byte at `+0x13A` proves this area contains gameplay state, and the fixture contains additional non-fill bytes at `+0x0C2`, `+0x10A`, and `+0x131`. A native decoder must preserve the entire 470-byte tail losslessly until its substructures are fully decoded.

Likewise, do not infer record occupancy from `+0x13A`: unused records contain `0xFF` there in this fixture, but no executable occupancy predicate has yet been proved.

## Tool support

`tools/analyze_character_records.py` now prints every byte in `+0x072..+0x247` that is neither `0x00` nor `0xFF`, alongside the already mapped core fields. This makes the sparse-tail survey reproducible on additional saves and should expose array boundaries as soon as saves with memorized spells, active effects, or other mutable character state are available.

## Next target

Trace CODE.2 consumers for runtime addresses near `0x2199A`, `0x219E2`, `0x21A09`, and especially the bit masks applied to `0x21A12`. Then compare additional SAVE fixtures with deliberately varied character state. No spell/effect interpretation should be promoted until executable and differential-save evidence agree.
