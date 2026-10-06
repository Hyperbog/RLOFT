# SAVE.DAT character post-inventory state (`+0x06A..+0x19D`)

Status: **executable reconstruction normalized to the corrected runtime character base DGROUP `0x218E0`.** Earlier revisions used `0x218D8`, making runtime-derived serialized offsets eight bytes too high. Runtime addresses and executable behavior remain unchanged.

## Dual spell preparation matrices

```text
matrix A: +0x06A..+0x0B9 = runtime 0x2194A..0x21999
matrix B: +0x0BA..+0x109 = runtime 0x2199A..0x219E9
cell = matrix + bank*10 + entry
```

Both are 8 x 10 packed cells. High nibble is ready/current count; low nibble is selected/planned count; low nibble `0xF` means unavailable. Matrix A is the scribeable spellbook family; matrix B is the distinct second casting family.

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

Incoming-damage processing at physical `0x47CE7..0x47D18` consumes this dword before remaining damage reaches HP: the pool is reduced by incoming damage, or exhausted and subtracted from the incoming quantity.

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

Runtime `0x219FB..0x21A02` corresponds to these eight bytes. The direct-address CODE.2 audit found no reference in this interval while adjacent fields have direct references. Preserve the bytes exactly; this classification does not prove that computed/indirect access is impossible.

## `+0x123..+0x126`: reversible Strength modifier for primary effect index 12

Runtime `0x21A03` maps to `+0x123`. The producer sets primary effect bit 12, schedules timer 12, applies a positive Strength adjustment through the certified Strength helper, and stores the actual returned adjustment here. Expiration negates it and applies the same helper.

```c
int32_t primary_effect_12_strength_adjustment;
```

## `+0x127..+0x12A`: reversible Dexterity-related auxiliary dword

Runtime `0x21A07` maps to `+0x127`. One transition adds its low byte to certified Dexterity current/effective `+0x35`; companion cleanup subtracts the same low byte. Preserve the full dword.

## `+0x12B..+0x12E`: item known/identified-state bits

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]` at `+0x040..+0x069`. CODE.2 moves the bit with an inventory item, clears it with the slot, can mark all occupied items, and passes the state to item rendering.

## Spell UI/casting bytes

```c
uint8_t selected_spell_bank;  // +0x12F, runtime 0x21A0F
uint8_t spell_list_scroll;    // +0x130, runtime 0x21A10
uint8_t casting_capabilities; // +0x131, runtime 0x21A11
```

`selected_spell_bank` is the zero-based spell-level bank. Spell-list expansion multiplies it by ten and indexes both 8x10 matrices. `spell_list_scroll` changes by exactly six in the spell UI and resets to zero when the bank changes. The casting byte is initialized from class state and used as a selector/mask by spell setup and UI paths.

## `+0x132..+0x19D`: primary timed effects

```c
uint32_t primary_effect_bits;       // +0x132, runtime 0x21A12
uint32_t primary_effect_expiry[26]; // +0x136..+0x19D, runtime 0x21A16..0x21A7D
```

Bit `i` corresponds to timer `i`. Expiration-message resources map contiguously from `T[588]` through `T[613]` for effect indexes `0..25`.

## Corrected reconstruction map

```text
+0x06A..+0x0B9   spell matrix A, 8 x 10 packed cells
+0x0BA..+0x109   spell matrix B, 8 x 10 packed cells
+0x10A           class/level-derived base attack target
+0x10B..+0x112   uint32 hand_ready_deadline[2]
+0x113..+0x116   VAMPIRIC TOUCH damage-absorption pool
+0x117..+0x11A   primary-effect-0 damage-absorption pool
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x126   reversible Strength adjustment for primary effect 12
+0x127..+0x12A   reversible Dexterity-related auxiliary dword
+0x12B..+0x12E   item known/identified bits
+0x12F           selected spell bank
+0x130           spell-list scroll offset
+0x131           class/casting capability selector
+0x132..+0x135   primary active-effect bits
+0x136..+0x19D   primary effect expiry[26]
```

Later state begins at corrected offset `+0x19E`. See the focused timer/tail documentation, which must use the same `record_offset = runtime_address - 0x218E0` rule.

## Native reconstruction rule

Use explicit legacy decoding. Preserve absolute hand-ready deadlines and all reserved bytes exactly. Model `+0x113` and `+0x117` as consumable signed protection quantities, not booleans. Runtime addresses are the stable executable anchors; serialized offsets must always be derived from the corrected base `0x218E0`.