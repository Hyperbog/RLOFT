# OBJECTS.DAT category-dependent semantics

Status: **the shipped nontrivial roles of record byte `+0x31` are now closed as container-state and persistent charge-state selectors; word `+0x32` is behaviorally identified as an intrinsic magic/aura marker independent from numerical enchantment.**

This document extends `FORMAT.md`. Relevant fields are:

```text
+0x2F  uint8  item_category
+0x31  int8   state_index                  // category/object-family specific
+0x32  uint16 intrinsic_magic_marker
+0x37  uint8  enchantment_bonus
```

The immutable CODE.2 reconstruction remains controlling evidence. Corpus correlations are used only where they agree with executable consumers.

## Category families

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

CODE.2 item-presentation/equipment logic groups categories `2..4`, `5..8`, and `9..11` in several paths. The labels above describe supplied content plus those executable groupings; they are not claimed as original source-enum names.

## `+0x31`: mutable-state selector

The byte is read as signed throughout CODE.2. The common semantic principle is now clear: object definitions use it to select **mutable state stored outside OBJECTS.DAT**.

It must not be globally interpreted as one particular state type.

### Category 1: container-state index

For `item_category == 1`, CODE.2 multiplies `+0x31` by ten and indexes the runtime container descriptor table at DGROUP `0x7330`.

That descriptor holds slot count, UI layout, weight capacity, container subtype, a pointer to the actual contents array, and per-slot state bits. Burden/search routines recurse through those contents, and the transfer code updates them directly.

All 62 supplied category-1 object definitions have distinct state indexes. The observed referenced set is:

```text
0..60 except 61, plus 62
```

See `CONTAINER_STATE.md` for the exact 10-byte descriptor layout and subtype compatibility rules.

### Charge-bearing rods/wands: persistent charge-counter index

The same physical byte selects a different state pool for charged magic devices.

CODE.2 item-use handlers for object IDs `0x4A..0x4F` and duplicate wand definitions `0x14A..0x14F`:

1. load signed object byte `+0x31`;
2. index the byte vector at DGROUP `0x67EC`;
3. reject use if the selected byte is zero;
4. decrement the selected byte on successful use;
5. invoke the item depletion/removal path when the counter reaches zero.

The runtime vector is exactly **15 bytes**. In the certified supplied CODE.2 data image every byte initializes to `12`.

SAVE serialization independently proves persistence: the writer writes `0x0F` bytes from `0x67EC`, and the loader restores the same `0x0F` bytes.

The supplied item definitions map as follows:

```text
ID  74  ROD OF SMITING             -> charge slot 0
ID  75  WAND OF ENEMY DETECTION    -> charge slot 1
ID  76  WAND OF FIREBALLS           -> charge slot 2
ID 330  WAND OF FIREBALLS           -> charge slot 3
ID 331  WAND OF FIREBALLS           -> charge slot 4
ID 332  WAND OF FIREBALLS           -> charge slot 5
ID 333  WAND OF FIREBALLS           -> charge slot 6
ID  77  WAND OF FROST               -> charge slot 7
ID 334  WAND OF FROST               -> charge slot 8
ID 335  WAND OF FROST               -> charge slot 9
ID  78  WAND OF MAGIC MISSILES      -> charge slot 10
ID  79  WAND OF PARALYZATION        -> charge slot 11
```

ROD OF SMITING stores zero and therefore selects charge slot zero. The item-description code has an explicit object-ID `0x4A` exception so its charge value is displayed even though `+0x31 == 0`.

The otherwise-identical duplicate FIREBALL/FROST definitions differ at `+0x31`, giving separate physical wand definitions independent persistent charge counters.

Slots 12..14 exist in the serialized charge vector but are not selected by the supplied OBJECTS records.

### Remaining categories

Every supplied record in categories `2..15` has `+0x31 == 0`.

The only nonzero category-0 values are the eleven wand definitions listed above. Thus the safe generic physical name is `state_index`, with category/object-specific semantic views:

```text
category 1           -> container_state_index
charged device       -> charge_counter_index
other shipped object -> zero / raw state_index
```

## `+0x32`: intrinsic magic/aura marker

The earlier neutral `special_property_marker` label can now be tightened from direct executable behavior.

While global magic-reveal state at DGROUP `0x6DAE` is active, two independent item presentation paths test this same condition:

```text
enchantment_bonus != 0
    OR
word(+0x32) != 0
```

- `0F308..0F367` selects the highlighted item-draw path when the condition is true.
- `1EB39..1EB9B` assigns the corresponding highlighted render/palette state for equipped items under the same condition.

The `0x6DAE` value is maintained by a timed character-status/spell path. Regardless of the original spell name, the OBJECTS-side meaning is direct: `+0x32` causes the item to present as magical under the engine's magic-reveal mode even when its numerical enchantment bonus is zero.

The supplied database contains only:

```text
0 : 199 records
1 : 137 records
```

The corpus agrees with this interpretation:

- all rings (category 5) are marked;
- both category-6 magical accessories are marked;
- all three category-7 special bracer/gauntlet items are marked;
- all 66 spell-scroll variants are marked;
- named potions, wands, artifacts, holy symbols, magic stones, special weapons and BAG OF HOLDING are marked;
- ordinary `+1/+2` weapons and armor may leave `+0x32` zero because `enchantment_bonus` independently satisfies the same magic-reveal predicate.

The safe semantic name is therefore **`intrinsic_magic_marker`** or **`magic_aura_marker`**.

It is deliberately not flattened to `is_magic`: the executable models magic visibility as the OR of this serialized marker and the separate numerical enchantment field.

## `+0x37`: enchantment remains independent

`+0x37` is the numerical enchantment bonus used in combat/defense and item description formatting. A normal and important supplied combination is:

```text
intrinsic_magic_marker == 0
enchantment_bonus      >  0
```

A faithful object model must preserve both fields independently.

## Cross-file consequence for SAVE.DAT

`OBJECTS.DAT` stores **which charge state slot** a charged device uses. The current charge values live outside the object database and are serialized in SAVE.DAT.

For the supplied quick-start save, the 15 initialized charge bytes (`0x0C` each) occur uniquely at:

```text
SAVE.DAT +0x0D59 .. +0x0D67
```

This byte match is consistent with the certified serializer writing the 15-byte runtime vector at `0x67EC`.

The architectural split is therefore:

```text
OBJECTS.DAT  static definition -> charge_counter_index
SAVE.DAT     mutable state      -> current charge_counter[15]
```

## Safe implementation boundary

A high-level object model may expose:

```text
+0x31  state_index
        container_state_index for category 1
        charge_counter_index for proved charged devices

+0x32  intrinsic_magic_marker
+0x37  enchantment_bonus
```

while retaining the exact raw bytes. Do not merge duplicated object definitions solely because their visible names/graphics match, and do not merge the intrinsic-magic marker with the numerical enchantment byte.