# SAVE.DAT global/non-character state

Status: **serializer/deserializer-certified structural reconstruction.** SAVE files are variable-length: the supplied quick-start fixture is `0x2448` bytes because its live-world-object count is zero. The persistent container bank is parsed record-by-record, and the formerly anonymous `D8A8`, `A7E4`, `F920`, `F91C`, and trailing `0x6FA0` state now have executable-backed behavioral identities.

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
0x006C..0x006F  interrupt-driven engine tick counter snapshot
0x0070..0x0073  uint32 global runtime state at DGROUP 0xCF4C
0x0074..0x0075  int16 global selector/state at DGROUP 0x8600
0x0076          uint8 companion selector/state at DGROUP 0x8602
0x0077..0x0996  active party: 4 * 0x248
0x0997..        live-world-object section and persistent container state
...             charged-item counters[15]
...             saved roster: 10 * 0x248
...             four 4-byte party container-UI contexts
```

All offsets after the world-object count can move when that count is nonzero.

## Fixed `0x77`-byte prefix

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
+0x6C  uint32 interrupt_tick_counter snapshot
+0x70  uint32 DGROUP 0xCF4C
+0x74  int16  DGROUP 0x8600
+0x76  uint8  DGROUP 0x8602
```

Quick-start values: level 22, coordinates `0x00048000/0x00048000`, facing 270, interrupt-tick value 1899 (`0x76B`), `CF4C=0x000E0000`, `8600=-1`, `8602=0`.

### `+0x6C`: interrupt-driven engine tick counter, not the gameplay-effect clock

This field is now distinguished from saved `A7E4` below. Physical `0x6B624` simply returns DGROUP `0xF58C`. The interrupt handler beginning at physical `0x6B62C` reads and increments `0xF58C`; physical `0x6B65C` restores a caller-supplied value under `cli`/`sti`. The SAVE path calls the getter before writing `+0x6C`, and the load path passes the restored dword to the setter.

Therefore `+0x6C` is an **interrupt-driven engine tick-counter snapshot**. Character effect/action deadlines do not use this value directly; those use the separate pausable gameplay counter `A7E4`.

## Active-party image

Exactly `0x920` bytes from runtime `0x218E0` (`4 * 0x248`).

## Dynamic middle section

Immediately after the party image:

```text
uint32 live_world_object_count;                  // DGROUP 0xC038
WorldObject22 live_world_objects[count];         // runtime 0x248EC, 0x16 each
uint32 spell_ui_selected_character[2];           // DGROUP 0xD8A8
uint32 gameplay_tick_counter;                    // DGROUP 0xA7E4
for container_state = 0..61:
    int16 object_id[descriptor.slot_count];
    uint8 per_slot_state_bits;
uint32 environment_shading_level;                // DGROUP 0xF920, clamped 0..10
uint32 level_light_adjustment;                   // DGROUP 0xF91C, clamped 0..8
uint8 charged_item_counter[15];
```

`DGROUP 0xC038` is a live-object count with executable limit `0x96` (150). Creation/removal code indexes a `0x16`-byte structure array at runtime `0x248EC`. The quick-start fixture count is zero, so every later fixture offset shifts by `live_world_object_count * 0x16` in saves containing live world objects.

## `D8A8[2]`: selected spell-UI characters

The two serialized dwords at DGROUP `0xD8A8` are character indexes retained by the two spell-interface groups.

Physical `0x5E2CC` accepts a character index, computes `group = character_index / 2`, validates that the character is active and has casting capability, then stores:

```text
D8A8[group] = character_index
```

It resets the associated spell-list UI state and expands that character's selected spell bank. Physical `0x5DF48` takes a spell-interface group, loads `D8A8[group]`, then reads that character's saved spell-bank/scroll fields and renders the spell interface. Combat/status paths compare changed character indexes against the same selected-character entries to request UI refreshes.

Safe archival identity:

```cpp
uint32_t spell_ui_selected_character[2];
```

The supplied quick-start fixture stores `[0, 0]`. The two entries are behavioral UI selections, not roster pointers.

## `A7E4`: pausable gameplay tick counter / deadline timebase

DGROUP `0xA7E4` is the persistent gameplay-time counter used by saved character deadlines.

