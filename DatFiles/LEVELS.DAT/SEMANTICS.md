# LEVELS.DAT / LEVnnn.DAT semantic reconstruction

Status: **container, cell-state, 10-byte control, compact-family expansion, fixed event-script table, event-callee, active header-tail/music, automap-cache, dynamic-door, loose-item, and annotation-tail semantics substantially reconstructed from certified CODE.2 and supplied DAT files.**

The immutable CODE.2 bytes remain controlling evidence. Names are assigned only where executable behavior or a directly loaded database proves them.

## Primary CODE.2 evidence

- level event interpreter: `002A8..006B8`
- level loader: `181B4..18D8B`
- standalone writer: `171FC..17837`
- persistent event flags: `1C124..1C219`
- indexed text reader: `1C368..1C408`
- numbered interactive event runner: `139AC`, `13AD8`, `13EF8`
- sound-effect request queue: `28110..28164`
- music-sequence subsystem: `28C54..290C2`
- 10-byte lookup helpers: `1E8C8..1EA03`
- legacy 11->22 conversions: `1866B..18918`
- family-C pickup/removal: `21704`
- family-C loose-item insertion: `2195C..21ADA`
- family-C movement/collision: `2388C`, `23B38`
- family-C scene emission: `2BDF4..2BFDC`
- dynamic 10-byte records: `20B40..20CF2`
- automap cache rebuild: `01558..01830`
- automap SAVE/export: `026E8..02A33`
- annotation lookup/legend: `23B8`, `2474`

## Fixed 0x2A4 header

The header is loaded to DGROUP `0x25618` and written verbatim.

```text
+0x001  int16 map_width
+0x003  int16 map_height
+0x00A  uint16 primary_resource_A[64]
+0x08A  uint16 alternate_resource_A[64]
+0x10A  uint16 primary_resource_B[64]
+0x18A  uint16 alternate_resource_B[64]
+0x20A  uint16 automap_style_control[64]
+0x28A  int16 family_A_count   (0x258A2)
+0x28C  int16 family_C_count   (0x258A4)
+0x28E  int16 family_B_count   (0x258A6)
+0x290  uint8 environment_profile_id_or_FF
+0x292  int16 special_npc_type[3]
+0x298  uint8 renderer_fill_base_A
+0x299  uint8 renderer_fill_base_B
+0x29A  int16 render_threshold_floor
+0x29C  uint8 level_music_sequence_id
```

The first four 64-word banks feed first-person resource/render selection and are mutable at runtime. The fifth is consumed by automap rendering.

The active header tail is behaviorally decoded in `HEADER_TAIL.md`. In particular, `+0x29A` is normalized from zero to one immediately after load, exactly explaining the sole fixed-header byte difference between archive level 22 and supplied `LEV022.DAT`.

Header `+0x29C` is now proved to be the default **level music-sequence ID**. Every direct CODE.2 consumer compares or routes it through the `0x28C54..0x290C2` music/sequence subsystem; the adjacent options controls separately route sound effects through `0x280xx` and music through `0x290xx`.

Byte `+0x291` and bytes `+0x29D..+0x2A3` have no direct absolute CODE.2 consumers found and are zero in all supplied archive records, but remain raw preserved bytes rather than format-mandated padding.

## 16-bit cell layer

Immediately after the header is a row-major `width * height` array at runtime `0x198E0`.

```text
0x003F  six-bit cell code
0x0040  transient current visibility/raycast mark
0x0080  special-cell gate
0x0100  neighbor/substitution render gate
0x0200  one-bit render attribute
0x0400  persistent automap explored/revealed mark
0x0800  conditional pass-through / special automap behavior
0x1000  per-cell lookup/render augmentation
0x2000  unresolved
0xC000  two-bit render selector
```

See `CELL_WORD.md` for consumer-level proof and archive statistics.

## 10-byte cell records

The archive stores one width-indexed collection. The loader partitions it into:

