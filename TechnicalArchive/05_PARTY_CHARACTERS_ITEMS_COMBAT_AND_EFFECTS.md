# Ravenloft: Strahd's Possession — Party, Characters, Items, Combat, and Effects

Status: **PHASE I / BASELINE 05 — RECONSTRUCTED FROM CERTIFIED CODE/DAT/SAVE EVIDENCE**  
Scope: character record ownership, party/roster state, inventory/equipment, object definitions and mutable item state, combat statistics, attack timing/damage, NPC combat definitions, death/revival/status, spell preparation, timed effects, and persistence boundaries.

## 1. Purpose

This unit integrates the reconstructed `0x248` character image with `OBJECTS.DAT`, `NPCS.DAT`, SAVE timing/effect state, inventory/equipment behavior, and combat/status transitions.

The shipped implementation combines AD&D-facing concepts with compact integer/state mechanisms. The archive preserves both layers without assuming that every player-visible rule maps cleanly to one serialized field.

The central ownership rule is:

```text
static databases              persistent/live actor state
------------------            ---------------------------
OBJECTS.DAT definitions   ->  character inventory slots
NPCS.DAT definitions      ->  live NPC records
                              SAVE character records
                              global item-state banks
                              effect/timer state
```

Definitions are shared and immutable at runtime unless separately documented; actor/item instances carry mutable state elsewhere.

## 2. Canonical character image

Claim: the fundamental serialized party/roster character image is exactly `0x248` bytes.

Class: **CONFIRMED**

Runtime active-party base:

```text
DGROUP 0x218E0
record n = 0x218E0 + n * 0x248
```

SAVE serializes:

```text
4 active-party records = 0x920 bytes
10 saved-roster records = 0x16D0 bytes
```

The same `4 * 0x248 = 0x920` geometry is used by the cross-process `CHARS` handoff documented in baseline 02.

Thus the `0x248` image is simultaneously:

- the live CODE.2 active-party record;
- a SAVE serialization unit;
- a roster serialization unit;
- the unit transferred across the CODE.1/CODE.2 process boundary.

A native implementation should treat this legacy image as a stable compatibility contract even when decoding it into safer typed host structures.

## 3. Character record partition

Class: **CONFIRMED / substantially reconstructed**

Current authoritative physical partition:

```text
+0x000..+0x00F   fixed 16-byte name storage
+0x010..+0x011   uint16 status_word
+0x012           portrait/appearance index
+0x013           sex_code
+0x014           race_code
+0x015..+0x017   int8 class_code[3]
+0x018..+0x019   uint16 age
+0x01A           alignment_code
+0x01B           int8 base_armor_class
+0x01C..+0x01D   int16 current_hp
+0x01E..+0x01F   int16 max_hp
+0x020..+0x025   int16 class_level[3]
+0x026..+0x031   int32 class_progress[3]
+0x032..+0x03F   ability base/current pairs + exceptional Strength
+0x040..+0x069   int16 item[21]
+0x06A..+0x0B9   spell matrix A, 8 x 10
+0x0BA..+0x109   spell matrix B, 8 x 10
+0x10A           class/level-derived base attack target
+0x10B..+0x112   hand_ready_deadline[2]
+0x113..+0x11A   two absorption/protection pools
+0x11B..+0x122   reserved/unreferenced serialized bytes
+0x123..+0x12A   reversible stat/effect auxiliaries
+0x12B..+0x12E   item_known_bits
+0x12F           selected spell bank
+0x130           spell-list scroll position
+0x131           casting capability selector/mask
+0x132..+0x19D   26-bit timed primary-effect subsystem
+0x19E..+0x1B5   three paired timed slots
+0x1B6..+0x1C1   equipment/effect auxiliaries
+0x1C2..+0x1E9   nine temporary-hand effects and expiries
+0x1EA..+0x1F5   late protection/recovery/effect state
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

The final region is part of the record image and is initialized/serialized even though direct semantic consumers have not been found. It is not disposable file padding.

## 4. Identity and class/progression representation

### 4.1 Proven enums

Class: **CONFIRMED / cross-validated**

Common `T` text lookups and the supplied corpus establish:

```text
sex_code
0 Male
1 Female

race_code
0 Human
1 Half-Elf
2 Halfling
3 Gnome
4 Dwarf
5 Elf

class_code
0 Fighter
1 Paladin
2 Ranger
3 Cleric
4 Mage
5 Thief
-1 absent later class slot

