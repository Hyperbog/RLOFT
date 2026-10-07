# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 05 — Level Archive, World Geometry, Mutable State, Collision, and Scene Sources

**Status:** PRIMARY WORLD-STATE RECONSTRUCTION  
**Documentation branch:** `code1-decompilation`  
**Pre-documentation preservation baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`

---

## 1. Scope

This chapter documents how the original engine represents a level as both authored baseline data and mutable runtime state. It covers:

- the 32-record `LEVELS.DAT` archive;
- standalone `LEVnnn.DAT` mutable snapshots;
- load precedence between standalone and archive forms;
- the fixed 0x2A4 level header;
- map dimensions and row-major cell addressing;
- the 16-bit packed cell word;
- ordinary and dynamic collision;
- 10-byte cell/control records;
- dynamic barrier/door state;
- 11-byte compact world records and their 22-byte live forms;
- NPC/creature, placed-object, and loose-item families;
- runtime dropped/thrown items;
- transient/moving-effect records;
- scene-source-class translation into RES3;
- the boundary between immutable definition databases and mutable level instances;
- the fixed per-level event table at the structural level;
- the generated automap/cache tail as serialized world state.

The detailed automap glyph/export/UI pipeline is intentionally left for the following preservation chapter; this chapter documents only the parts required to understand world-state ownership and persistence.

The supplied project binaries were independently rechecked before this chapter was written:

```text
LEVELS.DAT
  size     499,878
  SHA-256  50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91

LEV022.DAT
  size       6,028
  SHA-256  f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e
```

These match the certified repository identities exactly. [LEVELS final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md)

## 2. The engine has two level representations

The most important architectural distinction is:

```text
LEVELS.DAT
    compact authored/baseline level representation

LEVnnn.DAT
    expanded mutable live-state snapshot
```

CODE.2 loader `181B4..18D8B` first attempts to open the standalone level form. Only when that file is absent does it open `levels.dat`, read the archive directory entry, and load the compact baseline. [LEVELS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FORMAT.md) [CODE.2 continuity, Batch 32](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/CODE2_CONTINUITY_ADDENDUM.md)

The behavioral model is therefore:

```text
if standalone mutable level exists:
    restore live state from LEVnnn.DAT
else:
    instantiate live state from compact LEVELS.DAT baseline
```

This makes the standalone file a **per-level persistence override**, not an alternate static resource format.

## 3. LEVELS.DAT archive directory

The master archive begins:

```text
+0x0000  uint32 record_count = 32
+0x0004  uint32 record_offset[32]
+0x0084  first level record
```

The first record offset is exactly:

```text
4 + 32*4 = 0x84
```

All 32 offsets are strictly increasing and the final record extends to EOF. [LEVELS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FORMAT.md)

Archive-wide certification accounts for:

```text
levels                         32
packed 16-bit cells       115,584
10-byte cell records        1,659
family-A compact records    1,258
family-B compact records    4,324
family-C compact records      622
event-table physical slots   1,600
active event slots             394
active event commands          555
```

[LEVELS final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md)

## 4. Per-record preamble and section boundaries

Every archive record and standalone level begins with a 0x1D-byte preamble:

```text
+0x00  uint8  writer-unowned/raw byte
+0x01  uint32 boundary_0
+0x05  uint32 boundary_1
+0x09  uint32 boundary_2
+0x0D  uint32 boundary_3
+0x11  uint32 boundary_4
+0x15  uint32 boundary_5
+0x19  uint32 boundary_6
```

For archive records, `boundary_0 == 0x1D`.

The standalone writer does **not** own byte `+0x00`: it seeks directly to offset `0x1D` before writing payload data, then later seeks to offset 1 to write the seven boundary dwords. The supplied archive records store zero in byte 0, while `LEV022.DAT` contains `0xDF`. [LEVELS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FORMAT.md) [LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

A faithful serializer must therefore preserve an imported byte 0 verbatim rather than assigning it gameplay meaning or normalizing it.

## 5. Fixed 0x2A4 level header

At `boundary_0`, CODE.2 reads exactly 0x2A4 bytes into runtime level-header storage.

Materially identified fields include:

```text
+0x001  int16 map_width
+0x003  int16 map_height

+0x00A  uint16 primary_resource_A[64]
+0x08A  uint16 alternate_resource_A[64]
+0x10A  uint16 primary_resource_B[64]
+0x18A  uint16 alternate_resource_B[64]
+0x20A  uint16 automap_style_control[64]

