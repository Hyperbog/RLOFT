# OBJECTS.DAT equipment-slot routing

Status: **item-category to equipment-slot routing is decoded from certified CODE.2 `12180..1246E` and cross-checked against the supplied OBJECTS.DAT corpus. Slot names below are behavioral/UI names; original source enum names are not claimed.**

The character item array is addressed as:

```text
DGROUP 0x21920 + character_index * 0x248 + slot_index * 2
```

The ordinary inventory grid occupies slots `0..11`. Equipment placement uses slots `12..20`.

## Proven equipment routing

CODE.2's equipment hit/placement dispatcher subtracts category 3 and uses the six-entry jump table at `12168` for categories `3..8`.

```text
item_category  routed slot(s)   supplied content
-------------  ---------------  -----------------------------
3              17               helmets / headgear
4              16               body armor
5              12 or 13         rings
6              18               phylactery / stone accessory
7              20               bracers / gauntlets
8              16               robes / cloaks
```

Category 5 chooses slot 12 or 13 from the horizontal equipment-panel coordinate, proving a paired ring-slot UI.

Categories outside `3..8` use the generic hand path:

```text
panel side A -> slot 14
panel side B -> slot 15
```

This includes shields and weapon categories. The safe names are `hand_A` / `hand_B`; left/right orientation is not assigned solely from the binary coordinate test.

## Slot map

The executable-supported equipment region is therefore:

```text
slot 12  ring_A
slot 13  ring_B
slot 14  hand_A
slot 15  hand_B
slot 16  torso/body garment
slot 17  head
slot 18  dedicated accessory
slot 19  quiver
slot 20  bracer/gauntlet
```

Independent consumers corroborate several identities:

- `0F2D4` checks only slots 12 and 13 when looking for object `0x48`, `RING OF WIZARDRY`.
- the armor/AC paths read the body/head/hand equipment slots independently and apply OBJECTS armor/enchantment fields.
- slot 20 participates in the special bracer/gauntlet effect paths.

## Quiver exception

The generic side-A hand path calls the hard-coded predicate at `0F168`. If it matches, CODE.2 reroutes the item from slot 14 to slot 19.

Every supplied ID recognized by that predicate is a category-1 `QUIVER` definition:

```text
6, 265, 270, 288, 295, 305, 308,
309, 315, 317, 321, 326, 327
```

Thus slot 19 is safely the dedicated quiver slot.

The multiple QUIVER definitions are distinct object IDs because their category-1 `state_index` values select different container-state descriptors.

## Body-armor class restrictions

Category 4 normally routes to slot 16, but CODE.2 applies two character-class gates before placement.

### Single-class code 4

If:

```text
class_code[0] == 4
class_code[1] == -1
```

CODE.2 rejects the body-armor placement.

The original class-name enum is not assigned here; this document preserves the encoded class code rather than guessing its source label.

### Any class code 5

`23EAC(character,5)` tests all three stored class-code bytes. If any equals 5, body armor is accepted only when object ID passes predicate `12128`.

The supplied whitelist is exactly:

```text
0x00C  LEATHER ARMOR
0x010  ELVEN CHAIN MAIL
0x05F  ANCIENT ELVEN PLATE MAIL
0x0EC  LEATHER ARMOR
0x0EE  LEATHER ARMOR
```

All five are category 4. This is an item-ID whitelist in the executable, not a general material/category rule, so a faithful port should preserve the exact predicate unless it intentionally generalizes gameplay.

## Temporary touch/spell items

Equipment swap routine `117A0` refuses to move the currently equipped item when its object ID is in `0x7E..0x86`:

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

These are category-0 transient combat/spell pseudo-items. Their early-return treatment is distinct from ordinary static equipment compatibility.

## Generic-path implications

Because only categories `3..8` have dedicated slot dispatch, the remaining categories use hand slots unless another explicit item-ID exception applies. This includes:

```text
0   miscellaneous/devices/consumables
1   containers
2   shields
9   mace/staff/hammer weapons
10  edged/pole weapons
11  special weapons
12  keys
13  stones/sling ammunition
14  scrolls
15  arrows
```

This describes UI placement behavior, not a claim that every such item is tactically useful in a hand.

## Safe implementation boundary

A faithful UI/inventory implementation may expose the slot map above and should preserve these executable details:

- two distinct ring slots and two distinct hand slots;
- body armor and robes/cloaks sharing slot 16;
- the dedicated accessory, quiver, and bracer/gauntlet slots;
- the hard-coded QUIVER object-ID rerouting predicate;
- the encoded class-code body-armor restrictions;
- the immutable `0x7E..0x86` transient-item swap guard.

Do not replace the class-code or item-ID predicates with inferred AD&D rules unless intentionally implementing a gameplay change.