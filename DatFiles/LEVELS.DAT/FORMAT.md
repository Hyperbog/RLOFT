# LEVELS.DAT / LEVnnn.DAT structural reconstruction

Status: **structural layout proven against CODE.2 loader/writer and all available sample bytes**.

This document records only facts established by the audited CODE.2 binary and the supplied data files. Gameplay semantics for still-unnamed fields remain intentionally unresolved.

## Controlling evidence

- `LEVELS.DAT`: 499,878 bytes, SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`
- `LEV022.DAT`: 6,028 bytes, SHA-256 `f88b033ff4cdafdd89620e0fac51ff2fb81bce103a6d31a26b91464532c9f51e`
- Audited CODE.2 object-1 loader: logical `181B4..18D8B`
- Audited CODE.2 standalone level writer: logical `171FC..17837`

CODE.2 `181B4` first tries the standalone level file. If absent it opens `levels.dat`, seeks to `4 + level_id*4`, reads one dword, then seeks to `record_offset + 0x1D`. This proves the archive directory and the common 29-byte per-record preamble.

## LEVELS.DAT archive header

```text
+0x0000  uint32 record_count       = 32
+0x0004  uint32 record_offset[32]
+0x0084  first level record
```

The first directory offset is exactly `0x84 = 4 + 32*4`. All 32 offsets are strictly increasing. Record `i` occupies `offset[i] .. offset[i+1]-1`; record 31 ends at archive EOF.

## Common 0x1D-byte record preamble

Every archived level and `LEV022.DAT` uses:

```text
+0x00    uint8  marker/unknown
+0x01    uint32 boundary_0 = 0x1D
+0x05    uint32 boundary_1 = end of fixed header + 16-bit grid
+0x09    uint32 boundary_2 = end of variable column-list area
+0x0D    uint32 boundary_3 = end of record family A
+0x11    uint32 boundary_4 = end of record family B
+0x15    uint32 boundary_5 = end of record family C
+0x19    uint32 boundary_6 = end of fixed 0x514-byte block
```

The loader does not consume byte `+0x00`. The CODE.2 writer seeks directly to `0x1D` for payload output and later writes the seven dwords beginning at file offset `+0x01`. Archived records have marker byte `0x00`; the supplied `LEV022.DAT` has `0xDF`. No semantic meaning is assigned.

## Fixed header and map grid

At `boundary_0` the loader reads exactly `0x2A4` bytes into DGROUP `0x25618`.

Proven fields used immediately afterward:

```text
header +0x001  int16 width
header +0x003  int16 height
header +0x28A  int16 count_258A2
header +0x28C  int16 count_258A4
header +0x28E  int16 count_258A6
```

The next section contains `height` rows of `width` 16-bit values. Therefore:

```text
boundary_1 = 0x1D + 0x2A4 + width*height*2
```

This equality holds for all 32 archived levels and for `LEV022.DAT`.

## LEVELS.DAT legacy record body

The archive fallback path is a legacy compact representation.

From `boundary_1`, there are exactly `width` column lists. Each list is:

```text
int16 count
count * 10-byte records
```

The resulting cursor is exactly `boundary_2` for every archived level.

Three compact record families follow. CODE.2 reads each compact record as exactly 11 bytes and expands it into a 22-byte runtime structure:

```text
boundary_2..boundary_3 : count_258A2 * 11
boundary_3..boundary_4 : count_258A6 * 11
boundary_4..boundary_5 : count_258A4 * 11
boundary_5..boundary_6 : exactly 0x514 bytes
```

All 32 archive records satisfy these equations with no exceptions.

## Standalone LEVnnn.DAT body

The direct-save path stores the expanded mutable representation.

Its variable column area contains:

```text
width * { int16 count; count * 10-byte records }
1 byte field corresponding to DGROUP 0xCF50
width * { int16 count; count * 10-byte records }
```

Then:

```text
boundary_2..boundary_3 : count_258A2 * 22
boundary_3..boundary_4 : int16 count_258A6 + count_258A6 * 22
boundary_4..boundary_5 : count_258A4 * 22
boundary_5..boundary_6 : exactly 0x514 bytes
```

`LEV022.DAT` validates every one of these relationships.

## Common tail

Both forms end with:

```text
int16 width_again
int16 height_again
uint8 cell_bytes[width*height]
uint32 text_length
uint8 text[text_length]
```

The duplicated dimensions equal the fixed-header dimensions in all 32 archive records and in `LEV022.DAT`. Every supplied record terminates exactly after the length-prefixed text. In the current samples all archive text lengths and the `LEV022.DAT` text length are zero.

## Level 22 correlation

Archive record 22 begins at `LEVELS.DAT` file offset `0x5AD83` and is `0x1288` bytes long. `LEV022.DAT` is `0x178C` bytes long.

Both describe a `30 x 15` level and both carry:

- `count_258A2 = 1`
- `count_258A6 = 110`
- `count_258A4 = 0`

The size difference is explained structurally by the archive's compact 11-byte legacy records versus the standalone file's expanded 22-byte records plus the second variable-list group and one-byte state field.

## Current boundary

The container and section sizes are now structurally decoded and machine-checkable. The next phase is semantic naming: decode the `0x2A4` header, 16-bit cell words, 10-byte variable records, 11->22-byte conversion fields, the `0x514` block, and the final per-cell byte grid by tracing their consumers in certified `Code2.cpp`.