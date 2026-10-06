# LEVELS.DAT

Status: **archive/record structure proven; cell-state, automap, and dynamic door-state semantics substantially reconstructed; remaining field reconstruction in progress**.

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
- `AUTOMAP.md` — exact CODE.2 cache classifier, connected-map resolver, dynamic door states, five 64-word header lookup tables, and automap SAVE byte conversion.
- `DYNAMIC_CELL_RECORDS.md` — second 10-byte record-group animation state, open/closed endpoints, collision behavior, and automap class relationship.
- `CACHE_REBUILD_AUDIT.md` — instruction-level replay of CODE.2 `01558`, the `0x25622` animator trace, and the isolated 40-cell `LEV022.DAT` cache discrepancy.
- `inspect_levels.py` — structural and coordinate validator plus archive/direct correlation.
- `analyze_cells.py` — archive-wide cell-flag statistics and direct-save exploration/cache correlation.
- `analyze_dynamic_records.py` — archive/direct statistics for the proved dynamic 10-byte record fields.
- `audit_cache_rebuild.py` — machine-checkable ordinary-cell replay of the certified `01558` branches plus the isolated diagnostic variant used to characterize the supplied Level 22 discrepancy.

## Current semantic frontier

The six-bit cell code, transient current-view mark (`0x0040`), persistent automap explored/revealed mark (`0x0400`), and the generated nature of the final byte grid are established from CODE.2.

The automap pipeline is traced through export: ordinary connected walls, the four inset directions, dynamic door-capable states, floor classes, and the special `0xB2` hidden/secret marker path can be followed from internal cache state to exported CP437 bytes.

The dynamic second 10-byte record group is now materially decoded. Record byte `+6` is a position/state with stable endpoints **0 and 8**, byte `+7` is its signed animation delta, and `0xCF50` counts active animations. Collision code independently proves endpoint **0 = open/passable** and endpoint **8 = closed/blocking**; positions `1..7` are transitional and remain blocking on the traced movement path. For cells backed by such a record, automap class `0x04` is the fully closed endpoint and class `0x02` is the open/transitional branch. A missing group-2 record also yields class `0x04`, so that class is not globally synonymous with a closed door.

Across the supplied archive, **323 of 324** group-2 records begin stationary at endpoint 8 with low-two-bit mode 1; the single exception begins at endpoint 0. This is corroborating data-set evidence, not a format constraint.

The fixed header contains five consecutive 64-word cell-code lookup tables beginning at header `+0x00A`. The fifth table is consumed by automap rendering; all **2,048** supplied archive entries in that table are exactly `0xB5B1`, an archive-wide observation rather than a format constraint.

The earlier hypothesis that Level 22's saved cache could be explained by live animation of the `0x25622` lookup table has been disproved: `1EDAC` does not alter `0x0093`, and exact serialized-state replay differs only for 40 unexplored code-1 cells. Preserve the certified CODE.2 branch as authoritative while keeping that save-artifact discrepancy explicit.

Next work should close the remaining group-2 mode/eligibility bits and exporter overlays, then continue into the `0x514` block and still-unnamed header fields. Lossless extraction/repacking should preserve every unresolved bit and byte until its meaning is independently proved.