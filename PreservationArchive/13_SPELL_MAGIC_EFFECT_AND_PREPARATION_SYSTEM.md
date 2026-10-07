# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 13 — Spell Preparation, Dispatch, Persistent Magic, and Transient Effects

**Status:** PRIMARY SPELL/MAGIC RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter reconstructs the original magic system from persistent spell preparation through runtime dispatch and effect state.

It covers:

- Mage and priest spell-preparation matrices;
- selected/planned versus ready/memorized counts;
- class-derived spell-slot limits;
- Mage spellbook availability and scribing;
- Cleric/Paladin priest preparation;
- spell-family capability bits;
- selected-caster UI state;
- spell text/resource identity;
- stable legacy dispatch IDs;
- prepared spells versus priest-family nonprepared actions;
- category-14 scroll reuse of spell dispatch;
- charged-device reuse of spell/projectile effect systems;
- all 26 persistent primary character effects;
- ordinary versus special-duration effects;
- reversible stat effects and protection quantities;
- party-wide/global detector/light state;
- temporary hand/touch spell objects;
- class-3 projectile/area effect records;
- NPC spell/effect immunity masks;
- cure/restoration/removal behavior;
- faithful-port compatibility boundaries.

The central preservation distinction is that Ravenloft has several separate magic identities:

```text
prepared spell cell
legacy spell/action dispatch ID
persistent character effect index
temporary hand-effect index
class-3 presentation/projectile effect type
object ID for scroll/device
```

These domains overlap semantically but are **not interchangeable numeric enums**.

Primary sources: [spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md), [spell families](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_FAMILIES.md), [spell matrix](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_MATRIX.md), [primary effects](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md).

## 2. Each character stores two fixed 8×10 spell matrices

The character image contains:

```text
Mage matrix A
    +0x06A..+0x0B9
    runtime 0x2194A..0x21999

priest matrix B
    +0x0BA..+0x109
    runtime 0x2199A..0x219E9
```

Each is exactly:

```text
8 banks × 10 cells = 80 bytes
```

[Spell matrix](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_MATRIX.md)

The serialized geometry remains 80 bytes even though the shipped game uses only the first six spell-level banks.

## 3. One byte stores two preparation counts

Each spell cell is packed:

```text
bits 7..4
    currently ready / memorized copies

bits 3..0
    selected / planned copies

low nibble 0xF
    unavailable / non-selectable spell
```

Safe helpers:

```text
ready_count(cell)    = cell >> 4
selected_count(cell) = cell & 0x0F
unavailable(cell)    = (cell & 0x0F) == 0x0F
```

[Spell matrix](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_MATRIX.md)

The byte is not a boolean “known spell” flag.

## 4. Spell-level bank geometry

The memorization/prayer UI exposes:

```text
bank 0 -> spell level 1
bank 1 -> spell level 2
bank 2 -> spell level 3
bank 3 -> spell level 4
bank 4 -> spell level 5
bank 5 -> spell level 6
bank 6 -> reserved level-7 storage
bank 7 -> reserved level-8 storage
```

The UI stops after the first six active banks, but banks 6 and 7 remain part of the save ABI.

Do not compact the matrix to six banks in a compatibility serializer.

## 5. Mage and priest families have different active-entry counts

Executable count tables:

```text
priest matrix B
DGROUP 0x6420
[7, 6, 5, 4, 4, 2, 0, 0]

Mage matrix A
DGROUP 0x6440
[7, 6, 10, 4, 2, 6, 0, 0]
```

[Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md)

These tables answer:

> how many named spell cells exist in each bank?

They are distinct from per-character preparation-slot limits.

## 6. Shipped prepared-spell totals

The supplied active catalogue contains:

```text
Mage
    7 + 6 + 10 + 4 + 2 + 6
    = 35 prepared spell entries

priest
    7 + 6 + 5 + 4 + 4 + 2
    = 28 prepared spell entries
```

The remaining serialized cells are reserved, unavailable, or unused by the shipped catalogue.

## 7. Matrix A is the Mage/scribeable family

Class code:

```text
4 = Mage
```

sets casting capability bit:

```text
0x02
```

and uses matrix A.

Mage-only scribing logic operates exclusively on matrix A and can convert an unavailable cell into an available spellbook cell. [Spell families](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_FAMILIES.md)

This independently proves that matrix A is not simply another preparation list: it also represents Mage spellbook availability.

## 8. Matrix B is shared by Cleric and Paladin

Certified class ownership:

```text
class 3 Cleric
    capability bit 0x01
    -> matrix B

class 1 Paladin
    capability bit 0x04
    -> matrix B
```

[Spell families](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_FAMILIES.md)

Cleric and Paladin use the same serialized priest-style matrix but retain distinct capability bits and class/progression paths.

## 9. Casting capability is a bit mask

Character byte:

```text
+0x131  casting_capabilities
```

contains:

```text
0x01  Cleric preparation capability
0x02  Mage spellbook capability
0x04  Paladin priest capability
```

Combinations occur for multiclass characters.

It is not a single-choice caster-class enum.

## 10. Spell-slot limits are a third structure

Routine `40B00` constructs an eight-byte working vector at:

```text
DGROUP 0x6208
```

using:

- character index;
- class-level-derived state;
- a class-dependent progression/slot table.

[Spell matrix](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_MATRIX.md)

This vector answers:

> how many total copies may this character prepare at each spell level?

It is distinct from:

```text
active entry count
    how many named spell choices exist in bank

matrix cell
    how many copies are selected/ready for one spell
```

## 11. Preparation normalization enforces per-level capacity

Normalization sums selected/ready counts by bank and compares them with:

```text
slot_limit[bank]
```

from the working vector.

When required, counts are clamped/normalized to fit available preparation capacity.

The compatibility engine should preserve nibble-width arithmetic rather than unpacking to unconstrained host integers and only later clamping.

## 12. Mage normalization uses the Mage-shaped entry table

Mage normalization routine `3E99C`:

- operates on matrix A;
- bounds entry loops with `0x6440`;
- therefore uses:
  ```text
  [7,6,10,4,2,6,0,0]
  ```

[Spell matrix](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_MATRIX.md)

This agrees exactly with the raw spell labels in `T`.

## 13. Normal priest setup uses the priest-shaped entry table

Cleric and Paladin setup paths `3ED08/3EDC8`:

- operate on matrix B;
- use `0x6420`;
- therefore bound entries as:
  ```text
  [7,6,5,4,4,2,0,0]
  ```

This independently closes matrix-B ownership.

## 14. Original priest level-loss inconsistency

Routine `3EB28` is called after level loss for both:

- Cleric;
- Paladin.

It operates on **priest matrix B** but bounds its entry loops using:

```text
0x6440
```

the Mage-shaped entry-count table.

[Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md)

This does not reverse the matrix identities.

It is an original executable inconsistency/likely legacy bug.

Strict compatibility should reproduce it; a modernized rules path may fix it only as an explicit deviation.

## 15. Spell family, bank, and entry form the stable preparation identity

The safest native identity for a prepared cell is:

```text
SpellCellIdentity {
    family
    bank
    entry
}
```

where:

```text
family 0 = Mage
family 1 = priest
bank     = 0..7
entry    = 0..9
```

The legacy dispatch and text IDs are derived from that identity.

## 16. Text archive T is the authoritative spell-name resource

The supplied `T` file is:

```text
133,584 bytes
1,073 records
SHA-256
a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315
```

[Spell resource lookup](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_RESOURCE_LOOKUP.md)

Indexed decoding is:

```text
seek 4 + index*4
read record offset
seek record
read uint16 payload length
read payload bytes
XOR every byte with 0xDC
append runtime NUL
```

There is no unidentified intermediate spell-name database.

## 17. Mage spell text geometry

Mage family text index:

```text
T_index =
    0x42
  + bank * 10
  + entry
```

The reserved range is:

```text
T[0x42..0x91]
T[66..145]
```

## 18. Priest spell text geometry

Priest family text index:

```text
T_index =
    0x92
  + bank * 10
  + entry
```

Reserved range:

```text
T[0x92..0xE1]
T[146..225]
```

## 19. Dispatch ID is a compact family/bank/entry encoding

For prepared spells:

```text
spell_dispatch_id =
    bank * 10
  + entry
  + family * 80
```

and:

```text
T_index = 0x42 + spell_dispatch_id
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

This compact dispatch namespace is used by the player spell executor and also reused by scroll stubs.

## 20. Mage dispatch catalogue

### Level 1

```text
 0  ARMOR
 1  BURNING HANDS
 2  CHILL TOUCH
 3  DETECT UNDEAD
 4  LIGHT
 5  MAGIC MISSILE
 6  SHIELD
