# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 10 — Objects, Inventory, Equipment, Containers, and Item-State Architecture

**Status:** PRIMARY ITEM/INVENTORY RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the complete static/dynamic item model linking:

- `OBJECTS.DAT` definitions;
- character inventory and equipment slots;
- cursor-held item state;
- item-known/identified state;
- inventory UI placement;
- hand-pair compatibility;
- item weight and recursive burden;
- persistent containers;
- charged devices;
- consumables and equipment-derived effects;
- scroll/object-use dispatch;
- item pickup/drop;
- loose and thrown world items;
- resource linkage for inventory and scene rendering.

Combat attack resolution and spell execution are documented separately. This chapter records only the item-side state and dispatch boundaries that feed those systems.

The supplied `OBJECTS.DAT` was independently rechecked during this pass:

```text
size      21,844 bytes
records   336
stride    0x41 / 65 bytes
SHA-256   f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0
```

This exactly matches the certified archive. [OBJECTS README](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/README.md)

## 2. OBJECTS.DAT is the static object-definition database

File geometry:

```text
+0x0000  uint32 object_count = 336
+0x0004  ObjectRecord65 object[336]
```

Exact size equation:

```text
4 + 336 * 65 = 21,844
```

CODE.2 loader `168C8..16930` allocates exactly `count * 0x41` bytes and loads the records into runtime object-table storage referenced through DGROUP `0x255D8`. [OBJECTS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FORMAT.md)

Static definitions are immutable template data. Mutable instance state such as:

- container contents;
- wand charges;
- inventory-known state;
- cursor state;
- world position;
- thrown/moving state;

lives elsewhere.

## 3. Canonical 65-byte object record

```text
+0x00  char   name[40]
+0x28  uint16 local_shape_base
+0x2A  uint8  shape_stride
+0x2B  uint8  scene_influence_step
+0x2C  uint8  unresolved_2C
+0x2D  uint16 ui_graphic_index
+0x2F  uint8  item_category
+0x30  uint8  weight_half_pounds
+0x31  int8   state_index
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

[OBJECTS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FORMAT.md)

Every physical byte is structurally accounted for.

## 4. Name field is fixed-width and preservation-significant

The display name occupies 40 bytes.

All supplied records contain an internal NUL. The longest visible supplied name is 39 characters.

Sixteen valid object definitions have an empty display name.

Lossless tooling must retain the complete raw field and must not discard empty-name records as unused merely because their display string is blank.

## 5. Object IDs are stable indexes

The object ID used throughout CODE.2 is the zero-based record index:

```text
object_definition = object_table + object_id * 0x41
```

This ID is serialized directly in:

- character inventory words;
- cursor-held item state;
- container contents;
- LEVELS family B;
- LEVELS family C.

Object IDs are therefore part of the game's persistent ABI.

Do not renumber definitions to remove duplicates.

## 6. UI artwork and world artwork are separate namespaces

### UI artwork

`+0x2D` is the local inventory/UI graphic index.

INIT defines the object UI base:

```text
RES1 object UI base = 168
```

so:

```text
absolute_RES1_id = 168 + ui_graphic_index
```

All supplied values fall within the certified object-UI namespace. [OBJECTS final audit](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md)

### World artwork

Loose family-C scene instances use:

```text
variant_subrange_base =
    local_shape_base
  + scene_variant * shape_stride

absolute_RES3_id =
    3316 + variant_subrange_base
