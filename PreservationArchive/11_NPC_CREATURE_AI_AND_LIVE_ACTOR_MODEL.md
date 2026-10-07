# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 11 — NPC/Creature Definitions, Live Actors, Navigation, and Special Behavior

**Status:** PRIMARY NPC/CREATURE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter reconstructs the creature/NPC system from static definition through live world actor.

It covers:

- the 83-record `NPCS.DAT` definition table;
- LEVELS family-A placement and live 22-byte actor state;
- fixed-point position, facing, HP, countdowns, and runtime flags;
- scene-shape and animation inputs;
- collision extent and scene-cell influence;
- THAC0, AC, hit dice, melee damage, and XP;
- navigation through dynamic barriers and obstacles;
- immutable capability/effect flags;
- live held/slow/status state;
- effect and weapon immunities/resistances;
- poison, paralysis, disease, level drain, and Strength drain attacks;
- undead classification and turning behavior;
- attack/damage/death sound selection;
- death and XP state transitions;
- residual/unknown behavior bits;
- faithful-port implementation boundaries.

The later combat chapter documents complete player/NPC attack resolution. This chapter records the NPC-definition and live-actor state that the combat system consumes.

The supplied `NPCS.DAT` was independently rechecked during this preservation pass:

```text
size      5,731 bytes
records   83
stride    0x45 / 69 bytes
SHA-256   f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b
```

This matches the certified repository identity exactly. [NPCS README](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/README.md)

## 2. NPCS.DAT is the immutable creature-definition database

File geometry:

```text
+0x0000  uint32 npc_count = 83
+0x0004  NpcDefinition69 npc[83]
```

Exact size:

```text
4 + 83 * 69 = 5,731
```

CODE.2 loader `16810..1686A` allocates and reads exactly `count * 0x45` bytes. [NPCS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FORMAT.md)

LEVELS family A stores a type ID and indexes this table directly:

```text
definition = NPCS[type_id]
```

All 1,258 supplied family-A records use IDs `0..82`.

## 3. Canonical 69-byte definition

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

[NPCS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FORMAT.md)

All 69 physical bytes are structurally accounted for.

## 4. Name tails are preservation evidence

The first 20 bytes form the active NUL-terminated name.

All 83 supplied names terminate within the field and the longest active name is 18 characters.

However, 20 records contain nonzero bytes after the first NUL, including:

- `0x0D` residue;
- stale text fragments.

[NPCS final audit](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md)

Runtime name consumers ignore those bytes, but byte-faithful serialization must preserve them unless the raw field is intentionally edited.

## 5. Definition and live actor are different structures

The world-state chain is:

```text
NPCS.DAT
    immutable creature template

LEVELS.DAT family A
    compact baseline placement

live family-A record
    mutable actor state

LEVnnn.DAT family A
    serialized live actor state
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

The static definition is not modified when an actor moves, takes damage, changes facing, is held, or is hidden by story code.

## 6. Compact family-A placement

Archive form:

```text
+0x00  uint16 npc_type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_orientation
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

This is authored baseline placement, not the full actor state.

## 7. Live family-A actor record

The 22-byte live/direct-save form is:

```text
+0x00  uint32 y_position_16_16
+0x04  uint32 x_position_16_16
+0x08  uint32 runtime_flags
+0x0C  uint16 cached_cell_index
+0x0E  uint16 npc_type_id
+0x10  uint16 action_state_countdown
+0x12  int16  current_hp
+0x14  uint16 packed_runtime_state
```

Runtime array base:

```text
0x14D80
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

The standalone LEV writer copies these records directly.

## 8. Actor coordinates are 16.16 fixed point

Archive expansion constructs:

```text
y = (y_cell << 16) | y_fraction
x = (x_cell << 16) | x_fraction
```

Movement/AI paths mutate those dwords.

The live cached cell is:

```text
cell_index =
    high16(y_position) * map_width
  + high16(x_position)
```

and is refreshed when movement crosses a cell boundary.

Sub-cell actor movement is therefore persistent state.

## 9. Facing is packed into live state

Compact orientation low three bits become:

```text
packed_runtime_state bits 3..5
```

Safe extraction:

```text
facing = (packed_runtime_state >> 3) & 7
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

Runtime turning code rewrites the same bits.

Other bits in `+0x14` have additional roles, so the field must remain a raw packed word with a facing view rather than a standalone facing enum.

## 10. Definition runtime flags become mutable actor flags

On baseline expansion:

```text
live.runtime_flags =
    NPCS[npc_type].initial_runtime_flags
```

Definition field:

```text
NPCS +0x35..+0x38
```

is therefore a template.

Gameplay and wordcode modify the live `+0x08` copy.

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

Story code can, for example, alter suppression/visibility state in the live actor without changing the definition table.

