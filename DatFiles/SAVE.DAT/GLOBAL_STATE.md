# SAVE.DAT global/non-character state

Status: **serializer/deserializer-certified structural reconstruction.** SAVE files are variable-length: the supplied quick-start fixture is `0x2448` bytes because its live-world-object count is zero. The formerly generic 62-slot mutable bank is now identified as the persistent state backing OBJECTS.DAT category-1 container instances.

## Serializer anchor

CODE.2 save routine physical `0x416F8..0x41B79` writes the state in strict sequence; the load routine mirrors it.

For the supplied fixture:

```text
0x0000..0x0031  50-byte save-description field
0x0032..0x0051  32-byte persistent event flag vector
0x0052..0x0055  uint32 current level/map index
0x0056..0x0059  uint32 player/world coordinate A
0x005A..0x005D  uint32 player/world coordinate B
0x005E..0x0061  uint32 player facing/orientation
0x0062..0x006B  shared primary-effect counters/state
0x006C..0x006F  game-clock snapshot
0x0070..0x0073  global runtime state at DGROUP 0xCF4C
0x0074..0x0075  int16 global selector/state at DGROUP 0x8600
0x0076          uint8 companion selector/state at DGROUP 0x8602
0x0077..0x0996  active party: 4 * 0x248
0x0997..        live-world-object section and persistent container state
...             charged-item counters[15]
...             saved roster: 10 * 0x248
...             trailing 16-byte party-local auxiliary state
```

All offsets after the world-object count can move when that count is nonzero.

## Fixed 0x77-byte prefix

### `0x0000..0x0031`: save description

Exactly 50 bytes. The supplied fixture begins `QUICK START PARTY; ELVEN F/M - DWARVEN F/C\0` and zero-fills the rest.

### `0x0032..0x0051`: persistent event flags

Exactly 32 bytes from DGROUP `0x22200`: the persistent 256-bit event/game-state vector manipulated by LEVELS event set/clear/toggle/test operations. The quick-start fixture contains all zeros.

### `0x0052..0x0061`: level, position, orientation

```text
+0x52  uint32 DGROUP 0xC6C8   current level/map
+0x56  uint32 DGROUP 0x255D0  coordinate A
+0x5A  uint32 DGROUP 0x255D4  coordinate B
+0x5E  uint32 DGROUP 0xC658   facing/orientation
```

Fixture: level 22, coordinates `0x00048000/0x00048000`, facing 270. Preserve the coordinate dwords in their native fixed-point/runtime representation.

### `0x0062..0x006B`: shared timed/effect state

```text
+0x62  uint16 DGROUP 0xF03C
+0x64  uint16 DGROUP 0xF03E
+0x66  uint32 DGROUP 0xFD08
+0x6A  uint16 DGROUP 0x6DAE
```

Primary-effect expiration paths touch these for effects 18, 25, 17 and 20 respectively. Exact player-facing names remain pending effect-catalogue closure. Fixture values are zero.

### `0x006C..0x006F`: game clock

The save routine calls the engine time source immediately before serializing this dword. Fixture value: 1899 (`0x76B`). Character hand/effect deadlines use the same clock domain.

### `0x0070..0x0076`: remaining selectors

```text
+0x70  uint32 DGROUP 0xCF4C
+0x74  int16  DGROUP 0x8600
+0x76  uint8  DGROUP 0x8602
```

Fixture values: `0x000E0000`, `-1`, `0`. Exact names remain open.

## Active-party image

Exactly `0x920` bytes from runtime `0x218E0` (`4 * 0x248`).

## Dynamic middle section

Immediately after the party image:

```text
uint32 live_world_object_count;                  // DGROUP 0xC038
WorldObject22 live_world_objects[count];         // runtime 0x248EC, 0x16 each
uint32 party_roster_links_or_indices[2];         // DGROUP 0xD8A8
uint32 container/global counter;                 // DGROUP 0xA7E4
for container_state = 0..61:
    int16 object_id[descriptor.slot_count];
    uint8 per_slot_state_bits;
uint32 global_state_f920;
uint32 global_state_f91c;
uint8 charged_item_counter[15];
```

### Live world objects

`DGROUP 0xC038` is a live-object count with executable limit `0x96` (150). Creation/removal code indexes a `0x16`-byte structure array at runtime `0x248EC`. The quick-start fixture count is zero.

Thus every later fixture offset shifts by `live_world_object_count * 0x16` in saves containing live world objects.

### DGROUP `0xD8A8`

Eight bytes follow the dynamic object array. Party/spell routines consume them as character/roster indices or links. Preserve both dwords; exact names remain open.

