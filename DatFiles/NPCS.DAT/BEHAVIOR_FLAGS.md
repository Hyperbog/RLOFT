# NPCS.DAT behavior/capability flags

Status: **twenty individual capability/effect bits are now behaviorally decoded from certified CODE.2. Remaining bits stay raw until their exact consumers are closed.**

The physical fields are:

```text
+0x31..+0x34  uint32 behavior_flags     // also accessed by byte/unaligned word
+0x39         uint8  behavior_flags_39
+0x3A         uint8  behavior_flags_3A
```

The executable is controlling evidence. Creature names and serialized spell names are cross-checks, not the source of semantic assignments.

## Proved aliases

### `behavior_flags` (`+0x31..+0x34`)

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

CODE.2 combines the common spell-effect gate with effect-specific bits:

```text
Hold Undead      0x0102 = 0x0100 | 0x0002
Hold Person      0x0108 = 0x0100 | 0x0008
Hold Monster     0x0110 = 0x0100 | 0x0010
Lightning Bolt   0x0300 = 0x0100 | 0x0200
cold effects     0x0900 = 0x0100 | 0x0800
fire effects     0x2100 = 0x0100 | 0x2000
```

Mage and priest Hold Person independently use `0x0108`. All Hold variants that apply set the same live NPC held-state bit; Slow uses only `0x0100` and a different live state. See `HOLD_IMMUNITIES.md`.

The common area/effect routine skips an NPC whenever its raw dword intersects the caller's immunity mask. Fire/cold spell dispatch and the original bestiary descriptions independently support the elemental aliases. See `EFFECT_IMMUNITIES.md`.

Lightning Bolt and Death Spell are closed through complete serialized-scroll -> internal-effect -> handler -> target-mask chains. See `TARGETED_EFFECT_IMMUNITIES.md`.

Detect Undead exclusion and the turning-roll modifier are closed through independent display/turning paths. See `DETECTION_TURNING_FLAGS.md`.

These names are intentionally behavioral. The Hold bits can encode target exclusion as well as intrinsic resistance, and the turning modifier is a latent executable behavior rather than a content taxonomy.

#### `0x00000001` — Detect Undead effect exclusion

`OBJECTS.DAT` object 170 is `MAGE SCROLL OF 'DETECT UNDEAD'`. Its scroll stub pushes internal effect ID 3; the spell jump table maps effect 3 to `0xBC2A`, which increments active-effect counter `0xF03C`.

The scene/render path later tests that counter. When the separate broader detection counter `0xF03E` is zero, `0x2C724` executes:

```text
test byte [npc_definition + 0x31], 0x01
jne  skip_detection_marker
```

Therefore the safe alias is:

```text
behavior_flags & 0x00000001  detect_undead_effect_exclusion
```

Supplied count: **17**.

This remains separate from `behavior_flags_3A & 0x20`, the independently proved undead classification used by turning/combat systems.

#### `0x00000004` — Turn Undead roll penalty of four

The turning routine `0xB518..0xB6F0` first requires the proved undead-classification bit and reads NPC hit dice. Immediately before its turning-table threshold comparison it executes:

```text
B6B0  test byte [npc_definition + 0x31], 0x04
B6B5  je   B6BC
B6B7  sub  turn_roll, 4
...
B6C3  compare threshold against turn_roll
```

Thus the bit makes an otherwise eligible undead harder to turn by reducing the effective roll exactly four points.

Safe alias:

```text
behavior_flags & 0x00000004  turn_undead_roll_penalty_4
```

Eighteen supplied definitions carry the bit, but **none** of the 17 supplied definitions marked `undead_classification` also carry it. The capability is therefore executable-supported but dormant in the shipped NPC table; do not claim those 18 serialized carriers are actually turn-resistant in play.

#### `0x00000080` — Death Spell effect immunity

`OBJECTS.DAT` object 197 is `MAGE SCROLL OF 'DEATH SPELL'`. The scroll dispatcher maps it to internal effect 51; the spell jump table sends effect 51 to `0xC5EA`, which calls target-area routine `0xAED8`.