## 11. Action delay and live countdown are separate

Definition:

```text
NPCS +0x25  action_delay
```

Live actor:

```text
+0x10 action_state_countdown
```

Archive expansion initially writes zero. Actor reset/action paths later install the definition delay, and active behavior decrements the live value toward zero. [Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

Thus:

```text
definition delay
    = reusable template value

live countdown
    = mutable actor timing state
```

The exact wall-clock unit is not inferred.

## 12. Hit dice initialize current HP

Definition `+0x1D` is the hit-dice count used by spawn/reset logic.

For ordinary archive initialization, CODE.2 reads it, invokes the random/dice helper, and stores the result in:

```text
live actor +0x12 current_hp
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

NPC type 8 has a special initialization path that writes `0x37`.

A standalone LEV load restores the serialized current HP directly and must not reroll it.

## 13. LEV022 proves actor mutation

Archive level 22 contains one NPC:

```text
type       9  ASSASSIN
x cell    13
y cell     7
facing     2
```

The supplied standalone snapshot stores:

```text
type                    9
x cell                 18
y cell                  7
runtime flags   0x10000000
cached cell            228
countdown                4
current HP              15
facing                   6
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

This independently proves persistence of:

- movement;
- facing;
- HP;
- action timing;
- runtime flags.

## 14. Scene shapes use RES3 source class 0

Definition `+0x14` is the local shape base for RES3 source class 0.

Certified class-0 bank:

```text
RES3 0..2168
2169 entries
```

Because its base is zero:

```text
absolute_RES3_id =
    local_shape_id
```

numerically in this build. [NPCS final audit](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md)

The architectural identity still remains bank-local.

## 15. NPC appearance uses compiled type-7 scene shapes

The NPC scene system does not load a conventional bitmap sprite.

The RES3 class-0 entries are compiled type-7 scene shapes described in Component 04.

Animation/state logic applies offsets to the local shape base before the common type-7 renderer processes the selected shape.

Thus the NPC visual model is:

```text
NPC definition
    local shape base
    animation/state fields
        |
        v
class-0 RES3 type-7 shape
        |
        v
SINTAB/type-7 renderer
```

## 16. Collision half extent is encoded fixed-point state

Definition `+0x16` is used as a fixed-point collision half extent.

The supplied values are only:

```text
0x8000   76 definitions
0x0001    4 definitions
0xFFFF    3 definitions
```

[NPCS final audit](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md)

CODE.2 adds/subtracts the encoded value from live coordinates to form collision bounds.

Do not normalize these values into ordinary unsigned integer sizes.

## 17. Animation control fields

```text
+0x18  scene_frame_stride
+0x19  animation_cycle_divisor
+0x1A  animation_span
+0x1C  damage_reaction_animation_flag
```

[NPCS format](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FORMAT.md)

These feed scene-frame/state arithmetic and the alternate damage/reaction animation path.

The exact original artist/editor labels are not claimed; the arithmetic behavior is controlling.

## 18. Scene influence propagation

Definition `+0x1B` feeds the same scene-cell influence helper used by OBJECTS.

The encoded footprint is:

```text
step 1  base cell
step 2  base + 8 immediate neighbors
step 3  base + immediate ring + 16-cell outer ring
```

Only:

```text
PYRE ELEMENTAL
```

uses nonzero supplied value `3`.

Do not merge this with the nearby renderer scale bias; they enter different systems.

## 19. NPC renderer scale lookup bias

Definition `+0x27` is copied to generated scene-record `+0x0C`.

Renderer `36CEC` then:

1. derives an index from projected size;
2. adds this value;
3. indexes the table at runtime pointer `0x258C4`.

[Render bias](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RENDER_AUX_27.md)

Safe name:

```text
render_scale_lookup_bias
```

Again, only Pyre Elemental uses nonzero supplied value `3`.

## 20. Four directional offsets

```text
+0x29  uint16 directional_offset[4]
```

are indexed by live orientation/state and used in spatial vector/placement calculations.

The executable proves the indexed spatial use but not an original editor/source name.

Retain the conservative name and exact words.

## 21. Core combat template

The definition-level combat block is:

```text
+0x1D  hit_dice_count
+0x1E  melee_damage_dice
+0x1F  melee_damage_sides
+0x20  melee_damage_bonus
+0x21  signed armor_class
+0x22  thac0
+0x23  experience_award
```

[NPCS README](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/README.md)

Thirty-eight supplied definitions have nonzero hit dice.

Story/ambient definitions with zero combat values remain valid definitions and must not be filtered out as empty records.

## 22. Example creature definitions

```text
ZOMBIE
    HD 4
    AC 8
    THAC0 17
    damage 1d8
    XP 270

DOOM GUARD
    HD 5
    AC 2
    THAC0 15
    damage 1d10
    XP 2000

WEREBAT
    HD 4
    AC 5
    THAC0 17
    damage 2d4
    XP 420

PYRE ELEMENTAL
    HD 12
    AC 0
    THAC0 9
    damage 3d8
    XP 7000

HELLHOUND
    HD 7
    AC 4
    THAC0 13
    damage 1d10+4
    XP 1000
```

[NPCS README](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/README.md)

These are direct static-definition values, not manual-derived estimates.

## 23. Behavior flags are capability views over raw storage

Three definition fields own behavior/capability bits:

```text
+0x31..+0x34  uint32 behavior_flags
+0x39         uint8  behavior_flags_39
+0x3A         uint8  behavior_flags_3A
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

The raw fields are authoritative.

Named aliases are views.

A faithful serializer must not rebuild the raw values solely from named booleans because populated residual bits exist.

## 24. Main behavior-flags aliases

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

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

These aliases describe exact executable behavior, not inferred AD&D taxonomy.

## 25. Secondary attack/navigation flags

`behavior_flags_39` proved aliases:

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

All eight bits are populated in the supplied corpus, but only these four have direct semantic consumers.

## 26. Classification/special-attack flags

`behavior_flags_3A`:

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

The supplied populated mask is exactly `0x36`; every populated bit in this byte is semantically decoded.

## 27. Dynamic barrier activation is a navigation capability

When NPC navigation reaches a dynamic barrier/door, actors with:

```text
behavior_flags_39 & 0x40
```

can invoke the decoded barrier activation path instead of necessarily treating the barrier as blocking.

Supplied carriers:

```text
ZOMBIE
DOOM GUARD
SKELETON
BRIGAND
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

This links creature navigation directly to the dynamic barrier system documented in Component 05.

## 28. Obstacle passthrough is collision behavior, not a flying taxonomy

Actors with:

```text
behavior_flags_39 & 0x80
```

suppress blocking from selected placed/special obstacles.

Supplied carriers:

```text
SKELETAL BAT
WRAITH
ATMOSPHERE BAT
ATMOSPHERE RAT
ATMOSPHERE CROW
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

The presence of ATMOSPHERE RAT is decisive evidence against renaming this flag to simply `flying`.

The safe semantic name is the exact movement behavior:

```text
obstacle_passthrough
```

## 29. NPC navigation and world collision remain shared systems

Family-A actors move through the same level coordinate/cell space as the party and family-C items.

Their movement can encounter:

- ordinary blocking cell codes;
- special/dynamic cells;
- dynamic barrier records;
- placed/special obstacles;
- other live world-state constraints.

Creature capability flags modify those decisions rather than replacing the common world representation.

This is an important port architecture boundary:

```text
LevelCollisionModel
    shared topology/dynamic state

NpcNavigationPolicy
    actor-specific capability modifiers
```

## 30. Live hold state uses actor runtime flags

Hold Person, Hold Undead, and Hold Monster have different definition eligibility masks, but successful application converges on the same live actor state:

```text
live.runtime_flags_low |= 0x08
```

and installs a duration/countdown in live word `+0x10`.

[Hold immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

This demonstrates the static/live distinction:

```text
definition flags
    determine whether effect may apply

live runtime flags/countdown
    represent the applied effect state
```

## 31. Slow uses a different live runtime bit

The Slow effect tests only the common definition immunity gate `0x0100`.

When it applies, it sets:

```text
live runtime bit 0x80
```

rather than the hold bit `0x08`. [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

Static immunity bits and mutable live effect bits therefore occupy separate structures and separate namespaces.

## 32. Ambient/proximity activity is actor behavior

Definition `+0x3D` is used inside active/proximity behavior.

The path:

1. resolves the active actor's definition;
2. loads `ambient_or_proximity_sound_id`;
3. adds creature-sound base `0x258C0`;
4. probability-gates playback;
5. queues the sound.

[Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

This gives a proved active-behavior output without assigning a broader AI-state name than the executable supports.

## 33. NPC template/live-state checkpoint

The actor system at this stage is:

```text
NPCS.DAT definition
    |
    +-- visual/animation data
    +-- collision extent
    +-- combat stats
    +-- action delay
    +-- capability/immunity flags
    +-- special attack flags
    +-- sound IDs
    |
    v
LEVELS family-A baseline
    placement + type + orientation
    |
    v
live 22-byte actor
    |
    +-- fixed-point position
    +-- mutable runtime flags
    +-- cached cell
    +-- NPC type
    +-- action/status countdown
    +-- current HP
    +-- packed facing/runtime state
    |
    v
LEVnnn.DAT mutable persistence
```

The second half of this chapter closes combat/status capability behavior, immunities/resistances, sounds, death/XP, residual flags, and native-port regression requirements.
