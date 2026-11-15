# Ravenloft: Stone Prophet — original data formats (Phase 1, partial)

**Scope:** Executable-backed physical layouts of `PROPHET/CODE.2` and preserved extracted game-data files. No title-specific gameplay semantics or media search order is presumed. See [CROSS_TITLE_DIFF.md](../../CROSS_TITLE_DIFF.md) and [PHASE0_INVENTORY.md](../../PHASE0_INVENTORY.md).

**Original executable:** `MASTER/Ravenloft - Stone Prophet/PROPHET/CODE.2`, 820,497 bytes, SHA-256 `00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e`. CODE offsets below are within its authenticated LE code object. The Phase 1 audit verified twelve instruction witnesses.

## Binary field geometry

| File | Offset, width, endian | Observed data | Reader evidence | Confidence |
| --- | --- | --- | --- | --- |
| CD_Extracted/NPCS.DAT | `+0 u32 LE` count | 110 | CODE.2 `0x20EDD` filename; count global `0x480FC` | VERIFIED |
| CD_Extracted/NPCS.DAT | `+4`, 110 × 0x58-byte records | 9,680 body bytes | CODE.2 `0x20F0A` count × 0x58 | VERIFIED |
| CD_Extracted/NPCS.DAT | record `+0 char[20]` | 110/110 printable NUL-terminated names | full-corpus name probe | VERIFIED physical field |
| CD_Extracted/NPCS.DAT | record `+0x45..+0x57`, 19 bytes | nonzero in 8/19 byte-columns across file | aggregate byte-position probe | VERIFIED physical extension |
| CD_Extracted/OBJECTS.DAT | file `+0 u32 LE` count | 516 | CODE.2 `0x20FD9` filename and counted read | VERIFIED |
| CD_Extracted/OBJECTS.DAT | record stride 0x41 (65 bytes) | 516 entries | `0x21012` shift 6 + `0x21015` addition | VERIFIED |
| CD_Extracted/LEVELS.DAT | file `+0 u32 LE` count, `+4 u32[] LE` offsets | 43 levels, first at 0xB0 | 43/43 index checks | VERIFIED |
| CD_Extracted/LEVELS.DAT | record `+1` seven `u32 LE` boundaries | 29-byte preamble | 43/43 | VERIFIED |
| CD_Extracted/LEVELS.DAT | record `+0x1D` fixed header | 0x824 bytes in 43/43 records | CODE.2 `0x21E71` exact read size, `0x21E83` length check | VERIFIED |
| CD_Extracted/LEVELS.DAT | indexed script section | 0x514 bytes in 43/43 records | CODE.2 `0x2222D` read size, `0x2223B` length check | VERIFIED physical block |
| CHARS | block read length 0x1070; indexed stride 0x41C | 4 × 0x41C equals 0x1070 | `0x216B9` block length, `0x0473C` indexed multiply | VERIFIED operations; four-record grouping LIKELY |
| PROPHET/RES4 | entry-relative `+3 i16 LE` header length | Type-5: 963 records header 5; Type-6: 410 records header 5 | all indexed records scanned | VERIFIED |

### Type-5/6 and RES9 preservation

Stone `PROPHET/RES4` entry 0 is Type-5, with the common two-byte EH marker, type byte=5 at `+2`, signed little-endian header length=5 at `+3`. All 963 Type-5 entries have next-five-header-byte positions 0=3, 1=2, 3=0, 4=0; position 2 equals **16 in 63 records** or **48 in 900 records**. The Type-5 payload lengths range from 1,177 to 9,495 bytes. The meaning of each header value is UNKNOWN.

All 410 Type-6 entries in `PROPHET/RES4` have five-byte headers. Header byte 0 is always 3, bytes 3 and 4 are always zero, and bytes 1 and 2 vary. Direct reuse of Strahd's differently shaped Type-5 loader would be unjustified.

