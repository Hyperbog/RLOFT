# OBJECTS.DAT

Status: **fixed 336 × 0x41 record format proven; major rendering, UI, category, weight, combat, range, mutable-state, container, charge, magic-aura, and renderer-auxiliary semantics reconstructed; lossless parse/encode verified.**

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

`attack_profile` contains at least:

```text
bits 0..1  attack_mode
bits 2..5  weapon_speed_factor
```

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
```

## Documentation and tools

- `FORMAT.md` — complete fixed-record field map and executable evidence.
- `CATEGORY_SEMANTICS.md` — mutable state index, charge counters and intrinsic-magic marker.
- `CONTAINER_STATE.md` — 10-byte container descriptor and compatibility rules.
- `RENDER_AUX.md` — scene influence byte `+0x2B` and unresolved byte `+0x2C`.
- `inspect_objects.py` — lossless parser/encoder and basic corpus audit.
- `analyze_categories.py` — category/state/magic analysis plus optional SAVE charge-vector verification.
- `analyze_render_aux.py` — corpus audit for `+0x2B/+0x2C`.

## Current frontier

The static object format is now close to semantic closure. Highest-value remaining OBJECTS work:

1. map item categories to exact equipment slots and use restrictions;
2. refine remaining `attack_profile` low-two-bit modes where CODE.2 proves reusable semantics;
3. determine whether stronger source-level names exist for the scene influence accumulator without overclaiming renderer intent.

`+0x2C` remains explicitly unresolved and preserved. The mutable charge/container state belongs in SAVE/runtime reconstruction rather than in static OBJECTS records.