```

The common renderer may then select a view/state subframe inside that stride.

Thus an object definition can reference:

```text
one RES1 UI icon identity
one RES3 class-2 scene-shape namespace
```

through independent fields.

## 7. Scene variant is not universally heading

The live family-C state contains a three-bit selector used with `shape_stride`.

The completed renderer audit shows that this selector indexes a scene/state variant subrange; the common renderer may later choose a view-dependent frame inside the selected range.

Therefore:

```text
scene_variant
```

is safer than a universal `heading` label.

Normal family-C level expansion and dropped/thrown item creation initialize these variant bits to zero.

## 8. Latent scene influence field

`+0x2B` is executable-supported:

```text
scene_influence_step
```

When nonzero, family-C scene emission passes it to `2B088`.

The propagated footprint is:

```text
step 1   base map cell
step 2   base + eight immediate neighbors
step 3   base + immediate ring + outer 16-cell ring
```

[OBJECTS render auxiliary](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/RENDER_AUX.md)

All 336 supplied object definitions store zero, so this is latent engine-supported format capability.

## 9. Byte +0x2C remains raw

All 336 supplied records store:

```text
+0x2C = 0
```

No direct or aliasing OBJECTS-relative CODE.2 consumer was proved in the completed audit.

It remains:

```text
unresolved_2C
```

and must be preserved.

## 10. Item categories

All category values `0..15` occur in the supplied database:

```text
0   miscellaneous / devices / consumables
1   containers
2   shields
3   helmets / headgear
4   body armor
5   rings
6   phylactery / stone accessory family
7   bracers / gauntlets
8   robe / cloak family
9   mace / staff / hammer weapon family
10  edged / pole weapon family
11  special weapon family
12  keys
13  stones / sling ammunition
14  spell-scroll variants
15  arrows
```

[Category semantics](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

These are project behavioral/content labels, not claims about original source enum names.

## 11. Supplied category distribution

```text
category  0   93
category  1   62
category  2    7
category  3   13
category  4   12
category  5    6
category  6    2
category  7    3
category  8    6
category  9    9
category 10   23
category 11    3
category 12   25
category 13    3
category 14   66
category 15    3
```

CODE.2 also groups:

```text
2..4
5..8
9..11
```

in several equipment/presentation paths.

## 12. Weight is stored in half-pound units

`+0x30` is:

```text
uint8 weight_half_pounds
```

Examples:

```text
LONG SWORD       8 -> 4 lb
LEATHER ARMOR   30 -> 15 lb
PLATE MAIL     100 -> 50 lb
SLING BULLET     1 -> 0.5 lb
```

[OBJECTS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FORMAT.md)

The burden logic sums this byte recursively through container contents.

## 13. Mutable state selector is polymorphic

`+0x31` is a signed byte.

Its meaning depends on the object family:

```text
category 1 container
    -> container_state_index

charged device
    -> charge_counter_index

other supplied objects
    -> zero/raw state selector
```

[Category semantics](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

The physical field should therefore retain the generic compatibility name:

```text
state_index
```

with typed views layered above it.

## 14. Magic marker and enchantment are independent

The engine's magic-reveal predicate is:

```text
enchantment_bonus != 0
OR
intrinsic_magic_marker != 0
```

Thus:

```text
+0x32  intrinsic_magic_marker
+0x37  numerical enchantment_bonus
```

are separate state.

[Category semantics](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

A normal enchanted weapon can have:

```text
intrinsic_magic_marker = 0
enchantment_bonus      > 0
```

and still appear magical under detection.

Do not collapse both fields into one `is_magic` flag.

## 15. Armor adjustment is signed

`+0x34` is a signed Armor Class adjustment.

Representative values:

```text
shield         -1
leather armor  -2
chain mail     -5
bronze plate   -6
plate mail     -7
```

The later combat/defense chapter documents how these adjustments combine with character base AC, Dexterity, enchantment, and effects.

At the object boundary the requirement is:

```text
preserve signed int8 semantics
```

## 16. Packed attack profile

`+0x35` is:

```text
bits 0..1   hand_pair_class
bits 2..5   weapon_speed_factor
bits 6..15  preserved; zero in supplied database
```

[Attack profile](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)

The speed factor is extracted as:

```text
speed = (attack_profile >> 2) & 0x0F
```

and attack scheduling multiplies it by nine.

## 17. Weapon speed factor is encoded item timing

Representative supplied values:

```text
DAGGER                2
SHORT SWORD           3
QUARTERSTAFF          4
WARHAMMER             4
LONG SWORD            5
BROAD SWORD           6
SPEAR                  6
BATTLE AXE            7
MACE                   7
COMPOSITE BOW         7
HALBERD                9
TWO-HANDED SWORD     10
```

[Attack profile](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)

It is not a UI-only informational field; it feeds the hand/action deadline system documented in Component 09.

## 18. Hand-pair classes

Distribution across all 336 records:

```text
class 0   311
class 1    18
class 2     7
class 3     0
```

Class 2 is behaviorally proved:

```text
paired-hand-exclusive
```

If a qualifying item in one hand has class 2, the opposite hand is rejected.

[Attack profile](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)

Classes 0 and 1 remain distinct numeric pairable compatibility classes. Their original names are not proved.

## 19. Class-2 object set

All supplied class-2 definitions are:

```text
QUARTERSTAFF       three physical definitions
TWO-HANDED SWORD
HALBERD
COMPOSITE BOW
JAVELIN OF LIGHTNING
```

The presence of the bow and javelin means the safe behavioral label is:

```text
paired-hand-exclusive
```

rather than universally `two-handed melee`.

## 20. Damage profiles

```text
+0x38/+0x39/+0x3A
    small-target dice / sides / bonus

