# SAVE.DAT combat-feedback timed state (`+0x19E..+0x1B5`)

Status: **executable-certified semantic reconstruction. The former unnamed three-slot timer family is now closed as combat-feedback damage-display state.**

The region immediately following the 26 primary effect expiry dwords contains three independently indexed absolute gameplay deadlines and a parallel three-dword damage-value array.

## Serialized/runtime mapping

```text
runtime 0x21A7E = record +0x19E  deadline[0]
runtime 0x21A82 = record +0x1A2  deadline[1]
runtime 0x21A86 = record +0x1A6  deadline[2]

runtime 0x21A8A = record +0x1AA  damage_value[0]
runtime 0x21A8E = record +0x1AE  damage_value[1]
runtime 0x21A92 = record +0x1B2  damage_value[2]
```

Canonical semantic view:

```c
uint32_t combat_feedback_deadline[3];    // +0x19E..+0x1A9
uint32_t combat_feedback_damage[3];      // +0x1AA..+0x1B5
```

## Slot identities

Direct producer tracing closes all three indexes.

### Slots 0 and 1 — outgoing hand damage

Player attack routine logical `0xE5F0..0xEA81` receives the acting character and hand index.

After a successful hit, logical `0xEA35..0xEA5C` writes:

```text
deadline[hand] =
    current_gameplay_time
  + attack_duration / 2

damage_value[hand] =
    final damage amount
```

The hand index is the same 0/1 domain used by the character's paired hand/action system.

Thus:

```text
slot 0  recent outgoing damage for hand A
slot 1  recent outgoing damage for hand B
```

### Slot 2 — incoming damage

Common character-damage routine logical `0xCD44..0xCEE3` writes:

```text
damage_value[2] = incoming damage argument

deadline[2] =
    current_gameplay_time + 0x36
```

before applying Armor/Aid/Vampiric/HP processing.

Thus:

```text
slot 2  recent incoming character damage
```

The stored display value is the original incoming damage argument, not necessarily the final HP loss after true absorption pools.

## Initialization and expiration

Character initialization zeros all three deadlines.

Maintenance logical `0x1E4A0..0x1E532` walks exactly three deadlines for each active character. A matured nonzero deadline is cleared and the character is marked for UI/state refresh. The parallel damage dword is not cleared by this sweep.

## Direct presentation consumer

Logical `0x1ED37..0x1EDA4` selects one of the three slots.

If its deadline is active and not expired, the routine reads:

```text
combat_feedback_damage[slot]
```

and passes the formatted value into the character/HUD draw path.

If the deadline has expired, it clears the deadline and does not draw that feedback value.

This proves the family is presentation-linked combat feedback rather than a hidden gameplay buff/debuff timer.

## Relationship to hand-ready deadlines

These three display timers are distinct from:

```text
+0x10B/+0x10F
uint32 hand_ready_deadline[2]
```

The hand-ready deadlines govern whether each hand can act again.

The combat-feedback deadlines govern how long recent damage values remain visible.

Player attacks deliberately use the same computed attack duration in both systems:

```text
hand ready:
    current_time + full attack duration

outgoing damage display:
    current_time + attack duration/2
```

when a hit occurs.

## Reconstruction rule

Preserve the two arrays exactly and do not merge them with adjacent primary effects:

```text
primary effect bits                 +0x132..+0x135
primary effect expiry[26]           +0x136..+0x19D
combat feedback deadline[3]         +0x19E..+0x1A9
combat feedback damage[3]           +0x1AA..+0x1B5
```

The exact original DreamForge source identifier is unknown, but the gameplay/presentation semantics and per-index ownership are now CODE-PROVEN.