alignment_code
0 Lawful Good
1 Lawful Neutral
2 Neutral Good
3 True Neutral
4 Chaotic Good
5 Chaotic Neutral
```

The serialized enum is the Ravenloft implementation boundary. It should not be expanded to generic nine-alignment AD&D assumptions in compatibility code.

### 4.2 Multiclass representation

Each character has three parallel arrays:

```text
class_code[3]
class_level[3]
class_progress[3]
```

Absent later class slots use `-1` sentinels in the supplied data. Advancement code indexes these arrays in parallel and compares class-specific progress values against thresholds.

The original engine therefore represents multiclass identity directly inside the fixed character image, rather than through pointers to separate class objects.

## 5. Ability scores and derived combat state

The record stores base/current pairs for six abilities:

```text
Strength
Dexterity
Constitution
Intelligence
Wisdom
Charisma
```

followed by base/current exceptional Strength percentile.

Class: **CONFIRMED**

This dual representation is important because timed effects and equipment can modify current values while retaining a base value used for reversal/restoration.

Examples already proved:

- Dexterity contributes to defensive calculations and reversible timed effects.
- Intelligence contributes to Mage spellbook capacity.
- Wisdom contributes to the second casting-family preparation logic.
- Strength and exceptional Strength feed combat/equipment mechanics.
- Constitution participates in health/stat-loss behavior.

The architecture therefore treats ability modification as stateful mutation of current fields, sometimes coupled to dedicated saved reversal values later in the record.

## 6. Party-slot status word

The 16-bit `status_word` at `+0x010` combines occupancy, death, provenance, possession, and condition state.

Class: **CONFIRMED for the masks below; remaining bits remain raw**

Proved masks include:

```text
0x0080  source-pool/recruited/template-character provenance marker
0x0100  vengeful-spirit / GHOST possession state
0x0400  disease
0x0800  poison
0x1000  paralysis
0x4000  zero-HP death state
0x8000  empty/unoccupied active-party slot
```

These are not mutually interchangeable "inactive" states.

### 6.1 Empty versus dead

`0x8000` identifies a slot eligible for full 0x248-byte population/replacement. It is an occupancy state.

`0x4000` is set when damage drives current HP to zero or below and is cleared by a proved revival path. It is a reversible death state on an occupied character.

### 6.2 Possession affects rendering and story state

Status `0x0100` selects the second 46-slot RES1 character-portrait bank. Story analysis independently ties the bit to the GHOST/vengeful-spirit possession sequence.

This is an example of a gameplay state bit whose effects cross:

```text
story/interaction
    -> character status
    -> SAVE persistence
    -> portrait resource selection
```

## 7. HP, death, and revival lifecycle

Class: **CONFIRMED**

Damage path behavior:

```text
remaining damage
    -> subtract from int16 current_hp
    -> if current_hp <= 0:
         current_hp = 0
         set status 0x4000
         clear selected incompatible status bits
