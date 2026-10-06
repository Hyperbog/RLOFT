# LEVELS.DAT / LEVnnn.DAT semantic reconstruction

Status: **container, cell-state, 10-byte control, compact-family expansion, automap-cache, dynamic-door, and annotation-tail semantics substantially reconstructed from certified CODE.2 and supplied DAT files.**

The immutable CODE.2 bytes remain controlling evidence. Names are assigned only where executable behavior or a directly loaded database proves them.

## Primary CODE.2 evidence

- level loader: `181B4..18D8B`
- standalone writer: `171FC..17837`
- 10-byte lookup helpers: `1E8C8..1EA03`
- legacy 11->22 conversions: `1866B..18918`
- dynamic 10-byte records: `20B40..20CF2`
- 10-byte interaction dispatcher: around `22B09`
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
+0x29A  uint16 field_258B2
+0x29C  uint8 mode_258B4
```

The first four 64-word banks feed first-person resource/render selection and are mutable at runtime. The fifth is consumed by automap rendering. Additional header bytes remain unresolved.

## 16-bit cell layer

Immediately after the header is a row-major `width * height` array at runtime `0x198E0`.

Proved split:

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
- kind 2: persistent two-state interaction; signed `+2` dispatched through common event/script routine;
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

### Family A — count 0x258A2

Family A directly indexes the table loaded from `NPCS.DAT`:

- CODE.2 `16810` loads `NPCS.DAT` as `count * 0x45` bytes to pointer `0x22230`.
- expansion `186B9` indexes that exact pointer with `type_id * 0x45`.
- supplied `NPCS.DAT` contains 83 records, and all 1,258 family-A records use IDs `0..82`.

Family A is therefore an NPC/creature type family, in addition to the previously proved runtime world-record behavior.

The expansion also imports metadata dword `NPC[type].+0x35`, maps compact low-three flags to runtime bits 3..5, masks runtime flags with `0x3E39`, and derives runtime `+0x12` through metadata-dependent logic.

### Family B — count 0x258A6

Family B expands to runtime `0x22238`; its type word is runtime `+0x0E` at `0x22246`. Level 22's 110 archive family-B records reproduce all 110 supplied direct-save records byte-for-byte after the proved expansion.

The family is strongly tied to the `OBJECTS.DAT` namespace:

- CODE.2 `168C8` loads `OBJECTS.DAT` as `count * 0x41` records to pointer `0x255D8`;
- supplied `OBJECTS.DAT` contains 336 entries;
- all 4,324 family-B records use IDs `0..174`;
- family-B consumers test IDs that map to concrete item/key entries, including `0x9B = BLOOD KEY`;
- the automap placed-record overlay singles out family-B IDs `0x3C..0x41`, which are six potion/oil entries in the supplied object table.

This supports the safe description **placed object/item record family**. The precise distinction from family C is still unresolved.

### Family C — count 0x258A4

Family C preserves the common coordinates, maps compact type `0x54` to runtime `0x58`, zeros runtime `+0x10`, and masks runtime flags with `0xFE07` without importing compact byte `+0x0A` on this conversion path.

Its supplied type IDs range `0..335`, also within the 336-entry `OBJECTS.DAT` namespace. The exact semantic distinction between family B and family C remains open.

## Automap cache and overlays

The common-tail `width * height` byte array is a generated automap/display cache. The standalone writer calls `01558` immediately before serializing it.

The cache encodes explored floor, connected boundary/wall, dynamic door-capable states, inset states, and special hidden/secret-marker behavior. The exporter converts resolved states to the documented CP437 wall/floor/door/inset bytes.

The SAVE/export path then overlays:

1. placed family-B records (`'*'` for object IDs `0x3C..0x41`, literal `0xFE` otherwise);
2. automap annotation labels generated from the final variable-length annotation blob.

Annotation labels have precedence over placed-record symbols in the merged output. See `AUTOMAP.md` and `AUTOMAP_OVERLAYS.md`.

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

```text
0x2A4 header:        1 differing byte (+0x29A)
16-bit cell layer:  626 differing bytes / 900
0x514 block:         byte-identical
final byte cache:   320 differing bytes / 450
family B expansion: 110 / 110 records byte-identical
```

The final cache contains the separately documented 40-cell replay discrepancy and must not be treated as a fresh exact `01558` oracle for this sample.

## Current semantic boundary

Safe to expose semantically now:

- dimensions and five header lookup banks;
- packed cell-code/visibility/exploration fields and behavioral flags;
- 10-byte record kind, coordinates, proved control bits, and kind-specific fields;
- dynamic open/closed group-2 state;
- compact record coordinates/type IDs and family-specific conversion rules;
- family A as NPC/creature records and family B as placed object/item records;
- generated automap cache, base export, placed-object overlay, and annotation overlay;
- annotation-list tail.

The next major unresolved structure is the fixed `0x514` block, followed by remaining header fields, family-C identity, and still-unnamed control bits/overlay glyph values.