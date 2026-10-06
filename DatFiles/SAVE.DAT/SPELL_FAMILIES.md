# SAVE.DAT dual spell-family evidence

Status: **high-effort executable-certified family identities with raw-`T` catalogue closure.** Matrix A is the Mage/scribeable spellbook family; matrix B is the Cleric/Paladin priest-style preparation family. Offsets use runtime character base `0x218E0`.

Every `0x248` character record contains two adjacent packed preparation matrices:

```text
matrix A  +0x06A..+0x0B9  runtime 0x2194A..0x21999
matrix B  +0x0BA..+0x109  runtime 0x2199A..0x219E9
```

Both are 8 spell levels x 10 stored entries, with high nibble = ready copies and low nibble = selected copies (`0xF` low nibble = unavailable).

## Matrix A: Mage spellbook family

CODE.2 physical `0x4C554..0x4C6C3` operates exclusively on runtime matrix A and implements spell scribing. It converts a spell identifier to bank/entry, checks availability/capacity, rejects an already available spell, and converts an unavailable cell to available state.

The identifier geometry is:

```text
Mage/matrix-A T index = 0x42 + spell_level_index*10 + entry
```

Class-enum reconstruction establishes `class_code 4 = Mage`. Character initialization tests all three class slots for code 4; if present it sets casting-capability bit `0x02` and invokes the Mage spellbook initialization path.

The shipped executable entry-count table at DGROUP `0x6440` is:

```text
[7, 6, 10, 4, 2, 6, 0, 0]
```

The raw `T` resource independently matches those counts, yielding **35 prepared Mage spell entries** across levels 1-6.

## Matrix B: Cleric/Paladin priest-style family

The parallel runtime expansion reads matrix B and uses:

```text
priest/matrix-B T index = 0x92 + spell_level_index*10 + entry
```

Generated runtime records carry family discriminator 1, distinct from Mage family 0.

Class ownership is direct:

```text
class_code 3 = Cleric
    -> capability bit 0x01
    -> setup routine 0x3ED08
    -> matrix B at runtime 0x2199A

class_code 1 = Paladin
    -> capability bit 0x04
    -> setup routine 0x3EDC8
    -> matrix B at runtime 0x2199A
```

The normal priest setup paths use count table DGROUP `0x6420`:

```text
[7, 6, 5, 4, 4, 2, 0, 0]
```

Direct `T` decoding yields **28 prepared priest spell entries** across levels 1-6.

The first priest resource bank also contains `LAY HANDS` and `TURN UNDEAD` after its seven prepared entries. Because the executable bank count is seven, these are action labels sharing the priest-family text/dispatch namespace rather than prepared matrix cells.

See `SPELL_CATALOGUE.md` for every exact T label, record index and dispatch ID.

## Casting capability byte `+0x131`

Runtime `0x21A11` maps to record `+0x131`. Initialization derives these independent bits:

```text
0x01  Cleric preparation capability  (class code 3)
0x02  Mage spellbook capability       (class code 4)
0x04  Paladin priest capability       (class code 1)
```

The byte may contain combinations for multiclass characters and must remain a mask.

```c
uint8_t casting_capabilities; // +0x131
```

## Resource geometry

```text
matrix A / Mage    T[0x42..0x91]  = T[66..145]
matrix B / priest  T[0x92..0xE1]  = T[146..225]
```

Both ranges reserve eight ten-entry banks even though the shipped active counts for banks 7 and 8 are zero.

## Legacy level-loss inconsistency

Priest level-loss normalizer `0x3EB28` is called for both Paladin and Cleric class slots and operates on matrix B, but it bounds its entry loops with `0x6440`, the Mage-shaped count table, rather than normal priest table `0x6420`.

This original executable inconsistency does not alter the independently established family ownership. It should be preserved in a strict compatibility path or corrected only as an explicit modern-port deviation.

## Native-port consequence

```cpp
struct LegacySpellCell { uint8_t packed; };

LegacySpellCell mage_spellbook[8][10]; // SAVE +0x06A
LegacySpellCell priest_spells[8][10];  // SAVE +0x0BA
uint8_t casting_capabilities;          // SAVE +0x131
```

Preserve original matrix bytes, capability bits, T-index geometry and legacy count-table behavior at the compatibility boundary.

Earlier `+0x072`, `+0x0C2`, and `+0x139` offsets were produced from obsolete runtime base `0x218D8` and are superseded.