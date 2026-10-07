# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 09 — Party and Character Gameplay Model

**Status:** PRIMARY CHARACTER-STATE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents the character/party model as live gameplay state rather than only as a serialization format.

It covers:

- the fixed `0x248` character image as the native compatibility boundary;
- active/empty/dead/recruited/possessed character lifecycle;
- sex, race, class, alignment, portrait, and age;
- multiclass slot representation;
- class levels and progression;
- Armor Class, HP, and attack-target state;
- all six ability scores plus exceptional Strength;
- player-facing and engine-only status bits;
- Mage and priest spell-preparation matrices;
- class-derived casting capability;
- hand/action cooldown deadlines;
- primary effect bits and timers;
- reversible effect auxiliaries;
- post-effect timed slots;
- temporary hand-effect state;
- item-known/identified state;
- character-specific UI state;
- party/roster provenance boundaries;
- reserved/opaque character bytes;
- native-port modeling requirements.

Inventory/equipment behavior, object classes, combat attack resolution, and spell execution receive later dedicated components; they are referenced here only where character state owns their persistent fields.

Primary sources: [character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md), [core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md), [status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md), [post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md).

## 2. The character image is exactly 0x248 bytes

Runtime character `n` begins at:

```text
0x218E0 + n * 0x248
```

CODE.2 save/load transfers:

```text
0x920 = 4 * 0x248
```

