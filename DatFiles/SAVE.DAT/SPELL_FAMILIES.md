# SAVE.DAT dual spell-family evidence

Status: **matrix A is executable-certified as the scribeable spellbook family; matrix B is a separate class/progression-driven preparation family. The exact canonical label for matrix B remains under audit.**

Every `0x248` character record contains two adjacent packed preparation matrices:

```text
matrix A  +0x072..+0x0C1  runtime 0x2194A..0x21999
matrix B  +0x0C2..+0x111  runtime 0x2199A..0x219E9
```

Both are 8 spell levels x 10 stored entries, with high nibble = ready copies and low nibble = selected copies (`0xF` low nibble = unavailable).

## Matrix A: scribeable spellbook family

CODE.2 routine physical `0x4C554..0x4C6C3` is decisive. It operates exclusively on matrix A (`0x2194A`) and implements the executable's spell-scribing path:

1. It converts the incoming spell identifier into a spell-level bank and entry index.
2. It scans the selected bank's ten matrix-A cells and counts cells whose low nibble is not `0xF`.
3. It compares that count against a character-dependent limit (`0x6153[...]`).
4. It rejects a cell that is already available.
5. For an unavailable target cell, it builds the associated spell identifier from `0x42 + bank*10 + entry`, invokes the spell-name/text path, then clears the matrix-A cell to `0x00`, making the spell available for later selection/memorization.
6. The same executable image contains the user-facing strings `you do not understand this spell to scribe it.`, `SPELL SCRIBED.`, and `that spell is already in your spell book.` in the adjacent application string corpus.

This is sufficient to identify matrix A as the **scribeable spellbook spell family**. In AD&D gameplay terms this is the wizard/mage-style spellbook side of the dual system; native code should preserve the stronger executable description (`spellbook/scribeable`) where exact class naming is not required.

The spell identifier base used by this family is:

```text
matrix-A spell id = 0x42 + spell_level_index*10 + entry
```

This same `0x42 + bank*10` base is used when CODE.2 expands ready matrix-A copies into runtime spell/action records at physical `0x5E1A0..0x5E22A`.

## Matrix B: distinct preparation family

The parallel expansion path at physical `0x5E22A..0x5E2AA` reads matrix B and uses a different spell identifier base:

```text
matrix-B spell id = 0x92 + spell_level_index*10 + entry
```

The generated runtime records explicitly store a family discriminator: matrix-A expansions write `0` to runtime record byte `0x25921`, while matrix-B expansions write `1` to the same byte. This independently proves that the engine carries the two matrices as distinct spell families after SAVE.DAT decoding rather than merely as two pages of one array.

Matrix B's availability/progression setup is also class-sensitive. The normalization/initialization path chooses between table roots `0x62B0` and `0x6350` according to character field `+0x1D`, and level-up paths call two distinct matrix-B initialization helpers (`0x3ED08` and `0x3EDC8`).

The evidence strongly matches the non-spellbook divine/priest preparation side of the AD&D rules, but that label is not yet promoted to the serialized-format contract until the corresponding UI/list text or spell-name table is independently tied to the `0x92` family.

## Related selector byte `+0x139`

Runtime byte `0x21A11` maps to record `+0x139`. Character setup clears it and ORs masks `0x04`, `0x02`, and `0x01` according to class-related bytes before invoking spell-family initialization routines. Another UI path compares the complete byte against values `1`, `2`, `3`, and `4` to choose presentation state.

This establishes `+0x139` as a spellcasting/class-family selector or mask associated with the preparation subsystem. Exact bit-to-class names remain pending; preserve the byte verbatim.

## Native-port consequence

Model the matrices as separate families, not as one 160-byte undifferentiated spell array:

```cpp
struct LegacySpellCell {
    uint8_t packed;
};

LegacySpellCell spellbook[8][10]; // SAVE +0x072
LegacySpellCell family_b[8][10];  // SAVE +0x0C2
```

The names above describe current evidence; `family_b` should be renamed only after the `0x92` spell table/UI identity is certified.

## Next target

Trace the `0x42` and `0x92` spell-ID ranges into the spell-name/action tables to certify matrix B's canonical family name and enumerate per-level spell entries. Then map the `+0x139` selector bits and continue into the adjacent `+0x133..+0x138` fields and `+0x13A` state flags.