The installed `PROPHET/RES9` and `CD_Extracted/RES9` are both 626-record archives. **625 records are byte-identical; record 574 differs by five bytes**. Keep both copies; it is unproved which is chosen by the original file-search policy.

## Executable resource decoder and level-event VM (Phase 1 continuation)

**VERIFIED:** The CODE.2 six-type EH resource dispatcher at `0x40DF1` uses the little-endian jump table at `0x40C88`. Its Type-5 handler is `0x4C2C0` and Type-6 handler `0x4C358`. Both use record-relative `+5 u8 compression_method` followed by `+6 u32LE declared_decoded_size`; method 3 is used by all supplied Type-5/6 entries, and the executable has branches for methods 0 and 1.

The unchanged Strahd LZSS implementation decodes **2,542/2,542** Type-6 RES3 and **409/410** Type-6 RES4 records with exact input consumption. RES4 index **1309** yields 14 decoded bytes after consuming 11 of 15 encoded bytes; the four-byte tail is preserved and remains unexplained. Type-5 RES4: **900/963** records declaring **12,290** bytes decode fully; the **63** declaring **4,098** consume the full encoded stream when decoding a **4,096-byte prefix** instead of the declared **4,098**. This is a **verified stream-level observation**, not a reason to alter the serialized length.

The level-event dispatcher at CODE.2 `0x313` uses fourteen entries at table `0x264` (opcodes 0..13). All **43** levels have a valid 50 × 26-byte event table: **396** active slots and **469** active five-byte commands. Opcode 13 is implemented in the executable but absent in the supplied level archive.

`RES9` has **48/49** nonempty raw entries with a single-bank program record: `u16LE count + count u16LE words`. The remaining raw entry, **625**, is four bytes and begins with `u16LE=244`; its meaning is UNKNOWN. Unlike Strahd, none of the 48 programs parses under its known wordcode grammar. Record **574** is a 316-word single-bank program, and its installed vs CD copies differ at exactly five word indices: **246 (30→10), 261 (31→11), 268 (30→10), 273 (31→11), 313 (31→11)**, installed to CD. Semantic meaning and loader precedence remain UNKNOWN.

**Phase 1 RES9 update:** Stone CODE.2 uses 54 cases at table `0x1E10C` (dispatch `0x1E220`) on `u16LE opcode - 2`. Helper `0x1D118` handles opcode `0x13`/`0x1A` choice pairs (sentinel `0x26`); helper `0x1C6A4` consumes opcode `0x05`'s single character-template ID. Opcode `0x08` consumes one word; its callee `0x15200` does not advance the wordcode pointer. The candidate decoder linearly consumes **44/48** scripts; unresolved indices are **568, 575, 585, 595**. Of checked direct targets, **1,195** land on decoded starts while **10** in scripts `615..624` have unresolved target value `999`. **VERIFIED observed byte geometry and cursor reads; control-flow semantics not certified.** See [VM evidence](../../Resources/RES/CROSS_TITLE_WORDCODE_VM_PHASE1.md).

**Reproduce:**

```sh
python Tools/phase1_resource_event_vm.py --root preservation/MASTER --json audit/stone-vm.json
python Tools/phase1_decoder_dispatch.py --root preservation/MASTER --json audit/stone-dispatch.json
python Tools/phase1_type56_verify.py --root preservation/MASTER --json audit/stone-lzss.json
```

### Unresolved

The extra `0x580` bytes in the 0x824-byte level header contain nonzero bytes at **597 distinct byte positions** across 43 levels. Their layout, including widths and semantics, remains UNKNOWN. Same for the 88-byte NPC stat/action fields, CHARS and SAVE codecs, rules, event-script opcode semantics, cinematic ACT/DFA data, the Type-5 two-byte discrepancy and original RES9 loader precedence. No playable Stone Prophet native session was run.

**Reproduce:**

```sh
python Tools/phase1_compare.py --root preservation/MASTER --json audit/formats.json --markdown audit/formats.md
python Tools/phase1_code2_consumers.py --root preservation/MASTER --json audit/code2.json
```
