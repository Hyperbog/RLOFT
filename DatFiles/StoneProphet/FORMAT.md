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

### NPC definition and enlarged fixed level-header evidence

**Special-attack bit routing (original CODE.2):** The selected attack's `u16LE +3` flags are tested individually. Bit `0x04` is read at `0x1253E`, `0x80` at `0x12582`, `0x01` at `0x125CF`, `0x02` at `0x12658`, `0x20` at `0x126FF`, `0x10` at `0x1274D`, `0x40` at `0x127CD`, and `0x08` at `0x12835`. These branches call a limited set of different handlers (`0x32F70`, `0x3395C`, `0x11B94`, `0x343D8`), not all the same effect. In the 45 active original attack descriptors, bits 0x02/0x40/0x80 are unused although their code exists. Specific spells/status aliases remain UNKNOWN. [Machine witness regression](../../Tools/phase1_attack_effect_proof.py).

**Attack-selection and channel-effect evidence.** The original second-attack handler at CODE.2 `0x275F9..27649` masks descriptor-relative `+3` with `0x8D`, then checks live-actor flags at `+0x0E` and random thresholds of **20 / 70** before selecting the alternate attack. These are verified conditional branches, not independently named status effects. Among **45 declared active attacks**, relative `+4/+6` are always zero, `+5` has 0/3/4/6 counts **5/37/2/1**, `+7` has 0/0x80 counts **5/40**, and `+8` has 0/1/2/3 counts **5/16/23/1**. The five `+5=0` slots include **four nonempty attack records** (indices 4 twice, 24 once and 27 once) and the known empty second attack of index 27; path reachability is UNKNOWN and must not be 'fixed' by fabricating divisors. [Corpus audit](../../Tools/phase1_attack_control_channels.py) and [control-flow proof](../../Tools/phase1_attack_effect_proof.py).

The 64×4×4 table's four channels have **[3771, 3835, 1419, 1466]** nonzero values across the 43 levels. Both original call paths (`0x2E72C..2E7A3`, `0x2EA34..2EAA3`) apply record flag `+1 & 0x04`, match the selected table word against an expected 16-bit value, then enforce rectangular bounds via record `+0x0A..+0x0D`. The original **eleven-case** action dispatcher at `0x308BB..3092E` has **five distinct handler destinations**: serialized types 1/2/4 and 11 get dedicated branches, and 3/5..10 share one default. Types 2 and 4 call the two spatial filters; type 11 reads an extra action byte and selects another helper. Case table `0x3064C` is authenticated by [CODE.2 table regression](../../Tools/phase1_attack_tile_caseflow.py). These paths establish conditional spatial matching, but they do not by themselves identify channel 0/1/2/3 with specific texture or animation resources. The equivalent of Menzo's runtime table-value mutation is not yet proved in Stone.

**Phase 1 zero-cycle safety trace (original CODE.2, VERIFIED conditional machine path; actual encounter reachability UNKNOWN).** The five affected active descriptors are Death Dog `NPCS.DAT[4]` slots 0/1 (1d10 each), Skeleton Warrior `[24]` slot 0 (1d10), and Tomb Guardian `[27]` slots 0/1 (1d6 and entirely zero). All have `u8 attack period +5=0`, `u16LE attack distance +6=0` and zero `u16LE` control flags. At `0x2770C..27722`, original CODE.2 doubles those distances, **but replaces both thresholds with `0x10000` when global `u8 ds:0x16E86==1`**. The executable writes 1 to that global at `0xD948` and toggles bit 0 at `0x2E180`. With positive encounter eligibility, `0x342F8` returning 0, and coordinate differences no larger than those thresholds, `0x26CF0` can select live state 1 or 2 for two-attack definitions; the single-attack path chooses state 1. Both states dispatch through `0x27C89` to an unchecked `idiv` at `0x27CA8` using the zero period. This is a **statically satisfiable hazard, not an observed gameplay failure**. Do not assume an overlap prohibition, a safe divisor default, or a crash without an original session trace. [Executable and NPC sample audit](../../Tools/phase1_stone_attack_effect_paths.py).

