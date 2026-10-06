# OBJECTS.DAT category-dependent semantics

Status: **the category split and the two shipped nontrivial roles of record byte `+0x31` are now resolved; word `+0x32` remains a deliberately conservative special-property marker rather than a generic enchantment flag.**

This document extends `FORMAT.md`. The fixed record size is `0x41` bytes and the relevant fields are:

```text
+0x2F  uint8 item_category
+0x31  int8  category_specific_index
+0x32  uint16 special_property_marker
+0x37  uint8 enchantment_bonus
```

The immutable CODE.2 reconstruction remains controlling evidence. Corpus correlations are used only where they agree with the executable's category-specific consumers.

## Category families

CODE.2 item-presentation/equipment logic dispatches categories `2..11` through a ten-entry table. The certified semantic audit preserves the grouped behaviors `2..4`, `5..8`, and `9..11` rather than flattening them into one generic item path.

The supplied database contains:

```text
 0 : 93   miscellaneous/devices/consumables
 1 : 62   containers
 2 :  7   shields
 3 : 13   helmets/headgear
 4 : 12   body armor
 5 :  6   rings
 6 :  2   phylactery/stone accessory family
 7 :  3   bracers/gauntlets
 8 :  6   robes/cloaks
 9 :  9   mace/staff/hammer weapon family
10 : 23   edged/pole weapon family
11 :  3   special weapon family
12 : 25   keys
13 :  3   stones/sling ammunition
14 : 66   spell-scroll variants
15 :  3   arrows
```

The labels above describe supplied content plus the executable's category groupings; they are not claimed as original source-enum names.

## `+0x31`: category-specific index

The byte is signed in CODE.2 and must remain signed in a faithful reconstruction. The supplied database shows exactly two nontrivial uses.

### Category 1: container inventory/list index

CODE.2 burden routine `0F5F8` treats category `1` specially. It reads signed byte `+0x31`, selects that container's stored item list, and recursively includes the contents when computing total carried weight.

All 62 supplied category-1 records have distinct indexes. The observed set is:

```text
0..60 except 61, plus 62
```

The physical object type is therefore not itself the container-content slot; `+0x31` provides the indirection.

### Category 0: wand/device runtime-state slot

The only nonzero category-0 values occur on exactly 11 wand definitions:

```text
object  75  WAND OF ENEMY DETECTION     -> 1
object  76  WAND OF FIREBALLS           -> 2
object 330  WAND OF FIREBALLS           -> 3
object 331  WAND OF FIREBALLS           -> 4
object 332  WAND OF FIREBALLS           -> 5
object 333  WAND OF FIREBALLS           -> 6
object  77  WAND OF FROST               -> 7
object 334  WAND OF FROST               -> 8
object 335  WAND OF FROST               -> 9
object  78  WAND OF MAGIC MISSILES      -> 10
object  79  WAND OF PARALYZATION        -> 11
```

Thus the shipped wand slots are exactly `1..11`.

This is not a spell/type discriminator. The duplicated FIREBALL definitions `76,330,331,332,333` are byte-identical except for byte `+0x31`; likewise FROST definitions `77,334,335` differ only at `+0x31`. CODE.2's item-description/use family preserves signed subtype/state indexing. The safe interpretation is therefore a **wand/device runtime-state slot index**, allowing otherwise identical object definitions to address independent mutable state.

The exact contents of the pointed-to runtime state (for example charge count versus other wand state) should be documented only when that state table's serializer/consumers are independently closed.

### All other categories

Every supplied record in categories `2..15` has `+0x31 == 0`.

Therefore `+0x31` must remain a polymorphic/category-specific field. It must not be globally renamed `container_index`, `wand_charges`, or `spell_id`.

## `+0x32`: special-property marker, not enchantment

CODE.2 tests the 16-bit word at `+0x32` in item presentation/use logic. The supplied database contains only:

```text
0 : 199 records
1 : 137 records
```

The marker is strongly associated with items requiring special identity/effect handling, but it is **not equivalent to magical/enhanted**.

Useful corpus contrasts:

- all categories 5, 6, 7, 11 and 14 have marker `1`;
- all category-12 keys have marker `0`;
- all category-9 blunt/staff weapons have marker `0`, including `+1/+2` variants;
- generic `+1/+2` shield, helmet, armor and edged-weapon variants frequently have nonzero `enchantment_bonus` while marker `+0x32` remains zero;
- named/special weapons such as `BA'AL VERZI DAGGER`, `ANCIENT ELVEN SWORD`, `PALADIN'S SWORD` and `PALADIN'S DAGGER` use marker `1`;
- all 66 spell-specific category-14 scroll records use marker `1`.

Consequently `special_property_marker` remains the safe schema name. A reader/editor should expose it independently from `enchantment_bonus` and preserve the full 16-bit word even though the supplied corpus uses only 0/1.

## `+0x37`: enchantment remains independent

The separate byte `+0x37` is the numerical enchantment bonus used by combat/defense and displayed as `+N` where appropriate. Its independence from `+0x32` is important:

```text
special_property_marker == 0, enchantment_bonus > 0
```

is a normal combination in the supplied database.

The two fields should therefore not be merged into one `magic` boolean.

## Safe implementation boundary

A high-level object model may now expose category-dependent views:

```text
category == 1:
    +0x31 -> container_contents_slot

category == 0 and +0x31 != 0:
    +0x31 -> wand_device_state_slot

otherwise:
    +0x31 -> raw category_specific_index
```

For `+0x32`, retain the neutral name `special_property_marker` until the remaining presentation/use predicates are semantically closed. Lossless encoding must always preserve the raw record bytes as the authority.