# OBJECTS.DAT format reconstruction

Status: **fixed container/record layout proven; all 65 physical record bytes are accounted for, with major rendering, UI, weight, mutable-state, magic, equipment/combat, damage and range fields semantically reconstructed from certified CODE.2.**

## Controlling evidence

- `OBJECTS.DAT`: 21,844 bytes
- SHA-256: `f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0`
- CODE.2 loader: logical `168C8..16930`
- runtime object-table pointer: DGROUP `0x255D8`
- runtime object count: DGROUP `0x258E8`

The immutable executable remains controlling evidence. Unknowns stay raw; category-specific state is not flattened into a false universal field meaning.

## File layout

```text
+0x0000  uint32 object_count = 336
+0x0004  object_record objects[object_count]
```

Each object record is exactly `0x41` / 65 bytes:

```text
4 + 336 * 0x41 = 21,844
```

CODE.2 independently computes `(count << 6) + count`, allocates exactly that many bytes and loads the array.

## Canonical record schema

```text
+0x00  char   name[40]
+0x28  uint16 local_shape_base
+0x2A  uint8  shape_stride
+0x2B  uint8  scene_influence_step
+0x2C  uint8  unresolved_2C
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

All bytes are physically accounted for. This does not imply that every field is active for every item category.

## Name field

The first 40 bytes are a fixed NUL-padded display-name field. All supplied records contain a NUL inside the field and the longest supplied name is 39 characters. Sixteen valid records have empty display names; they must not be discarded.

## Scene-shape rendering: `+0x28`, `+0x2A`, `+0x2B`

Cross-checking the family-C emitter against the certified RES3 source-bank reconstruction tightens the first two names. OBJECTS definitions store class-2 **bank-local** scene-shape coordinates:

```text
variant_subrange_base = local_shape_base
                      + scene_variant * shape_stride
absolute_RES3_base    = 3316 + variant_subrange_base
```

The common scene helper may then select a view/state-dependent subframe within that stride. The live three-bit selector is therefore not safely named a universal heading. Normal family-C loading and thrown-item construction initialize its variant bits to zero; no class-2 periodic variant-cycle path has been proved.

The class-2 bank contains exactly 842 resources (`RES3:3316..4157`). Across all 336 definitions, every base/stride window is in range and the maximum supplied endpoint is exactly `834 + 8 = 842`.

For nonzero `scene_influence_step`, CODE.2 passes the byte and the object's map-cell index to `2B088`, which propagates an accumulated scene-cell influence:

```text
step 1  base cell
step 2  base + eight immediate neighbors
step 3  base + immediate ring + sixteen-cell outer ring
```

All 336 supplied records store zero at `+0x2B`, so this is executable-supported latent format behavior. `RENDER_AUX.md` records the exact algorithm.

`+0x2C` is zero in all supplied records and no OBJECTS-relative CODE.2 consumer has been proved. It remains raw/preserved.

## UI graphic: `+0x2D`

Inventory/UI paths read the unaligned word at `+0x2D` as the object's UI graphic/resource index. Logical variants may deliberately share this value, such as spell-specific scroll definitions using common scroll artwork.

## Item category: `+0x2F`

The supplied database uses all category values `0..15`:

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

CODE.2 explicitly special-cases category 1 and groups categories `2..4`, `5..8`, and `9..11` in several item-presentation/equipment paths. These names are project behavioral/content labels, not claimed original source enum names.

See `EQUIPMENT_SLOTS.md` for the proved equipment routing of categories 3..8 and the separate hand/admissibility layer.

## Weight: `+0x30`

Weight is stored in half-pound units. Recursive burden logic adds this byte for each item and follows category-1 container contents. Item description divides by two and displays a half-pound suffix for odd values.

Examples:

```text
LONG SWORD      8  -> 4 lb
LEATHER ARMOR  30  -> 15 lb
PLATE MAIL    100  -> 50 lb
SLING BULLET    1  -> 0.5 lb
```

## Mutable-state selector: `+0x31`

The byte is signed and selects mutable state stored outside static OBJECTS.DAT.

### Containers

For category 1, it selects a 10-byte runtime descriptor at DGROUP `0x7330`. The descriptor stores slot count, UI columns, capacity, UI offset, subtype, contents pointer and per-slot state bits. All 62 shipped container definitions have distinct state indexes.

See `CONTAINER_STATE.md`.

### Charged devices

For the proved rod/wand family it selects one of 15 persistent charge bytes at DGROUP `0x67EC`. CODE.2 rejects use at zero, decrements on successful use and performs depletion/removal at zero.

The supplied mappings use slots `0..11`; duplicate FIREBALL/FROST definitions differ only at `+0x31`, intentionally giving otherwise identical definitions separate persistent charge identities.

SAVE.DAT serializes the complete 15-byte charge vector. See `CATEGORY_SEMANTICS.md`.

The generic physical name remains `state_index`.

## Intrinsic magic marker: `+0x32`

Two independent item-presentation paths under magic-reveal state use:

```text
enchantment_bonus != 0
    OR
