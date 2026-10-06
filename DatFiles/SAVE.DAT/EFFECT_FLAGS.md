# SAVE.DAT timed character effects

Status: **high-effort executable-audited structure and expiration behavior, normalized to runtime character base DGROUP `0x218E0`; all 26 primary-effect slots now have producer-backed source identities.** See `PRIMARY_EFFECT_CATALOGUE.md` for the complete Armor-through-Wand-of-Enemy-Detection index catalogue.

Each `0x248` character record contains a persistent 26-entry primary-effect subsystem:

```text
+0x132..+0x135  uint32 active_effect_bits
+0x136..+0x19D  uint32 effect_expiry[26]
```

For effect index `i` in `0..25`, bit `i` corresponds to `effect_expiry[i]`. Some slots have special removal conditions and do not necessarily use a positive ordinary expiry value.

```c
uint32_t active_effect_bits;  // record +0x132
uint32_t effect_expiry[26];   // record +0x136
```

## Certified index identities

The application-side audit now closes the source identity of every primary slot:

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

These identities come from the spell dispatcher plus serialized spell-scroll names for spell-derived slots, and direct OBJECTS/item-use/equipment paths for the non-spell upper slots. They are not inferred merely from bit order. `PRIMARY_EFFECT_CATALOGUE.md` records the producer address and persistent auxiliary state for every index.

## Timer scan

CODE.2 physical `0x43957..0x43988` tests runtime `0x21A12` and walks effect indexes. For an active effect with a positive expiry dword, the engine compares the stored value with current gameplay time and invokes physical `0x43640` when the expiry is reached.

The expiration handler removes `1 << effect_index` from runtime `0x21A12` and zeros the corresponding timer at runtime `0x21A16 + effect_index*4`.

Effects with special semantics—including Invisibility 10' Radius, Negative Plane Protection, Fortify, and the equipment-derived lightning-protection state—can be removed through non-timer paths and must not be forced into a uniform timed model.

## Expiration text resources

The handler maps the effect index directly to the common `T` archive:

```text
expiration message T index = 0x24C + effect_index
```

Thus effect 0 uses `T[588]` and effect 25 uses `T[613]`. The producer catalogue now establishes slot identity independently of those strings; decoding `T[588..613]` remains a useful text-level corroboration and is still needed if exact original expiration wording is desired.

## Re-audited expiration dispatch

A high-effort instruction-by-instruction recheck of physical `0x436EF..0x4385A` corrected an earlier index-10/index-11 transcription error. The executable dispatch is:

```text
index  1  Mage Light -> invokes renderer-state update via 0x6CE5C(0xF91C)
index  6  Stoneskin -> decrements runtime 0x21AD2 / record +0x1F2
index 10  Priest Light -> invokes the same renderer-state update as index 1
index 11  Aid -> clears runtime 0x21ACA / record +0x1EA
index 12  Draw Upon Holy Might - STR -> reverses +0x123 through 0x4CF50
index 13  Draw Upon Holy Might - DEX -> subtracts low byte of +0x127 from current Dexterity +0x035
index 17  True Seeing -> decrements global dword 0xFD08
index 18  Detect Undead -> decrements global word 0xF03C
index 20  Detect Magic -> decrements global word 0x6DAE and dirties all four party display slots
index 21  Prayer -> clears the corresponding shared effect state across all four party records and dirties all four display slots
index 23  Potion of Giant Strength -> reverses modifier +0x1BE through 0x4CF50
index 25  Wand of Enemy Detection -> decrements global word 0xF03E
```

Indexes not listed still receive the common bit-clear, timer-clear, message, and refresh processing when they expire through this path.

### Correction history

An earlier revision incorrectly assigned the `0x21ACA` clear to effect index 10 and described index 11 as Strength-related cleanup. Direct branch decoding proves instead that indexes 1 and 10 converge on the same renderer-state update at physical `0x4376C`, while index 11 branches to physical `0x4378A` and clears `0x21ACA`. That older interpretation is superseded.

The prior offset-base correction remains valid: runtime `0x21A03` is serialized `+0x123`, not the obsolete `+0x12B` derived from base `0x218D8`.

## Producer-side cross-checks

The generic application path around physical `0x45394` ORs a caller-supplied mask into `active_effect_bits`, computes an absolute expiry when requested, and stores it in the matching timer slot. It is also used for instant/targeted actions, so only calls with an actual primary-effect mask and normal timed path are interpreted as primary-effect producers.

Representative producer chains:

- Mage dispatch 0 / Armor calls the helper with mask `0x00000001`, index 0, and initializes the `+0x117` protection pool;
- Mage dispatch 32 / Stoneskin uses mask `0x00000040`, index 6, and accumulates `+0x1F2` state;
- priest dispatch 90 / Aid uses mask `0x00000800`, index 11, and accumulates `+0x1EA` temporary protection;
- physical `0x458B9..0x45940` sets bit 12/timer 12 and stores the Draw Upon Holy Might Strength delta at `+0x123`;
- physical `0x45A49..0x45AD7` sets bit 13/timer 13 and stores/applies the Draw Upon Holy Might Dexterity delta at `+0x127` / current Dexterity `+0x035`;
- physical `0x61F67` sets bit 19 for OBJECTS ID 61 `POTION OF FIRE RESIST`;
- physical `0x4C9DE` derives bit 22 from equipped OBJECTS ID 103 `SHIELD OF LIGHTNING PROTECTION`;
- physical `0x61FEB..0x62002` stores a Strength adjustment at `+0x1BE` and sets bit 23 for OBJECTS ID 62 `POTION OF GIANT STRENGTH`;
- physical `0x62086` sets bit 24 for OBJECTS ID 64 `POTION OF SPEED`;
- physical `0x61D7C` sets bit 25 for OBJECTS ID 75 `WAND OF ENEMY DETECTION`, increments `0xF03E`, and consumes that object's charged-state counter.

A separate direct producer at physical `0x46A61` sets Mage Light bit `0x00000002` and its corresponding timer at runtime `0x21A1A`, independently confirming bit/timer index pairing.

## Reconstruction rule

A native port must preserve all 26 timer dwords exactly, including inactive or unusual values. Cancellation and expiration must synchronize the relevant bit/timer where the original does so and reproduce index-specific reversal/cleanup behavior. Do not equate primary-effect indexes with spell IDs: five upper slots are item/equipment-derived, and multiple spell effects have special nonstandard removal semantics.
