# NPCS.DAT

Status: **fixed 83 × 0x45 record format proven; scene/render parameters, collision extent, core combat statistics, XP, action delay, runtime-state templates and creature sound IDs reconstructed; lossless parse/encode verified. Behavior-flag bits remain the main semantic frontier.**

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

## Core combat fields

CODE.2 directly establishes:

```text
+0x1D                 hit-dice count used to initialize live NPC HP
+0x1E/+0x1F/+0x20    melee damage dice / sides / bonus
+0x21                 signed armor class
+0x22                 THAC0 / attack target number
+0x23                 experience award distributed on death
```

Examples from the supplied table:

```text
ZOMBIE          HD  4  AC 8  THAC0 17   1d8      XP  270
DOOM GUARD      HD  5  AC 2  THAC0 15   1d10     XP 2000
WEREBAT         HD  4  AC 5  THAC0 17   2d4      XP  420
PYRE ELEMENTAL  HD 12  AC 0  THAC0  9   3d8      XP 7000
HELLHOUND       HD  7  AC 4  THAC0 13   1d10+4   XP 1000
```

38 of the 83 definitions have nonzero hit dice. Story/ambient definitions generally leave the combat block zero.

## Scene/render fields

The base scene sprite is `+0x14`. `+0x18..+0x1A` drive frame/state arithmetic in the first-person scene emitter.

`+0x16` is used as the half-extent of the live NPC's fixed-point collision rectangle. Supplied values are:

```text
0x8000 : 76
0x0001 :  4
0xFFFF :  3
```

`+0x1B` uses the same scene-cell propagation helper as OBJECTS.DAT `scene_influence_step`; only PYRE ELEMENTAL has a nonzero value (`3`).

`+0x27` is copied to a generated render-record auxiliary field and is likewise nonzero only for PYRE ELEMENTAL (`3`). Its higher-level name remains unresolved.

## Packed runtime flags

Two important access-width boundaries are now explicit:

```text
+0x31..+0x34  uint32 behavior_flags
+0x35..+0x38  uint32 initial_runtime_flags
```

`behavior_flags` is also tested through byte and unaligned-word aliases. `initial_runtime_flags` is copied verbatim into the live 22-byte family-A world record on NPC initialization.

Therefore byte `+0x38` is **not** an independent damage statistic despite its varied high-bit values; it is the high byte of the live-state template.

`+0x39/+0x3A` are additional capability/behavior flag bytes and are the next major semantic target.

## Sound fields

The four final words are all executable-consumed sound/resource offsets:

```text
+0x3D  ambient/proximity sound
+0x3F  attack sound
+0x41  separate creature sound event, exact label unresolved
+0x43  death sound
```

They are 16-bit little-endian fields even though all supplied high bytes are zero.

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

## Documentation and tools

- `FORMAT.md` — physical schema and first semantic reconstruction pass.
- `inspect_npcs.py` — lossless parser/encoder and corpus audit.

## Current frontier

The next NPCS work is to decode the packed behavior/capability masks without flattening their overlapping byte/word/dword accesses:

1. `+0x31..+0x34` effect/immunity/AI behavior bits;
2. `+0x39/+0x3A` attack/status/capability bits;
3. tighten the exact role of `+0x41` and `render_aux_27`;
4. document any story-NPC-specific animation meanings only where code proves them.

The current schema already supports a lossless reader/editor and faithful reproduction of the core creature stat/render database.