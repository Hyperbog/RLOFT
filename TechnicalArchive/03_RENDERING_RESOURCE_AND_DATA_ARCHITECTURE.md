# Ravenloft: Strahd's Possession — Rendering, Resource, and Data Architecture

Status: **PHASE I / BASELINE 03 — RECONSTRUCTED FROM CERTIFIED RESOURCE/DAT/CODE EVIDENCE**  
Scope: RES archive routing, resource loaders, INIT namespace bases, first-person rendering, SINTAB micro-rasterizers, UI graphics, environment profiles, cinematics, text, and audio resource relationships.

## 1. Purpose

This unit integrates the certified `Resources/` workstream with the CODE.2 resource/render consumers and the reconstructed DAT files. The goal is to preserve the architecture that actually shipped: indexed RES archives, bank-local resource identifiers, compressed column textures, compiled scene-shape display lists, external executable rasterizer microcode, palette-index rendering, and data-driven namespace bases.

It intentionally avoids reducing the original implementation to modern generic terms such as "sprite sheet", "texture manager", or "asset bundle" where those labels would hide behavior encoded in the Ravenloft binaries.

Canonical closure record: `Resources/FINAL_RESOURCE_AUDIT.md`.

## 2. Source-set boundary

Claim: the dedicated resource/support corpus contains 33 accounted source files and no supplied file in that workstream is unclassified at the preservation boundary.

Class: **CONFIRMED**

```text
13  CINE00.ANM .. CINE12.ANM
13  RES0 .. RES12
 2  GM1.BNK / GM2.BNK
 5  COLORS / INIT / SAMPLE.AD / SINTAB / T
--
33  total
```

The resource audit distinguishes three kinds of closure:

- active Ravenloft runtime formats reconstructed from CODE/data;
- executable-supported but uninstantiated formats preserved from loader behavior;
- third-party/vendor assets identified sufficiently to separate them from DreamForge-native formats.

## 3. Runtime archive routing

### 3.1 INFO.DAT controls physical RES location

Claim: `INFO.DAT` is an installation/resource-location configuration consumed by CODE.2.

Class: **CONFIRMED**

Physical layout:

```text
+0x00  char  resource_location_digits[13]
+0x0D  int16 resource_path_length
+0x0F  byte  resource_path[resource_path_length]
```

Supplied bytes decode as:

```text
resource_location_digits = "0000000001111"
resource_path             = "D:"
```

CODE.2 resource initialization opens only RES selectors `0..10`. For each archive:

```text
selector == 0  -> "RES" + decimal(index)
selector != 0  -> resource_path + "\\" + "RES" + decimal(index)
```

Therefore the supplied installation routes:

```text
RES0..RES8   local/current path
RES9..RES10  D:\RES9 / D:\RES10
```

Digits 11 and 12 are serialized and preserved but have no proved CODE.2 indexed-resource consumer. This is important because supplied `RES11` and `RES12` exist physically yet are not silently promoted into the active archive set.

Canonical evidence: `DatFiles/INFO.DAT/FORMAT.md`.

### 3.2 Active archive handle model

Claim: CODE.2 maintains an indexed resource subsystem for RES0..RES10 and uses a separate raw-handle escape path only for interaction wordcode.

Class: **CONFIRMED**

Core functions:

```text
2E7F0   indexed archive opening/path construction
2EB60   typed EH resource read/dispatch
2EC74   raw archive-handle accessor
2F29C   generic indexed resource accessor/cache path
2F2EC   RES-family initialization
```

A whole-object call scan finds one direct raw-handle consumer of `2EC74`: the archive-9 wordcode loader at `13F70`. Ordinary resources flow through the typed/indexed subsystem.

## 4. RES outer and inner format

### 4.1 Outer archive

Class: **CONFIRMED**

```text
int32 entry_count
int32 absolute_offsets[entry_count]
byte  entry_payloads[]
```

Offsets are nondecreasing. Adjacent equal offsets intentionally encode zero-length entries. All 13 supplied archives round-trip byte-identically under the reconstructed parser.

Certified corpus totals:

