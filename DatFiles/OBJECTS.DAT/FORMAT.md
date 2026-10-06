# OBJECTS.DAT format reconstruction

Status: **container and fixed 0x41-byte record layout proven; rendering, inventory, mutable-state indirection, magic-aura, defensive, attack, damage, weight and range fields substantially decoded from certified CODE.2.**

## Controlling evidence

- `OBJECTS.DAT`: 21,844 bytes
- SHA-256: `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`
- CODE.2 loader: logical `168C8..16930`
- runtime table pointer: DGROUP `0x255D8`
- runtime object count: DGROUP `0x258E8`

The immutable CODE.2 binary remains controlling evidence. Category-specific state remains polymorphic where the executable uses the same physical field for different mutable state pools.

## File layout

```text
+0x0000  uint32 object_count = 336
+0x0004  object_record objects[object_count]
```

Each record is exactly `0x41` / 65 bytes:

```text
4 + 336 * 0x41 = 21,844
```

CODE.2 independently proves this by reading the count, computing `(count << 6) + count`, allocating exactly that many bytes, storing the pointer at `0x255D8`, and reading the record array.

## Proven record layout

```text
+0x00  char   name[40]                  // NUL-terminated inside fixed 40 bytes
+0x28  uint16 world_sprite_base
+0x2A  uint8  world_heading_stride
+0x2B  uint8  world_aux_selector        // executable-consumed; zero in supplied table
+0x2C  uint8  unresolved_2C             // zero in supplied table
+0x2D  uint16 ui_graphic_index          // unaligned little-endian word
+0x2F  uint8  item_category
+0x30  uint8  weight_half_pounds
+0x31  int8   state_index               // category/object-family specific
+0x32  uint16 intrinsic_magic_marker
+0x34  int8   armor_class_adjustment
+0x35  uint16 attack_profile
+0x37  uint8  enchantment_bonus
+0x38  uint8  damage_sm_dice
+0x39  uint8  damage_sm_sides
+0x3A  uint8  damage_sm_bonus
+0x3B  uint8  damage_lg_dice
+0x3C  uint8  damage_lg_sides
+0x3D  uint8  damage_lg_bonus
+0x3E  uint8  range_short
+0x3F  uint8  range_medium
+0x40  uint8  range_long
```

This accounts for all 65 bytes. Not every field is meaningful for every item category.

## `+0x00`: display name

The first 40 bytes are a fixed NUL-padded display-name field. All 336 supplied records contain a NUL inside the field; the maximum supplied name length is 39 characters.

Sixteen records have an empty display name:

```text
7, 9, 11, 13, 15, 17, 19, 21, 23, 25,
94, 96, 237, 239, 263, 287
```

Empty names are valid records and must not be discarded.

## `+0x28/+0x2A`: world sprite selection

Family-C loose-item rendering indexes OBJECTS.DAT and computes:

```text
world_sprite_frame = world_sprite_base
                   + heading_index * world_heading_stride
```

where `heading_index` is the live world record's three-bit heading (`0..7`).

`+0x28` is therefore a 16-bit world sprite/resource base and `+0x2A` its directional frame stride.

### `+0x2B`: auxiliary world-render selector

The same renderer tests byte `+0x2B`; if nonzero it passes that byte together with the current cell index to CODE.2 `2B088`.

All supplied records store zero. The field is real executable input, but its nonzero gameplay identity is not recoverable from this corpus and must be preserved raw.

### `+0x2C`

All 336 supplied records store zero. No stronger executable identity is currently proved.

## `+0x2D`: UI/inventory graphic index

Inventory/UI rendering reads the **word** at `+0x2D` and passes it as the item's UI graphic/resource index. Logical variants intentionally reuse this value; spell-specific scroll definitions, for example, share base scroll artwork.

The field is unaligned and must remain a 16-bit little-endian value.

## `+0x2F`: item category

