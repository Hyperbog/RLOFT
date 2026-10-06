# LEVELS.DAT / LEVnnn.DAT semantic reconstruction

Status: **first semantic pass proven from certified CODE.2 plus all supplied LEVELS.DAT records and LEV022.DAT**.

This file deliberately separates proved storage semantics from unresolved gameplay meaning. The immutable CODE.2 bytes remain controlling evidence.

## CODE.2 evidence used

- Level loader: object-1 logical `181B4..18D8B`
- Standalone writer: `171FC..17837`
- Width-derived neighbor table: `170AC..171CE`
- 10-byte list lookup: `1E8C8..1EA03`
- Legacy compact conversions: `1866B..18918`

## Fixed 0x2A4 header: proved fields

The loader reads the complete 0x2A4 bytes to DGROUP `0x25618` and the writer emits the same 0x2A4-byte region verbatim.

```text
header +0x001  int16 map_width        -> DGROUP 0x25619
header +0x003  int16 map_height       -> DGROUP 0x2561B
header +0x28A  int16 count_258A2
header +0x28C  int16 count_258A4
header +0x28E  int16 count_258A6
header +0x29A  uint16 field_258B2
header +0x29C  uint8  mode_258B4
```

`map_width` is the row stride used in every proven linear-cell expression; `map_height` is the row-count bound. CODE.2 repeatedly forms `cell_index = row * map_width + column`.

`mode_258B4` is compared against the active resource/mode value and selects resource-loading behavior. Its higher gameplay name remains unresolved.

For the supplied archive record 22 and `LEV022.DAT`, the 0x2A4 headers differ at only one byte: header `+0x29A` is `00` in the archive baseline and `01` in the standalone save. No semantic name is assigned yet.

## 16-bit per-cell layer

Immediately after the 0x2A4 header, CODE.2 loads `map_width * map_height` 16-bit words row-by-row into DGROUP `0x198E0`. The writer serializes the same row-major word layer.

This is a packed per-cell state/topology word, not a plain unqualified tile ID: consumers independently test or mutate low six bits and flags including `0x0040`, `0x0080`, `0x0400`, `0x0800`, `0x1000`, and others. Individual bit names remain unresolved.

## 10-byte column-list records

The archive stores one width-indexed collection of 10-byte records. Standalone saves store two width-indexed collections separated by byte `CF50`.

Two coordinate bytes are now proved:

```text
record +0x04  uint8 row
record +0x05  uint8 column
```

CODE.2 `1E8C8` selects the pointer array by column and compares record byte `+4` with the requested row. The archive bytes independently confirm the redundant column byte: **all 1,659 archived 10-byte records have byte `+5` equal to the owning column, and every byte `+4` is within the level height**.

The legacy loader splits the single archive collection into the two runtime collections:

- archive records with byte `+0 == 1` are diverted to the second runtime collection;
- all other records remain in the first runtime collection;
- diverted records are bucketed by their byte `+5` column.

The standalone writer shows the runtime termination rules rather than storing a permanent trailing sentinel:

- first list continues while byte `+0 != 0` **or** byte `+1 & 0x10 != 0`;
- second list continues while byte `+0 != 0`.

Other proved accesses include byte `+6` of second-list records being converted to a value shifted left by 12 and byte `+7` participating in mutable countdown/state logic. Their gameplay labels are still unresolved.

## Legacy 11-byte record coordinate encoding

All three archive families use the same compact positional skeleton:

```text
+0x00  uint16 type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_flags
```

The loader reconstructs two packed 32-bit coordinates in each 22-byte runtime record:

```text
runtime +0x00 = (y_cell << 16) | y_fraction
runtime +0x04 = (x_cell << 16) | x_fraction
runtime +0x0C = y_cell * map_width + x_cell
runtime +0x0E = type_id
runtime +0x10 = 0
```

The high words are used as signed cell coordinates and the low words as sub-cell fractions. Runtime code also consumes the full dwords, establishing the packed 16.16-style representation.

Across all 32 archive levels there are **6,204 compact 11-byte records**. Every `x_cell` is in `[0,map_width)` and every `y_cell` is in `[0,map_height)`.

## Family-specific legacy expansion

### Family A — header count `0x258A2`

The final audit independently identifies scans bounded by `258A2` as world-record scans, so `world record family` is supported terminology for this table.

In addition to the common coordinate/type fields, the loader:

- loads runtime dword `+0x08` from the type metadata table at `metadata[type_id*0x45 + 0x35]`;
- maps the compact flag byte's low three bits into runtime flag bits 3..5;
- then masks the runtime flag word with `0x3E39`;
- derives runtime word `+0x12` through type/metadata-dependent logic, with a special type-8 result of `0x37`.

The exact gameplay meanings of runtime `+0x08`, `+0x12`, and individual flag bits remain unresolved.

### Family B — header count `0x258A6`

The loader writes the common coordinate/type fields, zeros runtime `+0x10`, maps compact low-three flag bits to runtime bits 3..5, and masks the runtime flags with `0xFE3F`.

This mapping has a strong independent sample check: for archive level 22, applying the encoded family-B expansion to its 110 compact records reproduces **all 110 corresponding 22-byte records in LEV022.DAT byte-for-byte**.

The supplied pair therefore directly validates the compact-to-expanded field order, not merely the section sizes.

### Family C — header count `0x258A4`

The common coordinate/type mapping is preserved, with two family-specific rules:

- compact type `0x54` is converted to runtime type `0x58`;
- runtime flags are masked with `0xFE07` and the compact byte `+0x0A` is not imported by this conversion path.

Runtime `+0x10` is explicitly zeroed. Other untouched/derived words remain unresolved.

## Level 22 baseline/save correlation

Archive level 22 and `LEV022.DAT` both describe 30x15 cells with counts `1 / 110 / 0` for families A/B/C.

Additional byte-level comparison:

```text
0x2A4 header:        1 differing byte (+0x29A)
16-bit cell layer:  626 differing bytes / 900
0x514 block:         byte-identical
final byte layer:   320 differing bytes / 450
family B expansion: 110 / 110 records byte-identical
```

This proves that the fixed 0x514 block is unchanged in this baseline/save pair while both per-cell layers are mutable in actual saved state. It does not prove that the 0x514 block is globally immutable for every gameplay path.

## Current semantic boundary

Proved enough to implement safely now:

- archive/direct dimensions and counts;
- row-major 16-bit cell layer;
- row/column coordinates in 10-byte records;
- archive-to-runtime separation of the two 10-byte list groups;
- compact 11-byte type/coordinate/fraction layout;
- exact common 11->22 coordinate/index conversion;
- family-specific flag/type conversion rules.

Still unresolved and intentionally unnamed:

- most of the 0x2A4 header;
- individual 16-bit cell-word bits;
- 10-byte record bytes other than proved accesses;
- gameplay identities of families B and C;
- exact contents of the 0x514 block;
- semantic meaning of the final per-cell byte layer.
