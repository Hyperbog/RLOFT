# SAVE.DAT dual spell-family evidence

Status: **matrix A is executable-certified as the scribeable spellbook family; matrix B is a separate class/progression-driven preparation family. Offsets are normalized to runtime character base `0x218E0`.**

Every `0x248` character record contains two adjacent packed preparation matrices:

```text
matrix A  +0x06A..+0x0B9  runtime 0x2194A..0x21999
matrix B  +0x0BA..+0x109  runtime 0x2199A..0x219E9
```

Both are 8 spell levels x 10 stored entries, with high nibble = ready copies and low nibble = selected copies (`0xF` low nibble = unavailable).

## Matrix A: scribeable spellbook family

CODE.2 physical `0x4C554..0x4C6C3` operates exclusively on runtime matrix A (`0x2194A`) and implements spell scribing. It converts a spell identifier to bank/entry, checks availability/capacity, rejects an already available spell, and converts an unavailable cell to available state. The associated identifier base is:

```text
matrix-A spell id = 0x42 + spell_level_index*10 + entry
```

This same base is used when CODE.2 expands ready matrix-A copies into runtime spell/action records.

## Matrix B: distinct preparation family

The parallel expansion path reads runtime matrix B (`0x2199A`) and uses:

```text
matrix-B spell id = 0x92 + spell_level_index*10 + entry
```

Generated runtime records carry a family discriminator: matrix-A expansions write family 0 while matrix-B expansions write family 1. The two matrices therefore remain semantically distinct after SAVE decoding.

Matrix B's availability/progression setup is class-sensitive. Initialization chooses class-dependent table families and invokes distinct setup helpers. Evidence strongly matches the non-spellbook divine/priest side of the AD&D rules, but that player-facing label remains intentionally unpromoted until independently tied to the `0x92` resource family.

## Related selector byte `+0x131`

Runtime `0x21A11` maps to corrected record offset `+0x131`. Character setup clears it and ORs masks `0x04`, `0x02`, and `0x01` according to class state before spell-family initialization. UI paths also consume the complete byte as presentation/capability state.

This establishes `+0x131` as a spellcasting/class-family selector or capability mask. Exact bit-to-class names remain pending.

## Native-port consequence

```cpp
struct LegacySpellCell { uint8_t packed; };

LegacySpellCell spellbook[8][10]; // SAVE +0x06A
LegacySpellCell family_b[8][10];  // SAVE +0x0BA
uint8_t casting_capabilities;     // SAVE +0x131
```

Earlier `+0x072`, `+0x0C2`, and `+0x139` offsets were produced from the obsolete runtime base `0x218D8` and are superseded.