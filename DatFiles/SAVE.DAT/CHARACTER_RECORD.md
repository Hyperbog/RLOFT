# SAVE.DAT `0x248` character record

Status: **high-effort audited, offset-normalized executable-certified reconstruction.** Runtime character base is DGROUP `0x218E0`; the same fixed image is serialized for four active-party slots and ten saved-roster slots. Runtime character `n` begins at `0x218E0 + n*0x248`.

## Direct serializer proof

The corrected base is now directly anchored by the CODE.2 save/load path:

```text
physical 0x41949  push 0x920
physical 0x4194E  push 0x218E0
                 write active-party image

physical 0x41CF5  push 0x920
physical 0x41CFA  push 0x218E0
                 read active-party image
```

`0x920 = 4 * 0x248`. Earlier revisions using base `0x218D8` are superseded; those runtime-derived serialized offsets were eight bytes too high.

## Current authoritative partition

```text
+0x000..+0x00F   fixed 16-byte name storage
+0x010..+0x011   uint16 status_word
+0x012           portrait/appearance index
+0x013           sex_code: 0 Male, 1 Female
+0x014           race_code
+0x015..+0x017   int8 class_code[3]; -1 absent later class
+0x018..+0x019   uint16 age
+0x01A           alignment_code
+0x01B           int8 base_armor_class
+0x01C..+0x01D   int16 current_hp
+0x01E..+0x01F   int16 max_hp
+0x020..+0x025   int16 class_level[3]
+0x026..+0x031   int32 class_progress[3]; -1 absent sentinel
+0x032/+0x033     Strength base/current
+0x034/+0x035     Dexterity base/current
+0x036/+0x037     Constitution base/current
+0x038/+0x039     Intelligence base/current
+0x03A/+0x03B     Wisdom base/current
+0x03C/+0x03D     Charisma base/current
+0x03E/+0x03F     exceptional Strength percentile base/current
+0x040..+0x069   int16 item[21]
+0x06A..+0x0B9   Mage spellbook matrix: 8 x 10 packed cells
+0x0BA..+0x109   Cleric/Paladin priest-style matrix: 8 x 10 packed cells
+0x10A           class/level-derived base attack target
+0x10B..+0x112   uint32 hand_ready_deadline[2]
+0x113..+0x116   Vampiric Touch absorption pool
+0x117..+0x11A   primary-effect-0 Armor protection/durability quantity
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x126   reversible Strength modifier for primary effect index 12
+0x127..+0x12A   auxiliary dword; low byte participates in reversible Dexterity adjustment
+0x12B..+0x12E   uint32 item_known_bits; bits 0..20 map item[0..20]
+0x12F           selected spell bank
+0x130           spell-list scroll position
+0x131           class/casting capability selector/mask
+0x132..+0x135   uint32 primary active-effect bits
+0x136..+0x19D   uint32 primary_effect_expiry[26]
+0x19E..+0x1A9   uint32 combat_feedback_deadline[3]
+0x1AA..+0x1B5   uint32 combat_feedback_damage[3]
+0x1B6..+0x1B9   Stone Giant Gauntlets displaced/effective Strength auxiliary
+0x1BA..+0x1BD   serialized dword; no direct CODE.2 reference found
+0x1BE..+0x1C1   reversible modifier for primary effect index 23
+0x1C2..+0x1C5   uint32 temporary_hand_effect_bits
+0x1C6..+0x1E9   uint32 temporary_hand_effect_expiry[9]
+0x1EA..+0x1ED   consumable/additive protection pool; primary effect 11 producer/expiry state
+0x1EE..+0x1F1   recurring Strength-recovery deadline
+0x1F2..+0x1F5   accumulated state associated with primary effect index 6
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

## Identity enums

The character information screen resolves sex, race, class, and alignment through contiguous indexes in the common `T` text archive:

```text
race      T index = 0x0F + race_code       // physical 0x4B38F
class     T index = 0x15 + class_code      // physical 0x4B101 etc.
sex       T index = 0x1B + sex_code        // physical 0x4B3D6
alignment T index = 0x1D + alignment_code  // physical 0x4B35E
```

Cross-checking CODE.2 with the supplied quick-start fixture, official manual/cluebook, and independently matching historical SAVE-editor tables gives:

```text
sex_code
0 Male
1 Female

race_code
0 Human
1 Half-Elf
2 Halfling
3 Gnome
4 Dwarf
5 Elf

