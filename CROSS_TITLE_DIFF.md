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
| LEVELS | fixed header `+0x00A u16LE[]` lookup layout and indexing | 5 × 64 **resource/automap banks**, values 0..46513 | 64 cell types × 4 orientations × 4 u16 channels, values 0..134 | same 64 × 4 × 4 formula, values 0..193 | DIFFERENT resource-table organization; NEVER reinterpret Strahd banks as new-title records | Menzo `0x2B352/2B35A/2B384`, accessor `0x291F2..29244`; Stone accessor `0x33D42..33D94`; see `Tools/phase1_npc_table_provenance.py` | VERIFIED index arithmetic; individual channel meanings UNKNOWN |
| LEVELS | fixed-header `u16LE` family A,C,B counts; each maps to `11-byte` records | `+0x28A/28C/28E` | `+0x80A/80C/80E` | `+0x80A/80C/80E` | SAME physical count semantics; DIFFERENT offsets | `(boundary_3 - boundary_2)/11`, `(boundary_5 - boundary_4)/11`, `(boundary_4 - boundary_3)/11`; all 131 levels; original CODE.2 direct readers | VERIFIED |
| LEVELS | script-block length `u32` read after variable sections | 0x514 | 0x514 | 0x514 | IDENTICAL physical span; VM UNKNOWN | Menzo `0x1C210`/`0x1C21E`; Stone `0x2222D`/`0x2223B`; all 131 records | VERIFIED length only |
| NPCS | `+0x00 u32 LE` count, `+0x04` counted records | 83 | 98 | 110 | IDENTICAL outer layout | `NPCS.DAT:0` each title; original reader count and file size | VERIFIED |
| NPCS | record stride `u32` and `+0x00 char[20]` name | 0x45 | 0x58 | 0x58 | DIFFERENT stride; IDENTICAL name-prefix geometry | Menzo CODE.2 `0x1AF55/0x1AF82`; Stone `0x20EDD/0x20F0A`; 291/291 ASCII names | VERIFIED |
| NPCS | definition `u8` HP/HD input and original constructor | `+0x1D` (Strahd hit dice) | `+0x30` (d8 dice-count input) | `+0x30` (d8 or 6/7/8 multiplier by runtime mode) | SAME PURPOSE, DIFFERENT field offset and formula | Menzo `0x104F8`→`0x20818`; Stone `0x13844`→`0x29F08`; sample indices 0/1 | VERIFIED HP-initializer input; specific runtime mode intent UNKNOWN |
| NPCS | definition `+0x48 u32LE` initialization state, copied to live `+0x0E u32` | `+0x35` copied to live `+0x08` | `+0x48`→live `+0x0E` | `+0x48`→live `+0x0E` | SAME PURPOSE, DIFFERENT offset | Menzo `0x104D5/0x104EA`, Stone `0x137D2/0x137E4` | VERIFIED copied initial state; individual bits partly UNKNOWN |
| NPCS | `+0x1D u8 attack_count`, two 9-byte attack records at `+0x1E/+0x27` | Absent; one attack formula at `+0x1E..+0x20` | counts 0/1/2 = 67/15/16; six zero-count records retain residual descriptor bytes | counts 0/1/2 = 80/15/15; one counted second slot entirely zero | DIFFERENT; preserve inactive slots verbatim | Menzo `0x1FF3B/0x20077` and `0xFB0C..FB30`; Stone `0x275F0/0x277A5` and `0x12B67..12B8D`; original 208 records | VERIFIED attack count/stride/dice fields; auxiliary field aliases PARTIAL |
| NPCS | behavior-state bytes `+0x4C / +0x4D` (two `u8`) | `+0x39 / +0x3A` | `+0x4C / +0x4D` | `+0x4C / +0x4D` | SAME-LAYOUT-DIFFERENT-SEMANTICS (bits not equivalent by default) | Menzo `0xFBC7/0xFC46`; Stone `0x23995/0x2399F` | VERIFIED bit accesses; intent UNKNOWN |
| NPCS | four `u16LE` sound/resource ID candidates | `+0x3D/3F/41/43` | `+0x50/52/54/56` | `+0x50/52/54/56` | DIFFERENT offsets; route-specific behavior | Menzo `0x20103/0xFD7F/0x10676/0x1061B` call audio dispatcher; Stone `0x12A33` uses `+0x52` | VERIFIED widths/readsites; complete per-ID semantics LIKELY |
| LEVELS | live family-A runtime record allocation stride | 22 bytes (`0x16`) | 46 bytes (`0x2E`) | 50 bytes (`0x32`) | DIFFERENT | original per-type constructor `imul` Menzo `0x1048D` / Stone `0x1378A`, `DatFiles/LEVELS.DAT/FORMAT.md` for Strahd | VERIFIED indexed live allocation |
| OBJECTS | record stride `u32` | 0x41 | 0x41 | 0x41 | IDENTICAL physical stride; semantics UNKNOWN | Menzo CODE.2 `0x1B051/0x1B087/0x1B08A`; Stone `0x20FD9/0x21012/0x21015`; 1375 records | VERIFIED stride |
| CHARS | stored image bytes and inferred record stride | 0x920 = 4 × 0x248 | 0x1070 = 4 × 0x41C | 0x1070 = 4 × 0x41C | DIFFERENT Strahd vs Menzo/Stone | Menzo CODE.2 `0x1B69E` and `0x041C0`; Stone `0x216B9` and `0x0473C`; file-reader and indexed-imul instructions | VERIFIED sizes/stride usage; four-record grouping LIKELY |
| Cinematics | primary file extensions | ANM | DFA | DFA + ACT | DIFFERENT | Phase 0 media filenames, no video decoding | VERIFIED names only |

