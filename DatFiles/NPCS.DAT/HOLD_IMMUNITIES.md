# NPCS.DAT hold/status immunity bits

Status: **the three low status-specific bits previously observed in compound masks `0x0102`, `0x0108`, and `0x0110` are decoded as Hold Undead, Hold Person, and Hold Monster effect-immunity/exclusion gates. A later cross-audit corrected the cleric Hold Person scroll path: its scroll reuses dispatch ID 25 rather than proving prepared-priest dispatch ID 92.**

The immutable CODE.2 object remains controlling evidence. `OBJECTS.DAT` provides an independent serialized spell-name/order cross-check where the executable scroll stub actually carries the corresponding dispatch ID.

## Physical field

```text
NPCS.DAT record +0x31..+0x34  uint32 behavior_flags
```

These are bit aliases on the raw dword; the raw field must still be retained exactly because CODE.2 also reads it by byte and unaligned word.

## Dispatcher mapping

The CODE.2 spell/effect dispatcher uses a dword jump table at logical object-1 offset `0xB740`. The relevant handlers resolve to:

```text
dispatch 24 -> 0xC123  Slow
dispatch 25 -> 0xC201  Hold Person shared scroll/mage path
dispatch 26 -> 0xC179  Hold Undead
dispatch 41 -> 0xC2E0  Hold Monster
dispatch 92 -> 0xC273  prepared-priest Hold Person path
```

The serialized spell-scroll sequence independently verifies four direct mappings:

```text
dispatch 24  object 184  MAGE SCROLL OF 'SLOW'
dispatch 25  object 185  MAGE SCROLL OF 'HOLD PERSON'
dispatch 26  object 186  MAGE SCROLL OF 'HOLD UNDEAD'
dispatch 41  object 195  MAGE SCROLL OF 'HOLD MONSTER'
```

### Cleric Hold Person scroll correction

OBJECTS ID 211 is indeed:

```text
CLERIC SCROLL OF 'HOLD PERSON'
```

but it does **not** dispatch through ID 92. The category-14 scroll table maps both object 185 and object 211 to the same stub at logical object-1 `0x26300` / physical CODE.2 `0x61224`. That stub contains:

```text
57 56 6A 19 ...
          ^^ push 0x19 = dispatch 25
```

Thus the cleric scroll intentionally reuses the shared dispatch-25 Hold Person handler.

Prepared priest spell execution is a separate path. Its compact prepared-spell ID 92 reaches handler `0xC273`, which independently performs the Hold Person eligibility test and sets the same live held-state bit. The identity of handler 92 comes from that prepared-priest behavior; object 211 must not be cited as a serialized proof that the scroll itself passes ID 92.

This distinction matters because scroll-dispatch IDs and prepared-priest IDs are not interchangeable for every spell even when they implement the same named spell.

## `0x00000008` — Hold Person effect immunity/exclusion

The shared/mage Hold Person handler (`dispatch 25`) tests at logical `0xC23C`:

```text
test word [npc + 0x31], 0x0108
```

The prepared-priest Hold Person handler (`dispatch 92`) independently tests the same mask at logical `0xC2AF`:

```text
test word [npc + 0x31], 0x0108
```

Because `0x0108 == 0x0100 | 0x0008`, and `0x0100` is already proved to be the common spell-effect immunity gate, the status-specific component is:

```text
behavior_flags & 0x00000008  hold_person_effect_immunity
```

The name is behavioral: the bit causes either Hold Person handler to reject/skip the target. It does not claim whether the original source enum described this as immunity, creature-class eligibility, or both.

31 supplied NPC definitions carry the bit.

## `0x00000002` — Hold Undead effect immunity/exclusion

Hold Undead (`dispatch 26`) tests at logical `0xC1B4`:

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

Hold Monster (`dispatch 41`) tests at logical `0xC317`:

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

All four hold implementations that reach their application path—shared/mage Hold Person, Hold Undead, prepared-priest Hold Person, and Hold Monster—set the same live family-A runtime state bit:

```text
live_npc.runtime_flags_low |= 0x08
```

at logical sites:

```text
0xC252  shared/mage Hold Person (dispatch 25)
0xC1CA  Hold Undead (dispatch 26)
0xC2BF  prepared-priest Hold Person (dispatch 92)
0xC32D  Hold Monster (dispatch 41)
```

They then install a duration/countdown in live record word `+0x10`, with handler-specific duration formulas.

This shared state transition is the executable-level proof that the three low NPC bits belong to the same hold-status family while selecting different target/effect eligibility classes.

By contrast, Slow (`dispatch 24`) tests only the common `0x0100` gate and sets a different live runtime bit (`0x80`), demonstrating that the low hold bits are not generic timed-status immunity bits.

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

`analyze_hold_immunities.py` verifies the NPC bit distributions, direct spell-scroll mappings, the cleric-scroll reuse of dispatch 25, the dispatcher targets, exact mask-test instructions, and the shared live held-state mutation.