```

### Level 2

```text
10  AGANNAZER'S SCORCHER
11  BLUR
12  IMPROVED IDENTIFY
13  KNOCK
14  WIZARD LOCK
15  ICE KNIFE
```

### Level 3

```text
20  DISPEL MAGIC
21  FIREBALL
22  FLAME ARROW
23  HASTE
24  SLOW
25  HOLD PERSON
26  HOLD UNDEAD
27  INVISIBILITY 10' RAD
28  LIGHTNING BOLT
29  VAMPIRIC TOUCH
```

### Level 4

```text
30  DETECT SCRYING
31  ICE STORM
32  STONESKIN
33  WIZARD EYE
```

### Level 5

```text
40  CONE OF COLD
41  HOLD MONSTER
```

### Level 6

```text
50  CLAWS OF UMBER HULK
51  DEATH SPELL
52  LICH TOUCH
53  OTILUKE'S FREEZING SPHERE
54  TRUE SEEING
55  DRAGON SCALES
```

[Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md)

## 21. Priest dispatch catalogue

### Level 1 prepared entries

```text
80  BLESS
81  CURE LIGHT WOUNDS
82  CAUSE LIGHT WOUNDS
83  DETECT MAGIC
84  INVISIBILITY TO UNDEAD
85  LIGHT
86  MAGICAL STONE
```

### Level 2

```text
90  AID
91  FLAME BLADE
92  HOLD PERSON
93  SLOW POISON
94  DRAW STRENGTH
95  DRAW DEXTERITY
```

### Level 3

```text
100  CURE DISEASE
101  DISPEL MAGIC
102  NEGATIVE PLANE PROT.
103  PRAYER
104  REMOVE PARALYSIS
```

### Level 4

```text
110  CURE SERIOUS WOUNDS
111  CAUSE SERIOUS WOUNDS
112  FORTIFY
113  NEUTRALIZE POISON
```

### Level 5

```text
120  CURE CRITICAL WOUNDS
121  CAUSE CRITICAL WOUNDS
122  FLAME STRIKE
123  RAISE DEAD
```

### Level 6

```text
130  HEAL
131  HARM
```

The raw `T` record for Raise Dead contains a literal trailing space; byte-faithful text tooling should preserve the decoded payload exactly.

## 22. Lay Hands and Turn Undead share the priest action namespace but are not prepared cells

The first priest text bank also contains:

```text
T[153] dispatch 87  LAY HANDS
T[154] dispatch 88  TURN UNDEAD
```

But bank 0's executable preparation count is only seven.

Therefore dispatch 87/88 are:

```text
priest-family actions
```

but not:

```text
matrix-B prepared spell entries
```

[Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md)

This is an important distinction when designing a unified action bar.

## 23. Ready spell copies are expanded into runtime action records

CODE.2 expands the high-nibble ready counts from the persistent matrices into runtime spell/action records.

For a selected bank it walks every active entry and emits:

```text
one runtime record per ready copy
```

while retaining:

- family;
- bank;
- entry;
- character;
- derived dispatch/text identity.

[Spell resource lookup](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_RESOURCE_LOOKUP.md)

The runtime action list is therefore derived state; the character matrices remain persistent authority.

## 24. Selected-caster state is separately persistent

SAVE-global dwords:

```text
spell_ui_selected_character[2]
```

at runtime `0xD8A8` retain the currently selected character for the two spell-interface groups.

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Character-local fields:

```text
+0x12F  selected_spell_bank
+0x130  spell_list_scroll
```

retain that character's UI navigation state.

Thus spell UI state is divided between:

```text
global group -> selected character

character -> selected bank/scroll
```

## 25. Selected-caster groups are character-pair based

The selected-character setter computes:

```text
group = character_index / 2
```

and stores:

```text
D8A8[group] = character_index
```

after validating active/casting-capable state.

The two spell-interface groups therefore correspond to the two party-character pairs used by the HUD/action layout.

## 26. Pray/Memorize are modal preparation interfaces

The top-edge menu maps:

```text
column 1  pray
column 2  memorize
```

Before entering either interface, CODE.2:

1. saves the cursor-held object ID and known state;
2. clears the held cursor item;
3. enters the preparation UI;
4. restores the held item/state afterward.

[Input/UI chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

Preparation therefore edits character spell state in a modal UI context rather than using ordinary inventory dragging.

## 27. Mage scribing changes availability, not merely ready count

Mage scribing code operates on matrix A.

It:

1. derives bank/entry from spell identity;
2. checks cell availability/capacity;
3. rejects a spell already available;
4. converts an unavailable low-nibble sentinel into available spellbook state.

[Spell families](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_FAMILIES.md)

A scribed spell is therefore a persistent change to the Mage spellbook matrix, not just a one-use action.

## 28. Scroll object identity and spell identity are separate

Category-14 OBJECTS records carry scroll item identities.

Their use dispatcher maps specific object IDs to legacy spell dispatch IDs.

Representative examples:

```text
OBJECT 167  MAGE SCROLL OF 'ARMOR'
    -> dispatch 0

OBJECT 188  MAGE SCROLL OF 'LIGHTNING BOLT'
    -> dispatch 28

OBJECT 197  MAGE SCROLL OF 'DEATH SPELL'
    -> dispatch 51

OBJECT 202  CLERIC SCROLL OF 'BLESS'
    -> dispatch 80

OBJECT 218  CLERIC SCROLL OF 'PRAYER'
    -> dispatch 103

OBJECT 222  CLERIC SCROLL OF 'FORTIFY'
    -> dispatch 112
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

Do not use the object ID itself as a spell ID.

## 29. Scroll and prepared-spell dispatch converge

Once a scroll stub has produced the appropriate legacy dispatch ID, the downstream spell/effect handler can be shared with prepared casting.

This creates the architecture:

```text
prepared matrix cell
    |
    +--> legacy dispatch ID
             |
scroll object +--> same spell/effect handler
```

The source of casting remains distinct for inventory consumption and eligibility, but the effect implementation can converge.

## 30. Priest Hold Person demonstrates deliberate cross-family handler reuse

Prepared priest Hold Person uses:

```text
dispatch 92
```

but the Cleric Hold Person scroll intentionally reuses:

```text
dispatch 25
```

the Mage/shared Hold Person handler.

