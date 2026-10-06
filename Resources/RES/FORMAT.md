# RES format reconstruction

Status: **outer archives are byte-exact round-trippable; supplied EH types 1, 2, 3, 4, 6 and 7 are structurally decoded from CODE.2 and exhaustively validated. Type 5 exists in the executable dispatcher but is absent from the supplied archives.**

The immutable RES files and original CODE.2 bytes are controlling evidence. Community extraction tools are secondary references only.

## Outer archive

Every supplied `RES0`..`RES12` uses:

```text
int32 entry_count
int32 entry_offsets[entry_count]
byte  entry_payloads[]
```

Offsets are little-endian absolute file positions. `entry_offsets[0] == 4 + 4*entry_count`. Offsets are **non-decreasing**, not strictly increasing. Entry `i` occupies `[offset[i], offset[i+1])`; the final entry extends to EOF.

`RES1` contains 21 adjacent duplicate offsets and therefore 21 valid zero-length entries. `analyze_res.py` reconstructs all 13 outer archives byte-for-byte, including these duplicates.

## EH envelope and CODE.2 dispatch

CODE.2 `2EB60..2EC72` reads each resource envelope as:

```text
byte   magic[2]          // must be "EH" for normal typed resource path
uint8  resource_type
uint16 type_header_size
byte   type_header[type_header_size]
byte   payload[...]      // interpreted by selected loader
```

The executable dispatch at `2EDDA..2EEF7`, using the seven-entry table at `2EC98..2ECB3`, maps types exactly as follows:

| Type | CODE.2 loader | Supplied count | Confirmed supplied role |
|---:|---|---:|---|
| 1 | `2FD9C` | 1,591 | image/sprite descriptor + raw or LZSS pixel data |
| 2 | `301A8` | 15 | 128-entry glyph-width table + type-1-compatible font image descriptor |
| 3 | `286C4` | 172 | length-prefixed `FORM` / `XDIR` XMIDI sequence data |
| 4 | `28020` | 1,005 | length-prefixed Creative Voice File sampled-sound data |
| 5 | `374E0` | 0 | executable-supported structured resource; no supplied instance, semantics unresolved |
| 6 | `37590` | 1,319 | LZSS-compressed first-person column-major texture raster |
| 7 | `37590` | 8,362 | LZSS-compressed structured renderer geometry/data record |

Types 6 and 7 intentionally share the same generic decompression loader; their higher-level interpretation comes from their archive/consumer paths rather than a different on-disk compression wrapper.

## Type 1: image/sprite resource

All supplied type-1 resources use `type_header_size == 8`:

```text
+0x00 uint16 count
+0x02 int16  height
+0x04 uint8  width_units       // decoded width contribution is width_units * 8 bytes
+0x05 uint8  field_5           // semantic name not yet promoted
+0x06 uint8  compression
+0x07 uint8  field_7           // semantic name not yet promoted
```

CODE.2 `2FD9C` computes decoded bytes per element as:

```text
height * width_units * 8
```

and processes `count` elements.

Supplied compression methods:

- method `0`: 113 resources; payload is uncompressed and its length equals `count * height * width_units * 8` exactly in every case;
- method `3`: 1,478 resources; decoded by the CODE.2 `37700` LZSS family and consumes every compressed payload exactly;
- method `1` is supported by CODE.2 (`2FF32` / `2FC48`) but does not occur in the supplied archives;
- method `2` routes to the encoded error path at `2FFA4` and does not occur in the supplied archives.

The exact higher-level meaning of header bytes `+0x05` and `+0x07` remains deliberately unnamed until their renderer consumers are fully reconciled.

## Type 2: font resource

Every supplied type-2 resource uses `type_header_size == 136 (0x88)`.

CODE.2 `301A8` proves that the header is:

```text
+0x00 uint8 glyph_widths[128]
+0x80 uint16 count
+0x82 int16  height
+0x84 uint8  width_units
+0x85 uint8  field_5
+0x86 uint8  compression
+0x87 uint8  field_7
```

