# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 08 — SAVE, Party Lifecycle, Persistent State, and Module Handoff

**Status:** PRIMARY PERSISTENCE/STATE-OWNERSHIP RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter reconstructs persistence as a complete system rather than treating `SAVE.DAT`, standalone `LEVnnn.DAT` files, and `CHARS` as interchangeable save artifacts.

It covers:

- the distinct ownership of global save state, per-level mutable state, and inter-module transfer state;
- `SAVE.DAT` serializer/deserializer geometry;
- active party versus saved roster;
- the fixed `0x248` character image;
- the transient `CHARS` party image;
- current level/position/facing persistence;
- global event flags;
- held/cursor item persistence;
- three distinct saved timing domains;
- transient/moving effects;
- spell-interface selections;
- persistent container contents;
- renderer/environment state;
- charged-item counters;
- trailing party container-UI contexts;
- save/load slot workflow;
- current-level snapshot ordering before a user save;
- native-port compatibility boundaries.

Detailed character rules, equipment mechanics, item effects, spell effects, combat, and NPC AI are referenced only where persistence owns their serialized state. Those systems receive separate preservation chapters.

The supplied `SAVE.DAT` was independently rechecked during this pass:

```text
size     9,288 bytes / 0x2448
SHA-256  f898cb92fca6447954f5e0b2bf6f8ad2435e4a64ae3b9de74f8bf75e0ed10d5d
```

This exactly matches the certified repository fixture. [SAVE README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/README.md) [SAVE final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md)

## 2. Ravenloft has three different serialization contracts

The original game uses three materially different state images:

```text
SAVE.DAT
    global game + active party + roster + cross-level persistent systems

LEVnnn.DAT
    mutable state of one level/map instance

CHARS
    transient 4-character image crossing CODE.1/CODE.2 process boundary
```

They overlap in subject matter but not in ownership.

### 2.1 SAVE.DAT

`SAVE.DAT` owns state that survives across levels and belongs to the running game/session:

- save description;
- 256 persistent event/game-state bits;
- current level index;
- party world position and facing;
- global clocks;
- cursor-held item;
- four active character images;
- live transient class-3 effects;
- spell-interface selections;
- gameplay deadline clock;
- persistent container banks;
- renderer/environment controls;
- charged-device counters;
- ten-character saved roster;
- party-local container UI contexts.

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

### 2.2 LEVnnn.DAT

A standalone level file owns mutable state local to one level:

- cell-word mutation and discovery;
- dynamic barriers/doors;
- live NPC positions/HP/countdowns;
- placed-object records;
- loose/moving item records;
- level event table;
- automap cache;
- map annotations.

It is loaded in preference to the compact `LEVELS.DAT` baseline when present. [World/level preservation chapter](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)

### 2.3 CHARS

`CHARS` contains exactly:

```text
4 * 0x248 = 0x920 bytes
```

