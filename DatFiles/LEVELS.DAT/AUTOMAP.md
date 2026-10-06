# LEVELS.DAT automap classifier and export pipeline

Status: **internal cache generation, dynamic door-state relationship, and base ASCII export mapping proven from certified CODE.2**.

This document specializes `CELL_WORD.md`. The immutable CODE.2 object-1 bytes remain controlling evidence. Names such as `wall`, `floor`, `inset`, and `door` are used only where executable behavior and independently documented Ravenloft automap bytes support them.

## Runtime data flow

```text
DGROUP 0x198E0  width*height uint16 packed cell words
DGROUP 0x1760   width*height uint8 generated automap/display cache
DGROUP 0x12756  map width used by automap renderer/exporter
DGROUP 0x12754  map height used by automap renderer/exporter
DGROUP 0x25622  64-word primary cell-code resource lookup
DGROUP 0xCB3C   first width-indexed 10-byte record pointer group
DGROUP 0xCD3C   second width-indexed 10-byte record pointer group
```

The standalone level writer `171FC..17837` serializes the 16-bit cell words, later calls `01558`, and then serializes the byte cache. Thus the byte layer is generated state, not an independent topology grid.

## Exact cache classifier: `01558..01830`

The routine walks every `(column,row)` and forms:

```text
index = row * width + column
word  = cell_words[index]
code  = word & 0x003F
```

The default output is `0xFF`.

### Party-cell override

If the current coordinates equal the party cell (`0x255D2` column, `0x255D6` row), byte `0x258A8` selects:

```text
0x258A8 == 0xFF  -> 0x2F
otherwise        -> 0x2E
```

### Explored cells (`word & 0x0400`)

1. `code == 0` -> `0x2E/0x2F` floor-class path.
2. Otherwise read `resource = int16(0x25622[code])`.
3. `resource == 0x0093` -> `0x2E/0x2F` floor-class path.
4. `word & 0x0800` -> class `0x05`.
5. If `word & 0x0080` is clear -> class `0x01`.
6. If `0x0080` is set, call `1E8C8(column,row)`:
   - no 10-byte record -> class `0x03`;
   - record exists -> call `1E930(index)`;
     - result `== 0x8000` -> class `0x04`;
     - any other result -> class `0x02`.

### Unexplored cells

1. `code == 0` remains `0xFF`.
2. A nonzero code whose `0x25622[code] == 0x0093` takes the `0x2E/0x2F` path.
3. Otherwise all eight in-bounds neighbors are examined.
4. If any neighbor has `0x0400` and raw `tile_code == 0`, the current cell becomes class `0x01`.
5. Otherwise it remains `0xFF`.

This proves the cache's boundary-reveal rule.

## 10-byte-record state used by the classifier

`1E8C8(column,row)` searches both runtime record groups for a record whose byte `+4` equals `row`:

- first group at `0xCB3C[column]`;
- then second group at `0xCD3C[column]`.

`1E930(index)` searches the second group only and returns:

```text
matching_record.byte[6] << 12
```

or `0x8000` when no matching second-group record exists.

`1E9A0(index)`, used by first-person rendering, searches the first group only and returns:

```text
matching_record.byte[1] & 0x03
```

or zero if no record exists.

## Dynamic door state closed

CODE.2 `20B40..20C0B` proves that second-group record bytes `+6/+7` form an animation state:

```text
+6  uint8 position, persistent endpoints 0 and 8
+7  int8  delta, normally -1 / 0 / +1
```

The update loop adds byte `+7` into byte `+6` until the value reaches endpoint 0 or 8, then clears the delta. When no animations remain it normalizes every non-8 value to zero.

The interaction path `20C0C..20CF2` toggles low-two-bit mode 1 as follows:

```text
position 0 -> delta +1       # toward 8
nonzero   -> position 8,
             delta -1       # toward 0
```

Low-two-bit mode 2 sets delta `-1` only.

Most importantly, traversal/collision code at `0B164..0B18D` calls `1E930` for `0x0080` cells and treats return value zero as nonblocking while any nonzero result is blocking. Therefore:

```text
record[6] == 0    open / passable endpoint
record[6] == 8    closed / blocking endpoint
record[6] 1..7    in motion, still blocking on this traversal path
```

The automap relationship is therefore:

```text
cache class 0x04  fully closed endpoint when a group-2 record exists
cache class 0x02  open endpoint or transitional position 0..7
```

A missing group-2 record also makes `1E930` return `0x8000`, so class `0x04` is not globally synonymous with "closed door" without that qualification.

Across all supplied archive levels, 324 legacy records are diverted into runtime group 2. Of those, **323** start with byte `+6 == 8`, byte `+7 == 0`, and low-two-bit mode 1; one exceptional record starts at zero with mode 0. This archive-wide pattern strongly corroborates the shipped closed/stationary initialization without making it a format constraint.

See `DYNAMIC_CELL_RECORDS.md` and `analyze_dynamic_records.py` for the complete evidence and counts.

## Connected-map resolver: `009B0..00B6B`

Before drawing or exporting a cache value below `0x20`, CODE.2 resolves it using the four orthogonal neighbors. It constructs one mask for neighboring cache values `< 0x20` and another for neighboring floor classes `0x2E/0x2F`.

```text
class 0x01  generic connected wall/boundary via table 0x5FF0
class 0x02  emits 0x29 or 0x2B for the two door alignments; otherwise wall fallback
class 0x03  emits 0x20..0x23 from floor-neighbor direction; otherwise wall/inset fallback
class 0x04  emits 0x28 or 0x2A for the two door alignments; otherwise wall fallback
class 0x05  emits 0x33 when opposite floor neighbors align; otherwise class-0x01 fallback
```