## Persistent container-state bank

The next dword is DGROUP `0xA7E4`, followed by **62 serialized container-state descriptors' mutable payloads**. This is not level-local state.

The identification is instruction-proven by the independent OBJECTS.DAT audit:

- category-1 OBJECTS.DAT byte `+0x31` is a signed `container_state_index`;
- the index selects a 10-byte runtime descriptor at DGROUP `0x7330 + index*10`;
- all 62 shipped category-1 container definitions use distinct mutable-state identities;
- container UI, recursive burden/search, insertion and removal code dereference descriptor `+5` as the live `int16` object-ID contents array;
- descriptor byte `+9` is the per-slot state bitset.

The descriptor itself is runtime metadata:

```text
+0x00  uint8  slot_count
+0x01  uint8  ui_columns
+0x02  uint8  capacity_pounds
+0x03  uint8  ui_x_offset
+0x04  uint8  container_subtype
+0x05  uint32 contents_pointer
+0x09  uint8  per_slot_state_bits
```

The SAVE serializer does **not** write the 10-byte descriptors or their pointers. For each state index `0..61`, it writes only:

```text
2 * descriptor.slot_count bytes from descriptor.contents_pointer
1 byte from descriptor +9
```

Therefore the persistent on-disk logical form is:

```cpp
struct SavedContainerState {
    int16_t object_id[slot_count]; // -1 = empty
    uint8_t per_slot_state_bits;
};
```

`slot_count` is supplied by executable/static runtime metadata, so the 62 records are variable-width but their widths are fixed for this game build. In the quick-start fixture their concatenated payload is exactly `0x3AA` = 938 bytes.

This directly connects three previously separate reconstruction layers:

```text
OBJECTS.DAT category 1 +0x31
        -> container_state_index
        -> runtime descriptor 0x7330 + index*10
        -> live contents pointer / state bits
        -> SAVE persistent container payload
```

Container-state index 20 is BAG OF HOLDING. Specialized descriptor subtypes also cover sling pouches, scroll cases, key rings and quivers; see `DatFiles/OBJECTS.DAT/CONTAINER_STATE.md` for the certified descriptor semantics.

### Important index-62 boundary

The supplied OBJECTS corpus contains one category-1 definition referencing state index 62, while the audited SAVE loop serializes indexes `0..61` (`62` iterations). The certified CODE.2 data image previously showed descriptor 62 zero-filled. This mismatch is preserved as an explicit compatibility edge case rather than silently extending the SAVE bank. A native implementation must reproduce the shipped serializer boundary unless further executable evidence proves a special path for index 62.

### `0xF920` / `0xF91C`

Two dwords follow the container-state bank. Consumers clamp `0xF920` to `0..10` and `0xF91C` to `0..8` in one UI/control path. Exact names remain pending.

## Quick-start middle offsets

With `live_world_object_count == 0`:

```text
0x0997..0x099A  uint32 live_world_object_count = 0
0x099B..0x09A2  DGROUP 0xD8A8[2]
0x09A3..0x09A6  DGROUP 0xA7E4
0x09A7..0x0D50  62-container mutable payload (938 bytes)
0x0D51..0x0D54  DGROUP 0xF920
0x0D55..0x0D58  DGROUP 0xF91C
0x0D59..0x0D67  charged_item_counter[15]
0x0D68..0x2437  saved roster, 10 * 0x248
0x2438..0x2447  trailing 16-byte auxiliary state
```

For `N` live world objects, every fixture offset from `0x099B` onward shifts by `N * 0x16`.

## Saved roster and trailing state

After charges, CODE.2 writes `0x16D0` bytes from runtime `0xA808` (`10 * 0x248`). The final operation writes 16 bytes from DGROUP `0x6FA0`, indexed by gameplay code as four 4-byte party-local entries. The quick-start fixture stores `0xFF` in all 16 trailing bytes.

## Native parsing rule

```text
read fixed 0x77-byte prefix
read 4 * 0x248 party records
read live_world_object_count
read count * 0x16 live-world-object records
read DGROUP 0xD8A8[2]
read DGROUP 0xA7E4
for state_index 0..61:
    obtain slot_count from the certified container descriptor catalogue
    read slot_count signed 16-bit object IDs
    read one per-slot-state byte
read F920/F91C
read 15 charge counters
read 10 * 0x248 roster records
read 16 trailing bytes
```

Do not model `0x0D59`, `0x0D68`, or total size `0x2448` as universal constants. Do not interpret the 938-byte bank as LEVELS.DAT mutable state: it is persistent container-instance contents/state.