```

A separate recovery/revival path performs the inverse transition:

```text
current_hp = 1
clear status 0x4000
refresh character/UI state
consume/decrement associated recovery state
```

The architectural model is therefore explicit state transition, not an implicit `hp == 0` test alone. Compatibility code must preserve the status flag and HP mutation ordering.

## 8. Inventory layout

Each character stores 21 signed 16-bit item IDs at `+0x040`.

Class: **CONFIRMED**

Slot map:

```text
0..11   ordinary inventory grid
12      ring_A
13      ring_B
14      hand_A
15      hand_B
16      torso/body garment
17      head
18      dedicated accessory
19      quiver
20      bracer/gauntlet
```

`-1 / 0xFFFF` is the ordinary empty sentinel, but negative values are preserved rather than globally normalized.

A 32-bit `item_known_bits` field at `+0x12B` maps bits `0..20` one-for-one to these item instances and moves with item-instance operations.

## 9. OBJECTS.DAT definitions and inventory instances

`OBJECTS.DAT` contains 336 shared 0x41-byte definitions.

Class: **CONFIRMED / substantially decoded**

Important definition-level fields include:

```text
name[40]
world sprite base/stride
UI graphic index
item category
weight in half-pounds
polymorphic mutable-state index
intrinsic magic marker
armor-class adjustment
packed attack profile
enchantment bonus
two damage profiles
three range bands
```

Inventory slots store the object ID, not a copied 65-byte definition. Runtime code repeatedly resolves the ID back into the shared OBJECTS table.

This keeps instance state compact while allowing many logical variants to share art or combat fields.

## 10. Equipment routing

Class: **CONFIRMED**

Dedicated category routing:

```text
category 3  -> slot 17  headgear
category 4  -> slot 16  body armor
category 5  -> slot 12 or 13  rings
category 6  -> slot 18  dedicated accessory
category 7  -> slot 20  bracers/gauntlets
category 8  -> slot 16  robes/cloaks
```

Other categories enter the generic hand path, subject to item-specific exceptions and admissibility checks.

A hard-coded QUIVER object predicate reroutes qualifying quiver definitions into slot 19.

The engine also retains exact character-class and item-ID restrictions for body armor. These should be reproduced as encoded behavior rather than generalized into cleaner modern rule tables unless intentionally implementing a gameplay change.

## 11. Packed attack profile and action timing

OBJECTS field `+0x35` is a 16-bit packed attack profile:

```text
bits 0..1  hand_pair_class
bits 2..5  weapon_speed_factor
bits 6..15 preserved; zero in supplied records
```

Class: **CONFIRMED**

### 11.1 Weapon speed

The scheduler extracts:

```text
speed = (attack_profile >> 2) & 0x0F
```

and multiplies it by nine when installing attack action delay/deadline state.

Weapon speed is therefore integrated into the saved/live per-hand deadline mechanism rather than being only a display statistic.

### 11.2 Hand-pair compatibility

Low class `2` is proved **paired-hand-exclusive**: a qualifying item prevents simultaneous use of the opposite hand.

The supplied class includes quarterstaff, two-handed sword, halberd, composite bow, and javelin of lightning, so the safe semantic label is broader than `two_handed`.

Classes 0 and 1 remain distinct pairable compatibility classes whose exact historical enum names are not proved.

## 12. Hand action deadlines

Each character stores two 32-bit `hand_ready_deadline` values at `+0x10B`.

Class: **CONFIRMED as persistent action timing state**

These are compared against the pausable gameplay tick counter rather than the interrupt-driven engine tick snapshot.

The architecture therefore combines:

```text
OBJECTS weapon speed
        |
        v
action-delay calculation
        |
        v
character hand_ready_deadline[2]
        |
        v
SAVE persistence / gameplay clock comparisons
```

An ordinary attack's availability is persistent gameplay state, not recomputed statelessly from the weapon every frame.

## 13. Damage profiles and range

OBJECTS stores two independent three-byte damage profiles:

```text
small-target: dice, sides, bonus
large-target: dice, sides, bonus
```

and three range bytes:

```text
short
medium
long
```

Class: **CONFIRMED**

Examples match recognizable AD&D-style weapon notation, but the physical engine unit for range remains intentionally unnamed.

A faithful rules layer should preserve the two target-size damage profiles rather than reducing each weapon to one generic damage expression.

## 14. Armor and defensive state

Characters store a signed `base_armor_class` at `+0x01B`.

OBJECTS definitions contribute:

- signed armor-class adjustment;
- numerical enchantment bonus;
- category/slot restrictions.

Class: **CONFIRMED**

Defense calculation begins from the character field and then applies equipment, Dexterity, and conditional state. This means base AC belongs to the actor record while equipment effects are derived through object definitions at runtime.

The static armor definition is not copied into SAVE; the equipped object ID plus shared OBJECTS table reproduces its contribution.

## 15. NPC combat definition architecture

`NPCS.DAT` contains 83 shared 0x45-byte definitions indexed by LEVELS family-A instances.

Class: **CONFIRMED / substantially decoded**

Combat definition fields include:

```text
hit_dice_count
melee_damage_dice
melee_damage_sides
melee_damage_bonus
signed armor_class
thac0
experience_award
action_delay
behavior/effect flags
initial runtime flags
special attack/classification flags
context-specific sound IDs
```

The live NPC record carries mutable position, action state, HP/runtime flags, and animation state; the static definition remains the source of combat parameters and behavior gates.

Death resolution passes the static XP award into the party XP-distribution path.

## 16. NPC immunity and special-attack flags

NPC definitions contain several independently decoded flag fields.

Class: **CONFIRMED for proved aliases; residual bits preserved**

Examples from the primary 32-bit behavior flags include:

```text
detect-undead exclusion
hold-undead immunity
turn-undead penalty
hold-person immunity
hold-monster immunity
death-spell immunity
common-spell immunity
lightning-bolt immunity
cold immunity
fire immunity
weapon-category damage-floor rules
```

Secondary bytes include proved aliases such as:

```text
poison attack
paralysis attack
level drain attack
Strength drain attack
disease attack
undead classification
obstacle passthrough
dynamic-barrier activation
```

The engine often evaluates compound masks rather than one flag at a time. A native implementation must reproduce mask ordering and combined predicates rather than treating these as a loose list of independent booleans.

Residual populated bits with no proved semantic consumer remain part of the raw definition.

## 17. Condition state and cure paths

Party status masks independently establish several persistent conditions:

```text
0x0400 disease
0x0800 poison
0x1000 paralysis
```

Corresponding cure/neutralize/remove paths clear those states.

NPC special attacks can set matching effects through their definition flags and attack-resolution paths.

This creates a cross-system relation:

```text
NPC definition special-attack flag
       |
       v
