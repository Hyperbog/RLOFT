# SAVE.DAT global/non-character state

Status: **serializer/deserializer-certified structural reconstruction.** SAVE files are variable-length: the supplied quick-start fixture is `0x2448` bytes because its live-world-object count is zero. The formerly generic 62-slot mutable bank is the persistent state backing OBJECTS.DAT category-1 container instances, and its individual record widths are now recovered from the executable descriptor table.

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

`0x0000..0x0031` is the exact 50-byte save description. `0x0032..0x0051` is the 32-byte persistent 256-bit event/game-state vector manipulated by LEVELS event set/clear/toggle/test operations.

The remaining prefix is:

```text
+0x52  uint32 DGROUP 0xC6C8   current level/map
+0x56  uint32 DGROUP 0x255D0  coordinate A
+0x5A  uint32 DGROUP 0x255D4  coordinate B
+0x5E  uint32 DGROUP 0xC658   facing/orientation
+0x62  uint16 DGROUP 0xF03C   shared primary-effect state
+0x64  uint16 DGROUP 0xF03E   shared primary-effect state
+0x66  uint32 DGROUP 0xFD08   shared primary-effect state
+0x6A  uint16 DGROUP 0x6DAE   shared primary-effect state
+0x6C  uint32 game-clock snapshot
+0x70  uint32 DGROUP 0xCF4C
+0x74  int16  DGROUP 0x8600
+0x76  uint8  DGROUP 0x8602
```

Quick-start values: level 22, coordinates `0x00048000/0x00048000`, facing 270, clock 1899 (`0x76B`), `CF4C=0x000E0000`, `8600=-1`, `8602=0`. Exact names for the final selectors remain open.

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

`DGROUP 0xC038` is a live-object count with executable limit `0x96` (150). Creation/removal code indexes a `0x16`-byte structure array at runtime `0x248EC`. The quick-start fixture count is zero, so every later fixture offset shifts by `live_world_object_count * 0x16` in saves containing live world objects.

Eight bytes at DGROUP `0xD8A8` follow the dynamic array. Party/spell routines consume them as character/roster indices or links. Preserve both dwords; exact names remain open.

## Persistent container-state bank

The next dword is DGROUP `0xA7E4`, followed by 62 container mutable-state records. Category-1 `OBJECTS.DAT +0x31` is a signed `container_state_index` selecting a 10-byte runtime descriptor at DGROUP `0x7330 + index*10`.

Descriptor layout:

```text
+0x00  uint8  slot_count
+0x01  uint8  ui_columns
+0x02  uint8  capacity_pounds
+0x03  uint8  ui_x_offset
+0x04  uint8  container_subtype
+0x05  uint32 contents_pointer
+0x09  uint8  per_slot_state_bits
```

The SAVE serializer writes only `2*slot_count` bytes from `contents_pointer`, then byte `+9`. The on-disk logical form is therefore:

```cpp
struct SavedContainerState {
    int16_t object_id[slot_count]; // -1 = empty
    uint8_t per_slot_state_bits;
};
```

### Executable descriptor table and exact bank size

The initialized descriptor table is present in supplied CODE.2 at physical file offset `0x7D254`. The first seven descriptors directly establish the common families:

```text
family                    slots columns capacity ui_x subtype record bytes
POUCH                         4      2       8     14      0        9
SACK                          8      4      30      5      1       17
CHEST                         8      4     100      5      2       17
SLING POUCH                   8      4     127      5      3       17
IVORY SCROLL CASE             6      3     127     14      4       13
KEY RING                      6      3     127     14      5       13
QUIVER                        8      4     127      5      6       17
```

BAG OF HOLDING uses the SACK-shaped 8-slot/subtype-1 descriptor with capacity 255.

Across descriptors `0..61`, `slot_count` sums to exactly 438. Therefore the serialized bank is independently closed as:

```text
438 object-ID words = 876 bytes
62 state bytes       =  62 bytes
                         ---
                         938 bytes = 0x3AA
```

This proves the quick-start bank ends exactly at `0x0D50`, rather than merely inferring its size from the next known serializer field.

### Exact state widths and quick-start offsets

The state sequence below is derived from CODE.2 descriptor `slot_count`; names/OBJECTS IDs come from category-1 `OBJECTS.DAT +0x31`. Offsets are for the zero-live-object quick-start fixture.