+0x3B/+0x3C/+0x3D
    large-target dice / sides / bonus
```

The first profile is directly passed through the common combat dice path.

Supplied values match familiar AD&D weapon forms such as:

```text
LONG SWORD
    small 1d8
    large 1d12

TWO-HANDED SWORD
    small 1d10
    large 3d6
```

The later combat chapter closes target-size selection and attack resolution.

## 21. Range bytes

```text
+0x3E short
+0x3F medium
+0x40 long
```

Examples:

```text
DAGGER                1 /  2 /  3
COMPOSITE BOW         6 / 12 / 21
SLING                 5 / 10 / 20
ARROW                 5 / 10 / 18
JAVELIN OF LIGHTNING  2 /  4 /  6
```

The behavior is range-band state, but the physical-world unit remains historically unresolved.

## 22. Character item array has 21 slots

Each `0x248` character record stores:

```text
+0x040..+0x069
    int16 item[21]
```

Empty slot sentinel:

```text
-1 / 0xFFFF
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

Slots `0..11` are ordinary inventory.

Slots `12..20` are equipment.

## 23. Ordinary inventory layout

The full inventory UI draws slots `0..11` as:

```text
3 rows
x
4 columns
=
12 slots
```

with encoded layout:

```text
row = 0x129 + (slot / 4) * 0x22
col = 5     + (slot % 4) * 0x12
```

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

These are not backpack entries in a separate structure; they are the first twelve words of the canonical character item array.

## 24. Equipment slot map

Certified equipment slots:

```text
12  ring_A
13  ring_B
14  hand_A
15  hand_B
16  torso/body garment
17  head
18  dedicated accessory
19  quiver
20  bracer/gauntlet
```

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

The names are behavioral/UI labels. Original enum names are not claimed.

## 25. Category-directed equipment placement

CODE.2 routing:

```text
category 3 -> slot 17

category 4 -> slot 16
              with class restrictions

category 5 -> slot 12 or 13

category 6 -> slot 18

category 7 -> slot 20

category 8 -> slot 16

all other categories
    -> generic hand path
       slot 14 or 15
       unless explicit object-ID reroute applies
```

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

A faithful UI cannot treat all visible equipment locations as generic drag targets.

## 26. Ring slots are independently addressed

Category 5 routes to:

```text
slot 12
or
slot 13
```

based on equipment-panel horizontal position.

Independent consumers, such as the Ring of Wizardry test, inspect only these two slots.

This is a true paired-ring inventory model.

## 27. Quiver slot is an executable item-ID exception

The generic hand-A route calls a hard-coded object predicate.

If it matches, placement is redirected:

```text
slot 14 -> slot 19
```

Every supplied matching ID is a category-1 `QUIVER`:

```text
6, 265, 270, 288, 295, 305, 308,
309, 315, 317, 321, 326, 327
```

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

Therefore slot 19 is safely the dedicated quiver slot.

This is an exact item-ID whitelist, not merely:

```text
if category == container
```

## 28. Body armor has encoded class restrictions

Category 4 normally targets slot 16.

Two additional gates exist.

### Single class code 4

If:

```text
class_code[0] == 4
class_code[1] == -1
```

the armor placement is rejected.

Class code 4 is independently certified as Mage.

### Any class code 5

If any of the three class slots contains code 5, ordinary body armor is rejected unless the object ID is in this exact whitelist:

```text
0x00C  LEATHER ARMOR
0x010  ELVEN CHAIN MAIL
0x05F  ANCIENT ELVEN PLATE MAIL
0x0EC  LEATHER ARMOR
0x0EE  LEATHER ARMOR
```

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

A faithful implementation should preserve the literal executable predicate rather than replacing it with a generalized inferred tabletop rule.

## 29. Temporary spell-hand objects are protected from normal swapping

Inventory swap refuses to move currently equipped IDs:

```text
0x7E  CHILL TOUCH
0x7F  VAMPIRIC TOUCH
0x80  CLAWS OF THE UMBER HULK
0x81  CAUSE LIGHT WOUNDS
0x82  CAUSE SERIOUS WOUNDS
0x83  CAUSE CRITICAL WOUNDS
0x84  HARM
0x85  FLAME BLADE
0x86  LICH TOUCH
```

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

These are temporary combat/spell pseudo-items managed by the character temporary-hand-effect subsystem.

They are not ordinary draggable possessions despite occupying hand slots.

## 30. Cursor-held item is authoritative state

Global held-item state:

```text
0x8600  int16 object_id
        -1 = empty

0x8602  uint8 known/identified state
```

[Persistence prefix globals](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)

The item cursor graphic is reconstructed from:

```text
OBJECTS[object_id].ui_graphic_index
```

when this semantic state changes.

The cursor graphic is presentation derived from the held object; it is not the held object's identity.

## 31. Character known-state is a separate bitset

Character record:

```text
+0x12B..+0x12E
uint32 item_known_bits
```

Low bits correspond to inventory slots `0..20`.

[Character state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/09_PARTY_CHARACTER_GAMEPLAY_MODEL.md)

Thus:

```text
item ID
known/identified state
```

are separate instance properties.

## 32. Inventory swap moves identity and known state together

Swap routine `117A0`:

1. reads the character slot object ID;
2. extracts that slot's known bit;
3. reads cursor-held object ID;
4. reads cursor-known state;
5. writes cursor object into the character slot;
6. writes cursor-known state into that slot's known bit;
7. moves the previous slot object to the cursor;
8. moves the previous slot's known state to the cursor.

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

Identification therefore follows the item instance through ordinary inventory movement.

## 33. Item description uses known state

Item-description routine accepts:

```text
(object_id, known_flag)
```

It always emits the base object name.

When:

```text
known_flag != 0
AND
enchantment_bonus != 0
```

it also reveals the numerical enchantment suffix/value.

[Persistence prefix globals](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)

Thus unknown magical equipment can retain its true static enchantment in `OBJECTS.DAT` while the UI deliberately withholds that information.

## 34. Static definition and item instance must remain separate

A robust native model should distinguish:

```text
ObjectDefinition
    immutable OBJECTS.DAT record

ItemReference / item instance location
    object_id
    known state
    mutable state indirection
    location/owner
```

There is no unique heap object allocated for every item instance in the legacy game.

Instead, item identity is often a 16-bit definition ID stored inside:

- character slot;
- cursor;
- container slot;
- world family-C record.

Mutable properties are distributed among those owners.

## 35. Weight calculation is recursive through containers

Burden logic:

1. reads an object's own `weight_half_pounds`;
2. if category 1, follows the object's container state selector;
3. iterates the live container contents;
4. recursively adds contained object burden.

[OBJECTS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FORMAT.md)

Thus a container's carried burden is not simply the static container-object weight.

A native implementation must guard its own host recursion safely while preserving legacy valid-state results.

## 36. Container capacity is measured in whole pounds

Each container descriptor stores:

```text
uint8 capacity_pounds
```

Insertion logic computes recursive burden in half-pound units, doubles this descriptor byte, and rejects insertion when:

```text
prospective_half_pounds
>
capacity_pounds * 2
```

[Container state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)

Representative capacities:

```text
POUCH            8 lb
SACK            30 lb
CHEST          100 lb
SLING POUCH    127 lb
SCROLL CASE    127 lb
KEY RING       127 lb
QUIVER         127 lb
BAG OF HOLDING 255 lb
```

## 37. Object-state ownership checkpoint

At this point the item architecture is:

```text
OBJECTS.DAT
    static definition
        |
        +-- name
        +-- category
        +-- graphics
        +-- weight
        +-- AC
        +-- attack profile
        +-- damage
        +-- range
        +-- magic/enchantment
        +-- state selector
        |
        v
object ID

object ID can live in:
    |
    +-- character item[21]
    +-- cursor-held item
    +-- container slot
    +-- family-B placed object record
    +-- family-C loose/moving object record

additional mutable state:
    |
    +-- known bit
    +-- container descriptor/content
    +-- charge counter
    +-- world position/motion
    +-- character effect auxiliary
```

The next half of this chapter closes those mutable systems and their use transitions.
