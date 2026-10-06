# NPCS.DAT format reconstruction

Status: **container and fixed 0x45-byte record layout proven; name, scene/render parameters, core combat statistics, XP, runtime-state templates, and sound fields are behaviorally anchored in certified CODE.2. Flag bits remain raw unless independently decoded.**

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

CODE.2 `16810` independently reads the dword count, computes `count * 0x45`, allocates exactly that many bytes and reads the full table.

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

The names above are behavioral. In particular, the various flag fields are kept broad until individual bit semantics are proved.

## `+0x00`: fixed 20-byte name

All 83 supplied records contain a NUL terminator inside the first 20 bytes. The longest supplied name is 18 characters.

The early table begins:

```text
0  ZOMBIE
1  DOOM GUARD
2  WEREBAT
3  PYRE ELEMENTAL
4  SKELETON
5  WORG
6  SKELETAL BAT
7  HELLHOUND
8  STRAHD
```

The 20-byte extent is also structurally fixed by the first executable field at `+0x14`.

## Scene sprite and collision extent: `+0x14/+0x16`

First-person family-A scene emission at CODE.2 `2B721..2BA4F` copies the word at `+0x14` into the generated render record as its base sprite/resource value.

`+0x16` is independently loaded as a word in collision/proximity paths such as `8DD4..8E2C`; CODE.2 subtracts it from and adds it to the live NPC X/Y coordinates to construct a bounding box. It is therefore a collision/spatial half-extent in the engine's fixed-point coordinate domain.

Supplied `+0x16` values are:

```text
0x8000 : 76 records
0x0001 :  4 records
0xFFFF :  3 records
```

These are exact encoded extents/sentinels and must not be normalized.

## Animation fields: `+0x18..+0x1C`

The renderer uses `+0x18` as the frame stride when adding state/orientation-dependent offsets to `scene_sprite_base`.

`+0x19` participates both as an animation-group multiplier and as the divisor/modulus used when advancing a live NPC's orientation/phase.

`+0x1A` supplies the additional animation span in the transient/reaction animation branches.

`+0x1B`, when nonzero, is passed to the same `2B088` scene-cell propagation routine already identified for OBJECTS.DAT `scene_influence_step`. Only one supplied NPC uses it:

```text
PYRE ELEMENTAL -> 3
```

`+0x1C` is tested by the NPC damage/reaction routine `E284`; when nonzero the routine sets the live world record into the alternate reaction-animation state. The safe name is therefore `damage_reaction_animation_flag` rather than a content-specific animation label.

## Core combat block: `+0x1D..+0x24`

### Hit dice

Spawn/reset paths call the common dice routine using byte `+0x1D` as the dice count and six or eight sides depending on the encoded path, storing the result in the live NPC HP/state word. `hit_dice_count` is therefore a safe behavioral name.

38 supplied definitions have nonzero hit dice; the remaining story/ambient definitions store zero.

### Melee damage

The NPC attack path at `D767..D77D` calls the common dice routine with:

```text
count = npc[+0x1E]
sides = npc[+0x1F]
bonus = npc[+0x20]
```

This directly proves the three-byte melee damage profile.

Examples:

```text
ZOMBIE          1d8
DOOM GUARD      1d10
WORG            2d4
HELLHOUND       1d10+4
BONE GOLEM      3d8
```

### Armor class and THAC0

Combat hit-resolution code reads `+0x21` as a signed byte and incorporates it into the attack roll needed to hit the NPC. The supplied values align with the creature armor-class values; `armor_class` is therefore the project name.

The reciprocal NPC-attack resolution path reads byte `+0x22` and subtracts target/character modifiers before comparing against the rolled attack value. Supplied values align with the expected THAC0 progression, so `thac0` is supported terminology.

### Experience award

NPC death resolution loads the word at `+0x23` and passes it directly to `DDA4`, the routine that distributes experience among active party members. This proves `experience_award`.

Examples from the supplied database:

```text
WORG             120
ZOMBIE            270
WEREBAT           420
HELLHOUND        1000
DOOM GUARD       2000
PYRE ELEMENTAL   7000
```

## `+0x25`: action delay

The NPC behavior update at `1BDC9..1BEE0` copies the word at `+0x25` into live world-record field `+0x10` as the countdown following an action/state transition, and later decrements that live word until zero. `action_delay` is therefore a safe behavioral name; its wall-clock unit is not yet assigned.

## `+0x27` and `+0x29..+0x30`

`+0x27` is copied to generated scene render-record field `+0x0C`; only PYRE ELEMENTAL stores a nonzero supplied value (`3`). Its higher-level visual identity remains unresolved, so it stays `render_aux_27`.

CODE.2 indexes four 16-bit entries beginning at `+0x29` by a live orientation/state selector and passes the selected value into spatial vector/placement calculations. They are exposed conservatively as:

```text
uint16 directional_offset[4]
```

without assigning a stronger geometry/editor name.

## Behavior and runtime flags: `+0x31..+0x3A`

These bytes are heavily consumed as packed flags and deliberately alias across access widths.

### `+0x31..+0x34`

CODE.2:

- tests byte `+0x31` bits directly;
- tests the unaligned word at `+0x31` with masks such as `0x0102`, `0x0108`, and `0x0110`;
- loads the complete unaligned dword at `+0x31` and intersects it with requested effect/behavior masks.

Therefore these four bytes are modeled physically as one raw `uint32 behavior_flags`, while documentation may later expose proved bit aliases.

### `+0x35..+0x38`

NPC creation/reset code copies the complete dword at `+0x35` verbatim into live family-A world-record field `+0x08`. It is therefore an **initial runtime-flags/state template**, not a damage-dice block.

This overlapping dword interpretation is controlling: byte `+0x38` is the high byte of the initial runtime template and must not be independently normalized.

### `+0x39/+0x3A`

These are additional behavior/capability flag bytes. Multiple systems test individual bits, including `0x01/0x20/0x40/0x80` in `+0x39` and `0x02/0x04/0x10/0x20` in `+0x3A`.

Bit-level gameplay names remain a later semantic pass.

`+0x3B/+0x3C` are zero in all supplied records and no stronger role is assigned yet. They remain preserved raw bytes.

## Sound IDs: `+0x3D..+0x44`

The last eight bytes are four little-endian sound/resource IDs. Their high bytes are zero in all supplied records, but CODE.2 reads them as words and they must remain words.

```text
+0x3D  ambient/proximity sound ID
+0x3F  attack sound ID
+0x41  separate sound-event ID; exact event label unresolved
+0x43  death sound ID
```

Evidence:

- `1B99F..1B9BD` probabilistically plays `+0x3D` while an NPC is active/nearby;
- the main NPC attack path `DA05..DA1A` plays `+0x3F`;
- `E553..E570` plays `+0x41` through the same creature sound resource base;
- the death-resolution path `E4C9..E508` plays `+0x43`, falling back to the base sound when zero.

## Safe implementation boundary

A faithful reader/editor may expose the fields above while retaining every original byte. In particular:

- preserve the fixed 20-byte name field exactly;
- keep `+0x16` as its exact 16-bit fixed-point extent value;
- preserve all animation fields even for noncombat/story NPCs;
- expose hit dice, melee damage, armor class, THAC0 and XP as proved stat fields;
- retain `behavior_flags` and `initial_runtime_flags` with their exact unaligned/overlapping access semantics;
- do not split the initial-runtime-flags dword into invented combat stats;
- preserve unresolved `+0x3B/+0x3C` and all unknown flag bits;
- preserve sound IDs as 16-bit values.