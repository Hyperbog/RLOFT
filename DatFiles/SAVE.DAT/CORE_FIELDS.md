# SAVE.DAT early/core character fields

Status: **high-effort executable/fixture reconstruction using corrected runtime character base DGROUP `0x218E0`.** Sex, race, class, alignment, base Armor Class, and the six standard ability pairs are now cross-certified against executable text lookups, the supplied SAVE fixture, and published game documentation. Age remains a strongly supported serialized identity field.

## Runtime/file relationship

Character `n` begins at runtime `0x218E0 + n*0x248`. Serialized record offset is therefore `runtime_address - 0x218E0` after removing the character stride.

The base is directly proved by the save/load path, not inferred only from field values:

```text
physical 0x41949  push 0x920
physical 0x4194E  push 0x218E0
                 write party image

physical 0x41CF5  push 0x920
physical 0x41CFA  push 0x218E0
                 read party image
```

`0x920 = 4 * 0x248`.

## Authoritative early/core partition

```text
+0x000..+0x00F  fixed 16-byte name storage
+0x010..+0x011  uint16 status_word
+0x012          portrait/appearance index within an engine 23-entry sex-specific presentation bank
+0x013          uint8 sex_code
+0x014          uint8 race_code
+0x015..+0x017  int8 class_code[3]
+0x018..+0x019  uint16 age
+0x01A          uint8 alignment_code
+0x01B          int8 base_armor_class
+0x01C..+0x01D  int16 current_hp
+0x01E..+0x01F  int16 max_hp
+0x020..+0x025  int16 class_level[3]
+0x026..+0x031  int32 class_progress[3]
+0x032/+0x033   Strength base/current
+0x034/+0x035   Dexterity base/current
+0x036/+0x037   Constitution base/current
+0x038/+0x039   Intelligence base/current
+0x03A/+0x03B   Wisdom base/current
+0x03C/+0x03D   Charisma base/current
+0x03E/+0x03F   exceptional Strength percentile base/current
+0x040..+0x069  int16 item[21]
```

## Status/activity word `+0x010`

Runtime `0x218F0` maps to record `+0x010`. Party/UI iteration uses the high bit as an inactive-record discriminator; unused active-party slots contain `0x8000` while populated fixture records contain zero in that bit.

```c
uint16_t status_word;
bool record_is_active = ((status_word & 0x8000u) == 0);
```

The status audit now certifies recruited/template provenance `0x0080`, Vengeful-Spirit possession `0x0100`, Held `0x0200`, disease `0x0400`, poison `0x0800`, paralysis `0x1000`, death `0x4000`, and empty-slot `0x8000`, plus the separate Vengeful-Spirit warning/coercion state `0x0040`. Bit `0x2000` is consumed as an action/eligibility blocking tier but has no proved producer/player-facing identity in the supplied build; low bits `0x0001..0x0020` likewise remain raw. See `STATUS_WORD.md` for the bounded residual audit.

## `+0x012/+0x013`: portrait and sex

Runtime bytes `0x218F2` and `0x218F3` are consumed together by portrait/presentation code. At physical `0x4AEA4..0x4AEBF` and again at `0x5771E..0x57750`, the executable computes an engine presentation index equivalent to:

```text
presentation_index = signed(portrait_index) + sex_code * 23
```

A status condition can select a second 46-entry range (`2 * 23`).

The identity of `+0x013` is independently closed by the character information screen. At physical `0x4B3D6` the executable loads runtime `0x218F3`, adds `0x1B`, and passes the result to the indexed `T` text reader:

```text
sex text T index = 0x1B + sex_code
```

The official manual describes the two creation choices as male and female, and both the supplied fixture and independently produced historical character-editor table agree on:

```text
sex_code 0 = Male
sex_code 1 = Female
```

Thus:

```c
uint8_t portrait_index; // +0x012
uint8_t sex_code;       // +0x013: 0 Male, 1 Female
```

The engine presentation bank contains 23 slots per sex even though the shipped character-creation UI exposes a smaller player-selectable face subset.

## `+0x014`: race code

The character information screen gives direct resource geometry. Physical `0x4B38F` loads runtime `0x218F4`, adds `0x0F`, and calls the common indexed `T` reader:

```text
race text T index = 0x0F + race_code
```

The official creation sequence lists the six race cards in the same numeric order, and the supplied fixture provides strong anchors (`EHLIANNA` code 5 in the documented Elven quick-start slot; `ANVIL DAWN` code 4 in the Dwarven slot). Official cluebook NPC descriptions independently confirm Human and Half-Elf fixture records. A historical community SAVE editor reports the same numeric table.

Canonical mapping:

```text
0  Human
1  Half-Elf
2  Halfling
3  Gnome
4  Dwarf
5  Elf
```

```c
uint8_t race_code; // +0x014
```

## `+0x015..+0x017`: class codes

The three bytes are signed class slots; `-1` marks an absent later class. The character information screen directly maps each nonnegative code to `T`:

```text
class text T index = 0x15 + class_code
```

Physical `0x4B101`, `0x4B14E`, and `0x4B1AF` perform the three lookups. The official creation sequence, quick-start party, cluebook NPC classes, and historical community editor all converge on:

```text
0  Fighter
1  Paladin
2  Ranger
3  Cleric
4  Mage
5  Thief
-1 absent class slot
```

Additional executable behavior reinforces the table: class 4 activates the scribeable spellbook family, class 3 activates the cleric-style preparation family, and class 1 has its own spell-capability path.

```c
int8_t class_code[3]; // +0x015..+0x017
```

## `+0x018..+0x019`: age

The fixture stores a little-endian 16-bit demographic value whose populated values include 25, 27, 30, 35, 36, 40, 45, 52, 66, 113, and 150. The values correlate with the certified race codes: the quick-start elf records carry 113/150, the dwarf carries 66, while Human/Half-Elf records occupy lower values.