```text
runtime group 1 at 0xCB3C : archive kind != 1
runtime group 2 at 0xCD3C : archive kind == 1
```

Standalone saves serialize the two groups separately, with active-animation byte `0xCF50` between them.

Proved common fields:

```text
+0x00  uint8 kind
+0x01  uint8 control/state flags
+0x04  uint8 row
+0x05  uint8 column
```

All 1,659 supplied archive records have byte `+5` equal to the owning column and byte `+4` inside map height.

Important proved byte-1 roles:

```text
0x03  low-two-bit state/mode field
0x04  interaction eligibility gate
0x08  kind-3 geometry inclusion modifier
0x10  active continuation/render gate
0x40  interaction suppression gate
```

Kind-specific proof includes:

- kind 1: dynamic barrier/door record; `+6` position, `+7` signed animation delta;
- kind 2: persistent two-state interaction; signed `+2` dispatched through the level event-script interpreter;
- kind 4: conditional interaction/event path; signed `+2` dispatched on successful action and `+6` consumed as a kind-specific accepted-ID field;
- kind 10: byte `+9` is passed as a resource/mode argument.

For kind 1, endpoint `+6 == 0` is open/passable and `+6 == 8` is closed/blocking. Positions 1..7 are transitional and block on the traced traversal path.

See `CELL_RECORDS.md` and `DYNAMIC_CELL_RECORDS.md`.

## Legacy compact 11-byte families

All three archive families use:

```text
+0x00  uint16 type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_flags
```

The common 22-byte runtime expansion includes:

```text
runtime +0x00 = (y_cell << 16) | y_fraction
runtime +0x04 = (x_cell << 16) | x_fraction
runtime +0x0C = y_cell * map_width + x_cell
runtime +0x0E = type_id
runtime +0x10 = 0
```

Across all supplied levels there are 6,204 compact records and every stored cell coordinate is in bounds.

### Family A — NPC/creature world records; count 0x258A2

Family A directly indexes the table loaded from `NPCS.DAT`:

- CODE.2 `16810` loads `NPCS.DAT` as `count * 0x45` bytes to pointer `0x22230`.
- expansion `186B9` indexes that exact pointer with `type_id * 0x45`.
- supplied `NPCS.DAT` contains 83 records, and all 1,258 family-A records use IDs `0..82`.

Family A is therefore the NPC/creature type family. Its expanded/direct-save live record is now mapped:

```text
+0x00  y_position_16_16
+0x04  x_position_16_16
+0x08  runtime_flags, initialized from NPCS.DAT +0x35
+0x0C  cached cell index
+0x0E  NPCS.DAT type ID
+0x10  action/state countdown
+0x12  current HP
+0x14  packed runtime state; bits 3..5 are facing/orientation
```

The supplied LEV022.DAT independently proves that these are mutable actor fields rather than only loader scratch: its sole ASSASSIN has moved from archive x-cell 13 to direct-save x-cell 18, changed facing 2 to 6, and stores current HP 15 plus action-state countdown 4. See `FAMILY_A.md` and `../LEV022.DAT/MUTABLE_DELTA.md`.

### Family B — placed object/item map records; count 0x258A6

Family B expands to runtime `0x22238`; its type word is runtime `+0x0E` at `0x22246`. Level 22's 110 archive family-B records reproduce all 110 supplied direct-save records byte-for-byte after the proved expansion.

The family is tied to the `OBJECTS.DAT` namespace. Supplied `OBJECTS.DAT` contains 336 entries, all 4,324 family-B records use IDs `0..174`, and the automap placed-record overlay treats this family as placed object/item records. Its compact placement/orientation byte spans values `0..7` across the supplied archive.

### Family C — loose/pickable item world records; count 0x258A4

Family C expands to runtime array `0x17618`. Loader `18878..18918` preserves the common fixed-point coordinates, maps compact type `0x54` to runtime `0x58`, zeros runtime `+0x10`, and masks runtime flags with `0xFE07` without importing compact byte `+0x0A`.

