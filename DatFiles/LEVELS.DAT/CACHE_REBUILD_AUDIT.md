# LEVELS.DAT automap/cache rebuild audit

Status: **CODE.2 `01558` replay is instruction-accounted; the supplied `LEV022.DAT` final byte grid exposes one unresolved provenance/behavior discrepancy.**

This note records a negative result that is important for faithful reimplementation. It supersedes the earlier hypothesis that the Level 22 byte-grid discrepancy could be explained by a live transition in the lookup table beginning at DGROUP `0x25622`.

## Controlling evidence

- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- `LEV022.DAT` SHA-256: `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- Cache rebuild: object-1 logical `01558..01830`
- Standalone writer: `171FC..17837`
- Lookup-table animator: `1EDAC..1EEFA`
- Animator caller: `1F1C8` calls `1EDAC` at `1F1EB`, immediately before the first-person visibility reset/render setup path.

The immutable executable bytes remain authoritative. `LEV022.DAT` is evidence about one supplied save artifact, not authority over the executable.

## What `01558` actually does for ordinary cells

For each cell, `01558` starts with class `0xFF`. The high-byte test at `015E8` selects explored (`word & 0x0400`) versus unexplored handling.

For an explored cell:

1. raw six-bit code `0` goes to the common `0x2E/0x2F` class;
2. otherwise `table_25622[tile_code] == 0x0093` also goes to the same `0x2E/0x2F` class;
3. otherwise `0x0800` selects class `0x05`;
4. otherwise cells without `0x0080` select class `0x01`;
5. `0x0080` cells enter the 10-byte-record state path and may emit `0x02`, `0x03`, or `0x04`.

For an unexplored cell:

1. raw six-bit code `0` remains `0xFF`;
2. otherwise `table_25622[tile_code] == 0x0093` also goes to `0x2E/0x2F`;
3. otherwise the routine tests all eight neighbors and emits class `0x01` when at least one neighbor is both explored (`0x0400`) and raw-code-zero.

The eight-neighbor boundary test therefore uses the raw six-bit code of the neighbor; it does not consult `0x25622` for the neighbor.

## The `0x25622` animator does not explain Level 22

`1EDAC` iterates word offsets `0x00..0x7C` (table indices `0..62`) over two primary lookup lanes at `0x25622` and `0x25722`. For each entry it performs at most one replacement from the ten-word-pair table at object-2/DGROUP `0xBFBC`:

```text
015C -> 015D
015D -> 015E
015E -> 015C
00AA -> 00AB
00AB -> 00AA
011B -> 011C
011C -> 011D
011D -> 011B
0101 -> 0102
0102 -> 0101
```

It then cycles entries in ranges `0x0242..0x0244` and `0x0245..0x0247`. When one of those ranges advances, the paired table at `0x256A2` or `0x257A2` is written as the advanced primary value plus six.

Value `0x0093` is in none of those mappings or cycling ranges.

For Level 22, both the archive header and the supplied standalone save have:

```text
0x25622[0] = 0x0000
0x25622[1] = 0x0093
all other 0x25622 entries = 0x0000
```

The archive and save `0x2A4` headers differ at only the already-documented byte `+0x29A`; the `0x25622` table bytes are identical. Thus the direct `1EDAC` animation path cannot transform Level 22 tile code `1` away from `0x0093`.

## Exact Level 22 replay result

The supplied `LEV022.DAT` contains 450 cells:

```text
313 cells have 0x0400 explored
137 cells are unexplored
86 cells have tile_code == 1
364 cells have tile_code == 0
```

Among the 86 code-1 cells:

```text
46 are explored
40 are unexplored
```

The serialized final byte grid is:

```text
0x2E : 313
0xFF : 130
0x01 :   7
```

Replaying the ordinary-cell branches of the exact supplied CODE.2 `01558` using the serialized Level 22 header and cell words produces:

```text
0x2E : 353
0xFF :  97
```

There are exactly **40 mismatches**. Every mismatch is an unexplored `tile_code == 1` cell:

```text
predicted by supplied CODE.2 : 0x2E
saved LEV022.DAT            : 0xFF for 33 cells, 0x01 for 7 cells
```

No explored cell mismatches. In particular, all 46 explored code-1 cells serialize as `0x2E`, exactly as the `0x25622[1] == 0x0093` explored branch predicts.

If one changes only the unexplored-cell path so that `table_25622[tile_code] == 0x0093` does **not** take the `0x2E/0x2F` shortcut, the replay matches the supplied save **450/450 bytes**: the same 33 cells stay `0xFF`, and the same seven cells become boundary class `0x01` through the encoded eight-neighbor test.

That alternate replay is evidence about the save artifact; it is **not** permission to rewrite the certified CODE.2 decompilation. The supplied CODE.2 bytes unambiguously contain the unexplored `0x0093` shortcut at `01691..016C3`.

## Writer-order constraint

The standalone writer serializes the full `0x2A4` header from DGROUP `0x25618` at `172BC..172CC`, writes the mutable cell layer and intervening sections, then calls `01558` at `1769D` immediately before writing DGROUP `0x1760` as the final byte grid.

No call to `1EDAC` exists on that writer path between header serialization and the `01558` call. The low-level `31304` write wrapper resolves to the DOS/runtime write path and does not itself contain the table animation logic.

Therefore the previously proposed explanation — "the serialized header has one table state, but the cache was rebuilt from another state produced by `1EDAC`" — is not supported.

## Current conclusion

The executable semantics and the supplied save artifact disagree in one sharply bounded place: unexplored nonzero cells whose `0x25622` lookup value is `0x0093`.

The evidence currently supports these statements and no stronger one:

- `01558` in the supplied CODE.2 definitely applies the `0x0093` shortcut in both explored and unexplored branches.
- Level 22 tile code `1` definitely serializes with lookup value `0x0093`.
- `1EDAC` definitely cannot change `0x0093`.
- The supplied `LEV022.DAT` final byte grid behaves as though the unexplored `0x0093` shortcut were absent, while the explored shortcut remains present.
- The exact reason for that difference is unresolved. Plausible categories include save/executable revision provenance or another as-yet-unproved production path, but none is asserted without evidence.

A faithful port should implement the certified CODE.2 branch exactly. A compatibility/import layer may preserve serialized cache bytes from existing saves, but must not silently alter CODE.2 semantics to force this one artifact to reproduce.