**Important:** `IDENTICAL` above means **physical grammar or measured geometry** only. It never implies identical resource-ID namespaces, field meanings, effects, opcodes, or runtime routing. Where semantics are unknown, the classification does not silently assert them.

## New CODE.2 consumer witnesses

The Menzo NPC reader starts near `0x1AF50`: at `0x1AF55` a 32-bit immediate pushes the data-object address of `NPCS.DAT` (offset `0xFF0`). A four-byte count is read into global `0x3A600`, and `0x1AF82` computes `count × 0x58`. The Stone equivalent uses `NPCS.DAT` data offset `0x124C`, count at `0x480FC` and the multiply at `0x20F0A`. **Meaning:** byte extent of the runtime NPC catalog, not its stat fields. Sample: 98 and 110 file records produce 8,624 and 9,680 bytes after each four-byte count.

The original object catalog routines independently compute allocation based on `0x41` bytes per indexed object, using a shift by 6 and addition. Both executable routines add one to the count for allocation; **do not** infer an extra serialized record. The counted file extents are 523 × 65 and 516 × 65 bytes.

The Menzo/Stone level loader reads and checks exactly `0x824` bytes before the fixed 16-bit tile grid; both also read and check exactly `0x514` event-script bytes. **The 0x580-byte expansion is accounted for by a `u16LE[320]`→`u16LE[1024]` indexed table at header `+0x0A`**, ahead of three 16-bit record counts at `+0x80A/0x80C/0x80E`. The 1024-entry table is directly indexed by Menzo CODE.2 at `0x21837` and Stone CODE.2 at `0x2B100` (base pointers `0x39DA6`/`0x447D6`). The table's **gameplay/resource semantics remain UNKNOWN**, and Strahd values include numbers above 255 that the other two corpora do not. The remaining tail bytes `+0x810..+0x823` have direct original-code readers; do not treat them as unused padding.

### Phase 1: NPC attack records and tile-code/orientation channels

