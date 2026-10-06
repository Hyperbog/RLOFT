# LEVELS.DAT 16-bit cell word

Status: **behavioral bit layout closed to the supplied executable's evidence boundary. Gameplay names are used only where multiple independent paths support them; `0x2000` is preserved as a corpus-populated authoring/legacy marker with no direct runtime consumer or producer found in the supplied CODE.2.**

This document specializes `SEMANTICS.md`. The controlling storage is the row-major `map_width * map_height` word array loaded to DGROUP `0x198E0`.

## Executable evidence

Key CODE.2 object-1 ranges:

- `00454..00487`: replaces only the low six bits of a cell.
- `01558..01830`: rebuilds the final one-byte-per-cell map/display cache at DGROUP `0x1760`.
- `1E930`: dynamic special-cell collision state from the second 10-byte record group.
- `1E9A0`: first-group 10-byte record low-two-bit state lookup.
- `32F8C` / `33124`: first-person resource selection, special-neighbor substitution, state offsets, and two-bit decrement extraction.
- `3263C`: applies the two-bit decrement to both byte lanes of a packed render value with zero clamping.
- `335DC..336F2`: additional render pass for records carrying the one-bit prepass field.
- `336F7..`: ordinary all-record render pass.
- `334EC`: clears the transient `0x0040` bit over the complete map.
- `171FC..17837`: standalone writer; calls `01558` immediately before serializing the final byte grid.

The immutable CODE.2 object remains the controlling source.

## Proven field split

`00475` performs:

```text
cell.low_byte &= 0xC0
cell.word     |= (new_value & 0x003F)
```

Therefore bits `0..5` are an independently replaceable six-bit cell code. They are not flags.

The current safe model is:

```text
bits 0..5   0x003F  six-bit cell code
bit  6      0x0040  transient current visibility/raycast mark
bit  7      0x0080  special-geometry center / dynamic-collision gate
bit  8      0x0100  special-geometry frame / adjacent-code substitution gate
bit  9      0x0200  extra render-prepass enable
bit 10      0x0400  persistent automap explored/revealed mark
bit 11      0x0800  secret/hidden-passage topology marker
bit 12      0x1000  10-byte-record state resource-offset enable
bit 13      0x2000  preserved authoring/legacy marker; no direct runtime consumer proved
bits 14..15 0xC000  two-bit render shade/decrement level
```

The names below describe directly observed behavior. Where an external automap-format name is used, the distinction from executable-only proof is stated explicitly.

## `0x0040`: transient current-view mark

The first-person traversal code repeatedly ORs `0x0440` into every cell reached by its map/ray walk. At `334EC`, a map-wide loop executes:

```text
cell.low_byte &= 0xBF
```

for every cell, clearing `0x0040` and no other cell-word bit. Thus `0x0040` is a transient current render/visibility mark. Other world-record logic also tests this bit before processing records associated with a cell.

Across all 32 archived baseline levels, **0 of 115,584 cells** have `0x0040` preset. In the supplied `LEV022.DAT`, **313 of 450 cells** have it set.

## `0x0400`: persistent automap exploration mark

The same traversal operations that set current visibility set `0x0400` simultaneously through `OR 0x0440`. Unlike `0x0040`, `0x0400` is not cleared by the map-wide reset.

Routine `01558` uses high-byte bit `0x04` (`word 0x0400`) as the principal explored/unexplored test while constructing the byte map at `0x1760`. Unexplored cells begin as class `0xFF`; explored cells are assigned visible map classes according to cell code and special-cell state. Unexplored solid cells may still receive a boundary class when adjacent to explored empty space.

This matches the official manual's line-of-sight/exploration description. External corroboration is recorded in `AUTOMAP.md`; it is not used to assign binary offsets.

Archive-wide baseline frequency is only **3 / 115,584** cells for `0x0400`, consistent with almost all shipped maps beginning unexplored while permitting a few pre-revealed cells.

## `0x0080`: special-geometry center / dynamic-collision gate

Movement/collision logic tests `0x0080` before the ordinary low-six-bit blocking test. If it is set, the engine calls `1E930(cell_index)`.

`1E930` searches the second 10-byte record group. A matching kind-1 record returns `record[6] << 12`; no matching record returns `0x8000`. Since kind-1 position zero is independently proved open/passable and nonzero positions block on this path, `0x0080` is a special/dynamic collision-geometry gate.

It is not door-only. Among all 745 supplied `0x0080` cells:

```text
313  contain kind-1 dynamic records
149  contain kind-8 records
283  contain no 10-byte record
```

Automap classification likewise routes `0x0080` cells into special inset/door handling.

## `0x0100`: special-geometry frame / adjacent-code substitution

First-person renderer paths `32F8C` and `33124` inspect `0x0100`. When it is set, the renderer computes a direction-dependent neighboring cell. If that neighbor carries `0x0080`, the renderer substitutes that neighbor's low-six-bit code as the resource-code input and selects a `+1` resource variant.

Archive geometry strongly validates this center/frame relationship:

```text
0x0100 cells = 1,690
  orthogonal neighboring 0x0080 centers:
    0 ->    8
    1 -> 1,520
    2 ->  161
    3 ->    1

0x0080 cells = 745
  orthogonal neighboring 0x0100 frame cells:
    2 -> 391
    3 -> 353
    4 ->   1
```

Every supplied `0x0080` center has at least two `0x0100` frame neighbors. Only eight of 1,690 frame cells lack an adjacent `0x0080` center.

## `0x0200`: extra render-prepass enable

The renderer extracts:

```text
(cell & 0x0200) >> 9
```

and stores it in a one-bit generated render-record field. Routine `335DC` begins by scanning the render-record list and processing only records whose field equals one; at `336F7` the engine begins the ordinary all-record pass.

The safe interpretation is therefore **membership in an additional preliminary render pass**. The visual purpose of that pass is not named more strongly.

The supplied archive contains 4,957 cells with `0x0200`.

## `0x0800`: secret/hidden-passage topology marker

CODE.2 directly proves special pass-through and map behavior:

- when renderer-wide mode `FD08` is nonzero, a nonempty `0x0800` cell can continue through the same ray/traversal path used by an empty cell;
- other routines temporarily clear/restore the low-six-bit cell code on `0x0800` cells;
- automap rebuild assigns explored `0x0800` cells class `0x05`;
- the connected resolver can convert class `0x05` to resolved class `0x33` when the required opposite-floor geometry is present.

The SAVE exporter maps resolved `0x33` to CP437 byte `0xB2`. Independent Ravenloft automap-format documentation identifies that glyph as the secret-door marker. Accordingly, the binary proves special hidden/pass-through topology, while the literal `secret door` display name is externally corroborated.

All 47 supplied `0x0800` cells have nonzero low-six-bit codes and **none has a 10-byte record**.

The project-level behavioral name is therefore `secret/hidden-passage topology marker`.

## `0x1000`: 10-byte-record state resource-offset enable

Renderer paths `32F8C` and `33124` test `0x1000`. If no earlier adjacent-cell substitution has been selected they call `1E9A0(cell_index)`.

`1E9A0` searches the first runtime 10-byte record group at `0xCB3C`, finds the record for the cell, and returns:

```text
record[1] & 0x03
```

The renderer adds this `0..3` value to both resource IDs selected for the cell. Thus `0x1000` explicitly enables **resource/visual variants derived from the associated 10-byte record's low-two-bit state**.

Archive-wide correlation is exact for every non-kind-1 record:

```text
non-kind-1 / group-1 records: 1,335
on 0x1000 cells:               1,335 / 1,335

kind 0 : 358 / 358
kind 2 : 175 / 175
kind 3 : 381 / 381
kind 4 :  43 / 43
kind 5 :   1 / 1
kind 6 :  55 / 55
kind 7 :   2 / 2
kind 8 : 311 / 311
kind 10:   9 / 9
```

Kind-1 dynamic records are different: 313 / 324 occur on `0x1000` cells. The eleven 10-byte records lacking `0x1000` are all kind 1 in levels 12..14. Only two `0x1000` cells in the complete archive have no 10-byte record; both are level-15 boundary cells with six-bit code 13.

## `0xC000`: two-bit render shade/decrement level

The renderer extracts:

```text
shade = (cell & 0xC000) >> 14
```

and stores the `0..3` result in generated render-record field `+0x18`. Draw routine `3263C` loads the value, duplicates its low byte into both byte lanes of a packed 16-bit render value, subtracts it from both lanes, then clamps negative byte results to zero.

The two high bits are therefore one **two-bit render shade/decrement level**, not independent flags. The broader gameplay source of the decrement is not proved, so this is intentionally not renamed simply `lighting`.

Archive distribution:

```text
selector 0 : 115,582 cells
selector 1 :       0
selector 2 :       2
selector 3 :       0
```

The two selector-2 cells are level 0 at `(39,10)` and `(39,11)`, both raw word `0x8000` with cell code zero.

