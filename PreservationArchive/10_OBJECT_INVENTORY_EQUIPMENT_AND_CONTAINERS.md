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


## 38. Container state is external to OBJECTS.DAT

Category-1 objects do not store mutable contents in their 65-byte definitions.

Instead:

```text
OBJECTS +0x31
    -> signed container_state_index
    -> runtime descriptor table at 0x7330 + index*10
```

[Container state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)

Runtime descriptor:

```text
+0x00  uint8  slot_count
+0x01  uint8  ui_columns
+0x02  uint8  capacity_pounds
+0x03  uint8  ui_x_offset
+0x04  uint8  container_subtype
+0x05  uint32 contents_pointer
+0x09  uint8  per_slot_state_bits
```

The object definition identifies **which mutable container state** it uses.

## 39. Container contents are arrays of object IDs

Each container state owns:

```text
int16 object_id[slot_count]
```

with:

```text
-1 / 0xFFFF = empty slot
```

Recursive burden, search, UI rendering, insertion, and removal all dereference this live array.

Thus container contents are another location in which the same stable 16-bit OBJECTS IDs live.

## 40. Per-container slot-state bits are independently persistent

Descriptor byte `+0x09` is a bitset indexed by container slot.

CODE.2 has dedicated read/write helpers, and item-transfer paths move/update the corresponding bit when contents move.

The exact higher-level semantic identity of the bit remains unresolved.

It must therefore be modeled as:

```text
per_slot_state_bits
```

without inventing a universal `known`, `selected`, or `used` label.

## 41. Container subtype compatibility rules

Certified subtype values:

```text
0  POUCH
1  SACK / BAG OF HOLDING family
2  CHEST
3  SLING POUCH
4  IVORY SCROLL CASE
5  KEY RING
6  QUIVER
```

Specialized insertion gates:

```text
subtype 3 -> category 13  stone/sling ammunition
subtype 4 -> category 14  scrolls
subtype 5 -> category 12  keys
subtype 6 -> category 15  arrows
```

[Container state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)

Subtypes 0/1/2 use broader generic-container behavior.

## 42. Generic pouch/sack-style containers also consult hand-pair class

The subtype-0/1 insertion path rejects an inserted object when:

```text
object.attack_profile & 0x03 != 0
```

[Container state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)

Because the low two bits are now proved to be `hand_pair_class`, this is an actual interaction between weapon compatibility classification and generic container admission.

The exact original design label for this restriction remains unknown.

## 43. Container UI is a live nested inventory

Component 07 proved that open containers are rendered inside the selected character inventory column.

Click handler `11C3C` enforces:

- active container mode;
- subtype compatibility;
- target slot bounds;
- slot count;
- recursive/nesting restrictions;
- prospective burden/capacity.

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

Container legality is therefore enforced by engine interaction code, not merely by decorative UI layout.

## 44. Ordinary swap can auto-insert into a container

When:

```text
character slot already contains a compatible container
AND
cursor holds an item
```

ordinary inventory swap `117A0` can call the nested insertion helper.

On success:

```text
held item -> first compatible free container slot
cursor    -> empty
outer character slot remains the container object
```

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

This is original context-sensitive drag/drop behavior and should be explicit in a native interaction model.

## 45. Container state is SAVE-global, not embedded in characters

SAVE serialization writes descriptors/states `0..61`.

For each:

```text
2 * slot_count bytes of object IDs
1 byte per_slot_state_bits
```

Across those states:

```text
438 item slots * 2 bytes = 876
62 state bytes            =  62
                           ----
total                      = 938 bytes = 0x3AA
```