```text
99,663,006 bytes
12,562 entries
12,464 EH entries
21 zero-length entries
77 non-empty non-EH entries
```

### 4.2 EH typed envelope

Class: **CONFIRMED / RECONSTRUCTED**

```text
byte   magic[2] = "EH"
uint8  resource_type
word   type_header_size    // 16-bit field; sign-extended by CODE.2
byte   type_header[type_header_size]
byte   payload[]
```

CODE.2 dispatches resource types 1..7.

Safe architectural classification:

| Type | Runtime role |
|---|---|
| 1 | raster/image resource |
| 2 | font + 128 glyph-width table |
| 3 | XMIDI/XDIR music sequence |
| 4 | Creative Voice File sampled audio |
| 5 | loader-supported; no supplied instance; semantics withheld |
| 6 | column-major first-person surface texture |
| 7 | compiled scene-shape column display list |

The format definition belongs to `Resources/RES/FORMAT.md`; executable-only type-1 method 1 and type 5 behavior are retained in `Resources/RES/EXECUTABLE_ONLY_FORMATS.md` rather than invented from nonexistent samples.

## 5. INIT as the resource namespace root

Claim: `INIT` is not a generic opaque constants file. It is a compact runtime configuration/namespace file that supplies resource-bank bases, scene descriptors, and environment-profile tables.

Class: **CONFIRMED**

All 1,879 bytes are structurally accounted:

```text
0x0000  18 x uint16 fixed initialization values
0x0024  uint16 class-1 descriptor count = 175
0x0026  175 x 7-byte scene descriptors
0x04EF  uint16 class-3 descriptor count = 12
0x04F1  12 x 7-byte scene descriptors
0x0545  uint16 environment-profile count = 2
0x0547  two variable RES8 environment-profile tables
0x0757  EOF
```

The fixed words create multiple independent namespaces instead of one globally numbered asset space.

## 6. UI/resource namespaces from INIT

### 6.1 RES0 UI graphics

```text
INIT +0x00 = 15 -> DGROUP 0x258DC
```

Class: **CONFIRMED**

RES0 begins with 15 type-2 fonts at entries `0..14`. CODE.2 adds UI-relative graphic IDs to base 15 and requests archive 0, so:

```text
RES0 0..14    fonts
RES0 15..140  type-1 UI/raster resources
```

### 6.2 RES1 object/inventory graphics

```text
INIT +0x02 = 168 -> DGROUP 0x258D8
```

Class: **CONFIRMED**

`OBJECTS.DAT +0x2D` is a local UI graphic index `0..167`. Inventory/equipment presentation resolves:

```text
absolute_RES1_id = 168 + object.ui_graphic_index
```

Thus the complete supplied object UI namespace is `RES1:168..335`.

### 6.3 RES1 character portraits

```text
INIT +0x04 = 336 -> DGROUP 0x258E4
```

Class: **CONFIRMED / RECONSTRUCTED**

CODE.2 computes portrait resources as:

```text
local = int8(face_index) + uint8(sex) * 23
id    = 336 + local

if status_word & 0x0100:
    id += 46
```

The two 46-slot banks are:

```text
RES1 336..381  normal character portraits
RES1 382..427  vengeful-spirit/GHOST-possession portraits
```

The status-bit meaning is independently proved in the SAVE/story analysis. The two historically opaque raw entries `381` and `427` occupy the same final local slot in the two banks; they are not valid EH graphics and are not consumed by a raw archive-1 path.

Canonical evidence: `Resources/MISC/INIT/UI_RESOURCE_BASES.md`, `Resources/RES/RES1_PORTRAITS.md`, `DatFiles/SAVE.DAT/STATUS_POSSESSION.md`.

## 7. RES3 scene-shape namespace translation

Claim: scene databases store **bank-local** type-7 shape IDs and CODE.2 translates them to absolute RES3 IDs during scene-record construction.

Class: **CONFIRMED**

Four INIT words populate the source-class base table:

```text
INIT +0x06 =    0
INIT +0x08 = 2169
INIT +0x0A = 3316
INIT +0x0C = 4158
```

