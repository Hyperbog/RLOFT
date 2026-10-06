# SAVE.DAT early/core character fields

Status: **advanced executable/fixture reconstruction using corrected runtime character base DGROUP `0x218E0`.** The early identity block and standard AD&D ability sequence are now substantially resolved.

## Runtime/file relationship

Character `n` begins at runtime `0x218E0 + n*0x248`. Serialized record offset is `runtime_address - 0x218E0` after removing the character stride.

## Current early/core partition

```text
+0x000..+0x00F  fixed 16-byte name storage
+0x010..+0x011  uint16 status_word
+0x012          portrait/appearance index within a 23-entry presentation bank
+0x013          binary presentation/demographic bank selector; exact player-facing label pending
+0x014          race/rules selector
+0x015..+0x017  int8 class_code[3]
+0x018..+0x019  uint16 age
+0x01A          alignment code
+0x01B          signed base armor class
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
+0x03E/+0x03F   exceptional Strength base/current percentile
+0x040..+0x069  int16 item[21]
```

## Status/activity word `+0x010`

Runtime `0x218F0` maps to record `+0x010`. Party/UI iteration uses the high bit as an inactive-record discriminator; unused active-party slots contain `0x8000` while populated fixture records contain zero in that bit.

```c
uint16_t status_word;
bool record_is_active = ((status_word & 0x8000u) == 0);
```

Known masks from executable consumers include disease `0x0400`, poison `0x0800`, and paralysis `0x1000`.

## `+0x012/+0x013`: presentation selectors

Runtime bytes `0x218F2` and `0x218F3` are consumed together by character presentation code. At physical `0x4AEA4..0x4AEBF` and again at `0x5771E..0x57750`, the executable computes a presentation index using:

```text
index = table_base + (+0x013 * 23) + signed(+0x012)
```

A status bit can select a second presentation range by adding `0x2E` (46 = 2 x 23).

The fixture keeps `+0x012` in the 23-entry index domain and `+0x013` at 0/1 for populated records. This certifies `+0x012` as the within-bank portrait/appearance index and `+0x013` as the binary presentation/demographic bank selector. The exact original player-facing label for the binary selector is intentionally left open until the associated UI/resource table is decoded.

## `+0x014`: race/rules selector

Runtime `0x218F4` is a small categorical value used throughout class, combat, spell, and character-rule paths. The supplied populated fixture uses values in the expected compact race-code domain. Several mechanics branch on specific values, including special handling for values 2 and 5.

Together with its placement immediately before `class_code[3]`, its broad rules consumers, and the fixture demographics, this establishes it as the character race/rules selector. Numeric-code-to-race-name mapping remains a separate catalogue target.

```c
uint8_t race_code; // +0x014
```

## `+0x015..+0x017`: class slots

```c
int8_t class_code[3];
```

The executable treats these as three signed class-code slots. Later slots use `-1` for absence. The fixture agrees for both single- and multiclass records.

## `+0x018..+0x019`: age

The populated fixture values decode as plausible demographic ages and correlate with the race selector: examples include 25, 27, 30, 35, 36, 40, 45, 52, 66, 113, and 150. This field is serialized as a little-endian 16-bit quantity.

```c
uint16_t age; // +0x018
```

The wide range is significant: long-lived nonhuman characters occupy the high values while human-like records remain in the lower range.

## `+0x01A`: alignment code

The fixture stores small categorical values here (notably 2, 4, and 5 in the quick-start population), in the exact core-character position expected for AD&D alignment state. Executable character/rules paths preserve and consume the byte as a categorical selector rather than a numeric statistic.

```c
uint8_t alignment_code; // +0x01A
```

Exact numeric-code-to-alignment-name mapping remains to be catalogued before assigning symbolic enum names.

## `+0x01B`: base armor class

Runtime signed byte `0x218FB` maps to record `+0x01B`. Combat-defense calculation at physical `0x49EC4` loads it as the starting value, then applies equipment-derived modifiers, a Dexterity-indexed modifier, and other conditional adjustments.

Character initialization writes literal decimal `10`; every populated quick-start record also stores `0x0A`.

```c
int8_t base_armor_class; // +0x01B; initialized to AD&D AC 10
```

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

The executable indexes both arrays in parallel with `class_code[3]`. Advancement adds to progression, compares against class-indexed thresholds, and updates the corresponding level. Absent slots use `-1` in the supplied fixture.

## Ability-score region `+0x032..+0x03F`

The complete standard AD&D ability ordering is now supported by both fixture geometry and independent mechanical consumers:

```text
+0x032/+0x033  Strength      base/current
+0x034/+0x035  Dexterity     base/current
+0x036/+0x037  Constitution  base/current
+0x038/+0x039  Intelligence  base/current
+0x03A/+0x03B  Wisdom        base/current
+0x03C/+0x03D  Charisma      base/current
+0x03E/+0x03F  exceptional Strength percentile base/current
```

Key executable anchors include:

- `+0x035` / runtime `0x21915`: Dexterity current, used by reversible Dexterity effects and defense-related mechanics.
- `+0x036/+0x037` / runtime `0x21916/0x21917`: Constitution base/current. A severe-stat-loss path compares current against base, decrements current, and can force HP to 1 as the gap approaches the engine's threshold.
- `+0x039` / runtime `0x21919`: Intelligence current. Spellbook/scribing code uses it to index the table that limits matrix-A spellbook capacity.
- `+0x03B` / runtime `0x2191B`: Wisdom current. Spell-preparation setup uses it to index the bonus-spell adjustment table for the second casting family.
- `+0x032/+0x033` and `+0x03E/+0x03F`: Strength and exceptional Strength are independently certified by Strength adjustment/equipment logic.

With those mechanical anchors and the contiguous base/current pairs, `+0x03C/+0x03D` closes as Charisma, the remaining standard AD&D ability in canonical sequence.

All populated quick-start records normally begin with equal base/current values in each pair; temporary effects modify the current member while preserving the reference/base member.

## Inventory boundary

Runtime item storage begins at `0x21920`, exactly record `+0x040`, independently fixing the end of the ability region. Inventory contains 21 signed 16-bit entries through `+0x069`.

## Compatibility rule

Decode and encode the legacy record explicitly. Preserve raw categorical numeric codes at the import boundary even where their semantic family is known; do not renumber race, alignment, class, or presentation indexes until their complete legacy enum mappings are certified.

## Remaining early/core targets

1. Decode the presentation resource/table behind `+0x012/+0x013` and assign the binary selector's exact original label.
2. Catalogue numeric `race_code` values at `+0x014` to player-facing race names.
3. Catalogue numeric `alignment_code` values at `+0x01A` to the nine AD&D alignments.
4. Catalogue `class_code[3]` numeric values to exact class names.
5. Audit remaining `status_word` bits beyond the already certified disease/poison/paralysis/inactive masks.