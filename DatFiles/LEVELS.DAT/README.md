# LEVELS.DAT

Status: **archive/record structure proven; first cell-state and automap semantics proven; remaining field reconstruction in progress**.

- Size: 499,878 bytes
- SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- CODE.2 loader evidence: logical `181B4..18D8B`
- CODE.2 standalone level writer evidence: logical `171FC..17837`

## Proven structure

`LEVELS.DAT` begins with a 32-bit count of **32**, followed by exactly 32 little-endian record offsets. The directory ends at `0x84`, which is also the first record offset. CODE.2 independently proves this indexing rule by seeking to `4 + level_id*4`, reading the dword record offset, then seeking to `record_offset + 0x1D`.

All 32 archived records have been structurally validated against the loader. Each contains:

1. `0x1D`-byte preamble with seven section-boundary dwords beginning at byte 1.
2. Fixed `0x2A4`-byte base header.
3. `width * height` 16-bit packed cell-state grid.
4. Width-indexed variable lists of 10-byte records.
5. Three legacy compact record families stored as 11-byte records and expanded by CODE.2 into 22-byte runtime structures.
6. Fixed `0x514`-byte block.
7. Duplicated width/height words, generated `width * height` automap/display classification bytes, and a 32-bit length-prefixed tail.

Every boundary equation and final record length validates for all 32 levels with no exceptions.

## Documentation and tools

- `FORMAT.md` — byte-level archive/direct-save structure.
- `SEMANTICS.md` — proved header, list-record, and 11->22-byte record semantics.
- `CELL_WORD.md` — 16-bit cell code/flag model, visibility/exploration behavior, renderer consumers, and final automap byte-cache relationship.
- `inspect_levels.py` — structural and coordinate validator plus archive/direct correlation.
- `analyze_cells.py` — archive-wide cell-flag statistics and direct-save exploration/cache correlation.

## Current semantic frontier

The six-bit cell code, transient current-view mark (`0x0040`), persistent automap explored/revealed mark (`0x0400`), and the derived nature of the final byte grid are now established from CODE.2. Several other cell bits have proven renderer behaviors but intentionally retain behavioral names pending exact geometry/interaction identification.

Next work should close those remaining cell flags and internal byte-map classes, then continue into the `0x514` block and still-unnamed header fields. Lossless extraction/repacking should preserve every unresolved bit and byte until its meaning is independently proved.