[NPC hold immunities](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

Therefore:

```text
spell label identity
family dispatch identity
handler identity
```

can differ.

A modern dispatch table should preserve these historical aliases where compatibility matters.

## 31. Spell identity is not primary-effect identity

Examples:

```text
Mage dispatch 0 ARMOR
    -> primary effect index 0

Mage dispatch 4 LIGHT
    -> primary effect index 1

priest dispatch 85 LIGHT
    -> primary effect index 10

priest dispatch 90 AID
    -> primary effect index 11
```

But many spells do not become one of the 26 persistent character effect bits at all.

Other spells:

- deal immediate damage;
- create temporary hand objects;
- create class-3 world effects;
- alter NPC runtime state;
- perform cures/removal;
- open/lock world state;
- invoke story/detection state.

Thus the primary-effect index is a separate persistence namespace.

## 32. Persistent primary effects are a 26-slot legacy subsystem

Each character stores:

```text
+0x132  uint32 active_effect_bits
+0x136  uint32 effect_expiry[26]
```

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

For ordinary timer-backed effect index `i`:

```text
bit (1 << i)
    corresponds to
effect_expiry[i]
```

Bits 26..31 do not acquire additional timer slots.

## 33. Complete persistent-effect index

```text
 0 Armor
 1 Mage Light
 2 Shield
 3 Blur
 4 Haste
 5 Invisibility 10' Radius
 6 Stoneskin
 7 Dragon Scales
 8 Bless
 9 Invisibility to Undead
10 Priest Light
11 Aid
12 Draw Upon Holy Might - STR
13 Draw Upon Holy Might - DEX
14 Negative Plane Protection
15 Fortify
16 Slow Poison
17 True Seeing
18 Detect Undead
19 Potion of Fire Resistance
20 Detect Magic
21 Prayer
22 Shield of Lightning Protection equipped-state
23 Potion of Giant Strength
24 Potion of Speed
25 Wand of Enemy Detection
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

This list itself demonstrates why the subsystem must not be renamed a spell list: indexes 19 and 22..25 are item/equipment-derived.

## 34. Persistent effect labels are a separate T range

Expiration labels are:

```text
T[588..613]
```

and the engine displays:

```text
<character name>'s <effect label> wears off.
```

[T effect text](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/T_EFFECT_TEXT.md)

Producer identity remains authoritative because:

```text
effect 1  Mage Light
effect 10 Priest Light
```

both display simply:

```text
Light
```

## 35. Ordinary primary-effect expiration

For an active effect with a positive expiry deadline:

1. compare against SAVE-global gameplay tick `A7E4`;
2. when mature, clear the active bit;
3. display the expiration message;
4. zero the matching expiry dword;
5. run effect-specific cleanup when required.

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

Not every effect uses this ordinary lifecycle; the second half of this chapter documents special removal, global counters, temporary hand effects, and transient world effects.


## 36. Primary effects have multiple lifetime classes

The 26-bit subsystem does not imply one uniform timer model.

The supplied executable demonstrates at least these lifetime classes:

```text
ordinary timed
    bit + positive expiry
    common expiration handler

attack-broken
    Invisibility 10' Radius
    Invisibility to Undead

attack-consumed
    Negative Plane Protection

one-shot spell modifier
    Fortify

equipment-derived
    Shield of Lightning Protection

party-synchronized
    Prayer

shared-global-counter-backed
    True Seeing
    Detect Undead
    Detect Magic
    Wand of Enemy Detection

timed + auxiliary quantity
    Armor
    Stoneskin
    Aid
    Draw Upon Holy Might
    Potion of Giant Strength
```

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md) [Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

A native magic layer may expose common interfaces, but compatibility behavior must retain these different removal and cleanup rules.

## 37. Attacking breaks party-wide Invisibility 10' Radius

Player attack pre-handler logical `0xE578` executes before the attack's hit test.

It walks all four party-character records. For each record with:

```text
primary effect bit 5
Invisibility 10' Radius
```

it:

```text
clear bit 5
mark that character display dirty
```

No ordinary positive expiry slot is cleared in this branch because this producer/lifecycle does not depend on the standard timer path.

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md) [Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

Thus one attack by one party member breaks the radius invisibility state across the party.

## 38. The attacker separately loses Invisibility to Undead

The same pre-attack helper then examines the acting character.

If primary effect 9 is active:

```text
Invisibility to Undead
```

CODE.2:

```text
zeros effect_expiry[9]
clears effect bit 9
marks acting character display dirty
```

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

This removal occurs before whether the attack succeeds is known.

Therefore:

```text
attack command
    |
    +-- clear party effect 5
    +-- clear attacker's effect 9
    |
    v
target/hit resolution
```

is the compatibility ordering.

## 39. Negative Plane Protection is consumed by qualifying undead/drain attacks

Primary effect 14:

```text
Negative Plane Protection
```

has a special combat-side lifecycle.

The NPC attack family tests:

- NPC undead classification;
- NPC level-drain capability;
- NPC Strength-drain capability;
- the target character's effect-14 state.

When the protection intercepts a qualifying drain/undead path, the ordinary drain outcome is suppressed and a local “protection consumed” state is retained until after the damage phase.

The consumption tail then:

```text
effect_expiry[14] = 0
clear primary effect bit 14
run the encoded protection-consumption presentation/effect path
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md) [Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

It should therefore be modeled as a special one-hit/one-event protection state rather than forced through only the ordinary expiration sweep.

## 40. Fortify is a one-shot cure modifier

Primary effect 15:

```text
Fortify
```

is tested by the cure handlers.

Without Fortify, those handlers use their ordinary generated/random healing quantity.

With Fortify active, they substitute exact encoded maximum values:

```text
Cure Light Wounds       8
Cure Serious Wounds    16
Cure Critical Wounds   27
```

and clear Fortify immediately.

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

This is direct executable behavior. The archive does not need to infer the tabletop dice formula to preserve the result.

## 41. Prayer is synchronized across the party

Primary effect 21:

```text
Prayer
```

is installed through a party-wide producer path.

Its expiration cleanup does not behave like four completely unrelated per-character timers. The handler clears corresponding Prayer state across the party.

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

A native implementation may represent this as a party-scoped source effect with per-character legacy bits, but its legacy serializer must preserve the bits/timers exactly.

## 42. Shared detection/vision counters are SAVE-global state

Four primary effects also update shared counters outside the character record:

```text
effect 17 True Seeing
    SAVE prefix +0x66
    DGROUP 0xFD08
    uint32 shared state

effect 18 Detect Undead
    SAVE prefix +0x62
    DGROUP 0xF03C
    uint16 shared state

effect 20 Detect Magic
    SAVE prefix +0x6A
    DGROUP 0x6DAE
    uint16 shared state

effect 25 Wand of Enemy Detection
    SAVE prefix +0x64
    DGROUP 0xF03E
    uint16 shared state
```

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md) [Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

Producers increment/establish these shared states and expiration cleanup decrements them.

This allows multiple character/item effects to contribute to one global renderer/detection mode without reducing the persistent character bitsets to one global boolean.

## 43. Detect Undead and undead classification are intentionally separate

When the shared Detect Undead counter `0xF03C` is active, first-person presentation code resolves NPC definitions and checks:

```text
behavior_flags & 0x00000001
    detect_undead_effect_exclusion
```

A creature carrying that bit is excluded from the Detect Undead marker/display path.

This is separate from:

```text
behavior_flags_3A & 0x20
    undead_classification
```

which Turn Undead uses. [NPC detection/turning](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)

A native model must not collapse the two into one `is_undead` predicate.

## 44. Detect Magic changes item presentation through shared state

When shared magic-reveal state `0x6DAE` is nonzero, independent item presentation paths reveal magical state using the same global condition.

OBJECTS still retains the actual static magic/enchantment information:

```text
intrinsic_magic_marker
enchantment_bonus
```

while the shared effect state controls whether the UI/render path exposes it. [OBJECTS category semantics](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

This is a presentation-state interaction, not mutation of the item definitions.

## 45. Shield of Lightning Protection is equipment-derived magic state

Primary effect 22 is tied to equipped OBJECTS ID 103:

```text
SHIELD OF LIGHTNING PROTECTION
```

Equipment refresh code sets the bit when the qualifying shield is present and removes the state when it is absent. [Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

It is therefore not an ordinary timed cast effect despite sharing the same serialized primary-effect word.

Compatibility reconstruction should derive/reconcile it during equipment-state updates rather than scheduling it as a generic spell timer.

## 46. Armor carries a protection/durability quantity but does not absorb HP damage

Primary effect 0:

```text
Armor
```

uses character:

```text
+0x117  Armor protection/durability quantity
```

Incoming damage subtracts the full incoming amount from this quantity and clears Armor when it reaches zero or below.

Crucially, that subtraction does **not** reduce the local damage amount before subsequent true absorption and HP processing.

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md) [Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

This distinction was corrected during the combat preservation pass.

## 47. Aid is a true temporary protection pool

Primary effect 11:

```text
Aid
```

uses:

```text
+0x1EA  Aid temporary-hit-point/protection pool
```

Incoming damage consumes this pool before remaining damage reaches ordinary HP.

Expiration clears the pool as part of effect-11 cleanup. [Character tail](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

This is mechanically different from Armor's durability quantity.

## 48. Draw Upon Holy Might stores reversible deltas

Primary effects:

```text
12  Draw Upon Holy Might - STR
13  Draw Upon Holy Might - DEX
```

store their applied reversible values at:

```text
+0x123  Strength delta
+0x127  Dexterity delta / low-byte state
```

Expiration explicitly reverses those saved quantities rather than blindly recalculating the character from base attributes. [Effect flags](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

Those auxiliaries are therefore part of the effect's persistent identity.

## 49. Potion of Giant Strength similarly stores a reversible modifier

Primary effect 23 is item-derived:

```text
POTION OF GIANT STRENGTH
```

and retains its reversible modifier at:

```text
+0x1BE
```

Expiration passes the stored quantity through the Strength-adjustment reversal helper. [Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

This is another case where “effect active” is not fully represented by the bit/timer pair alone.

## 50. Stoneskin is an attack-interception effect with auxiliary state

Primary effect 6:

```text
Stoneskin
```

stores additional state at:

```text
+0x1F2  accumulated/remaining Stoneskin state
```

[Character tail](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

Combat ordering is exact:

1. NPC raw natural 20/1 handling occurs;
2. effective AC is calculated;
3. if Stoneskin is active, the Stoneskin consumption/removal helper is invoked;
4. the attack returns as a miss/intercept;
5. ordinary Blur/Invisibility/Bless comparison does not occur for that intercepted attack.

[Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

The helper also modifies the auxiliary Stoneskin state. A faithful magic layer should keep the primary bit/timer and the auxiliary count/state coupled.

## 51. Temporary hand spells form a separate nine-effect subsystem

Character state:

```text
+0x1C2  uint32 temporary_hand_effect_bits
+0x1C6  uint32 temporary_hand_effect_expiry[9]
```

maps indexes to pseudo-item IDs:

```text
0  0x7E  CHILL TOUCH
1  0x7F  VAMPIRIC TOUCH
2  0x80  CLAWS OF THE UMBER HULK
3  0x81  CAUSE LIGHT WOUNDS
4  0x82  CAUSE SERIOUS WOUNDS
5  0x83  CAUSE CRITICAL WOUNDS
6  0x84  HARM
7  0x85  FLAME BLADE
8  0x86  LICH TOUCH
```

[Character tail](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

These are not entries in the 26-bit primary-effect system.

## 52. Temporary hand spells materialize as protected pseudo-items

When active, the corresponding temporary object occupies a hand slot.

Normal inventory swapping explicitly refuses to move object IDs `0x7E..0x86`. [Object/inventory chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md)

The magic system therefore uses the ordinary combat-hand interface while protecting those spell-generated pseudo-items from ordinary inventory manipulation.

This creates the path:

```text
spell/action
    |
    v
temporary hand-effect bit + expiry
    |
    v
temporary OBJECTS pseudo-item in hand
    |
    v
ordinary player attack machinery
    |
    v
spell-specific hit/damage behavior
```

## 53. Temporary-hand expiration removes the hand object

Maintenance walks all nine temporary-hand expiry slots.

On maturation it:

1. clears the matching temporary-effect bit;
2. clears the corresponding deadline;
3. calls the removal helper;
4. removes the matching pseudo-object from either hand if still equipped.

[Character tail](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

Thus the timer and the actual hand inventory state must remain synchronized.

## 54. Vampiric Touch couples offense to later defense

Temporary pseudo-item:

```text
0x7F  VAMPIRIC TOUCH
```

uses a dedicated combat case.

After a successful hit, the produced damage amount is stored into:

```text
character +0x113
```

the true Vampiric Touch absorption pool. [Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

Later incoming damage consumes that pool after Aid but before ordinary HP.

The spell therefore links:

```text
successful offensive hand effect
    -> persistent defensive absorption quantity
```

rather than ending when its attack animation completes.

## 55. Class-3 transient effects are a third magic representation

Many visible/projectile/area spell effects use the SAVE-global transient array rather than character primary bits or temporary hand bits.

Runtime ownership:

```text
count  DGROUP 0xC038
array  runtime 0x248EC
limit  150 records
size   0x16 bytes each
```

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

These records are persisted by SAVE.DAT so moving/active spell effects can survive a save/load.

## 56. Transient-effect record

```text
+0x00  int32  coord_B_16_16
+0x04  int32  coord_A_16_16
+0x08  int32  heading_angle
+0x0C  uint16 map_cell_index
+0x0E  uint16 effect_type
+0x10  uint16 auxiliary_10
+0x12  uint16 type_specific_aux_12
+0x14  uint16 packed_state
```

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

This is a live effect instance, not a spell definition and not an OBJECTS/NPCS record.

## 57. Class-3 effect type is not spell dispatch ID

Certified type producers are:

```text
type 0
    Fireball
    Wand of Fireballs
    LEVELS opcode-3 effect

type 1
    Lightning Bolt
    LEVELS opcode-3 effect

type 2
    Ice Knife

type 3
    Cone of Cold
    Wand of Frost

type 4
    Flame Arrow

type 5
    no direct supplied-path producer identified

type 6
    Magic Missile
    Wand of Magic Missiles

type 7
    no direct supplied-path producer identified

type 8
    Burning Hands
    Agannazar's Scorcher presentation/effect family

type 9
    Ice Storm
    Otiluke's Freezing Sphere presentation/effect family

type 10
    Flame Strike

type 11
    Holy Symbol of Ravenkind area-effect path
```

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The same type can have multiple spell/item/script producers.

## 58. Class-3 effects use INIT descriptors and RES3 source class 3

The scene emitter resolves:

```text
descriptor =
    class3_descriptor_table
  + effect_type * 7
```

and derives the class-3 bank-local shape.

The final RES3 translation is:

```text
absolute_RES3_id =
    4158
  + class3_bank_local_shape
```

[Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md) [RES3 source banks](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/Resources/RES/TYPE7_SOURCE_BANKS.md)

Thus spell dispatch identity and renderer effect identity are separate namespaces.

## 59. Class-3 packed state carries direction, animation, and source context

Certified subfields of record `+0x14` are:

```text
bits 1..2   movement/collision state; exact labels unresolved
bits 3..5   direction sector 0..7
bits 6..8   animation/variant frame 0..7
bits11..13  source-actor/context code 0..7
other bits  preserve raw
```

Normal spell/action callers place party-character indexes `0..3` into the source/context field.

LEVELS event opcode 3 passes literal `4`, proving it is broader than an unconditional party-character index. [Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

## 60. The transient constructor deliberately performs partial initialization

The common constructor does not clear all `0x16` bytes.

It writes principal fields and modifies selected packed-state masks.

For type 8 it initializes `+0x12` to a pseudo-random value in:

```text
20..29
```

and maintenance decrements/removes the record when it reaches zero.

For other types, `+0x12` is not universally initialized by the common constructor. [Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

A faithful runtime/serializer must not “clean up” unknown auxiliaries globally.

## 61. Transient-effect removal preserves ordered-array behavior

Removal compacts later `0x16` records and decrements the global count.

The array therefore has ordered compact semantics analogous to family-C world items.

A native engine can use handles internally, but compatibility state traces should reproduce visible/order-sensitive iteration where relevant.



## 62. NPC spell eligibility is mask-driven and spell-specific

The creature definition does not expose one universal spell-resistance field.

Different magic paths construct different masks over the raw `NPCS.DAT behavior_flags` dword.

Certified examples include:

```text
common spell/effect gate
    0x0100

fire effect
    0x2100
    = common 0x0100
    | fire-specific 0x2000

cold effect
    0x0900
    = common 0x0100
    | cold-specific 0x0800

Lightning Bolt
    0x0300
    = common 0x0100
    | lightning-specific 0x0200

Hold Undead
    0x0102

Hold Person
    0x0108

Hold Monster
    0x0110
```

[NPC effect immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md) [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md) [Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

A faithful magic engine should preserve each encoded target predicate rather than mapping them all to one host-side `magic_resistance` percentage.

## 63. Fire and cold share the common gate but retain element-specific exclusions

Fire-family effect paths use:

```text
behavior_flags & 0x2100
```

Cold-family paths use:

```text
behavior_flags & 0x0900
```

The common `0x0100` bit therefore excludes both families, while the element-specific bits can exclude only their associated effect path. [NPC effect immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/EFFECT_IMMUNITIES.md)

This compositional mask behavior is more precise than assigning one inferred tabletop resistance category.

## 64. Lightning Bolt has its own exclusion path

The complete executable/resource chain is:

```text
OBJECTS 188
MAGE SCROLL OF 'LIGHTNING BOLT'
        |
        v
dispatch 28
        |
        v
class-3 transient effect type 1
        |
        v
moving-effect NPC collision
        |
        v
behavior_flags mask 0x0300
```

[Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

The additional bit:

```text
0x00000200
```

is therefore safely the Lightning Bolt effect exclusion/immunity bit.

## 65. Death Spell does not reuse the common spell gate

Mage dispatch 51:

```text
DEATH SPELL
```

enters a target-area routine that checks:

```text
behavior_flags & 0x00000080
```

and skips matching NPC targets. [Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

This path does **not** combine the common `0x0100` gate.

Thus even two effects that are both “magic” can use different definition predicates and different target traversal.

## 66. Hold effects share one live state but use different eligibility masks

Certified hold dispatches:

```text
25  Mage/shared Hold Person
26  Hold Undead
41  Hold Monster
92  prepared-priest Hold Person
```

use different target masks but converge on the same live family-A held state:

```text
live_npc.runtime_flags_low |= 0x08
```

plus a handler-specific duration/countdown in live actor word `+0x10`. [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

This gives a useful subsystem split:

```text
spell identity
    -> eligibility mask

successful application
    -> common live held-state representation
```

## 67. Slow is not another Hold variant

Mage dispatch 24:

```text
SLOW
```

tests the common `0x0100` exclusion but sets a different live actor runtime bit:

```text
0x80
```

rather than held-state bit `0x08`. [Hold immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/HOLD_IMMUNITIES.md)

Combat later uses this state in both directions:

- slowed NPC is easier for the party to hit;
- slowed NPC has a harder attack threshold.

[Combat chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/12_COMBAT_ATTACK_DAMAGE_SAVES_AND_PROGRESSION.md)

## 68. Turn Undead is an action in the priest dispatch namespace

Dispatch 88:

```text
TURN UNDEAD
```

shares the priest-family action namespace but lies beyond the level-1 matrix preparation count.

It calls the undead-turning resolution path rather than consuming a prepared matrix-B cell. [Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md) [NPC detection/turning](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md)

Target eligibility first requires:

```text
behavior_flags_3A & 0x20
    undead_classification
```

A separate main behavior bit `0x00000004` subtracts four from the favorable turning roll before threshold comparison.

No supplied NPC combines both bits, so that modifier is executable-supported but dormant for the shipped creature table.

## 69. Lay Hands is likewise outside prepared matrix-B cells

Dispatch 87:

```text
LAY HANDS
```

also occupies the priest-family text/dispatch bank but lies beyond the seven prepared level-1 priest entries. [Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md)

It must therefore remain distinguishable from:

```text
prepared spell cell
scroll spell
special class/action dispatch
```

even though all three can enter the same overall action dispatcher namespace.

## 70. Status-removal spells map directly to certified character bits

Three priest actions form exact producer/consumer pairs with the NPC status-attack system:

```text
dispatch 100  Cure Disease
    clears character status 0x0400

dispatch 104  Remove Paralysis
    clears character status 0x1000

dispatch 113  Neutralize Poison
    clears character status 0x0800
```

[Status attacks](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/STATUS_ATTACKS.md) [Character status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_WORD.md)

These exact bit lifecycles should control the native spell behavior.

## 71. Death and revival are character-state transitions, not occupancy changes

The character model distinguishes:

```text
0x4000  dead
0x8000  empty active-party slot
```

The certified revival path restores:

```text
current_hp = 1
clear status bit 0x4000
```

without converting the record through the empty-slot state. [Character status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/STATUS_WORD.md)

The priest spell catalogue contains dispatch 123 `RAISE DEAD`; the archive retains the exact revival-state behavior as the compatibility boundary without inventing a broader resurrection model beyond the proved call chains.

## 72. Cure and healing paths clamp against serialized maximum HP

Character state stores:

```text
current_hp
max_hp
```

Healing paths add their computed amount and clamp against `max_hp`. [Character record](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

Fortify modifies the generated cure amount before this ordinary healing/clamp behavior.

This keeps:

```text
spell healing quantity
character HP clamp
```

as separate layers.

## 73. Cause Wounds and Harm use temporary hand delivery

Priest actions:

```text
82   Cause Light Wounds
111  Cause Serious Wounds
121  Cause Critical Wounds
131  Harm
```

correspond to temporary hand pseudo-items:

```text
0x81  CAUSE LIGHT WOUNDS
0x82  CAUSE SERIOUS WOUNDS
0x83  CAUSE CRITICAL WOUNDS
0x84  HARM
```

[Spell catalogue](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_CATALOGUE.md) [Character tail](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_TAIL.md)

The effect is therefore armed into a hand and delivered through the player attack machinery rather than applied as an immediate target-HP subtraction at spell-selection time.

## 74. Mage touch spells use the same delivery architecture

Mage actions including:

```text
CHILL TOUCH
VAMPIRIC TOUCH
CLAWS OF THE UMBER HULK
LICH TOUCH
```

likewise map into the temporary hand-effect subsystem.

This unifies a set of otherwise different spell names under one execution mechanism:

```text
cast
    -> arm temporary hand pseudo-item
    -> ordinary attack targeting/hit path
    -> pseudo-item-specific damage/effect
    -> expiry/removal if not consumed earlier
```

## 75. Scrolls reuse spell handlers without becoming prepared spells

Category-14 OBJECTS records map to spell dispatch IDs.

Using a scroll:

- does not require that dispatch to be present as a ready copy in the character's preparation matrix;
- does not change the identity of the underlying spell handler;
- preserves the physical scroll object as a separate object/item identity.

Representative chain:

```text
OBJECTS 188
MAGE SCROLL OF 'LIGHTNING BOLT'
    -> scroll dispatcher
    -> dispatch 28
    -> ordinary Lightning Bolt handler
```

[Spell resource lookup](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/SPELL_RESOURCE_LOOKUP.md) [Targeted immunities](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/NPCS.DAT/TARGETED_EFFECT_IMMUNITIES.md)

## 76. Charged devices also reuse magic effect classes

Charged OBJECTS definitions select mutable charge bytes through `OBJECTS +0x31`.

Proved examples:

```text
WAND OF FIREBALLS
    -> class-3 type 0

WAND OF FROST
    -> class-3 type 3

WAND OF MAGIC MISSILES
    -> class-3 type 6

WAND OF ENEMY DETECTION
    -> primary effect 25
    -> shared enemy-detection state 0xF03E
```

[Object/inventory chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md) [Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The physical wand object, charge-counter identity, persistent character/global effect state, and renderer effect type are all different namespaces.

## 77. Device depletion remains item state, not spell state

Charged-device use:

1. resolves the object's `state_index`;
2. reads its SAVE-global charge byte;
3. rejects use when zero;
4. invokes the corresponding item/effect behavior;
5. decrements the charge on successful use;
6. runs depletion/removal behavior when the counter reaches zero.

[OBJECTS category semantics](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/OBJECTS.DAT/CATEGORY_SEMANTICS.md)

A native spell dispatcher should therefore not own wand charges.

## 78. LEVELS scripts can create the same class-3 effect types

LEVELS event opcode 3 calls the common transient-effect constructor.

Across the supplied level corpus, its serialized type argument uses:

```text
0
1
```

which are the same class-3 effect classes used by Fireball and Lightning Bolt paths. [LEVELS event scripts](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md) [Global SAVE state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Thus a class-3 renderer/effect type can originate from:

- prepared spell;
- scroll;
- charged device;
- level script.

Its producer is not encoded solely by `effect_type`.

## 79. Magic identity exists in multiple namespaces simultaneously

The complete system requires keeping at least these identities distinct:

```text
preparation identity
    family / bank / entry

spell dispatch ID
    compact executable action ID

text-resource ID
    T index

primary-effect index
    0..25 character persistent-effect slot

temporary-hand index
    0..8

temporary pseudo-object ID
    0x7E..0x86

class-3 effect type
    0..11

scroll/device OBJECTS ID

charged-device counter index

NPC target/immunity mask

source actor/context
```

Many mappings are one-to-many or many-to-one.

A native API that uses one universal `SpellId` for all of these without explicit conversion layers will obscure original behavior.