+0x28A  int16 family_A_count
+0x28C  int16 family_C_count
+0x28E  int16 family_B_count

+0x290  uint8 environment_profile_id_or_FF
+0x292  int16 special_npc_type[3]
+0x298  uint8 renderer_fill_base_A
+0x299  uint8 renderer_fill_base_B
+0x29A  int16 render_threshold_floor
+0x29C  uint8 level_music_sequence_id
```

[LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md) [Header-tail reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/HEADER_TAIL.md)

The first four 64-word banks are indexed by the six-bit cell code during first-person resource selection. The fifth bank participates in automap presentation. The header is therefore not merely map metadata: it is also a level-local resource-selection and rendering table.

## 6. Header state can be mutable

The fixed header is loaded into live RAM and the standalone writer later writes the live 0x2A4-byte block verbatim.

A proved example is `+0x29A`:

```text
archive value 0
    -> loader normalizes to 1
    -> standalone save writes 1
```

This exactly explains the only fixed-header byte difference between archive level 22 and `LEV022.DAT`. [Header-tail reconstruction](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/HEADER_TAIL.md) [LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

Thus a port should distinguish:

```text
serialized archive header
live normalized header
serialized standalone live header
```

rather than assuming all three are byte-identical.

## 7. Map grid and linear cell indexing

Immediately after the header is a row-major array of:

```text
map_width * map_height
```

16-bit cell words.

The common linear index is:

```text
cell_index = row * map_width + column
```

The same rule is independently used when compact world records are expanded:

```text
cached_cell_index = y_cell * map_width + x_cell
```

[LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md) [Family A](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

Across all 6,204 compact family records in the supplied archive, stored cell coordinates are in bounds. [LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md)

## 8. World positions use 16.16 fixed point

The three compact world families share cell + fraction coordinates:

```text
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
```

Archive expansion forms:

```text
runtime +0x00 = (y_cell << 16) | y_fraction
runtime +0x04 = (x_cell << 16) | x_fraction
```

[LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md)

The world coordinate model is therefore not cell-only. Actors and loose items retain sub-cell position at 1/65536-cell resolution in their 32-bit position dwords.

For compatibility documentation, the canonical axis convention is:

```text
runtime +0x00 = Y
runtime +0x04 = X
```

even though compact archive fields list X before Y.

## 9. Packed 16-bit cell word

The certified behavioral split is:

```text
bits 0..5   0x003F  six-bit cell code
bit  6      0x0040  transient current visibility/raycast mark
bit  7      0x0080  special-geometry center / dynamic-collision gate
bit  8      0x0100  special-geometry frame / adjacent-code substitution gate
bit  9      0x0200  extra render-prepass enable
bit 10      0x0400  persistent automap explored/revealed mark
bit 11      0x0800  secret/hidden-passage topology marker
bit 12      0x1000  10-byte-record state resource-offset enable
bit 13      0x2000  preserved authoring/legacy marker
bits14..15  0xC000  two-bit render shade/decrement level
```

[CELL_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_WORD.md)

The six-bit code is independently replaceable: event opcode 4 preserves the upper two low-byte bits while installing a new `0..63` code. It must therefore be modeled separately from the flags. [EVENT_SCRIPTS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)

## 10. Visibility and exploration are distinct bits

First-person traversal sets:

```text
0x0040 | 0x0400 = 0x0440
```

on reached cells.

Later, a map-wide renderer reset clears only `0x0040`; the `0x0400` exploration bit remains. [CELL_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_WORD.md)

The state model is therefore:

```text
0x0040 = visible/reached in current traversal/render cycle
0x0400 = persistently explored/revealed
```

This distinction is directly demonstrated by `LEV022.DAT`, whose explored session preserves both bits in 313 cells at save time. [LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

## 11. Ordinary versus special collision

The movement/collision model has a crucial branch:

```text
if cell has 0x0080:
    use dynamic/special-cell collision query
else:
    low-six-bit cell code != 0 is blocking
