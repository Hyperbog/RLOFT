# SAVE.DAT character post-inventory state (`+0x072..+0x1A5`)

Status: **executable reconstruction. Both spell matrices, spell-UI persistence bytes, item-identification metadata, a reversible Strength effect modifier, and the primary timed-effect core are certified.**

## Dual spell preparation matrices

```text
matrix A: +0x072..+0x0C1 = runtime 0x2194A..0x21999
matrix B: +0x0C2..+0x111 = runtime 0x2199A..0x219E9
cell = matrix + bank*10 + entry
```

Both are 8 x 10 packed cells. High nibble is ready/current count; low nibble is selected/planned count; low nibble `0xF` means unavailable. Matrix A is the scribeable spellbook family; matrix B is the distinct second casting family.

## `+0x112..+0x12A`

This 25-byte region remains persistent serialized state with no single safe semantic partition yet. Preserve it losslessly.

## `+0x12B..+0x12E`: reversible Strength modifier for primary effect index 12

Runtime `0x21A03` maps to record `+0x12B`.

Producer physical `0x458B9..0x45940` supplies the decisive linkage:

- it sets primary effect bit 12 (`0x00001000`) by ORing `0x10` into byte runtime `0x21A13`;
- it computes and stores the corresponding effect deadline at runtime `0x21A46`, which is `+0x13E + 12*4`;
- it computes a positive Strength adjustment from character state;
- it calls the certified Strength-adjustment helper `0x4CF50`;
- it stores that helper's actual returned adjustment in runtime dword `0x21A03` / record `+0x12B`.

Expiration helper physical `0x43799..0x437AF`, reached for the matching primary effect index, reads `0x21A03`, negates it, and passes it back to `0x4CF50`, reversing the Strength change.

Thus the safe semantic identity is:

```c
int32_t primary_effect_12_strength_adjustment; // +0x12B
```

The player-facing spell/effect name remains pending the primary-effect resource catalogue. Do not substitute a guessed AD&D spell name.

## `+0x12F..+0x132`: reversible Dexterity-related auxiliary dword

Runtime `0x21A07` maps to record `+0x12F`. One transition adds its low byte to certified Dexterity current/effective `+0x3D`; companion expiration/cleanup physical `0x437B0` subtracts the same low byte. Preserve the full dword because fixture values use upper bytes even where consumers operate on the low byte.

## `+0x133..+0x136`: item known/identified-state bits

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]`.

CODE.2 reads/writes selected slot bits, moves the bit with an inventory item, clears it with the slot, can mark all occupied items, and passes the state to item rendering. Native code must preserve this as item-instance state.

## `+0x137`: persistent selected spell bank/level index

Runtime byte `0x21A0F` is persistent spell-interface state, not an unknown gameplay flag.

Evidence:

- physical `0x5E32E..0x5E34D` reads `+0x137` and passes it with the selected character to spell-list expansion routine `0x5E0E4`;
- `0x5E0E4` multiplies that argument by ten and uses it as the bank offset into **both** persistent 8x10 spell matrices;
- physical `0x5E413..0x5E451` derives a new bank from spell-interface selection coordinates, resets `+0x138`, writes the derived byte to `+0x137`, and rebuilds the spell list.

Because matrix bank `0` corresponds to displayed spell level 1, the portable interpretation is:

```c
uint8_t selected_spell_bank; // +0x137, zero-based spell-level bank
```

## `+0x138`: persistent six-row spell-list scroll offset

Runtime byte `0x21A10` is the list offset used by the spell interface.

Physical `0x5DF89..0x5E0AB` loads `+0x137` and `+0x138`, renders the selected bank, and displays at most six expanded spell/action rows. Navigation physical `0x5E3A4..0x5E3F4` changes `+0x138` by exactly six when paging through a list. Selecting a new bank at `0x5E43D` resets it to zero.

Thus:

```c
uint8_t spell_list_scroll; // +0x138; paging offset, stepped by 6
```

This is persistent UI/navigation state embedded in the character image.

## `+0x139`: class/casting capability selector

Runtime `0x21A11` is initialized from class-related bytes and used as a selector/mask by spell setup and UI paths. Setup ORs `0x04` for class code 1, `0x01` for class code 3, and `0x02` when any class byte equals code 4. Exact class labels remain governed by the separate class-ID reconstruction.

## `+0x13A..+0x1A5`: primary timed effects

```c
uint32_t primary_effect_bits;       // +0x13A
uint32_t primary_effect_expiry[26]; // +0x13E..+0x1A5
```

Bit `i` corresponds to timer `i`. Expiration processing iterates indexes `0..25`, compares nonzero positive deadlines against current game time, and dispatches effect-specific cleanup through the same index.

Expiration-message resources map contiguously:

```text
effect 0  -> T[588]
...
effect 25 -> T[613]
```

The exact player-facing identities should be assigned from those resources and producer traces rather than AD&D inference.

## Current reconstruction map

```text
+0x072..+0x0C1   spell matrix A, 8 x 10 packed cells
+0x0C2..+0x111   spell matrix B, 8 x 10 packed cells
+0x112..+0x12A   unresolved persistent state
+0x12B..+0x12E   reversible Strength adjustment for primary effect 12
+0x12F..+0x132   reversible Dexterity-related auxiliary dword
+0x133..+0x136   item known/identified bits
+0x137           selected spell bank / zero-based spell level
+0x138           spell-list scroll offset, six-row paging
+0x139           class/casting capability selector
+0x13A..+0x13D   primary active-effect bits
+0x13E..+0x1A5   primary effect expiry[26]
```

Later state beginning at `+0x1A6` is documented in `TIMED_SLOTS.md` and `CHARACTER_TAIL.md`.

## Native reconstruction rule

Do not discard `+0x137/+0x138` as incidental UI bytes: the original SAVE persists them as part of each character record. A compatibility loader should retain them, even if a modern UI elects not to use the original navigation model.

Preserve `+0x112..+0x12A` and all upper bytes of auxiliary dwords exactly until their remaining consumers are certified.

## Next target

Continue backward through `+0x112..+0x12A`, then use the primary-effect producer/expiration linkage to name effect index 12 and the neighboring reversible Dexterity effect without speculation.