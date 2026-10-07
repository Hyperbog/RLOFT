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

