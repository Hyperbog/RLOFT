# OBJECTS.DAT

Status: **fixed container/record layout proven; major rendering, UI, category, weight, defensive, attack, damage and range fields substantially reconstructed; lossless parse/encode round-trip verified.**

- Size: 21,844 bytes
- SHA-256: `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`
- Record count: **336**
- Record size: **0x41 / 65 bytes**
- CODE.2 loader: logical `168C8..16930`
- Runtime table pointer: DGROUP `0x255D8`
- Runtime count: DGROUP `0x258E8`

## Proven physical layout

```text
+0x0000  uint32 count = 336
+0x0004  count * 0x41-byte object records
```

Exact size equation:

```text
4 + 336 * 65 = 21,844
```

CODE.2 independently computes the same record-table length as `(count << 6) + count` before allocating and reading the database.

## Current record model

```text
+0x00  char   name[40]
+0x28  uint16 world_sprite_base
+0x2A  uint8  world_heading_stride
+0x2B  uint8  world_aux_selector
+0x2C  uint8  unresolved_2C
+0x2D  uint16 ui_graphic_index
+0x2F  uint8  item_category
+0x30  uint8  weight_half_pounds
+0x31  int8   aux_index                  // category-specific
+0x32  uint16 special_property_marker
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

`attack_profile` contains at least:

```text
bits 0..1  attack_mode
bits 2..5  weapon_speed_factor
```

The database uses category 1 for containers; CODE.2 recursively follows their signed `aux_index` to include contents in total carried weight. Weight units are half-pounds.

The family-C loose-item renderer proves the world-frame formula:

```text
world_sprite_frame = world_sprite_base
                   + heading_index * world_heading_stride
```

LEVELS family B and family C both address this database by object type ID, tying placed and loose world items directly to these definitions.

## Supplied-corpus checks

The current parser validates:

```text
records                    336
record size                0x41
parse -> encode roundtrip  PASS (byte-for-byte)
blank display names         16
max display-name length     39
world_aux_selector nonzero   0
unresolved +0x2C nonzero     0
nonzero range records       17
category-1 aux indexes      62 unique / 62 records
```

The supplied table uses categories `0..15` and enchantment values `0..3`.

## Documentation and tools

- `FORMAT.md` — record layout and field-level CODE.2 evidence.
- `inspect_objects.py` — lossless parser/encoder plus corpus audit.

## Current frontier

The fixed record schema is already sufficient for a faithful reader/editor model while retaining all bytes. Remaining high-value OBJECTS work is narrower:

1. resolve the nonzero semantics of `+0x2B` from its renderer callee even though the shipped table stores zero;
2. determine whether any executable path gives `+0x2C` a role;
3. subdivide `+0x31` by category without flattening its polymorphic meanings;
4. tighten the exact purpose of the `+0x32` special-property marker;
5. map the item-category values to engine-facing behaviors and named equipment slots where CODE.2 proves them.

Unknown or category-specific bytes must remain preserved verbatim.