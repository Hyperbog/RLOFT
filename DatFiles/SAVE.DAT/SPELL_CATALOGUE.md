# SAVE.DAT shipped spell catalogue

Status: **raw-resource and executable count tables closed.** The supplied `T` file matches the repository-certified SHA-256 and directly supplies the shipped Mage/priest labels. CODE.2 supplies the per-bank active-entry counts used by the preparation machinery.

## Resource geometry

```text
Mage / matrix A   SAVE +0x06A..+0x0B9   T[0x42..0x91]
Priest / matrix B SAVE +0x0BA..+0x109   T[0x92..0xE1]
```

Both serialized matrices retain `8 x 10` cells, but only the first six spell-level banks are populated by the shipped game.

The dispatch relationship remains:

```text
spell_dispatch_id = T_index - 0x42
```

so Mage IDs occupy the low family and priest IDs begin at dispatch 80.

## Executable entry-count tables

The certified CODE.2 object contains these eight-dword tables:

```text
DGROUP 0x6420 / physical 0x7C344
priest counts = [7, 6, 5, 4, 4, 2, 0, 0]

DGROUP 0x6440 / physical 0x7C364
Mage counts   = [7, 6, 10, 4, 2, 6, 0, 0]
```

Independent consumers tie the tables to the serialized families:

- Cleric/Paladin setup routines `0x3ED08` and `0x3EDC8` access matrix B at runtime `0x2199A` and bound entries with `0x6420`.
- Mage normalization routine `0x3E99C` accesses matrix A at runtime `0x2194A` and bounds entries with `0x6440`.
- the spell-list UI at physical `0x40330` selects a count table by adding a `family * 0x20` byte offset to `0x6420`.

These counts exactly match the active nonblank prepared-spell labels in `T`, except for the deliberately separate priest-family action labels described below.

## Mage spellbook catalogue

### Level 1 — 7 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 66 | 0 | `ARMOR` |
| 1 | 67 | 1 | `BURNING HANDS` |
| 2 | 68 | 2 | `CHILL TOUCH` |
| 3 | 69 | 3 | `DETECT UNDEAD` |
| 4 | 70 | 4 | `LIGHT` |
| 5 | 71 | 5 | `MAGIC MISSILE` |
| 6 | 72 | 6 | `SHIELD` |

### Level 2 — 6 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 76 | 10 | `AGANNAZER'S SCORCHER` |
| 1 | 77 | 11 | `BLUR` |
| 2 | 78 | 12 | `IMPROVED IDENTIFY` |
| 3 | 79 | 13 | `KNOCK` |
| 4 | 80 | 14 | `WIZARD LOCK` |
| 5 | 81 | 15 | `ICE KNIFE` |

### Level 3 — 10 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 86 | 20 | `DISPEL MAGIC` |
| 1 | 87 | 21 | `FIREBALL` |
| 2 | 88 | 22 | `FLAME ARROW` |
| 3 | 89 | 23 | `HASTE` |
| 4 | 90 | 24 | `SLOW` |
| 5 | 91 | 25 | `HOLD PERSON` |
| 6 | 92 | 26 | `HOLD UNDEAD` |
| 7 | 93 | 27 | `INVISIBILITY 10' RAD` |
| 8 | 94 | 28 | `LIGHTNING BOLT` |
| 9 | 95 | 29 | `VAMPIRIC TOUCH` |

### Level 4 — 4 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 96 | 30 | `DETECT SCRYING` |
| 1 | 97 | 31 | `ICE STORM` |
| 2 | 98 | 32 | `STONESKIN` |
| 3 | 99 | 33 | `WIZARD EYE` |

### Level 5 — 2 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 106 | 40 | `CONE OF COLD` |
| 1 | 107 | 41 | `HOLD MONSTER` |

### Level 6 — 6 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 116 | 50 | `CLAWS OF UMBER HULK` |
| 1 | 117 | 51 | `DEATH SPELL` |
| 2 | 118 | 52 | `LICH TOUCH` |
| 3 | 119 | 53 | `OTILUKE'S FREEZING SPHERE` |
| 4 | 120 | 54 | `TRUE SEEING` |
| 5 | 121 | 55 | `DRAGON SCALES` |

