# Ravenloft resource reconstruction — final workstream audit

Status: **CERTIFIED COMPLETE FOR THE SUPPLIED RAVENLOFT RUNTIME / DATA SET.**

This audit closes the dedicated `Resources/` reverse-engineering workstream for the supplied *Ravenloft: Strahd's Possession* corpus. “Complete” here means that every supplied resource/support file is accounted for, active runtime formats are reconstructed to the level required for a faithful reader/renderer/VM/audio integration, byte-preservation requirements are explicit, and remaining unknowns are bounded to historical producer details, alternate-build provenance, or third-party driver internals.

The immutable shipped bytes remain controlling evidence. This audit does not assert original DreamForge source names where the executable proves only physical/behavioral semantics.

## 1. Source-set accounting

The Resources corpus contains **33 source files**:

```text
13  CINE00.ANM .. CINE12.ANM
13  RES0 .. RES12
 2  GM1.BNK / GM2.BNK
 5  COLORS / INIT / SAMPLE.AD / SINTAB / T
--
33  total
```

No supplied resource/support binary in this workstream is unaccounted for.

### ANM identities

| File | Bytes | SHA-256 |
|---|---:|---|
| CINE00.ANM | 13,781,936 | `6627ebb7e91cc62c56871ad5140a5df038fde198b079e94b010467433ce9eccb` |
| CINE01.ANM | 16,009,364 | `47a1e618f687351ebdb3e9ed07961f0feb3a42969f4cf984e48884c548153362` |
| CINE02.ANM | 7,498,347 | `62a06c3ecf78c3f7907279592cc042f652cbcafeb8694027ce72f5d6995d9199` |
| CINE03.ANM | 7,498,347 | `7da29505c4aac4f3449eb8feed9f119f9f80101fb0c91ffdb46938956e3b6ce6` |
| CINE04.ANM | 7,498,347 | `78f352452abfeb7c9f4f3edb34f30c4c44f30752b88e6e783a04d1a3c5d66c2f` |
| CINE05.ANM | 7,498,347 | `6f991c789a1a99c2d182bae55426b6e4b00e5ceb210871874518afd0a5aefbe6` |
| CINE06.ANM | 7,339,821 | `7149f7a9020a1d718970ce2fa610cfbfab76b5f89d95caae17b78e119d9e3925` |
| CINE07.ANM | 34,569,939 | `70cefedf8d2cb68525e6ebdcb8bf9e4be07e4619d12610b537e2f3c9841685ef` |
| CINE08.ANM | 9,255,472 | `cfbc8b5835906ceedcf74e8c202430cd0b5b4e4d4c7c00054d42b934def817f3` |
| CINE09.ANM | 5,485,203 | `287c3752048c5545c78f51d09035635bcd5422849770482918179a0d89f9387c` |
| CINE10.ANM | 56,273,227 | `e7d6404057d1ea472f0ee1ec870b9098fff9adb86020af08d2e531eae6961bfd` |
| CINE11.ANM | 57,092,507 | `9ee3f32320e0ae975b496ec69d34691ff45cc9e48fbf54a5de4a3f2fb5ef067e` |
| CINE12.ANM | 1,446,666 | `a39bdb4965ba27f9fd1b1f4a8fb2ac291a48da831070cdf226faab229b132291` |

### RES identities

| File | Bytes | SHA-256 |
|---|---:|---|
| RES0 | 2,186,991 | `5e30584947c9627e31b5dd2573ad75c2d3cc9fe796ac669e0ca5d02d6db7e61d` |
| RES1 | 354,310 | `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19` |
| RES2 | 63,736 | `bd46022e716fb1014ce0d0be98fa2c8a1cad7970df6943b1b4d54d017581a8f8` |
| RES3 | 5,228,638 | `44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36` |
| RES4 | 1,975,542 | `d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46` |
| RES5 | 1,510,960 | `f943c933cfe593bb2912bc25e10b72f1ecafe10f684526f38489616cedbe91b6` |
| RES6 | 6,210,899 | `697aa0803b2219dcb094bfae769ffb0c60cd7d89f7a7ba06c12ed6a4553f736d` |
| RES7 | 211,133 | `78b46977d1d3232824392ec1e4852519a6d2e20277adb0e78a67c7605ee9c4e1` |
| RES8 | 58,772 | `bfadd70c836260143f7e28b34128a0a16e7ca1743379a00176d2adfa525b28b4` |
| RES9 | 8,061,481 | `c3021a8bb16bf1d8b9234ac47a670e4a41b8a1c078cba34be34f0f89a5135543` |
| RES10 | 46,060,354 | `dc0c6b25a521f54d82bc564168859c74aa4b464413338d4b58d519b166e8e6f9` |
| RES11 | 15,589,639 | `5c231d0e117def4f465f9b8d24ea6a315f610919e5fdd0910b9c2446d435d903` |
| RES12 | 12,150,551 | `d03808be3f96ae48eab9f991433e8f7c4af989a6cf76ae9cb5a14f6964e40855` |