CODE.2 repeatedly dispatches on this byte. Category 1 is explicitly container-special, while several presentation/equipment paths group categories `2..4`, `5..8`, and `9..11`.

Supplied distribution:

```text
 0 : 93
 1 : 62
 2 :  7
 3 : 13
 4 : 12
 5 :  6
 6 :  2
 7 :  3
 8 :  6
 9 :  9
10 : 23
11 :  3
12 : 25
13 :  3
14 : 66
15 :  3
```

Safe content-level names are:

```text
0   miscellaneous/devices/consumables
1   containers
2   shields
3   helmets/headgear
4   body armor
5   rings
6   phylactery/stone accessory family
7   bracers/gauntlets
8   robe/cloak family
9   mace/staff/hammer weapon family
10  edged/pole weapon family
11  special weapon family
12  keys
13  stones/sling ammunition
14  spell-scroll variants
15  arrows
```

These are behavioral/content labels, not asserted original source-enum names.

## `+0x30`: weight in half-pounds

CODE.2 recursive burden logic adds byte `+0x30` for each item. Category-1 containers additionally recurse through their state-selected contents.

Item-description logic divides the accumulated value by two and prints a half-unit suffix for odd totals. Supplied values independently match expected weights:

```text
LONG SWORD      8  -> 4 lb
LEATHER ARMOR  30  -> 15 lb
PLATE MAIL    100  -> 50 lb
SLING BULLET    1  -> 0.5 lb
```

`weight_half_pounds` is therefore exact.

## `+0x31`: category/object-specific mutable-state index

This byte is read as signed. Its common role is to select mutable state stored outside the static OBJECTS record.

### Category 1 — container state

For containers, CODE.2 computes `state_index * 10` and indexes the runtime descriptor table at DGROUP `0x7330`. The 10-byte descriptor contains slot count, UI columns, capacity, subtype, contents pointer and per-slot state bits. All 62 supplied category-1 definitions have distinct state indexes.

See `CONTAINER_STATE.md` for the exact descriptor layout and specialized pouch/case/key-ring/quiver acceptance rules.

### Charged devices — persistent charge counter

For `ROD OF SMITING`, the wand definitions, and their duplicate physical wand records, the same byte selects the 15-byte charge-counter vector at DGROUP `0x67EC`. CODE.2 rejects use at zero, decrements on successful use and invokes depletion/removal when the selected counter reaches zero.

The supplied mappings are:

```text
74  ROD OF SMITING             -> 0
75  WAND OF ENEMY DETECTION    -> 1
76  WAND OF FIREBALLS           -> 2
330 WAND OF FIREBALLS           -> 3
331 WAND OF FIREBALLS           -> 4
332 WAND OF FIREBALLS           -> 5
333 WAND OF FIREBALLS           -> 6
77  WAND OF FROST               -> 7
334 WAND OF FROST               -> 8
335 WAND OF FROST               -> 9
78  WAND OF MAGIC MISSILES      -> 10
79  WAND OF PARALYZATION        -> 11
```

ROD OF SMITING uses slot zero despite storing zero in this byte; item-description code has an explicit object-ID `0x4A` exception so its charge count is displayed.

SAVE serialization writes and restores exactly 15 bytes from `0x67EC`, proving that the current counters are persistent save state rather than static OBJECTS data.

The generic schema name is therefore `state_index`; use semantic accessors such as `container_state_index` and `charge_counter_index` only when the object class proves them.

## `+0x32`: intrinsic magic/aura marker

This word participates directly in the engine's magic-reveal/highlight predicate.

While the runtime magic-reveal status is active, two independent presentation paths select the magical highlight state when:

```text
enchantment_bonus != 0
    OR
intrinsic_magic_marker != 0
```

Thus `+0x32` marks an intrinsic magical property/aura even when the numerical enchantment byte is zero.

Supplied distribution:

```text
0 : 199 records
1 : 137 records
```

