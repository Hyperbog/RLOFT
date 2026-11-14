# Phase 1 — Cross-title binary format differences

**Status: IN PROGRESS.** This is the Phase 1 cross-title diff deliverable; its first CODE.2 consumer evidence component is verified, but the full data-family, event-VM, save/rules, cinematics and rendering comparisons are not complete. The native engine is unchanged.

**Controlling media:** `DnD-Classics/MASTER` (read-only Git checkout). Menzo `CODE.2` SHA-256 `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96`; Stone `PROPHET/CODE.2` SHA-256 `00e225ec8f141a8fdb72ff0547fe93b2ea3aabdb4ec3b59d9261a5268de9203e`. The successful [CODE.2 consumer audit](https://github.com/Hyperbog/RLOFT/actions/runs/43622711732) checked **12/12 executable instruction witnesses for each title**, authenticated both whole binaries, and surveyed all 559/963 Type-5 records and 502/410 Type-6 records in the respective `RES4` files. Its [metadata-only artifact](https://github.com/Hyperbog/RLOFT/actions/runs/43622711732/artifacts/14124259185) records individual offsets, decoded instructions, record distributions, and the measured extensions.

**Reproduce (from repository root, with read-only preservation checkout):**

```sh
python Tools/phase1_compare.py --root preservation/MASTER --json audit/phase1-formats.json --markdown audit/PHASE1_FORMATS.md
python Tools/phase1_code2_consumers.py --root preservation/MASTER --json audit/phase1-code2-consumers.json
```

All CODE.2 addresses below are **LE code-object-relative**, not absolute disk file offsets. Original x86 immediates and stored DAT/DAT archive words use little-endian representation. `VERIFIED` indicates the reported physical operation/shape was checked against the authenticated original executables and/or every relevant record; it does **not** certify its gameplay semantics.

## Cross-title difference table — verified subset

| Family | Structure / field (width, offset, meaning) | Strahd | Menzo | Stone | Relation | Evidence | Confidence |
| --- | --- | --- | --- | --- | --- | --- | --- |
| RES | `+0x00 u32 LE` entry count, `+0x04 u32[] LE` absolute file offsets | RES0: 141 | RES0: 175 | RES0: 174 | IDENTICAL grammar, different counts | 35/35 indexed RES files pass; `Tools/phase1_compare.py` | VERIFIED |
| RES | entry-relative `+0x00..1` EH, `+0x02 u8` type, `+0x03 i16 LE` header length | EH envelope | EH envelope | EH envelope | IDENTICAL envelope | Survey every typed record; zero bad extents | VERIFIED |
| RES | `RES3` asset type | 4,234 Type-7 | 3,167 Type-6 | 2,542 Type-6 | DIFFERENT | `RES3:0` and whole-file type counts | VERIFIED |
| RES | Type-5 header `EH+0x03 i16 LE` size and header payload | no shipped Type-5 | 559 Type-5, header 5 | 963 Type-5, header 5 | ABSENT in Strahd; IDENTICAL Menzo/Stone header geometry | `RES4:0` and all Type-5 entries | VERIFIED physical shape |
| RES | Type-6 header length `EH+0x03 i16 LE` | 0 in Strahd RES4 | 5 in Menzo RES3/4 | 5 in Stone RES3/4 | DIFFERENT | Across RES3/4 archives | VERIFIED |
| LEVELS | `+0x00 u32 LE` count; `+0x04 u32[] LE` indexed record offsets | 32 | 56 | 43 | IDENTICAL grammar | First offset `4 + 4×count`; all entries valid | VERIFIED |
| LEVELS | record `+0x01..0x1C` seven `u32 LE` section boundaries | 29-byte prefix | same | same | IDENTICAL physical preamble | Every level record; 32/56/43 | VERIFIED |
| LEVELS | fixed header beginning at record-relative `+0x1D`; length `u32` selected by readsite | 0x2A4 | 0x824 | 0x824 | DIFFERENT Strahd vs Menzo/Stone | Menzo CODE.2 `0x1BE41`/`0x1BE53`; Stone `0x21E71`/`0x21E83`; all 131 spans | VERIFIED |
| LEVELS | script-block length `u32` read after variable sections | 0x514 | 0x514 | 0x514 | IDENTICAL physical span; VM UNKNOWN | Menzo `0x1C210`/`0x1C21E`; Stone `0x2222D`/`0x2223B`; all 131 records | VERIFIED length only |
| NPCS | `+0x00 u32 LE` count, `+0x04` counted records | 83 | 98 | 110 | IDENTICAL outer layout | `NPCS.DAT:0` each title; original reader count and file size | VERIFIED |
| NPCS | record stride `u32` and `+0x00 char[20]` name | 0x45 | 0x58 | 0x58 | DIFFERENT stride; IDENTICAL name-prefix geometry | Menzo CODE.2 `0x1AF55/0x1AF82`; Stone `0x20EDD/0x20F0A`; 291/291 ASCII names | VERIFIED |
| OBJECTS | record stride `u32` | 0x41 | 0x41 | 0x41 | IDENTICAL physical stride; semantics UNKNOWN | Menzo CODE.2 `0x1B051/0x1B087/0x1B08A`; Stone `0x20FD9/0x21012/0x21015`; 1375 records | VERIFIED stride |
| CHARS | stored image bytes and inferred record stride | 0x920 = 4 × 0x248 | 0x1070 = 4 × 0x41C | 0x1070 = 4 × 0x41C | DIFFERENT Strahd vs Menzo/Stone | Menzo CODE.2 `0x1B69E` and `0x041C0`; Stone `0x216B9` and `0x0473C`; file-reader and indexed-imul instructions | VERIFIED sizes/stride usage; four-record grouping LIKELY |
| Cinematics | primary file extensions | ANM | DFA | DFA + ACT | DIFFERENT | Phase 0 media filenames, no video decoding | VERIFIED names only |

**Important:** `IDENTICAL` above means **physical grammar or measured geometry** only. It never implies identical resource-ID namespaces, field meanings, effects, opcodes, or runtime routing. Where semantics are unknown, the classification does not silently assert them.

## New CODE.2 consumer witnesses

The Menzo NPC reader starts near `0x1AF50`: at `0x1AF55` a 32-bit immediate pushes the data-object address of `NPCS.DAT` (offset `0xFF0`). A four-byte count is read into global `0x3A600`, and `0x1AF82` computes `count × 0x58`. The Stone equivalent uses `NPCS.DAT` data offset `0x124C`, count at `0x480FC` and the multiply at `0x20F0A`. **Meaning:** byte extent of the runtime NPC catalog, not its stat fields. Sample: 98 and 110 file records produce 8,624 and 9,680 bytes after each four-byte count.

The original object catalog routines independently compute allocation based on `0x41` bytes per indexed object, using a shift by 6 and addition. Both executable routines add one to the count for allocation; **do not** infer an extra serialized record. The counted file extents are 523 × 65 and 516 × 65 bytes.

The Menzo/Stone level loader reads and checks exactly `0x824` bytes before the fixed 16-bit tile grid; both also read and check exactly `0x514` bytes for event-script data. The additional fixed-header span relative to Strahd is **0x580 = 1,408 bytes** (Menzo: nonzero values in 706 byte positions across 56 records; Stone: 597 positions across 43). No field names are assigned to this extension.

## Type-5 caution

The Menzo and Stone Type-5 RES4 records **all** have a *five-byte* type header (i16 LE field at entry-relative +3 equals 5). For each title, the five header bytes have two observed patterns: positions +0=3, +1=2, +2=16 or 48, +3=0, +4=0. Their **meaning is UNKNOWN**. Menzo has 85 records with +2=16 and 474 with +2=48; Stone has 63 and 900 respectively.

Strahd's [recovered Type-5 CODE.2 handler](Resources/RES/EXECUTABLE_ONLY_FORMATS.md) at `0x374E0` assumes **at least 28 header bytes** and was reconstructed from executable code without shipped Type-5 examples. Reusing it for these five-byte headers is unjustified. The original Menzo/Stone Type-5 handlers remain to be located and reconstructed. **This is a confirmed inter-title incompatibility risk, not a proven decoder equivalence.**

## Interim Phase 1 questions

- **Q1:** In the surveyed level-header, NPC, Type-5/6, and CHARS geometries, Menzo and Stone are the closer pair. Overall engine similarity remains **LIKELY, not a completed ranking**, until executable codecs and VM semantics are compared.
- **Q2:** RES index/envelope parsing, level-directory/preamble parsing and physical object-record iteration are shareable. Type dispatch, level header extensions, NPC fields, CHARS semantics, event opcodes and title-specific archive routing require further evidence.
- **Q3:** All three have physical `0x514` level event-script sections. A common *opcode grammar* is **UNKNOWN**, not proved by equal section size.
- **Q4:** At minimum, a future TitleProfile will need per-title RES type routing, fixed-level-header bytes, NPC stride/codec, CHARS stride/codec, script VM selector, rules tables and media search policy. This is a provisional evidence-derived inventory, **not a Phase 2 interface design**.
- **Q5:** Second playable title after Strahd: **not selected** until gameplay systems, script VM and character/save semantics are surveyed.

**Still unresolved:** Type-5/6 decoder implementation, additional level-header field semantics, 88-byte NPC field meanings, gameplay use of Stone RES9 record 574, CHARS/SAVE field-level schemas, rules and cinematics. Phase 1 is **not complete**. **Automated tests:** GitHub CODE.2 original-byte/record audit passed. **User-performed tests:** none. **Next Phase 1 investigation:** recover the Type-5/6 decoder dispatch and event-VM entrypoints in Menzo and Stone from authenticated CODE.2 images. No gameplay code has been changed.
