# Menzoberranzan — original data formats (Phase 1, partial)

**Scope:** Executable-backed file geometry for CODE.2, RES4 Type-5/6, LEVELS.DAT, NPCS.DAT, OBJECTS.DAT and CHARS. Semantics of unexamined fields are UNKNOWN. See [CROSS_TITLE_DIFF.md](../../CROSS_TITLE_DIFF.md) and [original Strahd format notes](../NPCS.DAT/FORMAT.md).

**Original executable:** `MASTER/Menzoberranzan/CODE.2`, 650,385 bytes, SHA-256 `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96`. Every code address below is an offset within its unpacked LE code object. The audited `Tools/phase1_code2_consumers.py` matches twelve exact instructions; no commercial bytes are bundled.

## Binary field geometry

| File | Offset, width, endian | Observed data | Reader evidence | Confidence |
| --- | --- | --- | --- | --- |
| NPCS.DAT | file `+0 u32 LE` count | 98 | read into global `0x3A600` | VERIFIED |
| NPCS.DAT | file `+4`, 98 × 0x58-byte records | 8,624 data bytes | CODE.2 `0x1AF55` filename, `0x1AF82` count × 0x58 | VERIFIED |
| NPCS.DAT | record `+0 char[20]` | 98/98 printable NUL-terminated names | full-record corpus probe | VERIFIED physical field |
| NPCS.DAT | record `+0x45..+0x57`, 19 bytes | nonzero in 12/19 byte-columns across file | per-column tally, not semantic decoding | VERIFIED physical extension |
| OBJECTS.DAT | file `+0 u32 LE` count | 523 | original CODE.2 `0x1B051` filename and counted allocation | VERIFIED |
| OBJECTS.DAT | record stride 0x41 (65 bytes) | 523 serialized entries | `0x1B087` shift 6 + `0x1B08A` addition | VERIFIED |
| LEVELS.DAT | file `+0 u32 LE` count, `+4 u32[] LE` offsets | 56 levels, first record at 0xE4 | 56/56 valid indexed offsets | VERIFIED |
| LEVELS.DAT | record `+1` seven `u32 LE` boundaries | 29-byte preamble | all 56 records | VERIFIED |
| LEVELS.DAT | record `+0x1D` fixed header, 0x824 bytes | 56/56 exact header-and-grid equation | CODE.2 `0x1BE41` push size, `0x1BE53` compare read length | VERIFIED |
| LEVELS.DAT | counted event block, 0x514 bytes | 56/56 matching boundaries | CODE.2 `0x1C210` push size, `0x1C21E` compare length | VERIFIED physical block |
| CHARS | full read size 0x1070; indexed access stride 0x41C | 0x1070 = 4 × 0x41C | CODE.2 `0x1B69E` push 0x1070; `0x041C0` indexed multiply 0x41C | VERIFIED operations; four-record interpretation LIKELY |
| RES4 | indexed entry-relative `+3 i16 LE` type header length | Type-5: 559 entries of header length 5; Type-6: 502 of length 5 | full typed-record corpus scan | VERIFIED |

### RES4 Type-5 header (physical-only)

At `RES4` entry 0: `+0..1` two-byte EH marker, `+2 u8` type=5, `+3 i16 LE` header_length=5. The next five bytes at entry `+5..+9` are constant at positions 0=3, 1=2, 3=0, 4=0; position 2 equals **16 in 85 entries** or **48 in 474 entries**. Across all 559 Type-5 records, the payload length (after the five-byte header) ranges from 1,123 to 9,676 bytes. Header bytes are not labeled as dimensions, compression methods or offsets until the original CODE.2 handler is recovered.

Type-6 in the same file likewise uses a five-byte header; first header byte is 3 and the last two are zero across all 502 entries, while middle bytes vary. Strahd's resource Type-5 loader assumes a different header size and cannot be adopted as-is.

### Unresolved

The `0x824` level header includes an extra 0x580 bytes compared with Strahd; **706 distinct positions** in this extension contain a nonzero byte in at least one of the 56 archived levels. The byte count and positions are known; the meanings and field widths are not.

All 88-byte NPC record values past their common ASCII name prefix remain uninterpreted; the 0x41C CHARS record fields, event VM opcode semantics, save serialization, rules, and Type-5/6 decoders require additional CODE.2 consumers. No Menzo native gameplay is implemented or tested.

**Reproduce:**

```sh
python Tools/phase1_compare.py --root preservation/MASTER --json audit/formats.json --markdown audit/formats.md
python Tools/phase1_code2_consumers.py --root preservation/MASTER --json audit/code2.json
```