The two serialized concepts are independent. Ordinary `+1/+2` weapon and armor variants may leave this word zero because their nonzero `enchantment_bonus` already satisfies the reveal predicate; rings, scrolls, wands and many named magical/special items use the intrinsic marker.

Do not flatten these fields into one `is_magic` boolean. See `CATEGORY_SEMANTICS.md`.

## `+0x34`: signed armor-class adjustment

This byte is consumed as signed defensive adjustment. Supplied examples include:

```text
shield         -1
leather armor  -2
chain mail     -5
bronze plate   -6
plate mail     -7
```

The worn-item AC path combines it with category-dependent enchantment handling.

## `+0x35`: packed attack profile

The supplied word values are all below 64. CODE.2 proves at least:

```text
bits 0..1  attack_mode
bits 2..5  weapon_speed_factor
```

The speed factor enters the attack-action scheduler; the low two bits participate in weapon-mode/compatibility logic. Do not rename them to handedness without further proof.

Corpus speed factors agree with expected weapon timing relationships: dagger 2, short sword 3, long sword 5, battle axe/mace 7, halberd 9, two-handed sword 10.

Bits above bit 5 are zero in the supplied table but must still be preserved.

## `+0x37`: enchantment bonus

The byte is used numerically in combat/defense and displayed as `+N` where appropriate.

Supplied distribution:

```text
0 : 296
1 :  18
2 :  19
3 :   3
```

It remains separate from `intrinsic_magic_marker` as described above.

## `+0x38..+0x3D`: two damage profiles

The first triplet is passed to the common dice routine during combat:

```text
+0x38 damage_sm_dice
+0x39 damage_sm_sides
+0x3A damage_sm_bonus
```

The second is the alternate target-size profile:

```text
+0x3B damage_lg_dice
+0x3C damage_lg_sides
+0x3D damage_lg_bonus
```

The corpus matches AD&D weapon notation, for example:

```text
LONG SWORD         1d8       / 1d12
BROAD SWORD        2d4       / 1d6+1
TWO-HANDED SWORD   1d10      / 3d6
MACE               1d6+1     / 1d6
WARHAMMER          1d4+1     / 1d4
```

## `+0x3E..+0x40`: range bands

Seventeen supplied records use nonzero final bytes. Their ordering is safely short/medium/long:

```text
DAGGER               1 /  2 /  3
SPEAR                1 /  2 /  3
COMPOSITE BOW        6 / 12 / 21
SLING                5 / 10 / 20
ARROW                5 / 10 / 18
JAVELIN OF LIGHTNING 2 /  4 /  6
```

The physical/gameplay unit remains unspecified.

## Cross-file relationships

LEVELS family B and family C both address OBJECTS.DAT by object type ID:

- family B = placed object/item map records;
- family C = loose/pickable item world records, also reused for dropped/thrown motion state.

SAVE.DAT persists mutable state selected by OBJECTS definitions, including the 15-byte charge-counter vector. OBJECTS.DAT therefore remains the static definition database; live charges/container contents belong to runtime/save state.

## Supplied-corpus invariants

```text
records                         336
world_aux_selector +0x2B          0 nonzero
unresolved +0x2C                  0 nonzero
intrinsic_magic_marker values     0 or 1 only
maximum display-name length      39
```

These are observations about the supplied file, not general format constraints.

## Safe implementation boundary

A faithful implementation may expose all fields above while preserving every raw byte. In particular:

- keep `+0x31` polymorphic as a mutable-state index;
- keep `+0x32` and `+0x37` as independent serialized magic/enchantment concepts;
- preserve executable-consumed `+0x2B` despite its all-zero supplied corpus;
- preserve unresolved `+0x2C`;
- keep attack-profile low bits behaviorally named;
- keep range units unspecified;
- retain empty-name records.

`inspect_objects.py` provides lossless parse/encode; `analyze_categories.py` validates the category/state/magic relationships.