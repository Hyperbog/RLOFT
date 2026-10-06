# OBJECTS.DAT format reconstruction

Status: **container and fixed 0x41-byte record layout proven; major rendering, inventory, combat, weight, damage, and range fields behaviorally decoded from certified CODE.2.**

## Controlling evidence

- `OBJECTS.DAT`: 21,844 bytes
- SHA-256: `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`
- CODE.2 loader: logical `168C8..16930`
- Runtime object table pointer: DGROUP `0x255D8`
- Runtime object count: DGROUP `0x258E8`

The immutable CODE.2 binary remains controlling evidence. Field names below are assigned only where consumers establish a stable behavior. Category-specific fields remain deliberately polymorphic.

## File layout

The file is exactly:

```text
+0x0000  uint32 object_count = 336
+0x0004  object_record objects[object_count]
```

Each record is exactly `0x41` (65) bytes.

The size equation is exact:

```text
4 + 336 * 0x41 = 21844
```

CODE.2 `168C8` independently proves this layout. It reads the dword count into `0x258E8`, computes `count * 0x41` as `(count << 6) + count`, allocates exactly that many bytes, stores the resulting pointer at `0x255D8`, and reads the complete record array.

## Proven record layout

```text
+0x00  char   name[40]                  // NUL-terminated inside fixed 40 bytes
+0x28  uint16 world_sprite_base
+0x2A  uint8  world_heading_stride
+0x2B  uint8  world_aux_selector        // consumed if nonzero; zero in supplied table
+0x2C  uint8  unresolved_2C             // zero in supplied table
+0x2D  uint16 ui_graphic_index
+0x2F  uint8  item_category
+0x30  uint8  weight_half_pounds
+0x31  int8   aux_index                 // meaning depends on item_category
+0x32  uint16 special_property_marker   // supplied values 0/1; exact concept not fully named
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

This layout accounts for all 65 bytes. It does **not** imply that every field is meaningful for every category.

## `+0x00`: fixed item display name

The first 40 bytes form a fixed NUL-padded name field. All 336 supplied records contain a NUL inside these 40 bytes; maximum supplied name length is 39 characters.

Sixteen records have an empty display name:

```text
7, 9, 11, 13, 15, 17, 19, 21, 23, 25,
94, 96, 237, 239, 263, 287
```

Empty names are valid records and must not be discarded.

## `+0x28/+0x2A`: world sprite frame selection

Family-C loose-item rendering at CODE.2 `2BE5E..2BEA8` indexes `OBJECTS.DAT` using the world record's object type, extracts a three-bit heading from the live 22-byte world record, and computes:

```text
world_sprite_frame = world_sprite_base
                   + heading_index * world_heading_stride
```

where `heading_index` is in `0..7`.

Thus `+0x28` is a 16-bit base world-sprite/resource index and `+0x2A` is its directional frame stride.

### `+0x2B`: auxiliary world-render selector

The same renderer tests byte `+0x2B`. If nonzero it passes the byte, together with the current cell index, into CODE.2 `2B088`.

All 336 supplied records store zero here, so the field is real executable input but its nonzero gameplay meaning cannot be learned from this corpus. Preserve it exactly.

### `+0x2C`

All 336 supplied records store zero at `+0x2C`. No stronger field identity is currently proved. It remains unresolved raw data.

## `+0x2D`: UI/inventory graphic index

Inventory/UI rendering indexes each object record and reads the **word** at `+0x2D` as the graphic/resource index passed to its item-slot rendering path.

The fact that multiple logical variants intentionally share this value corroborates the interpretation. For example the spell-specific mage-scroll records reuse the base mage-scroll UI graphic rather than assigning unique icons.

The field is unaligned and must remain a 16-bit little-endian value.

## `+0x2F`: item category

CODE.2 repeatedly dispatches on byte `+0x2F`. The semantic audit independently records a ten-entry item-type presentation dispatcher covering categories `2..11`, and container-weight logic explicitly treats category `1` as a container.

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

Corpus identities strongly associate the values with these broad groups:

```text
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

Category `0` contains miscellaneous/general items, magic consumables and devices. These labels describe the supplied content and CODE dispatch families; they are not claimed as original source-enum names.

## `+0x30`: weight in half-pound units

CODE.2 `0F5F8` recursively walks item/container lists and adds byte `+0x30` to the total weight. If the item is category 1 it treats signed byte `+0x31` as the contained-list index and recursively adds that container's contents.

The item-description path formats the resulting total by dividing by two and emits a half-unit suffix for odd values. Corpus values independently match expected object weights:

```text
LONG SWORD      8  -> 4 lb
LEATHER ARMOR  30  -> 15 lb
PLATE MAIL    100  -> 50 lb
SLING BULLET    1  -> 0.5 lb
```

`weight_half_pounds` is therefore a safe semantic name.

## `+0x31`: category-specific auxiliary index

CODE.2 reads this as a signed byte, but its meaning depends on the item class.

For category-1 containers, `0F5F8` uses it as the index of the container inventory/list to recurse into. All 62 supplied category-1 records have distinct auxiliary indices.

