# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 12 — Combat Resolution, Damage, Saving Throws, Protection, and XP

**Status:** PRIMARY COMBAT RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter reconstructs Ravenloft's combat execution order from the certified CODE.2 object and the already-closed character, OBJECTS, NPCS, LEVELS, and SAVE state models.

It covers:

- player attack target selection;
- player-versus-NPC d20 hit checks;
- NPC-versus-player d20 hit checks;
- character base attack target and NPC THAC0;
- effective player Armor Class;
- Strength and exceptional-Strength attack/damage adjustments;
- burden/encumbrance combat penalties;
- Bless, Aid, Blur, invisibility, Stoneskin, Armor, Dragon Scales, rings, bracers, and gauntlets;
- weapon dice, enchantment, categories, and damage floors;
- temporary hand/touch pseudo-items;
- Rod of Smiting special damage;
- NPC melee dice and secondary attacks;
- two character saving/resistance-table families;
- Armor durability versus true damage-absorption pools;
- character death;
- NPC damage/death;
- XP distribution and multiclass progression;
- attack/cooldown timing;
- recent-damage HUD feedback;
- faithful-port ordering and regression requirements.

The combat model is deliberately reconstructed from executable order rather than generalized from tabletop AD&D rules. Where the shipped binary differs from an expected tabletop interpretation, the binary behavior is controlling.

Primary executable source: [Code2/Code2.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp). Certification: [Code2/FINAL_SEMANTIC_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md).

## 2. Combat joins five previously separate data domains

The core attack path depends on:

```text
character record
    base attack target
    base AC
    current attributes
    status/effects
    hand deadlines
    inventory/equipment

OBJECTS.DAT
    attack profile
    enchantment
    damage dice
    category
    AC adjustment

NPCS.DAT
    AC
    THAC0
    damage dice
    behavior/immunity flags
    XP

family-A live actor
    current HP
    runtime hold/slow flags
    current position/state

SAVE-global time
    A7E4 gameplay tick counter
```

The faithful engine should keep these ownership boundaries intact even though one attack reads all of them.

## 3. Common dice helper

The common dice helper at logical `1C328` is repeatedly called in the form:

```text
dice(dice_count, sides, bonus_A, bonus_B)
```

Combat uses it for:

- d20 hit rolls;
- weapon damage;
- NPC melee damage;
- temporary hand/touch damage;
- status-duration/random quantities.

For hit rolls, the common call is effectively:

```text
1d20 + supplied bonus
```

with the second bonus argument zero.

Exact machine-width arithmetic and the helper's result ordering should be preserved.

## 4. Player attack orchestration

The central player attack path is logical `E5F0..EA81`.

Its high-level order is:

```text
select/validate target
    |
    v
break selected action-sensitive effects
    |
    v
player hit check C80C
    |
    +-- miss -> miss feedback
    |
    v
compute weapon/touch/unarmed damage
    |
    v
item-specific special handling
    |
    v
NPC weapon-category damage gate C910
    |
    v
NPC damage/death handler E284
    |
    v
install outgoing combat-feedback state
    |
    v
install hand-ready deadline
```

This sequence matters. A native implementation that applies weapon gating before item-specific damage or schedules the hand before final duration modifiers can diverge.

## 5. Player target selection

If the caller supplies target index `-1`, the attack path calls target selector `DB94`.

The result is a live family-A actor index or `-1` when no valid target is resolved.

Environment/profile and hand-pair constraints can further invalidate a selected target before the hit test.

The target is represented as:

```text
live actor index
    -> 0x16-byte family-A record
    -> NPCS definition through actor.type_id
```

This retains the definition/live-state split established in Component 11.

## 6. Attacking breaks selected character effects/state

Before the hit test the attack path calls logical `E578`.

That helper clears selected action-sensitive character states, including one character-local effect bit and associated timer/state, and marks UI refresh state.

The exact effect identities belong to the spell/effect chapter; the combat ordering is important:

> attack-side effect breaking occurs before hit resolution.

## 7. Player hit-check routine

Logical `C80C..C90C` receives:

```text
character_index
live_npc_record
npc_definition
weapon_object_id
apply_strength_adjustment
```

The ordinary player attack path passes the final flag as nonzero.

## 8. Held NPCs are automatically hit

The first player-hit test is:

```text
if live_npc.runtime_flags low byte & 0x08:
    return hit
```

The same `0x08` live bit is the held-state bit installed by Hold Person/Undead/Monster paths. [NPC chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md)

No d20 roll is performed for this branch.

## 9. Player attack threshold

For a non-held target, the routine constructs:

```text
threshold =
    character.base_attack_target
  - npc_definition.armor_class
  + attack_adjustment_accumulator
```

where:

```text
character.base_attack_target = record +0x10A
npc armor_class              = NPCS +0x21 signed int8
```

The accumulator incorporates weapon enchantment, Strength adjustment, and burden rank as described below.

## 10. Weapon enchantment enters the hit threshold

If:

```text
weapon_object_id != -1
```

the routine reads:

```text
OBJECTS[weapon].enchantment_bonus
```

from object byte `+0x37`.

That byte begins the attack-adjustment accumulator.

If no weapon is present, the initial value is zero.

This is independent from the object's intrinsic magic marker.

## 11. Strength attack adjustment

Logical `C7C4..C80A` reads:

```text
character current Strength          +0x33
current exceptional Strength        +0x3F
```

For ordinary Strength values it reads a signed attack-adjustment table.

For Strength 18 with nonzero exceptional percentile, it returns encoded bucket values:

```text
01..50   -> 1
51..99   -> 2
100      -> 3
```

The player hit routine subtracts this helper result from the attack-adjustment accumulator:

```text
accumulator -= strength_attack_adjustment
```

Preserve this sign/order exactly rather than translating the table into an assumed tabletop bonus convention.

