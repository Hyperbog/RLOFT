# SAVE.DAT early/core character fields

Status: **active executable reconstruction using corrected runtime character base DGROUP `0x218E0`.** The offset-normalization pass is complete for this region; semantic reconstruction is now proceeding field-by-field.

## Correct runtime/file relationship

Character `n` begins at runtime `0x218E0 + n*0x248`. Serialized record offset is therefore `runtime_address - 0x218E0` after removing the character stride.

## Current early/core partition

```text
+0x000..+0x00F  fixed 16-byte name storage
+0x010..+0x011  uint16 status_word
+0x012          signed demographic/appearance selector A; exact identity pending
+0x013          unsigned demographic/appearance selector B; exact identity pending
+0x014          unsigned identity/rules selector; exact identity pending
+0x015..+0x017  int8 class_code[3]
+0x018..+0x019  unresolved uint16/core value
+0x01A          unresolved byte
+0x01B          signed base armor-class byte
+0x01C..+0x01D  int16 current_hp
+0x01E..+0x01F  int16 max_hp
+0x020..+0x025  int16 class_level[3]
+0x026..+0x031  int32 class_progress[3]
+0x032/+0x033   Strength base/current
+0x034/+0x035   paired ability; current byte +0x035 is Dexterity
+0x036..+0x03D  four further paired ability bytes; exact identities pending
+0x03E/+0x03F   exceptional Strength base/current percentile
+0x040..+0x069  int16 item[21]
```

## Status/activity word `+0x010`

Runtime `0x218F0` maps to record `+0x010`. Party/UI iteration uses the high bit as an inactive-record discriminator; unused active-party slots contain `0x8000` while populated fixture records contain zero in that bit.

```c
uint16_t status_word;
bool record_is_active = ((status_word & 0x8000u) == 0);
```

Known masks from executable consumers include disease `0x0400`, poison `0x0800`, and paralysis `0x1000`. Other bits remain under audit.

## `+0x012/+0x013`: paired demographic/appearance selectors

Runtime bytes `0x218F2` and `0x218F3` are consumed together by character presentation code. At physical `0x4AEA4..0x4AEBF`, `+0x013` is multiplied by `0x17` (23), then `+0x012` is added to the resulting table-derived base. A status bit can select a second presentation range.

The fixture shows `+0x013` only as 0 or 1 across the populated quick-start characters, while `+0x012` varies over a broader small integer range. This is consistent with demographic/appearance indexing, but the exact source labels (for example sex/gender versus another binary selector, and race/portrait versus another categorical selector) are not yet promoted without a direct UI/resource cross-link.

## `+0x014`: identity/rules selector

Runtime `0x218F4` has numerous independent consumers across combat, spell, character, and UI paths. Fixture values are small (`0`, `1`, `5` in the audited quick-start population). Its exact player-facing identity remains pending caller/table reconstruction; it must not be folded into the three class-code bytes beginning at `+0x015`.

## `+0x015..+0x017`: class slots

```c
int8_t class_code[3];
```

The executable treats these as three signed class-code slots. Later slots use `-1` for absence. The fixture agrees for both single- and multiclass records.

## `+0x018..+0x01A`: unresolved core state

The fixture contains a nontrivial 16-bit value at `+0x018/+0x019` and a small byte at `+0x01A`. No canonical semantic label is assigned yet. These bytes are preserved exactly while their producers/consumers are traced.

## `+0x01B`: base armor class

Runtime signed byte `0x218FB` maps to record `+0x01B`. Combat-defense calculation at physical `0x49EC4` loads it as the starting value, then adds equipment-derived modifiers, a Dexterity-indexed modifier, and other conditional adjustments before returning the effective defense value.

Character initialization at physical `0x51AE7` writes literal decimal `10` to this byte. Every populated quick-start record also stores `0x0A` here.

This establishes:

```c
int8_t base_armor_class; // +0x01B; initialized to AD&D AC 10
```

It is the unmodified/base armor-class starting value, not the final equipment-adjusted AC.

## Hit points `+0x01C/+0x01E`

```c
int16_t current_hp; // +0x01C
int16_t max_hp;     // +0x01E
```

Damage subtracts from current HP; healing clamps current HP against max HP; advancement can increase both. Fixture examples include MISCHA `1/32`, ANSHELM `43/51`, and FHALKEN `25/34`, proving the fields are live current/max state rather than duplicated constants.

## Class levels and progression

```c
int16_t class_level[3];   // +0x020,+0x022,+0x024
int32_t class_progress[3]; // +0x026,+0x02A,+0x02E
```

The executable indexes both arrays in parallel with `class_code[3]`. Advancement adds to the progression dword, scales it against class-indexed threshold tables, and updates the corresponding level. Absent slots use `-1` in the supplied fixture.

## Ability region

Corrected runtime mapping proves:

```text
+0x032  Strength base/reference       runtime 0x21912
+0x033  Strength current/effective    runtime 0x21913
+0x035  Dexterity current/effective   runtime 0x21915
+0x03E  exceptional Strength base     runtime 0x2191E
+0x03F  exceptional Strength current  runtime 0x2191F
```

The fixture strongly supports a base/current pair layout throughout `+0x032..+0x03F`: populated records normally contain equal adjacent values. The exact identities of `+0x034`, `+0x036..+0x03D` are retained as open targets until their executable consumers certify the AD&D ability ordering.

The exceptional-Strength pair is independently supported by Strength logic that recognizes percentile bands `1..50`, `51..75`, `76..90`, `91..99`, and `100`.

## Inventory boundary

Runtime item storage begins at `0x21920`, exactly record `+0x040`, providing an independent boundary for the core/ability region. The inventory is 21 signed 16-bit entries through `+0x069`.

## Compatibility rule

Decode and encode the legacy record explicitly. Do not memory-map it to a compiler-native structure. Preserve unresolved bytes verbatim and do not promote categorical identities from fixture pattern alone.

## Next targets

1. Resolve `+0x012/+0x013` through their 23-entry presentation table and resource consumers.
2. Resolve `+0x014` from its combat/spell/UI table lookups.
3. Identify `+0x018..+0x01A` from producer paths.
4. Certify the remaining ability pairs `+0x034..+0x03D` from their mechanical consumers.