They partition all 4,234 RES3 entries exactly:

```text
RES3 0..2168      class 0 — NPC/creature scene shapes
RES3 2169..3315   class 1 — placed object/item family
RES3 3316..4157   class 2 — loose/pickable OBJECTS family
RES3 4158..4233   class 3 — transient/moving effects
```

Common translation helper `2B2F4` adds the selected class base to the bank-local shape identifier before geometry and draw consumers request archive 3.

Consequences:
- identical local ID values in different databases are not collisions;
- import/export tooling must preserve local IDs rather than rewriting stored data to absolute RES3 indices;
- renderer asset identity depends on both source class and local shape state.

`NPCS.DAT +0x14` and `OBJECTS.DAT +0x28`/associated stride fields are linked to these namespaces by the dedicated RES3 index ledgers.

## 8. Type-7 compiled scene-shape format

Claim: RES type 7 is not a bitmap sprite. It is a geometry/display-list format coupled to scale-specific generated x86 rasterizers.

Class: **CONFIRMED / RECONSTRUCTED**

Decoded body:

```text
+0x00 uint16 source_width
+0x02 int16  x_min
+0x04 int16  x_max
+0x06 int16  y_min
+0x08 int16  y_max
+0x0A uint16 column_list_offset[x_max-x_min+1]
...
```

Each column pointer addresses a zero-terminated descriptor list:

```text
int16 raster_stop_selector
int16 signed_adjustment
int16 raster_entry_selector
...
int16 0
```

Each active descriptor is six bytes. Pointer addresses may be unaligned and the engine relies on unaligned word reads.

CODE.2 `21BF0` consumes the geometry to form projected scene bounds. `36CEC` walks the per-column lists and `369D8` resolves selector fields against the currently selected scale-code block.

Complete-corpus validation:

```text
8,362 supplied type-7 resources
all decode to declared size
all header/table bounds valid
all reachable lists terminate
all renderer selectors valid/even
```

Source widths are 64, 96, or 128; this is geometry used by the first-person renderer, not inferred image metadata.

## 9. SINTAB: external projection data and executable rasterizer microcode

Claim: `SINTAB` is an external renderer package containing trigonometric/projection tables plus 127 scale-specific blocks of generated x86 rasterizer code.

Class: **CONFIRMED / RECONSTRUCTED**

CODE.2 loader: `3838..3A63`.

Top-level file includes:
- 450-entry signed-magnitude Q16 sine table;
- 900-entry Q16 tangent family;
- two 128-word projection tables;
- renderer constants;
- 256-entry signed view-angle table;
- 509-word runtime lookup;
- 127 variable scale blocks;
- preserved 512-byte zero trailer ignored by the active loader.

Every scale block begins:

```text
uint16 code_patch_offset[129]
uint16 scale_step[129]
byte   rasterizer_x86[]
```

Runtime initialization stores `block + 0x204` — the generated-code address — in the scale-code pointer table at DGROUP `0x18FE0`.

Type-7 descriptors use all 129 even selector values `0x0000..0x0100`, exactly matching the 129-entry offset domain. Renderer helpers recover the block prefix, resolve code addresses, temporarily patch selected code locations with `RET` (`0xC3`) or other control bytes, execute the generated rasterizer, and restore the bytes.

Historical architectural consequence:

```text
RES3 compiled shape
      |
      v
column display-list descriptor
      |
      v
selected SINTAB scale block
      |
      v
generated x86 rasterizer span
      |
      v
VGA framebuffer
```

Part of the renderer is therefore executable code stored in a data file.

A faithful port may translate those semantics into portable code but must preserve the original tables/code as canonical evidence and validate modern output against the encoded integer behavior.

## 10. Type-6 first-person texture pipeline

Claim: RES type 6 stores column-major surface textures as contiguous 128-texel vertical source columns.

Class: **CONFIRMED**

Type-6 resources share the type-7 LZSS wrapper. Renderer setup generates a selector in `0x40` increments; the draw path doubles it, therefore source pointers advance by:

```text
2 * 0x40 = 0x80 = 128 bytes
```

CODE.2 `338C8` vertically rescales one selected source column. Its fixed-point domain is `128 << 16`, proving 128 source texels per column.

Supplied decoded sizes:

```text
4096 bytes -> 32 columns x 128 texels
8192 bytes -> 64 columns x 128 texels
```

The sampler writes palette-index bytes into planar VGA scanlines and applies a per-draw subtraction/shading bias.

Safe architecture label: **column-major first-person surface texture source**. Narrower art terms remain index/level dependent.

## 11. RES4 texture-bank boundary

Claim: INIT value `600` is the start of the 32-column texture family inside RES4.

Class: **CONFIRMED**

Supplied RES4 contains 667 type-6 resources:

```text
RES4 0..599    decoded 8192 bytes -> 64 x 128
RES4 600..666  decoded 4096 bytes -> 32 x 128
```

Renderer consumers add the INIT base to selectors and request archive 4. This is an example of the runtime using INIT to define sub-banks within one RES archive rather than encoding all semantics into each resource header.

## 12. Environment profiles and RES8 backdrop graphics

Claim: LEVELS selects one of two INIT environment profiles; each profile drives phase-dependent RES8 backdrop resources plus azimuth-anchored graphics placements.

Class: **CONFIRMED / RECONSTRUCTED**

`LEVELS.DAT` fixed header byte `+0x290` has supplied values `0`, `1`, or `0xFF`:

```text
0 / 1  -> index one of two INIT environment profiles
0xFF   -> alternate/default level path
```

The final INIT section stores:

```text
uint16 profile_count = 2
repeat profile:
    uint16 res8_base
    uint16 stored_record_count
    placement[stored_record_count]

placement:
    int16 resource_selector
    int16 azimuth_degrees
    int16 draw_width_pixels
    int16 screen_y
```

Supplied profiles:

```text
profile 0  res8_base=8   records=38 (37 active + zero terminator)
profile 1  res8_base=45  records=27 (26 active + zero terminator)
```

Renderer `314B0` subtracts the camera/view angle from `azimuth_degrees`, wraps by 360 degrees, projects horizontal screen position, resolves archive-8 graphics, clips by width, and draws at `screen_y`.

This is safely described as **environment-profile backdrop/panorama placement architecture**. Historical editor labels such as sky/horizon/landmark remain unknown.

## 13. Palette architecture — COLORS

Claim: `COLORS` is a 15-palette VGA DAC-domain bank.

Class: **CONFIRMED**

```text
uint8 palette_count = 15
15 * 256 RGB triplets
```

Each palette occupies 768 bytes and every stored component lies in `0..63`, the VGA DAC domain.

CODE.2 palette loader seeks directly to:

```text
1 + palette_index * 768
```

and can read partial color ranges before applying runtime brightness normalization. The count byte is serialized metadata; the active loader's seek calculation does not depend on it as a bounds guard.

## 14. Text architecture — T

Claim: `T` is an indexed XOR-obfuscated text archive shared by dialogue, UI/event text, and cinematic subtitles.

Class: **CONFIRMED**

```text
uint32 record_count = 1073
uint32 record_offsets[1073]

record:
    uint16 encoded_length
    byte   encoded_text[encoded_length]

plain_byte = encoded_byte XOR 0xDC
```

CODE.2 `1C368` seeks through the index, reads the length-prefixed payload, null-terminates it in memory, and XOR-decodes each byte.

Important preservation detail: the file contains unreferenced slack regions. They are not additional dialogue records and must be retained byte-for-byte by round-trip tooling.

The interaction wordcode VM sets per-program text bases into this shared indexed archive. ANM subtitle packets also resolve text through the same reader family.

## 15. Cinematic architecture — ANM

Claim: CINE00..CINE12 use a frame/chunk packet stream whose handlers are integrated with palette, planar VGA, text, music, and sampled-audio subsystems.

Class: **CONFIRMED / RECONSTRUCTED**

Top-level grammar:

```text
uint16 frame_count
uint16 default_wait
repeat frame:
    uint16 chunk_count
    repeat chunk:
        uint16 chunk_id
        int32  chunk_size
        byte   payload[chunk_size]
```

Recovered chunk families include:
- palette update;
- blackout/reset;
- 320x400 full-frame RLE;
- sparse planar delta;
- music command;
- queued/direct SFX commands;
- indexed subtitle text;
- raw four-plane frame block.

All 13 supplied ANMs parse to EOF and reserialize byte-identically.

ANM therefore sits at a subsystem convergence point rather than being merely a standalone video codec.

## 16. Music, sampled audio, and voice banks

### RES5 music sequences

Class: **CONFIRMED**

RES type 3 is length-prefixed XMIDI/XDIR. Supplied RES5 contains 98 sequences. INIT provides an alternate offset of 49 used only for specific runtime music-driver/mode values:

```text
RES5 0..48   primary 49-sequence bank
RES5 49..97  alternate 49-sequence bank
```

The exact hardware labels for those runtime modes remain intentionally unpromoted without driver-enumeration proof.

### RES6 ordinary sampled SFX

Class: **CONFIRMED**

RES type 4 stores Creative Voice File payloads. INIT supplies several behaviorally distinct sub-bases used by ANM/UI, NPC creature sounds, death/common-action sounds, and gameplay effects.

One dormant direct-SFX base equals `266`, exactly the supplied RES6 entry count. Its only consumer is a wordcode opcode absent from the active bank; the out-of-range behavior is preserved rather than normalized.

### RES10 interaction speech/voice

Class: **CONFIRMED / RECONSTRUCTED**

Dialogue/audio paths temporarily switch the sampled-audio archive to RES10 and resolve voice IDs relative to INIT base 48 plus per-program sound sub-bases. They restore the ordinary RES6 archive afterward.

### SAMPLE.AD and GM1/GM2

Class: **CONFIRMED as vendor dependencies; inner ARIA bank semantics bounded**

`SAMPLE.AD` is a Miles AIL 2.x YM3812/OPL2 Global Timbre Library with 233 timbres. Its physical GTL grammar matches Miles' original tooling.

`GM1.BNK` and `GM2.BNK` are classified as Miles/ARIA General MIDI support-bank assets. Their detailed inner records belong to third-party driver preservation rather than DreamForge-native RES semantics and remain bounded accordingly.

## 17. Interaction/display wordcode as a resource subsystem

Claim: the active interaction VM is serialized as raw non-EH records in RES9 and deliberately bypasses the typed resource loader.

Class: **CONFIRMED / RECONSTRUCTED**

INIT proves:

```text
interaction typed-resource base = 103
wordcode displacement           = 414
first program entry             = 103 + 414 = RES9:517
```

Active bank:

```text
RES9:517..554 = 38 programs
11,396 program words
4,237 decoded instructions
42-opcode VM (0x00..0x29)
```

Container:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 companion_words[word_count]  // all zero in supplied data
```

Certified CODE.2 reads only `program_words`. No companion-array consumer exists in the certified object. The second bank is therefore a preserved serialized inactive/producer-side structure for this build, not runtime VM state that should be invented by a port.

Programs map to NPC identities, use `T` dialogue bases, draw typed RES9 graphics, and dispatch sound/music/cinematic operations. The VM is therefore part of the resource architecture as well as the gameplay/dialogue architecture.

## 18. Alternate/unselected RES11 and RES12 data

Claim: RES11/RES12 are supplied related resource sets but are not opened by the certified CODE.2 indexed RES0..RES10 subsystem.

Class: **CONFIRMED for active-path exclusion; RECONSTRUCTED for relationship**

Evidence:
- INFO loader normalizes 13 selectors but indexed RES initialization passes count 11.
- No proved selector-11/12 CODE.2 resource-loader consumer exists.
- RES12 contains an ID-aligned alternate wordcode bank whose instruction dialect is incompatible with the active RES9 VM at specific records.
- RES11/RES12 share substantial resource content with active archives.

Safe treatment:
- preserve and document them as supplied alternate/unselected resource material;
- do not label them floppy/CD/localization/prototype without independent provenance;
- do not silently merge them into the active RES namespace.

## 19. Renderer pipeline summary

Class: **RECONSTRUCTED from confirmed components**

```text
LEVELS / NPCS / OBJECTS / INIT
            |
            | bank-local IDs, scene classes, material/profile selectors
            v
   scene-record construction
            |
            +------------------------+
            |                        |
            v                        v