```

[CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

For `0x0080` cells, helper `1E930(cell_index)` searches the second 10-byte runtime record group. A matching dynamic record returns:

```text
record[6] << 12
```

and absence returns `0x8000`. The traced traversal treats zero as passable and nonzero as blocking. [DYNAMIC_CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md)

The `0x0080` bit is broader than “door”: many supplied `0x0080` cells have kind-8 or no 10-byte record at all. The correct archival name remains **special-geometry center / dynamic-collision gate**. [CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

## 12. Special geometry uses paired center/frame cells

Bit `0x0100` marks the neighboring/frame side of the same special-geometry system.

When rendering a `0x0100` cell, the engine checks a direction-dependent neighboring cell. If that neighbor carries `0x0080`, it can substitute the neighbor's six-bit code and choose a related resource variant. [CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

The archive geometry independently supports this relationship:

```text
0x0080 centers                 745
0x0100 frame cells           1,690

every 0x0080 center has 2..4 orthogonal 0x0100 neighbors
1,682 / 1,690 frame cells touch an 0x0080 center
```

[CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

A native level editor should therefore not present these as unrelated booleans.

## 13. Hidden-passage topology

Bit `0x0800` marks a special hidden/pass-through topology class.

CODE.2 proves that in a renderer-wide traversal mode, a nonempty `0x0800` cell can continue through the same path used by an empty cell. Automap generation also routes these cells through a special hidden-map class. [CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

All 47 supplied `0x0800` cells have nonzero cell codes and none has a 10-byte record.

The safe implementation name is **secret/hidden-passage topology marker**. Exact automap glyph behavior is documented separately in the next chapter.

## 14. Record-driven visual state

Bit `0x1000` connects a cell to the first 10-byte runtime record group.

When set, and when no earlier special-neighbor substitution wins, the renderer calls `1E9A0(cell_index)`, which returns:

```text
record[1] & 0x03
```

The engine adds that `0..3` state to both selected cell resource IDs. [CELL_FLAG_LINKS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_FLAG_LINKS.md)

Every one of the 1,335 supplied non-kind-1 10-byte records occurs on a `0x1000` cell. This makes the relation explicit:

```text
cell code -> base resource selection
record state -> small resource variant offset
```

## 15. Extra render pass and shade selector

Bit `0x0200` causes the generated render record to participate in an additional preliminary pass before the ordinary all-record pass. Its exact visual/editor term remains unknown. [CELL_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_WORD.md)

Bits `0xC000` form one two-bit `0..3` decrement selector. The renderer subtracts this value from both byte lanes of a packed render value and clamps results at zero. It is safely described as a **render shade/decrement level**, not universally as lighting. [CELL_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_WORD.md)

Only two supplied archive cells use a nonzero selector, both value 2.

## 16. Cell bit 0x2000 remains raw legacy/authoring state

A final audit examined all 76 direct CODE.2 references to the live cell-word array.

Every established cell field above has a direct consumer except `0x2000`. No direct instruction isolates, sets, clears, or branches on that bit. [CELL_WORD.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_WORD.md)

The only supplied instances are:

```text
level 12  (44,34)  0x2400
level 13  (44,34)  0x2400
level 14  (44,34)  0x2400
```

No 10-byte record or supplied event command targets those cells. [LEVELS final audit](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FINAL_AUDIT_2026-10-06.md)

It must be preserved exactly, with no invented gameplay behavior.

## 17. Legacy 10-byte cell-record collection

After the cell grid, archive records store exactly `map_width` lists:

```text
for each column:
    int16 count
    count * 10-byte record
