# LEVELS.DAT automap classifier and export pipeline

Status: **cache generation, dynamic door-state relationship, connected-map resolution, base CP437 export, placed-object overlay, and annotation overlay are reconstructed from certified CODE.2.**

The immutable CODE.2 object remains controlling evidence. Human-readable wall/floor/inset/door names are used where executable behavior and the independently documented Ravenloft automap byte format agree.

## Runtime data flow

```text
0x198E0  width*height uint16 packed cell words
0x1760   width*height uint8 generated automap/display cache
0x12756  automap width
0x12754  automap height
0x25622  primary cell-code resource lookup[64]
0xCB3C   first 10-byte record pointer group
0xCD3C   second 10-byte record pointer group
0x5760   variable automap annotation list
```

The standalone writer serializes cell words, calls `01558`, then serializes `0x1760`, proving the byte grid is generated state rather than independent topology.

## Cache classifier `01558..01830`

For each cell:

```text
index = row * width + column
word  = cell_words[index]
code  = word & 0x003F
default output = 0xFF
```

Party-cell override, with the internal class variant selected by the level environment/render profile:

```text
if row == high16(player_coord_B) && column == high16(player_coord_A):
    header +0x290 / DGROUP 0x258A8 == 0xFF -> 0x2F
    otherwise                              -> 0x2E
```

The direct CODE.2 sequence at `015AA..015E3` proves the coordinate comparison before the profile-byte test. Both `0x2E` and `0x2F` convert to export byte `0xFA`, so the text-file export intentionally collapses the party-cell distinction to the ordinary floor glyph.

Explored cells (`word & 0x0400`):

1. code zero -> `0x2E/0x2F`;
2. nonzero `0x25622[code] == 0x0093` -> `0x2E/0x2F`;
3. `0x0800` -> `0x05`;
4. no `0x0080` -> `0x01`;
5. `0x0080` with no 10-byte record -> `0x03`;
6. with record, `1E930(index) == 0x8000` -> `0x04`, otherwise `0x02`.

Unexplored cells remain `0xFF` except the encoded `0x0093` floor shortcut and ordinary nonzero cells adjacent in the eight-neighbor set to an explored raw-code-zero cell, which become boundary class `0x01`.

## Dynamic special-cell records

`1E930` searches runtime group 2 and returns `record[6] << 12`, or `0x8000` when no matching group-2 record exists.

CODE.2 proves for real group-2 records:

```text
record +6 == 0    open / passable
record +6 == 8    closed / blocking
record +6 1..7    transitional and blocking on the traced path
record +7         signed animation delta
```

Therefore, when a group-2 record exists:

```text
cache 0x04  fully closed endpoint
cache 0x02  open or transitional state
```

No matching group-2 record also yields cache `0x04`, so `0x04` is not globally synonymous with `closed door`.

See `DYNAMIC_CELL_RECORDS.md`.

## Connected-map resolver `009B0..00B6B`

For cache classes below `0x20`, the resolver builds orthogonal-neighbor masks for special/wall connectivity and floor adjacency.

```text
0x01  generic connected wall/boundary via table 0x5FF0
0x02  0x29/0x2B for aligned door geometry, otherwise wall fallback
0x03  0x20..0x23 inset result from floor-neighbor direction
0x04  0x28/0x2A for aligned door geometry, otherwise wall fallback
0x05  0x33 with opposite floor neighbors, otherwise class-0x01 fallback
```

Class `0x03` is inset-capable. Class `0x05` supports the hidden/secret-marker automap path.

## Base SAVE conversion `026E8..02A33`

Resolved values `0x20..0x33` are converted through the fixed table at `0x6011 + value`; other unresolved values below `0x20` use fallback byte `0xDB`.

```text
resolved  output
0x20      0xDF
0x21      0xDC
0x22      0xDD
0x23      0xDE
0x24      0xDF
0x25      0xDC
0x26      0xDD
0x27      0xDE
0x28      0xCD
0x29      0xCD
0x2A      0xBA
0x2B      0xBA
0x2C      0xFA
0x2D      0xFA
0x2E      0xFA
0x2F      0xFA
0x30      0xE9
0x31      0xB0
0x32      0x09
0x33      0xB2
fallback  0xDB
```

Daniel D'Agostino's Ravenloft-engine automap study independently identifies the relevant CP437 bytes as `0xDB` wall, `0xDC/0xDD/0xDE/0xDF` directional insets, `0xFA` floor, `0xB2` secret door, `0xBA` vertical door, and `0xCD` horizontal door:

<https://www.swordsandsoftware.com/ravenloft-automaps.php>

The binary proves the conversion table; the external study supplies those human-readable glyph names.

The official manual states that the automap is line-of-sight/exploration based and displays current map/object information. That is consistent with the reconstructed pipeline but is not used to assign binary offsets.


## Automap SAVE filename and PRINT path

A later direct CODE.2 xref pass closes two items that were previously community-corrobated rather than executable-proved.

Object-2/DGROUP offset `0x5FDA` contains the literal mutable template:

```text
AUTOMAPS\\AUTO???.MAP
```

Physical file location in the supplied CODE.2 is `0x7BEFE`.

Routine `02A34` receives the current level/map value, patches the three `?` bytes at DGROUP `0x5FE7..0x5FE9`, removes/recreates/opens that path through the runtime file wrappers, then calls exporter `026E8` in merged/file-output mode and closes the resulting handle. The placeholder writes are instruction-proven and should be reproduced exactly rather than replaced by an assumed formatting helper.

