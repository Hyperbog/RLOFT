# NPCS.DAT

Status: **practical format/semantic closure for faithful parsing and runtime reproduction. The fixed 83 × 0x45 record layout is proven; scene/render fields, collision extent, core combat statistics, XP, action delay, renderer scale bias, runtime-state templates, all four creature sound contexts, and twenty behavior/effect aliases are decoded; lossless parse/encode is verified. Remaining populated flag bits are explicitly partitioned as raw residual capacity or exact-but-intent-unknown executable behavior.**

- Size: 5,731 bytes
- SHA-256: `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b`
- Record count: **83**
- Record size: **0x45 / 69 bytes**
- CODE.2 loader: logical `16810..1686A`
- Runtime table pointer: DGROUP `0x22230`
- Runtime count: DGROUP `0x258F4`

## Proven physical layout

```text
+0x0000  uint32 count = 83
+0x0004  count * 0x45-byte NPC definitions
```

Exact size equation:

```text
4 + 83 * 69 = 5731
```

CODE.2 independently computes and reads the same `count * 0x45` allocation. `LEVELS.DAT` family A indexes this table directly by `type_id * 0x45`.

## Current record model

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

`inspect_npcs.py` uses these field widths while retaining each original 69-byte record as the preservation authority. Twenty supplied records contain nonzero bytes after the first NUL inside `name[20]`; the semantic encoder preserves that post-NUL residue rather than normalizing the entire field to zero padding.

## Core combat and spatial fields

CODE.2 directly establishes:

```text
+0x1D                 hit-dice count used to initialize live NPC HP
+0x1E/+0x1F/+0x20    melee damage dice / sides / bonus
+0x21                 signed armor class
+0x22                 THAC0 / attack target number
+0x23                 experience award distributed on death
```

Examples:

```text
ZOMBIE          HD  4  AC 8  THAC0 17   1d8      XP  270
DOOM GUARD      HD  5  AC 2  THAC0 15   1d10     XP 2000
WEREBAT         HD  4  AC 5  THAC0 17   2d4      XP  420
PYRE ELEMENTAL  HD 12  AC 0  THAC0  9   3d8      XP 7000
HELLHOUND       HD  7  AC 4  THAC0 13   1d10+4   XP 1000
```

`+0x14` is the class-0 bank-local base for the NPC's first-person compiled scene shapes. Because the class-0 RES3 bank begins at zero in this build, its local and absolute numeric IDs coincide. `+0x16` is a fixed-point collision half-extent. `+0x18..+0x1A` drive animation arithmetic. `+0x1B` feeds scene-cell influence propagation. `+0x1C` gates the alternate damage/reaction animation state. `+0x25` initializes a live action/state countdown.

The former `render_aux_27` field is closed: `+0x27` is copied into generated scene-record `+0x0C` and then passed to type-7 renderer `36CEC`, where it is added to the projected-size-derived lookup index before table `0x258C4` is read. The safe name is therefore `render_scale_lookup_bias`. Only PYRE ELEMENTAL uses a nonzero supplied value (`3`). See `RENDER_AUX_27.md`.

The four words at `+0x29` are indexed by live orientation/state and used in spatial vector/placement calculations; `directional_offset[4]` remains the conservative project name.

## Packed runtime/effect flags

The storage boundaries are controlling:

```text
+0x31..+0x34  uint32 behavior_flags
+0x35..+0x38  uint32 initial_runtime_flags
```

`initial_runtime_flags` is copied verbatim into the live family-A record. `behavior_flags` is deliberately accessed by byte, unaligned word, and dword aliases.

### Proved `behavior_flags` aliases

```text
0x00000001  detect_undead_effect_exclusion
0x00000002  hold_undead_effect_immunity
0x00000004  turn_undead_roll_penalty_4
0x00000008  hold_person_effect_immunity
0x00000010  hold_monster_effect_immunity
0x00000080  death_spell_effect_immunity
0x00000100  common_spell_effect_immunity
0x00000200  lightning_bolt_effect_immunity
0x00000800  cold_effect_immunity
0x00002000  fire_effect_immunity
0x00004000  weapon_category10_damage_floor_one
0x00008000  weapon_category9_damage_floor_one
```

