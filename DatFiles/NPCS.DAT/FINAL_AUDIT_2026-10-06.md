# NPCS.DAT final reconstruction audit — 2026-10-06

Status: **ARCHIVAL-COMPLETE FOR THE SUPPLIED RAVENLOFT BUILD, WITH BOUNDED RAW FLAG CAPACITY PRESERVED AND BYTE-EXACT SEMANTIC DECODE→ENCODE VERIFIED.**

This checkpoint re-audits the creature/NPC definition database against the supplied immutable `NPCS.DAT`, certified CODE.2 behavior, LEVELS family-A usage, RES3 class-0 scene-shape resources, combat/effect consumers, and creature-audio paths.

The audit found two consistency/tooling corrections:

1. `+0x14` is more precisely a **class-0 bank-local compiled scene-shape base**, not a generic sprite base;
2. the earlier parser's "encoder" merely concatenated retained raw records. A real field serializer is now implemented and independently reproduces the original file byte-for-byte while preserving post-NUL name residue and unresolved raw flag bits.

Neither correction changes serialized geometry or field offsets.

## Controlling source identity

```text
NPCS.DAT      5,731 bytes
SHA-256       f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b
records       83
record size   0x45 / 69 bytes
CODE.2 loader 16810..1686A
runtime table DGROUP 0x22230
runtime count DGROUP 0x258F4
```

Exact file equation:

```text
4 + 83 * 0x45 = 5,731
```

LEVELS family A indexes this table directly with `type_id * 0x45`. All 1,258 supplied family-A records remain within the 83-entry NPC namespace.

## Final canonical schema

```text
+0x00  char    name[20]
+0x14  uint16  local_shape_base
+0x16  uint16  collision_half_extent
+0x18  uint8   scene_frame_stride
+0x19  uint8   animation_cycle_divisor
+0x1A  uint8   animation_span
+0x1B  uint8   scene_influence_step
+0x1C  uint8   damage_reaction_animation_flag
+0x1D  uint8   hit_dice_count
+0x1E  uint8   melee_damage_dice
+0x1F  uint8   melee_damage_sides
+0x20  uint8   melee_damage_bonus
+0x21  int8    armor_class
+0x22  uint8   thac0
+0x23  uint16  experience_award
+0x25  uint16  action_delay
+0x27  uint16  render_scale_lookup_bias
+0x29  uint16  directional_offset[4]
+0x31  uint32  behavior_flags
+0x35  uint32  initial_runtime_flags
+0x39  uint8   behavior_flags_39
+0x3A  uint8   behavior_flags_3A
+0x3B  uint8   unresolved_3B
+0x3C  uint8   unresolved_3C
+0x3D  uint16  ambient_or_proximity_sound_id
+0x3F  uint16  attack_sound_id
+0x41  uint16  damage_reaction_sound_id
+0x43  uint16  death_sound_id
```

All 69 bytes are physically accounted for.

## Name-field preservation correction

All 83 names terminate within the fixed 20-byte field, and the longest active C string is 18 characters.

However, **20 records contain nonzero physical bytes after the first NUL terminator**. These include:

- one or two `0x0D` bytes in many early combat definitions;
- stale text fragments in a few records, including tail bytes spelling `ERERAV` and `EMO STRAH`.

These bytes are outside the active C-string name because runtime name consumers stop at the first NUL, but they are still part of the serialized field and therefore preservation evidence.

The semantic encoder now:

- starts from the exact source record;
- rewrites only the active name plus terminating NUL when the name is edited;
- preserves the remaining post-NUL bytes unless raw-field editing intentionally changes them;
- reserializes every other parsed field at its certified width/offset.

Decode→encode now reproduces the immutable source exactly rather than passing only because raw records were concatenated unchanged.

## RES3 class-0 scene-shape linkage

Family-A scene emission reads `NPCS.DAT +0x14` and uses it as the local base for source class 0.

The certified RES3 partition is:

```text
source class 0  RES3 0..2168  2169 entries
```

Thus:

```text
absolute_RES3_id = 0 + npc_local_shape_id
```

The zero class-0 bank base makes local and absolute values numerically identical in this build, but the architectural identity remains bank-local.

All 83 supplied bases are in range:

```text
minimum local base  0
maximum local base  2149
class-0 limit       2169
range check         PASS
```

Animation/state code applies offsets using `+0x18..+0x1A`; resource identity is therefore compiled type-7 scene-shape/display-list data, not a generic bitmap sprite.

## Spatial/render fields

`+0x16` remains an exact 16-bit collision half-extent encoding and must not be normalized to ordinary integer dimensions.