Executable anchors:

- physical `0x54BE8` is a trivial getter returning `A7E4`;
- physical `0x54BE0` increments `A7E4` while the gameplay timing gate is active;
- physical `0x59670(value)` writes the restored value to `A7E4` and synchronizes an internal maintenance baseline;
- the SAVE loader reads this dword and calls `0x59670`;
- primary-effect, temporary-hand, Strength-recovery, timed-slot, and hand/action deadline producers call `0x54BE8` and store `A7E4 + duration`;
- timer-maintenance paths compare those saved deadlines against the same getter.

Physical `0x59680` also uses elapsed `A7E4` for periodic gameplay maintenance. At intervals of `0x438` ticks it advances shared world/time state and performs party healing under the relevant conditions; at `0x21C` ticks it rotates another four-party maintenance operation. A resting path can advance `A7E4` directly by `0x438`.

Safe archival identity:

```cpp
uint32_t gameplay_tick_counter;
```

This is intentionally described in engine ticks rather than seconds/minutes because the exact real-time conversion is not required by the serializer evidence. Quick-start value: `1735` (`0x06C7`).

## Persistent container-state bank

The next 62 records are the mutable state backing category-1 `OBJECTS.DAT` containers. `OBJECTS.DAT +0x31` is a signed `container_state_index` selecting a 10-byte runtime descriptor at DGROUP `0x7330 + index*10`.

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

The SAVE serializer writes only `2*slot_count` bytes from `contents_pointer`, then byte `+9`:

```cpp
struct SavedContainerState {
    int16_t object_id[slot_count]; // -1 = empty
    uint8_t per_slot_state_bits;
};
```

### Executable descriptor table and exact bank size

The initialized descriptor table is present in supplied CODE.2 at physical file offset `0x7D254`. Common families are:

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

Across descriptors `0..61`, `slot_count` sums to exactly 438:

```text
438 object-ID words = 876 bytes
62 state bytes       =  62 bytes
                         ---
                         938 bytes = 0x3AA
```

### Exact state widths and quick-start offsets

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

### State-61 / state-62 shipped mismatch

The static data and executable boundary do not agree at the final container selector:

- OBJECTS ID 329 (`QUIVER`) stores `container_state_index = 62`;
- CODE.2 descriptor 61 is a fully initialized 8-slot subtype-6 QUIVER descriptor;
- CODE.2 descriptor 62 is ten zero bytes;
- SAVE serializes indexes `0..61` only;
- quick-start state 61 contains eight object ID `57` entries, a full arrow-family quiver-shaped payload.

The new direct-path audit found **no executable `62 -> 61` repair**. Generic open-container code at physical `0x5C5B7` sign-extends OBJECTS `+0x31`, multiplies it directly by ten, and reads descriptor `+4`; other container paths perform the same literal selector-to-descriptor calculation. Thus object 329's shipped selector addresses descriptor 62 as stored.

The pattern remains consistent with a shipped off-by-one/static-data mistake, but a faithful engine should **not silently normalize it**. Preserve OBJECTS selector 62, the zero descriptor 62 behavior, and serialized state 61 unless a separately documented compatibility fix is intentionally enabled.

Representative quick-start contents:

```text
state 0   POUCH          [71, -1, -1, -1]
state 1   SACK           [93, -1, 181, 182, 202, 223, -1, -1]
state 2   CHEST          [47, 47, 47, 47, 3, 63, 60, 65]
state 11  QUIVER         [57, 57, 57, 57, 57, 57, 57, 57]
state 20  BAG OF HOLDING [164, 164, 164, 164, 164, 164, 164, 164]
state 61  unreferenced   [57, 57, 57, 57, 57, 57, 57, 57]
```

Forty-one records contain at least one non-empty object ID; 21 are empty. Every `per_slot_state_bits` byte is zero in this fixture.

## `F920`: environment/time-of-day shading control

DGROUP `0xF920` is a renderer/environment shading control, clamped to `0..10` by physical `0x6CE00` and passed as the first argument to palette/shading-table generator `0x6C938`.