Direct CODE.2 consumers close its gameplay role:

- `2195C..21ADA` appends a loose object instance with OBJECTS.DAT type and world position, increments `0x258A4`, and enforces a 300-record runtime ceiling by dropping the oldest record when full;
- `224FA..22566` resolves a rendered scene entry of source class 2 to a family-C index and calls `21704`;
- `21704` passes the selected record's type through the item/UI path, compacts the 22-byte array, and decrements `0x258A4`, proving pickup/removal behavior;
- `2BDF4..2BFDC` walks visible family-C records, indexes OBJECTS.DAT with `type_id * 0x41`, and emits them as a distinct renderer source class;
- `23B38` creates an active family-C item with heading/lifetime state, while `2388C` advances its position, collision-tests it and updates/clears active-motion state;
- the periodic update path `1BF4D..1BFA0` scans and compacts those active moving items.

The movement path special-cases recognizable throwable OBJECTS.DAT types including `0x57 AXE OF HURLING`, `0x58 ARROW OF SLAY UNDEAD`, `0x59 DAGGER OF THROWING`, and `0x5A JAVELIN OF LIGHTNING`. Thrown/projectile behavior is therefore runtime state on the loose-item family rather than a separate serialized family.

Archive-wide family-C evidence:

```text
records                622
levels containing C     26
maximum in one level    90
unique object IDs       200
object ID range      0..335
compact flags             0 for all 622 supplied records
```

All family-C IDs are valid in the 336-entry supplied OBJECTS.DAT. The exact historical reason for the family-C-only legacy migration `0x54 -> 0x58` remains open. See `FAMILY_C.md` and `analyze_family_c.py`.

## Fixed 0x514 level event-script table

The fixed block is structurally and behaviorally identified. CODE.2 allocates exactly `0x514` bytes and stores its pointer at DGROUP `0xCF44`. Interpreter `002A8` selects:

```text
slot = event_table + script_index * 26
```

The block is exactly 50 slots of 26 bytes:

```text
slot +0x00  uint8 active_command_count
slot +0x01  command[0]  // 5 bytes
slot +0x06  command[1]
slot +0x0B  command[2]
slot +0x10  command[3]
slot +0x15  command[4]
```

Each command is `opcode + four raw argument bytes`. The interpreter supports opcodes `0..12` and executes only the first `active_command_count` commands.

Archive-wide:

```text
physical slots   1,600
active slots       394
active commands     555
max active count      5
```

One supplied slot proves inactive bytes are persistent: level 20 slot 1 has count 4 but a nonzero fifth command. Repackers must preserve all 26 bytes.

Proved opcode semantics now include:

- opcodes 0/1: coordinate-based cell-record action variants;
- opcode 2: toggle cell-record state;
- opcode 3: create a transient/moving effect record at a cell with quantized orientation;
- opcode 4: replace only the six-bit cell code at `(row,column)`;
- opcode 5: queue sound-effect request ID through `28110` with channel 0, aux `0xFF`, priority 0;
- opcode 6: timed transition from the current music sequence to the sequence ID encoded at command byte `+3`;
- opcode 7: modal indexed-text display; `1C368` resolves an offset table, reads a length-prefixed string, and XOR-decodes it with `0xDC`;
- opcode 8: set persistent global event flag `arg3`;
- opcode 9: clear persistent global event flag `arg3`;
- opcode 10: toggle persistent global event flag `arg3`;
- opcode 11: reserved special action for IDs `0x0B..0x0D`, otherwise run a numbered interactive event resource selected by `arg3` and stop the current event slot;
- opcode 12: assign command byte `+3` to global `0xBF44`.

The persistent event-flag vector begins at `0x22200`; SAVE.DAT writes and reads exactly `0x20` bytes from that address, proving a persistent 256-bit game/event-state vector. Only opcode 8 occurs in the supplied archive, setting flags `0x52`, `0x55`, `0x56`, and `0x57`.

