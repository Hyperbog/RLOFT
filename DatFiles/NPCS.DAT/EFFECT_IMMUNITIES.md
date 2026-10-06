# NPCS.DAT spell/effect immunity masks

Status: **three bits in the aliased `behavior_flags` dword at record `+0x31` are now behaviorally decoded from certified CODE.2: a common spell-effect immunity gate, cold-effect immunity, and fire-effect immunity.**

The immutable CODE.2 object remains controlling evidence. The original game manual is used only to corroborate the identity of spell dispatcher entries after the executable has established the mask mechanics.

## Physical field

```text
+0x31..+0x34  uint32 behavior_flags
```

This dword is also read through byte and unaligned-word aliases. The new names below are aliases on individual bits; they do not replace the raw 32-bit value.

## Area/effect mask mechanism

CODE.2 routine `B02C` is the common area/effect traversal used by several offensive spells and charged-device effects.

For each family-A NPC found in the affected cells it resolves the NPCS.DAT definition, loads:

```text
npc.behavior_flags = dword(npc + 0x31)
```

and performs the decisive test at logical `B2FB..B302`:

```text
if ((effect_immunity_mask & npc.behavior_flags) != 0)
    skip_this_npc;
```

Only when the intersection is zero does the routine call the NPC damage/effect handler at `E284`.

Therefore every bit supplied in the caller mask is a complete immunity gate for that particular effect path.

## `0x00002000` — fire-effect immunity

Two independent mage-spell dispatcher entries pass mask `0x2100` to `B02C`:

```text
B990 / effect id 1   -> mask 0x2100
B9E6 / effect id 10  -> mask 0x2100
```

The first computes damage as:

```text
2 * caster_level + 1d3
```

and the second uses `3d6` in the sustained/ranged path.

The executable's spell-effect index construction at `23592` is:

```text
effect_id = field_1E * 10
          + field_1F
          + field_21 * 80
```

For the mage spell table these two IDs align with the rule-book order for **Burning Hands** (first-level mage spell index 1) and **Agannazer's Scorcher** (second-level mage spell index 0). The manual independently describes both as fire/flame attacks and gives the same characteristic damage forms.

Because `0x2100 == 0x2000 | 0x0100`, bit `0x2000` is the element-specific portion of the fire mask.

Safe alias:

```text
behavior_flags & 0x00002000  fire_effect_immunity
```

Supplied records carrying the bit:

```text
PYRE ELEMENTAL
HELLHOUND
BANSHEE
SHADOW FIEND
ZOMBIE GOLEM
LIVING WALL
```

The rule-book bestiary independently states that Pyre Elementals are immune to fire-based spells and attacks, corroborating the executable-derived interpretation.

## `0x00000800` — cold-effect immunity

The corresponding cold-oriented dispatcher paths pass mask `0x0900`:

```text
BABB / effect id 31  -> mask 0x0900
BA60 / effect id 53  -> mask 0x0900
```

Effect id 31 is the fourth-level mage spell index 1 and executes a `3d10` area-damage path, matching **Ice Storm** in the game's spell ordering and manual damage description. Effect id 53 is the sixth-level mage spell index 3, matching **Otiluke's Freezing Sphere** in the same ordering.

Because `0x0900 == 0x0800 | 0x0100`, bit `0x0800` is the element-specific portion of the cold mask.

Safe alias:

```text
behavior_flags & 0x00000800  cold_effect_immunity
```

Supplied records carrying the bit:

```text
ZOMBIE
SKELETON
STRAHD
SHADOW
ZOMBIE LORD
VAMPIRE
BANSHEE
SPECTRE
WRAITH
SHADOW FIEND
WIGHT
LIVING WALL
```

The rule-book bestiary explicitly describes Wights and Wraiths as immune to cold-based spells, independently corroborating the bit identity.

## `0x00000100` — common spell-effect immunity

The same bit is present in both elemental masks:

```text
fire mask  0x2100 = 0x2000 | 0x0100
cold mask  0x0900 = 0x0800 | 0x0100
```

It is also tested as the common component of several single-target status-effect masks:

```text
C15F  byte +0x32 & 0x01      -> overall 0x0100
C1B4  word +0x31 & 0x0102
C23C  word +0x31 & 0x0108
C2AF  word +0x31 & 0x0108
C317  word +0x31 & 0x0110
```

Thus `0x0100` is not element-specific. It independently suppresses multiple spell/status effects and is ORed into both proved elemental damage masks.

The safe project name is deliberately scoped to observed spell-effect dispatch rather than claiming universal immunity to every magical system:

```text
behavior_flags & 0x00000100  common_spell_effect_immunity
```

Supplied definitions carrying it:

```text
DOOM GUARD
BANSHEE
ZOMBIE GOLEM
LIVING WALL
```

## Remaining low status-specific bits

The targeted masks show that additional low bits specialize the common immunity family:

```text
0x0102 = common_spell_effect_immunity | 0x0002
0x0108 = common_spell_effect_immunity | 0x0008
0x0110 = common_spell_effect_immunity | 0x0010
```

The associated handlers set timed live-NPC status state, but the exact source-level identity of bits `0x0002`, `0x0008`, and `0x0010` is not yet closed. They remain raw until the spell/effect entries and resulting runtime state are independently named.

## Corpus counts

Across all 83 supplied definitions:

```text
common_spell_effect_immunity  4
cold_effect_immunity         12
fire_effect_immunity          6
```

These bits overlap legitimately. For example, BANSHEE and LIVING WALL carry all three.

## Safe implementation boundary

A high-level model may expose:

```text
behavior_flags_common_spell_effect_immunity = flags & 0x00000100
behavior_flags_cold_effect_immunity          = flags & 0x00000800
behavior_flags_fire_effect_immunity          = flags & 0x00002000
```

while retaining the exact raw `uint32 behavior_flags` field and all unknown bits.

Do not collapse the three aliases into a single resistance enum: CODE.2 explicitly builds compound masks by ORing the common bit with an element-specific bit.