**CODE.2 eligibility helper and base-media reachability audit (Phase 1).** Stone's `0x342F8` is a four-argument signed interval-overlap predicate, **not a level-grid collision function**. The live attack caller `0x276F5` continues if it returns zero. The affected NPCs have definition `u16 +0x16=0`, reducing the condition to `view−384 < 212−live_actor[+0x12] < view−212`. The original actor constructor initializes `+0x12=212`; viewport setter `0x43888` permits view values **212..384**, of which **213..383** satisfy the helper for that constructor value. That is executable-backed **local feasibility**, not a runtime proof that these monsters enter attack states.

For original media, all **1,535** 11-byte family-A records in all **43** levels have first `u16LE` values `0..109` and type/coordinate/fraction domains consistent with the Strahd actor placement grammar; **none** has candidate NPC ID **4 (Death Dog), 24 (Skeleton Warrior) or 27 (Tomb Guardian)** at `+0`. Original CODE.2 constructor `0x10` has six direct callers: five hardcoded NPC IDs from **{1,17,77}**, and the event-opcode-13 caller at `0x70D`. All **469** archived level event commands have **zero opcode-13 uses**. **VERIFIED** original counts/code references; **LIKELY** candidate first-field type-ID mapping, pending full Stone loader proof. Other script layers, indirect calls, save restoration and dynamic encounters remain unexamined, so **actual zero-divisor encounter reachability remains UNKNOWN**. Preserve all source bytes. [Helper and viewport proof](../../Tools/phase1_stone_attack_eligibility.py), [constructor and level audit](../../Tools/phase1_stone_level_npc_encounters.py).

**Eight-caller eligibility helper census and dynamic mode lifecycle (CODE.2 original instructions, VERIFIED).** In addition to attack-state caller `0x276F5`, seven other direct calls use `0x342F8`: `0x122D8, 0x130F0, 0x2FF14, 0x3E4EC, 0x3E87D, 0x3E9AA, 0x46C4F`. Each passes a 172-unit interval and view data `0x2757C`; each skips its immediate processing path on a nonzero return. **Seven** read a `u16LE` from pointer-relative `+0x16`; the `0x3E9AA` caller instead supplies a **`u8` at pointer-relative `+0x07`**. Thus the helper is reused and its arguments must not universally be typed as an NPC definition. The attack-range override mode `u8 CODE.2 data +0x16E86` is **not constant**: direct writers include zero assignments `0x6EB2/0xCDE1`, copied-byte writes `0xCE96/0xD240`, a set-to-one `0xD948`, saved-byte restoration `0xDA37`, and XOR-bit toggle `0x2E180`. The function at `0xD924` saves the previous byte at `0xD941/0xD94F`, temporarily sets 1 at `0xD948`, and includes a restoration at `0xDA2E/0xDA37`. This **does not establish** how long mode 1 is active relative to a real attack, nor whether any zero-period template is ever constructed. Reproduce with [original-code caller/mode regression](../../Tools/phase1_stone_attack_eligibility.py).

**Special attack helpers partially reconstructed (CODE.2).** Flags `0x04`, `0x80`, `0x01`, `0x08` select numeric effect IDs `1`, `5`, `24`, `0` passed to `0x32F70` after a random-percentage threshold above 70. The callee bounds an effect-record table to **150 entries × 44 bytes**, not four separately named spells. Bit `0x02` checks two signed character words using boolean helper `0x3395C`, then can set bit `0x10` at runtime byte `0x15E1D + character_index×0x41C`. Bit `0x20` can set bit `0x08` at that same runtime address; bit `0x10` can set bit `0x10` at `0x15E1C + character_index×0x41C`. Both rely on d20 threshold helper `0x11B94`. Bit `0x40` calls this threshold helper, then identity helper `0x343D8` and writes its unchanged input back to the same runtime character word at `0x15E6C + character_index×0x41C`. **These CODE.2 addresses are runtime locations, not CHARS-file-relative offsets.** Exact spell/status meanings remain UNKNOWN. [Effect-ID and write proofs](../../Tools/phase1_stone_special_bit_operations.py).