combat resolution
       |
       v
party character status/effect state
       |
       v
SAVE persistence / UI / cure operations
```

## 18. Dual spell-preparation matrices

Each character stores two 80-byte matrices:

```text
matrix A  +0x06A
matrix B  +0x0BA
```

Each is indexed as:

```text
cell = matrix[bank * 10 + entry]
selected/planned = cell & 0x0F
ready/current    = cell >> 4
unavailable      = ((cell & 0x0F) == 0x0F)
```

Class: **CONFIRMED**

The two matrices connect to different `T` text ranges and casting families. Current evidence safely identifies matrix A with the scribeable spellbook family and matrix B with the second priest-style casting family, while detailed per-class spell-list limits remain under ongoing reconstruction.

The compact packed-nibble representation is part of the legacy save contract and should be preserved at import/export boundaries.

## 19. Selected spell UI state

The character record also stores:

```text
selected_spell_bank
spell_list_scroll
casting_capabilities
```

SAVE global state separately stores two selected-character indexes for the two spell-interface groups.

Class: **CONFIRMED**

Thus spell UI state is partly character-local and partly global interface state. It is persisted intentionally and should not be assumed disposable UI cache.

## 20. Primary timed-effect subsystem

Each character contains:

```text
uint32 active_effect_bits;       // +0x132
uint32 effect_expiry[26];        // +0x136
```

For effect index `i`, bit `i` corresponds exactly to `effect_expiry[i]`.

Class: **CONFIRMED**

The timer scan compares positive expiries against the **pausable gameplay tick counter**. On expiry it:

1. clears the effect bit;
2. clears the expiry dword;
3. executes effect-index-specific reversal/cleanup;
4. selects expiration message `T[588 + i]`;
5. refreshes relevant UI/state.

The subsystem therefore treats timed effects as first-class persistent character state rather than rebuilding them from transient spell objects.

## 21. Effect-specific reversible state

Several primary effects own saved auxiliary fields so expiration can reverse precisely what was applied.

Certified examples include:

```text
index 6   -> accumulated state at +0x1F2
index 11  -> consumable/protection pool at +0x1EA
index 12  -> reversible Strength adjustment at +0x123
index 13  -> reversible Dexterity adjustment using +0x127 low byte
index 23  -> reversible modifier at +0x1BE
```

Other effect indices modify shared/global state and decrement or clear those external values at expiration.

Class: **CONFIRMED for lifecycle; player-facing names remain incomplete for some indexes**

This architecture explains why a simple `(effect_id, remaining_time)` list would be insufficient for exact compatibility: several effects require additional saved quantitative state.

## 22. Two time domains revisited in combat/effects

Character action and effect deadlines use SAVE/global gameplay counter `A7E4`.

The separate interrupt-driven counter at SAVE prefix `+0x6C` is not the deadline clock.

Class: **CONFIRMED**

This matters directly for:

- effect expiration;
- attack hand readiness;
- temporary touch/spell-hand expiry;
- Strength recovery;
- periodic maintenance/healing;
- rest-time advancement.

A port that drives all of these from an interrupt/frame counter could reproduce incorrect pause/rest/timing behavior even if the numerical durations look similar.

## 23. Temporary hand/spell-effect items

Nine character-local temporary-hand effects use:

```text
uint32 temporary_hand_effect_bits
uint32 temporary_hand_effect_expiry[9]
```

They map to object IDs `0x7E..0x86`:

```text
CHILL TOUCH
VAMPIRIC TOUCH
CLAWS OF THE UMBER HULK
CAUSE LIGHT WOUNDS
CAUSE SERIOUS WOUNDS
CAUSE CRITICAL WOUNDS
HARM
FLAME BLADE
LICH TOUCH
```

Class: **CONFIRMED**

These are represented through OBJECTS-compatible pseudo-items but have special equipment/swap restrictions and dedicated timed state. They demonstrate that OBJECTS IDs can represent transient combat constructs as well as ordinary inventory goods.

A modern engine should retain that behavioral distinction even if it chooses a different internal object hierarchy.

## 24. Absorption/protection pools

Character state includes multiple quantitative protection/absorption pools, including:

- Vampiric Touch absorption state;
- a primary-effect-0 absorption pool;
- a late pool associated with primary effect 11.

Class: **CONFIRMED for storage/lifecycle; full user-facing labels remain partly open**

Damage handling can consume mitigation/absorption before applying remaining damage to HP. These pools must therefore participate in combat ordering, not merely effect display.

## 25. Equipment-driven reversible Strength state

Character `+0x1B6` is tied to OBJECTS ID `0x54`, `STONE GIANT GAUNTLETS`.

Class: **CONFIRMED**

The field preserves displaced/effective Strength information while the equipment forces a specific effective score. This is another example of mutable actor state being required to reverse an equipment effect accurately.

The engine does not rely solely on re-reading the static item definition when the item is removed.

## 26. Mutable item state banks

Static OBJECTS definitions select mutable global state in several ways.

### 26.1 Containers

Category-1 `state_index` selects a runtime container descriptor and mutable contents. SAVE persists 62 descriptor-backed records.

### 26.2 Charged devices

The same physical definition byte selects one of 15 persistent charge counters for the proved wand/rod/device family. Successful use decrements the counter; depletion/removal behavior occurs at zero.

Class: **CONFIRMED**

The architectural pattern is:

```text
OBJECTS definition
    shared immutable parameters
    mutable-state selector
              |
              v
