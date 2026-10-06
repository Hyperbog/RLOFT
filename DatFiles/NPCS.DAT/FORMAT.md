# NPCS.DAT format reconstruction

Status: **container and fixed 0x45-byte record layout proven; name, scene/render parameters, core combat statistics, XP, runtime-state templates, sound fields, and eighteen behavior/effect aliases are behaviorally anchored in certified CODE.2. Unknown flag bits remain raw.**

## Controlling evidence

- `NPCS.DAT`: 5,731 bytes
- SHA-256: `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b`
- CODE.2 loader: logical `16810..1686A`
- runtime NPC-definition pointer: DGROUP `0x22230`
- runtime NPC count: DGROUP `0x258F4`

LEVELS.DAT family A uses its `type_id` directly as an NPCS.DAT index and multiplies it by `0x45`, independently proving the same record stride.

## File layout

```text
+0x0000  uint32 npc_count = 83
+0x0004  npc_record npcs[npc_count]
```

Each definition is exactly `0x45` / 69 bytes:

```text
4 + 83 * 0x45 = 5731
```

CODE.2 independently reads the dword count, computes `count * 0x45`, allocates exactly that many bytes and reads the full table.

## Current record schema

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

The flag-field names are physical/behavioral. Individual aliases are exposed only where independent consumers prove them.

## Fixed name and scene fields

The first 20 bytes are a NUL-terminated fixed display-name field. All 83 supplied records terminate within the field; the longest supplied name is 18 characters.

First-person family-A scene emission uses `+0x14` as the base sprite/resource. `+0x16` is loaded as a 16-bit fixed-point collision half-extent and added/subtracted from live NPC coordinates to form bounding boxes. Supplied encoded values are exactly:

```text
0x8000 : 76
0x0001 :  4
0xFFFF :  3
```

They must not be normalized as ordinary integer sizes.

`+0x18..+0x1A` drive frame/state animation arithmetic. `+0x1B`, when nonzero, feeds the same scene-cell influence propagation helper used by OBJECTS.DAT; only PYRE ELEMENTAL stores `3`. `+0x1C` gates the alternate damage/reaction animation state.

## Core combat block: `+0x1D..+0x24`

Spawn/reset paths use `+0x1D` as hit-dice count. The attack path rolls melee damage directly from:

```text
count = npc[+0x1E]
sides = npc[+0x1F]
bonus = npc[+0x20]
```

`+0x21` is signed armor class; `+0x22` is the NPC attack target number/THAC0; death resolution passes `+0x23` directly to the party XP-distribution routine.

Examples:

```text
ZOMBIE          HD  4  AC 8  THAC0 17   1d8      XP  270
DOOM GUARD      HD  5  AC 2  THAC0 15   1d10     XP 2000
WEREBAT         HD  4  AC 5  THAC0 17   2d4      XP  420
PYRE ELEMENTAL  HD 12  AC 0  THAC0  9   3d8      XP 7000
HELLHOUND       HD  7  AC 4  THAC0 13   1d10+4   XP 1000
```

38 supplied definitions have nonzero hit dice.

## Action/render auxiliaries

`+0x25` is copied to live NPC word `+0x10` as an action/state countdown and later decremented to zero; its wall-clock unit is not yet assigned.

`+0x27` is copied to generated scene render-record field `+0x0C`; only PYRE ELEMENTAL stores a nonzero value (`3`). Its exact higher-level visual role remains unresolved.

CODE.2 indexes the four 16-bit entries at `+0x29` by live orientation/state and uses the selected value in spatial vector/placement calculations. They remain conservatively named `directional_offset[4]`.

## Behavior and runtime flags: `+0x31..+0x3A`

These fields are deliberately aliased across access widths.

### `+0x31..+0x34` — raw `uint32 behavior_flags`

CODE.2 tests this storage as byte, unaligned word, and full dword. Ten aliases are currently proved:

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

The executable constructs compound spell masks:

```text
0x0102  Hold Undead
0x0108  Hold Person
0x0110  Hold Monster
0x0300  Lightning Bolt
0x0900  cold effects
0x2100  fire effects
```

The common area/effect path skips a target whenever its `behavior_flags` intersects the caller's mask. Mage and priest Hold Person both use `0x0108`; Hold variants share the same live held-state bit. Fire/cold dispatcher paths independently establish the elemental components.

Lightning Bolt is independently closed through the serialized mage-scroll and moving-effect machinery. `OBJECTS.DAT` object 188 (`MAGE SCROLL OF 'LIGHTNING BOLT'`) maps to internal effect 28. Its handler creates moving-effect type 1; the type-1 NPC collision branch tests byte `npc+0x32` with mask `0x03`, which is dword mask `0x0300 = 0x0100 | 0x0200`. Thus `0x00000200` is a Lightning Bolt effect-immunity/exclusion bit in addition to the common `0x0100` gate.

