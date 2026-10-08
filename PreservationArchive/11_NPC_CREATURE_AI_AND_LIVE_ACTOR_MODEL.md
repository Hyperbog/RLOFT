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


## 34. Hold-effect eligibility is definition-driven

Three low bits specialize the common spell-effect gate:

```text
0x00000002  hold_undead_effect_immunity
0x00000008  hold_person_effect_immunity
0x00000010  hold_monster_effect_immunity
```

The encoded compound masks are:

```text
Hold Undead    0x0102
Hold Person    0x0108
Hold Monster   0x0110
```

where `0x0100` is the independently proved common spell-effect immunity bit. [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

These are effect-specific exclusion gates. They should not be reinterpreted as one creature-category enum.

## 35. Cleric and Mage Hold Person converge on the same live state

The shared/Mage Hold Person handler and prepared-priest Hold Person handler are separate dispatch paths, but both test:

```text
0x0108
```

and both install the same live held-state bit when successful.

The Cleric Hold Person **scroll** deliberately reuses dispatch ID 25 rather than prepared-priest dispatch 92. [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

Thus:

```text
spell source/dispatch
    may differ

definition eligibility
    same mask

live applied state
    same runtime hold bit
```

## 36. Common/fire/cold immunity masks are compositional

The common area/effect traversal performs:

```text
if ((effect_mask & npc.behavior_flags) != 0)
    skip target
```

Fire paths use:

```text
0x2100
= 0x2000 fire_effect_immunity
| 0x0100 common_spell_effect_immunity
```

Cold paths use:

```text
0x0900
= 0x0800 cold_effect_immunity
| 0x0100 common_spell_effect_immunity
```

[Effect immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md)

Do not collapse these into one generic `magic_resistance` property.

## 37. Supplied elemental-immunity counts

Across the 83 definitions:

```text
common spell-effect immunity   4
cold-effect immunity          12
fire-effect immunity           6
```

[NPCS effect immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md)

Overlaps are valid.

For example, a creature may carry:

```text
common
+
cold
+
fire
```

simultaneously.

## 38. Lightning Bolt has a dedicated additional exclusion bit

The complete chain is:

```text
OBJECTS 188
MAGE SCROLL OF 'LIGHTNING BOLT'
        |
        v
dispatch 28
        |
        v
class-3 moving effect type 1
        |
        v
NPC collision path
        |
        v
test behavior_flags mask 0x0300
```

where:

```text
0x0300 =
0x0100 common_spell_effect_immunity
|
0x0200 lightning_bolt_effect_immunity
```

[Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

Five supplied definitions carry the Lightning-specific bit.

## 39. Death Spell has an independent exclusion bit

The chain is:

```text
OBJECTS 197
MAGE SCROLL OF 'DEATH SPELL'
        |
        v
dispatch 51
        |
        v
target-area routine
        |
        v
test npc_definition +0x31 byte bit 0x80
```

This is:

```text
behavior_flags & 0x00000080
    death_spell_effect_immunity
```

[Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

Unlike Lightning Bolt, Death Spell does **not** include the common `0x0100` bit in its encoded target test.

## 40. Detect Undead exclusion and undead classification are separate

Two different flags exist:

```text
behavior_flags & 0x00000001
    detect_undead_effect_exclusion

behavior_flags_3A & 0x20
    undead_classification
```

[Detection/turning flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)

The first controls one detection marker/display path.

The second is the classification used by turning/repulsion and other undead-sensitive systems.

Do not infer:

```text
detect-undead visible
==
undead classification
```

from either bit alone.

## 41. Turn Undead requires classification first

Turning resolution:

1. rejects targets without `undead_classification`;
2. derives its roll/table state;
3. tests behavior bit `0x00000004`;
4. if set, subtracts exactly 4 from the favorable turn roll;
5. performs threshold comparison.

[Detection/turning flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)

Safe alias:

```text
turn_undead_roll_penalty_4
```

## 42. The turning modifier is dormant in the supplied NPC table

Eighteen supplied definitions carry `0x00000004`.

Seventeen definitions carry the separate undead-classification bit.

But **none** carries both.

Therefore the executable supports the modifier, but the supplied data never exercises it for a target that passes the undead-classification prerequisite. [Detection/turning flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)

This is latent executable capability, not evidence that those 18 creatures are “turn resistant.”

## 43. NPC melee status effects are separate from base damage

The ordinary melee attack uses definition:

```text
+0x1E  damage dice
+0x1F  damage sides
+0x20  damage bonus
```

After/beside the normal attack path, capability bits can apply additional status effects.

Proved secondary attack families include:

- poison;
- paralysis;
- disease;
- level drain;
- Strength drain.

These effects use separate resistance/save/eligibility logic and should not be folded into the base damage dice.

## 44. Poison attack

```text
behavior_flags_39 & 0x01
```

On successful application, the creature attack sets party status high-byte bit:

```text
0x08
```

Neutralize Poison later clears exactly that bit. [Status attacks](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/STATUS_ATTACKS.md)

Only supplied:

```text
DARKLING
```

carries this attack bit.

## 45. Paralysis attack

```text
behavior_flags_39 & 0x20
```

sets the party paralysis condition when its effect path succeeds.

The attack path explicitly honors Ring of Free Action protection.

Remove Paralysis clears the same status bit. [NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

Supplied carriers:

```text
GHAST
GHOUL
GHOUL LORD
INQUISITOR
BONE GOLEM
REVENANT
```

## 46. Disease attack

```text
behavior_flags_3A & 0x10
```

sets party status high-byte bit:

```text
0x04
```

and Cure Disease clears the same bit. [Status attacks](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/STATUS_ATTACKS.md)

Supplied carriers:

```text
ZOMBIE LORD
GHOUL LORD
INQUISITOR
```

The executable contains an encoded class-code-1 exemption before the common effect application path. Because class code 1 is independently Paladin, a native faithful implementation should preserve that exact exemption while avoiding broader inferred tabletop generalization.

## 47. Level-drain attack

```text
behavior_flags_3A & 0x02
```

causes the attack path to invoke the character level/progression reduction routine.

Supplied carriers:

```text
STRAHD
VAMPIRE
SPECTRE
WRAITH
WIGHT
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

The exact class-slot reduction behavior belongs to the combat/progression chapter; the NPC-side capability identity is closed here.

## 48. Strength-drain attack

```text
behavior_flags_3A & 0x04
```

calls the character Strength-adjustment routine with `-1`.

Supplied carriers:

```text
SHADOW
INQUISITOR
```

[NPCS behavior flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

The character Strength helper contains an explicit Stone Giant Gauntlets exception, creating a direct NPC-attack ↔ equipment interaction.

## 49. Weapon-category defenses are damage floors, not immunity

Before applying physical weapon damage to an NPC, CODE.2:

1. derives the weapon's OBJECTS category;
2. treats `weapon_id == -1` as category 9;
3. if object enchantment is nonzero, reroutes the hit to synthetic category 11;
4. applies NPC category-specific gates;
5. clamps resulting damage to at least 1.

[Weapon damage flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

Therefore the proved category 9/10 flags do **not** produce zero damage.

They reduce the post-gate hit to the engine's minimum one damage.

## 50. Category-9 one-damage floor

```text
behavior_flags & 0x00008000
```

applies to unenchanted category-9 weapon path.

The supplied category-9 family includes:

```text
MACE
QUARTERSTAFF
WARHAMMER
```

and related definitions. [Weapon damage flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

Safe name remains category-based rather than inferred as `blunt_resistance`.

## 51. Category-10 one-damage floor

```text
behavior_flags & 0x00004000
```

applies to unenchanted category-10 path.

That object family includes ordinary:

- axes;
- swords;
- daggers;
- pole/thrusting weapons.

[Weapon damage flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

Again, the executable category is controlling. Do not rename the flag to one guessed tabletop damage type.

## 52. Enchanted weapons enter synthetic category 11

For a nonnegative object ID:

```text
category = OBJECTS.item_category

if OBJECTS.enchantment_bonus != 0:
    category = 11
```

before the NPC weapon-gate branch. [Weapon damage flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

This means the category-9/category-10 resistance bits apply only to unenchanted definitions.

A native combat implementation must preserve this ordering.

## 53. Category-11 high-bit behavior includes a shipped signedness anomaly

Two additional main-dword bits are consumed:

```text
0x00020000
0x00040000
```

but their source-level intent is unresolved.

The executable:

- zero-extends the object's enchantment byte;
- compares against sign-extended immediate `-2`;
- uses signed conditional branches.

[Weapon damage flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

For the supplied `0..255` enchantment domain:

- `0x00020000` drives category-11 damage to the one-damage floor;
- `0x00040000` does not independently reduce supplied-object damage.

This exact machine behavior must be preserved in faithful mode rather than “fixing” the apparent signedness mismatch silently.

## 54. Creature attack sound is definition-driven

Definition `+0x3F` is used by normal and special attack paths.

Playback resource:

```text
creature_sound_base 0x258C0
+
attack_sound_id
```

is queued through the common sound request path. [Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

This sound selection belongs to the creature template rather than the live actor record.

## 55. Damage-reaction sound occurs only while the actor survives

Common NPC damage/effect routine tests live:

```text
current_hp > 0
```

after applying its HP/state changes.

If the actor remains alive, it resolves:

```text
creature_sound_base 0x258C0
+
damage_reaction_sound_id
```

from definition `+0x41`. [Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

The name is intentionally broader than `pain_sound` because the routine is also a common damage/effect handler.

## 56. Death sound uses a separate resource base

When live HP is zero or negative, the common handler enters death processing.

Definition:

```text
+0x43 death_sound_id
```

is resolved against:

```text
death_sound_base 0x258BC
```

rather than ordinary creature base `0x258C0`. [Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

If `death_sound_id == 0`, the engine queues the base death sound itself.

Thus:

```text
zero death sound ID
    != silence
```

## 57. Sound corpus organization

Across the 83 definitions:

```text
ambient/proximity nonzero   33
attack nonzero              34
damage reaction nonzero     35
death nonzero                9
```

Thirty-two definitions have exact consecutive triplets:

```text
ambient  = N
attack   = N+1
reaction = N+2
```

Examples:

```text
ZOMBIE          6 / 7 / 8
SKELETON        9 / 10 / 11
WEREBAT        15 / 16 / 17
PYRE ELEMENTAL 33 / 34 / 35
WEREWOLF       58 / 59 / 60
BONE GOLEM     82 / 83 / 84
LIVING WALL    94 / 95 / 96
```

[Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

The corpus pattern corroborates, but does not define, the executable-proved contexts.

## 58. Death is driven by live HP

The live actor field:

```text
+0x12 int16 current_hp
```

is tested by the common damage/death routine.

```text
current_hp > 0
    survivor reaction path

current_hp <= 0
    death resolution path
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)

Death therefore belongs to the mutable actor state, not an immutable NPC-definition flag.

## 59. Definition XP is consumed on death

Definition `+0x23` is passed to the party XP-distribution path during death resolution. [NPCS README](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/README.md)

Thus the ownership is:

```text
NPC definition
    XP reward amount

live actor
    HP/death state

death handler
    consumes definition XP
    -> party progression system
```

The later combat/progression chapter documents the recipient/distribution arithmetic.

## 60. Death handling can contain special/story branches

The central death path is not guaranteed to be only:

```text
HP <= 0
-> sound
-> XP
-> remove
```

CODE.2 contains special story/death convergence before final audio and cleanup for selected actors. [Sound fields](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

Additionally, level event/story logic can scan family-A actors by type and alter live runtime suppression state, as demonstrated by the FINAL STRAHD path. [LEVELS event callees](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/EVENT_CALLEES.md)

A modern actor destructor should not bypass these scripted/special transitions.

## 61. Level header contains three NPC-type filters

Level header:

```text
+0x292
+0x294
+0x296
```

stores three signed NPC type filters.

They are compared directly to family-A `npc_type_id` values. `-1` is predominantly the no-match sentinel. [LEVELS header tail](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/HEADER_TAIL.md)

This is another level ↔ NPCS namespace connection beyond ordinary placement.

The exact higher-level purpose of each filter is documented only to the proved call sites; do not invent editor names.

## 62. Residual main behavior bits must survive editing

The supplied main behavior dword populates every low bit `0..19`.

Proved semantic mask:

```text
0x0000EB9F
```

Residual populated mask:

```text
0x000F1460
```

[Residual flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md)

Six populated bits have no proved semantic consumer:

```text
0x00000020
0x00000040
0x00000400
0x00001000
0x00010000
0x00080000
```

They are real serialized state, not padding.

## 63. Two residual main bits have exact behavior but unknown intent

```text
0x00020000
0x00040000
```

are consumed in category-11 weapon handling.

Their exact machine behavior is preserved, but the intended original labels remain unknown. [Residual flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md)

This is a useful preservation category:

```text
behavior known
source intent/name unknown
```

## 64. Residual +0x39 bits must also survive

All eight bits of `behavior_flags_39` occur in the supplied table.

Proved mask:

```text
0xE1
```

Residual populated mask:

```text
0x1E
= 0x02 | 0x04 | 0x08 | 0x10
```

[Residual flags](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md)

These four bits must remain raw/preserved.

## 65. +0x3A has no populated residual bits

Supplied populated mask:

```text
0x36
```

which is exactly the proved semantic mask:

```text
0x02 level drain
0x04 Strength drain
0x10 disease
0x20 undead
```

Thus every populated bit in `+0x3A` is semantically accounted for in this corpus.

## 66. +0x3B/+0x3C remain raw zero bytes

Both bytes are zero in all 83 supplied records.

No direct NPC-definition consumer has been proved.

[NPCS final audit](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md)

They remain:

```text
unresolved_3B
unresolved_3C
```

not guaranteed padding.

## 67. Raw flag storage must remain authoritative

A bad native representation would be:

```text
struct NpcCapabilities {
    bool fire_immune;
    bool cold_immune;
    ...
}
```

with serialization regenerated only from known booleans.

That would destroy residual bits.

The faithful boundary should instead be:

```text
uint32 behavior_flags;
uint8  behavior_flags_39;
uint8  behavior_flags_3A;

named semantic accessors
    view/test bits without replacing raw storage
```

## 68. Live runtime flags require the same preservation discipline

Definition `initial_runtime_flags` is copied into live actor `runtime_flags`, then modified by:

- status application;
- movement/action logic;
- story/wordcode;
- visibility/suppression behavior.

A standalone LEV file serializes the live dword directly.

Therefore the live actor field should also remain raw-authoritative with named bit views layered above it.

## 69. Suggested native NPC architecture

A faithful modern implementation can separate:

```text
LegacyNpcDefinition
    exact 0x45-byte NPCS record

NpcDefinitionView
    visual fields
    collision
    combat stats
    capability aliases
    audio IDs

LegacyNpcLiveRecord
    exact 0x16-byte LEV-compatible actor state

NpcActor
    fixed-point transform
    current HP
    facing
    action/status countdown
    mutable runtime flags
    cached cell

NpcNavigationPolicy
    dynamic barrier activation
    obstacle passthrough
    common level collision

NpcCombatProfile
    THAC0
    damage dice
    special attacks
    immunity/resistance aliases

NpcPresentationProfile
    type-7 shape base
    animation fields
    render scale bias
    sounds
```

These are proposed native abstractions, not recovered DreamForge source names.

## 70. Template and actor must not share mutable fields

A strict port should not mutate:

```text
NPCS definition initial_runtime_flags
NPCS action_delay
NPCS hit_dice_count
```

when one actor is held, damaged, slowed, hidden, or killed.

Those definition values initialize or parameterize the live actor.

Each family-A actor owns its own:

```text
runtime_flags
countdown
HP
position
facing
```

## 71. Required NPC regression classes

### 71.1 Database identity

Verify:

```text
size      5,731
count     83
stride    69
SHA-256   certified value
```

and byte-exact decode/encode.

### 71.2 Name-tail preservation

Use records with nonzero post-NUL residue and ensure a no-op parse/reencode preserves every byte.

### 71.3 Definition-to-live expansion

For representative definitions verify:

- initial runtime flags copy;
- fixed-point position;
- cached cell;
- facing packing;
- HP initialization;
- actor type.

### 71.4 Standalone LEV restoration

Load a direct actor snapshot and ensure:

- HP is not rerolled;
- position is not reset;
- facing is not restored from baseline;
- countdown is not recreated from definition delay.

### 71.5 Collision extent

Test all three supplied encodings:

```text
0x8000
0x0001
0xFFFF
```

with original-width arithmetic.

### 71.6 Scene animation

Exercise shape base, frame stride, cycle divisor, animation span, and damage-reaction animation gate.

### 71.7 Render scale bias

Compare Pyre Elemental value 3 against ordinary zero-bias NPCs through the type-7 lookup path.

### 71.8 Action countdown

Verify definition delay installation, live decrement, and persistence through LEV save/load.

### 71.9 Hold eligibility

Construct targets carrying:

```text
0x0100
0x0002
0x0008
0x0010
combinations
```

and verify Hold Undead/Person/Monster independently.

### 71.10 Hold live state

After successful hold:

```text
live runtime bit 0x08
countdown installed
```

and preserve direct LEV serialization.

### 71.11 Slow state

Verify Slow's distinct runtime bit `0x80` and common-immunity test.

### 71.12 Fire/cold immunity

Test masks `0x2100` and `0x0900` with common-only, element-only, and combined definitions.

### 71.13 Lightning immunity

Test `0x0300` against class-3 effect type 1.

### 71.14 Death Spell

Verify dedicated `0x80` exclusion independent of common `0x0100`.

### 71.15 Detect Undead

Verify the dedicated exclusion bit affects detection marker/display without replacing undead classification.

### 71.16 Turning

Test:

- non-undead target rejection;
- undead target normal roll;
- synthetic undead + penalty-bit target for latent `-4` behavior.

### 71.17 Barrier navigation

Test dynamic barrier encounter with and without `behavior_flags_39 & 0x40`.

### 71.18 Obstacle passthrough

Test placed/special obstacle collision with and without `0x80`.

### 71.19 Status attacks

Exercise:

- poison;
- paralysis;
- disease;
- level drain;
- Strength drain.

Verify corresponding character-state mutations and protection/cure interactions.

### 71.20 Weapon resistance floors

Test unenchanted category 9 and 10 against matching/nonmatching flags and require minimum one damage rather than zero.

### 71.21 Enchanted reroute

Add nonzero enchantment and verify category 11 is selected before the category-9/10 gates.

### 71.22 Category-11 anomaly

Preserve exact zero-extend/signed-comparison behavior for `0x20000/0x40000`.

### 71.23 Audio

Verify separate:

- ambient/proximity;
- attack;
- surviving damage-reaction;
- death.

Test zero death ID fallback.

### 71.24 Death/XP

Reduce live HP to zero and verify death path reads definition XP rather than actor-local XP.

### 71.25 Residual flags

Inject/preserve every residual populated main and +0x39 bit through edit and runtime definition access.

### 71.26 LEV022 actor fixture

Require exact reproduction of the supplied Assassin's:

```text
x cell 18
y cell 7
flags 0x10000000
cell 228
countdown 4
HP 15
facing 6
```

## 72. Correction C-16 — NPCS.DAT is not live actor state

A creature's current:

- HP;
- position;
- facing;
- countdown;
- status/runtime flags;

does **not** live in NPCS.DAT.

NPCS is a static template database.

The mutable state lives in family-A records and is serialized by LEV snapshots.

A native implementation that directly mutates NPCS definitions during gameplay would incorrectly share state among every actor of that type.

## 73. Correction C-17 — behavior flags are not one monster-type enum

The three behavior fields simultaneously encode:

- effect eligibility;
- immunities;
- damage-floor behavior;
- navigation capability;
- special attacks;
- undead classification;
- unresolved legacy capacity.

They are overlapping bitfields, not one exclusive monster taxonomy.

## 74. Correction C-18 — weapon gates are not complete immunity

The proved category-9/category-10 flags force the intermediate damage to zero, but the same routine subsequently clamps damage to a minimum of one.

Therefore the externally meaningful behavior is:

```text
one-damage floor
```

not:

```text
immune to category
```

This ordering must remain explicit in the future combat implementation.

## 75. Historical unknowns retained

Bounded remaining questions include:

1. source-level names for six populated main behavior bits with no proved semantic consumer;
2. source-level intent/names for category-11 bits `0x20000/0x40000`;
3. source-level names for residual `+0x39` bits `0x02/0x04/0x08/0x10`;
4. original editor/source name for `directional_offset[4]`;
5. original source name for `scene_influence_step`;
6. whether alternate builds assign semantics to `+0x3B/+0x3C`;
7. finer historical labels for story/ambient animation states;
8. original high-level AI-state names around countdown/navigation behavior.

None prevents faithful parsing, actor reconstruction, navigation, rendering, status attacks, audio, combat gating, or LEV persistence.

## 76. Source ledger

### NPCS definition

- [DatFiles/NPCS.DAT/README.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/README.md)
- [DatFiles/NPCS.DAT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FORMAT.md)
- [DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/FINAL_AUDIT_2026-10-06.md)

### Behavior/effects

- [DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)
- [DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md)
- [DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md)
- [DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)
- [DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)
- [DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)
- [DatFiles/NPCS.DAT/STATUS_ATTACKS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/STATUS_ATTACKS.md)
- [DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

### Rendering/audio/world state

- [DatFiles/NPCS.DAT/RENDER_AUX_27.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/RENDER_AUX_27.md)
- [DatFiles/NPCS.DAT/SOUND_FIELDS.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/NPCS.DAT/SOUND_FIELDS.md)
- [DatFiles/LEVELS.DAT/FAMILY_A.md](https://github.com/Hyperbog/RLOFT/blob/f63ea449ed5c761ae6416f9e13966efd056c81c6/DatFiles/LEVELS.DAT/FAMILY_A.md)
- [Component 05 world state](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)
- [Component 04 renderer](https://github.com/Hyperbog/RLOFT/blob/1e7d4ec3d6d1d0aa098eb7c26ff4454783463340/PreservationArchive/04_VGA_RENDERING_PIPELINE.md)

## 77. Architectural checkpoint

The creature/NPC architecture is now:

```text
NPCS.DAT
    83 immutable definitions
        |
        +-- scene/animation template
        +-- collision extent
        +-- combat stats
        +-- action delay
        +-- capability/immunity raw flags
        +-- special attack raw flags
        +-- sound IDs
        |
        v
LEVELS family A compact placement
        |
        v
live 22-byte actor
        |
        +-- 16.16 position
        +-- runtime flags
        +-- cached cell
        +-- type ID
        +-- countdown
        +-- current HP
        +-- packed facing/state
        |
        +----------------+----------------+----------------+
        |                |                |                |
        v                v                v                v
   navigation        rendering        combat/status      audio
        |                |                |                |
   barriers           RES3 class0      THAC0/damage      ambient
   obstacles          animation        immunities         attack
   collision          type7 draw       drain/status       reaction
                                        death/XP          death
        |
        v
LEVnnn.DAT
    mutable actor persistence
```

The next preservation component should document **combat resolution**: player and NPC attack selection, THAC0/Armor Class rolls, damage dice, weapon category/enchantment routing, saves/resistance checks, ranged/thrown attacks, status application, absorption/protection pools, death, XP distribution, hand/action timing, and combat-facing integration with the item/NPC systems already reconstructed.