The final 8 bytes are not a separate unrelated font descriptor: `301A8` builds a temporary resource descriptor pointing at `header + 0x80` and calls the type-1 loader `2FD9C`. Thus the image body obeys the same decoded-size and compression rules as type 1, while the first 128 bytes remain attached as per-glyph width data.

All 15 supplied fonts have `count == 128`, `width_units == 1`, compression method `3`, and decode with exact compressed-stream exhaustion. Decoded bitmap sizes are 10,240 bytes (1 resource), 11,264 bytes (1), or 15,360 bytes (13), according to height.

## Type 3: XMIDI/XDIR sequence resource

All supplied type-3 resources have `type_header_size == 0` and use:

```text
uint32 blob_size
byte   blob[blob_size]
```

CODE.2 `286C4` reads the 32-bit length, allocates storage, stores the length, and copies that many bytes. All 172 supplied records satisfy `stored blob bytes == blob_size` exactly and every blob begins:

```text
FORM .... XDIR
```

This class belongs to the music/sequence subsystem. Independent DAT work already ties the neighboring `28Exx/29xxx` family to level music-sequence IDs and transitions.

## Type 4: Creative Voice File sampled sound

Normal supplied type-4 resources have `type_header_size == 0` and the same loader envelope:

```text
uint32 blob_size
byte   blob[blob_size]
```

CODE.2 `28020` reads the dword length, allocates `length + 4`, stores the length, then copies exactly `length` bytes. Every supplied blob starts with the literal `Creative Voice File\x1A` signature.

1,003 of 1,005 resources have an exact declared/stored length match. Two byte-identical shipped resources are anomalous:

- `RES11:4877`
- `RES12:430`
- entry SHA-256: `07db990a79ea8d02e89c150fec3d45a918a6efb563a99feb3a21f090315959eb`
- declared length: `71,273 (0x11669)`
- bytes physically stored after the length: `5,737 (0x1669)`
- discrepancy: exactly `65,536 (0x10000)` bytes

The executable reads the field as a full dword, so this discrepancy is preserved as a **shipped data anomaly**, not silently normalized to a 16-bit length. Its runtime consequence requires separate stream-boundary analysis.

## Types 6 and 7: shared LZSS wrapper

All supplied types 6 and 7 have `type_header_size == 0`:

```text
uint16 decoded_size
byte   compressed_data[...]
```

CODE.2 `37590` reads the size, allocates `decoded_size + 2`, stores the leading size word, initializes the compressed stream, and invokes the `37700` decompressor.

The reconstructed LZSS grammar is:

- 4,096-byte history window;
- history initialized to `0xFE`;
- initial history cursor `0xFEE`;
- flag bits consumed least-significant-bit first;
- flag 1: one literal byte;
- flag 0: two-byte back-reference;
- back-reference offset = `lo | ((hi & 0xF0) << 4)`;
- back-reference length = `(hi & 0x0F) + 3`;
- history cursor wraps modulo `0x1000`.

Exhaustive validation:

- all 1,319 type-6 streams decode to the declared size and consume their compressed payload exactly;
- all 8,362 type-7 streams decode to the declared size and consume their compressed payload exactly.

### Type 6: column-major first-person surface textures

The supplied type-6 records decode only to 4,096 bytes (134 resources) or 8,192 bytes (1,185 resources). The exact layout is now executable-backed rather than inferred from those byte counts.

CODE.2 `32F93..32FA0` computes renderer record field `+0x10` as `(0x255E8 >> 4) & 0x0FC0`, so the selector advances only in `0x40` units. Loader paths `33647..33664` and `336AF..336D4` double that field and add it to the archive-4 decoded-body pointer, producing an exact `0x80` / 128-byte source-column stride.

Sampler `338C8` independently proves that each selected block is one vertical source column: it computes fixed-point step `0x800000 / output_height`, where `0x800000 == 128 << 16`, reads one source byte per resampled scanline, subtracts a color/shading bias, writes it to VGA memory, and advances destination by `0x50` / 80 bytes (one 320-pixel planar scanline).

Therefore the supplied decoded bodies are exactly:

- 4,096 bytes = **32 columns x 128 texels**;
- 8,192 bytes = **64 columns x 128 texels**.

Storage is column-major: each contiguous 128-byte block is a complete vertical texture column. The safe project classification is **column-major first-person surface/wall texture raster**. See `TYPE6_RENDERER.md` for the full instruction-level evidence.

### Type 7 consumer evidence

Every supplied type-7 decoded body is at least 10 bytes. Renderer paths `21C1D..21C55` and `36E94..36F04` request archive `3`, skip the leading decoded-size word, then consume the body as structured 16-bit data.

The first five signed words are used as geometry:

```text
word0: reference extent used by scaling/centering math
word1, word2: ordered pair; renderer uses word2 - word1
word3, word4: ordered pair; renderer uses word4 - word3
```

Across all 8,362 supplied type-7 bodies, `word2 >= word1` and `word4 >= word3`. Further data contains word offsets/index structures used by the renderer (`36EF5..36F20`). The safe classification is **structured renderer geometry/data record**; a historical source-level name such as “sprite” is not asserted without stronger evidence.

## Non-EH records

There are 77 non-empty entries that do not begin with `EH`:

- `RES1:381` and `RES1:427`: four-byte `60 00 00 00` sentinels, still semantically unresolved;
- `RES9:517..554`: 38 executable-proved interaction/display wordcode containers;
- `RES12:370..406`: 37 ID-aligned alternate wordcode-shaped containers whose selection by this executable is not yet proven.

All 75 larger records use the exact serialized shape:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

Every supplied `zero_bank` word is zero. CODE.2 `13F70` bypasses the normal `EH` loader, opens archive 9 directly, seeks entry `0x258CC + 0x7FCC + program_id`, reads the leading count and exactly `count * 2` bytes of `program_words`, and does not read the zero bank. The supplied `INIT` loads `0x258CC = 103` from `+0x1E` and `0x7FCC = 414` from `+0x22`, so the runtime bank begins exactly at `103 + 414 = RES9:517` and covers program IDs 0..37.

CODE.2 `15534` dispatches wordcode opcodes `0..0x29` through a 42-entry table. Proven operations include archive-9 drawing, absolute branches, text-base changes, sound playback, music transitions, interpreter state-bit operations, and cinematic invocation. The safe classification is therefore **interaction/display wordcode** rather than malformed resources.

`RES12:370..406` has the identical count/program/zero-bank shape, and all 37 shared ordinal slots have the same early opcode-`0x19` text-base operand as the corresponding `RES9` programs. No equivalent raw archive-12 loader has yet been proven, so it remains an ID-aligned alternate bank rather than an asserted active runtime bank.

See `NON_EH.md` for the detailed loader/interpreter evidence.

## Validation status

`analyze_res.py` now validates the complete supplied corpus:

- 13 outer archives; byte-identical parse/reassembly;
- 12,562 total entries;
- 21 zero-length entries;
- 77 non-EH entries;
- 12,464 EH entries across types 1, 2, 3, 4, 6, and 7;
- exact method-0 size checks for all 113 supplied type-1 raw resources;
- exact method-3 LZSS stream exhaustion for all 1,478 type-1 and all 15 type-2 resources;
- exact XDIR length/signature checks for all 172 type-3 resources;
- exact VOC signature checks for all 1,005 type-4 resources, with the two documented shipped length anomalies;
- exact LZSS stream exhaustion for all 9,681 type-6/type-7 resources.

## Remaining work

1. finish all 42 interaction/display wordcode opcode semantics and determine the purpose of the serialized zero companion bank;
2. trace `RES12` alternate-bank selection or prove it unused by this executable, and resolve the two `RES1` sentinels;
3. continue type-7 structure recovery beyond the confirmed leading geometry fields and offset tables;
4. map RES texture/program/geometry indices back into LEVELS/OBJECTS/NPCS consumers and renderer resource codes;
5. reconstruct CODE.2's method-1 type-1 decoder even though no supplied resource uses it, for complete engine preservation;
6. keep type 5 explicitly unresolved until a sample or enough executable evidence establishes its format independently.
