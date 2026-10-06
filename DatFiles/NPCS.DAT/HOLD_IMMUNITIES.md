# NPCS.DAT hold/status immunity bits

Status: **the three low status-specific bits previously observed in compound masks `0x0102`, `0x0108`, and `0x0110` are now decoded as Hold Undead, Hold Person, and Hold Monster effect-immunity/exclusion gates.**

The immutable CODE.2 object remains controlling evidence. `OBJECTS.DAT` provides an independent serialized spell-name/order cross-check.

## Physical field

```text
NPCS.DAT record +0x31..+0x34  uint32 behavior_flags
```

These are bit aliases on the raw dword; the raw field must still be retained exactly because CODE.2 also reads it by byte and unaligned word.

## Dispatcher mapping

The CODE.2 spell/effect dispatcher uses a dword jump table at logical object-1 offset `0xB740`. The relevant effect IDs resolve to:

```text
effect 24 -> 0xC123
effect 25 -> 0xC201
effect 26 -> 0xC179
effect 41 -> 0xC2E0
effect 92 -> 0xC273
```

The serialized spell-scroll sequence in `OBJECTS.DAT` independently identifies those effect slots:

```text
effect 24  object 184  MAGE SCROLL OF 'SLOW'
effect 25  object 185  MAGE SCROLL OF 'HOLD PERSON'
effect 26  object 186  MAGE SCROLL OF 'HOLD UNDEAD'
effect 41  object 195  MAGE SCROLL OF 'HOLD MONSTER'
effect 92  object 211  CLERIC SCROLL OF 'HOLD PERSON'
```

The mage and cleric Hold Person implementations converge on the same immunity mask, which is a particularly strong cross-check.

## `0x00000008` — Hold Person effect immunity/exclusion

Mage Hold Person (`effect 25`) tests at logical `0xC23C`:

```text
test word [npc + 0x31], 0x0108
```

Priest Hold Person (`effect 92`) independently tests the same mask at logical `0xC2AF`:

```text
test word [npc + 0x31], 0x0108
```

Because `0x0108 == 0x0100 | 0x0008`, and `0x0100` is already proved to be the common spell-effect immunity gate, the status-specific component is:

```text
behavior_flags & 0x00000008  hold_person_effect_immunity
```

The name is behavioral: the bit causes the Hold Person handler to reject/skip the target. It does not claim whether the original source enum described this as immunity, creature class eligibility, or both.

31 supplied NPC definitions carry the bit.

## `0x00000002` — Hold Undead effect immunity/exclusion

Mage Hold Undead (`effect 26`) tests at logical `0xC1B4`:

```text
test word [npc + 0x31], 0x0102
```

Thus:

```text
0x0102 == common_spell_effect_immunity | 0x0002
```

and the low component can safely be exposed as:

```text
behavior_flags & 0x00000002  hold_undead_effect_immunity
```

25 supplied definitions carry the bit.

The encoded set includes both non-undead creatures and undead that the spell excludes. For that reason the documentation treats this as an effect-immunity/exclusion gate rather than a pure creature-taxonomy flag.

## `0x00000010` — Hold Monster effect immunity/exclusion

Mage Hold Monster (`effect 41`) tests at logical `0xC317`:

```text
test word [npc + 0x31], 0x0110
```

Therefore:

```text
0x0110 == common_spell_effect_immunity | 0x0010
```

and the status-specific alias is:

```text
behavior_flags & 0x00000010  hold_monster_effect_immunity
```

24 supplied definitions carry the bit.

## Shared live held state

All four hold handlers that reach their application path—mage Hold Person, mage Hold Undead, priest Hold Person, and mage Hold Monster—set the same live family-A runtime state bit:

```text
live_npc.runtime_flags_low |= 0x08
```

at logical sites:

```text
0xC252  mage Hold Person
0xC1CA  mage Hold Undead
0xC2BF  priest Hold Person
0xC32D  mage Hold Monster
```

They then install a duration/countdown in live record word `+0x10`, with spell-specific duration formulas.

This shared state transition is the executable-level proof that the three low bits all belong to the same hold-status family while selecting different target/effect eligibility classes.

By contrast, the immediately preceding Slow handler (`effect 24`) tests only the common `0x0100` gate and sets a different live runtime bit (`0x80`), demonstrating that the low hold bits are not generic timed-status immunity bits.

## Supplied-corpus counts

```text
hold_undead_effect_immunity   25
hold_person_effect_immunity   31
hold_monster_effect_immunity  24
common_spell_effect_immunity   4
```

The sets overlap by design and must not be collapsed into one enum.

## Safe implementation boundary

A high-level creature definition may expose:

```text
flags & 0x0002  hold_undead_effect_immunity
flags & 0x0008  hold_person_effect_immunity
flags & 0x0010  hold_monster_effect_immunity
flags & 0x0100  common_spell_effect_immunity
```

while preserving the original `uint32 behavior_flags` exactly.

`analyze_hold_immunities.py` verifies the NPC bit distributions, the serialized OBJECTS spell names, the CODE.2 jump-table targets, the exact mask-test instruction bytes, and the shared live held-state mutation.