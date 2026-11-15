# Menzoberranzan — original data formats (Phase 1, partial)

**Scope:** Executable-backed file geometry for CODE.2, RES4 Type-5/6, LEVELS.DAT, NPCS.DAT, OBJECTS.DAT and CHARS. Semantics of unexamined fields are UNKNOWN. See [CROSS_TITLE_DIFF.md](../../CROSS_TITLE_DIFF.md) and [original Strahd format notes](../NPCS.DAT/FORMAT.md).

**Original executable:** `MASTER/Menzoberranzan/CODE.2`, 650,385 bytes, SHA-256 `4a4fa725dddea00cb62ff44f44c48999cf6ed4c899b16c21826e8141ec7fda96`. Every code address below is an offset within its unpacked LE code object. The audited `Tools/phase1_code2_consumers.py` matches twelve exact instructions; no commercial bytes are bundled.

## Binary field geometry

| File | Offset, width, endian | Observed data | Reader evidence | Confidence |
| --- | --- | --- | --- | --- |
| NPCS.DAT | file `+0 u32 LE` count | 98 | read into global `0x3A600` | VERIFIED |
| NPCS.DAT | file `+4`, 98 × 0x58-byte records | 8,624 data bytes | CODE.2 `0x1AF55` filename, `0x1AF82` count × 0x58 | VERIFIED |
| NPCS.DAT | record `+0 char[20]` | 98/98 printable NUL-terminated names | full-record corpus probe | VERIFIED physical field |
| NPCS.DAT | record `+0x45..+0x57`, 19 bytes | nonzero in 9/19 byte-columns across file | per-column tally, not semantic decoding | VERIFIED physical extension |
| OBJECTS.DAT | file `+0 u32 LE` count | 523 | original CODE.2 `0x1B051` filename and counted allocation | VERIFIED |
| OBJECTS.DAT | record stride 0x41 (65 bytes) | 523 serialized entries | `0x1B087` shift 6 + `0x1B08A` addition | VERIFIED |
| LEVELS.DAT | file `+0 u32 LE` count, `+4 u32[] LE` offsets | 56 levels, first record at 0xE4 | 56/56 valid indexed offsets | VERIFIED |
| LEVELS.DAT | record `+1` seven `u32 LE` boundaries | 29-byte preamble | all 56 records | VERIFIED |
| LEVELS.DAT | record `+0x1D` fixed header, 0x824 bytes | 56/56 exact header-and-grid equation | CODE.2 `0x1BE41` push size, `0x1BE53` compare read length | VERIFIED |
| LEVELS.DAT | counted event block, 0x514 bytes | 56/56 matching boundaries | CODE.2 `0x1C210` push size, `0x1C21E` compare length | VERIFIED physical block |
| CHARS | full read size 0x1070; indexed access stride 0x41C | 0x1070 = 4 × 0x41C | CODE.2 `0x1B69E` push 0x1070; `0x041C0` indexed multiply 0x41C | VERIFIED operations; four-record interpretation LIKELY |
| RES4 | indexed entry-relative `+3 i16 LE` type header length | Type-5: 559 entries of header length 5; Type-6: 502 of length 5 | full typed-record corpus scan | VERIFIED |

### NPC definition and expanded level-header field evidence

**Attack descriptor auxiliary reads.** Each nine-byte record's relative `+3` has CODE.2 control-bit tests (e.g., `0x1FF30/0x1FF41`, mask `0x0C`). Relative `+5` is used as a modulo divisor for the live attack-phase counter (`0x203C5..0x203CF`); relative `+8` is compared with the resulting phase (`0x203E3`). Other bytes retain raw values because their purpose is not completely proved. Inactive descriptor bytes are **not guaranteed zero**: six records with attack count zero retain nonzero attack data, as does the second slot of one one-attack record. Preserve them byte-for-byte. [Exact readsite and full-corpus test](../../Tools/phase1_npc_attack_descriptors.py).

The original NPCS.DAT SHA-256 is `ad365e5da571e95306cf20f226399088563a7cc5d371d53db971e8c1e8449e39` (98 definitions, each `0x58` bytes). Data positions are **definition-record-relative**, not runtime offsets.