## 12. Burden/encumbrance rank affects attack accuracy

Logical `FE44` is the certified burden/rank classifier.

It first computes total carried burden through logical `FD84`:

- sums every character item;
- recursively includes category-1 container contents;
- uses OBJECTS half-pound weights;
- converts the resulting half-pound sum into the burden quantity used by its Strength/exceptional-Strength threshold tables.

[CODE.2 continuity](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

The classifier returns rank `0..4`.

The player attack threshold applies:

```text
rank 0  +0
rank 1  +0
rank 2  +1
rank 3  +2
rank 4  +4
```

Higher threshold means a harder hit check.

Thus inventory burden is mechanically part of combat accuracy.

## 13. Slowed NPCs are easier for the player to hit

After constructing the threshold, the routine checks live NPC runtime flag:

```text
0x80
```

the Slow state documented in Component 11.

If set:

```text
threshold -= 4
```

This is applied before the d20 result is compared.

## 14. Player d20 bonus sources

The player hit routine constructs a separate roll bonus.

It adds:

```text
+1  character primary effect 8  Bless
+1  character primary effect 11 Aid
+3  if equipment slot 20 contains object 0x54
     STONE GIANT GAUNTLETS
```

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md) [Object chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md)

It then calls the common dice helper as:

```text
1d20 + roll_bonus
```

## 15. Player auto-hit/auto-miss checks use the modified result

The encoded order is:

```text
result = 1d20 + roll_bonus

if result >= 20:
    hit

else if result == 1:
    miss

else:
    hit if result >= threshold
```

This is not equivalent to a generic rule that always tests the raw natural d20 before modifiers.

For example, a positive roll bonus can move a raw roll away from the literal encoded value 1 or up to/exceed 20 before these checks.

A faithful implementation must reproduce the binary ordering.

## 16. Player hit equation summary

For ordinary player attacks:

```text
A = weapon_enchantment_or_zero
A -= strength_attack_adjustment

A += burden_penalty:
     rank2 +1
     rank3 +2
     rank4 +4

threshold =
    base_attack_target
  - npc_AC
  + A

if NPC slowed:
    threshold -= 4

roll_bonus =
    Bless ? 1 : 0
  + Aid   ? 1 : 0
  + StoneGiantGauntlets ? 3 : 0

roll = 1d20 + roll_bonus

if roll >= 20 -> hit
if roll == 1  -> miss
else          -> roll >= threshold
```

This equation is reconstructed from CODE.2, not inferred from manuals.

## 17. Player Strength damage adjustment is separate from attack adjustment

The player damage path separately reads:

```text
current Strength
current exceptional Strength
```

and derives a damage adjustment.

For Strength 18, it checks character class membership and exceptional-percentile ranges before selecting its encoded damage adjustment.

This is a separate calculation from logical `C7C4` used by the hit check.

A native implementation should therefore expose separate helpers:

```text
strength_attack_adjustment
strength_damage_adjustment
```

even when both consult the same serialized attributes.

## 18. Ordinary weapon damage

For a normal weapon, logical `E86E..E899` reads:

```text
OBJECTS +0x38  damage_sm_dice
OBJECTS +0x39  damage_sm_sides
OBJECTS +0x3A  damage_sm_bonus
```

and invokes the common dice helper with:

```text
weapon dice
weapon small-target bonus
Strength damage adjustment
```

The resulting damage value is then subject to item-specific handling and the NPC weapon-category gate.

The OBJECTS record separately stores the alternate large-target profile at `+0x3B..+0x3D`; the ordinary central melee path above demonstrably reads the first profile.

## 19. Unarmed player damage

When:

```text
weapon_object_id == -1
```

the central player attack path uses:

```text
1d2
```

for the ordinary unarmed damage roll in this routine.

The later NPC weapon-category gate treats `weapon_id == -1` as synthetic category 9.

## 20. Temporary hand/touch pseudo-items have dedicated damage cases

Object IDs:

```text
0x7E..0x86
```

are the temporary hand/touch pseudo-items documented in Components 09/10.

The player attack path dispatches them through a dedicated nine-entry code table with hard-coded dice/effect behavior rather than ordinary OBJECTS weapon damage.

This is why ordinary inventory swapping refuses to move them.

Examples include:

- Chill Touch;
- Vampiric Touch;
- Cause Wounds variants;
- Harm;
- Flame Blade;
- Lich Touch.

## 21. Vampiric Touch stores damage as later protection state

For pseudo-item:

```text
0x7F VAMPIRIC TOUCH
```

after the hit/damage sequence the attack path stores the resulting damage value into character:

```text
+0x113
```

the certified Vampiric Touch absorption pool.

Thus offensive damage generation and later defensive absorption share one serialized auxiliary.

## 22. Rod of Smiting has dedicated target/charge damage handling

Object:

```text
0x4A ROD OF SMITING
```

bypasses the ordinary weapon-category gate in selected cases and contains target-type-specific damage/charge logic.

It directly consults:

```text
OBJECTS +0x31 charge_counter_index
SAVE-global charge vector 0x67EC
```

and can decrement its charge counter.

The exact special target cases remain item-specific executable behavior; do not reduce Rod of Smiting to ordinary weapon dice plus a generic enchantment.

## 23. NPC weapon-category damage gate

Normal player weapon damage enters logical:

```text
C910..C99E
```

before the NPC damage/death routine.

Inputs are:

```text
damage
NPC definition
weapon object ID
```

The routine derives an attack category:

```text
weapon == -1
    -> category 9

else
    category = OBJECTS.item_category

    if OBJECTS.enchantment_bonus != 0:
        category = 11
```