```

All 1,659 supplied records satisfy:

```text
record[5] == owning column
record[4] is a valid row
```

[LEVELS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FORMAT.md) [CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_RECORDS.md)

The current common layout is:

```text
+0x00  uint8 kind
+0x01  uint8 control/state flags
+0x02  int8  event/script selector on proved kinds
+0x03  uint8 raw/unresolved
+0x04  uint8 row
+0x05  uint8 column
+0x06  uint8 kind-specific
+0x07  uint8/int8 kind-specific
+0x08  uint8 raw/unresolved
+0x09  uint8 kind-specific
```

Bytes without direct behavior remain raw.

## 18. The loader splits 10-byte records into two live groups

The archive stores one collection, but CODE.2 divides it at load time:

```text
runtime group 1  DGROUP 0xCB3C  archive kind != 1
runtime group 2  DGROUP 0xCD3C  archive kind == 1
```

Standalone files serialize the two groups separately and insert one byte between them:

```text
DGROUP 0xCF50  active dynamic-animation count
```

[DYNAMIC_CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md)

This split is a runtime semantic transformation, not merely a parser convenience.

## 19. 10-byte record control byte

Record byte `+1` is a bitfield.

Proved fields include:

```text
0x03  low-two-bit state/mode
0x04  interaction-enable / eligibility gate
0x08  kind-3 geometry inclusion modifier
0x10  active continuation/render gate
0x40  interaction suppression/inhibit
```

[CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_RECORDS.md)

The low two bits have two separately proved uses:

- group-1 resource-state selection through `1E9A0`;
- group-2 dynamic interaction mode.

The same bit position therefore participates in kind/group-specific behavior and should not be flattened into one universal enum.

## 20. Dynamic barriers/doors

Kind 1 records are routed to runtime group 2.

For these records:

```text
+0x06  uint8 position/state, endpoints 0 and 8
+0x07  int8  animation delta, normally -1, 0, +1
```

When active-animation count is nonzero, CODE.2 repeatedly applies:

```text
record[6] += record[7]
```

until position reaches 0 or 8, then clears the delta and decrements the active count. [DYNAMIC_CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md)

Collision independently proves:

```text
position 0    open / passable
position 8    closed / blocking
position 1..7 transitional and blocking on the traced traversal path
```

[DYNAMIC_CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md)

Of 324 supplied archive kind-1 records, 323 begin at position 8 and stationary. This strongly corroborates the barrier/door interpretation while remaining a corpus observation rather than a format law.

## 21. Dynamic interaction modes

For kind 1, `record[1] & 3` selects interaction behavior.

Mode 1 toggles endpoints:

```text
0 -> animate +1 toward 8
8 -> animate -1 toward 0
```

Mode 2 starts an openward `-1` animation. Its higher-level gameplay/editor meaning remains unresolved. [DYNAMIC_CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/DYNAMIC_CELL_RECORDS.md)

Do not rename mode 2 without new primary evidence.

## 22. Other proved 10-byte record kinds

The interaction dispatcher recognizes kinds 1 through 10, although kind 9 does not occur in the supplied archive.

Materially proved examples are:

```text
kind 1   dynamic barrier/door interaction
kind 2   persistent two-state interaction; +2 is event slot selector
kind 4   conditional interaction/event path; +2 is event selector
kind 10  +9 is passed as a resource/mode selector
```

[CELL_RECORDS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/CELL_RECORDS.md)

Kinds 3, 5, 6, 7, and 8 have roles in other geometry/update paths, but the evidence does not justify speculative editor-facing names for all of them.

## 23. Three compact world-record families

After the 10-byte lists, `LEVELS.DAT` stores three arrays of 11-byte compact records.

All three share:

```text
+0x00  uint16 type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact family-specific state
```

The common live expansion is 22 bytes and includes:

```text
+0x00  y_position_16_16
+0x04  x_position_16_16
+0x0C  cached_cell_index
+0x0E  type_id
+0x10  live state initialized to 0 in common expansion
```

[LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md)

The families share geometric scaffolding but are not interchangeable; their definition databases, state initialization, rendering source classes, and mutation rules differ.


## 24. Family A — NPC/creature world instances

Family A is counted by header `+0x28A` and directly indexes the 83-entry `NPCS.DAT` definition table.

Archive form:

```text
+0x00  uint16 npc_type_id
+0x02  int16  x_cell
+0x04  uint16 x_fraction
+0x06  int16  y_cell
+0x08  uint16 y_fraction
+0x0A  uint8  compact_orientation
```

Live/direct-save form:

```text
+0x00  uint32 y_position_16_16
+0x04  uint32 x_position_16_16
+0x08  uint32 runtime_flags
+0x0C  uint16 cached_cell_index
+0x0E  uint16 npc_type_id
+0x10  uint16 action_state_countdown
+0x12  int16  current_hp
+0x14  uint16 packed_runtime_state
```

The record is exactly `0x16` bytes. [Family A](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

### 24.1 Definition versus instance state

On archive load, `NPCS.DAT +0x35` supplies initial runtime flags. The engine copies them into live record `+0x08`, after which gameplay modifies the live copy.

Current HP at `+0x12` is likewise instantiated from NPC definition data and random/dice initialization, then persists as mutable actor state. `+0x10` is an action/state countdown that active behavior decrements. [Family A](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

The ownership chain is:

```text
NPCS.DAT
    immutable NPC type/template

LEVELS.DAT family A
    compact baseline placement/type/orientation

live family-A array
    mutable actor instance

LEVnnn.DAT family A
    serialized live actor instance