[Persistence chapter](https://github.com/Hyperbog/RLOFT/blob/6363f0af3d84d9559010ea38053e525af79c50ab/PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md)

A character carries a container object ID, but the container contents are global mutable state selected by that object's static definition.

## 46. Container definitions represent distinct state identities

All 62 supplied category-1 object definitions use distinct state indexes.

Therefore duplicate visible names such as:

```text
CHEST
POUCH
QUIVER
SCROLL CASE
```

can be separate persistent container instances.

They must not be deduplicated by name or artwork.

## 47. State-61 / selector-62 quiver mismatch is preserved

The final shipped quiver exposes a bounded static/runtime mismatch:

```text
OBJECTS ID 329 QUIVER
    state_index = 62

descriptor 61
    valid 8-slot subtype-6 quiver descriptor

descriptor 62
    ten zero bytes

SAVE
    persists only descriptor/state indexes 0..61

quick-start state 61
    contains eight arrow-family object IDs
```

[Container state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)

Direct path auditing found no executable:

```text
62 -> 61
```

repair.

Faithful behavior preserves:

- OBJECTS selector 62;
- zero descriptor 62;
- serialized state 61;
- absence of an automatic remap.

A later remaster may intentionally repair it only as a documented compatibility change.

## 48. Charged devices reuse the same physical state-index byte

For charged rods/wands, `OBJECTS +0x31` instead selects:

```text
uint8 charged_item_counter[15]
```

at runtime `0x67EC`.

[Category semantics](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

Use flow:

1. load charge-counter index from object definition;
2. read selected counter;
3. reject use when zero;
4. on successful use, decrement counter;
5. when the counter reaches zero, run depletion/removal handling.

SAVE serializes all 15 bytes.

## 49. Supplied charged-device map

```text
ID  74  ROD OF SMITING            -> slot 0
ID  75  WAND OF ENEMY DETECTION   -> slot 1
ID  76  WAND OF FIREBALLS          -> slot 2
ID 330  WAND OF FIREBALLS          -> slot 3
ID 331  WAND OF FIREBALLS          -> slot 4
ID 332  WAND OF FIREBALLS          -> slot 5
ID 333  WAND OF FIREBALLS          -> slot 6
ID  77  WAND OF FROST              -> slot 7
ID 334  WAND OF FROST              -> slot 8
ID 335  WAND OF FROST              -> slot 9
ID  78  WAND OF MAGIC MISSILES     -> slot 10
ID  79  WAND OF PARALYZATION       -> slot 11
```

Slots 12..14 exist in the persistent vector but are not selected by the supplied OBJECTS definitions.

[Category semantics](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

## 50. Rod of Smiting proves index zero is valid

OBJECTS ID 74 stores:

```text
state_index = 0
```

and therefore selects charge slot zero.

Item-description code has an explicit ID-74 exception so its charge state can be presented even though a generic:

```text
state_index != 0
```

test would miss it.

Thus zero is a valid charge-counter index, not universally “no state.”

## 51. Duplicate wand definitions intentionally carry separate charge state

The duplicate FIREBALL/FROST records are otherwise effectively identical for visible identity/behavior but differ at `+0x31`.

That difference gives each physical definition its own mutable charge counter.

Therefore:

```text
same name
same UI artwork
same effect
```

does **not** imply definitions can be merged.

Persistent state identity is encoded partly by duplicate record identity.

## 52. Consumable item effects feed the character effect system

Certified item-use examples:

```text
OBJECTS 61  POTION OF FIRE RESIST
    -> primary character effect 19

OBJECTS 62  POTION OF GIANT STRENGTH
    -> primary effect 23
    -> stores reversible Strength modifier at character +0x1BE

OBJECTS 64  POTION OF SPEED
    -> primary effect 24

OBJECTS 75  WAND OF ENEMY DETECTION
    -> primary effect 25
    -> shared enemy-detection counter
    -> consumes charge state
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

Object use therefore often installs state into the character record rather than mutating the static object definition.

## 53. Equipment can derive persistent effect bits

OBJECTS ID 103:

```text
SHIELD OF LIGHTNING PROTECTION
```

is checked by equipment-update code.

When qualifying equipment is present, character primary effect bit/index 22 is derived/set; when the qualifying shield is absent, that equipment-derived state is removed through its special path.

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

This is not an ordinary consumable timer.

The native equipment system must support:

```text
equip state
    -> derived character state

unequip state
    -> derived state removal/rebuild
```

## 54. Stone Giant Gauntlets have reversible equipment state

OBJECTS ID:

```text
0x54 / 84  STONE GIANT GAUNTLETS
```

uses character dword:

```text
+0x1B6
```

to preserve displaced/effective Strength while the gauntlets force effective Strength to 20.

[Character tail](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

The effect belongs to slot 20, the proved bracer/gauntlet equipment slot.

This is another reason equip/unequip cannot be implemented as a simple object-ID assignment.

## 55. Ring of Wizardry is slot-specific equipment state

The executable's Ring of Wizardry consumer checks only:

```text
slot 12
slot 13
```

for object ID `0x48`.

[Equipment slots](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)

This independently confirms that ring behavior is tied to the dedicated ring slots rather than merely carrying the item somewhere in ordinary inventory.

## 56. Scrolls are item objects that route into spell dispatch

Category 14 contains 66 spell-scroll variants.

Representative exact mappings:

```text
object 167  MAGE SCROLL OF 'ARMOR'
    -> Mage dispatch 0

object 171  MAGE SCROLL OF 'LIGHT'
    -> Mage dispatch 4

object 187  MAGE SCROLL OF 'INVISIBILITY 10 FOOT'
    -> Mage dispatch 27

object 202  CLERIC SCROLL OF 'BLESS'
    -> priest dispatch 80

object 218  CLERIC SCROLL OF 'PRAYER'
    -> priest dispatch 103

object 222  CLERIC SCROLL OF 'FORTIFY'
    -> priest dispatch 112
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

Thus the object-use layer can be a producer for the same legacy spell/action dispatch namespace used by prepared spells.

A scroll object ID is not itself the spell dispatch ID.

## 57. Item effects may create transient world-effect records

Charged devices independently reuse the class-3 transient/moving-effect system documented in Components 05 and 08.

Examples:

```text
WAND OF FIREBALLS
    -> class-3 effect type 0

WAND OF FROST
    -> class-3 effect type 3

WAND OF MAGIC MISSILES
    -> class-3 effect type 6
```

[Persistence global state](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Static object use can therefore produce:

```text
character effects
or
transient world effects
or
direct combat/action behavior
```

depending on the object.

## 58. Pickable world items are family-C instances

LEVELS family C is the loose/pickable item world family.

Its live record stores:

```text
world position
cached cell index
OBJECTS.DAT type ID
runtime/motion state
```

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

The first-person selection path resolves a rendered class-2 scene entry to a family-C index and calls pickup/removal routine `21704`.

That routine transfers the selected object type through the item/UI path, then compacts the family-C array.

## 59. Pickup removes the world instance

Family-C pickup:

1. identifies the selected live family-C record;
2. passes its object type and associated flag-derived state to the item/UI transition path;
3. removes the record;
4. memmoves later 22-byte records down;
5. decrements the family-C count.

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

The world instance is therefore consumed by pickup rather than remaining as a hidden duplicate.

## 60. Inventory can drop the held item into the world

While full inventory mode is active, logical key `0x44` checks the cursor-held item.

When one exists, the engine calls the family-C loose-item constructor with the party's current world coordinates, then clears/updates held-item UI state.

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

Thus:

```text
cursor-held item
    -> family-C world item
```

is an actual object-state transition.

## 61. Dropped items are not stored in a separate drop queue

Loose-item constructor `2195C..21ADA` appends directly to the same family-C array loaded from level state.

It stores:

- object type;
- fixed-point world position;
- cell index;
- initialized runtime state.

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

This means player drops become ordinary level-world item instances and are later serialized through standalone LEV state.

## 62. Family-C runtime ceiling is 300

The array limit is:

```text
0x12C = 300 records
```

When inserting while full, the engine discards the oldest record by shifting the remaining 299 records down before appending the new item.

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

This is observable ordered-array behavior and must not be replaced by an unbounded world-item container in strict compatibility mode.

## 63. Thrown items reuse family C

Thrown/moving items are not a separate object-instance type.

Routine `23B38` creates a family-C record with:

- object ID;
- world position;
- heading/direction state;
- motion/lifetime state;
- active-motion flag.

Routine `2388C` advances those records, collision-tests them, updates position/cell, and can end motion.

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

Recognizable type-sensitive paths include:

```text
AXE OF HURLING
ARROW OF SLAY UNDEAD
DAGGER OF THROWING
JAVELIN OF LIGHTNING
```

## 64. One legacy object-ID migration occurs before runtime item use

Compact LEVELS family-C type:

```text
0x54
```

is remapped on load to:

```text
0x58
```

In the current object table:

```text
0x54  STONE GIANT GAUNTLETS
0x58  ARROW OF SLAY UNDEAD
```

[Family C](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)

Every later OBJECTS lookup sees `0x58`.

This is a serialization-version migration in the level loader, not a global alias between those two current object definitions.

## 65. Placed family-B objects are a distinct world family

LEVELS family B also stores OBJECTS type IDs, but its role differs from family C.

Family B:

- represents placed map object/item records;
- uses source class 1 scene descriptors;
- participates in automap object overlay;
- has different serialized flags/state behavior.

Family C:

- represents loose/pickable instances;
- emits RES3 source class 2;
- can be inserted/removed at runtime;
- is reused for dropped/thrown items.

[World-state chapter](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)

Do not unify the two merely because both carry OBJECTS IDs.

## 66. Item-instance identity is location plus auxiliary state

The legacy architecture does not allocate one universal serialized item-instance record.

Instead, an item instance is represented by:

```text
object ID
+
current owner/location
+
location-specific mutable state
```

Examples:

### Character slot

```text
object_id
known bit
```

### Cursor

```text
object_id
known byte
```

### Container

```text
object_id
container-slot state bit
```

### Family C

```text
object_id
position
motion/state
```

### Charged device

```text
object definition's state_index
-> shared persistent charge byte
```

This distributed design must be understood before mapping the game to a modern entity/component representation.

## 67. Suggested native object model

A compatible modern architecture can expose:

```text
LegacyObjectDefinition
    exact 0x41-byte OBJECTS record

ObjectId
    stable uint16-compatible definition index

InventoryLocation
    character + slot

CursorItemState
    object ID
    known state

ContainerState
    descriptor
    item slots
    per-slot bits

WorldItemState
    family-C 22-byte-compatible state

PlacedObjectState
    family-B state

ChargeBank
    15 persistent counters

EquipmentDerivedState
    effects/attribute overrides derived from equipped IDs
```

These are proposed native abstractions, not recovered original source types.

## 68. Equip/unequip should be treated as a transaction

A strict equipment transition can require:

1. validate category/slot;
2. validate character-class restrictions;
3. validate hand-pair compatibility;
4. preserve/transplant known state;
5. remove old equipment-derived side effects;
6. install new object ID;
7. apply new equipment-derived side effects;
8. recompute defense/ability/presentation state;
9. update hand readiness/UI dirtiness;
10. rebuild cursor state for swapped-out item.

The exact original code spreads these consequences across helpers.

A native implementation should centralize them carefully while preserving original ordering where that ordering is observable.

## 69. Container insertion should be treated as a transaction

A faithful container insert must:

1. resolve static container object's `state_index`;
2. resolve the live descriptor;
3. validate subtype;
4. validate recursion/nesting rule;
5. calculate recursive burden;
6. compare against capacity;
7. find/validate target slot;
8. move object ID;
9. move/preserve slot-state bit;
10. update cursor/outer inventory state;
11. redraw affected UI.

Do not update the static OBJECTS record.

## 70. Required object/inventory regression classes

### 70.1 Database identity

Verify:

```text
size      21,844
count     336
stride    65
SHA-256   certified value
```

and byte-exact decode/encode.

### 70.2 UI resource linkage

For all 336 records:

```text
168 + ui_graphic_index
```

must resolve inside the certified RES1 object-UI namespace.

### 70.3 RES3 world linkage

For every supplied object definition and valid scene variant:

```text
3316 + local_shape_base + variant*shape_stride
```

must remain inside the certified class-2 RES3 bank.

### 70.4 Signed fields

Test negative:

- armor adjustment;
- signed state index where relevant.

Do not allow host unsigned promotion to change behavior.

### 70.5 21-slot character inventory

Round-trip all 21 slots including `-1` empties.

### 70.6 Known-state movement

Test:

```text
character -> cursor
cursor -> character
character slot -> another slot through cursor
```

and require known state to follow item identity.

### 70.7 Equipment routing

Test categories 3..8 and generic-hand categories against exact target slots.

### 70.8 Quiver exceptions

Test all thirteen hard-coded quiver IDs and at least one nonmatching category-1 object.

### 70.9 Armor restrictions

Test:

- single-class Mage rejection;
- class-code-5 whitelist acceptance;
- class-code-5 nonwhitelist rejection.

### 70.10 Hand-pair classes

Test classes 0, 1, and 2 in both hand positions.

Class 2 must invalidate the opposite hand.

### 70.11 Weapon speed

Verify attack-ready duration generation from:

```text
weapon_speed_factor * 9
```

at the item-side scheduling boundary.

### 70.12 Container subtype

Test each specialized subtype against allowed and disallowed item categories.

### 70.13 Container capacity

Test exact boundary:

```text
prospective == capacity*2
```

and one half-pound above.

### 70.14 Nested burden

Construct nested containers and verify recursive burden equals legacy results.

### 70.15 State-61 mismatch

Preserve:

```text
object 329 selector 62
descriptor 62 zero
state 61 separately serialized
```

without repair.

### 70.16 Charge bank

Test all supplied charge-slot mappings independently.

### 70.17 Duplicate charged definitions

Verify otherwise-identical duplicate wands consume independent counters.

### 70.18 Charge exhaustion

Test:

```text
counter 2 -> use -> 1
counter 1 -> use -> 0 + depletion path
counter 0 -> rejected use
```

### 70.19 Consumables

Verify item-produced primary effects and reversible auxiliaries for the proved potion cases.

### 70.20 Equipment-derived state

Equip/unequip:

- Shield of Lightning Protection;
- Stone Giant Gauntlets;
- Ring of Wizardry path where applicable.

### 70.21 Temporary hand items

Attempt ordinary swap on IDs `0x7E..0x86`; require rejection.

### 70.22 World pickup

Pick family-C item and verify:

- world count decreases;
- array compacts;
- selected type enters item/UI transition;
- remaining record order is preserved.

### 70.23 Drop

Drop cursor-held item and verify:

- family-C count increments;
- object type preserved;
- world position derives from party position;
- cursor state clears.

### 70.24 300-item limit

Fill family C to 300 and insert one more; require oldest-record eviction and ordered compaction.

### 70.25 Thrown state

Verify moving family-C item remains the same object ID and transitions back/out according to original motion handling.

## 71. Preservation/editor rules

A lossless object editor must preserve:

- all 65 bytes of all 336 records;
- blank names;
- duplicate definitions;
- raw `+0x2C`;
- latent `+0x2B`;
- attack-profile bits 6..15;
- distinct intrinsic-magic and enchantment fields;
- polymorphic `state_index`;
- exact object IDs.

A save/level editor must additionally preserve the mutable state pools that static definitions point into.

## 72. Do not infer unique item-instance IDs where the original has none

A modern engine may assign internal UUIDs for convenience, but compatibility serialization must not depend on them.

The original persistence model uses:

```text
stable definition ID
+
container/slot/world position
+
separate mutable-state banks
```

This is especially important for duplicated wands and containers, where duplicate **definition** records themselves encode state identity.

## 73. Correction C-13 — OBJECTS.DAT does not contain complete item state

Any architecture that treats one `OBJECTS.DAT` record as:

```text
complete item instance
```

is incorrect.

It is a static definition.

Mutable state may live in:

- character known bits;
- cursor state;
- container bank;
- charged-device vector;
- character effect auxiliaries;
- LEVELS/LEV world records.

The faithful port should model the static/dynamic split explicitly.

## 74. Correction C-14 — category does not fully determine equipment placement

Category provides the primary routing, but exact placement also depends on:

- ring-side position;
- hard-coded quiver object IDs;
- class-code armor gates;
- exact armor whitelist;
- hand-pair class;
- transient hand-item guards.

Therefore:

```text
slot = category_to_slot[category]
```

alone is insufficient.

## 75. Correction C-15 — thrown items are not a separate projectile database

Dropped, loose, pickable, and thrown physical items all converge on the family-C live world-record family.

Thrown state is runtime motion state layered over a normal loose-item OBJECTS identity.

This differs from class-3 spell/effect projectiles, which are a separate transient-effect family.

## 76. Historical unknowns retained

Remaining bounded item/object questions include:

1. original source-level names for hand-pair classes 0 and 1;
2. original source/editor name for `scene_influence_step`;
3. any historical consumer/name for `+0x2C`;
4. physical-world units for range bytes;
5. historical database provenance behind duplicated/revisioned object definitions;
6. exact original source name for container `per_slot_state_bits`;
7. intended design/source explanation of quiver selector 62 versus state 61;
8. exact original source taxonomy for item categories.

None blocks faithful parsing, inventory behavior, state persistence, or world-item reconstruction.

## 77. Source ledger

### OBJECTS.DAT

- [DatFiles/OBJECTS.DAT/README.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/README.md)
- [DatFiles/OBJECTS.DAT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FORMAT.md)
- [DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)
- [DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md)
- [DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)
- [DatFiles/OBJECTS.DAT/CONTAINER_STATE.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/CONTAINER_STATE.md)
- [DatFiles/OBJECTS.DAT/RENDER_AUX.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/RENDER_AUX.md)
- [DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/OBJECTS.DAT/FINAL_AUDIT_2026-10-06.md)

### Character/SAVE state

- [DatFiles/SAVE.DAT/PREFIX_GLOBALS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)
- [DatFiles/SAVE.DAT/GLOBAL_STATE.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/GLOBAL_STATE.md)
- [DatFiles/SAVE.DAT/CHARACTER_TAIL.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)
- [DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

### World/UI integration

- [DatFiles/LEVELS.DAT/FAMILY_C.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_C.md)
- [Component 05 world state](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)
- [Component 07 input/UI/inventory](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)
- [Component 08 persistence](https://github.com/Hyperbog/RLOFT/blob/6363f0af3d84d9559010ea38053e525af79c50ab/PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md)
- [Component 09 character model](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/PreservationArchive/09_PARTY_CHARACTER_GAMEPLAY_MODEL.md)

## 78. Architectural checkpoint

The object/inventory system is now:

```text
OBJECTS.DAT
    336 static definitions
          |
          v
       object ID
          |
          +------------------+-------------------+
          |                  |                   |
          v                  v                   v
   character item[21]     container slot      family-C world item
          |                  |                   |
          |                  |                   +-- drop
          |                  |                   +-- pickup
          |                  |                   +-- thrown/moving state
          |                  |
          |                  +-- subtype/capacity rules
          |                  +-- per-slot state bit
          |
          +-- ordinary inventory 0..11
          +-- equipment 12..20
          +-- known-state bit
          +-- hand-pair arbitration
          +-- equip/unequip effects
          |
          v
     cursor-held state
      object ID + known
          |
          +-- inventory swap
          +-- container insertion
          +-- world drop


OBJECTS +0x31
    |
    +-- category-1 container -> persistent container state
    |
    +-- charged device -> persistent 15-byte charge bank


item use
    |
    +-- character effect
    +-- spell dispatch
    +-- class-3 world effect
    +-- combat/action behavior
```

The next preservation component should reconstruct **NPC/creature definitions and live actor behavior**: the 83-record NPCS.DAT table, AI/runtime flags, perception and movement, facing/animation, action countdowns, attack behavior, immunities, sounds, special monster abilities, world-family-A state, and the exact boundary between creature template and live actor instance.