intrinsic_magic_marker != 0
```

Thus this word is an intrinsic magic/aura marker independent from the numerical enchantment byte.

Supplied values are only:

```text
0 : 199 records
1 : 137 records
```

Do not merge `+0x32` and `+0x37` into one serialized `is_magic` field.

## Armor and enchantment: `+0x34`, `+0x37`

`+0x34` is a signed armor-class adjustment. Examples include shield `-1`, leather armor `-2`, chain mail `-5`, bronze plate `-6`, and plate mail `-7`.

`+0x37` is the numerical enchantment bonus used by combat/defense and item-description formatting. Supplied values are `0..3`.

## Packed attack profile: `+0x35`

CODE.2 now supports the tighter behavioral split:

```text
bits 0..1  hand_pair_class
bits 2..5  weapon_speed_factor
bits 6..15 preserved; zero in the supplied database
```

The speed factor feeds attack timing. `23FC4..24A5D` reads the low two bits while arbitrating the proved hand slots 14/15.

Hand-pair class `2` is **paired-hand-exclusive**: when a qualifying hand item has class 2, CODE.2 invalidates use of the opposite hand. The seven supplied class-2 definitions are QUARTERSTAFF (three physical definitions), TWO-HANDED SWORD, HALBERD, COMPOSITE BOW, and JAVELIN OF LIGHTNING.

Because the class includes ranged/thrown items as well as obvious two-handed melee weapons, `paired_hand_exclusive` is safer than a universal `two_handed` label.

Classes 0 and 1 are pairable compatibility classes subject to actor roles, item-specific predicates, and additional weight arbitration. Their exact original enum names remain unresolved.

See `ATTACK_PROFILE.md` and `analyze_attack_profile.py`.

## Damage profiles: `+0x38..+0x3D`

Two three-byte dice profiles are stored:

```text
+0x38/+0x39/+0x3A  small-target dice / sides / bonus
+0x3B/+0x3C/+0x3D  large-target dice / sides / bonus
```

The first is directly passed through the common combat dice path; the second is the alternate target-size profile. Supplied values match the expected AD&D notation, e.g. LONG SWORD `1d8 / 1d12` and TWO-HANDED SWORD `1d10 / 3d6`.

## Range bands: `+0x3E..+0x40`

The final three bytes are short/medium/long missile or thrown ranges. Examples:

```text
DAGGER               1 /  2 /  3
COMPOSITE BOW        6 / 12 / 21
SLING                5 / 10 / 20
ARROW                5 / 10 / 18
JAVELIN OF LIGHTNING 2 /  4 /  6
```

The physical game unit remains unspecified.

## Cross-file relationships

LEVELS.DAT uses OBJECTS type IDs in two distinct world-record families:

```text
family B  placed object/item records
family C  loose/pickable items, also reused for dropped/thrown motion
```

SAVE.DAT persists mutable state selected by static object definitions, including the 15-byte charged-item vector. Static definition bytes and live state must remain separate in a faithful port/editor.

## Supplied-corpus checks

```text
records                          336
record size                      0x41
parse -> encode                  byte-exact PASS
blank names                       16
maximum name length               39
scene_influence_step nonzero       0
unresolved +0x2C nonzero           0
intrinsic_magic_marker values      0 or 1
container state indexes            62 unique / 62
hand_pair_class                  311 / 18 / 7 / 0
nonzero range records              17
```

## Safe implementation boundary

A faithful reader/editor may expose the canonical fields above while preserving the original 65-byte record as the authority. In particular:

- retain `state_index` as a polymorphic mutable-state selector;
- retain separate intrinsic-magic and numerical-enchantment fields;
- expose `scene_influence_step` using its exact propagation behavior even though shipped values are zero;
- preserve `+0x2C` raw;
- expose attack-profile low bits as `hand_pair_class`, with class 2 named `paired_hand_exclusive` and classes 0/1 left numeric;
- do not infer range units;
- retain empty-name records.

`inspect_objects.py` is the lossless parser/encoder. `analyze_categories.py`, `analyze_render_aux.py`, `analyze_equipment_slots.py`, and `analyze_attack_profile.py` provide corpus validation.