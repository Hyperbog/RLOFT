# NPCS.DAT

Status: **fixed 83 × 0x45 record format proven; scene/render parameters, collision extent, core combat statistics, XP, action delay, runtime-state templates and creature sound IDs reconstructed; eighteen behavior/effect bits decoded; lossless parse/encode verified.**

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

CODE.2 independently computes and reads the same `count * 0x45` allocation. LEVELS.DAT family A also indexes this table directly by `type_id * 0x45`.

## Current record model

```text
+0x00  char    name[20]
+0x14  uint16  scene_sprite_base
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
+0x27  uint16  render_aux_27
+0x29  uint16  directional_offset[4]
+0x31  uint32  behavior_flags
+0x35  uint32  initial_runtime_flags
+0x39  uint8   behavior_flags_39
+0x3A  uint8   behavior_flags_3A
+0x3B  uint8   unresolved_3B
+0x3C  uint8   unresolved_3C
+0x3D  uint16  ambient_or_proximity_sound_id
+0x3F  uint16  attack_sound_id
+0x41  uint16  sound_event_41_id
+0x43  uint16  death_sound_id
```

See `FORMAT.md` for executable evidence and access-width details.

## Core combat and scene fields

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

`+0x14` is the base first-person scene sprite/resource. `+0x18..+0x1A` participate in animation frame arithmetic. `+0x16` is the fixed-point collision half-extent. `+0x1B` uses the same scene-cell influence propagation helper as OBJECTS.DAT; only PYRE ELEMENTAL has a nonzero supplied value (`3`). `+0x27` is also nonzero only for PYRE ELEMENTAL and remains behaviorally named `render_aux_27`.

## Packed runtime flags

Two access-width boundaries remain controlling:

```text
+0x31..+0x34  uint32 behavior_flags
+0x35..+0x38  uint32 initial_runtime_flags
```

`behavior_flags` is also read by byte and unaligned word. `initial_runtime_flags` is copied verbatim into the live family-A record on NPC initialization. Do not split or normalize these overlapping representations.

### Proved `behavior_flags` aliases

