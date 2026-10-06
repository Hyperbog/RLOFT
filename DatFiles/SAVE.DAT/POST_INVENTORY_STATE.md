# SAVE.DAT character post-inventory state (`+0x06A..+0x19D`)

Status: **high-effort executable reconstruction normalized to corrected runtime character base DGROUP `0x218E0`.** Spell-family/class identities and primary effect 12/13 auxiliary fields are now cross-certified.

## Dual spell preparation matrices

```text
Mage matrix A:   +0x06A..+0x0B9 = runtime 0x2194A..0x21999
Priest matrix B: +0x0BA..+0x109 = runtime 0x2199A..0x219E9
cell = matrix + bank*10 + entry
```

Both are 8 x 10 packed cells. High nibble is ready/current count; low nibble is selected/planned count; low nibble `0xF` means unavailable.

Matrix A is the Mage/scribeable spellbook family. Matrix B is the shared Cleric/Paladin priest-style preparation family. Class initialization proves the ownership: class code 4 (Mage) activates matrix A; class code 3 (Cleric) and class code 1 (Paladin) invoke distinct setup helpers that both read/write matrix B.

## `+0x10A`: class/level-derived combat target byte

Runtime byte `0x219EA` maps to record `+0x10A`. Character setup computes this from class/level progression and returns `20 - progression`; later combat code reads it directly.

```c
uint8_t base_attack_target; // THAC0-style descending attack target
```

## `+0x10B..+0x112`: two hand/action ready deadlines

```c
uint32_t hand_ready_deadline[2]; // +0x10B, +0x10F
```

Maintenance loops exactly two entries, compares them with current game time, clears matured deadlines, and marks the character UI dirty. Hand/item-use code blocks the corresponding hand while its value remains nonzero. Producers store `current_time + duration`.

## `+0x113..+0x116`: VAMPIRIC TOUCH damage-absorption pool

Runtime dword `0x219F3` maps to `+0x113`. In the object-use/combat path at physical `0x4988F..0x498A2`, object ID `0x7F` stores the resulting combat quantity here. OBJECTS.DAT object `0x7F` is `VAMPIRIC TOUCH`.

Incoming-damage processing at physical `0x47CE7..0x47D18` consumes this dword before remaining damage reaches HP.

```c
int32_t vampiric_touch_absorption_pool; // +0x113
```

## `+0x117..+0x11A`: primary-effect-0 damage-absorption pool

Runtime dword `0x219F7` maps to `+0x117`. Physical `0x45435..0x45454` initializes primary effect timer slot 0, obtains a generated protection quantity, adds 8, and stores it here. Incoming-damage processing consumes the pool; depletion clamps it to zero and clears primary effect bit 0.

```c
int32_t primary_effect_0_absorption_pool; // +0x117
```

The player-facing effect name remains pending `T[588]`/producer catalogue closure.

## `+0x11B..+0x122`: reserved/unreferenced serialized subrange

Runtime `0x219FB..0x21A02` corresponds to these eight bytes. Direct-address audit found no reference in this interval while adjacent fields have direct references. Preserve the bytes exactly; computed/indirect access is not ruled out.

## `+0x123..+0x126`: primary-effect-12 reversible Strength modifier

Runtime `0x21A03` maps to `+0x123`. Physical `0x458B9..0x45940` sets overall primary-effect bit 12, schedules timer index 12, applies a Strength adjustment, and stores the actual returned adjustment here. Expiration at physical `0x43799` negates the stored value and applies the same Strength helper.

```c
int32_t primary_effect_12_strength_adjustment; // +0x123
```

## `+0x127..+0x12A`: primary-effect-13 reversible Dexterity modifier

Runtime `0x21A07` maps to `+0x127`. Physical `0x45A49..0x45AD7` sets overall primary-effect bit 13, schedules timer index 13, stores the applied adjustment in this dword, and adds its low byte to current Dexterity at runtime `0x21915` / record `+0x035`. Expiration subtracts the same low byte.

```c
int32_t primary_effect_13_dexterity_adjustment; // +0x127
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

## `+0x132..+0x19D`: primary timed effects

```c
uint32_t primary_effect_bits;       // +0x132, runtime 0x21A12
uint32_t primary_effect_expiry[26]; // +0x136..+0x19D, runtime 0x21A16..0x21A7D
```

Bit `i` corresponds to timer `i`. Expiration-message resources map contiguously from `T[588]` through `T[613]` for effect indexes `0..25`. See `EFFECT_FLAGS.md` for the re-audited index-specific cleanup dispatch.

## Corrected reconstruction map

```text
+0x06A..+0x0B9   Mage spellbook matrix, 8 x 10 packed cells
+0x0BA..+0x109   Cleric/Paladin priest matrix, 8 x 10 packed cells
+0x10A           class/level-derived base attack target
+0x10B..+0x112   uint32 hand_ready_deadline[2]
+0x113..+0x116   VAMPIRIC TOUCH damage-absorption pool
+0x117..+0x11A   primary-effect-0 damage-absorption pool
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x126   reversible Strength adjustment for primary effect 12
+0x127..+0x12A   reversible Dexterity adjustment for primary effect 13
+0x12B..+0x12E   item known/identified bits
+0x12F           selected spell bank
+0x130           spell-list scroll offset
+0x131           class/casting capability mask
+0x132..+0x135   primary active-effect bits
+0x136..+0x19D   primary effect expiry[26]
```

Later state begins at `+0x19E`.

## Native reconstruction rule

Use explicit legacy decoding. Preserve absolute hand-ready deadlines and all reserved bytes exactly. Model `+0x113` and `+0x117` as consumable signed protection quantities, not booleans. Preserve the `+0x131` capability mask as serialized; derive native class capabilities separately. Runtime addresses are stable executable anchors and serialized offsets use corrected base `0x218E0`.