Shipped prepared-Mage total: **35 spell entries**.

## Cleric/Paladin priest catalogue

### Level 1 — 7 prepared entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 146 | 80 | `BLESS` |
| 1 | 147 | 81 | `CURE LIGHT WOUNDS` |
| 2 | 148 | 82 | `CAUSE LIGHT WOUNDS` |
| 3 | 149 | 83 | `DETECT MAGIC` |
| 4 | 150 | 84 | `INVISIBILITY TO UNDEAD` |
| 5 | 151 | 85 | `LIGHT` |
| 6 | 152 | 86 | `MAGICAL STONE` |

Two additional nonblank labels occupy the same ten-record resource bank but lie beyond the executable preparation count:

```text
T[153] / dispatch 87  LAY HANDS
T[154] / dispatch 88  TURN UNDEAD
```

They therefore share the priest-family action namespace but are **not matrix-B prepared-spell cells** in the shipped memorization catalogue.

### Level 2 — 6 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 156 | 90 | `AID` |
| 1 | 157 | 91 | `FLAME BLADE` |
| 2 | 158 | 92 | `HOLD PERSON` |
| 3 | 159 | 93 | `SLOW POISON` |
| 4 | 160 | 94 | `DRAW STRENGTH` |
| 5 | 161 | 95 | `DRAW DEXTERITY` |

### Level 3 — 5 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 166 | 100 | `CURE DISEASE` |
| 1 | 167 | 101 | `DISPEL MAGIC` |
| 2 | 168 | 102 | `NEGATIVE PLANE PROT.` |
| 3 | 169 | 103 | `PRAYER` |
| 4 | 170 | 104 | `REMOVE PARALYSIS` |

### Level 4 — 4 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 176 | 110 | `CURE SERIOUS WOUNDS` |
| 1 | 177 | 111 | `CAUSE SERIOUS WOUNDS` |
| 2 | 178 | 112 | `FORTIFY` |
| 3 | 179 | 113 | `NEUTRALIZE POISON` |

### Level 5 — 4 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 186 | 120 | `CURE CRITICAL WOUNDS` |
| 1 | 187 | 121 | `CAUSE CRITICAL WOUNDS` |
| 2 | 188 | 122 | `FLAME STRIKE` |
| 3 | 189 | 123 | `RAISE DEAD ` |

`T[189]` contains a literal trailing space after `RAISE DEAD`; preservation tools should not silently rewrite the decoded payload if byte-faithful text round-tripping is desired.

### Level 6 — 2 entries

| Entry | T index | Dispatch | Exact T label |
|---:|---:|---:|---|
| 0 | 196 | 130 | `HEAL` |
| 1 | 197 | 131 | `HARM` |

Shipped prepared-priest total: **28 spell entries**.

## Reserved cells

The serialized format still reserves eight banks of ten cells for each family. Banks 7 and 8 have executable counts of zero in this game, and unused entries in the populated banks remain part of the serialized `0x50`-byte family image.

Do not compact the matrices to 35/28 entries in a compatibility serializer.

## Legacy inconsistency in priest level-loss normalization

One executable quirk must be preserved in archival documentation.

Routine `0x3EB28` is called after level loss for both Paladin (`class_code 1`) and Cleric (`class_code 3`) characters. It operates on **priest matrix B** at runtime `0x2199A`, but its entry loops are bounded by table `0x6440`—the table whose values match the Mage catalogue—rather than the priest table `0x6420` used by the normal Cleric/Paladin setup paths.

This is not evidence that the family identities are reversed: `0x3ED08`/`0x3EDC8`, the T ranges, class capability bits, and scribing path independently establish matrix B as priest and matrix A as Mage. It is instead a shipped code-path inconsistency/likely legacy bug.

A faithful emulation layer should reproduce that path if exact level-loss behavior matters; a modernized rules layer may optionally correct it, but only as an explicit compatibility deviation.

## Native-port rule

Maintain the stable tuple `(family, bank, entry)` and derive the legacy T/dispatch ID from it. Preserve all 160 matrix bytes exactly at the save boundary, including unused cells, even though the shipped active catalogue contains only 35 Mage and 28 prepared-priest entries.