Time-of-day maintenance at physical `0x6CEB8` extracts the high word of `CF4C`, normalizes it modulo 24, and—when the level uses a non-`0xFF` environment profile—loads `F920` from a 24-entry signed-word table at `0xF924 + hour*2`. It then rebuilds renderer shading tables. A separate renderer path at physical `0x5A1F0` uses `F920` as an index into an 11-entry table.

For levels whose header environment profile is `0xFF`, level loading instead initializes `F920` from the first byte of a level-indexed fallback table at physical `0x53AAA..0x53AC4`.

Safe behavioral name:

```cpp
uint32_t environment_shading_level; // clamped 0..10
```

The original editor/player-facing term is not known. Quick-start value: 0.

## `F91C`: level light adjustment combined with party effects

DGROUP `0xF91C` is clamped to `0..8` by physical `0x6CE5C`. Every palette/shading recomputation calculates:

```text
palette_light_parameter = F91C + party_effect_light_override
```

where physical `0x6CD98` returns 8 when any active party character has primary effect bit mask `0x0402` (effects 1 or 10), otherwise 0. `0x6C938` itself caps the combined argument at 8.

For `environment_profile_id == 0xFF`, level loading initializes `F91C` from the second byte of the same level-indexed fallback table used for `F920`. Expiration/application paths for effects 1 and 10 trigger `0x6CE5C(F91C)` to rebuild the renderer tables after party effect state changes.

Safe behavioral name:

```cpp
uint32_t level_light_adjustment; // clamped 0..8
```

This identifies the renderer role without prematurely assigning the exact in-game terminology or names of effects 1/10. Quick-start value: 0.

## Quick-start middle offsets

```text
0x0997..0x099A  uint32 live_world_object_count = 0
0x099B..0x09A2  spell_ui_selected_character[2] = [0,0]
0x09A3..0x09A6  gameplay_tick_counter = 1735
0x09A7..0x0D50  62-container mutable payload (938 bytes)
0x0D51..0x0D54  environment_shading_level = 0
0x0D55..0x0D58  level_light_adjustment = 0
0x0D59..0x0D67  charged_item_counter[15]
0x0D68..0x2437  saved roster, 10 * 0x248
0x2438..0x2447  party_container_ui_context[4]
```

## Trailing `0x6FA0`: four party container-UI contexts

The final 16 bytes are four 4-byte UI contexts indexed by party member. They are not generic opaque trailing counters.

Opening a category-1 object at physical `0x5C56C..0x5C5E4` builds one context directly from the object's container descriptor and object definition. Rendering/transition routines `0x4BED0` and `0x4BFD8` step byte `+2` from `0 -> 3` while opening and `3 -> 0` while closing. Generic container rendering at `0x4B994` uses phase 3 to draw the actual slot contents.

Per-party layout:

```text
+0  int8  container_subtype_or_minus1
+1  int8  container_state_index_or_minus1
+2  int8  container_ui_phase_or_minus1   // 0..3 while active; 3 = fully open/content phase
+3  int8  container_object_id_or_minus1
```

Evidence for the other bytes is direct:

- `+0` receives descriptor `+4` (`container_subtype`);
- `+1` receives OBJECTS `+0x31` (`container_state_index`) and later selects the descriptor/content pointer;
- `+3` receives the active object's definition ID byte and is later multiplied by the `0x41` OBJECTS-record stride for object-property lookup;
- party-slot swap logic swaps all four bytes together, preserving the UI context with the corresponding party position.

All four contexts are initialized to `FF FF FF FF`; the quick-start fixture stores that inactive form for all four entries.

A compatibility model can expose:

```cpp
struct PartyContainerUiContext {
    int8_t container_subtype;
    int8_t container_state_index;
    int8_t ui_phase;
    int8_t container_object_id;
};
PartyContainerUiContext party_container_ui_context[4];
```

Preserve the signed-byte representation and `-1` sentinel exactly.

## Native parsing rule

Read sequentially. After the dynamic live-world-object array, read `spell_ui_selected_character[2]`, `gameplay_tick_counter`, states `0..61` using the certified slot-count catalogue, the two renderer controls, 15 charge counters, ten `0x248` roster records, and four 4-byte party container-UI contexts.

Do not model `0x0D59`, `0x0D68`, `0x2438`, or total size `0x2448` as universal constants. Preserve the state-61/62 mismatch rather than silently repairing shipped data.