**VERIFIED original definition grammar:** `NPCS.DAT` record `+0x1D u8 attack_count` is 0..2. The following `+0x1E..+0x26` and `+0x27..+0x2F` are two nine-byte attack descriptors. The first three bytes of each descriptor are **damage dice count, die sides and unsigned damage modifier**: Menzo CODE.2 at `0xFB13..0xFB30` indexes `(live_attack_index-1)*9`, loads those bytes and calls `0x20818`; Stone at `0x12B6C..0x12B8D` does the same and calls `0x29ED0` with runtime dice mode. Both games' attack-choice code explicitly checks `attack_count == 2` before separate two-attack control paths (Menzo `0x20077`, Stone `0x277A5`). Descriptor-relative `+5` is read as the divisor of the attack-phase counter; `+8` is compared with the resulting phase. Descriptor-relative `+3` has tested control bits (e.g., Menzo `0x1FF30/1FF41`), but its exact flag names and descriptor `+4/+6/+7` remain UNKNOWN. **Losslessness:** Menzo has six `attack_count=0` definitions with nonzero stale slots and one `attack_count=1` definition retaining a second slot; Stone has one two-attack definition with a zeroed second slot. Never normalize inactive slots.

**VERIFIED new title lookup geometry:** The header `+0x00A..+0x809` is accessed by `u16LE table[64][4][4]`: **six-bit cell type × orientation × channel**. Menzo `0x2B352/0x2B35A/0x2B384` extracts `cell_word & 0x3F` and uses a 32-byte row; both games' accessors multiply orientation by eight bytes and select four 16-bit values (Menzo `0x291FE..29244`, Stone `0x33D4E..33D94`). This is not interchangeable with **Strahd's five separate 64-entry resource/automap arrays**. Both titles' actual accessors take `(packed_cell_word & 0x3F, caller_byte_0x0E, caller_byte_0x0F)` at Menzo `0x25B8E/0x25D63` and Stone `0x2E760/0x2EA64`, and the returned 16-bit value is compared against a caller-provided expected value. **VERIFIED inputs and comparison; channel/resource names are not yet certified.** For additional corpus-level corroboration, group zero is empty in every Menzo/Stone level; of cell-code groups used in maps, **803 Menzo** and **712 Stone** groups have nonzero corresponding table entries (code 0 and some other used codes legitimately have empty groups). See `Tools/phase1_npc_region_level_groups.py`, `phase1_npc_table_provenance.py`, and `phase1_npc_attack_descriptors.py`.

### Phase 1: attack-control effects and tile-channel consumers

**NPC attack-control bit behavior (VERIFIED machine paths).** Menzo CODE.2 tests attack-descriptor-relative `+3 & 0x0C` in both attack slots (`0x1FF30/1FF41`), and the same mask in the target-selection path (`0x2004E/20059`) can set a distance/selection threshold to **`0xB0000`** (`0x20054/2005F`). The meaning of individual bits 2 and 3 is not independently established. Stone CODE.2 `0x275F9..27649` tests the second attack's `+3 & 0x8D`, checks live actor state at `+0x0E`, and applies random thresholds of 20 and 70 before possible alternate-attack selection. Neither path proves a universal cross-title flag enumeration.

**All serialized auxiliary attack bytes were surveyed** by [the attack-control analyzer](Tools/phase1_attack_control_channels.py) and [CODE.2 effect regression](Tools/phase1_attack_effect_proof.py). Across **47 active Menzo attacks**, relative `+4/+6` are zero, `+5` (cycle divisor) is **3**, `+7` is **0x80**, and `+8` (trigger phase) is **1**. Across **45 declared active Stone attacks**, `+4/+6` are zero; `+5` has **five zeros, thirty-seven 3s, two 4s, one 6**; `+7` is **0x80 in 40** and zero in five; `+8` has **five zeros, sixteen 1s, twenty-three 2s, one 3**. **Five Stone active slots have cycle divisor zero**: four retain other attack data and one is completely empty. Do not silently divide by zero, invent a populated attack, or repair original bytes; the original runtime guards for those four populated slots are still UNKNOWN. Relative `+7` has no validated effect readsite and remains opaque.

