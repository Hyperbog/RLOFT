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

## Phase 1 continuation — resource decoder and event VM (2026-11-14)

**Reproducible checks:**

```sh
python Tools/phase1_resource_event_vm.py --root preservation/MASTER --repo-root . --json audit/phase1-resource-event-vm.json
python Tools/phase1_decoder_dispatch.py --root preservation/MASTER --repo-root . --json audit/phase1-decoder-dispatch.json
python Tools/phase1_type56_verify.py --root preservation/MASTER --repo-root . --json audit/phase1-type56-lzss.json
```

All executed on the read-only original media; the last command reuses the **unchanged** `Resources/RES/analyze_res.py` Strahd LZSS routine. [Successful machine-code dispatcher audit](https://github.com/Hyperbog/RLOFT/actions/runs/43632586957); Type-5/6 corpus verification results and explicit anomalies are included in the workflow artifact. CODE.2 whole-file identities remain the pinned hashes above.

### Level-event script VM

Every level record in Strahd (32), Menzo (56), Stone (43) has `0x514 = 50 × 0x1A` bytes between boundaries 5 and 6. Interpreting each 26-byte slot as `u8 command_count` followed by five `[u8 opcode,u8 arg1,u8 arg2,u8 arg3,u8 arg4]` commands produces **zero invalid counts (greater than 5) in all 6,550 slots**. Active commands: Strahd **555**, Menzo **393**, Stone **469**. This is a **VERIFIED shared physical grammar**; inactive command bytes must be retained losslessly.

Executable inspection independently confirmed `slot_index × 0x1A`, `command_index × 5` and a type-indexed jump table. Strahd's interpreter begins at CODE.2 `0x2A8` and has **13 cases (0..12)** in the table at `0x274` (see `DatFiles/LEVELS.DAT/EVENT_SCRIPTS.md`); Menzo **14 cases (0..13)** at `0x272` / table `0x1C8`; Stone **14 cases** at `0x313` / table `0x264`. **Correction:** the original Strahd table is `0x274` and its executor begins `0x2A8`; the two new address pairs are direct CODE.2 witnesses. Menzo's corpus executes opcode **13 eight times**, while Stone's shipped levels execute only up to **12** even though its binary supports 13.

| Opcode | Strahd handler | Menzo handler | Stone handler | Evidence status |
| --- | ---: | ---: | ---: | --- |
| 0 | `0x306` | `0x27A` | `0x31B` | VERIFIED dispatch offsets |
| 1 | `0x378` | `0x2E3` | `0x39B` | VERIFIED |
| 2 | `0x3D2` | `0x35D` | `0x406` | VERIFIED |
| 3 | `0x48D` | `0x409` | `0x4B5` | VERIFIED |
| 4 | `0x454` | `0x3D4` | `0x480` | VERIFIED |
| 5 | `0x540` | `0x4EF` | `0x594` | VERIFIED |
| 6 | `0x559` | `0x508` | `0x5AD` | VERIFIED |
| 7 | `0x5B3` | `0x572` | `0x617` | VERIFIED |
| 8 | `0x57D` | `0x53C` | `0x5E1` | VERIFIED |
| 9 | `0x58F` | `0x54E` | `0x5F3` | VERIFIED |
| 10 | `0x5A1` | `0x560` | `0x605` | VERIFIED |
| 11 | `0x4E6` | `0x4C4` | `0x574` | VERIFIED |
| 12 | `0x68F` | `0x64E` | `0x6F3` | VERIFIED |
| 13 | absent | `0x659` | `0x6FE` | VERIFIED, semantics UNKNOWN |

These original disassembly destinations are **code-object-relative 32-bit little-endian jump-table entries**, not raw bytes copied into the source. A shared event-slot **decoder** is justified; a shared **opcode executor** is not yet certified. Both Menzo/Stone case-13 paths load three command bytes and invoke the original call target `0x10`; its effect remains UNKNOWN.

### Type-5 and Type-6 archive record decoders

Both Menzo/Stone CODE.2 EH dispatchers subtract one from the original `u8` type and dispatch **Types 1..6**. Menzo dispatcher `0x34E4D` with table `0x34CD8`; Stone `0x40DF1` with table `0x40C88`. Type-5 and Type-6 executable handlers:

| Type | Menzo CODE.2 | Stone CODE.2 | Shared physical header |
| --- | --- | --- | --- |
| 5 | `0x3E6E0` | `0x4C2C0` | `EH + u8 type=5 + i16LE header_len=5 + u8 compression + u32LE decoded_len` |
| 6 | `0x3E778` | `0x4C358` | `EH + u8 type=6 + i16LE header_len=5 + u8 compression + u32LE decoded_len` |

Both functions select compression **0, 1 or 3**, with method 3 entering their LZSS path; all observed Type-5/6 entries use compression **3**. Methods 0/1 are executable-supported but lack the current cross-title resource samples. This **differs** from Strahd's shipped Type-6/7 zero-extra-header variant and Strahd's uninstantiated Type-5 28-byte loader contract.

**Actual Strahd LZSS oracle across original Menzo/Stone assets:**

| Archive | Records | Method-3 exact output + full input consumption | Exceptions |
| --- | ---: | ---: | --- |
| Menzo RES3 Type-6 | 3,167 | 3,167 | 0 |
| Menzo RES4 Type-6 | 502 | 502 | 0 |
| Stone RES3 Type-6 | 2,542 | 2,542 | 0 |
| Stone RES4 Type-6 | 410 | 409 | Record **1309**: 14 decoded bytes, 11/15 encoded bytes consumed; 4-byte tail |
| Menzo RES4 Type-5 | 559 | 474 | **85** of declared output size 4,098 |
| Stone RES4 Type-5 | 963 | 900 | **63** of declared output size 4,098 |

Every **1,374** Type-5 record whose declared decoded length is **12,290** succeeds exactly. All **148** records declaring **4,098** bytes fail the Strahd oracle at that length but **succeed and consume the complete compressed payload at 4,096 bytes**. **VERIFIED** decoded-prefix and full-consumption observation, **UNKNOWN** producer reason for the two extra declared bytes or whether the original CODE.2 runtime deliberately initializes/preserves them separately. Keep the header `0x1002` **byte-exact** and the unaccounted two-byte semantic status **UNKNOWN**; do not silently rewrite it.

### RES9 interaction-program variants and Stone revision

Strahd **38/38** and Menzo **31/31** non-EH script records match `u16LE word_count; u16LE program_words[count]; u16LE zero_bank[count]`. The unchanged Strahd `0x00..0x29` wordcode parser decodes **38/38 Strahd**, **0/31 Menzo**: Menzo clearly requires opcode/arity verification and is not yet compatible semantically.

Stone has **48/49** nonempty non-EH records matching `u16LE word_count; u16LE program_words[count]` with **no zero bank**. Record **625** is a distinct four-byte raw marker (`u16LE at +0 = 244`); its purpose is **UNKNOWN**. Strahd's instruction grammar parses **0/48** Stone scripts, initially encountering opcode-domain/operand disagreement at word 6 in the common prefix. This does **not** mean those programs are invalid.

Stone RES9 record **574**, 634 bytes, begins `u16LE word_count=316`. Between `PROPHET/RES9` and `CD_Extracted/RES9`, exactly five 16-bit program words differ: zero-based word indices **246, 261, 268, 273, 313**, in that order `30→10, 31→11, 30→10, 31→11, 31→11` (installed → CD). **VERIFIED byte-level revision, UNKNOWN opcode/operand meaning**. The original asset-selection precedence is still not proven.

### RES9 original executable opcode/operand handoff (latest Phase 1)

The new [Menzo/Stone RES9 opcode reconstruction](Resources/RES/CROSS_TITLE_WORDCODE_VM_PHASE1.md) identifies authenticated CODE.2 dispatch tables of **43** and **54** entries, both selecting the target as `u16le_encoded_opcode - 2`. The corresponding source-checked handlers have materially different operand footprints from Strahd for early instructions, including encoded opcode `0x03` (two fixed words in both newer games). [Verified readsite extractor](Tools/phase1_res9_handler_map.py), [source-driven width hypothesis](Tools/phase1_res9_arity_correlate.py) and [original-media CI](.github/workflows/phase1-res9-opcodes.yml) preserve full tables and negative controls. Preliminary fixed-arity coverage is **5/31 Menzo**, **20/48 Stone**; this is **PARTIAL**, not a finished VM parser or gameplay implementation.

### Explicit hypotheses for unresolved producer/VM differences

- **Type-5 0x1002 length discrepancy — HYPOTHESIS A:** a two-byte logical header is represented by the declared allocation length but is not emitted by the LZSS stream; the stream itself supplies 4,096 bytes. **HYPOTHESIS B:** the producer declared an allocation/expected output length that exceeds what the compressed stream yields, and the original reader tolerates a short stream or otherwise initializes the last two bytes. Both are consistent with 148/148 full-consumption 4,096-byte prefix decodes; neither is certified without tracing the original low-level resource reader and its end-of-stream behavior.
- **Menzo/Stone interaction VM — HYPOTHESIS A:** both extend Strahd's `0..0x29` opcode set (e.g. Menzo program RES9 index 419 reaches apparent `0x2A`, Stone index 567 apparent `0x37`). **HYPOTHESIS B:** one or more inherited opcode operand counts changed, so existing data operands appear as invalid Strahd opcodes (as happened historically in Strahd RES12). Reconstruct the original Menzo/Stone wordcode dispatch/operand fetchers before deciding.
- **Concrete Menzo event-VM sample:** `LEVELS.DAT` zero-based level 8, script slot 1, command 0 contains opcode **13**, and level 8 slot 2 contains opcode 13 in commands 0 and 1. The source-byte interpreter at `0x267` accepts opcodes 0..13 and dispatches 13 to `0x659`. *Meaning of case 13 remains UNKNOWN.* **Confidence: VERIFIED for opcode value/selection.**

**Blocking unknowns:** original executable meaning of Type-5's two extra declared decoded bytes; full Menzo/Stone RES9 instruction grammar and wordcode dispatcher; cross-title event opcode semantics and triggers; remaining NPC/level/character/rules codecs. None was inferred from absent or leftover media. Phase 1 remains **IN PROGRESS**; no native player code changed.

## Interim Phase 1 questions

- **Q1:** In the surveyed level-header, NPC, Type-5/6, and CHARS geometries, Menzo and Stone are the closer pair. Overall engine similarity remains **LIKELY, not a completed ranking**, until executable codecs and VM semantics are compared.
- **Q2:** RES index/envelope parsing, level-directory/preamble parsing and physical object-record iteration are shareable. Type dispatch, level header extensions, NPC fields, CHARS semantics, event opcodes and title-specific archive routing require further evidence.
- **Q3:** All three have physical `0x514` level event-script sections. A common *opcode grammar* is **UNKNOWN**, not proved by equal section size.
- **Q4:** At minimum, a future TitleProfile will need per-title RES type routing, fixed-level-header bytes, NPC stride/codec, CHARS stride/codec, script VM selector, rules tables and media search policy. This is a provisional evidence-derived inventory, **not a Phase 2 interface design**.
- **Q5:** Second playable title after Strahd: **not selected** until gameplay systems, script VM and character/save semantics are surveyed.

**Still unresolved:** Type-5/6 decoder implementation, additional level-header field semantics, 88-byte NPC field meanings, gameplay use of Stone RES9 record 574, CHARS/SAVE field-level schemas, rules and cinematics. Phase 1 is **not complete**. **Automated tests:** GitHub CODE.2 original-byte/record audit passed. **User-performed tests:** none. **Next Phase 1 investigation:** recover the Type-5/6 decoder dispatch and event-VM entrypoints in Menzo and Stone from authenticated CODE.2 images. No gameplay code has been changed.
