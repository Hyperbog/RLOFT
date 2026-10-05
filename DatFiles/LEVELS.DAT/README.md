# LEVELS.DAT

Status: **archive and record structure proven; semantic field reconstruction in progress**.

- Size: 499,878 bytes
- SHA-256: `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- CODE.2 loader evidence: logical `181B4..18D8B`
- CODE.2 standalone level writer evidence: logical `171FC..17837`

## Proven structure

`LEVELS.DAT` begins with a 32-bit count of **32**, followed by exactly 32 little-endian record offsets. The directory ends at `0x84`, which is also the first record offset. CODE.2 independently proves this indexing rule by seeking to `4 + level_id*4`, reading the dword record offset, then seeking to `record_offset + 0x1D`.

All 32 archived records have now been structurally validated against the loader. Each contains:

1. `0x1D`-byte preamble with seven section-boundary dwords beginning at byte 1.
2. Fixed `0x2A4`-byte base header.
3. `width * height` 16-bit grid.
4. Width-indexed variable lists of 10-byte records.
5. Three legacy compact record families stored as 11-byte records and expanded by CODE.2 into 22-byte runtime structures.
6. Fixed `0x514`-byte block.
7. Duplicated width/height words, `width * height` byte grid, and a 32-bit length-prefixed tail.

Every boundary equation and final record length validates for all 32 levels with no exceptions.

See `FORMAT.md` for the byte-level specification and `inspect_levels.py` for the machine-checkable validator.

## Next phase

Trace consumers of the fixed header, cell words, 10-byte lists, 11->22-byte conversion fields, `0x514` block, and final byte grid through certified `Code2.cpp`. Assign semantic names only when directly proved, then add lossless extraction/repacking once the complete field model is stable.