RES4 type-6 textures          RES3 type-7 shapes
column-major texels           geometry + column lists
            |                        |
            |                        v
            |                 SINTAB scale block
            |                 generated x86 code
            |                        |
            +------------+-----------+
                         |
                         v
                planar VGA rendering
                         |
                COLORS palette state
```

Parallel presentation paths use RES0/RES1 UI graphics, RES8 environment graphics, `T` text, ANM cinematic packets, and RES5/6/10 audio families.

The architecture is highly table-driven, but the tables include executable code and bank-translation rules; it is not a simple collection of independent files.

## 20. Preservation and modernization boundary

A native reconstruction may replace low-level implementation mechanisms with safe portable equivalents, but it should preserve and test the following historical semantics:

1. **Archive identity and bank-local indexes.** Do not rewrite DAT/INIT local IDs into absolute resource numbers in legacy files.
2. **Zero-length and raw records.** Duplicate offsets, reserved slots, raw markers, malformed-but-shipped VOC lengths, and unconsumed trailers/slack are part of the source corpus.
3. **Exact integer rendering domains.** Type-6 128-texel column resampling, type-7 selector domains, SINTAB integer tables, and palette-index arithmetic must remain validation references.
4. **Generated-rasterizer behavior.** Portable code may replace self-modifying x86 at runtime, but must reproduce the encoded draw semantics.
5. **Archive routing.** INFO's local/alternate path distinction and active RES0..RES10 boundary are part of the shipped installation architecture.
6. **Vendor/game boundary.** Miles GTL/ARIA assets and drivers should remain distinguishable from DreamForge-native resource formats.
7. **Evidence-bounded unknowns.** Alternate RES11/12 provenance, type-5 source semantics, inactive wordcode companion arrays, and historical producer marker meanings remain unknown where the supplied runtime does not decide them.

## 21. Canonical evidence

Primary repository evidence for this unit:

- `Resources/FINAL_RESOURCE_AUDIT.md`
- `Resources/RES/FORMAT.md`
- `Resources/RES/TYPE6_RENDERER.md`
- `Resources/RES/TYPE7_RENDERER.md`
- `Resources/RES/TYPE7_SOURCE_BANKS.md`
- `Resources/RES/WORDCODE_VM.md`
- `Resources/MISC/INIT/FORMAT.md`
- `Resources/MISC/INIT/UI_RESOURCE_BASES.md`
- `Resources/MISC/INIT/ENVIRONMENT_PROFILES.md`
- `Resources/MISC/SINTAB/FORMAT.md`
- `Resources/MISC/COLORS/FORMAT.md`
- `Resources/MISC/T/FORMAT.md`
- `Resources/ANM/FORMAT.md`
- `DatFiles/INFO.DAT/FORMAT.md`
- `DatFiles/LEVELS.DAT/HEADER_TAIL.md`
- `DatFiles/OBJECTS.DAT/FORMAT.md`
- `DatFiles/NPCS.DAT/FORMAT.md`
- `DatFiles/SAVE.DAT/STATUS_POSSESSION.md`
- certified CODE.2 object/reconstruction and final audit records.

## 22. Next evidence unit

`TechnicalArchive/04_WORLD_STATE_LEVELS_AND_SIMULATION_ARCHITECTURE.md`

The next unit should integrate the now-deep `LEVELS.DAT`, `LEV022.DAT`, `NPCS.DAT`, `OBJECTS.DAT`, and SAVE cross-references into the world-state and simulation model: fixed level headers, cell words/flags, world record families, object/NPC runtime records, level transition/save behavior, automap state, events, and the boundary between persistent serialized state and generated renderer state.