Opcode 11's default numbered-resource path cross-links INFO.DAT. CODE.2 reads a 13-byte digit configuration vector followed by a signed 16-bit path length and path bytes; the supplied path is `D:`. Routine `139AC` constructs `<path>\\NN`, so numbered event 7 resolves to `D:\\07` in the supplied install image.

Opcode 5 uses the separate sound-effect subsystem, while opcode 6 and header `+0x29C` use the music/sequence subsystem. The two supplied opcode-6 instances select music IDs `0x2F` and `0x2E`.

See `EVENT_SCRIPTS.md`, `EVENT_CALLEES.md`, `analyze_event_scripts.py`, and `analyze_event_callees.py`.

## Automap cache and overlays

The common-tail `width * height` byte array is a generated automap/display cache. The standalone writer calls `01558` immediately before serializing it.

The SAVE/export path overlays placed family-B object/item records and automap annotation labels after the base cache conversion. Annotation labels have final precedence. See `AUTOMAP.md` and `AUTOMAP_OVERLAYS.md`.

## Common annotation tail

After duplicated dimensions and automap cache, the common tail is:

```text
uint32 annotation_blob_length
uint8  annotation_blob[annotation_blob_length]
```

CODE.2 loads this into a zero-filled `0x800`-byte buffer at `0x5760`. Runtime entries are:

```text
int16 raw_x
int16 raw_y
int16 payload_length
uint8 payload[payload_length]
```

A zero-length entry terminates the in-memory list and is not included in the serialized length. The exporter maps raw coordinates with signed `/8` and `/5` scaling, writes sequential map labels, and emits a matching label/text legend.

All supplied archive records and `LEV022.DAT` have annotation length zero.

## Level 22 correlation

Archive record 22 and `LEV022.DAT` are both 30x15 with family counts A/B/C = `1 / 110 / 0`.

The byte-count comparison is now refined into semantic deltas:

```text
0x2A4 header:
  exactly +0x29A changes 0 -> 1 (proved loader normalization)

16-bit cell layer:
  exactly 313 / 450 words change
  every changed word has archive XOR direct == 0x0440
  no topology/code/other cell flag changes

10-byte lists:
  archive/group-1 10 / 10 byte-identical
  active animation count = 0
  group 2 = empty

family A:
  one ASSASSIN moves x=13 -> 18
  facing 2 -> 6
  current HP = 15
  action-state countdown = 4

family B:
  110 / 110 deterministic expansions byte-identical

family C:
  0 / empty in both

0x514 event table:
  byte-identical; all zero

final byte cache:
  archive all 0xFF
  direct = 313*0x2E + 130*0xFF + 7*0x01
  subject to the separately bounded 40-cell replay/provenance discrepancy
```

The standalone preamble byte 0 is also now proved writer-unowned: the writer never writes offset 0. See `../LEV022.DAT/MUTABLE_DELTA.md`.

## Current semantic boundary

Safe to expose semantically now:

- dimensions, five header lookup banks, active header-tail renderer/profile controls, and default level music sequence;
- packed cell-code/visibility/exploration fields and behavioral flags;
- 10-byte record kind, coordinates, proved control bits, and kind-specific fields;
- dynamic open/closed group-2 state;
- family A as NPC/creature records;
- family B as placed object/item map records;
- family C as loose/pickable item records with dropped/thrown runtime motion state;
- fixed 50-slot level event-script table including sound, music, indexed-message, persistent-flag, numbered-event, cell mutation, and global-state actions;
- generated automap cache, base export, placed-object overlay, and annotation overlay;
- annotation-list tail.

Remaining high-value LEVELS work is the few still-behavioral cell/control-bit names, reusable semantics in the three story-specific opcode-11 branches if any, and the historical reason for the family-C `0x54 -> 0x58` legacy migration. After that, the next value is in fully reconstructing the other DAT databases using the cross-links already proved here.
