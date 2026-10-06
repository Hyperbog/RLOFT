# SAVE.DAT dual spell-family evidence

Status: **high-effort executable-certified family identities. Matrix A is the Mage/scribeable spellbook family; matrix B is the Cleric/Paladin priest-style preparation family. Offsets use runtime character base `0x218E0`.**

Every `0x248` character record contains two adjacent packed preparation matrices:

```text
matrix A  +0x06A..+0x0B9  runtime 0x2194A..0x21999
matrix B  +0x0BA..+0x109  runtime 0x2199A..0x219E9
```

Both are 8 spell levels x 10 stored entries, with high nibble = ready copies and low nibble = selected copies (`0xF` low nibble = unavailable).

## Matrix A: Mage spellbook family

CODE.2 physical `0x4C554..0x4C6C3` operates exclusively on runtime matrix A (`0x2194A`) and implements spell scribing. It converts a spell identifier to bank/entry, checks availability/capacity, rejects an already available spell, and converts an unavailable cell to available state.

The identifier geometry is:

```text
Mage/matrix-A spell id = 0x42 + spell_level_index*10 + entry
```

Class-enum reconstruction independently establishes `class_code 4 = Mage`. Character initialization at physical `0x51B27..0x51B61` tests all three class slots for code 4; if present it sets casting-capability bit `0x02` and invokes the spellbook initialization path. This closes matrix A as the **Mage/scribeable spellbook family** rather than merely a generic scribeable family.

## Matrix B: Cleric/Paladin priest-style preparation family

The parallel runtime expansion reads matrix B (`0x2199A`) and uses:

```text
priest/matrix-B spell id = 0x92 + spell_level_index*10 + entry
```

Generated runtime records carry family discriminator 1, distinct from matrix A family 0.

The class identity is now directly recoverable from character initialization plus the certified class enum:

```text
class_code 3 = Cleric
    -> sets casting-capability bit 0x01
    -> calls physical 0x3ED08
    -> 0x3ED08 reads/writes matrix B at runtime 0x2199A

class_code 1 = Paladin
    -> sets casting-capability bit 0x04
    -> calls physical 0x3EDC8
    -> 0x3EDC8 reads/writes matrix B at runtime 0x2199A
```

Both helpers use class/level-dependent tables while enabling/disabling matrix-B cells. Therefore matrix B is not an unidentified second family: it is the **priest/divine preparation family shared by Clerics and Paladins**, with different progression/setup paths for those classes.

This is stronger than assigning matrix B simply to Clerics. Paladins use the same serialized matrix family through a distinct level-progression helper.

## Casting capability byte `+0x131`

Runtime `0x21A11` maps to record `+0x131`. Initialization clears it and derives these bits from certified class codes:

```text
bit 0 / 0x01  Cleric preparation capability (class code 3)
bit 1 / 0x02  Mage spellbook capability      (class code 4)
bit 2 / 0x04  Paladin priest capability       (class code 1)
```

The byte may contain combinations for multiclass characters, so it should remain a mask rather than a single enum.

```c
uint8_t casting_capabilities; // +0x131
```

## Family/resource geometry

```text
matrix A / Mage        T indexes 0x42..0x91
matrix B / priest      T indexes 0x92..0xE1
```

Storage reserves eight 10-entry banks per family. Per-bank executable tables still determine which cells are meaningful at each spell level.

## Native-port consequence

```cpp
struct LegacySpellCell { uint8_t packed; };

LegacySpellCell mage_spellbook[8][10]; // SAVE +0x06A
LegacySpellCell priest_spells[8][10];  // SAVE +0x0BA
uint8_t casting_capabilities;          // SAVE +0x131
```

Preserve the original matrix bytes and capability mask exactly at the compatibility boundary. A native rules layer may expose Mage/Cleric/Paladin abstractions, but legacy serialization must retain the shared matrix-B representation.

Earlier `+0x072`, `+0x0C2`, and `+0x139` offsets were produced from obsolete runtime base `0x218D8` and are superseded.