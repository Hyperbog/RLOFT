# SAVE.DAT timed character effects

Status: **high-effort executable-audited structure, producer identities, expiration behavior, and raw-`T` text corroboration are closed.** Offsets use runtime character base DGROUP `0x218E0`. See `PRIMARY_EFFECT_CATALOGUE.md` for producer details and `T_EFFECT_TEXT.md` for exact decoded labels.

Each `0x248` character record contains:

```text
+0x132..+0x135  uint32 active_effect_bits
+0x136..+0x19D  uint32 effect_expiry[26]
```

For effect index `i` in `0..25`, bit `i` corresponds to `effect_expiry[i]`. Some effects have special removal conditions and do not necessarily carry an ordinary positive expiry.

## Certified identities

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

These identities come from application producers, spell dispatch, OBJECTS item/equipment paths, and now the original `T` labels. They are not inferred merely from bit order.

## Timer scan

CODE.2 physical `0x43957..0x43988` tests runtime `0x21A12` and walks effect indexes. For an active effect with a positive expiry dword, the engine compares it with the pausable gameplay tick counter and invokes physical `0x43640` when it matures.

The common handler:

1. removes `1 << effect_index` from runtime `0x21A12`;
2. displays the expiration text;
3. zeros `0x21A16 + effect_index*4`;
4. runs index-specific cleanup where required.

Effects such as Invisibility 10' Radius, Negative Plane Protection, Fortify, and the equipment-derived lightning-protection state also have non-timer removal paths and must not be forced into a uniform timer model.

## Exact expiration text path

The handler selects:

```text
T index = 0x24C + effect_index
```

The supplied raw `T` file matches the certified SHA-256 and decodes `T[588..613]` to:

```text
 0 Armor                 13 Draw Dexterity
 1 Light                 14 Neg. Plane Prot.
 2 Shield                15 Fortify
 3 Blur                  16 Slow Poison
 4 Haste                 17 True Seeing
 5 Invisibility          18 Detect Undead
 6 Stoneskin             19 Fire Resistance
 7 Dragon Scales         20 Detect Magic
 8 Bless                 21 Prayer
 9 Invis. to Undead      22 Lightning Prot.
10 Light                 23 Giant Strength
11 Aid                   24 Speed
12 Draw Strength         25 Detect Enemy
```

CODE.2 physical `0x43640` assembles the visible line as:

```text
"%s's " + T[0x24C + effect_index] + " wears off."
```

The prefix and suffix come from DGROUP pointers `0x669C` and `0x66A0` respectively.

Important consequence: `T` alone cannot distinguish effect 1 from 10 because both are labeled `Light`; producer tracing is required to separate Mage Light from priest Light. Likewise the upper item/equipment-derived effects use generic labels rather than source-item names.

## Re-audited expiration dispatch

A high-effort instruction-by-instruction recheck of physical `0x436EF..0x4385A` corrected an earlier index-10/index-11 transcription error. Index-specific cleanup is:

```text
index  1  Mage Light -> renderer-state update via 0x6CE5C(0xF91C)
index  6  Stoneskin -> decrements runtime 0x21AD2 / record +0x1F2
index 10  Priest Light -> same renderer-state update as index 1
index 11  Aid -> clears runtime 0x21ACA / record +0x1EA
index 12  Draw Upon Holy Might - STR -> reverses +0x123 through 0x4CF50
index 13  Draw Upon Holy Might - DEX -> subtracts low byte of +0x127 from current Dexterity +0x035
index 17  True Seeing -> decrements global dword 0xFD08
index 18  Detect Undead -> decrements global word 0xF03C
index 20  Detect Magic -> decrements global word 0x6DAE and dirties all four party displays
index 21  Prayer -> clears corresponding shared effect state across all four party records
index 23  Potion of Giant Strength -> reverses modifier +0x1BE through 0x4CF50
index 25  Wand of Enemy Detection -> decrements global word 0xF03E
```

Indexes not listed still receive the common bit clear, timer clear, text, and refresh processing when they expire through this path.