A second direct caller at `03321` invokes `026E8` in stream/non-merged mode with fixed FILE-like object pointer DGROUP `0xF764`. The five consecutive runtime FILE objects at `0xF6F4 + n*0x1C` map this pointer to the fifth DOS standard stream. Watcom's DOS C library documents the startup standard-stream order as `stdin`, `stdout`, `stderr`, `stdaux`, `stdprn`; therefore `0xF764` is the **standard printer stream (`stdprn`)**.

This closes the executable-side Save/Print architecture:

```text
SAVE automap
    -> patch AUTOMAPS\\AUTO???.MAP template
    -> create/open file
    -> 026E8 merged map output
    -> annotation legend

PRINT automap
    -> 026E8 stdprn stream output
    -> three-layer textual rows
    -> annotation legend
```

Official Watcom standard-stream documentation:
<https://openwatcom.org/ftp/manuals/1.5/clib.pdf>

Daniel D'Agostino's independent Ravenloft automap study reports the same user-facing Save/Print intent and DOS/IBM-character-set rendering:
<https://www.swordsandsoftware.com/ravenloft-automaps.php>

## Export overlays

The exporter builds three map buffers:

1. base glyphs from cache `0x1760`;
2. placed-record/object symbols;
3. automap annotation labels.

In merged output, non-space buffer-2 bytes replace the base, then non-space annotation bytes replace buffer 2. Annotation labels therefore have highest precedence.

### Placed object/item overlay

For each visible/non-`0xFF` cell, exporter `026E8` repeatedly calls `00700`, which in the active level path scans family B (`0x22238`, count `0x258A6`). It reads each matching record's runtime type word `+0x0E` and emits:

```text
type 0x3C..0x41 -> '*' (0x2A)
other type       -> 0xFE
```

CODE.2 `168C8` loads `OBJECTS.DAT` as `count * 0x41` records. The supplied entries `0x3C..0x41` are:

```text
POTION OF EXTRA HEALING
POTION OF FIRE RESIST
POTION OF GIANT STRENGTH
POTION OF HEALING
POTION OF SPEED
OIL OF FIERY BURNING
```

Family-B IDs all lie inside the supplied 336-entry object database and other family-B consumers use IDs corresponding to concrete keys/items (for example `0x9B`, BLOOD KEY). This supports family B as a placed object/item record family. Literal `0xFE` remains unnamed until its final font/display semantics are needed.

### Annotation-label overlay

Routine `23B8` walks entries at `0x5760`:

```text
int16 raw_x
int16 raw_y
int16 payload_length
uint8 payload[payload_length]
```

It maps coordinates using signed truncation toward zero:

```text
map_x = raw_x / 8
map_y = raw_y / 5
```

and places the entry's sequential label into the third map buffer. Labels begin `A` through `Z`; later two-letter labels use the exact encoded increment ordering. Routine `2474` walks the same list and emits the matching `label: payload` legend.

The final level tail is therefore the serialized automap annotation list, not generic text. See `AUTOMAP_OVERLAYS.md`.

## Header lookup banks

The `0x2A4` header contains five consecutive 64-word tables:

```text
+0x00A  0x25622 primary resource A[64]
+0x08A  0x256A2 alternate resource A[64]
+0x10A  0x25722 primary resource B[64]
+0x18A  0x257A2 alternate resource B[64]
+0x20A  0x25822 automap style/control[64]
```

First-person rendering indexes the first four by six-bit cell code. Bit `0x0100` can substitute an adjacent `0x0080` cell's code; bit `0x1000` can add the low two state bits returned by `1E9A0` when no earlier substitution occurred.

Automap drawing loads low/high bytes of `0x25822[tile_code]` into renderer globals and then restores defaults `0xB1/0xB5`. All 2,048 fifth-table entries in the supplied archive are `0xB5B1`; that is an observation, not a format rule.

## Primary-table animation `1EDAC`

`1EDAC` mutates primary resource tables with cycles including:

```text
0x15C -> 0x15D -> 0x15E -> 0x15C
0x0AA -> 0x0AB -> 0x0AA
0x11B -> 0x11C -> 0x11D -> 0x11B
0x101 -> 0x102 -> 0x101
```

It also derives companion values for `0x242..0x247` cycles. It does not rewrite `0x0093`.

## LEV022 cache-replay boundary

The supplied `LEV022.DAT` is structurally valid but its saved cache cannot be reproduced by a single exact execution of certified `01558` using the same file's serialized header/cell state. Exactly 40 unexplored code-1 cells conflict with the encoded `0x25622[1] == 0x0093` shortcut; all other deterministic ordinary-cell checks agree.

This is retained as a provenance/runtime-state discrepancy rather than patched by speculation. See `CACHE_REBUILD_AUDIT.md` and `audit_cache_rebuild.py`.

## Safe implementation boundary

A faithful implementation may now reproduce:

- live cache generation from complete runtime state;
- connected wall/inset/door resolution;
- base CP437 map conversion;
- dynamic open/closed group-2 behavior;
- family-B placed-object overlay and its `'*'` range;
- annotation labels, legend, and overlay precedence;
- the five 64-word header tables losslessly.

Still unresolved include the precise gameplay meaning of group-2 mode 2, the broad semantic name for cell flag `0x0800`, literal exporter values `0xE9`, `0xB0`, `0x09`, and `0xFE`, and the provenance reason for the 40-cell `LEV022.DAT` discrepancy.