| Original field | Width | Executable-backed operation | Confidence |
| --- | --- | --- | --- |
| `+0x30` | `u8` | Normal NPC constructor `0x104F8` passes this hit-dice count to `0x20818` with 8-sided dice; the helper rolls and sums, minimum 1 | VERIFIED HP calculation |
| `+0x48` | `u32LE` | `0x104D5/0x104EA` copies original definition value unchanged to live-record `+0x0E` | VERIFIED initialization state copy; bit meanings incomplete |
| `+0x4C/+0x4D` | 2 × `u8` | Original flag tests include `0xFBC7` and `0xFC46` | VERIFIED bit tests; overall gameplay semantics UNKNOWN |
| `+0x50/52/54/56` | 4 × `u16LE` | Read as separate audio/resource index fields; callsites `0x20103`, `0xFD7F`, `0x10676`, `0x1061B` use resource base `0x3A63C` and dispatcher `0x302B8` | VERIFIED widths/readsites, individual aliases LIKELY |
| `+0x1D` / `+0x1E..+0x26` / `+0x27..+0x2F` | `u8 attack_count; attack[2]` (9 bytes each) | 98 definitions: counts 0/1/2 = 67/15/16. Each attack starts `u8 dice_count, u8 die_sides, u8 damage_bonus`. | CODE.2 `0x1FF3B/0x20077` checks two attacks; `0xFB13..0xFB30` indexes `(live_attack_index−1)*9` and calls damage dice helper `0x20818`. | VERIFIED physical attack grammar and damage operands |

Example: NPC index **0** has hit-dice count **6** at `+0x30` and initial-state dword `0x20000000` at `+0x48`. The original NPC *definition* stride `0x58` is distinct from the constructed *live* NPC stride **`0x2E`** (46 bytes), shown by CODE.2 `0x1047C/0x1048D`.

The LEVELS.DAT SHA-256 is `d3509c9288cc5b1e14b6a9e6c4e2d496e336b3dcaca05555a65e5ed4e564d8aa`. Every one of its **56** levels has the following `0x824`-byte fixed header, beginning at record-relative `+0x1D`:

| Header offset | Physical field and evidence | Confidence |
| --- | --- | --- |
| `+0x00A..+0x809` | `u16LE[64][4][4]`: cell-code group × orientation × four channels | CODE.2 `0x2B352/0x2B35A` reads grid cell and masks low six bits, `0x2B384` selects a 32-byte group; `0x291FE..0x29244` indexes orientation×8 bytes and channel. Individual channel semantics UNKNOWN. | VERIFIED index arithmetic and table grouping |
| `+0x80A` | `u16LE` family A count, equals `(boundary_3−boundary_2)/11` in 56/56 levels | VERIFIED |
| `+0x80C` | `u16LE` family C count, equals `(boundary_5−boundary_4)/11` in 56/56 levels | VERIFIED |
| `+0x80E` | `u16LE` family B count, equals `(boundary_4−boundary_3)/11` in 56/56 levels | VERIFIED |
| `+0x810..+0x823` | 20-byte tail with populated fields and original direct readers; individual meanings not yet decoded | VERIFIED raw span; semantics UNKNOWN |

The header table is grouped by **64 cell codes × 4 orientations × 4 u16 channels**, unlike Strahd's five independent 64-word resource/automap banks. Indexed entry zero is empty across all archived levels; rendering uses 32-byte code groups and eight-byte orientation subgroups. The precise asset/channel meaning is not assigned merely from these dimensions. [Executable index proof](../../Tools/phase1_npc_table_provenance.py).

The widened table has **21,336 nonzero entries** across 56 levels, encoded values **0..134**, and no value above 255. For example, level index **0** has **70×70** cells and A/C/B counts **26/27/133**. The table is **704 additional u16 entries** relative to Strahd's 320-entry layout, explaining the entire `0x580`-byte difference in header length; identical field meanings are not presumed.

**Reproduce with immutable `DnD-Classics/MASTER` media:**

```sh
python Tools/phase1_npc_level_fields.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-fields.json
python Tools/phase1_npc_level_consumers.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-consumers.json
python Tools/phase1_npc_level_semantics.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-semantic-witnesses.json
```

### RES4 Type-5 header (physical-only)

