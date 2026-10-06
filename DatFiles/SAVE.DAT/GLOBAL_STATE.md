# SAVE.DAT global/non-character state

Status: **serializer/deserializer-certified structural reconstruction.** This note maps the non-character state surrounding the `0x248` character images and corrects an important earlier assumption: SAVE files are not intrinsically fixed-length. The supplied quick-start fixture is `0x2448` bytes because its live-world-object count is zero.

## Serializer anchor

The CODE.2 save routine writes fields in one strict sequence. Physical disassembly `0x416F8..0x41B79` shows every source address and byte count; the load routine mirrors the sequence.

For the supplied fixture the layout is:

```text
0x0000..0x0031  50-byte save-description field
0x0032..0x0051  32-byte persistent event flag vector
0x0052..0x0055  uint32 current level/map index
0x0056..0x0059  uint32 player/world coordinate A
0x005A..0x005D  uint32 player/world coordinate B
0x005E..0x0061  uint32 player facing/orientation state
0x0062..0x0063  uint16 shared primary-effect counter A
0x0064..0x0065  uint16 shared primary-effect counter B
0x0066..0x0069  uint32 shared primary-effect counter/state C
0x006A..0x006B  uint16 shared primary-effect counter D
0x006C..0x006F  uint32 saved game-clock value
0x0070..0x0073  uint32 global runtime state at DGROUP 0xCF4C
0x0074..0x0075  int16 global selector/state at DGROUP 0x8600
0x0076          uint8 companion selector/state at DGROUP 0x8602
0x0077..0x0996  active party: 4 * 0x248 bytes
0x0997..        live-world-object section and level-local mutable arrays
...             charged-item counters[15]
...             saved roster: 10 * 0x248 bytes
...             trailing 16-byte party-local auxiliary state
```

All offsets after `0x099A` can move when the live-world-object count is nonzero. Offsets shown below for the middle/late sections are therefore fixture offsets unless explicitly expressed as formulas.

## Fixed 0x77-byte prefix

### `0x0000..0x0031`: save description

CODE.2 always writes exactly 50 bytes from its save-description buffer. The supplied fixture begins:

```text
QUICK START PARTY; ELVEN F/M - DWARVEN F/C\0
```

and zero-fills the remainder of the 50-byte field.

### `0x0032..0x0051`: persistent event flags

The serializer writes exactly 32 bytes from DGROUP `0x22200`. Existing LEVELS/CODE.2 audits establish this as the persistent 256-bit event/game-state vector manipulated by the set/clear/toggle/test event operations.

The quick-start fixture contains all zero bits here.

### `0x0052..0x0061`: level, position, orientation

The next four dwords are written directly from:

```text
file +0x52  DGROUP 0xC6C8
file +0x56  DGROUP 0x255D0
file +0x5A  DGROUP 0x255D4
file +0x5E  DGROUP 0xC658
```

`0xC6C8` is repeatedly compared against level/map numbers by world/level logic and is the current level/map index. `0x255D0` and `0x255D4` are the two world/player position coordinates used by spatial code. `0xC658` is the saved player orientation/facing state.

Fixture values are:

```text
level/map = 22
coord A   = 0x00048000
coord B   = 0x00048000
facing    = 270
```

The coordinate dwords are preserved in their native fixed-point/runtime representation; a faithful decoder should not truncate them to the 16-bit subfields used by some consumers.

### `0x0062..0x006B`: shared timed/effect state

The serializer writes four global values:

```text
+0x62  uint16 DGROUP 0xF03C
+0x64  uint16 DGROUP 0xF03E
+0x66  uint32 DGROUP 0xFD08
+0x6A  uint16 DGROUP 0x6DAE
```

The primary-effect expiration audit proves these are shared counters/state touched by effects 18, 25, 17, and 20 respectively. Exact player-facing effect identities remain dependent on closure of the primary-effect catalogue. The quick-start fixture stores zero in all four.

### `0x006C..0x006F`: game clock

Before writing this field, the save routine calls the engine time source at physical `0x6B624` and serializes the returned dword. This is therefore a save-time game-clock snapshot rather than a static global buffer.

Fixture value: `1899` (`0x0000076B`). Character hand/effect deadlines elsewhere in SAVE use the same game-clock domain.

### `0x0070..0x0076`: remaining global selectors

The final seven prefix bytes are copied from:

```text
+0x70  uint32 DGROUP 0xCF4C
+0x74  int16  DGROUP 0x8600
+0x76  uint8  DGROUP 0x8602
```

These values have direct gameplay/UI consumers but their exact archival names are not yet closed. Preserve their raw values. Fixture values are `0x000E0000`, `-1`, and `0`.