**Nine-byte attack descriptor — typed grammar:** relative `+0/+1/+2` are `u8` dice count, die sides and damage bonus; `+3 u16LE` is the control flag word; `+5 u8` is the cycle divisor; `+6 u16LE` is the attack-distance input; `+8 u8` is the trigger phase. CODE.2 `0x12301/0x12309` reads and doubles the entire distance word, and `0x1230B` reads the 16-bit flags. **Five** declared active slots have zero period and distance (four populated, one empty). Live states **1 and 2** both enter code `0x27C89`, which loads divisor at `0x27C9E` and runs `idiv` at `0x27CA8` **without a local zero check**. Runtime execution for those source records remains UNKNOWN, but the `0x16E86==1` override demonstrates a conditionally feasible transition despite their zero serialized ranges. Do not infer a safe guard or repair bytes. [Original archive and machine readsites](../../Tools/phase1_npc_attack_descriptors.py), [state dispatch](../../Tools/phase1_attack_tile_caseflow.py).

The original `CD_Extracted/NPCS.DAT` SHA-256 is `646e6936e496a66bfa2768cb2288d9b5a959cd0042ab8530f1ecc0b3f24c3a02` (110 definitions, `0x58` bytes each). The following offsets refer to the **serialized NPC definition**, not the constructed live actor:

| Original field | Width | Executable-backed operation | Confidence |
| --- | --- | --- | --- |
| `+0x30` | `u8` | Normal NPC constructor `0x13844` passes hit-dice count to `0x29F08`. Runtime selector `0x1A4BC` controls the HP rule: 0 = d8 rolls, 1 = count×6, 2 = count×7, 3 = count×8. Result stored in live `+0x2A` | VERIFIED computation, runtime selector purpose UNKNOWN |
| `+0x48` | `u32LE` | `0x137D2/0x137E4` and `0x23968/0x2396B` copy definition value unchanged to live `+0x0E` | VERIFIED state initialization |
| `+0x4C` | `u8` | `0x23995` tests bit `0x80`; on the taken path it sets live `+0x0E` bit `0x20` | VERIFIED bit behavior; effect meaning UNKNOWN |
| `+0x4D` | `u8` | `0x2399F` tests bit `0x08`; on the taken path it sets live `+0x0F` bit `0x20` | VERIFIED bit behavior; effect meaning UNKNOWN |
| `+0x50/52/54/56` | four `u16LE` | Independent sound/resource-ID candidate fields; `+0x52` at `0x12A33` uses base `0x48138` and dispatcher `0x3B71C`; `+0x54` is zero in all 110 definitions | VERIFIED values, some readsites; individual purposes unresolved |
| `+0x1D`; `+0x1E..+0x26`; `+0x27..+0x2F` | `u8 attack_count` plus two nine-byte attack descriptors | 110 NPCs: counts 0/1/2 = 80/15/15; each descriptor starts with unsigned dice count, sides, bonus. | CODE.2 `0x275F0/0x277A5` branches on two attacks; `0x12B6C..0x12B8D` indexes `(live_attack_index−1)*9` and calls damage helper `0x29ED0`. | VERIFIED attack grammar/rolled damage |

Example: NPC index **1** contains **13** at `+0x30` and `0x20000000` at `+0x48`. The original **88-byte definition** record is copied selectively into a different **50-byte (`0x32`) live NPC** record (constructor stride at CODE.2 `0x1378A`). Neither game has been executed during this analysis.

The original `CD_Extracted/LEVELS.DAT` SHA-256 is `212b56deb635af954a257744a15c26bc48d801b6e09b0ef7fc2b614a425513b2`. Every one of its **43** level records has this fixed header at record-relative `+0x1D`:

| Header offset | Physical field and evidence | Confidence |
| --- | --- | --- |
| `+0x00A..+0x809` | `u16LE[64][4][4]` grouped by code, orientation and channel | Original CODE.2 accessor `0x33D42..0x33D94`: group×32 bytes, orientation×8, channel×2. Renderer `0x4423A/0x44244` uses 32-byte groups. Complete producer of the selected group index not yet certified. | VERIFIED layout/consumer; channel meanings UNKNOWN |
| `+0x80A` | `u16LE` family A count = `(boundary_3−boundary_2)/11` for 43/43 levels | VERIFIED |
| `+0x80C` | `u16LE` family C count = `(boundary_5−boundary_4)/11` for 43/43 levels | VERIFIED |
| `+0x80E` | `u16LE` family B count = `(boundary_4−boundary_3)/11` for 43/43 levels | VERIFIED |
| `+0x810..+0x823` | 20-byte residual tail with original direct consumers and populated bytes | VERIFIED raw span; field meanings UNKNOWN |

The indexed table is a **64 × 4 × 4** code/orientation/channel lookup (16 words = 32 bytes per group); unlike Strahd, it is not five independent 64-word resource banks. Executable callsites verify four-channel selection and rendering use. Stone's original callers at CODE.2 `0x2E750..0x2E760` and `0x2EA5D..0x2EA64` explicitly pass **packed_cell_word & 0x3F** to the accessor, with orientation-like byte `+0x0E` and channel byte `+0x0F` from the caller record. The returned value is compared with a caller-supplied expected value. Individual channel and downstream gameplay meanings still require tracing. [Exact indexed arithmetic](../../Tools/phase1_npc_table_provenance.py).

The indexed table stores **10,491 nonzero entries** across 43 levels; values range **0..193** and never exceed 255. Example level **0** is **36×36** cells with A/C/B counts **58/64/172**. Its table contains 704 more 16-bit entries than Strahd's, exactly accounting for the `0x580`-byte fixed-header expansion. Table-value meaning and readsite effect are still under examination.

**Reproduce from read-only preservation media:**

```sh
python Tools/phase1_npc_level_fields.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-fields.json
python Tools/phase1_npc_level_consumers.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-consumers.json
python Tools/phase1_npc_level_semantics.py --root preservation/MASTER --repo-root . --json audit/phase1-npc-level-semantic-witnesses.json
```

### Type-5/6 and RES9 preservation

Stone `PROPHET/RES4` entry 0 is Type-5, with the common two-byte EH marker, type byte=5 at `+2`, signed little-endian header length=5 at `+3`. All 963 Type-5 entries have next-five-header-byte positions 0=3, 1=2, 3=0, 4=0; position 2 equals **16 in 63 records** or **48 in 900 records**. The Type-5 payload lengths range from 1,177 to 9,495 bytes. The meaning of each header value is UNKNOWN.

All 410 Type-6 entries in `PROPHET/RES4` have five-byte headers. Header byte 0 is always 3, bytes 3 and 4 are always zero, and bytes 1 and 2 vary. Direct reuse of Strahd's differently shaped Type-5 loader would be unjustified.

The installed `PROPHET/RES9` and `CD_Extracted/RES9` are both 626-record archives. **625 records are byte-identical; record 574 differs by five bytes**. Keep both copies; it is unproved which is chosen by the original file-search policy.

## Executable resource decoder and level-event VM (Phase 1 continuation)

**VERIFIED:** The CODE.2 six-type EH resource dispatcher at `0x40DF1` uses the little-endian jump table at `0x40C88`. Its Type-5 handler is `0x4C2C0` and Type-6 handler `0x4C358`. Both use record-relative `+5 u8 compression_method` followed by `+6 u32LE declared_decoded_size`; method 3 is used by all supplied Type-5/6 entries, and the executable has branches for methods 0 and 1.

