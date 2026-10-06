# LEVELS.DAT automap classifier and export pipeline

Status: **internal cache generation and base ASCII export mapping proven from certified CODE.2**.

This document specializes `CELL_WORD.md`. The immutable CODE.2 object-1 bytes remain controlling evidence. Names such as `wall`, `floor`, `inset`, and `door` are used only where the executable export bytes and independently documented Ravenloft automap format support them. Exact gameplay identities remain unresolved where noted.

## Runtime data flow

The relevant runtime layers are:

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

If the current coordinates equal the party cell (`0x255D2` column, `0x255D6` row), the output is selected directly from byte `0x258A8`:

```text
0x258A8 == 0xFF  -> 0x2F
otherwise        -> 0x2E
```

### Explored cells (`word & 0x0400`)

For explored cells:

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

For cells without `0x0400`:

1. `code == 0` remains `0xFF`.
2. A nonzero code whose `0x25622[code] == 0x0093` takes the `0x2E/0x2F` path.
3. Otherwise the routine examines all eight neighboring cells that are in bounds.
4. If any neighbor has `0x0400` **and** raw `tile_code == 0`, the current cell becomes class `0x01`.
5. Otherwise it remains `0xFF`.

This proves the cache's boundary-reveal rule: an otherwise hidden non-floor cell may be exposed as a wall/boundary class when adjacent to explored empty floor.

## 10-byte-record state used by the classifier

`1E8C8(column,row)` searches both runtime record groups for a record whose byte `+4` equals `row`:

- first group at `0xCB3C[column]`;
- then second group at `0xCD3C[column]`.

`1E930(index)` converts the linear index back to row/column, searches the **second group only**, and returns:

```text
matching_record.byte[6] << 12
```

If there is no matching second-group record it returns `0x8000`.

`1E9A0(index)`, used by first-person rendering rather than `01558`, searches the **first group only** and returns:

```text
matching_record.byte[1] & 0x03
```

or zero if no record exists.

These routines establish that classes `0x02..0x04` are dynamic-record-dependent states rather than arbitrary static tile numbers.

## Connected-map resolver: `009B0..00B6B`

Before drawing or exporting a cache value below `0x20`, CODE.2 resolves it using the four orthogonal neighbors.

Two four-bit masks are constructed:

- one records neighboring cache values `< 0x20` (wall/special-map connectivity);
- one records neighboring floor classes `0x2E/0x2F`.

The current cache class then selects the result:

```text
class 0x01  generic connected wall/boundary result via table 0x5FF0
class 0x02  emits 0x29 or 0x2B for the two door alignments; otherwise wall fallback
class 0x03  emits 0x20..0x23 from floor-neighbor direction; otherwise wall/inset fallback
class 0x04  emits 0x28 or 0x2A for the two door alignments; otherwise wall fallback
class 0x05  emits 0x33 when opposite floor neighbors align; otherwise class-0x01 fallback
```

Classes `0x02` and `0x04` therefore represent two dynamic states of door-capable geometry. The executable proves that they differ; this pass does **not** assign which one is open versus closed.

Class `0x03` is the inset-producing special state.

Class `0x05` is the special hidden/secret-marker state in automap output. Its exact gameplay label is discussed conservatively below.

## Automap `SAVE` exporter: `026E8..02A33`

The exporter reads `0x1760`, calls `009B0` for cache classes below `0x20`, and converts resolved values `0x20..0x33` through a fixed byte table at `0x6011 + value`. If the resolver returns another value below `0x20`, exporter byte `0x6030` (`0xDB`) is used as the ordinary wall fallback.

The fixed conversion is:

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

The binary independently proves the byte conversion; the external source supplies the human-readable names.

Consequences:

- classes `0x02` and `0x04` are door-capable states because their aligned cases export as `0xCD`/`0xBA`;
- class `0x03` is an inset-capable state because it exports through `0xDF/DC/DD/DE`;
- class `0x05` exports as `0xB2` when it lies between opposite floor neighbors, supporting a secret-door/hidden-passage automap role;
- `0x2E/0x2F` export as floor (`0xFA`).

The official manual states that the automap reflects current object state and displays walls, insets, doors, floors, trap doors, rugs, plates, illusionary walls, the party, trees, creatures, NPCs, and furniture. That is consistent with the executable pipeline but does not by itself identify every internal class.

Official manual mirror: <https://www.bestoldgames.net/download/games/ravenloft/ravenloft-dos-manual.pdf>

