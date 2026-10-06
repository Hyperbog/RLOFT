# SAVE.DAT `0x248` character record

Status: **consolidated executable-certified reconstruction. The record is structurally bounded; remaining unknowns are isolated and must be preserved losslessly.**

The same `0x248`-byte legacy image is serialized for four active-party slots and ten saved-roster slots. Runtime party-record base is DGROUP `0x218D8`; addresses below are relative to `record_base + character_index * 0x248`.

## Current authoritative partition

```text
+0x000..+0x00F   fixed 16-byte name storage
+0x010..+0x017   identity/class metadata; partially unresolved
+0x018..+0x019   status word; high-byte disease/poison/paralysis bits certified
+0x01A..+0x025   core fields; partially resolved
+0x026..+0x031   three uint32 slots; 0xFFFFFFFF absent sentinel, semantics pending
+0x032..+0x039   four byte pairs; semantics pending
+0x03A/+0x03B   Strength base / current-effective
+0x03C/+0x03D   paired field; +0x03D Dexterity current-effective
+0x03E/+0x03F   paired field unresolved
+0x040..+0x045   three uint16 fields unresolved
+0x046/+0x047   exceptional-Strength base / current-effective percentile
+0x048..+0x071   int16 item[21]
+0x072..+0x0C1   spell matrix A: 8 x 10 packed cells; scribeable spellbook family
+0x0C2..+0x111   spell matrix B: 8 x 10 packed cells; second casting family
+0x112..+0x12A   unresolved persistent state
+0x12B..+0x12E   executable-accessed dword; semantics unresolved
+0x12F..+0x132   auxiliary dword; low byte participates in reversible Dexterity adjustment
+0x133..+0x136   uint32 item known/identified bits; bits 0..20 map item[0..20]
+0x137..+0x138   executable-accessed bytes; semantics unresolved
+0x139           class/casting capability selector/mask
+0x13A..+0x13D   uint32 primary active-effect bits
+0x13E..+0x1A5   uint32 primary_effect_expiry[26]
+0x1A6..+0x1B1   uint32 timed_slot_deadline[3]
+0x1B2..+0x1BD   uint32 timed_slot_aux[3]
+0x1BE..+0x1C1   Stone Giant Gauntlets displaced/effective Strength auxiliary
+0x1C2..+0x1C5   serialized dword; no direct CODE.2 reference found
+0x1C6..+0x1C9   reversible modifier for primary effect index 23
+0x1CA..+0x1CD   uint32 temporary-hand-effect bits
+0x1CE..+0x1F1   uint32 temporary_hand_effect_expiry[9]
+0x1F2..+0x1F5   consumable/additive persistent gameplay pool
+0x1F6..+0x1F9   recurring Strength-recovery deadline
+0x1FA..+0x1FD   accumulated state associated with primary effect index 6
+0x1FE..+0x247   reserved/unreferenced serialized tail in current executable audit
```

## Certified core fields

| Offset | Size | Meaning |
|---|---:|---|
| `+0x000` | `0x10` | character name storage |
| `+0x018` | 2 | status word |
| `+0x019` | 1 | condition bits: `0x04` disease, `0x08` poison, `0x10` paralysis |
| `+0x01D` | 1 | class-related discriminator |
| `+0x03A` | 1 | Strength base/reference |
| `+0x03B` | 1 | Strength current/effective |
| `+0x03D` | 1 | Dexterity current/effective |
| `+0x046` | 1 | exceptional Strength base percentile |
| `+0x047` | 1 | exceptional Strength current percentile |
| `+0x048` | `0x2A` | signed `int16_le item[21]` |

Direct executable consumers, rather than fixture visual pairing, control these assignments. Strength logic recognizes AD&D exceptional-percentile ranges `1..50`, `51..75`, `76..90`, `91..99`, and `100`.

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

Entries are signed 16-bit values. `-1 / 0xFFFF` is the ordinary empty sentinel, but other negative values occur and must be preserved.

## Dual spell-preparation matrices

Both matrices are 80-byte arrays with identical packed-cell geometry:

```text
cell = matrix[bank * 10 + entry]
bank  = 0..7       // displayed spell level = bank + 1
entry = 0..9

selected/planned = cell & 0x0F
ready/current    = cell >> 4
unavailable      = ((cell & 0x0F) == 0x0F)
```

A successful spell use decrements only the high nibble. Management paths separately account for selected and ready counts and enforce availability/capacity constraints.

Matrix A at `+0x072` is independently tied to the scribeable spellbook family. Matrix B at `+0x0C2` is the parallel second casting family. Resource expansion maps them to the indexed `T` archive as:

```text
matrix A T index = 0x42 + bank*10 + entry
matrix B T index = 0x92 + bank*10 + entry
```

See `SPELL_MATRIX.md`, `SPELL_FAMILIES.md`, and `SPELL_RESOURCE_LOOKUP.md` for executable evidence and the conservative family-label policy.

## Item known/identified state