The unchanged Strahd LZSS implementation decodes **2,542/2,542** Type-6 RES3 and **409/410** Type-6 RES4 records with exact input consumption. RES4 index **1309** yields 14 decoded bytes after consuming 11 of 15 encoded bytes; the four-byte tail is preserved and remains unexplained. Type-5 RES4: **900/963** records declaring **12,290** bytes decode fully; the **63** declaring **4,098** consume the full encoded stream when decoding a **4,096-byte prefix** instead of the declared **4,098**. This is a **verified stream-level observation**, not a reason to alter the serialized length.

The level-event dispatcher at CODE.2 `0x313` uses fourteen entries at table `0x264` (opcodes 0..13). All **43** levels have a valid 50 × 26-byte event table: **396** active slots and **469** active five-byte commands. Opcode 13 is implemented in the executable but absent in the supplied level archive.

`RES9` has **48/49** nonempty raw entries with a single-bank program record: `u16LE count + count u16LE words`. The remaining raw entry, **625**, is four bytes and begins with `u16LE=244`; its meaning is UNKNOWN. Unlike Strahd, none of the 48 programs parses under its known wordcode grammar. Record **574** is a 316-word single-bank program, and its installed vs CD copies differ at exactly five word indices: **246 (30→10), 261 (31→11), 268 (30→10), 273 (31→11), 313 (31→11)**, installed to CD. Semantic meaning and loader precedence remain UNKNOWN.

**Phase 1 RES9 update:** Stone CODE.2 uses 54 cases at table `0x1E10C` (dispatch `0x1E220`) on `u16LE opcode - 2`. Helper `0x1D118` handles opcode `0x13`/`0x1A` choice pairs (sentinel `0x26`); helper `0x1C6A4` consumes opcode `0x05`'s single character-template ID. Opcode `0x08` is **two serialized words**: a condition at handler `0x1E3CB` followed by an absolute word target loaded at `0x1E3F4` when the condition succeeds or skipped at `0x1EB7A` when false. This resolves the former parser failures in RES9:568, 575, 585 and 595: **48/48** scripts now decode. **1,473/1,473** inspected direct targets land on decoded starts; **24** `0xFFFF` timer fields retain unknown semantics. In RES9:615..624, `999` is not a branch target: opcode `0x2E` reads `(999,21,27)` and helper `0x1D594` treats `999` as an empty (`-1`) match sentinel; `27` is the valid branch target. **VERIFIED physical structure, original instructions and target geometry; full gameplay execution not tested.** See [VM evidence](../../Resources/RES/CROSS_TITLE_WORDCODE_VM_PHASE1.md).

**Reproduce:**

```sh
python Tools/phase1_resource_event_vm.py --root preservation/MASTER --json audit/stone-vm.json
python Tools/phase1_decoder_dispatch.py --root preservation/MASTER --json audit/stone-dispatch.json
python Tools/phase1_type56_verify.py --root preservation/MASTER --json audit/stone-lzss.json
```

### Unresolved

The formerly unexplained `0x580` bytes are **704 additional u16LE entries** in the enlarged fixed-header indexed table. **597 byte positions** in that extension are nonzero in at least one of the 43 archived levels. The table's gameplay effect and final 20-byte tail fields remain unidentified. For NPCs, the +0x1D attack count, both nine-byte attack records, +0x30 HP formula and +0x48 initial-state copy are proved. Individual attack-control helper effects, exact distance units, and possible scripted or restored instantiation of Stone's zero-period NPCs remain unresolved; examined direct level/spawn sources contain none of these three type IDs. Additional status/sound names also require tracing. CHARS/SAVE codecs, rules, event-VM effects, ACT/DFA cinematics, the Type-5 two-byte discrepancy and RES9 loader precedence remain open. No playable Stone Prophet native session was run.

**Reproduce:**

```sh
python Tools/phase1_compare.py --root preservation/MASTER --json audit/formats.json --markdown audit/formats.md
python Tools/phase1_code2_consumers.py --root preservation/MASTER --json audit/code2.json
```