Other categories reuse the same physical byte for different state tables; wand/device consumers, for example, use it to index category-specific runtime state. It must **not** be globally renamed `container`, `charges`, or `spell`.

Safe schema name: `aux_index`.

## `+0x32`: special-property marker

CODE.2 tests the 16-bit word at `+0x32` in multiple item presentation/use paths. The supplied table contains only:

```text
0 : 199 records
1 : 137 records
```

Nonzero values correlate strongly with named/special consumables, artifacts and devices, but this is not simply an enchantment flag: ordinary `+1/+2` armor/weapon variants can have nonzero `enchantment_bonus` while this word remains zero.

It is therefore preserved/exposed conservatively as `special_property_marker` until all category-specific consumers are closed.

## `+0x34` and `+0x37`: armor and enchantment

Byte `+0x34` is consumed as a **signed** armor-class adjustment. Supplied examples include:

```text
shield         -1
leather armor  -2
chain mail     -5
bronze plate   -6
plate mail     -7
```

CODE.2's worn-item AC path combines this base adjustment with a category-dependent helper that applies byte `+0x37` as an additional negative defensive adjustment for wearable categories.

Byte `+0x37` is also rendered as a visible `+N` modifier in item descriptions and is used in weapon damage calculation. Supplied distribution:

```text
0 : 296
1 :  18
2 :  19
3 :   3
```

`enchantment_bonus` is therefore supported terminology for `+0x37`.

## `+0x35`: packed attack profile

The object table stores a 16-bit word at `+0x35`. Supplied values are all below 64:

```text
0, 4, 8, 12, 16, 17, 18, 21, 24, 25, 29, 30, 38, 42, 60
```

CODE.2 independently consumes two packed components:

```text
bits 0..1  attack_mode
bits 2..5  weapon_speed_factor
```

The low two bits participate in weapon-mode/compatibility dispatches. The speed factor is extracted in the attack-action scheduler; CODE.2 multiplies it by nine before passing it as the action timing duration to the timer/deadline routine.

Corpus values match classic weapon timing relationships: dagger speed 2, short sword 3, long sword 5, battle axe/mace 7, halberd 9, two-handed sword 10.

Bits above bit 5 are zero in the supplied table, but must still be preserved for arbitrary inputs.

## `+0x38..+0x3D`: two damage profiles

Combat code reads the first triplet and passes it to the common dice routine together with `enchantment_bonus`:

```text
+0x38 damage_sm_dice
+0x39 damage_sm_sides
+0x3A damage_sm_bonus
```

The second triplet is consumed as the alternate target-size damage profile:

```text
+0x3B damage_lg_dice
+0x3C damage_lg_sides
+0x3D damage_lg_bonus
```

The corpus matches AD&D weapon damage notation exactly. Examples:

```text
LONG SWORD         1d8       / 1d12
BROAD SWORD        2d4       / 1d6+1
TWO-HANDED SWORD   1d10      / 3d6
MACE               1d6+1     / 1d6
WARHAMMER          1d4+1     / 1d4
```

The `sm`/`lg` names describe the two target-size profiles used by the game; preserve raw bytes even for categories that do not use them.

## `+0x3E..+0x40`: three range bands

Seventeen supplied records contain nonzero values in the final three bytes. The values align with short/medium/long missile or thrown-weapon ranges:

```text
DAGGER               1 /  2 /  3
SPEAR                1 /  2 /  3
COMPOSITE BOW        6 / 12 / 21
SLING                5 / 10 / 20
ARROW                5 / 10 / 18
JAVELIN OF LIGHTNING 2 /  4 /  6
```

The ordering is therefore safely `range_short`, `range_medium`, `range_long`. The physical/gameplay unit is not assigned here.

## Corpus invariants and observations

For the supplied 336 records:

```text
world_aux_selector +0x2B = 0 in all records
unresolved +0x2C         = 0 in all records
special_property_marker  = only 0 or 1
maximum display-name length = 39
```

These are observations about this data set, not general format constraints.

## Cross-file relationship to LEVELS.DAT

LEVELS family B and family C both use `OBJECTS.DAT` type IDs:

- family B = placed object/item map records;
- family C = loose/pickable item world records, reused for dropped/thrown motion state.

The family-C renderer directly uses the `world_sprite_base` and `world_heading_stride` fields decoded above. Thus `OBJECTS.DAT` is the authoritative item/object definition database behind both persistent level placements and loose runtime items.

## Safe implementation boundary

A faithful implementation can expose every physical field above while preserving all raw bytes. Important restrictions:

- `+0x31` is polymorphic and must remain category-specific;
- `+0x32` must not be simplified to `is_magic`;
- `+0x2B` is executable-consumed despite being zero in the supplied table;
- `+0x2C` remains unresolved;
- low attack-profile bits are `attack_mode`, not an asserted handedness enum;
- range units remain unspecified;
- empty names are valid records.

`inspect_objects.py` provides a lossless parser/encoder and archive-wide validation.