At `RES4` entry 0: `+0..1` two-byte EH marker, `+2 u8` type=5, `+3 i16 LE` header_length=5. The next five bytes at entry `+5..+9` are constant at positions 0=3, 1=2, 3=0, 4=0; position 2 equals **16 in 85 entries** or **48 in 474 entries**. Across all 559 Type-5 records, the payload length (after the five-byte header) ranges from 1,123 to 9,676 bytes. Header bytes are not labeled as dimensions, compression methods or offsets until the original CODE.2 handler is recovered.

Type-6 in the same file likewise uses a five-byte header; first header byte is 3 and the last two are zero across all 502 entries, while middle bytes vary. Strahd's resource Type-5 loader assumes a different header size and cannot be adopted as-is.

## Executable resource decoder and level-event VM (Phase 1 continuation)

**VERIFIED:** The original CODE.2 `0x34E4D` six-way typed dispatcher indexes a table at `0x34CD8` after subtracting one from the EH type byte. Its Type-5 handler is `0x3E6E0` and Type-6 handler `0x3E778`. Both consume a record-relative `+5 u8 compression_method` and `+6 u32 little-endian decoded_size` (EH header length 5), selecting methods 0, 1 and 3; supplied data use only method 3. Type-5 declared decoded lengths are **12,290 (474 records)** and **4,098 (85 records)**.

The unchanged Strahd LZSS oracle consumes every encoded byte and returns the declared output length for all **3,167 Type-6 RES3**, **502 Type-6 RES4** and **474 larger Type-5 RES4** entries. For all **85 smaller Type-5** records it consumes the full stream when asked for **4,096** bytes, not the declared **4,098**. Preserve original headers. The two-byte producer/runtime discrepancy is **UNKNOWN**, not an asset repair instruction.

The level-event interpreter at CODE.2 `0x200` dispatches opcode bytes `0..13` using the original little-endian jump table `0x1C8` and jump `0x272`. All **56** levels have 50 physical 26-byte event slots; **352** are active, containing **393** five-byte commands. Opcode 13 occurs **eight times**. This is a verified serialized VM shape and dispatcher, not complete opcode semantics.

`RES9` has **31** raw programs of `u16LE count + count u16LE program words + count u16LE zero words`. All 31 match the structural container. **0/31** parse under Strahd's unchanged 42-opcode grammar, but Menzo's title-specific reconstructed grammar now decodes **31/31** with exact validated direct-target boundaries. Higher-level operation semantics remain incomplete.

**Phase 1 RES9 update:** The authenticated Menzo CODE.2 dispatcher at `0x19B4D` selects 43 cases from a table at `0x19A6C` after subtracting two from a `u16LE` encoded opcode. Helper `0x19610` decodes opcode `0x13`/`0x1A` choice pairs (sentinel `0x21`); transitive helper `0x18AC0` consumes opcode `0x05`'s single character-template ID. All **31/31** preserved RES9 scripts now linearly decode under this title-specific structural grammar; **876/876** checked direct target operands land on decoded instruction starts, with **45** timer operands containing unresolved `0xFFFF`. **VERIFIED structure; runtime effects remain UNKNOWN.** See [VM evidence](../../Resources/RES/CROSS_TITLE_WORDCODE_VM_PHASE1.md).

**Reproduce:**

```sh
python Tools/phase1_resource_event_vm.py --root preservation/MASTER --json audit/menzo-vm.json
python Tools/phase1_decoder_dispatch.py --root preservation/MASTER --json audit/menzo-dispatch.json
python Tools/phase1_type56_verify.py --root preservation/MASTER --json audit/menzo-lzss.json
```

### Unresolved

The additional `0x580` bytes expand the header's 16-bit indexed table from 320 to 1024 entries, with all three record-family counts following the widened table. **706 distinct byte positions** within the former extension region contain nonzero values. Their original CODE.2 readsites are documented above; the meaning of the table values and the final 20-byte header tail is still unresolved.

The attack-count and 9-byte attack-record grammar is proved, but descriptor-relative `+3` control-flag aliases, `+4/+6/+7` raw fields and broader NPC status/sound meanings remain incomplete. CHARS, level-event effects, saves, rules and Type-5's declared-length discrepancy remain open. No Menzo gameplay was implemented or tested.

**Reproduce:**

```sh
python Tools/phase1_compare.py --root preservation/MASTER --json audit/formats.json --markdown audit/formats.md
python Tools/phase1_code2_consumers.py --root preservation/MASTER --json audit/code2.json
```