and is produced by CODE.1, consumed by CODE.2, and deleted by the launcher on restart/final cleanup. It is a process-boundary transfer artifact, not a user save. [CHARS xref reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

The faithful native architecture must not collapse these three contracts into one generic “save game struct.”

## 3. SAVE.DAT is variable-length

The serializer is CODE.2 physical `0x416F8..0x41B79`. Its load path mirrors the same order. [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The top-level structure is:

```text
fixed 0x77-byte global prefix

4 * 0x248 active-party records

uint32 live_effect_count
live_effect_count * 0x16 transient/moving-effect records

uint32 spell_ui_selected_character[2]
uint32 gameplay_tick_counter

62 variable-width persistent container-state records

uint32 environment_shading_level
uint32 level_light_adjustment

uint8 charged_item_counter[15]

10 * 0x248 saved-roster records

4 * 4-byte party_container_ui_context
```

[SAVE README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/README.md)

Because the transient-effect array is counted, every later file offset moves by:

```text
live_effect_count * 0x16
```

The quick-start fixture has zero live transient effects; therefore its late offsets and total length are **fixture anchors**, not universal constants.

## 4. Supplied quick-start geometry

For the supplied zero-effect fixture:

```text
0x0000..0x0076   0x77   fixed prefix
0x0077..0x0996   0x920  active party
0x0997..0x099A   4      live_effect_count = 0
0x099B..0x09A2   8      spell UI selected characters
0x09A3..0x09A6   4      gameplay tick counter
0x09A7..0x0D50   0x3AA persistent container bank
0x0D51..0x0D58   8      renderer/environment controls
0x0D59..0x0D67   0x0F   charged-device counters
0x0D68..0x2437   0x16D0 saved roster
0x2438..0x2447   0x10   party container UI contexts
end              0x2448
```

[SAVE final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md)

A parser must read sequentially; seeking directly to `0x0D68` for the roster is only valid for a zero-live-effect save.

## 5. Fixed 0x77-byte global prefix

The prefix is:

```text
+0x00..+0x31  50-byte save description

+0x32..+0x51  32-byte persistent event/game-state vector

+0x52..+0x55  uint32 current level/map index
+0x56..+0x59  uint32 player/world coordinate A
+0x5A..+0x5D  uint32 player/world coordinate B
+0x5E..+0x61  uint32 player facing/orientation

+0x62..+0x6B  four shared primary-effect/global state values

+0x6C..+0x6F  interrupt-driven engine tick snapshot

+0x70..+0x73  24-hour 16.16 time-of-day

+0x74..+0x75  int16 cursor-held OBJECTS.DAT ID
+0x76         uint8 cursor-held known/identified state
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The quick-start description is:

```text
QUICK START PARTY; ELVEN F/M - DWARVEN F/C
```

within the fixed 50-byte field.

## 6. Persistent event flags belong to SAVE, not one level

`SAVE +0x32..+0x51` is the exact 32-byte / 256-bit global event-state vector manipulated by the level-event system.

LEVELS opcodes 8, 9, and 10 set, clear, and toggle this same vector. [World/level preservation chapter](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)

Thus:

```text
level event script
    defines a local command

SAVE global flag vector
    retains cross-level result/state
```

A native engine should not bury these 256 bits inside one level object.

## 7. Current level and party transform

SAVE stores:

```text
current level/map index
coordinate A
coordinate B
facing/orientation
```

The two coordinates are the same live world-coordinate family used by first-person movement and transient world effects. They are retained as complete 32-bit values rather than just cell indexes. [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The supplied quick-start values are:

```text
level        22
coord A      0x00048000
coord B      0x00048000
facing       270
```

A load therefore has enough global state to know **which level snapshot to activate and where/how to place the party within it**.

## 8. Three different saved time domains

The executable proves three distinct clocks:

```text
1. DGROUP 0xF58C
   interrupt-driven engine tick counter
   SAVE +0x6C

2. DGROUP 0xA7E4
   pausable gameplay/deadline tick counter
   serialized after transient effects

3. DGROUP 0xCF4C
   24-hour 16.16 world time-of-day
   SAVE +0x70
```

[SAVE prefix globals](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md) [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

They must remain separate.

### 8.1 Interrupt tick snapshot

`F58C` is incremented by an interrupt handler. SAVE uses an explicit getter and load uses a setter under interrupt control.

This is engine/runtime time, not the deadline basis used by character effects.

### 8.2 Pausable gameplay/deadline ticks

`A7E4` is the timebase for:

- primary effect expiries;
- hand/action ready deadlines;
- temporary hand-effect expiries;
- secondary timed slots;
- Strength recovery;
- periodic gameplay maintenance.

Save/load restores it explicitly.

### 8.3 Time-of-day

`CF4C` is a 16.16 fixed-point hour clock:

```text
high word  integer hour
low word   fractional hour
```

The engine normalizes the hour modulo 24 and uses it in environment/shading behavior. Fine-grained advancement modifies the full dword. [SAVE prefix globals](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)

A port that replaces all three with one host timestamp would destroy original deadline and environment behavior.

## 9. Cursor-held item is persistent game state

The prefix stores:

```text
int16 cursor_item_object_id
uint8 cursor_item_known
```

`-1` means no held object.

Inventory transfer code moves the known/identified state with the object identity, exactly as character `item_known_bits` move with character inventory items. [SAVE prefix globals](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)

Load does not merely assign the fields. It calls the normal held-item setter so the cursor graphic and associated runtime UI state are rebuilt.

This is an important reconstruction rule:

> serialized state stores semantic item identity, not the transient rendered cursor sprite.

## 10. Active party is four complete character images

Immediately after the prefix, SAVE transfers:

```text
0x920 = 4 * 0x248
```

bytes from runtime character base `0x218E0`. [Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

Each active-party slot is therefore the complete `0x248` legacy character image, not a compact reference into another character database.

Unused active slots are represented through character status `0x8000`, not by requiring every byte in the record to be zero.

## 11. CHARS uses the same four-record image but has different ownership

The CODE.1/CODE.2 handoff file also carries:

```text
4 * 0x248 = 0x920
```

bytes.

Therefore:

```text
SAVE active-party block
CHARS transfer payload
```

share the same serialized character-image geometry. [CHARS xref](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)

They are still not equivalent artifacts:

```text
SAVE.DAT
    long-lived user save state
    contains party plus many global systems

CHARS
    temporary module handoff
    contains party only
    deleted by supervisor
```

A native port can use one character serializer for both compatibility boundaries while preserving two distinct lifecycle APIs.

## 12. The 0x248 character image is a compatibility boundary

The certified high-level partition is:

```text
+0x000..+0x00F  name
+0x010..+0x011  status word
+0x012          portrait/appearance
+0x013          sex
+0x014          race
+0x015..+0x017  class codes
+0x018..+0x019  age
+0x01A          alignment
+0x01B          base Armor Class
+0x01C..+0x01F  current/max HP
+0x020..+0x031  class levels/progression
+0x032..+0x03F  six abilities + exceptional Strength
+0x040..+0x069  item[21]
+0x06A..+0x109  two 8x10 spell matrices
+0x10A..+0x1F5  combat/UI/effect/timer state
+0x1F6..+0x247  serialized reserved/unreferenced terminal region
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The terminal bytes are part of the legacy record and must be preserved exactly; they are not extension space for the native port.

## 13. Active versus empty versus dead are different states

The status word proves:

```text
0x4000  dead / zero-HP death state
0x8000  empty/unoccupied party slot
```

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

A dead character remains an occupied record. An empty party slot is a different high-bit state.

Load/save/UI logic must not use:

```text
current_hp == 0
```

as a replacement for occupancy.

## 14. Ten-character saved roster uses the same record schema

After the dynamic/global middle blocks, SAVE stores:

```text
10 * 0x248 = 0x16D0 bytes
```

of saved-roster character records. [SAVE README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/README.md)

Thus:

```text
active party slots     4 records
saved roster          10 records
record schema          identical 0x248 image
```

The saved roster is not a list of character IDs. It stores complete character state images.

This explains why recruited/predefined characters can retain mutable character state independently of the four currently active positions.

## 15. Recruited/template provenance remains in the record

Character status bit:

```text
0x0080  recruited/template provenance
```

is set when a predefined/template character image is installed into the active party. [Character status](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)

The bit is persistent and later participates in the finale's original-versus-recruited party predicate. [Runtime transition protocol](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)

Party membership and character provenance are therefore separate concepts.

## 16. Save/load must preserve character raw state, not recompute it

The `0x248` image contains many values that are not safely regenerated from class/race/items:

- current HP;
- effect bits;
- absolute effect deadlines;
- hand ready deadlines;
- temporary hand-effect objects/timers;
- absorption pools;
- reversible stat-modifier quantities;
- item known bits;
- spell matrix counts;
- spell UI bank/scroll fields;
- opaque/reserved bytes.

[Character record](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)

The correct native strategy is:

```text
decode legacy record
    -> typed runtime state + retained raw compatibility fields

save
    -> reconstruct exact legacy image
```

not:

```text
load only high-level identity
    -> recalculate all runtime state
```



## 17. Live transient/moving effects are cross-level SAVE state

Immediately after the active-party block, SAVE writes:

```text
uint32 live_effect_count
live_effect_count * 0x16 live_effect_records
```

Runtime ownership:

```text
count   DGROUP 0xC038
array   runtime 0x248EC
limit   0x96 = 150 records
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The record layout is:

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

This is the same class-3 transient/moving-effect family connected to LEVELS opcode 3 and RES3 source class 3. It is saved globally because active effects can exist at the instant a save is made and must resume after load.

The constructor does not clear every byte/word of a new record. Unknown auxiliary and packed-state bits are therefore preservation-significant.

## 18. Effect type is a renderer/runtime class, not a spell ID

The twelve-value `effect_type` domain is shared by multiple producers.

Certified examples include:

```text
type 0   Fireball / Wand of Fireballs / level-script effect
type 1   Lightning Bolt / level-script effect
type 2   Ice Knife
type 3   Cone of Cold / Wand of Frost
type 4   Flame Arrow
type 6   Magic Missile / Wand of Magic Missiles
type 8   Burning Hands / Agannazar's Scorcher presentation family
type 9   Ice Storm / Otiluke's Freezing Sphere presentation family
type 10  Flame Strike
type 11  Holy Symbol of Ravenkind effect path
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Values 5 and 7 remain valid descriptor slots without a certified direct producer in the audited supplied paths.

A native serializer should preserve the numeric class exactly and should not rename the stored value to a specific spell enum.

## 19. Spell-interface selection is deliberately persistent

The next eight bytes are:

```text
uint32 spell_ui_selected_character[2]
```

at runtime `0xD8A8`.

The spell UI groups characters by:

```text
group = character_index / 2
```

and stores the currently selected active caster:

```text
D8A8[group] = character_index
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

These dwords are therefore persistent UI/gameplay selections, not roster pointers and not character identity storage.

Component 07 documents how the Pray/Memorize interfaces consume them.

## 20. Gameplay deadline counter is restored before timed systems resume

After the two spell selections, SAVE stores:

```text
uint32 gameplay_tick_counter
```

from `A7E4`.

The load path does not merely assign the saved dword. It calls the runtime setter so the associated maintenance baseline is synchronized. [SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

Character records store **absolute deadlines in this same time domain**.

Examples include:

- primary effect expiries;
- hand/action-ready deadlines;
- temporary hand-effect expiries;
- secondary timed slots;
- recurring Strength recovery.

Therefore save/load fidelity requires restoring:

```text
saved absolute deadlines
+
their common A7E4 timebase
```

together.

Converting the character deadlines to host-relative durations during load is possible internally, but the compatibility layer must preserve the original modulo-32-bit comparison behavior.

## 21. Persistent containers are global mutable object state

The next variable-width block consists of exactly 62 serialized container states.

`OBJECTS.DAT +0x31` is a signed `container_state_index` into the runtime descriptor table:

```text
descriptor = 0x7330 + index * 10
```

Descriptor:

```text
+0x00  uint8  slot_count
+0x01  uint8  ui_columns
+0x02  uint8  capacity_pounds
+0x03  uint8  ui_x_offset
+0x04  uint8  container_subtype
+0x05  uint32 contents_pointer
+0x09  uint8  per_slot_state_bits
```

The serializer writes, for each state `0..61`:

```text
int16 object_id[slot_count]
uint8 per_slot_state_bits
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

This is **container-instance state**, not character-local inventory.

A pouch/chest/quiver stored in a character inventory slot can therefore point to persistent contents that live in this SAVE-global container bank.

## 22. Container bank size is descriptor-driven

Across descriptors `0..61`:

```text
total item slots         438
serialized item words    876 bytes
state bytes               62 bytes
total                    938 bytes / 0x3AA
```

The supplied fixture therefore has a `0x3AA` container block.

Container families include:

- pouch;
- sack;
- chest;
- sling pouch;
- ivory scroll case;
- key ring;
- quiver;
- Bag of Holding.

The reader must use the descriptor slot counts in sequence. The bank is not an array of fixed-size container records.

## 23. Shipped state-61 / selector-62 quiver mismatch is preserved

A concrete shipped inconsistency remains:

```text
OBJECTS ID 329 (QUIVER)
    container_state_index = 62

descriptor 61
    initialized 8-slot quiver-shaped descriptor

descriptor 62
    ten zero bytes

SAVE serializer
    writes states 0..61 only

quick-start SAVE state 61
    eight arrow-family object IDs
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The direct executable audit found no `62 -> 61` repair. Generic container code multiplies the OBJECTS selector literally by ten.

The evidence is consistent with a shipped static-data/off-by-one defect, but that diagnosis is not strong enough to rewrite the historical data.

Faithful rule:

```text
preserve selector 62
preserve zero descriptor 62
preserve serialized state 61
do not silently redirect 62 to 61
```

A later remaster compatibility option can intentionally fix it if desired and documented.

## 24. Environment shading and level-light state are saved explicitly

After container state:

```text
uint32 environment_shading_level   // F920, range 0..10
uint32 level_light_adjustment      // F91C, range 0..8
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

`F920` is tied to environment/time-of-day shading selection.

`F91C` participates in palette/light rebuilding and is combined with party light effects.

These values can be recomputed in some circumstances, but the original SAVE format stores them anyway. A faithful loader restores the serialized values through the encoded post-load setter paths before normal rendering continues.

This is another distinction between:

```text
derived in ordinary gameplay
```

and:

```text
non-persistent
```

The original designers explicitly persisted these derived renderer controls.

## 25. Fifteen charged-device counters are cross-character global state

The next serialized block is:

```text
uint8 charged_item_counter[15]
```

at runtime `0x67EC`. [SAVE README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/README.md)

Executable item/effect paths consume this vector for charged devices, including proved wand/detection behavior. [Primary effect catalogue](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)

The counters are not stored inside the `0x248` character inventory records, even when a charged object is carried by a character.

Thus item identity and charged-device mutable quantity can live in separate serialization domains.

Detailed object-to-counter mapping belongs to the item/equipment component; the SAVE boundary is simply the exact 15-byte vector.

## 26. Saved roster follows the global mutable state

The ten-character roster is serialized only after:

- live transient effects;
- spell UI selections;
- gameplay tick counter;
- all 62 containers;
- renderer/light controls;
- charged-device counters.

The order reinforces that the roster is a separate persistent bank, not an extension of the active four-character array.

For the zero-effect fixture:

```text
roster starts at 0x0D68
size             0x16D0
```

but that starting offset shifts with the live-effect count.

## 27. Four party-local container UI contexts terminate SAVE

The final 16 bytes are four four-byte records at runtime `0x6FA0`:

```text
+0  int8 container_subtype_or_minus1
+1  int8 container_state_index_or_minus1
+2  int8 container_ui_phase_or_minus1
+3  int8 container_object_id_or_minus1
```

[SAVE global state](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)

The opening/closing animation moves `ui_phase` through:

```text
0 -> 3  opening
3 -> 0  closing
3       fully open / content display
```

Party-slot rearrangement swaps the complete four-byte context with the party position.

The quick-start save stores:

```text
FF FF FF FF
```

for all four inactive contexts.

UI modal state can therefore be part of persistent game state when the original game chooses to serialize it.

## 28. The working persistence directory is the live save image

A new byte-level audit during this preservation pass closed the outer slot workflow.

The immutable CODE.2 data object contains:

```text
WORK
SAVE??\SAVE.DAT
\
SAVE.DAT
\*.*
..
???????????
```

and the CODE.2 save serializer itself constructs:

```text
WORK\SAVE.DAT
```

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

Therefore `WORK` is the runtime's live persistence set.

Conceptually:

```text
WORK\
    SAVE.DAT
    LEVnnn.DAT
    other working files copied by *.*
```

A numbered user save is a snapshot of this directory.

## 29. Quicksave is directory SAVE00

The quicksave path at logical `077A0`:

1. calls `171FC(current_level)`;
2. writes `WORK\SAVE.DAT` using save-description selector 0;
3. forms directory `SAVE00`;
4. creates/enters it;
5. deletes its existing files using FCB wildcard delete;
6. returns to the parent;
7. wildcard-copies:
   ```text
   WORK\*.* -> SAVE00\
   ```

Description-table entry zero is the shipped string:

```text
QUICKSAVE
```

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

This closes the historical meaning of slot 0.

## 30. Ordinary user slots are SAVE01 through SAVE10

The normal slot UI has ten rows, internally zero-based `0..9`.

Before creating a directory it computes:

```text
slot_number = selected_row + 1
```

and writes normal decimal tens/ones into the template:

```text
SAVE01
SAVE02
...
SAVE09
SAVE10
```

Each slot has a 50-byte description entry.

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

This is distinct from the automap `AUTO???.MAP` digit-order quirk documented in Component 06.

## 31. Ordinary SAVE packages the entire WORK directory

After the user selects/edits a slot:

1. `066E8(slot_number)` writes `WORK\SAVE.DAT`;
2. CODE.2 creates/enters `SAVE##`;
3. existing slot files are deleted;
4. CODE.2 returns to the parent directory;
5. wildcard copier `073A0` performs:
   ```text
   WORK\*.* -> SAVE##\
   ```

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

The user's saved game is therefore **not complete if only SAVE.DAT is retained**. Its per-level mutable snapshots travel with it in the same numbered directory.

## 32. LOAD replaces WORK with the chosen slot snapshot

Quickload uses `SAVE00`.

Normal load maps UI row `0..9` to `SAVE01..SAVE10`.

The load workflow:

1. verifies/enters selected `SAVE##`;
2. returns to parent;
3. enters `WORK`;
4. deletes existing WORK files;
5. returns to parent;
6. wildcard-copies:
   ```text
   SAVE##\*.* -> WORK\
   ```
7. opens `WORK\SAVE.DAT`;
8. restores global/party state;
9. invokes the normal level loader, which now sees the selected slot's restored `WORK\LEVnnn.DAT` snapshots.

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

The original game therefore uses a working-copy architecture:

```text
SAVE## directory
     |
     | copy all files
     v
WORK directory
     |
     +-- SAVE.DAT
     +-- LEV snapshots
     |
     v
runtime
```

## 33. Wildcard copying is an actual whole-file copier

Routine `073A0`:

- appends `\*.*` to the source directory;
- uses DOS find-first/find-next;
- gets each file name from the 0x2C-byte find structure;
- constructs source and destination paths;
- obtains source size;
- allocates enough memory for the complete source;
- reads the file;
- creates destination;
- writes the complete expected size;
- frees the temporary allocation;
- continues enumeration.

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

The final semantic audit records a fidelity quirk: the source short-read result is not independently required to equal the source size before the destination write, while the destination write count is checked against the full expected size. [CODE.2 final semantic audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)

A native port need not reproduce unsafe file-I/O implementation mistakes, but compatibility tests should preserve observable failure/success behavior where practical.

## 34. Slot replacement explicitly deletes previous files

Before copying into a save slot or restoring into WORK, CODE.2 changes into the target directory and calls an FCB delete helper initialized with:

```text
???????????
```

The helper executes:

```text
INT 21h / AH=13h
```

FCB delete.

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

This means slot replacement is a **replace snapshot** operation rather than an overlay-copy that leaves stale old level files behind.

That distinction is essential: a stale `LEVnnn.DAT` from an older slot state could otherwise override `LEVELS.DAT` after load.

## 35. Current level is synchronized before the user save is packaged

Component 07 established that the top-edge SAVE command calls:

```text
171FC(current_level)
```

before entering the broader save UI.

The dedicated quicksave path does the same and then writes `WORK\SAVE.DAT`.

[Input/UI preservation chapter](https://github.com/Hyperbog/RLOFT/blob/5c5520783c781321b210e5cee8f07598fec45279/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md) [Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

Thus the intended transaction order is:

```text
serialize current mutable level
    |
    v
serialize global/party SAVE.DAT
    |
    v
copy complete WORK set into slot directory
```

This prevents the global save from pointing at an older current-level snapshot.

## 36. Load reconstructs semantic runtime state after raw bytes are read

The load tail performs more than bulk reads.

Examples:

- held-item ID/known flag are read to temporaries and passed through the normal cursor-item setter;
- interrupt-tick snapshot is restored through the dedicated setter;
- gameplay tick is restored through the gameplay-time setter;
- active-party class/casting capability state is repaired/rederived from loaded character classes;
- charged-device bytes are copied into their live vector;
- ten roster records are copied from a temporary `0x248` buffer into the roster bank;
- trailing container UI contexts are restored;
- renderer/environment controls are passed through runtime setter paths;
- selected runtime/dirty flags are reinitialized;
- the current-level global is temporarily forced to `-1`;
- normal level loader `181B4(saved_level)` is called;
- the loaded transient-effect count is restored after the level-load path;
- post-level cell/state probe `06C68` runs.

[CODE.2 final semantic audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)

This gives the correct implementation rule:

> deserialization restores semantic state through subsystem reconstruction points; it is not one giant memcpy into a permanent runtime image.

## 37. Level load after SAVE restoration uses restored WORK snapshots

The load tail explicitly calls:

```text
current_level = -1
181B4(saved_level)
```

after the selected save slot has already been copied back to WORK.

Therefore the normal level loader now resolves:

```text
WORK\LEVnnn.DAT
```

for the saved current level when that snapshot exists.

This ties Components 05 and 08 together:

```text
SAVE.DAT current level
    tells engine which level to activate

restored LEVnnn.DAT
    supplies that level's mutable live-state snapshot
```

Neither file alone is the complete restoration state.

## 38. Transient-effect count is restored around level activation

The load routine reads the saved live-effect count and effect records, then later calls the normal level loader.

After that level activation path it writes the saved count back to runtime `C038`.

The exact sequencing matters because level initialization can touch/reset runtime world-family state.

A faithful native implementation can avoid such temporary overwrite internally, but must ensure that the final post-load transient-effect array/count matches the SAVE image rather than a level-load default.

## 39. Save serializer uses strict short-write checks

For each fixed block, the original serializer compares the write result with the exact requested byte count.

On mismatch:

```text
close SAVE handle
return failure (1)
```

On complete success:

```text
close handle
return success (0)
```

[CODE.2 final semantic audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)

A native serializer should preserve the transactional expectation that incomplete output is failure, even if its host I/O implementation differs.

## 40. Save label has one preserved odd special path

When serializer argument is:

```text
-1 / 0xFFFFFFFF
```

the original code copies only five bytes from the `TEMP` source string into the existing 50-byte label buffer, then writes the full 50-byte field.

[CODE.2 final semantic audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)

The exact historical caller purpose/source-level name is not established here.

It is nevertheless machine behavior and should not be normalized into a clean 50-byte overwrite in a byte-faithful compatibility serializer.



## 41. Save/load UI has original failure and cancel asymmetries

The slot workflows are not perfectly symmetric at the implementation level.

The certified semantic audit records:

- short-write failure in `066E8` closes the SAVE handle and returns failure;
- wildcard-copy failure reaches slot-specific message/error paths;
- normal SAVE and LOAD selection do not surface every failure through the same UI routine;
- right-click/cancel exits in the original save/load selection loops intentionally leave selected original runtime handles unclosed.

[CODE.2 final semantic audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)

A native faithful port should preserve:

```text
which action succeeds/fails
which UI path is taken
which data is or is not committed
```

but it should not deliberately leak host file handles merely to reproduce an invisible implementation defect.

This is an example of the distinction between **observable compatibility behavior** and **unsafe period implementation mechanics**.

## 42. MAKSAV.BAT independently corroborates the directory model

The supplied runtime script `MAKSAV.BAT` creates `SAVE01`, copies:

```text
LEV022.DAT
SAVE.DAT
```

into that directory, then deletes the working copies. [Runtime README](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/README.md)

That batch file is quick-start/install packaging rather than the interactive CODE.2 save routine, but it independently confirms the same historical save-slot concept:

```text
SAVE## directory
    contains global SAVE.DAT
    plus standalone level snapshots
```

The CODE.2 workflow remains controlling for actual in-game save/load behavior.

## 43. Persistence ownership graph

The recovered persistent-state architecture is:

```text
                          USER SAVE SLOT
                    SAVE00 or SAVE01..SAVE10
                               |
                               | complete directory copy
                               v
                             WORK
                 +-------------+-------------+
                 |                           |
                 v                           v
             SAVE.DAT                    LEVnnn.DAT
        global/cross-level          per-level mutable snapshot
                 |                           |
        +--------+---------+                 |
        |        |         |                 |
        v        v         v                 |
     party    roster    globals              |
        |        |         |                 |
        |        |         +-- event flags   |
        |        |         +-- clocks        |
        |        |         +-- containers    |
        |        |         +-- charges       |
        |        |         +-- renderer state|
        |        |                           |
        +--------+---------------------------+
                 |
                 v
               runtime
                 |
                 +-- current level activated through normal LEV/LEVELS loader
                 +-- held-item graphics rebuilt
                 +-- timing baselines synchronized
                 +-- transient effects restored
                 +-- UI selections/contexts restored


SEPARATE PROCESS-HANDOFF CONTRACT

CODE.1
   |
   | 0x920 party image
   v
 CHARS
   |
   v
CODE.2
```

This graph is the persistence model a native reimplementation should preserve even if physical host-file packaging changes.

## 44. Suggested faithful native persistence model

A clean native implementation can expose:

```text
LegacyCharacterImage
    exact 0x248 compatibility representation

PartyState
    four active LegacyCharacterImage-derived states

RosterState
    ten saved character states

GlobalGameState
    event bits
    level ID
    world transform
    clocks
    held item
    transient effects
    spell selections
    containers
    light/environment controls
    charged devices
    UI contexts

LevelRuntimeState
    one mutable level

WorkingPersistenceSet
    GlobalGameState serialization
    zero or more LevelRuntimeState snapshots

SaveSlot
    snapshot of WorkingPersistenceSet

ModulePartyTransfer
    CHARS-compatible four-character transfer only
```

Suggested modern type boundaries are implementation guidance, not recovered DreamForge names.

## 45. Preserve semantic setters on load

Where the original loader invokes a subsystem setter after reading raw values, the native implementation should preserve that reconstruction boundary.

Examples:

```text
cursor item
    deserialize ID/known
    -> install through held-item/cursor subsystem

interrupt tick
    deserialize
    -> restore through timer subsystem

gameplay tick
    deserialize
    -> restore + synchronize maintenance baseline

renderer/light controls
    deserialize
    -> rebuild/refresh relevant render state

saved level ID
    deserialize
    -> activate through normal level loader
```

This prevents save loading from bypassing side effects required to make semantic runtime state consistent with the serialized values.

## 46. Required persistence regression classes

### 46.1 Exact fixture parse

For supplied `SAVE.DAT`, verify:

```text
size                   0x2448
live effect count      0
active party bytes     0x920
container bank         0x3AA
roster                  10 * 0x248
trailing contexts      16 bytes
final cursor            exact EOF
```

### 46.2 Variable-length saves

Construct saves with live-effect counts:

```text
0
1
representative middle value
0x96 maximum
```

and verify every following field shifts by exactly:

```text
count * 0x16
```

without changing its own serialized width.

### 46.3 Active-party round trip

For all four `0x248` records, require byte-preserving round trip including:

- empty-slot records;
- dead characters;
- status bits;
- unknown bytes;
- reserved terminal region.

### 46.4 CHARS compatibility

Verify:

```text
SAVE active-party block
    can be represented as same four 0x248 images

CHARS
    is exactly those four images only
```

without accidentally copying SAVE-global state into the transfer file.

### 46.5 Time domains

Save/load independently varied:

```text
F58C interrupt ticks
A7E4 gameplay ticks
CF4C time of day
```

and prove one does not overwrite/derive the other.

### 46.6 Absolute deadlines

Construct character effect/hand deadlines around the saved `A7E4` value and verify expiry ordering after reload, including unsigned/wrapped boundary cases where used by the original routine.

### 46.7 Cursor-held item

Save:

```text
empty
known item
unknown item
```

and verify semantic cursor image/state is rebuilt through the ordinary installation path.

### 46.8 Containers

For every descriptor state `0..61`:

- use its exact slot count;
- preserve `-1` empty sentinel;
- preserve state byte;
- round-trip nested object IDs.

Maintain an explicit state-61/selector-62 fixture and do **not** auto-repair it in faithful mode.

### 46.9 Renderer controls

Vary `F920/F91C`, load, and verify subsequent palette/light reconstruction starts from the serialized values.

### 46.10 Charged-device vector

Round-trip all 15 bytes independently of character inventory placement.

### 46.11 Roster

Round-trip ten complete distinct character records and verify active-party records remain an independent four-record block.

### 46.12 UI contexts

Test all four contexts with:

```text
inactive -1 form
opening phases 0..2
fully open phase 3
closing transition state
```

and preserve signed-byte sentinels.

### 46.13 Quicksave slot

Verify transaction:

```text
current LEV snapshot
WORK\SAVE.DAT
clear SAVE00
WORK\*.* -> SAVE00
```

### 46.14 Normal save slots

For visible rows 0..9, verify directory mapping:

```text
0 -> SAVE01
...
8 -> SAVE09
9 -> SAVE10
```

### 46.15 Slot replacement

Seed a destination slot with a stale `LEVxxx.DAT` not present in WORK.

After save, prove that stale file no longer exists.

This specifically tests the FCB-delete-before-copy semantics.

### 46.16 Load replacement

Seed WORK with a stale level snapshot not present in selected SAVE slot.

After load-copy, prove the stale file has been removed before selected slot files are restored.

### 46.17 Post-load level activation

After selecting a save with a mutable current-level `LEVnnn.DAT`, verify:

```text
SAVE current level ID
+
restored WORK\LEVnnn.DAT
```

produce the saved mutable world, not the compact `LEVELS.DAT` baseline.

### 46.18 Failure injection

Inject:

- SAVE short write;
- source open failure;
- destination creation failure;
- destination short write;
- missing slot directory.

Verify the original success/failure contract and prevent partial state from being treated as a valid save.

## 47. Preservation/repacker rules

A lossless SAVE tool must preserve:

- the complete 50-byte description;
- all 256 global event bits;
- complete 32-bit coordinates/facing;
- all three time domains;
- cursor item plus known flag;
- every byte of all fourteen serialized `0x248` character images;
- variable transient-effect record count and raw fields;
- spell-interface selections;
- all 62 container states;
- the selector-62 mismatch;
- both renderer controls;
- all 15 charge bytes;
- four trailing container UI contexts.

A complete **slot** preservation tool must additionally preserve every file in the selected `SAVE##` directory, not merely `SAVE.DAT`.

## 48. Correction C-09 — SAVE.DAT alone is not a complete Ravenloft save slot

Any documentation, converter, or native architecture that treats:

```text
SAVE.DAT == complete saved game
```

is incomplete.

The executable-proven user-save model is:

```text
SAVE##\
    SAVE.DAT
    LEVnnn.DAT snapshots
    other WORK files included by wildcard copy
```

and load restores the complete directory snapshot into `WORK` before runtime reconstruction. [Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

This is a newly closed architectural correction from the preservation pass.

## 49. Correction C-10 — a loaded save is restored through WORK, not read in-place from SAVE##

The numbered slot is not the runtime's active backing store.

The executable explicitly performs:

```text
selected SAVE## directory
       |
       | wildcard copy after clearing WORK
       v
WORK
       |
       +--> WORK\SAVE.DAT
       +--> WORK\LEVnnn.DAT
       |
       v
runtime loader
```

[Canonical SAVE slot workflow](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

A faithful native port can abstract this physical copy, but its logical ownership should preserve a mutable working state distinct from an immutable selected-slot snapshot.

## 50. Historical unknowns retained

The supplied-build persistence system is structurally closed, but these narrower historical questions remain:

1. original DreamForge source-level names for the four shared prefix state values at SAVE `+0x62..+0x6B`;
2. exact historical purpose/caller meaning of the serializer's special `slot == -1` five-byte `TEMP` label path;
3. historical source names for transient-effect types 5 and 7 and several auxiliary/packed fields;
4. historical source-level explanation for the container state-61 / selector-62 mismatch;
5. exact player-facing/source names for character status bit `0x2000` and low bits `0x0001..0x0020`;
6. semantic identity of serialized character dword `+0x1BA`;
7. historical source names for the three paired post-effect timed slots/auxiliary quantities;
8. whether any shipped variant used more working-file classes inside save slots than the supplied build demonstrates.

These remain explicit `UNKNOWN` or bounded legacy states. They are not license to normalize the serialized bytes.

## 51. Source ledger

### SAVE/DAT reconstruction

- [DatFiles/SAVE.DAT/README.md](https://github.com/Hyperbog/RLOFT/blob/cb4fbbc63b4306d8e9523eb4fef368229332f865/DatFiles/SAVE.DAT/README.md)
- [DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/FINAL_AUDIT_2026-10-06.md)
- [DatFiles/SAVE.DAT/GLOBAL_STATE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/GLOBAL_STATE.md)
- [DatFiles/SAVE.DAT/PREFIX_GLOBALS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PREFIX_GLOBALS.md)
- [DatFiles/SAVE.DAT/CHARACTER_RECORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/CHARACTER_RECORD.md)
- [DatFiles/SAVE.DAT/STATUS_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/STATUS_WORD.md)
- [DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_INVENTORY_STATE.md)
- [DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/POST_EFFECT_TIMERS.md)
- [DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/SAVE.DAT/PRIMARY_EFFECT_CATALOGUE.md)
- [DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md](https://github.com/Hyperbog/RLOFT/blob/5b475da87b26903b570a99d69c697bd61da4b352/DatFiles/SAVE.DAT/SAVE_SLOT_WORKFLOW.md)

### Runtime/module transition evidence

- [Runtime/CHARS_XREF_RECONSTRUCTION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/CHARS_XREF_RECONSTRUCTION.md)
- [Runtime/STATE_TRANSITION_PROTOCOL.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/STATE_TRANSITION_PROTOCOL.md)
- [Runtime/README.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Runtime/README.md)

### Executable reconstruction

- [Code2/Code2.cpp](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/Code2.cpp)
- [Code2/FINAL_SEMANTIC_AUDIT.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_SEMANTIC_AUDIT.md)
- [Code2/FINAL_CERTIFICATION.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md)

### Cross-component ownership

- [Component 01 — executable/module cycle](https://github.com/Hyperbog/RLOFT/blob/5feafdf672b64c5065bced86d4a8b7d579c60679/PreservationArchive/01_EXECUTABLE_CONTAINER_AND_MODULE_CYCLE.md)
- [Component 05 — world/LEV state](https://github.com/Hyperbog/RLOFT/blob/5ad18eff1519ec6b98470095aa18baf5f082c6cc/PreservationArchive/05_WORLD_LEVEL_STATE_AND_EVENTS.md)
- [Component 07 — input/UI/save command](https://github.com/Hyperbog/RLOFT/blob/5c5520783c781321b210e5cee8f07598fec45279/PreservationArchive/07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)

## 52. Architectural checkpoint

The persistence foundation is now:

```text
                         LIVE GAME
                             |
          +------------------+-------------------+
          |                                      |
          v                                      v
     global/party state                     current level state
          |                                      |
          v                                      v
   WORK\SAVE.DAT                         WORK\LEVnnn.DAT
          |                                      |
          +------------------+-------------------+
                             |
                             | wildcard directory snapshot
                             v
               SAVE00 / SAVE01..SAVE10


SAVE.DAT
    |
    +-- description
    +-- 256 global event bits
    +-- current level
    +-- party transform
    +-- interrupt tick
    +-- time of day
    +-- cursor-held item
    +-- active party 4 * 0x248
    +-- class-3 transient effects
    +-- spell UI selections
    +-- gameplay tick
    +-- 62 persistent containers
    +-- environment/light controls
    +-- 15 charged-device counters
    +-- saved roster 10 * 0x248
    +-- four party container UI contexts


LOAD
    SAVE##
       |
       | clear/replace WORK
       v
    WORK
       |
       +-- deserialize SAVE.DAT
       +-- rebuild semantic subsystems
       +-- activate saved current level
               |
               +-- standalone LEV if restored
               +-- compact LEVELS fallback otherwise


CHARS
    remains separate:
    transient 0x920 CODE.1 -> CODE.2 party transfer
```

The next preservation component should document the **party and character gameplay model itself**: race/sex/class identity, class combinations and levels, ability scores, HP/AC, age, alignment, status flags, recruited versus original party membership, spell matrices, primary/timed effects, character progression, and the exact live-state rules that the SAVE serializer preserves.