runtime state bank
              |
              v
SAVE persistence
```

A faithful object system must not copy `OBJECTS.DAT` records and mutate their definition bytes to represent per-instance charges or contents.

## 27. Item identity, magic, and presentation

OBJECTS keeps two separate magic-related fields:

```text
intrinsic_magic_marker
numerical enchantment_bonus
```

Magic-reveal presentation considers either source.

Class: **CONFIRMED**

They must not be collapsed into one serialized `is_magic` field. An item may be intrinsically magical without a nonzero numerical combat bonus.

The UI graphic index is also independent from world sprite selection. Multiple logical item definitions may deliberately share UI artwork while retaining different gameplay identities and mutable-state selectors.

## 28. Combat/audio/render cross-links for NPCs

NPC definitions end with four separate 16-bit sound IDs:

```text
ambient/proximity
attack
damage reaction
death
```

Class: **CONFIRMED**

Different contexts use different RES6 sound bases, including a distinct death-sound base. Combat therefore connects definition-level sound IDs to the resource namespace rather than storing absolute audio resources directly in live actor records.

NPC render state likewise derives from definition-level scene bases/animation parameters plus live orientation/action state and INIT's RES3 source-class translation.

## 29. Party wipe and combat-state ownership

The runtime state-transition audit proves that a party with no active character reaches a terminal failure presentation and requests supervisor status 12.

Class: **CONFIRMED / RECONSTRUCTED**

Relevant status distinctions:

```text
0x4000 dead occupied character
0x8000 empty party slot
```

Party wipe is therefore determined from character-record state, not from one global `game_over` Boolean. The same supervisor transition is also used by the successful finale, so combat failure cause remains separate from runtime restart mechanism.

## 30. Character provenance and finale predicate

Characters populated from the saved/source roster path receive status marker `0x0080`. The finale cinematic selector counts only qualifying living party members lacking that marker.

Class: **CONFIRMED for structural provenance; user-facing class naming supported by later runtime audit**

This creates a historically unusual cross-link between character provenance and ending presentation. A port should retain the legacy provenance bit at the compatibility boundary even if it exposes a more explicit typed origin field internally.

## 31. Persistence map

Class: **RECONSTRUCTED summary**

```text
Character identity/stats/inventory/spells/effects
        |
        +--> SAVE active party (4 x 0x248)
        +--> SAVE roster (10 x 0x248)
        `--> CHARS transfer (4 x 0x248)

OBJECTS definitions
        +--> equipped/inventory item IDs in character record
        +--> global persistent container state
        `--> 15-byte charged-device state

