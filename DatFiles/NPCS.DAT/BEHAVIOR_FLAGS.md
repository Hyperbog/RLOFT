# NPCS.DAT behavior/capability flags

Status: **six individual capability bits are now behaviorally decoded from certified CODE.2. The remaining bits stay raw until their exact effect/AI consumers are closed.**

This document extends `FORMAT.md`. The relevant physical fields are:

```text
+0x31..+0x34  uint32 behavior_flags     // also accessed by byte/unaligned word
+0x39         uint8  behavior_flags_39
+0x3A         uint8  behavior_flags_3A
```

The executable remains controlling evidence. Creature-name correlations below are validation, not the source of the semantic names.

## Proven `+0x3A` flags

### `0x20` — undead classification

This bit is consumed by several independent CODE.2 systems.

At logical `B5C7` and `B717`, the engine filters family-A NPCs on `npc[+0x3A] & 0x20` in the party's turning/repulsion-style effect path. The same path reads `hit_dice_count` and compares it to party/class progression before applying the effect. Other combat/detection paths at `D52D`, `D78B`, and `1B72B` test the same bit.

The supplied database marks exactly 17 definitions:

```text
ZOMBIE
SKELETON
SKELETAL BAT
SHADOW
ZOMBIE LORD
VAMPIRE
GHAST
GHOUL
GHOUL LORD
INQUISITOR
SPECTRE
WRAITH
BONE GOLEM
REVENANT
SHADOW FIEND
WIGHT
ZOMBIE GOLEM
```

Safe semantic name:

```text
undead_classification
```

### `0x02` — level-drain attack

The main NPC attack routine tests this bit at logical `D7B3`. When the target is not protected, it calls `D1E0`.

`D1E0` selects one of the character's class-level words at runtime `0x21900/+2/+4`, decrements the selected level and updates the associated progression/experience state. This directly proves a level-drain special attack.

Marked definitions:

```text
STRAHD
VAMPIRE
SPECTRE
WRAITH
WIGHT
```

Safe name:

```text
level_drain_attack
```

### `0x04` — Strength-drain attack

The attack routine tests this bit at logical `D7E8` and eventually invokes `1202C(actor,-1)` on an unprotected target.

`1202C` adjusts the character byte at `0x21913`, clamps it to 20, and contains the explicit equipped-item exception for object `0x54` (`STONE GIANT GAUNTLETS`). The same helper is used to restore/read that ability value. This identifies the affected statistic as Strength.

Marked definitions:

```text
SHADOW
INQUISITOR
```

Safe name:

```text
strength_drain_attack
```

### `0x10` — unresolved status attack

This bit is used by the attack routine at logical `D967`; on an unprotected target it sets live party status bit `0x04`. One character-class value (`+0x218F5 == 1`) is explicitly immune before the saving/effect test.

Only these supplied definitions carry the bit:

```text
ZOMBIE LORD
GHOUL LORD
INQUISITOR
```

The exact named condition is not yet executable-proved, so the bit remains unresolved rather than being labeled from AD&D expectations.

## Proven `+0x39` flags

### `0x20` — paralysis attack

The main attack routine tests this bit at logical `D8A5`. Before applying the condition it checks for the relevant target protection, including equipped object `0x45`, `RING OF FREE ACTION`. Successful application enters the live immobilizing/status path.

Marked definitions:

```text
GHAST
GHOUL
GHOUL LORD
INQUISITOR
BONE GOLEM
REVENANT
```

The Free Action protection plus the attack/status consumer make `paralysis_attack` a safe project name.

### `0x40` — dynamic-barrier activation capability

NPC navigation tests this bit at logical `2D767` after the cell collision path resolves a dynamic barrier record. If the bit is present, the NPC may invoke the already-decoded 10-byte dynamic-barrier activation routine rather than simply treating the barrier as permanently blocking.

Marked definitions:

```text
ZOMBIE
DOOM GUARD
SKELETON
BRIGAND
```

Safe behavioral name:

```text
dynamic_barrier_activation
```

This is deliberately narrower than a source-level name such as `opens_doors`: the proved operation is interaction with the dynamic barrier/door record family.

### `0x80` — obstacle/special-collision passthrough

Collision handling at logical `2CE8A` tests this bit after a placed-object collision candidate has been found. When set, the function changes the collision result to nonblocking. A second navigation path at `2D7FB` uses the same bit to suppress blocking for another special-obstacle case.

Marked definitions:

```text
SKELETAL BAT
WRAITH
ATMOSPHERE BAT
ATMOSPHERE RAT
ATMOSPHERE CROW
```

Safe behavioral name:

```text
obstacle_passthrough
```

Do not rename this bit `flying`; the supplied `ATMOSPHERE RAT` record demonstrates that the encoded behavior is collision-related rather than a universal locomotion taxonomy.

### `0x01` — unresolved status attack

At logical `D947`, this bit gates an attack effect which sets live party status bit `0x08` after an effect/saving check. Only `DARKLING` carries it in the supplied database. The exact named status remains unresolved.

## `+0x31..+0x34`: generalized effect/immunity mask family

The four bytes beginning at `+0x31` must remain one aliased 32-bit field physically.

CODE.2 uses it in several forms:

```text
B2FB   load dword +0x31 and intersect with a caller-supplied effect mask
B477   test byte  +0x32 with mask 0x21
C15F   test byte  +0x32 with mask 0x01
C1B4   test word  +0x31 with mask 0x0102
C23C   test word  +0x31 with mask 0x0108
C2AF   test word  +0x31 with mask 0x0108
C317   test word  +0x31 with mask 0x0110
AFCE   test byte  +0x31 bit 0x80 in an area/effect path
B6B0   test byte  +0x31 bit 0x04 while computing an undead-turning threshold
```

This establishes the field as a generalized capability/effect-resistance/immunity mask family, but it does **not** yet justify assigning individual spell/immunity names to most bits. The exact overlapping access widths must be preserved.

## Supplied bit distributions

Across all 83 definitions:

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

Bits absent from the supplied database remain representable raw data and must not be normalized.

## Safe implementation boundary

A high-level model may expose these proved aliases while retaining the original raw fields:

```text
behavior_flags_3A & 0x20  undead_classification
behavior_flags_3A & 0x02  level_drain_attack
behavior_flags_3A & 0x04  strength_drain_attack

behavior_flags_39 & 0x20  paralysis_attack
behavior_flags_39 & 0x40  dynamic_barrier_activation
behavior_flags_39 & 0x80  obstacle_passthrough
```

Keep `+0x31..+0x34`, the remaining `+0x39` bits, and `+0x3A & 0x10` available as raw flags until their exact executable semantics are closed.