A historical SAVE editor independently identifies the corresponding character field as Age and requires it when constructing a valid record. No contradictory executable use has been found.

```c
uint16_t age; // +0x018
```

Confidence is high from fixture/editor agreement, though unlike sex/race/class/alignment this field has not yet been tied to a direct literal-address `T` lookup.

## `+0x01A`: alignment code

Physical `0x4B35E` loads runtime `0x218FA`, adds `0x1D`, and passes the result to the common `T` reader:

```text
alignment text T index = 0x1D + alignment_code
```

STRAHD'S POSSESSION supports **six** player alignments, not the full nine-alignment AD&D grid. The official manual lists the six supported alignments; its rules-section order, the executable's contiguous text lookup, official cluebook NPC alignments, and the independently produced historical SAVE-editor table agree on the complete numeric mapping:

```text
0  Lawful Good
1  Lawful Neutral
2  Neutral Good
3  True Neutral
4  Chaotic Good
5  Chaotic Neutral
```

Fixture/cluebook anchors independently verify several entries: Velika, Anshelm, and Fhalken use code 4 and are Chaotic Good; Ryszard and Vladislav use code 5 and are Chaotic Neutral; Tatian uses code 2 and is Neutral Good in the published NPC data.

```c
uint8_t alignment_code; // +0x01A
```

Earlier wording that referred to "nine AD&D alignments" is incorrect for this game and is superseded.

## `+0x01B`: base Armor Class

Runtime signed byte `0x218FB` maps to record `+0x01B`. Defense calculation at physical `0x49EC4` loads it as its starting value before applying equipment, Dexterity, and conditional modifiers. Character initialization at physical `0x51AE7` writes literal decimal 10. Every populated quick-start record also stores `0x0A`.

```c
int8_t base_armor_class; // +0x01B; initialized to AC 10
```

This is base/unmodified Armor Class, not final effective AC.

## Hit points `+0x01C/+0x01E`

```c
int16_t current_hp; // +0x01C
int16_t max_hp;     // +0x01E
```

Damage subtracts from current HP; healing clamps against max HP; advancement can increase both. Fixture examples include MISCHA `1/32`, ANSHELM `43/51`, and FHALKEN `25/34`.

## Class levels and progression

```c
int16_t class_level[3];    // +0x020,+0x022,+0x024
int32_t class_progress[3]; // +0x026,+0x02A,+0x02E
```

The executable indexes both arrays in parallel with `class_code[3]`. Advancement adds to progression, compares against class-indexed threshold tables, and updates the corresponding level. Absent slots use `-1` in the supplied fixture.

## Ability-score region `+0x032..+0x03F`

The complete memory order is:

```text
+0x032/+0x033  Strength      base/current
+0x034/+0x035  Dexterity     base/current
+0x036/+0x037  Constitution  base/current
+0x038/+0x039  Intelligence  base/current
+0x03A/+0x03B  Wisdom        base/current
+0x03C/+0x03D  Charisma      base/current
+0x03E/+0x03F  exceptional Strength percentile base/current
```

### Executable anchors

- Physical `0x4B485..0x4B51C` displays six current ability bytes beginning at runtime `0x21913` and advances the source pointer by two for each ability; `0x4B555..0x4B5E0` does the parallel base-value display from `0x21912`.
- `+0x035` / `0x21915` is Dexterity current: defense and reversible Dexterity-effect paths consume it.
- `+0x036/+0x037` / `0x21916/0x21917` is Constitution base/current: severe-stat-loss logic compares current with base, decrements current, and can force HP to 1.
- `+0x039` / `0x21919` is Intelligence current: spellbook/scribing capacity indexes a table through it.
- `+0x03B` / `0x2191B` is Wisdom current: second-family spell preparation uses it for bonus-spell adjustment.
- Strength and exceptional Strength are independently certified by Strength adjustment, combat, and equipment logic.
- Charisma is the remaining sixth pair in the executable's six-score display sequence.

### Fixture/document cross-check

Published cluebook stat blocks reproduce the SAVE bytes exactly in this ordering. For example, Velika is stored as STR 14, DEX 18, CON 16, INT 13, WIS 12, CHA 16; Anshelm as STR 18/00, DEX 13, CON 16, INT 11, WIS 10, CHA 14; and Fhalken as STR 18/15, DEX 13, CON 14, INT 12, WIS 10, CHA 11. This independently verifies the memory ordering rather than deriving Charisma only by elimination.

The original manual lists the same six abilities for player-facing character creation, although its prose order differs from the internal SAVE layout.

## Inventory boundary

Runtime item storage begins at `0x21920`, exactly record `+0x040`, independently fixing the end of the ability region. Inventory contains 21 signed 16-bit entries through `+0x069`.

## Evidence hierarchy used for enum closure

The mappings above were not accepted from community material alone. Evidence was reconciled in this order:

1. CODE.2 field consumers and direct `T` lookup formulas;
2. supplied immutable SAVE.DAT fixture and quick-start description;
3. official STRAHD'S POSSESSION manual and cluebook character data;
4. historical community SAVE-editor tables, accepted only where they match the first three sources.

## Compatibility rule

Decode and encode the legacy record explicitly. Preserve raw enum numbers at the compatibility boundary even though their names are now mapped; do not renumber them in a byte-faithful importer/exporter.

## Remaining early/core targets

1. Catalogue the exact player-facing portrait resources behind `+0x012` and distinguish player-selectable faces from internal 23-entry bank capacity.
2. Identify any runtime consumer of `age` that provides stronger executable semantics than fixture/editor agreement.
3. Preserve residual status bits exactly; reopen their semantics only if a concrete producer or resource identity is found.