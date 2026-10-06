# OBJECTS.DAT

Status: **fixed 336 × 0x41 record format proven; major rendering, UI, category, weight, combat, range, mutable-state, container, charge, magic-aura, renderer-auxiliary, equipment-slot, and hand-pair semantics reconstructed; lossless parse/encode verified.**

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

## Current record model

```text
+0x00  char   name[40]
+0x28  uint16 world_sprite_base
+0x2A  uint8  world_heading_stride
+0x2B  uint8  scene_influence_step
+0x2C  uint8  unresolved_2C
+0x2D  uint16 ui_graphic_index
+0x2F  uint8  item_category
+0x30  uint8  weight_half_pounds
+0x31  int8   state_index                 // polymorphic mutable-state selector
+0x32  uint16 intrinsic_magic_marker
+0x34  int8   armor_class_adjustment
+0x35  uint16 attack_profile
+0x37  uint8  enchantment_bonus
+0x38..0x3A   small-target damage profile
+0x3B..0x3D   large-target damage profile
+0x3E         range_short
+0x3F         range_medium
+0x40         range_long
```

`attack_profile` is now behaviorally split as:

```text
bits 0..1  hand_pair_class
bits 2..5  weapon_speed_factor
```

Pair class 2 is proved **paired-hand-exclusive**. Values 0 and 1 remain distinct pairable compatibility classes pending stronger source-level labels.

The family-C renderer proves:

```text
world_sprite_frame = world_sprite_base
                   + heading_index * world_heading_stride
```

LEVELS family B and C both address this database by object type ID.

## `+0x2B`: scene influence propagation

When a loose family-C object is emitted into the first-person scene, CODE.2 passes nonzero object byte `+0x2B` to `2B088` together with the object's current map-cell index.

That routine updates the scene-cell accumulator for the base cell and, for larger encoded values, neighboring rings:

```text
step 1  base cell
step 2  base + eight immediate neighbors
step 3  base + immediate ring + sixteen-cell outer ring
```

The exact accumulator propagation is documented in `RENDER_AUX.md`. All 336 supplied object records store zero here, so this is executable-supported latent format capability rather than a feature exercised by the shipped OBJECTS table.

`+0x2C` remains raw: all supplied records store zero and no OBJECTS-relative CODE.2 consumer was found, including no unaligned `+0x2B` word access that aliases it.

## `+0x31`: mutable-state selector

The byte is signed and deliberately polymorphic.

### Containers

For category `1`, it selects a 10-byte container-state descriptor at runtime `0x7330`. That descriptor contains:

```text
slot_count
UI column count
capacity in whole pounds
UI x offset
container subtype
contents pointer
per-slot state bits
```

Specialized subtypes enforce:

```text
SLING POUCH -> category 13 ammunition
SCROLL CASE -> category 14 scrolls
KEY RING    -> category 12 keys
QUIVER      -> category 15 arrows
```

All 62 shipped container definitions use distinct state indexes. See `CONTAINER_STATE.md`.

### Charged rods/wands

For the proved charged-device family, `+0x31` selects one of **15 persistent charge counters** at runtime `0x67EC`.

CODE.2 tests the selected counter, decrements it on successful use and runs the depletion/removal path at zero. The supplied runtime image initializes all 15 counters to `12`.

OBJECTS records select slots 0..11; duplicate FIREBALL and FROST definitions intentionally differ only at `+0x31` so they can maintain independent charge state.

SAVE.DAT writes/restores the entire 15-byte counter vector.

## `+0x32`: intrinsic magic marker

This field is executable-backed rather than merely corpus-associated.

When magic-reveal state is active, CODE.2 highlights an item when:

```text
enchantment_bonus != 0
OR
intrinsic_magic_marker != 0
```

The field is therefore a serialized intrinsic magic/aura marker independent from numerical enchantment.

Supplied counts:

```text
marker 0 : 199
marker 1 : 137
```

Do not merge `+0x32` and `+0x37` into a single `is_magic` value.

## Equipment-slot routing

CODE.2 `12180..1246E` maps item categories to equipment slots in the character item array at `0x21920 + character*0x248 + slot*2`.

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

Dedicated category routing is:

```text
category 3 -> slot 17
category 4 -> slot 16
category 5 -> slot 12 or 13
category 6 -> slot 18
category 7 -> slot 20
category 8 -> slot 16
```

All remaining categories use the two generic hand slots unless an explicit object-ID exception applies. The hard-coded exception predicate at `0F168` contains thirteen object IDs; every one is a category-1 `QUIVER`, proving slot 19 as the dedicated quiver slot.

Category-4 body armor has exact class-code restrictions, including a five-object executable whitelist for characters carrying class code 5. Transient spell/touch pseudo-items `0x7E..0x86` are protected from ordinary swapping.

See `EQUIPMENT_SLOTS.md` and `analyze_equipment_slots.py`.

## Hand-pair profile

The low two bits of `attack_profile` are read by `23FC4..24A5D` while arbitrating equipment slots 14/15.

For class 2, CODE.2 invalidates the opposite hand. The complete supplied class-2 set is:

```text
QUARTERSTAFF
TWO-HANDED SWORD
HALBERD
COMPOSITE BOW
JAVELIN OF LIGHTNING
```

with duplicate QUARTERSTAFF definitions accounting for seven physical records total. Because the set includes ranged/thrown definitions as well as obvious two-handed melee weapons, the safe name is **paired-hand-exclusive**, not simply `two_handed`.

Class 1 contains larger pairable one-hand weapons such as BATTLE AXE, MACE, LONG SWORD, BROAD SWORD, WARHAMMER and SPEAR. Class 0 contains many default records plus shorter/special weapons such as SHORT SWORD, DAGGER and THROWING KNIFE. The exact source-level distinction between classes 0 and 1 remains unresolved.

See `ATTACK_PROFILE.md` and `analyze_attack_profile.py`.

## Supplied-corpus validation

Current tools validate:

```text
records                              336
record size                          0x41
parse -> encode                      PASS byte-for-byte
blank display names                   16
max display-name length               39
scene_influence_step nonzero           0
unresolved +0x2C nonzero               0
container state indexes               62 unique / 62
container indexes                     0..60 except 61, plus 62
charged state-slot mappings           PASS
charged duplicate definitions differ  only at +0x31
intrinsic-magic marker values          0/1 only
SAVE initial charge vector             unique at +0x0D59
quiver equipment exceptions            13 / 13 resolve to QUIVER
class-code-5 armor whitelist            5 / 5 category-4 records
transient touch-item guard              9 / 9 category-0 pseudo-items
hand_pair_class counts                 311 / 18 / 7 / 0
pair-class-2 definitions                7 physical records / 5 names
```

## Documentation and tools

- `FORMAT.md` — complete fixed-record field map and executable evidence.
- `CATEGORY_SEMANTICS.md` — mutable state index, charge counters and intrinsic-magic marker.
- `CONTAINER_STATE.md` — 10-byte container descriptor and compatibility rules.
- `RENDER_AUX.md` — scene influence byte `+0x2B` and unresolved byte `+0x2C`.
- `EQUIPMENT_SLOTS.md` — exact equipment-slot/category routing and hard-coded restrictions.
- `ATTACK_PROFILE.md` — packed hand-pair class and weapon-speed semantics.
- `inspect_objects.py` — lossless parser/encoder and basic corpus audit.
- `analyze_categories.py` — category/state/magic analysis plus optional SAVE charge-vector verification.
- `analyze_render_aux.py` — corpus audit for `+0x2B/+0x2C`.
- `analyze_equipment_slots.py` — equipment category, quiver, armor-whitelist and transient-item audit.
- `analyze_attack_profile.py` — pairing-class and weapon-speed corpus audit.

## Current frontier

The static object format is now at practical semantic closure for faithful parsing/editing. Remaining OBJECTS questions are narrow:

1. determine whether stronger source-level names exist for pairing classes 0/1 without inferring them from weapon names;
2. determine whether stronger source-level names exist for the scene influence accumulator without overclaiming renderer intent;
3. keep `+0x2C` explicitly unresolved unless a real consumer is found.

None of these questions block a lossless OBJECTS reader/editor, equipment model, combat metadata model, or SAVE/runtime state linkage. The next major database target is `NPCS.DAT`.