```text
state OBJECTS family                    slots SAVE offset
 0      0    POUCH                        4   09A7
 1      1    SACK                         8   09B0
 2      2    CHEST                        8   09C1
 3      3    SLING POUCH                  8   09D2
 4      4    IVORY SCROLL CASE            6   09E3
 5      5    KEY RING                     6   09F0
 6      6    QUIVER                       8   09FD
 7    264    IVORY SCROLL CASE            6   0A0E
 8    265    QUIVER                       8   0A1B
 9    268    CHEST                        8   0A2C
10    269    POUCH                        4   0A3D
11    270    QUIVER                       8   0A46
12    271    CHEST                        8   0A57
13    272    KEY RING                     6   0A68
14    273    CHEST                        8   0A75
15    274    POUCH                        4   0A86
16    278    CHEST                        8   0A8F
17    279    POUCH                        4   0AA0
18    280    CHEST                        8   0AA9
19    281    SLING POUCH                  8   0ABA
20     80    BAG OF HOLDING               8   0ACB
21    282    CHEST                        8   0ADC
22    283    CHEST                        8   0AED
23    284    CHEST                        8   0AFE
24    285    KEY RING                     6   0B0F
25    288    QUIVER                       8   0B1C
26    289    CHEST                        8   0B2D
27    290    POUCH                        4   0B3E
28    291    IVORY SCROLL CASE            6   0B47
29    295    QUIVER                       8   0B54
30    297    CHEST                        8   0B65
31    298    CHEST                        8   0B76
32    299    SACK                         8   0B87
33    300    CHEST                        8   0B98
34    302    SACK                         8   0BA9
35    303    SLING POUCH                  8   0BBA
36    304    SACK                         8   0BCB
37    305    QUIVER                       8   0BDC
38    306    POUCH                        4   0BED
39    307    SACK                         8   0BF6
40    308    QUIVER                       8   0C07
41    309    QUIVER                       8   0C18
42    310    POUCH                        4   0C29
43    311    SLING POUCH                  8   0C32
44    312    SLING POUCH                  8   0C43
45    313    IVORY SCROLL CASE            6   0C54
46    314    IVORY SCROLL CASE            6   0C61
47    315    QUIVER                       8   0C6E
48    316    SLING POUCH                  8   0C7F
49    317    QUIVER                       8   0C90
50    318    POUCH                        4   0CA1
51    319    SLING POUCH                  8   0CAA
52    320    SACK                         8   0CBB
53    321    QUIVER                       8   0CCC
54    322    IVORY SCROLL CASE            6   0CDD
55    323    SLING POUCH                  8   0CEA
56    324    POUCH                        4   0CFB
57    325    IVORY SCROLL CASE            6   0D04
58    326    QUIVER                       8   0D11
59    327    QUIVER                       8   0D22
60    328    IVORY SCROLL CASE            6   0D33
61    none   QUIVER-shaped descriptor     8   0D40
```

For `N` live world objects add `N*0x16` to every offset in this table.

### Index-61 / index-62 compatibility anomaly

The shipped data contain a concrete boundary mismatch:

- OBJECTS ID 329 (`QUIVER`) stores `container_state_index = 62`.
- CODE.2 descriptor 61 is a fully initialized 8-slot subtype-6 QUIVER descriptor.
- CODE.2 descriptor 62 is ten zero bytes.
- SAVE loops exactly while state index `< 0x3E`, persisting indexes `0..61`; state 62 is never serialized by this loop.
- quick-start state 61 contains eight object ID `57` entries, i.e. a full arrow-family quiver-shaped payload.

This strongly suggests state 61 is the persistent state intended for the final quiver and that the static selector 62 is a shipped off-by-one/data anomaly. That interpretation is not yet promoted to an executable translation rule because no direct path mapping literal selector 62 to descriptor 61 has been proved. Preserve the shipped values; do not rewrite OBJECTS.DAT merely to normalize the discrepancy.

### Quick-start contents

The fixture parses exactly through all 62 descriptor-sized records. Forty-one records contain at least one non-empty object ID; 21 are empty. Every `per_slot_state_bits` byte is zero in this fixture.

Representative contents:

```text
state 0   POUCH          [71, -1, -1, -1]
state 1   SACK           [93, -1, 181, 182, 202, 223, -1, -1]
state 2   CHEST          [47, 47, 47, 47, 3, 63, 60, 65]
state 11  QUIVER         [57, 57, 57, 57, 57, 57, 57, 57]
state 20  BAG OF HOLDING [164, 164, 164, 164, 164, 164, 164, 164]
state 61  unreferenced   [57, 57, 57, 57, 57, 57, 57, 57]
```

Repeated IDs represent multiple item instances of the same object definition, not quantities.

### `0xF920` / `0xF91C`

Two dwords follow the container-state bank. Consumers clamp `0xF920` to `0..10` and `0xF91C` to `0..8` in one UI/control path. Exact names remain pending.

## Quick-start middle offsets

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

## Saved roster and trailing state

After charges, CODE.2 writes `0x16D0` bytes from runtime `0xA808` (`10 * 0x248`). The final operation writes 16 bytes from DGROUP `0x6FA0`, indexed by gameplay code as four 4-byte party-local entries. The quick-start fixture stores `0xFF` in all 16 trailing bytes.

## Native parsing rule

Read sequentially. After the dynamic live-world-object array and fixed middle globals, parse states `0..61` using the certified slot-count catalogue above, then F920/F91C, 15 charge counters, ten `0x248` roster records and the 16-byte tail. Do not model `0x0D59`, `0x0D68`, or total size `0x2448` as universal constants, and do not treat the 938-byte container bank as an anonymous blob in the logical model.