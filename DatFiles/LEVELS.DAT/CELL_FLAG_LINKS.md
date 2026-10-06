# LEVELS.DAT remaining packed-cell flag links

Status: **the remaining high-value cell-word flags are tightened from certified CODE.2 consumers plus archive-wide structural correlations. `0x2000` remains unresolved.**

This document supplements `CELL_WORD.md`. The row-major 16-bit cell array is loaded at DGROUP `0x198E0`; bits `0..5` are the independently replaceable six-bit cell code.

## Refined safe field model

```text
0x003F  six-bit cell code
0x0040  transient current visibility/raycast mark
0x0080  special-geometry center / dynamic-collision gate
0x0100  special-geometry frame / adjacent-code substitution gate
0x0200  extra render-prepass enable
0x0400  persistent automap explored/revealed mark
0x0800  secret/hidden-passage topology marker
0x1000  10-byte-record state resource-offset enable
0x2000  unresolved
0xC000  two-bit render shade/decrement level (0..3)
```

Names remain behavioral where the executable does not expose original editor terminology.

## `0x0080`: special-geometry center / dynamic-collision gate

Movement/collision code tests `0x0080` before ordinary low-six-bit blocking. When set it calls `1E930(cell_index)` instead:

```text
if (cell & 0x0080):
    blocked = (1E930(cell_index) != 0)
else:
    blocked = ((cell & 0x003F) != 0)
```

`1E930` searches the second 10-byte record group. A matching kind-1 dynamic record returns `record[6] << 12`; no matching record returns `0x8000`. Since kind-1 position zero is independently proved open/passable and nonzero positions block on this path, `0x0080` is a special/dynamic collision-geometry gate.

It is broader than `door`: among the 745 supplied `0x0080` cells,

```text
313  contain kind-1 dynamic records
149  contain kind-8 records
283  contain no 10-byte record
```

Automap classification also routes `0x0080` cells into the special inset/door resolver. The safe name therefore remains special-geometry center/dynamic-collision gate rather than a door-only bit.

## `0x0100`: special-geometry frame / adjacent-code substitution

First-person renderer paths `32F8C` and `33124` inspect `0x0100`. When it is set they compute one direction-dependent neighboring cell. If that neighbor carries `0x0080`, the renderer substitutes the neighbor's six-bit cell code as the resource-code input and enables a `+1` resource variant offset.

The archive geometry makes the pairing explicit:

```text
0x0100 cells = 1,690
  adjacent 0x0080 centers:
    0 ->    8
    1 -> 1,520
    2 ->  161
    3 ->    1

0x0080 cells = 745
  adjacent 0x0100 frame cells:
    2 -> 391
    3 -> 353
    4 ->   1
```

Every supplied `0x0080` center has at least two orthogonal `0x0100` neighbors, and 1,682 / 1,690 `0x0100` cells touch an `0x0080` center. These are therefore paired special-geometry center/frame flags, not independent renderer switches.

## `0x0200`: extra render-prepass enable

The renderer extracts:

```text
(cell & 0x0200) >> 9
```

into a one-bit render-record field. Routine `335DC` first scans the generated render-record list and processes only records whose field equals one. At `336F7` it begins the ordinary all-record render pass.

Thus `0x0200` is safely described as **enabling membership in an additional preliminary render pass**. The visual reason for that pass is not named more strongly.

The supplied archive contains 4,957 cells with this bit set.

## `0x1000`: 10-byte-record state resource-offset enable

Renderer paths `32F8C` and `33124` test `0x1000`. If no earlier adjacent-cell override was selected, they call `1E9A0(cell_index)`.

`1E9A0` searches the first runtime 10-byte record group (`0xCB3C`), matches the cell, and returns:

```text
record[1] & 0x03
```

The renderer then adds that `0..3` state value to both selected resource IDs. Therefore `0x1000` explicitly enables a **visual/resource variant offset derived from the associated 10-byte record's low-two-bit state**.

Archive-wide correlation is nearly exact and cleanly separates the first/runtime group:

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

kind 1 dynamic records: 313 / 324 on 0x1000 cells
```

Only two supplied `0x1000` cells lack a 10-byte record; both are level 15 boundary cells with code 13. The eleven 10-byte records not on `0x1000` cells are all kind-1 dynamic records in levels 12..14.

## `0x0800`: secret/hidden-passage topology marker

CODE.2 alone proves two important behaviors:

1. in first-person traversal, when renderer-wide mode `FD08` is nonzero, a nonempty `0x0800` cell can continue through the same path used by an empty cell;
2. automap rebuild assigns `0x0800` explored cells class `0x05`; the connected resolver can turn that into resolved class `0x33` when the required opposite-floor geometry is present.

The SAVE automap exporter maps resolved `0x33` to CP437 byte `0xB2`. Independent Ravenloft automap-format documentation identifies that exported byte as the secret-door marker. Thus the binary proves special pass-through/hidden-map topology, while the literal human label **secret door** is externally corroborated rather than inferred from the bit alone.

All 47 supplied `0x0800` cells:

- have nonzero six-bit cell codes;
- contain no 10-byte record.

The safe project-level name is therefore **secret/hidden-passage topology marker**.

## `0xC000`: two-bit render shade/decrement level

The renderer extracts:

```text
shade = (cell & 0xC000) >> 14
```

and stores the `0..3` value in generated render-record field `+0x18`. Draw routine `3263C` loads this value, duplicates it into both bytes of a packed 16-bit render value, subtracts it from both byte lanes, and clamps each result at zero.

This is direct evidence for a **two-bit render shade/decrement level**, not two independent flags. It is intentionally not named `lighting` because the broader gameplay source of the value is not proved.

Archive distribution:

```text
selector 0 : 115,582 cells
selector 1 :       0
selector 2 :       2
selector 3 :       0
```

The two selector-2 cells are in level 0 at `(39,10)` and `(39,11)`, both raw word `0x8000` with six-bit code zero.

## `0x2000`: still unresolved

No direct cell-word consumer has been proved for `0x2000`.

Only three supplied archive cells carry it. All three are the same coordinate `(44,34)` in levels 12, 13, and 14, with raw word `0x2400` (`0x2000 | automap_explored`) and cell code zero.

This recurrence is useful evidence but not enough to assign semantics. The bit remains raw/preserved and unnamed.

## Safe implementation boundary

A faithful reader/editor may now expose the refined behavioral names above, while preserving the complete 16-bit word verbatim. In particular:

- do not collapse `0x0080` to `door`; it covers a broader special-geometry center role;
- treat `0x0100` as its neighboring/frame substitution partner;
- use `0x0200` only as an extra-render-prepass membership bit;
- derive `0x1000` visual variants from first-group 10-byte record state exactly as CODE.2 does;
- treat `0x0800` as hidden/secret-passage topology, with the literal secret-door glyph name attributed to the automap export format;
- treat `0xC000` as one two-bit decrement selector;
- leave `0x2000` unresolved.
