# LEVELS.DAT 16-bit cell word

Status: **behavioral bit layout proven from certified CODE.2; gameplay names are used only where multiple independent paths support them**.

This document specializes `SEMANTICS.md`. The controlling storage is the row-major `map_width * map_height` word array loaded to DGROUP `0x198E0`.

## Executable evidence

Key CODE.2 object-1 ranges:

- `00454..00487`: replaces only the low six bits of a cell.
- `01558..01830`: rebuilds the final one-byte-per-cell map/display cache at DGROUP `0x1760`.
- `032C3B..0334A?`: first-person traversal/render paths inspect cell code/flags and mark reached cells.
- `0334C4..03350?`: clears the transient `0x0040` bit over the complete map.
- `171FC..17837`: standalone writer; calls `01558` immediately before serializing the final byte grid.

The immutable CODE.2 object remains the controlling source; punctuation above marks intentionally approximate routine end labels where only the relevant instruction cluster is needed.

## Proven field split

`00475` performs:

```text
cell.low_byte &= 0xC0
cell.word     |= (new_value & 0x003F)
```

Therefore bits `0..5` are an independently replaceable six-bit cell code. They are not flags.

```text
bits 0..5   0x003F  tile_code
bit  6      0x0040  current visibility/raycast mark
bit  7      0x0080  special-cell gate; exact gameplay identity unresolved
bit  8      0x0100  neighbor/substitution render gate
bit  9      0x0200  one-bit render attribute
bit 10      0x0400  automap explored/revealed
bit 11      0x0800  conditional pass-through/special-map behavior
bit 12      0x1000  per-cell lookup/render augmentation gate
bit 13      0x2000  unresolved
bits 14..15 0xC000  two-bit render selector
```

The names below describe directly observed behavior, not guessed level-editor terminology.

## `0x0040`: transient current-view mark

The first-person traversal code repeatedly ORs `0x0440` into every cell reached by its map/ray walk. At `334EC`, a map-wide loop executes:

```text
cell.low_byte &= 0xBF
```

for every cell, clearing `0x0040` and no other cell-word bit. Thus `0x0040` is a transient current render/visibility mark. Other world-record logic also tests this bit before processing records associated with a cell.

Across all 32 archived baseline levels, **0 of 115,584 cells** have `0x0040` preset. In the supplied `LEV022.DAT`, **313 of 450 cells** have it set.

## `0x0400`: persistent automap exploration mark

The same traversal operations that set current visibility set `0x0400` simultaneously through `OR 0x0440`. Unlike `0x0040`, `0x0400` is not cleared by the map-wide reset.

Routine `01558` uses high-byte bit `0x04` (`word 0x0400`) as the principal explored/unexplored test while constructing the byte map at `0x1760`. Unexplored cells begin as class `0xFF`; explored cells are assigned visible map classes according to their cell code and special-cell state. Unexplored solid cells may still receive a boundary class when adjacent to explored empty space, allowing the automap to outline explored territory.

This matches the official manual: the area map is based on party line-of-sight and only explored parts of a dungeon are shown. The manual also states that map objects reflect current state.

External corroboration:

- Official manual mirror: <https://www.bestoldgames.net/download/games/ravenloft/ravenloft-dos-manual.pdf>
- Daniel D'Agostino's Ravenloft-engine automap study: <https://swordsandsoftware.com/ravenloft-automaps.php>

The community study independently observes a grid-cell automap, an explicit unexplored state, and exported wall/floor/door/tree/furniture classes. Those observations agree with the executable architecture but are not used to assign binary offsets.

Archive-wide baseline frequency is only **3 / 115,584** cells for `0x0400`, consistent with almost all shipped maps beginning unexplored while permitting a few pre-revealed cells.

## Other directly observed flags

### `0x0080`

Selects special geometry/state handling in both automap classification and first-person rendering. `01558` calls the 10-byte cell-record lookup path when this bit is set. Exact gameplay identity remains unresolved.

### `0x0100`

In the renderer, enables an adjacent-cell substitution path. The neighboring cell's `0x0080` state and low-six-bit code can replace render inputs. Kept behaviorally named until the geometry represented by this pairing is proved.

### `0x0200`

Extracted as `(word & 0x0200) >> 9` into a one-bit render attribute before wall/surface rendering.

### `0x0800`

If a renderer-wide mode value at `FD08` is nonzero, a nonempty cell carrying `0x0800` can be traversed through the same continuation path used by empty cells. Other routines temporarily clear/restore the low-six-bit code on `0x0800` cells. This is clearly special mutable topology, but the gameplay label is not yet proved.

### `0x1000`

Causes a per-cell lookup (`1E9A0(cell_index)`) whose result augments rendering when no earlier override is active.

### `0xC000`

The renderer extracts `(word & 0xC000) >> 14` and passes the resulting two-bit value to its rendering helper. The two bits are therefore one two-bit selector, not safely nameable as independent gameplay flags.

`0x2000` has no separately proved direct meaning in the traced consumers and remains unresolved.

## Final byte-grid relationship

The common tail's `width * height` byte array is loaded at DGROUP `0x1760`. The standalone writer calls `01558` immediately before writing this array, proving that it is a **generated map/automap classification cache**, not a second independent topology layer.

`01558` emits observed internal classes including:

```text
0xFF  default/unexplored
0x01  generic visible/boundary class in ordinary non-special paths
0x02..0x04  special-cell classes derived through 10-byte record state
0x05  class selected by cell flag 0x0800
0x2E/0x2F  floor/current-party style class selected partly by header state
```

These are internal classification bytes. They must not be equated directly with the ASCII bytes written by the automap `SAVE` feature; the latter is a later rendering/export stage.

For archive level 22 the byte grid is `0xFF` for all 450 cells. In `LEV022.DAT` it is:

```text
0x2E : 313
0xFF : 130
0x01 :   7
```

The same standalone save has exactly **313 `0x0400` cells** and **313 `0x0040` cells**. Cell-by-cell, all 313 explored cells carry byte class `0x2E`; the remaining seven non-`0xFF` cells are boundary class `0x01`.

One header-table-dependent branch does not reconstruct those seven saved boundary cells from the serialized header bytes alone. CODE.2 also contains routines that mutate the table beginning at DGROUP `0x25622`; until the exact live-table transition at save time is closed, no stronger byte-for-byte cache regeneration claim is made.

## Archive-wide cell statistics

Across 115,584 archived cells:

```text
tile_code != 0   27,602
0x0040                 0
0x0080               745
0x0100             1,690
0x0200             4,957
0x0400                 3
0x0800                47
0x1000             1,650
0x2000                 3
0x4000                 0
0x8000                 2
```

These counts are evidence about the supplied data set, not format constraints.

## Safe implementation boundary

A faithful reader can now expose `tile_code`, `current_visible`, and `automap_explored` explicitly and preserve every remaining flag bit losslessly. Editor-facing names for `0x0080`, `0x0100`, `0x0200`, `0x0800`, `0x1000`, `0x2000`, and `0xC000` should remain behavioral until their associated render/interaction objects are identified.