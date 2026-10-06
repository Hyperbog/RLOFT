# OBJECTS.DAT final reconstruction audit — 2026-10-06

Status: **ARCHIVAL-COMPLETE FOR THE SUPPLIED RAVENLOFT BUILD, WITH BOUNDED RAW/LATENT FIELDS PRESERVED AND BYTE-EXACT DECODE→ENCODE VERIFIED.**

This checkpoint re-audits the static object/item database against the supplied immutable `OBJECTS.DAT`, certified CODE.2 behavior, the reconstructed SAVE/LEVELS state model, and the certified RES1/RES3 resource namespaces. It corrects one over-specific renderer label discovered during cross-workstream consistency review, without changing any file geometry or serialized byte interpretation.

## Controlling source identity

```text
OBJECTS.DAT   21,844 bytes
SHA-256       f23f0e7c1856904aa0156b0bf174cab794a41a327352b6ca00c3591ae5f2eed0
records       336
record size   0x41 / 65 bytes
CODE.2 loader 168C8..16930
runtime table DGROUP 0x255D8
runtime count DGROUP 0x258E8
```

The exact file equation remains:

```text
4 + 336 * 0x41 = 21,844
```

All 65 bytes of every supplied record are accounted for by the canonical schema. The supplied parser preserves the original raw record as authority and decode→encode reproduces the source file byte-for-byte.

## Final canonical schema

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
+0x38..0x3A   small-target damage dice/sides/bonus
+0x3B..0x3D   large-target damage dice/sides/bonus
+0x3E         range_short
+0x3F         range_medium
+0x40         range_long
```

## Renderer/resource consistency correction

Earlier OBJECTS notes called `+0x28` a world-sprite base and `+0x2A` a heading stride. The completed RES3 source-bank reconstruction proves a more precise model.

Family-C scene emission treats them as class-2 bank-local scene-shape coordinates:

```text
variant_subrange_base = local_shape_base
                      + scene_variant * shape_stride

absolute_RES3_base    = 3316 + variant_subrange_base
```

The common scene helper may then choose a view/state-dependent subframe inside that stride. Therefore the live three-bit selector is not universally a heading.

The supplied class-2 bank is exactly:

```text
RES3 3316..4157 = 842 resources
```

Across all 336 OBJECTS definitions:

```text
unique local_shape_base values      121
unique (base,stride) pairs          123
largest supplied local base         834
largest supplied base+stride        842
all base/stride windows in range    PASS
```

This correction changes terminology only. No serialized field width, offset, value, or runtime lookup equation is altered.

## UI resource namespace closure

`+0x2D` remains the local inventory/UI graphic index. The supplied corpus uses values `0..167`; INIT establishes the OBJECTS UI base at RES1 index 168:

```text
absolute_RES1_id = 168 + ui_graphic_index
```

Thus every supplied OBJECTS UI reference lies inside the certified OBJECTS UI namespace `RES1:168..335`.

## Mutable-state linkage closure

`+0x31` is a signed polymorphic selector into mutable state stored outside the static database.

Category-1 containers use 62 distinct selectors:

```text
0..60, 62
```

The shipped selector-62 anomaly remains intentional compatibility evidence:

- OBJECTS ID 329 `QUIVER` stores selector 62;
- runtime descriptor 61 is quiver-shaped;
- runtime descriptor 62 is zero-filled;
- SAVE serializes descriptor/state range 0..61;
- no executable `62 -> 61` repair was found.

Faithful compatibility preserves this mismatch rather than silently normalizing it.

Charged rods/wands use the same physical byte as an index into the 15-byte persistent charge vector at DGROUP `0x67EC`. Supplied definitions select slots `0..11`; the quick-start SAVE contains the unique initialized fifteen-byte `0x0C` vector at `+0x0D59`. Duplicate FIREBALL/FROST definitions differ only at record byte `+0x31`, proving separate persistent charge identities.

## Combat/equipment closure

The packed word at `+0x35` remains:

```text
bits 0..1  hand_pair_class
bits 2..5  weapon_speed_factor
bits 6..15 preserved; zero in supplied database
```

Corpus counts:

```text
hand_pair_class 0   311
hand_pair_class 1    18
hand_pair_class 2     7
hand_pair_class 3     0
```

Class 2 is behaviorally proved paired-hand-exclusive. Its seven physical records represent five names: QUARTERSTAFF, TWO-HANDED SWORD, HALBERD, COMPOSITE BOW, and JAVELIN OF LIGHTNING. Classes 0 and 1 remain distinct numeric compatibility classes because stronger original enum names are not proved.

Equipment-slot routing, category restrictions, the thirteen hard-coded QUIVER IDs, the class-code-5 armor whitelist, and transient touch-item protections remain consistent with the supplied database.

## Latent and raw fields

Two renderer-adjacent bytes remain correctly bounded:

- `+0x2B scene_influence_step`: executable behavior is proved through routine `2B088`, but all 336 supplied records store zero. It is latent engine-supported capacity.
- `+0x2C unresolved_2C`: all 336 supplied records store zero and no OBJECTS-relative CODE.2 consumer has been proved, including no aliasing unaligned word access. It remains raw/preserved.

These are not structural holes. The database can be parsed, edited, re-encoded, and linked to runtime state without inventing semantics for either field.

## Corpus recheck

```text
records                               336
record size                           0x41
decode -> encode                      PASS byte-for-byte
blank display names                    16
maximum display-name length            39
scene_influence_step nonzero            0
unresolved +0x2C nonzero                0
intrinsic_magic_marker values        0/1 only
nonzero range records                   17
attack_profile bits 6..15 nonzero       0
container selectors unique             62 / 62
charged state-slot mappings            PASS
charged duplicate differences          +0x31 only
RES3 class-2 windows in range           PASS
RES1 UI indices in range                PASS
```

## Bounded remaining questions

The following may be refined by future historical/source recovery but do not block reconstruction:

- original source-level names for hand-pair classes 0 and 1;
- original source/editor name for the scene-cell influence accumulator;
- physical-world units of the three range bytes;
- historical provenance of otherwise valid duplicated definitions and legacy object-table revisions;
- a source-level name for `+0x2C`, if any consumer is ever found in another build.

No supplied byte is unaccounted for because of these naming limits.

## Final decision

For the supplied STRAHD'S POSSESSION build, `OBJECTS.DAT` has no unresolved structural region or semantic ambiguity that blocks a faithful reader/editor, byte-exact round-trip serializer, equipment/combat model, resource resolver, LEVELS linkage, or SAVE mutable-state linkage.

The OBJECTS reconstruction is therefore **archival-complete to the supplied evidence boundary**. Future findings should refine historical labels or cross-version provenance, not silently reinterpret or normalize the certified serialized layout.