**Four table channels are distinct in the corpus**, rather than identical copies. For each orientation and each of 64 code groups, the counts of nonzero channel entries across all archived levels are **Menzo [6,041, 6,319, 4,658, 4,318]** and **Stone [3,771, 3,835, 1,419, 1,466]**. Exact original lookup callers require record flag `+0x01 & 0x04`, compare `table[cell_code][selector_0x0E][selector_0x0F]` to a desired 16-bit value, then test rectangle bounds from record bytes `+0x0A..+0x0D`. Their returned match value feeds a higher-level **10-case Menzo / 11-case Stone dispatch** (Menzo `0x27576..275E9`, Stone `0x308BB..3092E`). Specific game events downstream remain UNKNOWN; do not label every channel a texture. The low six bits of the packed grid cell and channel bounds are original-code-backed.

**Menzo table modification (VERIFIED operation, not observed execution):** CODE.2 `0x2B352..2B3BB` selects low-six-bit cell codes **0x33..0x3E** when a separate runtime threshold holds. For each of the group's sixteen `u16LE` values `v >= 0x5A`, it can store **`v - (v % 5) + 4`**. This is actual mutable table-state arithmetic. Under hypothetical satisfaction of the runtime gate, **184 original archived table words in six levels** would change; the tool does not execute the game or alter any record. Corresponding Stone mutation logic is not yet established.

## Type-5 caution

The Menzo and Stone Type-5 RES4 records **all** have a *five-byte* type header (i16 LE field at entry-relative +3 equals 5). For each title, the five header bytes have two observed patterns: positions +0=3, +1=2, +2=16 or 48, +3=0, +4=0. Their **meaning is UNKNOWN**. Menzo has 85 records with +2=16 and 474 with +2=48; Stone has 63 and 900 respectively.

Strahd's [recovered Type-5 CODE.2 handler](Resources/RES/EXECUTABLE_ONLY_FORMATS.md) at `0x374E0` assumes **at least 28 header bytes** and was reconstructed from executable code without shipped Type-5 examples. Reusing it for these five-byte headers is unjustified. The original Menzo/Stone Type-5 handlers are now located at CODE.2 `0x3E6E0` / `0x4C2C0`; stream decoding is documented below, with the 148-record declared-length discrepancy still unresolved. **This is a confirmed inter-title incompatibility risk, not a proven decoder equivalence.**

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

The new [Menzo/Stone RES9 opcode reconstruction](Resources/RES/CROSS_TITLE_WORDCODE_VM_PHASE1.md) identifies authenticated CODE.2 dispatch tables of **43** and **54** entries, both selecting the target as `u16le_encoded_opcode - 2`. The corresponding source-checked handlers have materially different operand footprints from Strahd for early instructions, including encoded opcode `0x03` (two fixed words in both newer games). [Verified readsite extractor](Tools/phase1_res9_handler_map.py), [source-driven width hypothesis](Tools/phase1_res9_arity_correlate.py) and [original-media CI](.github/workflows/phase1-res9-opcodes.yml) preserve full tables and negative controls. The CODE.2-corroborated serialized parser now consumes **31/31 Menzo** and **48/48 Stone** programs; **876** and **1,473** checked direct targets respectively land on decoded instruction starts, with **zero** boundary mismatches. Stone's ten former targets `999` were parser-alignment artifacts: opcode `0x08` is a two-word conditional, and opcode `0x2E` accepts `(999,21,27)`, where `999` is explicitly interpreted by helper `0x1D594` as an empty-value match and `27` is the real branch target. The **45 Menzo** and **24 Stone** `0xFFFF` timer operands are still classified separately. Full gameplay semantics are not established.

### Explicit hypotheses for unresolved producer/VM differences

