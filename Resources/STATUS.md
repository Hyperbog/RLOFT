# Resources reconstruction status

Status: **Ravenloft resource reconstruction is functionally complete for the supplied executable/data set.** All shipped ANM, RES, and MISC files have structural grammars or vendor/runtime classifications sufficient for faithful preservation and native-port planning. Remaining open items are explicitly bounded historical, alternate-build, or third-party-driver questions.

This ledger is the continuity/checkpoint document for the `Resources/` track.

## Source set

The preserved resource set contains 33 files:

```text
13 CINE*.ANM
13 RES0..RES12
 2 GM*.BNK
 5 MISC/support files
```

Original binaries remain immutable evidence and are identified by exact size/SHA-256 in their child directories.

## Completion definition

A resource family is considered complete for this project when, as applicable:

1. its physical container grammar is reconstructed from original bytes;
2. executable consumers/producers are reconciled where present;
3. supplied records are exhaustively validated;
4. parser -> serializer round-trip is byte-identical where a serializer model is appropriate;
5. unknown bytes/semantics are preserved rather than normalized;
6. middleware/vendor assets are distinguished from DreamForge-native formats;
7. unsupported/unused executable branches are documented if they affect faithful engine reconstruction.

Historical source/editor names are **not** required for completion when the executable only proves physical behavior.

---

# ANM cinematics — COMPLETE

Canonical files:

- `ANM/FORMAT.md`
- `ANM/INVENTORY.md`
- `ANM/analyze_anm.py`

Recovered outer grammar:

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

CODE.2 handlers for chunk IDs 0..9 are mapped, including palette updates, reset, full-frame row RLE, sparse planar delta, music/SFX/text commands, raw VGA planes, and direct SFX.

All 13 supplied cinematics parse to EOF and reserialize byte-identically. All 73 type-2 RLE chunks, 809 type-3 deltas, and 2,796 type-8 plane chunks validate against executable-defined dimensions/grammar.

Chunk type 4 is executable-supported but absent from the supplied cinematic set; its control behavior remains conservatively documented rather than synthetically exercised.

---

# RES archives — COMPLETE FOR SUPPLIED RUNTIME

Canonical overview:

- `RES/FORMAT.md`
- `RES/INVENTORY.md`
- `RES/analyze_res.py`

Outer corpus:

```text
99,663,006 bytes
12,562 entries
12,464 typed EH resources
21 zero-length entries
77 non-empty non-EH records
```

All 13 archives parse/reassemble byte-identically, including duplicate offsets/empty entries.

## EH types

```text
type 1  raster/image resources
type 2  fonts + 128 glyph widths
type 3  XMIDI/XDIR music sequences
type 4  Creative Voice File sampled audio
type 5  executable-supported, no Ravenloft instance
type 6  32x128 / 64x128 column-major first-person textures
type 7  compiled scene-shape column display lists
```

All supplied compressed families exhaust their compressed streams exactly.

The two shipped VOC length anomalies are preserved/documented rather than repaired.

## Type-7 renderer closure

Canonical files:

- `RES/TYPE7_RENDERER.md`
- `RES/TYPE7_SOURCE_BANKS.md`
- `RES/TYPE7_NPC_INDEX.md`
- `RES/TYPE7_OBJECT_INDEX.md`
- `RES/analyze_type7.py`
- `RES/analyze_type7_banks.py`

All 8,362 supplied type-7 bodies validate their geometry header, source-column pointer table, 6-byte compiled draw lists, and terminators.

RES3's 4,234 entries are partitioned exactly through `INIT` into four source namespaces:

```text
0..2168      NPC/creature shapes
2169..3315  placed object/item family
3316..4157  loose/pickable OBJECTS family
4158..4233  transient/moving-effect family
```

Type-7 raster selectors map exactly onto SINTAB's 129-entry micro-rasterizer selector domain.

## Wordcode closure

Canonical files:

- `RES/NON_EH.md`
- `RES/WORDCODE_VM.md`
- `RES/WORDCODE_PROGRAM_INDEX.md`
- `RES/RES12_WORDCODE.md`
- `RES/analyze_wordcode.py`

The active `RES9:517..554` bank is a 42-opcode (`0x00..0x29`) 16-bit interaction/display VM:

```text
38 programs
11,396 program words
4,237 decoded instructions
0 parse errors
0 invalid direct control-flow targets
```

NPC program identities and text/audio/cinematic sub-bases are mapped.

The serialized equal-length zero bank is proved **unread by the supplied CODE.2**. Its producer/editor purpose remains `UNKNOWN` but does not block runtime fidelity.

RES12 contains an ID-aligned alternate/incompatible interaction bank. No raw archive-12 loader exists in certified CODE.2; exact edition/build provenance remains intentionally unknown.

## RES1 raw records

`RES1:381` and `RES1:427` are raw four-byte `0x00000060` records. Normal EH loading cannot consume them and the certified object contains no archive-1 raw-handle request. They are preserved as inactive serialized markers with historical meaning `UNKNOWN`.

See `RES/RES1_SENTINELS.md`.

## Executable-only RES branches

`RES/EXECUTABLE_ONLY_FORMATS.md` preserves two important code paths absent from the shipped data:

- type-1 compression method 1: signed-control RLE, including executable-specific `-128 -> 129-byte repeat` behavior;
- resource type 5: exact 28-byte loader read footprint + variable-tail contract, while semantic field names remain unknown because no Ravenloft sample exists.

Type-1 compression selector 2 is proved to be an error path, not a missing decoder.

---

# MISC/support files — COMPLETE

## COLORS

Canonical directory: `MISC/COLORS/`

Exact layout:

```text
uint8 palette_count = 15
15 * (256 RGB triplets)
```

RGB values are 6-bit VGA DAC-domain components. CODE.2 palette-range reading and runtime brightness normalization are mapped.

## INIT

Canonical files:

- `MISC/INIT/FORMAT.md`
- `MISC/INIT/ENVIRONMENT_PROFILES.md`
- `MISC/INIT/analyze_init.py`

All 1,879 bytes are structurally accounted for and byte-identically round-trippable:

```text
18 fixed initialization words
175 class-1 scene descriptors
12 class-3 scene descriptors
2 RES8 environment-profile backdrop/panorama tables
```

The environment profile ID is selected by `LEVELS.DAT header +0x290` values 0/1; `0xFF` uses the alternate/default level path. Placement azimuth, width, screen Y, RES8 base resolution, and terminal-zero records are executable-backed.

## SINTAB

Canonical directory: `MISC/SINTAB/`

Structurally reconstructed as the first-person renderer support/microcode package rather than merely a sine table.

Contains fixed trig/projection data and 127 scale blocks, each with:

```text
129 uint16 code offsets
129 uint16 resampling steps
variable x86 rasterizer code
```

All 129 type-7 selector values are valid in every block. Complete supplied type-7 linkage is validated.

## T

Canonical directory: `MISC/T/`

Indexed text archive:

```text
uint32 count = 1073
uint32 offsets[1073]
record:
    uint16 encoded_length
    byte encoded_text[encoded_length]
plain = encoded XOR 0xDC
```

25 zero-length records and all unreferenced slack/residue are preserved. Round-trip is byte-identical.

## SAMPLE.AD

Canonical directory: `MISC/SAMPLE.AD/`

Confirmed from original Miles AIL source as a YM3812/OPL2 Global Timbre Library (GTL).

Supplied file contains 233 indexed timbres across MIDI bank/patch namespaces; header offsets and all 14-byte OPL2 timbre records close exactly at EOF.

This is a Miles middleware asset, not a DreamForge-native format.

---

# GM1.BNK / GM2.BNK — ROLE COMPLETE; INNER VENDOR GRAMMAR DEFERRED

Canonical files:

- `BNK/ARIA_BANKS.md`
- `BNK/GM1.BNK/README.md`
- `BNK/GM2.BNK/README.md`

Confirmed classification:

- Sierra Semiconductor ARIA General MIDI support banks;
- `GM1.BNK` corresponds to 512 KiB ROM configuration;
- `GM2.BNK` corresponds to 1 MiB ROM configuration;
- Ravenloft ships the corresponding Miles AIL/32 ARIA XMIDI driver `A32ARXM.DLL`.

They are **not** AdLib Instrument Maker BNKs and must not be parsed using the OPL GTL source-bank grammar.

The publicly preserved Miles AIL 2.14 tree also contains GM1/GM2, establishing middleware provenance, but those reference files have different sizes and are not canonical substitutes for Ravenloft's shipped bytes.

### Deliberately deferred

Inner ARIA record/field semantics require either:

1. an authentic ARIA bank specification; or
2. reconstruction of Ravenloft's shipped `A32ARXM.DLL` consumer.

That belongs to the `Runtime/` third-party-driver track. It is **not a blocker** for the initial faithful DreamForge engine/resource reconstruction because the native game/resource code treats these as external driver support assets.

---

# What is not a blocker

The following remain evidence-class `UNKNOWN` or deferred, but do not represent missing parsing of active Ravenloft resource behavior:

1. historical producer purpose of the wordcode zero companion arrays;
2. exact edition/build provenance of RES11/RES12 alternate content;
3. producer meaning of the two inactive RES1 `0x00000060` records;
4. original source/editor names for several physically understood fields;
5. semantic names inside executable-only type 5, absent from Ravenloft data;
6. inner Sierra ARIA GM1/GM2 bank grammar, which is owned by the third-party audio driver;
7. ANM chunk type 4 higher-level producer semantics, because the supplied cinematics never use it.

These should remain explicit unknowns unless new primary evidence appears. Cross-title files may be used later as comparative evidence, never as retroactive proof of Ravenloft behavior.

# Recommended handoff

For the **initial faithful decompilation / native-port foundation**, the Resources track can now be treated as complete enough to hand off to:

- Technical Archive integration;
- engine-facing resource APIs/loaders;
- deterministic extraction/repacking tests;
- modern rendering/audio backend design while retaining original-format compatibility.

Optional future work should be scheduled under the appropriate scope:

- `A32ARXM.DLL` if exact ARIA bank semantics become important;
- alternate-build executables if RES11/RES12 provenance needs identification;
- other DreamForge titles only for comparative-family archaeology after Ravenloft's own evidence remains clearly separated.