Supplied values:

```text
0x8000  76 records
0x0001   4 records
0xFFFF   3 records
```

`+0x1B` routes through the same scene-cell influence propagation helper used by other renderer source families. Only PYRE ELEMENTAL has a nonzero supplied value (`3`).

`+0x27` is closed as `render_scale_lookup_bias`; only PYRE ELEMENTAL stores a nonzero value (`3`).

The four words at `+0x29` remain conservatively named `directional_offset[4]`. Their exact source-level/editor name is not needed for faithful behavior.

## Combat-stat closure

Executable consumers directly establish:

```text
+0x1D  hit-dice count used for initial HP
+0x1E  melee damage dice count
+0x1F  melee damage die sides
+0x20  melee damage bonus
+0x21  signed armor class
+0x22  THAC0 / attack target number
+0x23  XP award on death
+0x25  initial action/state countdown
```

Thirty-eight supplied definitions have nonzero hit dice. Story/ambient definitions with zero combat fields remain valid records rather than being filtered out.

## Behavior/effect flag closure

The controlling physical fields remain exact raw storage:

```text
+0x31..+0x34  uint32 behavior_flags
+0x35..+0x38  uint32 initial_runtime_flags
+0x39         uint8  behavior_flags_39
+0x3A         uint8  behavior_flags_3A
```

The proved semantic aliases remain unchanged.

Final supplied masks:

```text
behavior_flags populated mask   0x000FFFFF
proved semantic mask            0x0000EB9F
residual populated mask         0x000F1460

behavior_flags_39 populated     0xFF
proved semantic mask            0xE1
residual populated mask         0x1E

behavior_flags_3A populated     0x36
proved semantic mask            0x36
residual populated mask         0x00
```

Residual classification remains:

- `0x20`, `0x40`, `0x400`, `0x1000`, `0x10000`, `0x80000`: serialized carriers with no proved semantic consumer;
- `0x20000`, `0x40000`: exact category-11 weapon-gate behavior is consumed, but intended source-level labels remain unknown;
- `behavior_flags_39` bits `0x02/0x04/0x08/0x10`: populated but no proved direct semantic branch;
- every populated `behavior_flags_3A` bit is decoded.

A faithful implementation must preserve complete raw fields and expose aliases as views; it must not regenerate the raw words from only named booleans.

## Raw zero fields

`+0x3B` and `+0x3C` are zero in all 83 supplied records and have no proved NPC-definition consumer in the certified build.

They remain `unresolved_3B` / `unresolved_3C`, not mandatory padding. The semantic serializer preserves them.

## Creature sound closure

All four trailing words remain behaviorally separated:

```text
+0x3D  ambient_or_proximity_sound_id
+0x3F  attack_sound_id
+0x41  damage_reaction_sound_id
+0x43  death_sound_id
```

The first three use the creature-sound base at runtime `0x258C0`; death uses the distinct base at `0x258BC`. Zero death ID retains the original fallback behavior rather than meaning "no sound."

No audio-field contradiction was found during the cross-workstream audit.

## Corpus recheck

```text
records                                  83
record size                              0x45
semantic decode -> encode                PASS byte-for-byte
SHA-256 after rebuild                    f2dd1175...e5f2eed0
maximum active name length               18
nonzero post-NUL name-tail records       20
collision extent encodings               3 distinct
nonzero hit-dice definitions             38
nonzero scene-influence definitions       1
nonzero render-scale lookup bias          1
unresolved +0x3B nonzero                  0
unresolved +0x3C nonzero                  0
RES3 class-0 base range                  PASS
residual flag masks                      PASS
```

## Bounded remaining questions

The following remain archival enrichment questions rather than format holes:

- original source-level names for six unused/populated main behavior bits;
- intended source-level names for category-11 bits `0x20000/0x40000`;
- original names for four residual `+0x39` bits;
- original source/editor names for the directional-offset vector and scene-cell influence quantity;
- whether another historical build gives semantic use to `+0x3B/+0x3C`;
- finer story-NPC animation labels where the executable only proves arithmetic behavior.

None blocks faithful parsing, editing, scene rendering, combat/status behavior, audio routing, LEVELS linkage, or byte-exact serialization.

## Final decision

For the supplied STRAHD'S POSSESSION build, `NPCS.DAT` has no unresolved structural region or unbounded semantic ambiguity that prevents preservation-grade implementation.

The NPCS reconstruction is therefore **archival-complete to the supplied evidence boundary**. Future discoveries should refine historical/source labels or cross-version provenance, not silently normalize the certified bytes.