Class `0x03` is the inset-producing special state. Class `0x05` is the hidden/secret-marker state in automap output.

The class-2/class-4 internal distinction records dynamic state, but both aligned variants export to the same horizontal or vertical CP437 door glyph, so the text export does not preserve open/closed state.

## Automap `SAVE` exporter: `026E8..02A33`

The exporter reads `0x1760`, calls `009B0` for cache classes below `0x20`, and converts resolved values `0x20..0x33` through a fixed byte table at `0x6011 + value`. If the resolver returns another value below `0x20`, exporter byte `0x6030` (`0xDB`) is used as the ordinary wall fallback.

```text
resolved  exported byte
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

The published Ravenloft automap study independently identifies the relevant CP437 bytes as:

```text
0xDB wall
0xDC bottom inset
0xDD left inset
0xDE right inset
0xDF top inset
0xFA floor
0xB2 secret door
0xBA vertical door
0xCD horizontal door
```

Source: <https://www.swordsandsoftware.com/ravenloft-automaps.php>

The binary proves the byte conversion; the external source supplies the human-readable names.

The official manual states that the automap reflects current object state and displays walls, insets, doors, floors, trap doors, rugs, plates, illusionary walls, the party, trees, creatures, NPCs, and furniture. That is consistent with the executable pipeline but does not identify every internal overlay class.

Official manual mirror: <https://www.bestoldgames.net/download/games/ravenloft/ravenloft-dos-manual.pdf>

## Export overlays after the base map

The exporter builds multiple row buffers and overlays additional world/object information after the base cache conversion. One proven overlay walks cell-associated runtime records via `00700`; records whose type word is in `0x3C..0x41` emit ASCII `'*'`, while other records on that path emit `0xFE`.

Those overlay categories remain unnamed until the associated type table is closed.

## Header lookup tables used by rendering

The `0x2A4` header begins at runtime `0x25618`. Five consecutive 64-word tables are behaviorally identified:

```text
header +0x00A  DGROUP 0x25622  primary resource lookup A[64]
header +0x08A  DGROUP 0x256A2  alternate resource lookup A[64]
header +0x10A  DGROUP 0x25722  primary resource lookup B[64]
header +0x18A  DGROUP 0x257A2  alternate resource lookup B[64]
header +0x20A  DGROUP 0x25822  automap-render style/control word[64]
```

The first-person renderer indexes the primary/alternate A/B tables by the six-bit cell code. Bit `0x0100` can substitute an adjacent `0x0080` cell's code into that lookup. Bit `0x1000` can add the low two state bits returned by `1E9A0` when that neighbor substitution did not occur.

The automap drawing routine `00B6C` loads the low and high bytes of `0x25822[tile_code]` into renderer globals before drawing a cell, then restores defaults `0xB1/0xB5`.

Across all 32 supplied archive records, all **2,048** entries of `0x25822` are exactly `0xB5B1`. This is a data-set observation, not a format constraint.

## Primary-table runtime normalization: `1EDAC`

`1EDAC` walks the primary resource tables at `0x25622` and `0x25722`. It applies these ten `(old,new)` pairs from object-2 offset `0xBFBC`:

```text
0x15C -> 0x15D
0x15D -> 0x15E
0x15E -> 0x15C
0x0AA -> 0x0AB
0x0AB -> 0x0AA
0x11B -> 0x11C
0x11C -> 0x11D
0x11D -> 0x11B
0x101 -> 0x102
0x102 -> 0x101
```

For A-table values `0x242..0x244` and `0x245..0x247`, it also derives the next member of the corresponding three-value cycle into alternate table `0x256A2`. A later conditional path can replace alternate value `0x69` with `0x8E` for both A and B planes.

This normalization does **not** rewrite resource `0x0093`.

## `LEV022.DAT` cache-replay boundary

Archive level 22 and the supplied `LEV022.DAT` remain a strong validation pair for storage layout, record expansion, and exploration state, but the serialized LEV cache is not a byte-for-byte replay oracle for this exact CODE.2 `01558` routine.

The direct file contains `0x25622[1] = 0x0093`, while its 450 cell words divide as:

```text
code 1, unexplored : 40 cells  (33 cache 0xFF, 7 cache 0x01)
code 1, explored   : 46 cells  (cache 0x2E)
code 0, unexplored : 97 cells  (cache 0xFF)
code 0, explored   : 267 cells (cache 0x2E)
```

Exact `01558` sends every nonzero code whose primary resource is `0x0093` through the `0x2E/0x2F` path regardless of exploration. Replaying the serialized state therefore differs for exactly the 40 unexplored code-1 cells. The writer serializes the header and cell-word array before calling `01558`; `1EDAC` does not rewrite `0x0093`. No speculative repair is made.

See `CACHE_REBUILD_AUDIT.md` and `audit_cache_rebuild.py`.

## Safe implementation boundary

A faithful implementation can now:

- regenerate the CODE.2 cache according to `01558` from complete live state;
- distinguish dynamic group-2 endpoint 0 as open/passable and endpoint 8 as closed/blocking;
- interpret class `0x04` as the fully closed group-2 endpoint when such a record exists, and class `0x02` as the open/transitional branch;
- resolve cache classes through `009B0`;
- export the proved base CP437 map bytes through the `026E8` conversion table;
- preserve both 10-byte record groups and all five 64-word header lookup tables losslessly.

Still unresolved:

- the stronger gameplay meaning of group-2 byte-1 mode 2 and eligibility bits `0x04/0x40`;
- the exact gameplay identity of `0x0800` outside its proved pass-through and `0xB2` map behavior;
- exporter overlay types represented by `0xE9`, `0xB0`, `0x09`, `'*'`, and `0xFE`;
- the provenance reason for the 40-cell `LEV022.DAT` cache discrepancy.