# SAVE.DAT

Status: **high-effort audited active reconstruction.** The serializer/deserializer order is certified; the shared `0x248` character image is substantially reconstructed; the variable-length global layout, persistent container bank, gameplay clock, renderer controls, and trailing container-UI contexts are now behaviorally mapped.

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

The supplied quick-start fixture has `live_world_object_count == 0`, so its late absolute offsets are:

| Fixture offset range | Size | Interpretation |
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

For a save containing `N` live world objects, every fixture offset from `0x099B` onward shifts by `N * 0x16`. Thus `0x0D59`, `0x0D68`, `0x2438`, and total size `0x2448` are fixture anchors, not universal format constants.

See `GLOBAL_STATE.md` for the detailed executable evidence and full container-state catalogue.

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
0x0070..0x0076  remaining global selectors/state
```

The supplied fixture stores level/map `22`, coordinates `0x00048000/0x00048000`, facing `270`, and interrupt-tick value `1899`.

### Two distinct time domains

Earlier notes loosely called `+0x6C` the game clock. The executable distinguishes two saved counters:

- prefix `+0x6C` is DGROUP `0xF58C`, incremented by the engine interrupt handler and restored under `cli/sti`;
- middle-state DGROUP `0xA7E4` is the **pausable gameplay tick counter** used to create and expire character effect/action deadlines and to drive periodic gameplay maintenance.

This distinction is required for faithful timing behavior.

## Character image

The serializer directly transfers `0x920 = 4 × 0x248` bytes beginning at runtime `0x218E0`, proving the corrected character base and stride.

Major certified record regions:

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
+0x10A..+0x1F5   combat, spell UI, timed effects, temporary-hand effects, recovery/protection state
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

Identity enums and detailed field evidence are in `CORE_FIELDS.md` and `CHARACTER_RECORD.md`.

## Dynamic live-world-object section

Immediately after the active party, CODE.2 writes `DGROUP 0xC038` as a 32-bit count, then `count * 0x16` bytes from runtime `0x248EC`.

```text
structure stride = 0x16
maximum count    = 0x96 (150)
```

A parser must follow the count rather than seek directly to the quick-start fixture's late offsets.

## Selected spell-UI characters

The two dwords at DGROUP `0xD8A8` are retained selected-character indexes for two spell-interface groups. Selection code computes `group = character_index / 2`, stores the character index, resets associated spell-list state, and rebuilds that character's spell list. Rendering later reloads these indexes.

Quick-start value: `[0, 0]`.

## Gameplay tick counter

DGROUP `0xA7E4` is the persistent pausable gameplay-time counter. Character effect timers, hand/action deadlines, temporary spell-hand deadlines, recovery deadlines, and periodic healing/maintenance all use this timebase.

Quick-start value: `1735` (`0x06C7`).

## Persistent container-state bank

The former anonymous `0x3AA` bank is the mutable state backing category-1 OBJECTS.DAT containers.

```text
OBJECTS.DAT +0x31
    -> signed container_state_index
    -> runtime descriptor 0x7330 + index*10
    -> int16 contents array + per-slot state bits
    -> SAVE persistent container record
```

Each serialized record contains:

```cpp
int16_t object_id[slot_count]; // -1 = empty
uint8_t per_slot_state_bits;
```

The 62 executable descriptors contain 438 total slots:

```text
438 * 2-byte object IDs = 876 bytes
62 state bytes          =  62 bytes
                          ---------
                          938 bytes = 0x3AA
```

The bank includes pouches, sacks, chests, Bag of Holding, sling pouches, scroll cases, key rings, and quivers. `GLOBAL_STATE.md` lists every state index, associated OBJECTS definition, slot count, and quick-start file offset.

### Shipped state-61/62 mismatch

OBJECTS ID 329 (`QUIVER`) stores selector 62, but descriptor 62 is zero-filled; the SAVE loop persists only indexes `0..61`; descriptor/state 61 is quiver-shaped and the quick-start state contains eight arrows.

A direct executable audit found no `62 -> 61` repair: generic container code indexes descriptor 62 literally. This is consistent with a shipped off-by-one/static-data bug, but faithful compatibility should preserve the original mismatch rather than silently correcting it.

## Renderer/environment controls

Two dwords follow the container bank:

```text
F920  environment_shading_level   clamped 0..10
F91C  level_light_adjustment      clamped 0..8
```

`F920` feeds palette/shading-table generation and, for levels with an active environment profile, is refreshed from a 24-entry time-of-day table using the hour-like high word of `CF4C` modulo 24. Levels using the `0xFF` profile fallback initialize it from a level-indexed table.

`F91C` is combined with a party-wide lighting override generated when primary effect 1 or 10 is active, then passed into the same shading-table generator. Exact original UI/editor terminology remains unknown, so the names are behavioral.

The quick-start fixture stores zero in both.

## Charged-item counters and roster

The 15-byte charged-device vector at DGROUP `0x67EC` follows the renderer controls. The supplied zero-world-object fixture places it at `0x0D59..0x0D67`.

The serializer then writes `0x16D0 = 10 × 0x248` saved-roster bytes from runtime `0xA808`.

## Final 16 bytes: party container-UI contexts

DGROUP `0x6FA0` is four party-local 4-byte contexts:

```text
+0  int8 container_subtype_or_minus1
+1  int8 container_state_index_or_minus1
+2  int8 container_ui_phase_or_minus1
+3  int8 container_object_id_or_minus1
```

Opening/closing routines animate `ui_phase` through `0→3` and `3→0`; phase 3 is the fully open/content-rendering phase. All four bytes move together when party positions are swapped.

The quick-start fixture contains four inactive `FF FF FF FF` contexts.

## Tooling

```bash
python DatFiles/SAVE.DAT/tools/analyze_save.py /path/to/SAVE.DAT
python DatFiles/SAVE.DAT/tools/analyze_character_records.py /path/to/SAVE.DAT
```

The top-level analyzer follows serializer order, parses all 62 container records using the executable slot-count catalogue, and derives late offsets dynamically.

## Native-port compatibility rule

Treat SAVE.DAT as an explicitly decoded little-endian legacy runtime image. Preserve legacy enum numbers, signed sentinels, unknown fields, the state-61/62 mismatch, and dynamic serializer ordering exactly unless an intentional compatibility-fix layer is separately enabled.

## Remaining reconstruction targets

1. Resolve prefix globals `CF4C`, `8600`, and `8602` beyond their current behavioral anchors.
2. Decode/catalogue `T[588..613]` for all 26 primary-effect names, including the lighting effects that feed the renderer override.
3. Recover exact Mage/priest spell lists and per-level entry limits.
4. Audit remaining character `status_word` bits and portrait/status presentation variants.
5. Continue semantic recovery of live-world-object fields and any remaining global runtime state needed for a native SAVE implementation.

The principal SAVE uncertainty is now **semantic naming of a small number of globals and effect/catalogue identities**, not byte ordering or container-bank structure.