- **Type-5 0x1002 length discrepancy — HYPOTHESIS A:** a two-byte logical header is represented by the declared allocation length but is not emitted by the LZSS stream; the stream itself supplies 4,096 bytes. **HYPOTHESIS B:** the producer declared an allocation/expected output length that exceeds what the compressed stream yields, and the original reader tolerates a short stream or otherwise initializes the last two bytes. Both are consistent with 148/148 full-consumption 4,096-byte prefix decodes; neither is certified without tracing the original low-level resource reader and its end-of-stream behavior.
- **Menzo/Stone interaction VM — VERIFIED:** Both binaries subtract two from an encoded `u16LE` opcode and dispatch 43/54 cases. Original transitive handlers establish opcode-05 character-template operands, conditional choice tables terminated by `0x21`/`0x26`, and the two-word Stone opcode `0x08`. All four formerly failing Stone scripts (568, 575, 585, 595) now parse with aligned targets; `999` is an empty-value sentinel in opcode `0x2E`, never a target in records 615..624. **UNKNOWN:** complete opcode gameplay effects and actual runtime state/trigger behavior.
- **Concrete Menzo event-VM sample:** `LEVELS.DAT` zero-based level 8, script slot 1, command 0 contains opcode **13**, and level 8 slot 2 contains opcode 13 in commands 0 and 1. The source-byte interpreter at `0x267` accepts opcodes 0..13 and dispatches 13 to `0x659`. *Meaning of case 13 remains UNKNOWN.* **Confidence: VERIFIED for opcode value/selection.**

**Blocking unknowns:** Type-5's two extra declared decoded bytes; RES9 execution and `0xFFFF` timers; event triggers; individual attack-control bit names, descriptor-relative `+7` and Stone zero-period attack reachability; downstream event consequences and resource identities of the four tile-table channels, Stone table mutation rules and the 20-byte level tail; CHARS/SAVE and rules codecs. None was inferred from absent or leftover media. Phase 1 remains **IN PROGRESS**; no native player code changed.

## Interim Phase 1 questions

- **Q1:** In the surveyed level-header, NPC, Type-5/6, and CHARS geometries, Menzo and Stone are the closer pair. Overall engine similarity remains **LIKELY, not a completed ranking**, until executable codecs and VM semantics are compared.
- **Q2:** RES index/envelope parsing, level-directory/preamble parsing and physical object-record iteration are shareable. Type dispatch, level header extensions, NPC fields, CHARS semantics, event opcodes and title-specific archive routing require further evidence.
- **Q3:** All three have physical `0x514` level event-script sections. A common *opcode grammar* is **UNKNOWN**, not proved by equal section size.
- **Q4:** At minimum, a future TitleProfile will need per-title RES type routing, fixed-level-header bytes, NPC stride/codec, CHARS stride/codec, script VM selector, rules tables and media search policy. This is a provisional evidence-derived inventory, **not a Phase 2 interface design**.
- **Q5:** Second playable title after Strahd: **not selected** until gameplay systems, script VM and character/save semantics are surveyed.

**Still unresolved:** Type-5 stream-length anomalies; individual attack-control mask semantics, descriptor byte `+7`, four Stone nonempty declared zero-divisor slots' reachable call paths and additional NPC status/sound aliases; tile-table channel ID interpretation and higher-level action dispatch, Stone dynamic table modifications and header tail; Stone RES9:574 asset precedence, CHARS/SAVE, rules, cinematics and behavioral interaction VM. Phase 1 is **IN PROGRESS**. **Automated tests:** 79/79 Menzo/Stone RES9 programs and 2,349/2,349 direct targets passed previously; the current authenticated NPC/LEVELS audit validates **208 NPC definitions, 99 enlarged headers and the 32-level Strahd reference** with SHA-pinned media and original CODE.2 field readsites. **User-performed tests:** none. **Next:** prove specific attack control bits, Stone zero-period call guards, and the gameplay outcomes of tile-filter action dispatch; no native gameplay code changed.
