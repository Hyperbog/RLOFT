# NPCS.DAT format reconstruction

Status: **ARCHIVAL-COMPLETE FOR THE SUPPLIED BUILD. All 69 physical bytes are accounted for; certified CODE.2 behavior, LEVELS linkage, RES3 class-0 shape identity, audio routing, raw residual flags, and byte-exact semantic decode→encode are reconciled.**

## Controlling evidence

- `NPCS.DAT`: 5,731 bytes
- SHA-256: `f2dd1175bf42e8291b95952282dfc0cfb84455aaa532987cdd7380094c8eb30b`
- CODE.2 loader: logical `16810..1686A`
- runtime NPC-definition pointer: DGROUP `0x22230`
- runtime NPC count: DGROUP `0x258F4`

`LEVELS.DAT` family A uses its `type_id` directly as an NPCS.DAT index and multiplies it by `0x45`, independently proving the same record stride.

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

## Record schema

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

The flag-field names are physical/behavioral. Individual aliases are exposed only where independent executable consumers prove them.

## Fixed name and scene fields

The first 20 bytes are a NUL-terminated fixed display-name field. All 83 supplied records terminate within the field; the longest name is 18 characters. Twenty records contain nonzero residual bytes after the first NUL terminator, including `0x0D` bytes and stale text fragments. Runtime C-string consumers stop at the first NUL, but a preservation-grade serializer must retain the remaining physical bytes unchanged unless an intentional raw-field edit is made.

First-person family-A scene emission uses `+0x14` as the class-0 bank-local compiled-shape base. The class-0 bank is `RES3:0..2168`, so the bank translation is numerically invisible in this build (`absolute_RES3_id = 0 + local_shape_id`). `+0x16` is loaded as a 16-bit fixed-point collision half-extent and added/subtracted from live NPC coordinates to form bounding boxes. Supplied encodings are:

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

## Action/render auxiliaries

`+0x25` is copied to live NPC word `+0x10` as an action/state countdown and later decremented toward zero. Its wall-clock unit is intentionally not inferred.

`+0x27` is executable-closed as `render_scale_lookup_bias`. NPC scene builder `2BACE` copies it verbatim to generated scene-record word `+0x0C`. Draw paths pass that word as renderer `36CEC` argument 4. Inside `36CEC`, it is added to an index derived from projected size before the word table at runtime pointer `0x258C4` is indexed. Only PYRE ELEMENTAL stores a nonzero supplied value (`3`). See `RENDER_AUX_27.md`.

CODE.2 indexes the four 16-bit values at `+0x29` by live orientation/state and uses the selected value in spatial vector/placement calculations. They remain conservatively named `directional_offset[4]`.

## Behavior and runtime flags: `+0x31..+0x3A`

These fields deliberately alias across access widths.

### `+0x31..+0x34` — raw `uint32 behavior_flags`

Twelve semantic aliases are proved:

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

Known compound masks include:

```text
0x0102  Hold Undead
0x0108  Hold Person
0x0110  Hold Monster
0x0300  Lightning Bolt
0x0900  cold effects
0x2100  fire effects
```

The common area/effect routine at `B2FB` loads the complete dword and intersects it with a caller-supplied mask. All five supplied CODE.2 callers use only `0x2100` or `0x0900`, so this generic path consumes only the decoded common/fire/cold bits.

The physical weapon gate `C910..C99E` loads the dword once, then handles decoded category-9/category-10 bits plus two unresolved-intent category-11 bits (`0x00020000` and `0x00040000`). The latter behavior must be preserved exactly; see `WEAPON_DAMAGE_FLAGS.md`.

The supplied corpus populates every dword bit `0..19`:

```text
populated mask = 0x000FFFFF
proved mask    = 0x0000EB9F
residual mask  = 0x000F1460
```

`RESIDUAL_FLAGS.md` partitions those residuals into six stored bits with no proved semantic consumer and two category-11 bits with exact executable behavior but unknown intended source labels.

### `+0x35..+0x38` — initial runtime flags

NPC initialization copies this entire dword verbatim into live family-A record field `+0x08`. Byte `+0x38` is therefore the high byte of this state template, not an independent statistic.

### `+0x39` — secondary behavior/attack byte

Proved aliases:

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

The supplied byte uses all eight bits. Residual bits `0x02`, `0x04`, `0x08`, and `0x10` have no proved direct NPC-definition branch and must remain raw. See `RESIDUAL_FLAGS.md`.

### `+0x3A` — classification/special-attack byte

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

The supplied populated mask is exactly `0x36`; therefore every populated `+0x3A` bit is semantically decoded.

`+0x3B/+0x3C` are zero in all supplied records and no NPC-definition consumer has been proved. They remain preserved raw bytes, not assumed mandatory padding.

## Sound IDs: `+0x3D..+0x44`

The final eight bytes are four distinct little-endian 16-bit sound/resource IDs:

```text
+0x3D  ambient_or_proximity_sound_id
+0x3F  attack_sound_id
+0x41  damage_reaction_sound_id
+0x43  death_sound_id
```

Executable contexts:

- `+0x3D`: probability-gated active/proximity behavior;
- `+0x3F`: normal and special attack paths;
- `+0x41`: common damage/effect handler when live HP remains positive after the operation;
- `+0x43`: death-resolution path, with a distinct death-sound resource base and zero-value fallback.

`+0x3D/+0x3F/+0x41` use creature-sound base `0x258C0`; `+0x43` uses death-sound base `0x258BC`. Thirty-two supplied definitions encode consecutive ambient/attack/reaction triplets. See `SOUND_FIELDS.md`.

## Status/effect controls

Several independently decoded paths provide useful cross-checks:

```text
party status 0x04  Cure Disease
party status 0x08  Neutralize Poison
party status 0x10  Remove Paralysis
```

Lightning Bolt is closed through serialized OBJECTS entry 188 -> internal effect 28 -> moving-effect type 1 -> compound NPC mask `0x0300`. Death Spell is closed through OBJECTS entry 197 -> effect 51 -> target routine `AED8` -> direct `+0x31 & 0x80` exclusion. Detect Undead and turning have separate dedicated paths documented in `DETECTION_TURNING_FLAGS.md`.

## Safe implementation boundary

A faithful reader/editor may expose the proved fields and aliases while retaining every original byte. In particular:

- preserve the fixed 20-byte name field exactly;
- keep `+0x16` as its exact 16-bit fixed-point extent encoding;
- preserve animation/render values even for story/ambient definitions;
- expose HD, melee damage, AC, THAC0 and XP as proved stats;
- implement `render_scale_lookup_bias` as the exact type-7 lookup-index adjustment, not a guessed visual property;
- retain `behavior_flags` and `initial_runtime_flags` as exact raw dwords with alias views;
- reproduce effect-mask, turning, navigation and weapon-gate ordering exactly;
- retain residual bits identified by `RESIDUAL_FLAGS.md`;
- preserve `+0x3B/+0x3C`;
- preserve all sound IDs as 16-bit values and their separate resource bases.

The lossless semantic parser/encoder is `inspect_npcs.py`; semantic analyzers and evidence modules are indexed from `README.md`. Final certification is recorded in `FINAL_AUDIT_2026-10-06.md`.