Compound masks include:

```text
Hold Undead      0x0102
Hold Person      0x0108
Hold Monster     0x0110
Lightning Bolt   0x0300
cold effects     0x0900
fire effects     0x2100
```

The category-9/category-10 weapon gates reduce the intermediate hit to zero and are then subject to CODE.2's minimum-one-damage clamp. They are therefore resistance/damage-floor behavior, not complete immunity.

### Proved `behavior_flags_39` aliases

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

### Proved `behavior_flags_3A` aliases

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

The poison/disease/paralysis cure mapping is independently closed through the cleric-effect dispatcher:

```text
party status 0x04  Cure Disease
party status 0x08  Neutralize Poison
party status 0x10  Remove Paralysis
```

## Residual flag boundary

`RESIDUAL_FLAGS.md` performs the explicit closure audit rather than guessing the remaining bits.

For the supplied corpus:

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

Six main-dword bits (`0x20`, `0x40`, `0x400`, `0x1000`, `0x10000`, `0x80000`) have serialized carriers but no proved semantic consumer. `0x20000` and `0x40000` are consumed by the anomalous category-11 weapon branch, but their intended source-level meaning is not proved. Four populated `+0x39` bits (`0x02/0x04/0x08/0x10`) likewise remain raw. Every populated `+0x3A` bit is decoded.

Do not rebuild the raw flag fields from named booleans; preserve their exact serialized values.

## Sound fields

All four trailing words are behaviorally separated:

```text
+0x3D  ambient_or_proximity_sound_id
+0x3F  attack_sound_id
+0x41  damage_reaction_sound_id
+0x43  death_sound_id
```

`+0x41` is selected by the common NPC damage/effect handler only when the target survives the operation. `+0x43` is selected on the death path and uses a distinct death-sound resource base with a zero-value fallback. See `SOUND_FIELDS.md`.

## Supplied-corpus validation

```text
records                              83
record size                          0x45
parse -> encode                      PASS byte-for-byte
maximum name length                  18
collision extents                    0x0001 / 0x8000 / 0xFFFF only
nonzero hit-dice definitions         38
nonzero scene influence               1
nonzero render-scale lookup bias      1
unresolved +0x3B nonzero              0
unresolved +0x3C nonzero              0
```

## Documentation and tools

- `FORMAT.md` — physical schema and executable-backed field map.
- `BEHAVIOR_FLAGS.md` — canonical proved alias ledger.
- `RESIDUAL_FLAGS.md` — explicit residual-bit audit and preservation boundary.
- `EFFECT_IMMUNITIES.md` — common/fire/cold effects.
- `HOLD_IMMUNITIES.md` — Hold Undead/Person/Monster masks.
- `TARGETED_EFFECT_IMMUNITIES.md` — Lightning Bolt and Death Spell chains.
- `DETECTION_TURNING_FLAGS.md` — Detect Undead exclusion and turning-roll modifier.
- `STATUS_ATTACKS.md` — poison/disease/paralysis status mapping.
- `WEAPON_DAMAGE_FLAGS.md` — physical weapon gates and category-11 anomaly.
- `SOUND_FIELDS.md` — all four sound contexts.
- `RENDER_AUX_27.md` — type-7 projected-scale lookup bias.
- `inspect_npcs.py` — lossless parser/encoder.
- `analyze_behavior_flags.py` — aggregate bit audit.
- `analyze_residual_flags.py` — residual masks plus full-dword CODE.2 consumer verification.
- specialized analyzers accompany each semantic module.

## Current frontier

NPCS.DAT is at **practical preservation-grade closure**. Remaining questions are non-blocking and explicitly bounded:

1. six populated main-dword bits and four `+0x39` bits have no proved semantic consumer and remain raw;
2. category-11 bits `0x20000/0x40000` have exact executable behavior but unresolved intended source-level names;
3. `+0x3B/+0x3C` are zero in all supplied definitions but are preserved rather than declared mandatory padding;
4. story-NPC-specific animation interpretation may be documented later only if code proves stronger names.

None of these prevents a faithful reader/editor, creature database implementation, combat model, renderer linkage, or effect/status system. The next major DAT reverse-engineering target is `SAVE.DAT`.