```

A direct LEV load must restore the live fields rather than rerolling HP or reinitializing position, countdown, flags, or facing.

### 24.2 Facing is embedded in packed live state

The compact orientation's low three bits become live bits 3..5:

```text
facing = (packed_runtime_state >> 3) & 7
```

Runtime turning logic rewrites those same bits. Other bits have separate runtime roles, so `+0x14` must remain a packed state word rather than being reduced to a facing enum. [Family A](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

## 25. Family B — placed object/item map records

Family B is counted by header `+0x28E` and expands into the 22-byte runtime array at `0x22238`.

The family is tied to the object/item and class-1 scene namespaces:

- the supplied archive contains 4,324 family-B records;
- all type IDs lie in `0..174`;
- the automap exporter treats them as placed object/item records and resolves names/types through the object database;
- the same `0..174` domain indexes the 175-entry class-1 scene descriptor table loaded from INIT. [LEVELS semantics](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/SEMANTICS.md) [RES3 source banks](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)

Level 22 is an important control fixture: applying the certified archive expansion to all 110 family-B compact records reproduces all 110 standalone records byte-for-byte. No family-B gameplay mutation occurred in the supplied saved session. [LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

The safe family name is **placed object/item map records**. It should not be merged with family C simply because both ultimately reference object/item definitions.

## 26. Family B scene representation is descriptor-driven

Family-B scene emission uses source class 1.

Its type ID indexes the 175 seven-byte descriptors stored in INIT:

```text
+0x00  uint16 local_shape_base
+0x02  uint16 spatial_extent_or_fixed_point_field
+0x04  uint8  shape_stride
+0x05  uint8  variant_count
+0x06  uint8  scene_influence_step
```

[RES3 source banks](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)

The engine constructs a class-1 bank-local shape from live variant state and the descriptor's stride, then the common scene helper adds RES3 source-class base `2169`.

Thus the family-B serialized type is not itself an absolute RES3 shape ID.

## 27. Family C — loose/pickable item world instances

Family C is counted by header `+0x28C` and expands to the live 22-byte array at `0x17618`.

Its type field indexes `OBJECTS.DAT`, but its runtime role differs sharply from family B:

- records can be inserted dynamically as loose/dropped objects;
- visible records emit scene source class 2;
- selected records can be picked up and removed;
- the same record form can carry thrown/moving-item state;
- the runtime family is capped at 300 records. [Family C](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_C.md)

The safe family name is **loose/pickable item world records**.

## 28. Family C runtime ceiling and ordered compaction

Loose-item insertion at `2195C..21ADA` appends a 22-byte record, computes its cell index, initializes live state, and increments the family count.

The encoded ceiling is:

```text
300 records = 0x12C
```

If the array is already full, the engine discards the oldest record by shifting the remaining 299 records down before appending the new record. [Family C](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_C.md)

Pickup/removal likewise compacts the array after deletion.

Therefore the family is an **ordered compact array**. A native port should not silently replace it with an unordered container if ordering can affect selection, rendering, or eviction behavior.

## 29. Family C also carries thrown/moving items

Routine `23B38` creates an active family-C instance with position, type, heading/direction state, a motion/lifetime word, and a live motion flag.

Routine `2388C` advances these instances, updates 16.16 position and cached cell, performs collision/contact handling, decrements motion state, and can clear active movement. [Family C](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_C.md)

The path includes type-specific handling for recognizable `OBJECTS.DAT` IDs:

```text
0x57  AXE OF HURLING
0x58  ARROW OF SLAY UNDEAD
0x59  DAGGER OF THROWING
0x5A  JAVELIN OF LIGHTNING
```

[Family C](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_C.md)

Thrown/projectile behavior is therefore runtime state layered onto the loose-item family, not a separate serialized projectile family.

## 30. Legacy family-C object-ID migration

The compact archive loader applies one explicit family-C-only migration:

```text
compact type 0x54 -> runtime type 0x58
```

In the supplied current `OBJECTS.DAT`:

```text
0x54  STONE GIANT GAUNTLETS
0x58  ARROW OF SLAY UNDEAD
```

[Family C](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_C.md)

The conversion occurs before ordinary runtime object lookups. A faithful archive loader must therefore preserve the migration exactly; interpreting compact `0x54` literally against the current object table would construct the wrong item.

The exact historical object-database revision that made this alias necessary remains unknown.

## 31. Runtime transient/moving-effect family

The engine also maintains a separate 22-byte live-world-record family for transient/moving effects.

Level event opcode 3 calls constructor `08A80`, which creates this record family. Scene emitter `2BFEC..2C22E` later walks the same runtime family and maps its type through the 12-entry class-3 INIT descriptor table. [EVENT_SCRIPTS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md) [RES3 source banks](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)

The safe classification is **transient/moving-effect scene family**.

It must remain distinct from family C: both can move, but they use different definition tables, source classes, constructors, and scene-emission paths.

## 32. Four scene-source classes connect world state to RES3

World-to-scene construction resolves four source classes:

```text
class 0  family A / NPC-creature
class 1  family B / INIT class-1 descriptor
class 2  family C / loose-item OBJECTS.DAT
class 3  transient/moving effect
```

INIT supplies RES3 bank bases:

```text
class 0       0
class 1    2169
class 2    3316
class 3    4158
```

and the common translation is:

```text
absolute_RES3_id =
    source_class_base[source_class]
  + bank_local_shape_id