## Export overlays after the base map

The exporter builds multiple row buffers and overlays additional world/object information after the base cache conversion. One proven overlay walks cell-associated runtime records via `00700`; records whose type word is in `0x3C..0x41` emit ASCII `'*'`, while other records on that path emit `0xFE`.

Those overlay categories are intentionally not assigned creature/furniture names here without closing the associated type table.

## Header lookup tables used by rendering

The 0x2A4 header begins at runtime `0x25618`. Five consecutive 64-word tables are therefore now structurally and behaviorally identifiable:

```text
header +0x00A  DGROUP 0x25622  primary resource lookup A[64]
header +0x08A  DGROUP 0x256A2  alternate resource lookup A[64]
header +0x10A  DGROUP 0x25722  primary resource lookup B[64]
header +0x18A  DGROUP 0x257A2  alternate resource lookup B[64]
header +0x20A  DGROUP 0x25822  automap-render style/control word[64]
```

The first-person renderer indexes the primary/alternate A/B tables by the six-bit cell code. Bit `0x0100` can substitute an adjacent `0x0080` cell's code into that lookup. Bit `0x1000` can add the low two state bits returned by `1E9A0` when that neighbor substitution did not occur.

The automap drawing routine `00B6C` loads the low and high bytes of `0x25822[tile_code]` into renderer globals before drawing a cell, then restores defaults `0xB1/0xB5`.

Across **all 32 supplied archive records, all 2,048 entries** of the `0x25822` table are exactly `0xB5B1`. This is a data-set observation, not a format constraint.

## Primary-table runtime normalization: `1EDAC`

`1EDAC` walks the primary resource tables at `0x25622` and `0x25722`. For each word it searches ten `(old,new)` pairs stored at data-object offset `0xBFBC`:

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

For A-table values `0x242..0x244` and `0x245..0x247`, it also derives the next member of the corresponding three-value cycle into the alternate table `0x256A2`. A later conditional path can replace alternate value `0x69` with `0x8E` for both A and B planes.

Importantly, this normalization table does **not** rewrite resource `0x0093`.

## `LEV022.DAT` cache-replay boundary

Archive level 22 and the supplied `LEV022.DAT` remain a strong validation pair for storage layout, record expansion, and persistent exploration state, but the serialized LEV cache is **not** a byte-for-byte replay oracle for this exact CODE.2 `01558` routine.

The direct file contains:

```text
0x25622[1] = 0x0093
```

and its 450 cell words divide as:

```text
code 1, unexplored : 40 cells  (33 cache 0xFF, 7 cache 0x01)
code 1, explored   : 46 cells  (cache 0x2E)
code 0, unexplored : 97 cells  (cache 0xFF)
code 0, explored   : 267 cells (cache 0x2E)
```

Exact CODE.2 `01558` sends **every nonzero code whose primary resource is `0x0093`** through the `0x2E/0x2F` floor-class path, whether or not `0x0400` is set. Replaying the serialized header and word layer would therefore classify those 40 unexplored code-1 cells differently from the bytes stored in the supplied file.

The writer serializes the same header region and cell-word array before calling `01558`; `1EDAC` does not rewrite `0x0093`. No speculative repair is made. The exact provenance/runtime-build reason for this 40-cell discrepancy remains unresolved.

What the pair still proves independently:

- the six-bit cell codes are unchanged in all 450 cells;
- exactly 313 cells gain `0x0040` and the same 313 gain `0x0400`;
- the direct cache contains 313 `0x2E`, 130 `0xFF`, and 7 `0x01` bytes;
- all 110 family-B compact/direct records match after the proven expansion.

## Safe implementation boundary

A faithful implementation can now:

- regenerate the CODE.2 automap cache according to `01558` from live runtime state;
- resolve cache classes through `009B0`;
- export the proved base CP437 map bytes through the `026E8` conversion table;
- preserve both dynamic 10-byte record groups and use their proved row/state accesses;
- expose the five 64-word header lookup tables losslessly.

It should **not** yet:

- assume class `0x02` or `0x04` is specifically open or closed without further state tracing;
- equate `0x0800` exclusively with a named gameplay feature outside its proven pass-through and `0xB2` automap behavior;
- force `LEV022.DAT`'s stored cache to equal an exact CODE.2 replay until its provenance discrepancy is resolved;
- assign names to exporter values `0xE9`, `0xB0`, or `0x09` without tracing their overlay use.