The next byte, `0x0077`, is exactly the first byte of active-party character record 0.

## Active-party image

The serializer then writes exactly `0x920` bytes from runtime `0x218E0`:

```text
4 * 0x248 = 0x920
```

This independently certifies both the corrected character base and record stride.

## Dynamic middle section

Immediately after the party image, the serializer writes:

```text
uint32 live_world_object_count;                 // DGROUP 0xC038
WorldObject22 live_world_objects[count];        // runtime 0x248EC, 0x16 bytes each
uint32 party_roster_links_or_indices[2];        // DGROUP 0xD8A8, 8 bytes
uint32 level_local_counter;                     // DGROUP 0xA7E4
for level_slot = 0..61:
    uint16 level_mutable_words[length[level_slot]];
    uint8  level_mutable_byte[level_slot];
uint32 global_state_f920;
uint32 global_state_f91c;
uint8  charged_item_counter[15];
```

### Live world objects

`DGROUP 0xC038` is a live object count with a hard executable limit of `0x96` (150). World-object creation/removal code indexes a `0x16`-byte structure array at runtime `0x248EC` and increments/decrements this count.

Therefore:

```text
middle object payload size = live_world_object_count * 0x16
```

The quick-start fixture has count `0`, so no `0x16`-byte world-object records appear in this particular file. This is the reason the fixture's later offsets are stable at the previously documented positions.

A native SAVE parser must read the count and advance dynamically. It must **not** assume that charged-item counters always begin at absolute `0x0D59` or that the roster always begins at `0x0D68`.

### Two 32-bit party/roster links at DGROUP `0xD8A8`

Eight bytes follow the dynamic object array. Spell/party routines consume these as character/roster indices or links. Exact symbolic naming remains open; preserve both dwords.

### Level-local mutable-state bank

The serializer then writes `DGROUP 0xA7E4`, followed by 62 descriptor-driven mutable arrays. For each descriptor at runtime table `0x7330 + slot*10`, it writes:

```text
2 * descriptor.byte_0 bytes from descriptor.pointer_at_5
1 byte from descriptor +9
```

This is a persistent per-level/per-slot mutable-state bank. The descriptor table fixes the widths; the data themselves are runtime mutable state. In the supplied fixture the complete 62-slot payload occupies 938 bytes (`0x3AA`).

The exact per-slot semantic catalogue remains to be correlated with LEVELS.DAT and level initialization code.

### `0xF920` / `0xF91C`

Two final dwords precede the charged-item vector. Executable consumers clamp `0xF920` to `0..10` and `0xF91C` to `0..8` in one UI/control path. Their exact archival names remain pending.

## Fixture middle offsets

Because `live_world_object_count == 0` in the supplied quick-start file:

```text
0x0997..0x099A  uint32 live_world_object_count = 0
0x099B..0x09A2  DGROUP 0xD8A8[2]
0x09A3..0x09A6  DGROUP 0xA7E4
0x09A7..0x0D50  62-slot mutable-state payload (938 bytes)
0x0D51..0x0D54  DGROUP 0xF920
0x0D55..0x0D58  DGROUP 0xF91C
0x0D59..0x0D67  charged_item_counter[15]
0x0D68..0x2437  saved roster, 10 * 0x248
0x2438..0x2447  trailing 16-byte auxiliary state
```

For a save with `N` live world objects, every fixture offset from `0x099B` onward shifts by `N * 0x16` bytes.

## Saved roster

After the 15 charged-item counters, CODE.2 writes exactly `0x16D0` bytes from runtime `0xA808`:

```text
10 * 0x248 = 0x16D0
```

This is the ten-record saved-character roster. Its absolute start is dynamic because it follows the live-world-object array.

## Trailing 16 bytes

The final serializer operation writes 16 bytes from DGROUP `0x6FA0`. Gameplay code indexes this as four 4-byte party-local entries and directly uses bytes `+0`, `+1`, and `+3` of each entry. The exact symbolic structure is still under audit.

The quick-start fixture stores all `0xFF` here. Preserve all 16 bytes exactly.

## Parsing rule

A faithful parser should derive later positions sequentially from serializer order:

```text
read fixed 0x77-byte prefix
read 4 * 0x248 party records
read live_world_object_count
read count * 0x16 live-world-object bytes
read fixed middle globals
read the 62 descriptor-sized mutable-state blocks
read 15 charge counters
read 10 * 0x248 roster records
read 16 trailing bytes
```

Do not model `0x0D59`, `0x0D68`, or total file size `0x2448` as universal format constants. They are certified offsets for the supplied zero-live-object fixture.