### Correction history

An earlier revision incorrectly assigned the `0x21ACA` clear to effect index 10 and described index 11 as Strength-related cleanup. Direct branch decoding proves indexes 1 and 10 converge on the renderer update, while index 11 clears `0x21ACA`. That older interpretation is superseded.

The prior offset-base correction also remains valid: runtime `0x21A03` is serialized `+0x123`, not the obsolete `+0x12B` derived from base `0x218D8`.

## Producer-side cross-checks

Representative producer chains:

- Armor / index 0 initializes the `+0x117` protection pool;
- Stoneskin / index 6 accumulates `+0x1F2` state;
- Aid / index 11 accumulates `+0x1EA` temporary protection;
- Draw Upon Holy Might STR/DEX set indexes 12/13 and store reversible deltas at `+0x123/+0x127`;
- OBJECTS ID 61 `POTION OF FIRE RESIST` sets index 19;
- equipped OBJECTS ID 103 `SHIELD OF LIGHTNING PROTECTION` derives index 22;
- OBJECTS ID 62 `POTION OF GIANT STRENGTH` sets index 23 and stores `+0x1BE`;
- OBJECTS ID 64 `POTION OF SPEED` sets index 24;
- OBJECTS ID 75 `WAND OF ENEMY DETECTION` sets index 25 and consumes charged-item state.

A separate direct producer sets Mage Light bit `0x00000002` and timer index 1, independently confirming bit/timer pairing.

## Reconstruction rule

Preserve all 26 timer dwords exactly, including inactive or unusual values. Cancellation and expiration must synchronize the relevant bit/timer only where the original does so and reproduce index-specific cleanup. Keep original T labels separate from normalized internal names so a faithful UI can reproduce the legacy wording exactly.

## Directly closed special-removal paths

A preservation-pass re-disassembly closed three previously broad/special lifecycle descriptions.

### Invisibility 10' Radius and Invisibility to Undead break on attack

Logical `0xE578`, called by the player attack path before hit resolution, performs two distinct removals.

First it loops over all four active-party record positions. For each record whose primary-effect bit 5 is set:

```text
effect 5  Invisibility 10' Radius
```

it:

- marks that character display dirty;
- clears primary-effect bit 5.

The helper does not zero an ordinary expiry slot because the producer path does not install one here.

It then examines the **acting character**. If primary-effect bit 9 is set:

```text
effect 9  Invisibility to Undead
```

it:

- zeros effect-expiry slot 9 at record `+0x15A`;
- clears effect bit 9;
- marks the character display dirty.

Thus one physical attack action breaks the party-wide radius invisibility state across the party and separately breaks the attacker's Invisibility to Undead state.

### Negative Plane Protection is consumed by qualifying undead/drain attacks

In the ordinary NPC attack family, when a target character has effect bit 14:

```text
Negative Plane Protection
```

the executable recognizes qualifying undead/drain circumstances before the corresponding level/Strength-drain outcomes are allowed.

The protected path sets a local protection-consumed marker instead of running the ordinary drain result. After the attack's damage phase, that marker causes CODE.2 to:

- run the protection-consumption presentation/effect path;
- zero effect-expiry slot 14 at record `+0x16E`;
- clear primary-effect bit 14.

The result is a one-hit/special-consumption lifecycle rather than a uniform ordinary positive-timer effect.

### Fortify is a one-shot cure maximizer

Cure handlers test primary-effect bit 15:

```text
Fortify
```

on the cure-source/caster-side character state.

When inactive, the handlers use their ordinary random healing quantities.

When active, the same handlers substitute fixed maximum values and clear Fortify immediately:

```text
Cure Light Wounds       ->  8
Cure Serious Wounds     -> 16
Cure Critical Wounds    -> 27
```

Fortify therefore belongs to a one-shot spell-modification lifecycle, not the common timed-expiration family.

These direct paths tighten the earlier manual-correlated descriptions and should control native compatibility behavior.