NPCS definitions
        `--> per-level/live family-A actor instances
             (persistent level mutation handled by LEVnnn.DAT/world state)

Timed character state
        `--> deadlines against SAVE gameplay_tick_counter
```

Static definitions, actor records, global state banks, and process-transfer images therefore intersect without becoming one storage format.

## 32. Safe native combat/character model

A port may expose typed modern structures, but compatibility should retain these layers conceptually:

```cpp
struct Character {
    LegacyIdentity identity;
    LegacyStatus status;
    Stats base_and_current_stats;
    InventorySlots item_ids;
    SpellMatrices spells;
    ActionDeadlines hands;
    TimedEffects effects;
    TemporaryHandEffects touch_effects;
    EffectAuxiliaryState reversible_state;
};

struct ObjectDefinition {
    RenderFields render;
    ItemCategory category;
    uint8_t state_selector;
    CombatFields combat;
};

struct NpcDefinition {
    RenderFields render;
    CombatStats combat;
    BehaviorMasks behavior;
    SoundIds sounds;
};
```

These are explanatory host-side abstractions, not claimed original source structs.

Compatibility requirements:

1. retain exact 0x248 serialization and legacy enum values;
2. distinguish empty (`0x8000`) from dead (`0x4000`);
3. preserve unknown status/behavior bits;
4. keep base and current abilities distinct;
5. preserve hand/equipment slot numbers and item-ID exceptions;
6. retain OBJECTS state selectors rather than embedding all mutable state in items;
7. reproduce weapon-speed/action-deadline timing against the gameplay clock;
8. preserve two damage profiles and hand-pair classes;
9. implement NPC immunity/special-attack masks with encoded compound behavior;
10. preserve all 26 primary effect slots and their extra reversal state;
11. retain reserved character bytes on legacy round trips.

## 33. Material unknowns retained

The following remain bounded rather than guessed:

- remaining character status-word masks;
- exact player-facing names for every one of the 26 primary effect indexes;
- source-level meanings of several saved effect auxiliary quantities;
- original enum names for hand-pair classes 0 and 1;
- physical unit of weapon range bands;
- some populated NPC behavior bits whose intended semantics remain unresolved;
- exact source-level classification of all transient OBJECTS pseudo-items;
- historical symbolic names for the character provenance/status flags.

These gaps do not prevent preservation of the encoded mechanisms.

## 34. Canonical evidence

Primary repository evidence for this unit:

- `DatFiles/SAVE.DAT/CHARACTER_RECORD.md`
- `DatFiles/SAVE.DAT/CORE_FIELDS.md`
- `DatFiles/SAVE.DAT/EFFECT_FLAGS.md`
- `DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md`
- `DatFiles/SAVE.DAT/GLOBAL_STATE.md`
- `DatFiles/SAVE.DAT/STATUS_POSSESSION.md`
- `DatFiles/OBJECTS.DAT/FORMAT.md`
- `DatFiles/OBJECTS.DAT/EQUIPMENT_SLOTS.md`
- `DatFiles/OBJECTS.DAT/ATTACK_PROFILE.md`
- `DatFiles/OBJECTS.DAT/CONTAINER_STATE.md`
- `DatFiles/NPCS.DAT/FORMAT.md`
- `DatFiles/NPCS.DAT/WEAPON_DAMAGE_FLAGS.md`
- `DatFiles/NPCS.DAT/DETECTION_TURNING_FLAGS.md`
- `DatFiles/NPCS.DAT/RESIDUAL_FLAGS.md`
- `Runtime/CHARACTER_STATUS_4000_DEATH_STATE.md`
- `Runtime/CHARACTER_STATUS_0080_8000_OWNERSHIP.md`
- `Runtime/STATE_TRANSITION_PROTOCOL.md`
- certified CODE.2 reconstruction and final audit records.

## 35. Next evidence unit

`TechnicalArchive/06_TIMING_EVENTS_UI_INPUT_AND_PRESENTATION_ARCHITECTURE.md`

The next Phase-I unit should reconstruct the scheduling/presentation boundary: interrupt timer versus gameplay clock, registered callbacks/events, input queues, mouse/UI dispatchers, modal interaction flow, wordcode timers/choices, renderer refresh/invalidation, and the distinction between simulation-owned timing and presentation-owned polling. It should retain the original address-distinct wrappers, queue ordering, and pause/deadline behavior where those mechanisms are architecturally observable.