```text
0x00000002  hold_undead_effect_immunity
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

CODE.2 constructs these compound spell masks:

```text
Hold Undead      0x0102 = 0x0100 | 0x0002
Hold Person      0x0108 = 0x0100 | 0x0008
Hold Monster     0x0110 = 0x0100 | 0x0010
Lightning Bolt   0x0300 = 0x0100 | 0x0200
fire effects     0x2100 = 0x0100 | 0x2000
cold effects     0x0900 = 0x0100 | 0x0800
```

Mage and priest Hold Person independently use `0x0108`; the Hold variants share the same live held-state bit. Elemental aliases are additionally consistent with the game's bestiary descriptions.

Lightning Bolt is closed through `OBJECTS.DAT` object 188 -> effect 28 -> moving-effect type 1 -> NPC mask `0x0300`. Death Spell is closed through object 197 -> effect 51 -> target routine `AED8` -> direct `npc[+0x31] & 0x80` exclusion. See `TARGETED_EFFECT_IMMUNITIES.md`.

The two weapon-category gates come from CODE.2 `0xC910..0xC99E`. Before testing them, the engine reroutes every nonzero-enchantment OBJECTS definition to category 11. An unenchanted category-10 hit against `0x00004000`, or an unenchanted/category-fallback-9 hit against `0x00008000`, is zeroed and then clamped by the same routine to **one damage minimum**. They are damage-floor/resistance behaviors, not zero-damage immunity. See `WEAPON_DAMAGE_FLAGS.md`.

### Proved `behavior_flags_39` aliases

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

The poison identity is executable-closed: the attack sets party status bit `0x08`; internal cure code `0x71` clears it; cleric effect ID `113` supplies code `0x71`; OBJECTS.DAT object `223` is `CLERIC SCROLL OF 'NEUTRALIZE POISON'`.

Paralysis is independently anchored by `RING OF FREE ACTION` and the Remove Paralysis cure path. Navigation with `0x40` can activate the dynamic barrier/door record family; `0x80` suppresses blocking from placed/special obstacles.

### Proved `behavior_flags_3A` aliases

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

The disease identity is executable-closed: the attack sets party status bit `0x04`; internal cure code `0x64` clears it; cleric effect ID `100` supplies that code; OBJECTS.DAT object `215` is `CLERIC SCROLL OF 'CURE DISEASE'`.

The level-drain path decrements character class levels/progression; the Strength-drain path calls the Strength-adjustment helper with `-1`; turning, detection and combat paths independently use `0x20` as the undead classification.

The three cure/status controls line up exactly:

```text
status 0x04  code 0x64  Cure Disease
status 0x08  code 0x71  Neutralize Poison
status 0x10  code 0x68  Remove Paralysis
```

See `BEHAVIOR_FLAGS.md` and `STATUS_ATTACKS.md` for the full executable evidence.

## Sound fields

The final four words are executable-consumed sound/resource offsets:

```text
+0x3D  ambient/proximity sound
+0x3F  attack sound
+0x41  separate creature sound event, exact label unresolved
+0x43  death sound
```

They remain 16-bit values even though supplied high bytes are zero.

## Supplied-corpus validation

`inspect_npcs.py` verifies:

```text
records                     83
record size                 0x45
parse -> encode             PASS byte-for-byte
maximum name length          18
collision extents           0x0001 / 0x8000 / 0xFFFF only
nonzero hit-dice records     38
nonzero scene influence       1
nonzero render_aux_27          1
unresolved +0x3B nonzero       0
unresolved +0x3C nonzero       0
```

Flag analyzers verify:

```text
hold_undead_effect_immunity           25
hold_person_effect_immunity           31
hold_monster_effect_immunity          24
death_spell_effect_immunity           11
common_spell_effect_immunity           4
lightning_bolt_effect_immunity         5
cold_effect_immunity                  12
fire_effect_immunity                   6
weapon_category10_damage_floor_one    16
weapon_category9_damage_floor_one     14
poison_attack                           1
disease_attack                          3
undead_classification                  17
level_drain_attack                      5
strength_drain_attack                   2
paralysis_attack                        6
dynamic_barrier_activation              4
obstacle_passthrough                    5
```

## Documentation and tools

- `FORMAT.md` — physical schema and semantic reconstruction.
- `BEHAVIOR_FLAGS.md` — canonical proved flag ledger and unresolved boundary.
- `EFFECT_IMMUNITIES.md` — common/fire/cold spell-effect immunity mechanics.
- `HOLD_IMMUNITIES.md` — Hold Undead/Person/Monster immunity masks and shared live held state.
- `TARGETED_EFFECT_IMMUNITIES.md` — Lightning Bolt and Death Spell exclusion chains.
- `STATUS_ATTACKS.md` — poison/disease/paralysis party-status mapping and cure-dispatch proof.
- `WEAPON_DAMAGE_FLAGS.md` — category-9/category-10 one-damage gates and unresolved category-11 branch behavior.
- `inspect_npcs.py` — lossless parser/encoder and corpus audit.
- `analyze_behavior_flags.py` — raw bit distribution and proved semantic-set audit.
- `analyze_effect_immunities.py` — common/fire/cold immunity validation.
- `analyze_hold_immunities.py` — NPCS/OBJECTS/CODE.2 hold-family validation.
- `analyze_targeted_effect_immunities.py` — scroll/effect/moving-record/target-mask verification for Lightning Bolt and Death Spell.
- `analyze_status_attacks.py` — NPC status setters, cure handlers and spell-name validation.
- `analyze_weapon_damage_flags.py` — NPC/OBJECTS category sets and exact CODE.2 damage-gate signatures.

## Current frontier

The remaining NPCS work is narrower:

1. resolve additional raw `behavior_flags` bits through spell/effect/AI callers, with `0x00000004` the strongest current candidate because it directly modifies undead-turning resolution;
2. tighten the exact role of `+0x41` and `render_aux_27`;
3. document story-NPC-specific animation meanings only where code proves them;
4. preserve anomalous category-11 weapon-gate bits raw unless their intended source-level contract can be independently established.

The current schema supports a lossless reader/editor and faithful reproduction of the creature stat/render/effect database while preserving every unresolved bit verbatim.
