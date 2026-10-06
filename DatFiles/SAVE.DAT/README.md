# SAVE.DAT

Status: **high-effort audited late-stage reconstruction.** Serializer/deserializer order is certified; the shared `0x248` character image is substantially reconstructed; variable-length global layout, three saved timing domains, persistent container state, renderer controls, cursor-held item state, shipped spell catalogues, all 26 primary-effect identities/text labels, named character conditions, and trailing container-UI contexts are behaviorally mapped.

- Supplied fixture size: 9,288 bytes (`0x2448`)
- Supplied fixture SHA-256: `f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d`
- Fixed 50-byte description contains `QUICK START PARTY; ELVEN F/M - DWARVEN F/C`.
- Primary CODE.2 path: save `0000:067FE`, load `0000:06CBA`.

## Serializer-certified top-level structure

SAVE.DAT is **variable-length** because it serializes a counted array of live world objects:

```text
fixed 0x77-byte global prefix
4 * 0x248 active-party character records
uint32 live_world_object_count
count * 0x16 live-world-object records
spell_ui_selected_character[2]
gameplay_tick_counter
62 variable-width persistent container-state records
environment_shading_level
level_light_adjustment
15 charged-item counters
10 * 0x248 saved-roster character records
4 * 4-byte party container-UI contexts
```

The supplied quick-start fixture has `live_world_object_count == 0`, giving:

| Fixture range | Size | Interpretation |
|---|---:|---|
| `0x0000–0x0076` | `0x77` | fixed global prefix |
| `0x0077–0x0996` | `0x920` | active party: `4 × 0x248` |
| `0x0997–0x099A` | 4 | live-world-object count = 0 |
| `0x099B–0x09A2` | 8 | selected spell-UI character indexes |
| `0x09A3–0x09A6` | 4 | gameplay tick counter |
| `0x09A7–0x0D50` | `0x3AA` | 62 persistent container-state records |
| `0x0D51–0x0D58` | 8 | renderer/environment controls |
| `0x0D59–0x0D67` | `0x0F` | charged-item counters |
| `0x0D68–0x2437` | `0x16D0` | saved roster: `10 × 0x248` |
| `0x2438–0x2447` | `0x10` | `party_container_ui_context[4]` |

For `N` live world objects, every fixture offset from `0x099B` onward shifts by `N * 0x16`. Thus `0x0D59`, `0x0D68`, `0x2438`, and total size `0x2448` are fixture anchors, not universal format constants.

See `GLOBAL_STATE.md` for the complete global/container reconstruction and `PREFIX_GLOBALS.md` for fixed-prefix evidence.

## Fixed prefix

```text
0x0000..0x0031  50-byte save description
0x0032..0x0051  256 persistent event flags
0x0052..0x0055  current level/map index
0x0056..0x0059  player/world coordinate A
0x005A..0x005D  player/world coordinate B
0x005E..0x0061  player facing/orientation
0x0062..0x006B  four shared primary-effect counters/state values
0x006C..0x006F  interrupt-driven engine tick-counter snapshot
0x0070..0x0073  24-hour 16.16 fixed-point time-of-day clock
0x0074..0x0075  signed cursor-held OBJECTS.DAT object ID; -1 empty
0x0076          cursor-held item known/identified flag
```

Quick-start: level 22, coordinates `0x00048000/0x00048000`, facing 270, interrupt ticks 1899, time-of-day `0x000E0000` = 14:00, cursor item `-1`, cursor-known flag `0`.

### Three distinct time domains

The executable preserves three separate concepts:

- `+0x6C` / DGROUP `0xF58C`: interrupt-driven engine tick counter;
- middle DGROUP `0xA7E4`: pausable gameplay tick counter used by action/effect deadlines and periodic maintenance;
- `+0x70` / DGROUP `0xCF4C`: 24-hour 16.16 fixed-point world time-of-day.

Do not collapse these into one native clock.

### Cursor-held item

```cpp
int16_t cursor_item_object_id; // OBJECTS.DAT ID; -1 empty
uint8_t cursor_item_known;     // zero unknown; nonzero known/identified
```

The loader restores the pair through the normal held-item setter so the legacy cursor graphic is reconstructed. Inventory-transfer code moves the known flag alongside the item exactly as character `item_known_bits` follow inventory slots.

## Character image

The serializer directly transfers `0x920 = 4 × 0x248` bytes beginning at runtime `0x218E0`, certifying the corrected character base and stride.

Major certified regions:

```text
+0x000..+0x00F   name
+0x010..+0x011   status word
+0x012           portrait/appearance index
+0x013           sex code
+0x014           race code
+0x015..+0x017   class_code[3]
+0x018..+0x019   age
+0x01A           alignment code
+0x01B           base Armor Class
+0x01C..+0x01F   current/max HP
+0x020..+0x031   class levels + progression
+0x032..+0x03F   six AD&D ability base/current pairs + exceptional Strength
+0x040..+0x069   item[21]
+0x06A..+0x0B9   Mage spellbook matrix
+0x0BA..+0x109   Cleric/Paladin priest-style spell matrix
+0x10A..+0x1F5   combat, spell UI, primary/secondary timed effects and recovery state
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

See `CORE_FIELDS.md`, `CHARACTER_RECORD.md`, `STATUS_WORD.md`, and the focused post-inventory/effect notes.

## Character status word

Raw `T` plus the executable status-display loop closes the five player-facing condition bits:

```text
0x0100  POSSESSED
0x0200  HELD
0x0400  DISEASED
0x0800  POISONED
0x1000  PARALYZED
```

Other certified states include:

```text
0x0080  recruited/template-character provenance
0x4000  dead / zero-HP death state
0x8000  empty/unoccupied active-party slot
```

`0x0040` is a separate Vengeful-Spirit warning/coercion presentation/action-blocking state: the possession maintenance routine displays one of raw `T[351..354]` and then sets this bit. `0x2000` remains intentionally unnamed pending producer evidence. See `STATUS_WORD.md` and `STATUS_POSSESSION.md`.

## Shipped spell catalogues closed

Both serialized preparation families remain fixed `8 × 10` cell images, but CODE.2 count tables and raw `T` establish the active shipped entries:

```text
Mage counts by level:   [7, 6, 10, 4, 2, 6, 0, 0] -> 35 prepared entries
priest counts by level: [7, 6,  5, 4, 4, 2, 0, 0] -> 28 prepared entries
```

The exact Mage and Cleric/Paladin spell labels, T indexes, and dispatch IDs are catalogued in `SPELL_CATALOGUE.md`.

Two priest-family resource records, `LAY HANDS` and `TURN UNDEAD`, occupy T slots after the seven level-1 priest spell entries but lie beyond the executable preparation count; they share the priest action namespace without being matrix-B memorization cells.

A shipped compatibility quirk is also documented: priest level-loss normalizer `0x3EB28` operates on matrix B but uses the Mage-shaped entry-count table `0x6440` rather than normal priest table `0x6420`.

## Primary effect catalogue and original text closed

Character `+0x132..+0x19D` is a 26-bit primary-effect set plus 26 corresponding expiry dwords. Application producers identify every slot:

```text
 0 Armor                             13 Draw Upon Holy Might - DEX
 1 Mage Light                        14 Negative Plane Protection
 2 Shield                            15 Fortify
 3 Blur                              16 Slow Poison
 4 Haste                             17 True Seeing
 5 Invisibility 10' Radius           18 Detect Undead
 6 Stoneskin                         19 Potion of Fire Resistance
 7 Dragon Scales                     20 Detect Magic
 8 Bless                             21 Prayer
 9 Invisibility to Undead            22 Shield of Lightning Protection equipped-state
10 Priest Light                      23 Potion of Giant Strength
11 Aid                               24 Potion of Speed
12 Draw Upon Holy Might - STR        25 Wand of Enemy Detection
```

The supplied raw `T` resource independently closes `T[588..613]`, and CODE.2 constructs ordinary expiry presentation as:

```text
"%s's " + T[588 + effect_index] + " wears off."
```

The raw text range contains generic display labels—both Mage and priest Light are simply `Light`, for example—so producer tracing remains necessary for exact effect ownership. See `PRIMARY_EFFECT_CATALOGUE.md`, `EFFECT_FLAGS.md`, and `T_EFFECT_TEXT.md`.

## Dynamic live transient/moving-effect section

Immediately after the active party, CODE.2 writes `DGROUP 0xC038` as a 32-bit count followed by `count * 0x16` bytes from runtime `0x248EC`.

Cross-file executable evidence now identifies these records as the RES3 source-class-3 **transient/moving-effect family**. LEVELS event opcode 3 and spell/item paths create them; the class-3 scene emitter consumes the same array.

```text
+0x00  int32  coordinate B, 16.16-style fixed point
+0x04  int32  coordinate A, 16.16-style fixed point
+0x08  int32  heading/orientation angle
+0x0C  uint16 cached map-cell index
+0x0E  uint16 transient-effect type
+0x10  uint16 auxiliary word; constructor initializes zero
+0x12  uint16 type-specific auxiliary; type 8 uses lifetime countdown
+0x14  uint16 packed movement/direction/frame/source state