### BNK/MISC identities

| File | Bytes | SHA-256 |
|---|---:|---|
| GM1.BNK | 23,907 | `843f012e7980e0a58722aa5c7a6e88b652f2f8d404834638cd994ca0869df5a8` |
| GM2.BNK | 24,273 | `b4af656059a6afba5db2da972c3506ca08d899ca04c65389e0bec7aaf6536339` |
| COLORS | 11,521 | `e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5` |
| INIT | 1,879 | `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276` |
| SAMPLE.AD | 4,662 | `79ee9f4ad3134be906ce493072ce0c3b7169c9013d3c25947ac385aaf3f9633b` |
| SINTAB | 333,757 | `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87` |
| T | 133,584 | `a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315` |

## 2. Active format closure

### ANM

**Class: CONFIRMED / RECONSTRUCTED.**

The cinematic grammar, CODE.2 chunk dispatcher, packet-size rules, full-frame RLE, sparse planar deltas, palette/reset behavior, subtitle lookup, music/SFX commands, and raw four-plane frames are reconstructed. All 13 supplied files parse to EOF and reserialize byte-identically. Executable-supported chunk type 4 is absent from the supplied corpus and remains conservatively documented.

Canonical evidence: `ANM/FORMAT.md`, `ANM/INVENTORY.md`, `ANM/analyze_anm.py`.

### RES outer container

**Class: CONFIRMED.**

All supplied archives use a signed 32-bit entry count followed by absolute little-endian offsets. Offsets are nondecreasing; duplicate offsets encode zero-length entries. All 13 supplied archives reserialize byte-identically.

Correct aggregate corpus:

```text
99,663,006 bytes
12,562 entries
12,464 EH entries
21 zero-length entries
77 non-empty non-EH entries
```

### EH resources

**Class: CONFIRMED / RECONSTRUCTED.**

The common envelope is:

```text
'EH'
uint8  resource_type
uint16 type_header_size
byte   type_header[type_header_size]
byte   payload[]
```

The executable dispatch is reconstructed for types 1..7.

- **type 1:** raster/image resources; supplied compression methods 0 and 3; method 1 executable-only signed-control RLE reconstructed separately; selector 2 is an error path.
- **type 2:** fonts with 128-byte glyph-width table followed by a type-1-compatible descriptor.
- **type 3:** length-prefixed XMIDI/XDIR music sequences.
- **type 4:** length-prefixed Creative Voice File sampled audio; two shipped malformed-length anomalies are preserved exactly.
- **type 5:** executable-supported, absent from Ravenloft data; exact loader read/tail contract is documented without invented semantics.
- **type 6:** 32×128 or 64×128 column-major first-person texture sources.
- **type 7:** compiled scene-shape column display lists consumed by scale-specific rasterizer microcode.

Canonical evidence: `RES/FORMAT.md`, `RES/EXECUTABLE_ONLY_FORMATS.md`, `RES/TYPE6_RENDERER.md`, `RES/TYPE7_RENDERER.md`.

## 3. Renderer/resource architecture closure

### RES3 scene-shape namespaces

**Class: CONFIRMED.**

`INIT +0x06..+0x0C` supplies four source-class bases that partition all 4,234 RES3 entries without gap or overlap:

```text
0..2168      class 0 — NPC/creature scene shapes
2169..3315  class 1 — placed object/item family
3316..4157  class 2 — loose/pickable OBJECTS family
4158..4233  class 3 — transient/moving effects
```

`NPCS.DAT`, `OBJECTS.DAT`, and INIT scene descriptors store bank-local IDs; CODE.2 adds the source-class base during scene-record construction.

