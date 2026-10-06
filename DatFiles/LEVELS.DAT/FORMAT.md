# LEVELS.DAT / LEVnnn.DAT structural reconstruction

Status: **structural layout proven against CODE.2 loader/writer and all available sample bytes; common tail annotation blob identified.**

This document records facts established by the audited CODE.2 binary and supplied data files. Unnamed gameplay fields remain intentionally unresolved.

## Controlling evidence

- `LEVELS.DAT`: 499,878 bytes, SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- `LEV022.DAT`: 6,028 bytes, SHA-256 `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- CODE.2 loader: logical `181B4..18D8B`
- CODE.2 standalone writer: logical `171FC..17837`

CODE.2 `181B4` first tries the standalone level file. If absent it opens `levels.dat`, seeks to `4 + level_id*4`, reads one dword, then seeks to `record_offset + 0x1D`. This proves the archive directory and common 29-byte per-record preamble.

## LEVELS.DAT archive header

```text
+0x0000  uint32 record_count = 32
+0x0004  uint32 record_offset[32]
+0x0084  first level record
```

The first offset is exactly `0x84 = 4 + 32*4`. All offsets are strictly increasing. Record `i` ends at the next offset, with record 31 ending at EOF.

## Common 0x1D-byte preamble

```text
+0x00  uint8  marker/unknown
+0x01  uint32 boundary_0 = 0x1D
+0x05  uint32 boundary_1 = end fixed header + 16-bit grid
+0x09  uint32 boundary_2 = end variable 10-byte lists
+0x0D  uint32 boundary_3 = end family A
+0x11  uint32 boundary_4 = end family B
+0x15  uint32 boundary_5 = end family C
+0x19  uint32 boundary_6 = end fixed 0x514 block
```

The loader does not consume byte `+0x00`. The writer seeks directly to `0x1D` for payload output and later writes the seven boundary dwords beginning at file offset `+0x01`. Archived records have marker `0x00`; supplied `LEV022.DAT` has `0xDF`. Its meaning remains unresolved.

## Fixed header and 16-bit cell grid

At `boundary_0` the loader reads exactly `0x2A4` bytes into DGROUP `0x25618`.

Proven fields include:

```text
header +0x001  int16 width
header +0x003  int16 height
header +0x28A  int16 count_258A2 (family A)
header +0x28C  int16 count_258A4 (family C)
header +0x28E  int16 count_258A6 (family B)
```

The next section contains `height` rows of `width` 16-bit packed cell words:

```text
boundary_1 = 0x1D + 0x2A4 + width*height*2
```

This equality holds for all 32 archive records and `LEV022.DAT`.

## LEVELS.DAT legacy body

From `boundary_1` are exactly `width` column lists:

```text
int16 count
count * 10-byte records
```

The resulting cursor equals `boundary_2` for every archived level.

Three compact record families follow. Each archive record is exactly 11 bytes and is expanded by CODE.2 into a 22-byte runtime structure:

```text
boundary_2..boundary_3 : count_258A2 * 11
boundary_3..boundary_4 : count_258A6 * 11
boundary_4..boundary_5 : count_258A4 * 11
boundary_5..boundary_6 : exactly 0x514 bytes
```

All 32 archive records satisfy these equations without exception.

## Standalone LEVnnn.DAT body

The direct-save path stores the expanded mutable representation:

```text
width * { int16 count; count * 10-byte group-1 records }
uint8 active_animation_count (DGROUP 0xCF50)
width * { int16 count; count * 10-byte group-2 records }
```

Then:

```text
boundary_2..boundary_3 : count_258A2 * 22
boundary_3..boundary_4 : int16 count_258A6 + count_258A6 * 22
boundary_4..boundary_5 : count_258A4 * 22
boundary_5..boundary_6 : exactly 0x514 bytes
```

`LEV022.DAT` validates every relationship.

## Common tail

Both forms end with:

```text
int16 width_again
int16 height_again
uint8 automap_cache[width*height]
uint32 annotation_blob_length
uint8 annotation_blob[annotation_blob_length]
```

The duplicated dimensions equal the fixed-header dimensions in all supplied records. The byte grid is generated automap/display state; see `CELL_WORD.md` and `AUTOMAP.md`.

### Annotation blob

CODE.2 loads the length-prefixed blob into an `0x800`-byte zero-filled runtime buffer at `0x5760`. Its in-memory records are:

```text
int16 raw_x
int16 raw_y
int16 payload_length
uint8 payload[payload_length]
```

A zero `payload_length` terminates the in-memory list. Writer routine `1C28` walks records and returns the number of bytes preceding that zero-length terminator; the terminator itself is therefore not serialized. Loader code reads a blob only when the serialized length is below `0x7FF`.

Automap exporter routines `23B8` and `2474` use this list to place map labels and emit the corresponding label/text legend, proving that the tail blob is the automap annotation/note list rather than generic unrelated text.

All 32 supplied archive records and `LEV022.DAT` have annotation length zero.

## Level 22 correlation

Archive record 22 begins at file offset `0x5AD83` and is `0x1288` bytes long. `LEV022.DAT` is `0x178C` bytes.

Both describe `30 x 15` cells and carry family counts:

```text
A = 1
B = 110
C = 0
```

The size difference is explained by compact 11-byte archive records versus expanded 22-byte direct-save records, plus the second 10-byte-record group and its one-byte animation-count state.

## Current boundary

The archive/direct container, section boundaries, mutable/direct representation split, generated automap-cache tail, and annotation blob are structurally decoded. Semantic work continues in `SEMANTICS.md`, `CELL_RECORDS.md`, `CELL_WORD.md`, `AUTOMAP.md`, and the still-unresolved `0x514` block.