Death Spell uses a separate dedicated bit. `OBJECTS.DAT` object 197 (`MAGE SCROLL OF 'DEATH SPELL'`) maps to internal effect 51, whose target-area routine tests byte `npc+0x31` with `0x80` and skips that target when set. Thus `0x00000080` is a Death Spell effect-immunity/exclusion bit rather than part of the common `0x0100` family.

See `TARGETED_EFFECT_IMMUNITIES.md` and `analyze_targeted_effect_immunities.py` for both complete chains.

The separate physical-attack gate at CODE.2 `0xC910..0xC99E` resolves OBJECTS.DAT category and enchantment before applying two high bits. Nonzero enchantment reroutes the item to category 11. On the remaining paths:

```text
category 10 + 0x00004000 -> temporary damage 0 -> final minimum 1
category  9 + 0x00008000 -> temporary damage 0 -> final minimum 1
```

Thus these aliases are **one-damage floors**, not zero-damage immunities. The supplied category-9 content is mace/quarterstaff/warhammer; category 10 contains ordinary axe/sword/dagger/spear/halberd definitions and variants. The aliases stay category-based rather than claiming an original AD&D damage-type enum.

The same category-11 branch tests `0x00020000` and `0x00040000` around zero-extended enchantment loads and signed comparisons to `-2`. Their exact executable oddity is documented in `WEAPON_DAMAGE_FLAGS.md`; those bits remain raw rather than being assigned speculative source-level names.

Preserve the full raw dword; these aliases are not a replacement enum. See `EFFECT_IMMUNITIES.md`, `HOLD_IMMUNITIES.md`, `TARGETED_EFFECT_IMMUNITIES.md`, and `WEAPON_DAMAGE_FLAGS.md`.

### `+0x35..+0x38` — initial runtime flags

NPC initialization copies this entire dword verbatim into live family-A record field `+0x08`. Byte `+0x38` is therefore the high byte of this state template, not an independent combat statistic.

### `+0x39` — secondary behavior/attack flags

Proved aliases:

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

`0x01` is closed through the party-status dispatcher: the NPC attack path sets status byte bit `0x08`; internal code `0x71` clears it; cleric effect ID `113` supplies that code; serialized OBJECTS.DAT object `223` is `CLERIC SCROLL OF 'NEUTRALIZE POISON'`.

`0x20` enters the paralysis path, including `RING OF FREE ACTION` protection. `0x40` permits dynamic barrier/door activation during NPC navigation. `0x80` suppresses blocking from placed/special obstacle collision cases.

### `+0x3A` — secondary classification/special-attack flags

Proved aliases:

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

`0x02` enters the class-level decrement/progression path. `0x04` calls the Strength-adjustment routine with `-1`, which contains the explicit `STONE GIANT GAUNTLETS` exception.

`0x10` is closed through the cure dispatcher: the attack path sets party status bit `0x04`; internal code `0x64` clears it; cleric effect ID `100` supplies that code; OBJECTS.DAT object `215` is `CLERIC SCROLL OF 'CURE DISEASE'`.

Turning, detection, and combat systems independently filter `0x20` as the undead classification.

A useful control case is paralysis:

```text
party status 0x04  internal code 0x64  Cure Disease
party status 0x08  internal code 0x71  Neutralize Poison
party status 0x10  internal code 0x68  Remove Paralysis
```

See `BEHAVIOR_FLAGS.md` and `STATUS_ATTACKS.md` for the complete executable ledger.

`+0x3B/+0x3C` are zero in all supplied records and remain preserved raw bytes.

## Sound IDs: `+0x3D..+0x44`

The final eight bytes are four little-endian 16-bit sound/resource IDs:

```text
+0x3D  ambient/proximity sound ID
+0x3F  attack sound ID
+0x41  separate sound-event ID; exact label unresolved
+0x43  death sound ID
```

CODE.2 probabilistically plays `+0x3D` during active/proximity behavior, uses `+0x3F` in the main attack path, plays `+0x41` through the creature sound resource base in a separate event path, and consumes `+0x43` on death with a base-sound fallback.

## Safe implementation boundary

A faithful reader/editor may expose the proved fields and aliases while retaining every original byte. In particular:

- preserve the fixed 20-byte name field exactly;
- keep `+0x16` as its exact 16-bit fixed-point extent encoding;
- preserve all animation/render fields even for noncombat/story definitions;
- expose HD, melee damage, AC, THAC0 and XP as proved stats;
- retain `behavior_flags` and `initial_runtime_flags` with their exact overlapping access semantics;
- reproduce the weapon-gate order, including enchantment rerouting and final minimum-one clamp;
- expose only independently proved bit aliases and preserve every other bit raw;
- preserve unresolved `+0x3B/+0x3C`;
- preserve sound IDs as 16-bit values.

The lossless parser/encoder is `inspect_npcs.py`. Semantic analyzers are indexed from `README.md`.