[NPC weapon flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

## 24. Category-9 and category-10 defenses are one-damage floors

For unenchanted category 9:

```text
if npc.behavior_flags & 0x00008000:
    intermediate damage = 0
```

For unenchanted category 10:

```text
if npc.behavior_flags & 0x00004000:
    intermediate damage = 0
```

But the routine ends with:

```text
if damage < 1:
    damage = 1
```

Therefore these are not total immunity flags.

Externally they create:

```text
minimum-one-damage result
```

for matching weapon categories.

## 25. Enchanted weapons are rerouted before those category gates

Any nonzero OBJECTS enchantment byte forces:

```text
category = 11
```

before the category-9/category-10 logic.

Thus an enchanted weapon does not take the same NPC resistance branch as its unenchanted category.

Order is:

```text
read physical category
    |
    +-- enchantment != 0 -> synthetic 11
    |
    v
NPC category gate
```

## 26. Category-11 high-bit behavior preserves a signedness anomaly

For synthetic category 11, NPC main behavior bits:

```text
0x00020000
0x00040000
```

enter additional checks.

The executable zero-extends OBJECTS enchantment and compares it with sign-extended immediate `-2` using signed conditional branches.

For the supplied nonnegative enchantment domain:

- `0x00020000` drives damage to the one-damage floor;
- `0x00040000` does not independently reduce supplied-object damage.

[NPC weapon flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)

This apparent signedness oddity is preserved behavior, not a license for a cleanup rewrite.

## 27. Player attack duration comes from weapon speed

After resolving the action, the player attack path computes:

```text
weapon_speed_factor =
    (OBJECTS.attack_profile >> 2) & 0x0F

base_duration =
    weapon_speed_factor * 9
```

For:

- unarmed attacks;
- temporary hand/touch pseudo-items;

the routine uses literal:

```text
0x12 = 18
```

equivalent to a speed-factor-2 duration.

[OBJECTS attack profile](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)

## 28. Haste and Potion of Speed halve attack duration

The player attack path tests combined primary-effect mask:

```text
0x01000010
```

which corresponds to:

```text
bit 4   Haste
bit 24  Potion of Speed
```

If either is active:

```text
duration = signed_half(duration)
```

using the encoded signed-rounding sequence.

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

## 29. Hand-ready deadline installation

Logical `24B48` receives the character, hand index, presentation/control state, and duration.

When duration is nonzero:

```text
hand_ready_deadline[hand] =
    current_gameplay_time + duration
```

where:

```text
hand_ready_deadline[0] = character +0x10B
hand_ready_deadline[1] = character +0x10F
```

This is the actual hand cooldown gate documented structurally in Component 09.

## 30. Successful attacks also create recent outgoing damage feedback

When the attack hit-check succeeds, logical `EA35..EA5C` stores:

```text
combat_feedback_deadline[hand] =
    current_gameplay_time + duration/2

combat_feedback_damage[hand] =
    final damage value
```

[Combat-feedback state](https://github.com/Hyperbog/RLOFT/blob/d40af8a3660ca126f1635279fda7c4015a3dff4c/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)

Thus combat timing has two separate uses:

```text
full duration
    -> next hand availability

half duration
    -> recent outgoing damage display lifetime
```

## 31. NPC attack path

The main ordinary NPC attack path is logical `D551..DACA`.

After actor-specific targeting/spatial checks it performs:

```text
target character selection
    |
    v
NPC hit check C9A0
    |
    +-- miss -> feedback
    |
    v
NPC melee damage dice
    |
    v
special drain/status effects
    |
    v
common character damage CD44
    |
    v
attack sound
```

Some specific NPC types take special attack/projectile/story branches before this ordinary path.

## 32. NPC hit-check routine

Logical `C9A0..CA35` receives:

```text
target character
NPC definition
live NPC actor
```

It starts with an unmodified:

```text
1d20
```

and tests literal natural values before most defensive modifiers.

## 33. NPC natural 20/1 ordering

The encoded order is:

```text
roll = 1d20

if roll == 20:
    hit

if roll == 1:
    miss

otherwise:
    compute effective AC
    apply defensive-effect modifiers
    compare against threshold
```

This differs from the player hit routine, whose automatic checks use the already-bonused result.

The distinction is machine-proven and should remain explicit.

## 34. Effective character AC

Logical `EFA0..F166` constructs effective Armor Class from:

```text
character.base_armor_class
+
equipped armor/head/hand adjustments
+
eligible equipment enchantment
+
Dexterity adjustment
+
burden penalties
+
character effects
+
special equipment
```

It returns a signed integer; lower values are better because NPC hit threshold is:

```text
npc.THAC0 - effective_AC
```

## 35. Armor/enchantment equipment contribution

For qualifying equipped objects, helper `EF34` returns:

```text
-enchantment_bonus
```

only for object categories:

```text
2..8
```

Otherwise it returns zero.

Effective AC independently adds each item's signed:

```text
OBJECTS +0x34 armor_class_adjustment
```

where applicable.

The routine processes:

- torso slot 16;
- head slot 17;
- hand A slot 14 when `24478` says usable;
- hand B slot 15 when `24478` says usable.

## 36. Dexterity contributes through a signed table

The effective-AC helper reads:

```text
character current Dexterity +0x35
```

and indexes a signed table at runtime `0x6FAF`.

The table contribution is added directly to effective AC.

Preserve the executable table rather than substituting a separately reconstructed tabletop formula.

## 37. Bracers of Protection special path

If equipment slot 20 contains:

```text
OBJECTS 0x51  BRACERS OF PROTECTION
```

and the helper determining category-4 body armor reports false, the routine:

1. caps the current intermediate AC at 5 when it is worse than 5;
2. reapplies the torso enchantment helper contribution.

This is a dedicated object-ID behavior, not a generic category-7 rule.

## 38. Burden/encumbrance rank also worsens effective AC

Effective AC calls the same burden classifier `FE44`.

It applies:

```text
rank 0  +0 AC
rank 1  +0 AC
rank 2  +0 AC
rank 3  +1 AC
rank 4  +3 AC
```

Because higher AC is worse in this system, rank 3/4 are defensive penalties.

Combined with player attack effects:

```text
rank 2  attack threshold +1
rank 3  attack threshold +2, AC +1
rank 4  attack threshold +4, AC +3
```

Encumbrance is therefore a two-sided combat mechanic.

## 39. Armor spell caps effective AC

If primary effect 0 `Armor` is active, and:

- effective AC is worse than 6;
- category-4 body-armor helper is false;

then:

```text
effective_AC = 6
```

This is a live effect on effective defense, separate from the Armor protection/durability quantity at character `+0x117`.

## 40. Dragon Scales and Ring of Protection modify AC

Primary effect 7:

```text
Dragon Scales
```

applies:

```text
effective_AC -= 2
```

The ring helper tests dedicated ring slots 12/13 for:

```text
OBJECTS 0x46  RING OF PROTECTION
```

and applies:

```text
effective_AC -= 1
```

when present.

These are explicit executable modifiers.

## 41. NPC THAC0 threshold

After effective AC:

```text
threshold =
    npc_definition.thac0
  - effective_character_AC
```

NPC THAC0 is definition byte:

```text
NPCS +0x22
```

[NPCS chapter](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md)

## 42. Stoneskin intercepts an NPC attack before ordinary comparison

Before applying invisibility/Bless/Blur roll modifiers, the NPC hit routine tests primary effect 6:

```text
Stoneskin
```

If active, it calls the effect-consumption/removal helper for index 6 and returns a miss/no-hit result.

Thus Stoneskin intercept occurs:

```text
after natural 20/1 checks
after effective AC computation
before ordinary threshold comparison
```

This ordering is exact.

## 43. Defensive roll modifiers

For a non-Stoneskin ordinary comparison, the NPC roll variable is modified:

```text
primary effect 5  Invisibility 10' Radius
    roll -= 2

primary effect 8  Bless
    roll -= 1

primary effect 3  Blur
    roll -= 2
```

The encoded Bless behavior is preserved as written even if one might expect a different tabletop interpretation.

Binary behavior is controlling.

## 44. Slow makes the NPC less accurate

If live NPC runtime flag `0x80` is set:

```text
threshold += 4
```

Since the NPC must meet/exceed the threshold, Slow makes the attack harder.

The same live bit made the slowed NPC easier for player attacks by subtracting 4 from the player threshold.

## 45. NPC hit equation summary

```text
raw_roll = 1d20

if raw_roll == 20 -> hit
if raw_roll == 1  -> miss

effective_AC = EFA0(character)

if Stoneskin:
    consume Stoneskin layer/state
    -> miss

modified_roll = raw_roll

if Invisibility 10' Radius:
    modified_roll -= 2

if Bless:
    modified_roll -= 1

if Blur:
    modified_roll -= 2

threshold = npc.THAC0 - effective_AC

if NPC slowed:
    threshold += 4

hit = modified_roll >= threshold
```

This exact ordering should become the native reference implementation.



## 46. NPC melee damage uses definition dice directly

After an ordinary NPC attack passes the hit check, the main attack path reads:

```text
NPCS +0x1E  melee_damage_dice
NPCS +0x1F  melee_damage_sides
NPCS +0x20  melee_damage_bonus
```

and sends those values through the common dice helper.

[NPCS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FORMAT.md)

The resulting damage value is still not necessarily the target character's final HP loss. Special status/drain processing and the common character-damage routine execute around/after the basic melee calculation.

## 47. Secondary NPC attacks are separate capability paths

NPC melee can additionally trigger definition-controlled effects:

```text
behavior_flags_39 & 0x01  poison
behavior_flags_39 & 0x20  paralysis

behavior_flags_3A & 0x02  level drain
behavior_flags_3A & 0x04  Strength drain
behavior_flags_3A & 0x10  disease
```

[NPC behavior flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)

These are not encoded as extra dice in `+0x1E..+0x20`.

A native combat profile should therefore separate:

```text
base melee damage
secondary status/drain capability
```

## 48. Common character status-saving helper

Logical `CEE4` is a class/level-indexed d20 saving helper used by several NPC status/drain paths.

Its table lookup is:

```text
class_code = character.class_code[0]

table_offset =
    class_code * 0x5C
  + character.class_level[0] * 4

threshold =
    dword table at 0x6888 + table_offset

roll = 1d20

success =
    roll >= threshold
```

The executable uses the character's **first class slot** for this helper.

Direct uses include several NPC special-attack/status paths such as poison, paralysis, disease, and drain processing.

The safe preservation name is:

```text
character_status_save_family_A
```

The exact original AD&D saving-throw category name is not asserted solely from this table geometry.

## 49. Class/level indexing is executable state, not a generic save API

The `CEE4` helper multiplies the stored class code by `0x5C` and the stored first-class level by four.

That is a concrete data-layout dependency:

```text
save family A
    indexed by class_code[0]
    and class_level[0]
```

A modern engine may expose a named saving-throw abstraction, but faithful behavior should still be regression-tested against the original table bytes and exact first-class selection rule.

## 50. A second saving-throw table family exists

Logical `CF2C(character, modifier)` performs a distinct class/level-indexed saving calculation.

It uses the same broad class/level selection form but a different table family beginning at runtime `0x6AB0`.

The encoded comparison is:

```text
threshold =
    save_table_family_B[class, level]
  + caller_modifier

roll = 1d20

success =
    roll >= threshold
```

This is not the same lookup table used by `CEE4`.

Therefore Ravenloft combat contains at least two mechanically distinct saving-table families.

## 51. Save family B is directly tied to damage mitigation

A direct caller at logical `92B8` invokes `CF2C` during a damage/effect path.

When the save succeeds, the caller halves damage using the executable's signed-half sequence.

Thus a safe behavioral name is:

```text
modifier_bearing_damage_save_family_B
```

until a stronger historical source identifies the exact original saving-throw category.

The preservation rule is:

> do not collapse all combat saving throws into one class/level table.

## 52. Saving-throw success is compared after table/modifier construction

Both saving helpers use an ordinary d20 result compared to a calculated threshold.

They are not the same as the player/NPC **attack** natural-1/natural-20 hit logic documented above.

The native model should therefore keep:

```text
attack roll
saving throw family A
saving throw family B
```

as separate reference functions.

## 53. Common character-damage routine

Logical `CD44..CEE3` is the central HP-damage application path for party characters.

Its reconstructed high-level order is:

```text
record incoming damage feedback
    |
    v
deplete Armor protection quantity
    |
    v
consume Aid true absorption
    |
    v
consume Vampiric Touch true absorption
    |
    v
subtract remaining damage from HP
    |
    v
refresh character state/UI
    |
    +-- still alive -> return
    |
    v
death/cleanup processing
```

This order is CODE-PROVEN and is important because the three protection quantities do not have the same semantics.

## 54. Incoming combat-feedback is recorded before mitigation

At entry, the routine stores the original incoming damage argument in combat-feedback slot 2:

```text
combat_feedback_damage[2] =
    original incoming damage

combat_feedback_deadline[2] =
    gameplay_time + 0x36
```

[Combat-feedback state](https://github.com/Hyperbog/RLOFT/blob/d40af8a3660ca126f1635279fda7c4015a3dff4c/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)

Therefore the short-lived HUD damage number is not necessarily the same as final HP loss after true absorption pools.

## 55. Armor +0x117 is durability/protection state, not HP absorption

Character dword:

```text
+0x117  Armor protection/durability quantity
```

is decremented by the **full incoming damage amount**.

If the result reaches zero or below:

```text
quantity = 0
clear primary effect 0 Armor
```

But the routine does **not** subtract this quantity from the local damage value before continuing.

[Post-inventory character state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

Thus Armor can wear down in response to hits without absorbing HP damage at this stage.

This corrects the earlier over-specific `armor_absorption_pool` interpretation.

## 56. Aid +0x1EA is a true damage-absorption pool

Character dword:

```text
+0x1EA
```

associated with primary effect 11 Aid is consumed against remaining damage.

Encoded behavior:

```text
if damage >= aid_pool:
    damage -= aid_pool
    aid_pool = 0
else:
    aid_pool -= damage
    damage = 0
```

Only the post-Aid remaining value continues toward later protection/HP processing.

[Character record](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

## 57. Vampiric Touch +0x113 is another true absorption pool

After Aid processing, character:

```text
+0x113  Vampiric Touch absorption pool
```

is applied using the same absorb-until-empty form:

```text
if damage >= vampiric_pool:
    damage -= vampiric_pool
    vampiric_pool = 0
else:
    vampiric_pool -= damage
    damage = 0
```

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

The exact protection sequence is therefore:

```text
Armor durability
    does not reduce damage

Aid pool
    reduces damage

Vampiric Touch pool
    reduces remaining damage

HP
    receives final remainder
```

## 58. Character HP subtraction occurs only after true absorption

The common routine subtracts the post-pool damage value from:

```text
character.current_hp
```

at record `+0x01C`.

This means a fully absorbed hit can still:

- consume Armor durability;
- create incoming combat-feedback;
- leave HP unchanged.

That is observable behavior and should be covered by native tests.

## 59. Character death state

If HP falls to zero or below, the routine:

```text
current_hp = 0

status_word |= 0x4000   // death

status high byte &= 0xE3
```

The high-byte mask clears the proved:

- disease;
- poison;
- paralysis;

condition bits while preserving other compatible state.

[Character status](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

It then calls the character death/cleanup helper and refreshes dependent UI/spell selection state.

## 60. Party incapacitation/game-over test

After character death processing, the engine scans all four active-party records.

The terminal condition tests character status against mask:

```text
0xD040
```

and, when the complete party satisfies the encoded incapacitated/dead/blocked condition, enters the game-over path.

This is not equivalent to merely asking:

```text
all current_hp == 0
```

because the condition is status-mask based.



## 61. NPC damage is applied to live actor HP

Logical `E284..E552` is the common NPC damage/death family.

Its target is the **live family-A actor record**:

```text
+0x12  int16 current_hp
```

rather than an HP field in NPCS.DAT.

[Family-A live actor](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

This preserves the definition/instance split:

```text
NPCS.DAT
    immutable HD/AC/THAC0/damage/XP template

family-A actor
    mutable current HP
```

## 62. Surviving NPCs take the reaction-sound path

After damage/effect application:

```text
if live.current_hp > 0
```

the common handler can queue the definition's:

```text
+0x41 damage_reaction_sound_id
```

against the normal creature-sound base.

[NPC sound fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

The surviving reaction path is therefore separate from death audio and death-state transitions.

## 63. NPC death branches on live HP <= 0

When:

```text
live.current_hp <= 0
```

the damage handler enters its death path.

That path can include:

- special/story-specific handling;
- death animation/state mutation;
- definition XP transfer;
- death-sound selection;
- scene/world refresh.

The exact special branches vary by actor/type; a native engine should not replace the original death family with a generic unconditional actor deletion.

## 64. NPC death sound uses the dedicated death bank

Definition:

```text
NPCS +0x43  death_sound_id
```

is resolved relative to the dedicated death-sound base rather than the ordinary creature-sound base.

A serialized value of zero still resolves to the base death sound and therefore does **not** mean silence. [NPC sound fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/SOUND_FIELDS.md)

This should be represented as:

```text
death_resource = death_sound_base + death_sound_id
```

not:

```text
if death_sound_id == 0:
    no sound
```

## 65. Definition XP is passed into party progression

On NPC death, definition word:

```text
NPCS +0x23  experience_award
```

is passed directly to logical XP-distribution routine `DDA4..DFC0`.

[NPCS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FORMAT.md)

The dead live actor does not carry a separate mutable XP field.

## 66. XP eligibility uses character status, not HP alone

XP distribution first counts active-party characters whose:

```text
status_word & 0xF240 == 0
```

Only those records participate in the party-member divisor and later receive class progression.

This means XP eligibility is not simply:

```text
record occupied
AND current_hp > 0
```

It is a specific status-mask predicate.

A faithful progression layer must preserve that predicate exactly.

## 67. Party-member XP share uses rounded integer division

Let:

```text
X = NPC experience_award
N = eligible party-member count
```

The routine computes:

```text
party_share =
    (X + (N >> 1)) / N
```

for the positive XP domain used by the death path.

If the quotient becomes zero:

```text
party_share = 1
```

Thus the game performs rounded integer division rather than simple truncation.

## 68. Multiclass XP is divided again

For each eligible character the routine counts occupied class slots:

```text
class_count =
    1
  + (class_code[1] != -1)
  + (class_code[2] != -1)
```

Then:

```text
class_share =
    (party_share + (class_count >> 1))
  / class_count

if class_share == 0:
    class_share = 1
```

The same resulting class share is added independently to every occupied:

```text
class_progress[i]
```

field.

[Character class layout](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

## 69. Two-stage rounding can change exact total allocation

Because XP is rounded at:

1. the party-member division;
2. the per-character class division;

and each stage has a minimum result of 1, the resulting total progression added across every class record is not guaranteed to be a perfect algebraic partition of the original XP amount.

This is encoded behavior.

A native port must not “correct” it by carrying fractional remainders globally unless intentionally changing gameplay.

## 70. Class progression uses class-specific threshold tables

After adding class XP, the routine selects a class-specific threshold region:

```text
table_base =
    0x7D9C
  + class_code * 0x28
```

and compares threshold values against:

```text
class_progress / 1000
```

to derive the current level.

The resulting level is written back to the matching:

```text
class_level[i]
```

slot when it changes.

The progression model is therefore class-slot-local throughout:

```text
class_code[i]
class_progress[i]
class_level[i]
```

## 71. Level changes trigger side effects

When a class level changes, CODE.2 invokes class/character level-up side-effect logic rather than merely incrementing the serialized level word.

Those side effects can update derived character state.

The complete class-specific level-up rules are outside this combat chapter's scope, but the progression ordering is:

```text
add class XP
    |
    v
recompute level from threshold table
    |
    +-- unchanged -> continue
    |
    v
store new class level
    |
    v
run level-change side effects
```

## 72. Base attack target is recomputed after XP processing

After processing the character's occupied class slots, XP distribution calls logical `DBD4(character)`.

Its result is written to:

```text
character +0x10A
base_attack_target
```

[Post-inventory character state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

Thus the combat target byte is a serialized **derived value** that can change immediately after level progression.

The persistence layer preserves it, but progression logic also knows how to regenerate it.

## 73. Combat and progression therefore form a feedback loop

```text
NPC dies
    |
    v
definition XP
    |
    v
party eligibility
    |
    v
party rounded share
    |
    v
class rounded share
    |
    v
class_progress[]
    |
    v
class_level[]
    |
    v
base_attack_target +0x10A
    |
    v
future player hit thresholds
```

This is why the native port should not isolate progression completely from combat-state recomputation.

## 74. Special NPC attack branch can bypass ordinary melee amount

The main NPC attack family contains a special type-specific branch for NPC type 15.

For each eligible active character, that branch invokes saving helper `CEE4`.

On save failure it builds a lethal-scale damage quantity from the target's current life/protection state and sends that through common character damage.

The exact higher-level creature/attack label is intentionally not generalized here; the preservation point is:

> not every NPC attack is ordinary melee dice followed by status side effects.

Some actor/type-specific attack paths construct damage by their own executable rules.

## 75. Special status attacks and base damage remain independently ordered

For the ordinary NPC attack family, the safe architecture is:

```text
hit check
    |
    v
base melee dice
    |
    +-- level-drain capability
    +-- Strength-drain capability
    +-- paralysis capability
    +-- poison capability
    +-- disease capability
    |
    v
saving/protection/eligibility checks
    |
    v
common character damage
```

Some status side effects can be prevented even though the base physical hit still deals damage.

Conversely, actor-specific branches may apply a nonstandard damage quantity.

## 76. Character protection effects occupy different layers

The combat path demonstrates at least four distinct defensive concepts:

```text
attack avoidance/modification
    Blur
    Invisibility 10' Radius
    Bless encoded defensive modifier
    Slow on attacker
    effective AC

attack interception
    Stoneskin

persistent protection durability
    Armor +0x117

true HP-damage absorption
    Aid +0x1EA
    Vampiric Touch +0x113
```

These should not be collapsed into one generic `damage_reduction` list.

## 77. Stoneskin is not an HP absorption pool in this path

The NPC hit routine consumes Stoneskin before ordinary threshold comparison and returns no hit.

Character:

```text
+0x1F2
```

stores accumulated Stoneskin-associated state in addition to primary effect bit/timer state. [Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

Thus Stoneskin differs structurally and behaviorally from Aid/Vampiric damage absorption.

## 78. Combat feedback is presentation state with persistence geometry

The three recent-damage slots are serialized inside the `0x248` character image:

```text
slot 0  outgoing hand A damage
slot 1  outgoing hand B damage
slot 2  incoming damage

deadline[3]
damage[3]
```

They can therefore survive a SAVE image even though they are short-lived UI feedback.

[Combat-feedback state](https://github.com/Hyperbog/RLOFT/blob/d40af8a3660ca126f1635279fda7c4015a3dff4c/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)

This is another example of UI/presentation state intentionally embedded inside persistent character state.

## 79. Faithful combat reference architecture

A clean native implementation can separate:

```text
AttackRollModel
    player hit equation
    NPC hit equation

EffectiveDefenseModel
    equipment
    Dexterity
    burden
    effects

WeaponDamageModel
    OBJECTS dice
    Strength damage adjustment
    enchantment/category routing
    special item dispatch

NpcDamageGateModel
    category floors
    residual category-11 behavior

CharacterProtectionModel
    Stoneskin interception
    Armor durability
    Aid absorption
    Vampiric absorption

SavingThrowModel
    family A status saves
    family B modifier-bearing damage saves

NpcSecondaryAttackModel
    poison
    paralysis
    disease
    level drain
    Strength drain

ActionTimingModel
    hand-ready deadlines
    speed-factor timing
    Haste/Speed acceleration

CombatFeedbackModel
    outgoing/incoming recent damage slots

ProgressionModel
    XP eligibility
    party/class split
    level thresholds
    derived attack-target update
```

These are proposed native abstractions, not recovered original source type names.

## 80. Required combat regression classes

### 80.1 Player hit automatic branches

Test player roll results around:

```text
modified result == 1
modified result == 19
modified result == 20
```

including positive Bless/Aid/Gauntlet roll bonuses.

Verify the automatic checks use the **modified result**.

### 80.2 NPC hit automatic branches

Test raw natural:

```text
1
20
```

and verify those branches occur before Blur/Invisibility/Bless modifications.

### 80.3 Player threshold assembly

Independently vary:

- base attack target;
- NPC signed AC;
- weapon enchantment;
- Strength/exceptional Strength;
- burden ranks 0..4;
- NPC Slow state.

Require exact threshold arithmetic.

### 80.4 Player roll bonuses

Test all combinations of:

- Bless;
- Aid;
- Stone Giant Gauntlets.

Verify additive result before the automatic `>=20` / `==1` tests.

### 80.5 Effective AC

Construct fixtures independently varying:

- base AC;
- torso armor;
- headgear;
- both hand slots;
- enchantment;
- Dexterity;
- burden;
- Armor;
- Dragon Scales;
- Ring of Protection;
- Bracers of Protection.

Compare against original signed arithmetic/order.

### 80.6 Bracers special path

Test object `0x51` with:

- no category-4 armor;
- category-4 armor;
- intermediate AC better/worse than 5.

### 80.7 Stoneskin ordering

Test NPC natural 20 while Stoneskin is active and verify encoded natural-20 ordering relative to Stoneskin.

Test ordinary nonautomatic rolls and verify Stoneskin consumes/intercepts before later threshold modifiers.

### 80.8 Player ordinary damage

Test:

- weapon small-target dice;
- Strength damage adjustment;
- unarmed `1d2`;
- enchantment/category gate;
- one-damage NPC floor.

### 80.9 Temporary hand items

Exercise all nine `0x7E..0x86` pseudo-items through their dedicated dispatch rather than OBJECTS ordinary dice.

### 80.10 Rod of Smiting

Test charge-state and target-specific special behavior separately from generic weapon resolution.

### 80.11 NPC melee damage

Verify NPC definition dice/sides/bonus are used directly before secondary attack effects.

### 80.12 Saving throw family A

For every class/level fixture, compare:

```text
0x6888 + class*0x5C + level*4
```

threshold lookup and d20 success boundary.

### 80.13 Saving throw family B

Verify the independent `0x6AB0` table family, caller modifier, and direct damage-halving consumer.

### 80.14 Armor durability correction

Apply damage with only Armor active.

Require:

- `+0x117` decremented by full incoming damage;
- Armor cleared on depletion;
- HP damage **not** reduced by the Armor quantity.

### 80.15 Aid absorption

Apply damage around the exact Aid pool boundary:

```text
damage < pool
damage == pool
damage > pool
```

and verify post-Aid remainder.

### 80.16 Vampiric absorption

Repeat equivalent boundary tests after Aid processing.

### 80.17 Combined protection ordering

Construct:

```text
Armor
Aid
Vampiric Touch
```

simultaneously and verify:

```text
Armor durability first
Aid absorption second
Vampiric absorption third
HP fourth
```

### 80.18 Incoming combat feedback

Verify slot 2 stores the **original incoming argument**, not post-absorption HP loss.

### 80.19 Character death

Drive HP below zero and verify:

- clamp to zero;
- set death bit;
- clear disease/poison/paralysis bits;
- run cleanup/UI path.

### 80.20 Party terminal-state test

Construct party status combinations around mask `0xD040`; do not substitute an all-zero-HP test.

### 80.21 NPC damage/death

Verify:

- live HP mutation;
- surviving reaction sound;
- death sound bank;
- zero death-ID base sound;
- definition XP consumption.

### 80.22 XP party share

Test multiple eligible-party counts and verify:

```text
(XP + N/2) / N
```

integer rounding plus minimum 1.

### 80.23 XP multiclass split

For one-, two-, and three-class characters verify the second rounded division and per-class progression increments.

### 80.24 Level threshold/update

Cross class-specific threshold boundaries and verify:

- class level update;
- level-change side effects;
- base attack target recomputation.

### 80.25 Hand timing

For representative weapon speeds verify:

```text
duration = speed * 9
```

and Haste/Potion of Speed half-duration behavior.

### 80.26 Combat feedback timing

On successful player attacks verify:

```text
feedback deadline =
    current time + attack duration/2
```

while hand-ready deadline uses the full duration.

## 81. Correction C-19 — player and NPC automatic hit/miss ordering differs

The two central d20 hit routines are not interchangeable.

Player attack:

```text
1d20 + bonuses
then >=20 auto hit / ==1 auto miss
```

NPC attack:

```text
raw 1d20
then natural 20 auto hit / natural 1 auto miss
then defensive modifiers
```

A shared generic d20 helper that applies the same natural-roll semantics to both sides would alter original behavior.

## 82. Correction C-20 — Armor +0x117 is not a true HP absorption pool

The earlier broad label `Armor absorption pool` was too strong.

Direct disassembly proves:

```text
incoming damage depletes +0x117
depletion clears Armor
but +0x117 does not subtract from remaining damage
```

Aid and Vampiric Touch are the true damage-absorption pools in the common character-damage routine.

The canonical SAVE/character documentation has been corrected accordingly.

## 83. Correction C-21 — saving throws are not one table

CODE.2 uses at least two class/level-indexed saving-table families:

```text
CEE4
    table family 0x6888
    common NPC status/drain save path

CF2C
    table family 0x6AB0
    explicit caller modifier
    direct damage-mitigation consumer
```

The exact historical AD&D category names remain an enrichment target, but the two executable families must remain distinct.

## 84. Correction C-22 — +0x19E..+0x1B5 is combat-feedback state

The former unnamed three paired timer/auxiliary slots are now CODE-PROVEN:

```text
slot 0
    outgoing hand-A damage feedback

slot 1
    outgoing hand-B damage feedback

slot 2
    incoming character-damage feedback
```

Each slot has an absolute gameplay-time display deadline and a damage value.

The canonical SAVE docs and open-question ledger have been updated to remove these from the unresolved set.

## 85. Historical unknowns retained

The combat system is mechanically reconstructable for the supplied build, but these historical/source-level questions remain:

1. original DreamForge source names for saving-table family A and family B;
2. original source labels for burden ranks `0..4`;
3. physical real-time unit represented by attack duration values;
4. original source-level intent of NPC category-11 behavior bits `0x20000/0x40000`, despite exact machine behavior being preserved;
5. original high-level name for the type-15 special NPC attack branch;
6. source-level labels for selected class-level progression tables/helpers;
7. exact tabletop/source terminology for selected signed ability tables where the executable arithmetic is already known.

These are naming/provenance gaps, not blockers to faithful combat execution.

## 86. Source ledger

### Character/combat state

- [DatFiles/SAVE.DAT/CHARACTER_RECORD.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)
- [DatFiles/SAVE.DAT/CORE_FIELDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)
- [DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)
- [DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md](https://github.com/Hyperbog/RLOFT/blob/d40af8a3660ca126f1635279fda7c4015a3dff4c/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)
- [DatFiles/SAVE.DAT/STATUS_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)
- [DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

### OBJECTS weapon/equipment state

- [DatFiles/OBJECTS.DAT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/FORMAT.md)
- [DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md)
- [Component 10](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/10_OBJECT_INVENTORY_EQUIPMENT_AND_CONTAINERS.md)

### NPC combat state

- [DatFiles/NPCS.DAT/FORMAT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/FORMAT.md)
- [DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/BEHAVIOR_FLAGS.md)
- [DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md)
- [DatFiles/NPCS.DAT/STATUS_ATTACKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/NPCS.DAT/STATUS_ATTACKS.md)
- [Component 11](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/11_NPC_CREATURE_AI_AND_LIVE_ACTOR_MODEL.md)

### Executable reconstruction

- [Code2/Code2.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)
- [Code2/CODE2_CONTINUITY_ADDENDUM.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)
- [Code2/FINAL_SEMANTIC_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)
- [Code2/FINAL_CERTIFICATION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

## 87. Architectural checkpoint

The combat architecture is now:

```text
PLAYER ATTACK
    |
    +-- character base attack target
    +-- weapon enchantment
    +-- Strength attack adjustment
    +-- burden rank
    +-- Bless/Aid/Gauntlet roll bonuses
    +-- target NPC AC / Slow state
    |
    v
modified player d20 hit check
    |
    v
weapon/unarmed/touch damage
    |
    +-- Strength damage adjustment
    +-- item special cases
    +-- NPC category/enchantment gate
    |
    v
NPC live HP / death / XP
    |
    +-- reaction/death sound
    +-- party/class XP
    +-- class levels
    +-- recompute base attack target
    |
    v
hand-ready deadline
+ outgoing damage feedback


NPC ATTACK
    |
    +-- raw natural d20
    +-- target effective AC
    |      equipment
    |      Dexterity
    |      burden
    |      Armor/Dragon Scales/ring/bracers
    +-- Stoneskin interception
    +-- Blur/Invisibility/Bless modifiers
    +-- NPC THAC0 / Slow state
    |
    v
NPC hit result
    |
    v
melee dice + secondary status/drain effects
    |
    +-- saving family A
    +-- special protection predicates
    |
    v
COMMON CHARACTER DAMAGE
    |
    +-- record incoming feedback
    +-- deplete Armor durability
    +-- absorb with Aid
    +-- absorb with Vampiric Touch
    +-- subtract HP
    |
    +-- alive
    |
    +-- dead
          death status
          cleanup
          party terminal-state test


OTHER DAMAGE/EFFECT PATHS
    |
    +-- saving family B
    +-- signed half-damage result
```

This closes the central combat execution model to the supplied-build evidence boundary.

The next preservation component should reconstruct the **spell and magic system**: Mage/priest spell matrices, memorization/prayer preparation, spell dispatch IDs, spell/resource lookup, targeting modes, persistent effects and expiries, area/projectile class-3 effects, scroll/device reuse of spell handlers, light/detection/global counters, restoration/cure behavior, and the separation between spell identity, presentation effect type, and serialized character effect state.