### Type-7 compiled shapes and SINTAB

**Class: CONFIRMED / RECONSTRUCTED.**

Type-7 decoded bodies contain:

```text
10-byte geometry header
16-bit relative column-pointer table
zero-terminated lists of 6-byte draw descriptors
```

The two descriptor selector fields range across all 129 even values `0x0000..0x0100`.

`SINTAB` supplies 127 scale blocks. Every block has two 129-word tables followed by executable x86 rasterizer code. The first table maps type-7 selectors to code offsets; CODE.2 temporarily patches a selected stop location with `RET`, executes the scale-specific micro-rasterizer, and restores the original byte.

All 8,362 supplied type-7 resources validate structurally and all selector targets are valid against every relevant SINTAB block.

This preserves a key historical architecture: part of Ravenloft's renderer is executable microcode stored in external data rather than ordinary image pixels alone.

## 4. INIT is fully accounted

**Class: CONFIRMED.**

All 1,879 bytes are parsed to EOF:

```text
0x0000  18 uint16 initialization values
0x0024  class-1 descriptor count = 175
0x0026  175 × 7-byte descriptors
0x04EF  class-3 descriptor count = 12
0x04F1  12 × 7-byte descriptors
0x0545  environment-profile count = 2
0x0547  two variable RES8 environment/backdrop profiles
0x0757  EOF
```

The first three words are active UI/resource bases:

```text
15   RES0 UI-graphics base
168  RES1 object/inventory UI base
336  RES1 character-portrait base
```

The following four words are the RES3 scene-shape bases listed above.

The final profile tables link `LEVELS.DAT header +0x290` values 0/1 to azimuth-anchored RES8 backdrop/environment placements; `0xFF` selects the alternate/default level path.

Canonical evidence: `MISC/INIT/FORMAT.md`, `MISC/INIT/UI_RESOURCE_BASES.md`, `MISC/INIT/ENVIRONMENT_PROFILES.md`, `MISC/INIT/analyze_init.py`.

## 5. RES1 UI and portrait namespaces

**Class: CONFIRMED.**

`OBJECTS.DAT +0x2D` is a 0..167 UI-graphic index. Adding INIT base 168 maps exactly to `RES1:168..335`.

Character portraits use:

```text
local = face_index + sex_code * 23
id    = 336 + local
if status_word & 0x0100:
    id += 46
```

The two 46-slot banks are therefore:

```text
RES1 336..381  normal portraits
RES1 382..427  vengeful-spirit-possessed portraits
```

SAVE status bit `0x0100` is independently proved as the GHOST/vengeful-spirit possession state. GHOST interaction program 9 sets it on a selected party member; defeating NPC type 18 / GHOUL LORD sets the completion flag; the resolution branch clears possession and restores the normal portrait bank.

The formerly opaque RES1 raw records are now positionally explained:

```text
380 / 426  reserved female face 21: zero-length
381 / 427  reserved female face 22: raw 60 00 00 00
```

Their literal producer-side `0x60` meaning remains unknown. They are not valid EH graphics and are not consumed through a raw archive-1 path in certified CODE.2.

Canonical evidence: `RES/RES1_PORTRAITS.md`, `RES/RES1_SENTINELS.md`, `DatFiles/SAVE.DAT/STATUS_POSSESSION.md`.

## 6. Interaction/display wordcode closure

**Class: CONFIRMED / RECONSTRUCTED.**

The active raw bank is `RES9:517..554`, proved by:

```text
INIT interaction base 103
+ INIT wordcode displacement 414
= entry 517
```

The 38 active records contain a 42-opcode (`0x00..0x29`) 16-bit-word VM. All 11,396 program words decode into 4,237 instructions with zero parse errors and zero invalid direct control-flow targets.

Programs map directly to NPC types 34..70 plus type 82. Their text bases link to `T`; resource bases link to typed RES9 graphics; sound/music/cinematic operations link to the reconstructed audio/ANM subsystems.