## `0x2000`: preserved authoring/legacy marker

A final direct-reference audit closes this bit to a bounded negative result rather than a guessed gameplay identity.

The certified CODE.2 object-1 image has 76 direct instruction references to the live cell-word array at DGROUP `0x198E0`. Those references explicitly isolate or modify `0x0040`, `0x0080`, `0x0100`, `0x0200`, `0x0400`, `0x0800`, `0x1000`, and `0xC000`, but none masks, compares, sets, or clears `0x2000`.

A whole-object search for literal immediate `0x2000` finds only three unrelated uses:

```text
object1 0x093D6  transient-effect packed-state comparison
object1 0x0C749  character status-word comparison
object1 0x357DF  fixed-point graphics arithmetic
```

None addresses or derives from the cell array.

Corpus evidence remains unusually narrow: only three of 115,584 archived cells carry the bit, all at coordinate `(44,34)` in levels 12, 13, and 14. Each stores raw word `0x2400` (`0x2000 | automap_explored`) with six-bit code zero. The surrounding 5x5 cell neighborhood is otherwise zero in all three levels, and no supplied 10-byte record or event command targets that coordinate.

The safest supplied-build classification is therefore **preserved authoring/legacy marker with no directly observed runtime behavior**. This is not proof that the original editor called it unused or padding, and computed/indirect access cannot be disproved solely from literal-reference analysis. A lossless implementation must round-trip the bit exactly and should not synthesize a gameplay effect for it.

## Final byte-grid relationship

The common tail's `width * height` byte array is loaded at DGROUP `0x1760`. The standalone writer calls `01558` immediately before writing this array, proving that it is a **generated map/automap classification cache**, not a second independent topology layer.

`01558` emits observed internal classes including:

```text
0xFF  default/unexplored
0x01  generic visible/boundary class in ordinary non-special paths
0x02..0x04  special-cell classes derived through 10-byte record state
0x05  class selected by cell flag 0x0800
0x2E/0x2F  common class selected by raw code zero, lookup value 0x0093, and header state
```

These are internal classification bytes. They must not be equated directly with the later ASCII/CP437 bytes written by the automap SAVE feature.

For archive level 22 the byte grid is `0xFF` for all 450 cells. In `LEV022.DAT` it is:

```text
0x2E : 313
0xFF : 130
0x01 :   7
```

The same standalone save has exactly **313 `0x0400` cells** and **313 `0x0040` cells**. Cell-by-cell, all 313 explored cells carry byte class `0x2E`; the remaining seven non-`0xFF` cells are boundary class `0x01`.

### Level 22 replay discrepancy

Instruction-level replay established that `01558` consults `0x25622[tile_code]` in both explored and unexplored branches. Level 22 serializes `0x25622[1] == 0x0093` in both archive and standalone headers. The only direct table animator does not rewrite `0x0093`.

Using the serialized `LEV022.DAT` header and cell words, exact ordinary-cell CODE.2 replay therefore predicts class `0x2E` for all 40 unexplored code-1 cells. The save instead contains `0xFF` for 33 and boundary class `0x01` for seven. Those are the only 40 mismatches; all 46 explored code-1 cells match the encoded path.

This remains a save/provenance or production-path discrepancy, not a reason to alter certified CODE.2 semantics. See `CACHE_REBUILD_AUDIT.md`.

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

These counts are evidence about the supplied data set, not format constraints. `CELL_FLAG_LINKS.md` and `analyze_cell_flag_links.py` contain the cross-record and adjacency audit supporting the refined names.

## Safe implementation boundary

A faithful reader can now expose the behavioral fields above while preserving the entire word verbatim. In particular:

- `0x0080` is broader than a door flag;
- `0x0100` is its paired neighboring/frame substitution flag;
- `0x0200` controls membership in an extra render prepass;
- `0x0800` marks hidden/secret-passage topology, with the literal secret-door map glyph externally corroborated;
- `0x1000` enables a visual resource offset from first-group 10-byte record state;
- `0xC000` is one two-bit decrement selector;
- `0x2000` has no direct runtime consumer/producer in the supplied CODE.2 audit; preserve it as raw authoring/legacy state.

A faithful CODE.2 cache rebuild must retain the encoded `0x0093` test in both explored and unexplored paths even though the supplied Level 22 save does not reproduce that one branch from its serialized state. Import/repack tooling should preserve existing serialized cache bytes unless and until the save provenance discrepancy is independently resolved.