class_code
0 Fighter
1 Paladin
2 Ranger
3 Cleric
4 Mage
5 Thief
-1 absent later slot

alignment_code
0 Lawful Good
1 Lawful Neutral
2 Neutral Good
3 True Neutral
4 Chaotic Good
5 Chaotic Neutral
```

STRAHD'S POSSESSION exposes six non-evil alignments; earlier generic references to nine AD&D alignment values do not describe this serialized enum.

Portrait rendering uses `portrait_index + sex_code*23`. CODE.1 character creation exposes exactly face indexes `0..16`; index 17 is explicitly disabled on the final selection page and 18..22 are not reached by the creation-page cycle. The supplied roster uses the upper valid slots for predefined characters: male 17..22 and female 17..20. Female slots 21..22 are invalid/reserved in RES1. Status `0x0100` selects the parallel 46-entry possessed portrait bank.

`+0x018` is executable-certified age. CODE.1 generation routine physical `0x2FA18..0x2FA6D` derives starting age from race using the shipped table `15+1d4, 15+1d6, 20+3d4, 60+3d12, 40+5d6, 100+5d6` for Human through Elf respectively. Physical `0x30112` later displays the stored word through the numeric character-information path.

## Record activity

The status word is runtime `0x218F0`, serialized `+0x010`. Party/UI iteration skips the populated-character path when the high bit is set:

```c
bool record_is_active = ((status_word & 0x8000u) == 0);
```

Unused fixture records contain `00 80` at `+0x010..+0x011`; an all-zero occupancy test is invalid. Certified masks include Vengeful-Spirit warning/coercion `0x0040`, recruited/template provenance `0x0080`, possession `0x0100`, Held `0x0200`, disease `0x0400`, poison `0x0800`, paralysis `0x1000`, death `0x4000`, and empty slot `0x8000`. Bit `0x2000` is an engine-recognized blocking tier without a proved producer/player-facing identity; low bits `0x0001..0x0020` remain raw/preserved.

## Armor Class and hit points

Runtime signed byte `0x218FB` / `+0x01B` is the base Armor Class. Defense calculation starts from it before applying equipment, Dexterity, and conditional modifiers; initialization writes literal `10`.

```c
int8_t  base_armor_class; // +0x01B
int16_t current_hp;       // +0x01C
int16_t max_hp;           // +0x01E
```

Damage subtracts from current HP and healing clamps against max HP. Non-full fixture records such as MISCHA `1/32`, ANSHELM `43/51`, and FHALKEN `25/34` independently confirm the current/max distinction.

## Class progression

```c
int16_t class_level[3];    // +0x020,+0x022,+0x024
int32_t class_progress[3]; // +0x026,+0x02A,+0x02E
```

The executable indexes these arrays in parallel with `class_code[3]`. Advancement adds to progression, compares against class-indexed thresholds, and updates the matching level. Absent slots use `-1` in the supplied fixture.

## Ability-score layout

Physical `0x4B485..0x4B51C` displays six current scores from runtime `0x21913`, advancing the source pointer by two for each score; `0x4B555..0x4B5E0` displays the parallel base values from `0x21912`. Mechanical consumers and published NPC stat blocks close the order:

```text
+0x032/+0x033  Strength      base/current
+0x034/+0x035  Dexterity     base/current
+0x036/+0x037  Constitution  base/current
+0x038/+0x039  Intelligence  base/current
+0x03A/+0x03B  Wisdom        base/current
+0x03C/+0x03D  Charisma      base/current
+0x03E/+0x03F  exceptional Strength percentile base/current
```

Key anchors: Dexterity affects defense and reversible effects; Constitution participates in severe stat-loss/HP behavior; Intelligence indexes spellbook capacity; Wisdom indexes second-family bonus preparation; Strength and exceptional Strength are independently consumed throughout combat/equipment logic. Official cluebook stats for multiple roster NPCs reproduce these serialized values exactly in this memory order.

## Inventory

```text
item[0..11]   ordinary inventory grid
item[12]      ring A
item[13]      ring B
item[14]      hand A
item[15]      hand B
item[16]      torso/body garment
item[17]      head
item[18]      dedicated accessory
item[19]      quiver
item[20]      bracer/gauntlet
```

Entries are signed 16-bit values. `-1 / 0xFFFF` is the ordinary empty sentinel, but other negative values must be preserved.

## Dual spell-preparation matrices

Both matrices are 80-byte arrays:

```text
cell = matrix[bank * 10 + entry]
bank  = 0..7
entry = 0..9
selected/planned = cell & 0x0F
ready/current    = cell >> 4
unavailable      = ((cell & 0x0F) == 0x0F)
```

Matrix A is `+0x06A`; matrix B is `+0x0BA`:

```text
matrix A T index = 0x42 + bank*10 + entry
matrix B T index = 0x92 + bank*10 + entry
```

## Post-spell combat and UI state

```c
uint8_t  base_attack_target;                    // +0x10A
uint32_t hand_ready_deadline[2];                // +0x10B
int32_t  vampiric_touch_absorption_pool;        // +0x113
int32_t  primary_effect_0_absorption_pool;      // +0x117
uint8_t  reserved_11B_122[8];                   // +0x11B
int32_t  primary_effect_12_strength_adjustment; // +0x123
uint32_t item_known_bits;                       // +0x12B
uint8_t  selected_spell_bank;                   // +0x12F
uint8_t  spell_list_scroll;                     // +0x130
uint8_t  casting_capabilities;                  // +0x131
```

The item-known bits `0..20` correspond one-for-one to `item[0..20]` and move with item instances.

## Timed state

```c
uint32_t primary_effect_bits;       // +0x132
uint32_t primary_effect_expiry[26]; // +0x136
uint32_t combat_feedback_deadline[3]; // +0x19E
uint32_t combat_feedback_damage[3];   // +0x1AA
```

Primary effect bit `i` corresponds to expiry `i`; expiration resources map to `T[588]..T[613]`. The three paired combat-feedback slots are separate from the primary-effect timer family; slots 0/1 hold recent outgoing hand damage and slot 2 holds recent incoming damage.

A high-effort re-audit corrected one earlier dispatch transcription: primary effect **11**, not 10, owns the `+0x1EA` pool cleanup. Effect 10 shares an external-state update with effect 1. Effect 12 uses the reversible Strength state at `+0x123`, effect 13 the Dexterity auxiliary at `+0x127`, and effect 23 the reversible modifier at `+0x1BE`. See `EFFECT_FLAGS.md`.

## Late character state

`+0x1B6` is tied to OBJECTS.DAT object `0x54`, `STONE GIANT GAUNTLETS`, preserving displaced Strength while the item forces effective Strength to 20.

```c
uint32_t temporary_hand_effect_bits;      // +0x1C2
uint32_t temporary_hand_effect_expiry[9]; // +0x1C6
```

Temporary-hand indexes map to objects `0x7E..0x86`: CHILL TOUCH, VAMPIRIC TOUCH, CLAWS OF THE UMBER HULK, CAUSE LIGHT WOUNDS, CAUSE SERIOUS WOUNDS, CAUSE CRITICAL WOUNDS, HARM, FLAME BLADE, and LICH TOUCH.

`+0x1EA` is a consumable/protection pool associated with primary effect 11; `+0x1EE` is the recurring Strength-recovery deadline; `+0x1F2` is accumulated state associated with primary effect 6.

## Terminal region

The final 82 bytes are bounded as:

```text
record  +0x1F6..+0x247
runtime 0x21AD6..0x21B27
```

A literal-address audit found no character-field references in this interval. The only literal collision, runtime `0x21B24`, occurs at physical `0x5CAA7` as a code jump-table base, not a character access.

Initialization provides stronger boundary evidence: physical `0x51A04..0x51A3A` computes `0x21B28 - 0x219EB` and zeroes `[0x219EB,0x21B28)`. Thus bytes through `0x21B27` are explicitly initialized as part of the character image, and `0x21B28 - 0x218E0 = 0x248` exactly.

All 12 populated quick-start records store zero at `+0x240..+0x247`; unused active-party records retain `0xFF` fill. The terminal region is therefore serialized reserved/unreferenced state, not disposable file padding. Preserve it exactly.

## Native-port rule

Do not memory-map the legacy image onto a modern compiler-native structure. Decode explicitly into typed native state and serialize at exact legacy offsets. Preserve opaque/reserved bytes verbatim and retain legacy enum numbers at the import/export boundary.

## Remaining semantic targets

The record's principal identity, status, portrait, class/progression, ability, inventory, spell-family, and primary-effect catalogues are closed. Remaining bounded questions are the exact gameplay identity of opaque `+0x1BA`, narrower source-level names for a few auxiliary effect quantities, and any residual status-bit producer discovered during final audit. These do not alter the certified `0x248` geometry or compatibility boundary.