`+0x133..+0x136` is a 32-bit bitset. Bits `0..20` correspond one-for-one to `item[0..20]`.

CODE.2 reads and writes a selected slot bit, transfers that bit with an item during inventory movement, clears it when the item leaves a slot, can bulk-mark all occupied slots, and passes the state into item rendering. This establishes persistent item-instance known/identified state rather than a generic character flag field.

Portable model:

```c
uint32_t item_known_bits;
```

When an item moves, its known bit must move with it.

## Primary timed effects

The field at `+0x13A` is not merely an undifferentiated flag dword. It heads a 26-entry timed-effect subsystem:

```c
uint32_t primary_effect_bits;       // +0x13A
uint32_t primary_effect_expiry[26]; // +0x13E
```

Bit `i` corresponds to `primary_effect_expiry[i]`. Positive timer values are absolute game-clock deadlines. Expiration clears/toggles the associated state and dispatches effect-specific cleanup.

The expiration handler maps effect indexes directly to `T` resource records:

```text
effect 0  -> T[588]
...
effect 25 -> T[613]
```

Several effects have additional persistent auxiliary fields later in the record. See `EFFECT_FLAGS.md`.

## Three paired timed slots

Immediately after the primary timer table are three independent paired states:

```c
uint32_t timed_slot_deadline[3]; // +0x1A6
uint32_t timed_slot_aux[3];      // +0x1B2
```

Each deadline is an absolute game-clock value paired with the auxiliary value at the same index. This subsystem is distinct from both effect tables.

## Stone Giant Gauntlets state

`+0x1BE` is directly tied to OBJECTS.DAT object `0x54`, `STONE GIANT GAUNTLETS`. Equipment-transition logic preserves displaced Strength here while the gauntlets force effective Strength to 20, then restores the saved value when appropriate.

This proves that equipment-derived state cannot always be reconstructed from inventory alone after load.

## Temporary spell-created hand effects

`+0x1CA` and `+0x1CE` form a second, semantically closed timer subsystem:

```c
uint32_t temporary_hand_effect_bits;
uint32_t temporary_hand_effect_expiry[9];
```

Exact index mapping:

```text
0  object 0x007E  CHILL TOUCH
1  object 0x007F  VAMPIRIC TOUCH
2  object 0x0080  CLAWS OF THE UMBER HULK
3  object 0x0081  CAUSE LIGHT WOUNDS
4  object 0x0082  CAUSE SERIOUS WOUNDS
5  object 0x0083  CAUSE CRITICAL WOUNDS
6  object 0x0084  HARM
7  object 0x0085  FLAME BLADE
8  object 0x0086  LICH TOUCH
```

Expiration removes the matching temporary object from either hand if it remains equipped. These nine states must not be merged with the primary 26-effect system.

## Strength recovery and terminal state

`+0x1F6` is an independent recurring Strength-recovery deadline. When due, CODE.2 restores one Strength point, compares current/effective Strength against base Strength, and schedules another recovery deadline when further restoration is required.

The final 74 bytes `+0x1FE..+0x247` have no direct CODE.2 references in the current exhaustive address audit and are zero in every populated character in the supplied quick-start SAVE. They are therefore classified conservatively as **reserved/unreferenced serialized tail**, not disposable padding. Preserve them exactly because indirect/computed access cannot be categorically excluded.

Likewise, `+0x1C2..+0x1C5` is serialized state with no direct reference found and must remain opaque.

## Empty-record representation

Unused active-party records are not zero-filled. In the supplied fixture most bytes are `0xFF`, while `+0x10..+0x11` are `00 80`. Occupancy must not be inferred with an all-zero test.

## Native-port rule

Do not memory-map the legacy image onto a modern packed C/C++ structure. Decode explicitly into typed native state, retain a raw/opaque representation for unresolved bytes, and serialize fields at their exact legacy offsets. In particular preserve:

- signed item IDs and unusual negative values;
- both spell matrices byte-for-byte;
- item known bits with item movement;
- all absolute effect deadlines;
- Stone Giant Gauntlets displaced Strength;
- temporary hand-effect timers;
- unresolved/reserved regions losslessly.

## Remaining semantic targets

The character record is now structurally bounded. Remaining work is semantic closure of isolated fields rather than discovery of major arrays:

1. `+0x010..+0x017` identity/class metadata;
2. `+0x01A..+0x045` unresolved core-stat/class fields, including the three sentinel dwords;
3. `+0x112..+0x12E` persistent post-spell state;
4. `+0x137..+0x138` auxiliary bytes;
5. exact gameplay identities for the primary 26 effect indexes and the three paired timed slots;
6. exact meanings of `+0x1C2`, `+0x1F2`, and `+0x1FA` where current behavior is structural but the player-facing name is not yet certified.

Supporting executable evidence is maintained in `POST_INVENTORY_STATE.md`, `EFFECT_FLAGS.md`, `TIMED_SLOTS.md`, and `CHARACTER_TAIL.md`.