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
