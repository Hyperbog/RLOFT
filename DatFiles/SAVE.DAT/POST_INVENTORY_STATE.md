# SAVE.DAT character post-inventory state (`+0x06A..+0x19D`)

Status: **high-effort executable reconstruction normalized to corrected runtime character base DGROUP `0x218E0`.** Spell-family/class identities, shipped spell catalogues, and all 26 primary-effect identities are now cross-certified.

## Dual spell preparation matrices

```text
Mage matrix A:   +0x06A..+0x0B9 = runtime 0x2194A..0x21999
Priest matrix B: +0x0BA..+0x109 = runtime 0x2199A..0x219E9
cell = matrix + bank*10 + entry
```

Both are `8 x 10` packed cells. High nibble is ready/current count; low nibble is selected/planned count; low nibble `0xF` means unavailable.

Matrix A is the Mage/scribeable spellbook family. Matrix B is the shared Cleric/Paladin priest-style family. The shipped active-entry counts are:

```text
Mage:   [7, 6, 10, 4, 2, 6, 0, 0]
priest: [7, 6,  5, 4, 4, 2, 0, 0]
```

See `SPELL_CATALOGUE.md`, `SPELL_MATRIX.md`, and `SPELL_FAMILIES.md` for the exact T labels and executable table evidence.

## `+0x10A`: class/level-derived combat target byte

Runtime byte `0x219EA` maps to record `+0x10A`. Character setup computes this from class/level progression and returns `20 - progression`; later combat code reads it directly.

```c
uint8_t base_attack_target; // THAC0-style descending attack target
```

## `+0x10B..+0x112`: two hand/action ready deadlines

```c
uint32_t hand_ready_deadline[2]; // +0x10B, +0x10F
```

Maintenance loops exactly two entries, compares them with current gameplay time, clears matured deadlines, and marks the character UI dirty. Hand/item-use code blocks the corresponding hand while its value remains nonzero. Producers store `current_time + duration`.

## `+0x113..+0x116`: Vampiric Touch absorption pool

Runtime dword `0x219F3` maps to `+0x113`. In the object-use/combat path at physical `0x4988F..0x498A2`, object ID `0x7F` stores the resulting combat quantity here. OBJECTS.DAT object `0x7F` is `VAMPIRIC TOUCH`.

Incoming-damage processing at physical `0x47CE7..0x47D18` consumes this dword before remaining damage reaches HP.

```c
int32_t vampiric_touch_absorption_pool; // +0x113
```

## `+0x117..+0x11A`: Armor absorption pool

Runtime dword `0x219F7` maps to `+0x117`. Physical `0x45435..0x45454` initializes primary effect 0, obtains a generated protection quantity, adds 8, and stores it here. Incoming-damage processing consumes the pool; depletion clamps it to zero and clears primary effect bit 0.

Producer tracing identifies effect 0 as **Armor**, and raw `T[588]` independently decodes to `Armor`.

```c
int32_t armor_absorption_pool; // +0x117
```

## `+0x11B..+0x122`: reserved/unreferenced serialized subrange

Runtime `0x219FB..0x21A02` corresponds to these eight bytes. Direct-address audit found no reference in this interval while adjacent fields have direct references. Preserve the bytes exactly; computed/indirect access is not ruled out.

## `+0x123..+0x126`: Draw Upon Holy Might - STR reversible modifier

Runtime `0x21A03` maps to `+0x123`. Physical `0x458B9..0x45940` sets primary effect 12, schedules timer index 12, applies a Strength adjustment, and stores the actual adjustment here. Expiration at physical `0x43799` negates the stored value and applies the same Strength helper.

```c
int32_t draw_holy_might_strength_adjustment; // +0x123
```

## `+0x127..+0x12A`: Draw Upon Holy Might - DEX reversible modifier

Runtime `0x21A07` maps to `+0x127`. Physical `0x45A49..0x45AD7` sets primary effect 13, schedules timer index 13, stores the applied adjustment in this dword, and adds its low byte to current Dexterity at runtime `0x21915` / record `+0x035`. Expiration subtracts the same low byte.

```c
int32_t draw_holy_might_dexterity_adjustment; // +0x127
```

Preserve the full dword even though the immediate Dexterity transfer uses its low byte.

## `+0x12B..+0x12E`: item known/identified-state bits

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]` at `+0x040..+0x069`. CODE.2 moves the bit with an inventory item, clears it with the slot, can mark all occupied items, and passes the state to item rendering.

## Spell UI/casting bytes

```c
uint8_t selected_spell_bank;  // +0x12F, runtime 0x21A0F
uint8_t spell_list_scroll;    // +0x130, runtime 0x21A10
uint8_t casting_capabilities; // +0x131, runtime 0x21A11
```

`selected_spell_bank` is the zero-based spell-level bank. `spell_list_scroll` changes by six rows and resets to zero when the bank changes.

The casting byte is a class-derived mask:

```text
0x01  Cleric preparation capability (class code 3)
0x02  Mage spellbook capability      (class code 4)
0x04  Paladin priest capability       (class code 1)
```

Combinations are possible for multiclass records; do not treat the byte as a single-choice enum.

## `+0x132..+0x19D`: primary effects

```c
uint32_t primary_effect_bits;       // +0x132, runtime 0x21A12
uint32_t primary_effect_expiry[26]; // +0x136..+0x19D, runtime 0x21A16..0x21A7D
```

Bit `i` corresponds to timer `i`. Producer-side reconstruction identifies all 26 slots, and the exact expiration labels are directly decoded from `T[588..613]`.

CODE.2 constructs ordinary expiration presentation as:

```text
"%s's " + T[588 + effect_index] + " wears off."
```

See `PRIMARY_EFFECT_CATALOGUE.md`, `EFFECT_FLAGS.md`, and `T_EFFECT_TEXT.md`.

## Corrected reconstruction map

```text
+0x06A..+0x0B9   Mage spellbook matrix, 8 x 10 packed cells
+0x0BA..+0x109   Cleric/Paladin priest matrix, 8 x 10 packed cells
+0x10A           class/level-derived base attack target
+0x10B..+0x112   uint32 hand_ready_deadline[2]
+0x113..+0x116   Vampiric Touch damage-absorption pool
+0x117..+0x11A   Armor damage-absorption pool
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x126   Draw Upon Holy Might - STR reversible adjustment
+0x127..+0x12A   Draw Upon Holy Might - DEX reversible adjustment
+0x12B..+0x12E   item known/identified bits
+0x12F           selected spell bank
+0x130           spell-list scroll offset
+0x131           class/casting capability mask
+0x132..+0x135   primary active-effect bits
+0x136..+0x19D   primary effect expiry[26]
```

Later state begins at `+0x19E`.

## Native reconstruction rule

Use explicit legacy decoding. Preserve absolute hand-ready deadlines, both complete 80-byte spell matrices, all effect timers, and all reserved bytes exactly. Model `+0x113` and `+0x117` as consumable signed protection quantities rather than booleans. Preserve the `+0x131` capability mask as serialized; derive native class capabilities separately.