```

[RES3 source banks](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Resources/RES/TYPE7_SOURCE_BANKS.md)

Those four bases partition all 4,234 RES3 entries exactly.

This is the architectural bridge between mutable world instances and the type-7 renderer documented in Component 04.

## 33. Scene identity must preserve local namespaces

The original engine stores local identities in several different definition spaces:

```text
NPCS.DAT
INIT class-1 descriptors
OBJECTS.DAT
INIT class-3 descriptors
```

Only during scene construction are these translated into absolute RES3 IDs.

A faithful engine/editor should therefore preserve:

```text
world family
type ID
live state/variant
source class
bank-local shape ID
absolute RES3 ID (derived)
```

Flattening stored shape identities into absolute RES3 IDs would destroy the original namespace design and make lossless round-trip editing unsafe.

## 34. Standalone LEV body stores expanded live state

The direct-save form serializes expanded runtime state:

```text
width * {
    int16 count
    count * 10-byte group-1 records
}

uint8 active_dynamic_animation_count

width * {
    int16 count
    count * 10-byte group-2 records
}

family A:
    live 22-byte actor records

family B:
    serialized count word
    expanded/live 22-byte placed-object records

family C:
    live 22-byte loose/moving-item records

fixed 0x514 event table
common tail
```

[LEVELS format](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FORMAT.md)

The archive and standalone forms therefore differ in both **representation** and **state ownership**.

## 35. LEV022 is direct evidence of mutable world persistence

Archive level 22 and supplied `LEV022.DAT` both describe:

```text
map           30 x 15 = 450 cells
family A       1
family B     110
family C       0
event table   0x514 bytes, all zero
annotations   0 bytes
```

[LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

Exactly 313 of 450 cell words differ, and every difference is:

```text
archive_word XOR standalone_word = 0x0440
```

Only current-visibility and persistent-exploration bits changed. Cell codes and every other cell flag remain identical.

## 36. LEV022 actor mutation

The single family-A record is NPC type 9, `ASSASSIN`.

Archive baseline:

```text
x cell   13
y cell    7
facing    2
```

Standalone live state:

```text
x cell                   18
y cell                    7
cached cell             228
action/state countdown    4
current HP               15
facing                    6
runtime flags    0x10000000
```

[Family A](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/FAMILY_A.md)

This is direct proof that the standalone level snapshot preserves movement, facing, current HP, and live action timing.

## 37. Unchanged Level-22 state is also diagnostic

The comparison proves that these subsystems did not mutate in the supplied session:

- all ten group-1 10-byte records are byte-identical;
- active dynamic-animation count is zero;
- group 2 is empty;
- all 110 family-B records equal deterministic archive expansion;
- family C is empty in both forms;
- the event table is byte-identical and all zero. [LEV022 mutable delta](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEV022.DAT/MUTABLE_DELTA.md)

This cleanly separates deterministic baseline expansion from actual play-session mutation.

## 38. The event table is level-local persistent state

After the three world-record families, both archive and standalone forms contain exactly:

```text
0x514 bytes = 50 slots * 26 bytes
```

Each slot is:

```text
+0x00  uint8 active_command_count
+0x01  command[0]   // 5 bytes
+0x06  command[1]
+0x0B  command[2]
+0x10  command[3]
+0x15  command[4]
```

and each command is:

```text
uint8 opcode
uint8 arg1
uint8 arg2
uint8 arg3
uint8 arg4
```

[EVENT_SCRIPTS.md](https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md)

Only the first `active_command_count` commands execute, but inactive command bytes remain persistent raw state. A lossless serializer must preserve all 26 bytes of every slot.