bytes for the active party. The saved roster stores ten more records with the same geometry. [Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The exact compatibility image is therefore:

```text
LegacyCharacterImage[0x248]
```

not a compiler-native struct and not a sparse set of high-level attributes.

## 3. Authoritative character partition

```text
+0x000..+0x00F  fixed 16-byte name
+0x010..+0x011  status word
+0x012          portrait index
+0x013          sex
+0x014          race
+0x015..+0x017  class[3]
+0x018..+0x019  age
+0x01A          alignment
+0x01B          base Armor Class
+0x01C..+0x01F  current/max HP
+0x020..+0x025  class level[3]
+0x026..+0x031  class progress[3]
+0x032..+0x03F  abilities and exceptional Strength
+0x040..+0x069  item[21]
+0x06A..+0x0B9  Mage matrix
+0x0BA..+0x109  priest matrix
+0x10A          base attack target
+0x10B..+0x112  hand/action deadlines
+0x113..+0x11A  damage-absorption pools
+0x11B..+0x122  reserved
+0x123..+0x12A  reversible STR/DEX effect auxiliaries
+0x12B..+0x12E  item-known bitset
+0x12F..+0x131  spell UI/casting state
+0x132..+0x19D  primary effect bits + 26 expiries
+0x19E..+0x1B5  three paired timed slots
+0x1B6..+0x1BD  equipment/opaque late auxiliaries
+0x1BE..+0x1C1  reversible Giant Strength modifier
+0x1C2..+0x1E9  temporary hand-effect bits + expiries
+0x1EA..+0x1F5  protection/recovery/accumulated effect state
+0x1F6..+0x247  reserved/unreferenced terminal region
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

## 4. Occupancy is status-driven

The active-party array always has four physical records.

A slot is unoccupied when:

```text
status_word & 0x8000 != 0
```

not when the record is all zero. Unused fixture records retain nonzero and `0xFF` bytes. [Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

Therefore:

```text
empty slot
    != zeroed character

dead character
    != empty slot
```

## 5. Death and occupancy are separate states

Certified status bits include:

```text
0x4000  dead / zero-HP death state
0x8000  empty/unoccupied active-party slot
```

Lethal damage clamps HP to zero and sets `0x4000`. Revival restores HP to 1 and clears `0x4000`. The record remains occupied throughout. [Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

A native party container should therefore model:

```text
slot occupancy
character life/death
```

as independent state dimensions.

## 6. Recruited/template provenance is persistent

Status bit:

```text
0x0080
```

is set when a predefined/template character image is installed into an active slot. It later participates in story/finale predicates. [Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

This is not a transient “currently recruited” flag in the narrow UI sense; it is provenance attached to the character image.

Thus:

```text
original/player-created character
recruited/template-derived character
```

remain distinguishable after save/load and party rearrangement.

## 7. Possession and Vengeful-Spirit coercion are separate bits

Two related states exist:

```text
0x0100  possessed by the Vengeful Spirit
0x0040  Vengeful-Spirit warning/coercion presentation/action-blocking state
```

The status screen maps `0x0100` directly to T text `POSSESSED`.

The `0x0040` bit is set by a separate warning/coercion routine after selecting one of four Vengeful-Spirit messages. It is also consumed by action-selection blocking masks. [Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

Do not merge these into one possession enum.

## 8. Player-facing status conditions

Exact T/resource mapping proves:

```text
0x0100  POSSESSED
0x0200  HELD
0x0400  DISEASED
0x0800  POISONED
0x1000  PARALYZED
```

[Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

Disease, poison, and paralysis have proved producer/curer lifecycles. Held is independently text-mapped. Possession belongs to the Vengeful-Spirit story state.

## 9. Engine-only/residual status bits

`0x2000` is consumed as an action/eligibility blocking tier, but no direct lifecycle producer or player-facing identity has been proved.

Low bits:

```text
0x0001
0x0002
0x0004
0x0008
0x0010
0x0020
```

also remain without a defensible gameplay identity. [Status word](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

These bits are **preserved raw**, not declared padding and not assigned generic AD&D labels.

## 10. Identity enums

### Sex

```text
0 Male
1 Female
```

### Race

```text
0 Human
1 Half-Elf
2 Halfling
3 Gnome
4 Dwarf
5 Elf
```

### Class

```text
0 Fighter
1 Paladin
2 Ranger
3 Cleric
4 Mage
5 Thief
-1 absent class slot
```

### Alignment

Ravenloft exposes six non-evil alignments:

```text
0 Lawful Good
1 Lawful Neutral
2 Neutral Good
3 True Neutral
4 Chaotic Good
5 Chaotic Neutral
```

[Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

These mappings are cross-certified from executable T-index formulas, the supplied fixture, official game documentation, and matching published NPC data.

## 11. Class representation is slot-based, not a bitmask

The record stores:

```text
int8 class_code[3]
int16 class_level[3]
int32 class_progress[3]
```

with the three arrays indexed in parallel.

Absent later class slots use:

```text
class_code     = -1
class_progress = -1
```

in the supplied fixture. [Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

A multiclass character is therefore represented as up to three ordered class slots, each with its own level/progression state.

## 12. Class progression is per slot

Advancement logic:

1. chooses the relevant class slot;
2. adds progression to its `class_progress[i]`;
3. compares against class-indexed progression thresholds;
4. increments the corresponding `class_level[i]`;
5. updates derived state.

[Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

The serialized progression dwords should not be collapsed into one total XP field.

## 13. Age is generated and retained

Age is not decorative source metadata.

CODE.1 generates it from race using:

```text
Human      15 + 1d4
Half-Elf   15 + 1d6
Halfling   20 + 3d4
Gnome      60 + 3d12
Dwarf      40 + 5d6
Elf       100 + 5d6
```

and later displays the stored word. [Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

The native character model should retain the serialized age as a value rather than recomputing it from race.

## 14. Portrait identity is sex-banked

Presentation index is:

```text
portrait_index + sex_code * 23
```

with a second 46-entry range selected under possession status. [Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

Player creation only permits portrait indexes `0..16`; predefined roster characters use additional valid upper indexes. Female indexes 21..22 are invalid/reserved resources.

A native port should preserve the stored portrait index and sex separately rather than flattening them to a single generated resource ID on disk.

## 15. Base Armor Class is not effective Armor Class

Character `+0x01B` is signed:

```text
int8 base_armor_class
```

Initialization writes 10.

Defense computation starts from this byte and then applies equipment, Dexterity, and conditional modifiers. [Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

Thus:

```text
serialized base AC
    != final displayed/effective defense
```

## 16. HP model

```text
int16 current_hp
int16 max_hp
```

Damage subtracts from current HP. Healing clamps to max HP. Advancement may alter maximum HP. Death state is reflected separately in status bit `0x4000`. [Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

This permits states such as:

```text
alive but injured
dead with occupied slot
revived at 1 HP
```

without changing record occupancy.

## 17. Six ability pairs plus exceptional Strength

The memory order is:

```text
+0x032/+0x033  Strength      base/current
+0x034/+0x035  Dexterity     base/current
+0x036/+0x037  Constitution  base/current
+0x038/+0x039  Intelligence  base/current
+0x03A/+0x03B  Wisdom        base/current
+0x03C/+0x03D  Charisma      base/current
+0x03E/+0x03F  exceptional Strength percentile base/current
```

[Core fields](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)

The explicit base/current pairing is essential because temporary effects and equipment can alter current values while preserving original/base values for reversal.

## 18. Ability modifiers are stateful and reversible

Examples include:

- Draw Upon Holy Might Strength stores its applied delta at `+0x123`;
- Draw Upon Holy Might Dexterity stores its applied delta at `+0x127`;
- Potion of Giant Strength stores a reversible quantity at `+0x1BE`;
- Stone Giant Gauntlets use `+0x1B6` to preserve displaced/effective Strength state.

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

A modern implementation should therefore not model temporary attribute changes only as:

```text
current = recompute(base, active_effects)
```

without also preserving the original serialized reversible auxiliary state.

## 19. Combat attack target is stored character state

`+0x10A` is a class/level-derived attack-target byte.

Character setup computes it from class progression; combat code later reads it directly. [Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

A future combat chapter will document exact attack resolution. At the character-state boundary, the key fact is:

```text
base attack target is materialized and serialized
```

rather than always recomputed on demand.

## 20. Two hand/action deadlines

```text
uint32 hand_ready_deadline[2]
```

at `+0x10B` and `+0x10F`.

Item/hand actions write:

```text
current_gameplay_time + duration
```

and hand-use paths block while the corresponding value remains nonzero and unexpired. Maintenance clears matured entries and marks UI state dirty. [Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

These are absolute deadlines in the SAVE-restored gameplay timebase documented in Component 08.

## 21. Mage spell matrix

Record:

```text
+0x06A..+0x0B9
```

is an `8 x 10` Mage matrix.

Each packed byte:

```text
low nibble   selected/planned count
high nibble  ready/current count
low == 0xF   unavailable entry
```

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

The shipped active-entry counts by bank are:

```text
[7, 6, 10, 4, 2, 6, 0, 0]
```

This is both spellbook availability and preparation/current state, depending on nibble.

## 22. Priest/Paladin spell matrix

Record:

```text
+0x0BA..+0x109
```

is the parallel `8 x 10` priest-style matrix shared by Cleric/Paladin capability.

The shipped active-entry counts are:

```text
[7, 6, 5, 4, 4, 2, 0, 0]
```

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

The two matrices are separate serialized state and should remain separate even if a modern spell UI unifies presentation.

## 23. Casting capability is a mask

Character byte `+0x131` is:

```text
0x01  Cleric preparation capability
0x02  Mage spellbook capability
0x04  Paladin priest capability
```

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

Combinations can occur. It is not a single-choice enum.

The field is class-derived but serialized. A native port may derive a typed capability set, but should retain the legacy byte for exact round-trip compatibility.

## 24. Character-local spell UI state is serialized

```text
+0x12F  selected spell bank
+0x130  spell-list scroll position
```

[Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

Together with the SAVE-global selected-caster dwords documented in Component 08, these fields show that the spell interface intentionally preserves user navigation state.

## 25. Item-known state belongs to the character

`+0x12B..+0x12E` is a 32-bit bitset whose low 21 bits correspond one-for-one with character item slots `0..20`.

When an item moves between character slots, its known/identified bit moves with it. [Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

Thus item identity and character-local knowledge are distinct serialized values.

The inventory/equipment chapter will document slot semantics and transfer rules in detail.

## 26. Primary effect representation

Each character stores:

```text
uint32 primary_effect_bits
uint32 primary_effect_expiry[26]
```

at `+0x132..`. Bit `i` corresponds to expiry slot `i`. [Effect flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

Only indexes `0..25` exist in the timer array. Bits 26..31 must not be synthesized as additional timer-backed effects.

## 27. Complete primary effect index

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

[Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

The list includes spell, potion, wand, and equipment-derived states. It is therefore an engine effect index, not a spell-ID array.

## 28. Primary effects do not share one lifetime model

Some effects use ordinary positive expiry deadlines.

Others have additional/special removal behavior:

- Invisibility 10' Radius;
- Negative Plane Protection;
- Fortify;
- Shield of Lightning Protection equipped-state.

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

The native abstraction should therefore be:

```text
legacy effect bit
optional/special deadline
effect-specific auxiliary state
effect-specific removal rule
```

not one universal `TimedEffect(duration)` type.

## 29. Primary effect expiration text is index-stable

For ordinary expiry handling, CODE.2 constructs:

```text
<character name>'s <T[588 + effect_index]> wears off.
```

[T effect text](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/T_EFFECT_TEXT.md)

The raw T labels are compatibility presentation strings; normalized internal names may be more specific.

Example:

```text
effect 1   producer identity Mage Light
effect 10  producer identity Priest Light

both legacy expiration label:
"Light"
```

A faithful UI should preserve the original label path when reproducing legacy presentation.

## 30. Effect cleanup can modify external/global state

Expiration is not just:

```text
clear bit
clear timer
```

Index-specific cleanup includes:

- renderer lighting rebuild for Mage/Priest Light;
- Stoneskin auxiliary-state decrement;
- Aid protection-pool clear;
- reversal of Draw Upon Holy Might stat deltas;
- global True Seeing/Detect Undead/Detect Magic/Enemy Detection counters;
- party-wide Prayer clearing;
- reversal of Potion of Giant Strength.

[Effect flags](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)

Thus character effect state has cross-character and global side effects.

## 31. Armor and Vampiric Touch use consumable pools

Two early post-inventory dwords are not ordinary timers:

```text
+0x113  Vampiric Touch absorption pool
+0x117  Armor absorption pool
```

Incoming damage consumes these quantities before remaining damage reaches HP. Armor depletion also clears effect 0. [Post-inventory state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)

They must be modeled as signed/consumable quantities, not booleans.

## 32. Three additional paired timed slots

After the 26 primary expiry dwords:

```text
uint32 timed_slot_deadline[3]
uint32 timed_slot_aux[3]
```

[Post-effect timers](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)

Each deadline is paired one-to-one with its auxiliary dword.

Maintenance clears matured nonzero deadlines and marks character UI/state dirty. A separate consumer selects a slot and uses its auxiliary while active.

Exact source/gameplay names remain unresolved. These slots must remain distinct from the 26 primary effects.

## 33. Temporary hand-effect family

Late record state contains:

```text
uint32 temporary_hand_effect_bits
uint32 temporary_hand_effect_expiry[9]
```

at `+0x1C2..`.

The nine indexes correspond to temporary hand/touch objects:

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

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

This is a third effect family distinct from:

- primary effect bits;
- three post-effect timed slots.

## 34. Additional late effect state

Certified late fields include:

```text
+0x1EA  Aid-style consumable/additive protection pool
+0x1EE  recurring Strength-recovery deadline
+0x1F2  accumulated Stoneskin state
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

This reinforces a recurring engine pattern:

> one visible effect can require several pieces of serialized state beyond its active bit.

## 35. Character UI and gameplay state are interleaved

The record includes both mechanical and UI-facing fields:

```text
selected spell bank
spell-list scroll
casting capability
item known bits
hand-ready deadlines
effect timers
```

A modern design may separate UI view-model state from character mechanics internally, but exact legacy import/export must reconstruct the original combined image.

## 36. Active party and saved roster are parallel character banks

Component 08 established:

```text
active party   4 * 0x248
saved roster  10 * 0x248
```

using the same record format. [Persistence chapter](https://github.com/Hyperbog/RLOFT/blob/PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md)

The roster does not merely store template IDs. It stores complete mutable character images.

A recruited character can therefore have persistent state even when not occupying one of the four active party positions.

## 37. Character creation and runtime use share the same image

CODE.1 creates/edits the same `0x248` image transferred later through `CHARS`.

CODE.2 then consumes that image directly as its active party representation. [CHARS xref](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

This means the record is not merely a SAVE file artifact. It is the cross-module character ABI.

## 38. Reserved bytes are part of the ABI

Two explicitly bounded reserved areas are especially important:

```text
+0x11B..+0x122   8 bytes
+0x1F6..+0x247  82 bytes
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The terminal region is initialized as part of the full `0x248` image, serialized, and transferred through SAVE/CHARS despite lacking direct character-field references in the current audit.

A faithful port must retain it verbatim.

## 39. Opaque +0x1BA remains serialized state

Record dword:

```text
+0x1BA..+0x1BD
```

is serialized but has no direct CODE.2 field reference in the completed audit. [Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

It must remain:

```text
opaque_preserved_dword
```

rather than being dropped or assigned a guessed AD&D meaning.

## 40. Suggested native character model

A clean faithful implementation can separate typed state from the compatibility image:

```text
LegacyCharacterImage
    exact 0x248 byte-preserving form

CharacterIdentity
    name
    portrait
    sex
    race
    alignment
    provenance

ClassProgression
    up to 3 ordered class slots

AbilityState
    base/current six abilities
    exceptional Strength

VitalState
    base AC
    current/max HP
    death status

CharacterStatus
    named status bits
    preserved unknown bits

SpellPreparationState
    Mage matrix
    priest matrix
    casting capabilities
    UI bank/scroll

CharacterTimingState
    two hand deadlines
    26 primary expiries
    3 auxiliary timed slots
    9 temporary hand expiries
    recovery deadlines

CharacterEffectState
    primary bits
    absorption pools
    reversible deltas
    accumulated quantities

InventoryCompatibilityState
    item IDs
    known bits

OpaqueLegacyState
    reserved and unknown fields
```

These are proposed native types, not recovered original source names.

## 41. Derived state should remain distinguishable from serialized state

Some character values are derivable from other fields:

- casting capability from classes;
- attack target from class/level;
- effective AC from base AC, Dexterity, equipment, and effects;
- selected portrait resource from portrait index, sex, possession state.

The original record nevertheless serializes several derived values directly.

Faithful native design should distinguish:

```text
legacy serialized value
native recomputed value
```

and regression-test that they agree where the original expects them to.

Do not discard serialized derived bytes merely because a modern engine can recalculate them.

## 42. Required character regression classes

### 42.1 Record geometry

For all character banks:

```text
record size = 0x248
field offsets exact
terminal byte = +0x247
```

### 42.2 Occupancy/death

Verify:

```text
0x8000 -> empty
0x4000 -> dead but occupied
revival -> HP 1, clear 0x4000
```

### 42.3 Enum identity

Round-trip all supported:

- sex;
- race;
- class;
- alignment;
- portrait index.

Preserve numeric codes.

### 42.4 Multiclass slots

Test one-, two-, and three-class records with independent levels/progression and `-1` absent slots.

### 42.5 Abilities

Round-trip base/current pairs independently and verify reversible effects alter/recover only the original targeted current values.

### 42.6 HP/AC

Test:

- injured living character;
- death;
- revival;
- base AC distinct from effective AC.

### 42.7 Status bits

Test every named status plus unknown bits.

Unknown bits must survive load/save even when native gameplay ignores them.

### 42.8 Spell matrices

For both 8x10 matrices test:

```text
low selected count
high ready count
0xF unavailable sentinel
```

without normalizing unused banks.

### 42.9 Casting mask

Test combined capability values for multiclass cases.

### 42.10 Hand deadlines

Save/load two independent future deadlines against a restored `A7E4` gameplay timebase.

### 42.11 Primary effects

For all 26 indexes, verify bit/timer pairing and effect-specific cleanup where present.

### 42.12 Non-timer primary effects

Test special removal semantics independently of ordinary positive expiry.

### 42.13 Absorption pools

Test partial depletion, zero clamp, and associated effect removal.

### 42.14 Paired timed slots

Round-trip all three deadlines and auxiliaries without merging them into primary effects.

### 42.15 Temporary hand effects

Round-trip all nine bit/expiry pairs.

### 42.16 Reserved/opaque bytes

Fill reserved regions and `+0x1BA` with nonzero synthetic data, import/export without gameplay interpretation, and require exact byte preservation.

### 42.17 CHARS/SAVE parity

Verify the same four active `0x248` images can pass through:

```text
SAVE active-party block
CHARS transfer image
```

without structural conversion loss.

## 43. Correction C-11 — status is not one enum

The serialized status word is a bitset containing independent and partially orthogonal concepts:

```text
story/provenance
conditions
action-blocking state
death
occupancy
unknown legacy bits
```

Treating it as one mutually exclusive character-state enum is incorrect.

The native model should preserve the bitset at the compatibility boundary even if higher-level helpers expose named predicates.

## 44. Correction C-12 — spell/effect state is not one timer table

The character record contains at least four separate timed/effect families:

```text
2 hand/action deadlines
26 primary effect expiries
3 paired auxiliary timed slots
9 temporary hand-effect expiries
```

plus additional special deadlines/pools.

A generic single `effects[]` list can be used internally only if import/export preserves the original family boundaries and cleanup semantics.

## 45. Historical unknowns retained

Remaining bounded questions include:

1. exact player-facing/source identity of status bit `0x2000`;
2. exact ownership of status low bits `0x0001..0x0020`;
3. exact gameplay/source identity of `+0x1BA`;
4. source-level names and gameplay identity of the three paired timed slots;
5. precise names for selected auxiliary effect quantities;
6. whether any alternate build actively consumed the terminal reserved region;
7. exact original struct/type names used by DreamForge for the `0x248` image.

These do not block faithful character reconstruction.

## 46. Source ledger

### Character core

- [DatFiles/SAVE.DAT/CHARACTER_RECORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)
- [DatFiles/SAVE.DAT/CORE_FIELDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CORE_FIELDS.md)
- [DatFiles/SAVE.DAT/STATUS_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

### Spell/effect state

- [DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)
- [DatFiles/SAVE.DAT/EFFECT_FLAGS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/EFFECT_FLAGS.md)
- [DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)
- [DatFiles/SAVE.DAT/T_EFFECT_TEXT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/T_EFFECT_TEXT.md)
- [DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)

### Persistence/module boundary

- [PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/PreservationArchive/08_SAVE_PARTY_AND_PERSISTENCE_ARCHITECTURE.md)
- [Runtime/CHARS_XREF_RECONSTRUCTION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

## 47. Architectural checkpoint

The character model can now be summarized as:

```text
LegacyCharacterImage 0x248
    |
    +-- identity/provenance
    |     name
    |     sex/race/alignment
    |     portrait
    |     original/recruited
    |
    +-- class progression
    |     up to 3 class slots
    |     levels
    |     progression
    |
    +-- vital/combat state
    |     base AC
    |     current/max HP
    |     attack target
    |     hand deadlines
    |
    +-- abilities
    |     base/current six scores
    |     exceptional Strength
    |     reversible modifiers
    |
    +-- inventory-facing state
    |     21 item IDs
    |     known bits
    |
    +-- spell preparation
    |     Mage 8x10
    |     priest 8x10
    |     casting mask
    |     UI bank/scroll
    |
    +-- status
    |     story/provenance bits
    |     conditions
    |     death
    |     occupancy
    |     preserved unknown bits
    |
    +-- effects
    |     26 primary bits/expiries
    |     3 paired timed slots
    |     9 temporary hand effects
    |     pools/recovery/accumulators
    |
    +-- opaque compatibility state
          reserved ranges
          +0x1BA unknown dword

four active records
    |
    +--> SAVE.DAT active party
    +--> CHARS inter-module transfer

ten roster records
    |
    +--> SAVE.DAT saved roster
```

The next preservation component should document the **object/item/inventory/equipment system**: OBJECTS.DAT type definitions, the 21 character slots, hand-pair rules, weight, containers, equip/unequip side effects, known-state transfer, consumables, charged devices, item categories, object use dispatch, loose-world item transitions, and container persistence.