Each serialized wordcode container is physically:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 companion_words[word_count]   // all zero in supplied files
```

Certified CODE.2 reads only the first word array. A whole-object scan finds exactly one call to raw archive-handle helper `2EC74`, at the active archive-9 loader. The runtime VM buffer at `0x13020` has 47 absolute references: one load destination and interpreter-side accesses; there is no parallel companion-array writer/reader.

Therefore the second equal-length bank is **serialized but not consumed by the supplied CODE.2 runtime**. Its historical producer/editor purpose remains `UNKNOWN`; round-trip tools must preserve it verbatim.

`RES12:370..406` is an ID-aligned alternate/incompatible interaction bank. Its inner dialect is not bytecode-compatible with the active VM, and certified CODE.2 has no raw archive-12 loader. Exact edition/build provenance remains unknown.

Canonical evidence: `RES/NON_EH.md`, `RES/WORDCODE_VM.md`, `RES/WORDCODE_PROGRAM_INDEX.md`, `RES/RES12_WORDCODE.md`, `RES/analyze_wordcode.py`.

## 7. Text, palette, and standalone support formats

### T

**Class: CONFIRMED.**

`T` contains 1,073 offset-indexed XOR-`0xDC` text records. Twenty-five records are legitimately zero length. Thirty unreferenced gaps totaling 1,804 bytes contain residue/slack and are preserved byte-for-byte rather than interpreted as dialogue. The last indexed record reaches EOF exactly.

### COLORS

**Class: CONFIRMED.**

`COLORS` is exactly one count byte (`15`) followed by 15 palettes × 256 RGB triplets. All components lie in the 0..63 VGA DAC domain. CODE.2 seeks directly to `1 + palette_index*768` and can load partial palette ranges.

### SAMPLE.AD

**Class: CONFIRMED middleware identification.**

The supplied file is a Miles Design AIL 2.x YM3812/OPL2 Global Timbre Library. Its structure matches Miles' original GLIB tooling: 233 `(patch, bank, absolute_offset)` directory entries and contiguous 14-byte OPL2 timbres.

## 8. Third-party BNK support files

**Class: CONFIRMED role; deferred inner grammar.**

`GM1.BNK` and `GM2.BNK` are Sierra Semiconductor ARIA General MIDI support banks associated with the Miles AIL/32 ARIA XMIDI path (`A32ARXM.DLL`). They are not AdLib Instrument Maker banks and must not be parsed with the OPL GTL grammar.

The game/runtime treats them as external driver support assets. Exact ARIA record semantics are therefore assigned to the Runtime/third-party-driver track unless an authoritative ARIA bank specification is recovered.

## 9. Cross-file dependency map

The major proved relationships are:

```text
LEVELS.DAT
 |-- environment profile ------> INIT profile table ------> RES8 graphics
 |-- music sequence ID --------> RES type-3 / music subsystem
 |-- event SFX ----------------> RES type-4 / SFX subsystem
 `-- world records ------------> NPCS / OBJECTS / INIT scene descriptors
                                      |
                                      v
                              RES3 bank-local IDs
                                      |
                                      v
                              RES3 type-7 shapes
                                      |
                                      v
                                SINTAB microcode
                                      |
                                      v
                                  VGA output

NPCS.DAT --------> RES3 NPC shapes
   |
   `-------------> RES9 wordcode program IDs
                        |-- T dialogue
                        |-- RES9 typed interaction graphics
                        |-- SFX/music
                        `-- ANM cinematics

OBJECTS.DAT ------> RES3 world/loose-item shapes
   `--------------> RES1 inventory/UI graphics

SAVE.DAT ---------> character face/sex/status
                        |
                        `--> RES1 normal/possessed portrait bank
```

This map is behavioral and executable-backed. It does not claim the original source tree used the same module names.

## 10. Validation/tooling audit

Permanent analyzers cover the reconstructed families, including:

- `ANM/analyze_anm.py`
- `RES/analyze_res.py`
- `RES/analyze_type7.py`
- `RES/analyze_type7_banks.py`
- `RES/analyze_wordcode.py`
- `MISC/INIT/analyze_init.py`
- `MISC/SINTAB/analyze_sintab.py`
- `MISC/T/analyze_t.py`

The analyzers preserve rather than normalize anomalies and reserved/unknown material. Terms in tooling are evidence-neutral where historical semantics are unknown; in particular the wordcode trailing array is called a **companion bank**, not a runtime “state bank.”

## 11. Superseded hypotheses

The following earlier working interpretations must not be reintroduced:

- ANM second header word is not a generic “version”; it is the default wait/timing value.
- ANM full-frame RLE native row width is 320, not a community-derived alternate width.
- type 6 is not merely an arbitrary raster buffer; its 128-byte column stride and 32/64-column layouts are executable-backed.
- type 7 is not an ordinary bitmap sprite; it is a compiled scene-shape/display-list format tied to SINTAB microcode.
- `NPCS.DAT +0x14` / OBJECTS scene IDs are bank-local until CODE.2 adds the INIT source-class base.
- RES1 `381/427` are not unexplained random archive records; their placement is terminal reserved face slots, while only the literal `0x60` producer meaning remains unknown.
- status bit `0x0100` is not a generic alternate-portrait flag; it is vengeful-spirit/GHOST possession.
- the second wordcode array is not established runtime state; certified CODE.2 never consumes it.
- `SINTAB` is not merely a sine table; it also owns the scale-specific rasterizer microcode package.
- `SAMPLE.AD` is not DreamForge-native mystery data; it is a Miles AIL OPL2 GTL.
- `GM1.BNK`/`GM2.BNK` are not AdLib source-bank files; they belong to the ARIA driver path.

## 12. Deliberately bounded unknowns / deferred work

These do not block faithful reconstruction of the supplied runtime:

1. historical producer/editor purpose of the zero wordcode companion arrays;
2. why the RES1 producer wrote literal `0x00000060` in the terminal reserved face slot;
3. exact edition/build provenance and intended executable for RES11/RES12 alternate content;
4. original source/editor names for physically understood fields;
5. semantic field names for executable-only resource type 5, absent from supplied Ravenloft data;
6. higher-level producer semantics of unused ANM chunk type 4;
7. inner ARIA GM1/GM2 grammar, pending `A32ARXM.DLL` or authoritative vendor specification.

## 13. Certification boundary

For an initial faithful decompilation/native port, the Resources workstream is **closed**. A faithful implementation has enough evidence to:

- identify and round-trip the shipped containers;
- decode every active supplied resource family;
- reproduce the active compression paths;
- resolve runtime resource namespaces and cross-file indices;
- reproduce cinematic, text, palette, first-person texture, scene-shape, interaction-VM, and middleware-selection behavior;
- preserve invalid/reserved/anomalous records without normalization;
- distinguish DreamForge-native data from Miles/ARIA middleware assets.

Future work in this directory should require new primary evidence or should be clearly labeled comparative/historical archaeology. Runtime-driver reconstruction and alternate-release provenance belong to their respective workstreams rather than reopening already closed Ravenloft resource semantics.


## 2026-10-06 executable-fidelity addendum

A fresh adversarial re-audit preserved the architectural certification but corrected two low-level runtime details. CODE.2 sign-extends the EH header-size word and the type-1/type-2 raster `count`/`height` words. CODE.2 LZSS routine `37700` initializes only history bytes `0x000..0xFED` to `0xFE`, leaving `0xFEE..0xFFF` untouched by that routine before starting at cursor `0xFEE`.

The committed validators now model the signed fields and the partial dictionary initialization explicitly; an attempted read of an as-yet-undefined tail byte is a certification failure rather than being silently normalized to `0xFE`. The stricter LZSS edge was subsequently re-run against the original canonical RES binaries: all 11,174 shipped LZSS streams completed with zero undefined-tail history reads and zero compressed-stream trailing-byte failures. See `FULL_REAUDIT_2026-10-06.md` and `verify_resources.py`.


## Final post-fix certification result

The original 33-file resource/support corpus was re-supplied after the validator corrections and matched the canonical manifest exactly: **33/33 size and SHA-256 identities passed**.

The corrected CODE.2-faithful LZSS initialization model was then exercised over every shipped compressed resource:

- type 1: 1,478 streams;
- type 2: 15 streams;
- type 6: 1,319 streams;
- type 7: 8,362 streams;
- total: **11,174 streams**.

No stream read any of the 18 dictionary bytes left uninitialized by CODE.2 before those bytes had been defined, and no stream left trailing compressed bytes.

Therefore the resource workstream is **audit-clean for the supplied canonical Ravenloft runtime/data set**. Remaining unknowns are historical, alternate-build, or third-party-driver questions and do not block faithful reconstruction of the supplied build.