structure stride = 0x16
maximum count    = 0x96 (150)
```

Packed state certifies direction bits 3..5, animation/variant bits 6..8, and source-actor/context bits 11..13; movement bits 1..2 are behaviorally consumed but their exact enum labels remain pending. The constructor only partially initializes some auxiliary/packed fields, so a faithful serializer must preserve raw values rather than normalize them.

A parser must follow the count rather than seek directly to the quick-start fixture's late offsets. See `GLOBAL_STATE.md` for the constructor, movement, removal, INIT descriptor, and RES3 renderer evidence.

## Selected spell UI and gameplay ticks

`DGROUP 0xD8A8[2]` retains selected-character indexes for the two spell-interface groups. Quick-start value: `[0,0]`.

`DGROUP 0xA7E4` is the persistent pausable gameplay tick counter used by primary effects, hand/action readiness, temporary spell-hand state, Strength recovery, timed slots, and periodic healing/maintenance. Quick-start value: `1735` (`0x06C7`).

## Persistent container-state bank

The `0x3AA` quick-start bank is mutable state for category-1 OBJECTS.DAT containers:

```text
OBJECTS.DAT +0x31
    -> signed container_state_index
    -> runtime descriptor 0x7330 + index*10
    -> int16 contents array + per-slot state bits
    -> SAVE persistent container record
```

Each serialized record contains:

```cpp
int16_t object_id[slot_count]; // -1 empty
uint8_t per_slot_state_bits;
```

The 62 executable descriptors contain 438 total slots, closing the bank exactly as `438*2 + 62 = 938 = 0x3AA` bytes. `GLOBAL_STATE.md` lists every state index, OBJECTS identity, width, and quick-start offset.

### Shipped state-61/62 mismatch

OBJECTS ID 329 (`QUIVER`) stores selector 62, while descriptor 62 is zero-filled. SAVE persists only descriptors 0..61; descriptor/state 61 is quiver-shaped and the quick-start payload contains eight arrows. Generic container code does not repair `62 -> 61`. Faithful compatibility therefore preserves the shipped mismatch unless an explicit optional fix is enabled.

## Renderer/environment controls

Two dwords follow the container bank:

```text
F920  environment_shading_level   clamped 0..10
F91C  level_light_adjustment      clamped 0..8
```

`F920` participates in palette/shading generation and can follow the integer hour of `CF4C`. `F91C` is combined with a party-wide light override. Primary effects **1 Mage Light** and **10 Priest Light** are the two effect bits that trigger that renderer-lighting recomputation.

Quick-start stores zero in both controls.

## Charged devices, roster and trailing UI state

The 15-byte charged-device vector at DGROUP `0x67EC` follows renderer controls. The serializer then writes `0x16D0 = 10 × 0x248` bytes from the saved-character roster.

The final 16 bytes are four party-local container UI contexts:

```text
+0  int8 container_subtype_or_minus1
+1  int8 container_state_index_or_minus1
+2  int8 container_ui_phase_or_minus1
+3  int8 container_object_id_or_minus1
```

Opening/closing transitions animate phase `0→3` and `3→0`; phase 3 is fully open. Quick-start stores four inactive `FF FF FF FF` contexts.

## Tooling

```bash
python DatFiles/SAVE.DAT/tools/analyze_save.py /path/to/SAVE.DAT
python DatFiles/SAVE.DAT/tools/analyze_character_records.py /path/to/SAVE.DAT
python DatFiles/SAVE.DAT/tools/verify_spell_text_catalogues.py /path/to/T /path/to/CODE.2
```

The character analyzer decodes certified status bits and active primary-effect identities. The spell/text verifier pins both source-file hashes, all 26 effect labels, both spell catalogues, priest action-only labels, and both executable entry-count tables.

## Native-port compatibility rule

Treat SAVE.DAT as an explicitly decoded little-endian legacy runtime image. Preserve legacy enum/index numbers, signed sentinels, fixed-point time precision, status bits, both fixed-size spell matrices, all 26 primary-effect bits/timers, unknown/reserved bytes, dynamic serializer ordering, and the container state-61/62 mismatch exactly unless an explicit compatibility-fix layer says otherwise.

## Remaining reconstruction targets

1. Complete the exact player-facing portrait-resource catalogue/subset behind `+0x012` and any remaining status-presentation details.
2. Identify a stronger runtime consumer for serialized `age` if one exists.
3. Finish the residual packed-state/auxiliary semantics of the now-identified `0x16` transient/moving-effect record where producer evidence permits.
4. Run a final SAVE-wide consistency/stale-offset audit before declaring the format archival-complete.

Status bit `0x2000` and low bits `0x0001..0x0020` have reached a bounded evidence result: the shipped runtime recognizes/preserves them, but the direct producer audit does not justify player-facing names. They remain compatibility-preserved raw state rather than open invitations to infer AD&D conditions.

The remaining SAVE work is now predominantly **live-object semantics and final residual/audit closure**, not basic serializer geometry, spell cataloguing, or primary-effect discovery.