For each eligible NPC, `0xAFCE` executes:

```text
test byte [npc_definition + 0x31], 0x80
jne  skip_target
```

The bit therefore excludes the NPC from Death Spell's application path.

Supplied count: **11**.

#### `0x00000200` — Lightning Bolt effect immunity

`OBJECTS.DAT` object 188 is `MAGE SCROLL OF 'LIGHTNING BOLT'`. The scroll dispatcher maps it to internal effect 28. Handler `0xC370` creates moving-effect type 1 through `0x8A80`; that constructor stores its first argument into transient-record word `+0x0E`.

The type-1 collision branch at `0x8E77` tests:

```text
test byte [npc_definition + 0x32], 0x03
```

Because byte `+0x32` is the second byte of the dword beginning at `+0x31`, this is:

```text
0x0300 = 0x0100 common_spell_effect_immunity
       | 0x0200 lightning_bolt_effect_immunity
```

A nonzero intersection skips the NPC effect/damage call.

Supplied count: **5**.

#### `0x00004000` / `0x00008000` — weapon-category damage floors

CODE.2 `0xC910..0xC99E` is called immediately before NPC damage application. It reads OBJECTS.DAT `item_category` (`+0x2F`) and `enchantment_bonus` (`+0x37`). Any nonzero enchantment is first rerouted to synthetic category 11; a weapon/object ID of `-1` is treated as category 9.

For the remaining category paths:

```text
category 10 + behavior_flags 0x00004000 -> damage = 0
category  9 + behavior_flags 0x00008000 -> damage = 0
```

The routine then clamps any result below 1 back to 1. These flags therefore impose a **one-damage floor**, not zero-damage immunity.

Supplied counts:

```text
weapon_category10_damage_floor_one  16
weapon_category9_damage_floor_one   14
```

See `WEAPON_DAMAGE_FLAGS.md` for the exact branch order and unresolved category-11 high-bit comparisons.

### `behavior_flags_39` (`+0x39`)

```text
0x01  poison_attack
0x20  paralysis_attack
0x40  dynamic_barrier_activation
0x80  obstacle_passthrough
```

#### `0x01` — poison attack

The NPC attack path tests `npc[+0x39] & 0x01` at logical `0xD947`. If the effect passes the common resistance/save check, it sets party status bit `0x08`.

The character-effect dispatcher handles internal code `0x71` by clearing that bit. Cleric effect ID `113` supplies code `0x71`; serialized OBJECTS.DAT object 223 is:

```text
CLERIC SCROLL OF 'NEUTRALIZE POISON'
```

Therefore `0x01` is safely `poison_attack`. Only `DARKLING` carries it in the supplied table.

#### `0x20` — paralysis attack

The main attack path tests this bit and the protection path explicitly honors `RING OF FREE ACTION`. Successful application sets the paralysis status. Remove Paralysis independently clears the same party-status bit.

Marked definitions:

```text
GHAST
GHOUL
GHOUL LORD
INQUISITOR
BONE GOLEM
REVENANT
```

#### `0x40` — dynamic barrier activation

NPC navigation checks this bit when movement resolves a dynamic barrier/door record. Flagged NPCs may invoke the decoded barrier activation routine rather than always treating the barrier as blocking.

Marked definitions:

```text
ZOMBIE
DOOM GUARD
SKELETON
BRIGAND
```

#### `0x80` — obstacle passthrough

Collision/navigation code uses this bit to suppress blocking from placed/special obstacles.

Marked definitions:

```text
SKELETAL BAT
WRAITH
ATMOSPHERE BAT
ATMOSPHERE RAT
ATMOSPHERE CROW
```

Do not rename it `flying`; the encoded behavior is collision-specific and includes `ATMOSPHERE RAT`.

### `behavior_flags_3A` (`+0x3A`)

```text
0x02  level_drain_attack
0x04  strength_drain_attack
0x10  disease_attack
0x20  undead_classification
```

#### `0x02` — level drain

The attack path calls the routine that decrements one of the target character's class-level words and updates associated progression/experience state.

Marked definitions:

```text
STRAHD
VAMPIRE
SPECTRE
WRAITH
WIGHT
```

#### `0x04` — Strength drain

The attack path calls the character Strength-adjustment routine with `-1`. That helper contains the explicit `STONE GIANT GAUNTLETS` exception.

Marked definitions:

```text
SHADOW
INQUISITOR
```

#### `0x10` — disease attack

The attack path sets party status bit `0x04`. Internal cure code `0x64` clears exactly that bit; cleric effect ID `100` supplies that code; OBJECTS.DAT object 215 is `CLERIC SCROLL OF 'CURE DISEASE'`.

Supplied definitions:

```text
ZOMBIE LORD
GHOUL LORD
INQUISITOR
```

#### `0x20` — undead classification

Turning/repulsion, detection and combat paths independently filter this bit as the undead class. Seventeen supplied definitions carry it.

## Cure/status control mapping

The status-removal dispatcher gives an independent control set:

```text
party status 0x04  internal code 0x64  Cure Disease
party status 0x08  internal code 0x71  Neutralize Poison
party status 0x10  internal code 0x68  Remove Paralysis
```

The corresponding cleric dispatcher IDs are `100`, `113`, and `104`; serialized scroll definitions are OBJECTS IDs `215`, `223`, and `219`.

## Remaining `behavior_flags` dword

CODE.2 continues to read `+0x31..+0x34` using overlapping widths. Remaining notable raw cases include:

```text
C96D  dword +0x31 & 0x00020000 in category-11 weapon path
C980  dword +0x31 & 0x00040000 in category-11 weapon path
```

Those category-11 tests use zero-extended enchantment bytes followed by signed comparisons to `-2`. Their exact binary behavior is documented in `WEAPON_DAMAGE_FLAGS.md`, but the intended source-level contract is not proved, so the bits remain raw.

Many other bits remain a generalized effect/capability/immunity family. Preserve the complete dword and do not flatten aliases into an enum.

## Supplied bit distributions

```text
behavior_flags (+0x31 dword)
0x00000001 17   0x00000002 25   0x00000004 18   0x00000008 31
0x00000010 24   0x00000020 22   0x00000040 18   0x00000080 11
0x00000100  4   0x00000200  5   0x00000400 16   0x00000800 12
0x00001000  4   0x00002000  6   0x00004000 16   0x00008000 14
0x00010000 15   0x00020000  3   0x00040000  1   0x00080000  1

behavior_flags_39
0x01 1   0x02 7   0x04 8   0x08 1
0x10 2   0x20 6   0x40 4   0x80 5

behavior_flags_3A
0x02 5   0x04 2   0x10 3   0x20 17
```

## Safe implementation boundary

A high-level model may expose these aliases while retaining the exact raw fields:

```text
behavior_flags & 0x00000001  detect_undead_effect_exclusion
behavior_flags & 0x00000002  hold_undead_effect_immunity
behavior_flags & 0x00000004  turn_undead_roll_penalty_4
behavior_flags & 0x00000008  hold_person_effect_immunity
behavior_flags & 0x00000010  hold_monster_effect_immunity
behavior_flags & 0x00000080  death_spell_effect_immunity
behavior_flags & 0x00000100  common_spell_effect_immunity
behavior_flags & 0x00000200  lightning_bolt_effect_immunity
behavior_flags & 0x00000800  cold_effect_immunity
behavior_flags & 0x00002000  fire_effect_immunity
behavior_flags & 0x00004000  weapon_category10_damage_floor_one
behavior_flags & 0x00008000  weapon_category9_damage_floor_one

behavior_flags_39 & 0x01      poison_attack
behavior_flags_39 & 0x20      paralysis_attack
behavior_flags_39 & 0x40      dynamic_barrier_activation
behavior_flags_39 & 0x80      obstacle_passthrough

behavior_flags_3A & 0x02      level_drain_attack
behavior_flags_3A & 0x04      strength_drain_attack
behavior_flags_3A & 0x10      disease_attack
behavior_flags_3A & 0x20      undead_classification
```

All remaining bits stay raw until their executable semantics are independently closed.