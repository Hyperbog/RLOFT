# OBJECTS.DAT packed attack profile (`+0x35`)

Status: **weapon speed and hand-pair behavior are decoded from certified CODE.2. The low two bits are a hand-pair compatibility class; class 2 is behaviorally proved paired-hand-exclusive. Classes 0 and 1 remain distinct pairable classes without asserted source-level names.**

The physical field is an unaligned-independent 16-bit little-endian word at object-record `+0x35`:

```text
bits 0..1  hand_pair_class
bits 2..5  weapon_speed_factor
bits 6..15 preserved; zero in the supplied database
```

## Speed factor: bits 2..5

CODE.2 extracts:

```text
speed = (attack_profile >> 2) & 0x0F
```

and the attack-action scheduler multiplies the value by nine before installing the corresponding action delay/deadline.

The supplied corpus aligns with recognizable relative weapon speeds:

```text
DAGGER               2
SHORT SWORD          3
QUARTERSTAFF         4
WARHAMMER            4
LONG SWORD           5
BROAD SWORD          6
SPEAR                 6
BATTLE AXE           7
MACE                  7
COMPOSITE BOW        7
HALBERD               9
TWO-HANDED SWORD    10
```

The project name `weapon_speed_factor` is therefore supported.

## Low two bits: hand-pair class

The former neutral name `attack_mode` can be tightened from `23FC4..24A5D`.

`app51_low_pair_class` returns exactly:

```text
object.attack_profile & 0x0003
```

and `function_24478` uses the result while arbitrating actor equipment slots 14 and 15, the proved paired hand slots.

This makes **`hand_pair_class`** the safer behavioral field name. It describes where the bits are actually consumed without claiming an original AD&D enum.

### Class 2 — paired-hand-exclusive

CODE.2 applies a strong rule for value 2:

- if one qualifying hand item has class 2, the opposite hand is rejected;
- if two qualifying hand items are present and either has class 2, the pair is rejected.

All seven supplied class-2 records are:

```text
ID  38  QUARTERSTAFF
ID  44  TWO-HANDED SWORD
ID  45  HALBERD
ID  48  COMPOSITE BOW
ID  90  JAVELIN OF LIGHTNING
ID 248  QUARTERSTAFF
ID 249  QUARTERSTAFF
```

Because COMPOSITE BOW and JAVELIN OF LIGHTNING also use the class, the executable-safe label is **paired-hand-exclusive** rather than simply `two_handed`.

### Class 1

Among the supplied ordinary weapon families, class 1 is used by:

```text
BATTLE AXE
MACE
LONG SWORD
BROAD SWORD
WARHAMMER
SPEAR
ANCIENT ELVEN SWORD
PALADIN'S SWORD
```

including duplicate definitions.

These are pairable under the class-0/1 logic, subject to actor-role predicates and the additional encoded weight/rank arbitration. The exact source-level meaning separating class 1 from class 0 is not proved.

### Class 0

Class 0 includes many nonweapons because the field defaults to zero, plus weapon definitions such as:

```text
SHORT SWORD
DAGGER
THROWING KNIFE
ROD OF SMITING
AXE OF HURLING
DAGGER OF THROWING
BA'AL VERZI DAGGER
PALADIN'S DAGGER
```

It therefore must not be renamed to a universal item property such as `one_handed` or `light_weapon` without stronger evidence. For hand-qualified items it is one of the pairable compatibility classes.

## Supplied distribution

Across all 336 object records:

```text
hand_pair_class 0 : 311
hand_pair_class 1 :  18
hand_pair_class 2 :   7
hand_pair_class 3 :   0
```

Class 3 is executable-representable but absent from the supplied database.

The category-9/10/11 weapon-family subset contains:

```text
class 0 : 12 records
class 1 : 18 records
class 2 :  5 records
```

The additional class-2 definitions are COMPOSITE BOW (category 0) and JAVELIN OF LIGHTNING (category 0), both explicitly admitted to the hand-compatibility machinery by item-ID predicates.

## Weight interaction

For two qualifying pairable hand items whose classes are 0/1, `function_24478` also compares OBJECTS byte `+0x30`, independently proved to be weight in half-pound units, while resolving which side remains admissible under the current actor role/state.

That encoded behavior is another reason not to reduce `hand_pair_class` to a simple handedness boolean.

## Safe implementation boundary

A faithful implementation may expose:

```text
attack_profile.hand_pair_class
attack_profile.weapon_speed_factor
```

and may label class 2 `paired_hand_exclusive`.

Classes 0 and 1 should remain numeric compatibility classes until a stronger source-level identity is proved. Preserve bits 6..15 even though all supplied records store zero there.
