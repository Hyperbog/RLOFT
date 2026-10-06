# RES format reconstruction

Status: **the supplied RES0..RES12 corpus is structurally reconstructed and byte-exact round-trippable. All supplied EH resource types are decoded/validated, type-6/type-7 renderer layouts are behaviorally mapped, the active non-EH wordcode bank is reconstructed, and executable-only type-1 method 1 / type-5 loader behavior is documented. Remaining unknowns are historical/producer semantics, not unparsed Ravenloft runtime data.**

The immutable RES files and certified CODE.2 object are controlling evidence. Community tools and cross-title data are secondary only.

## Outer archive

Every supplied archive uses:

```text
int32 entry_count
int32 entry_offsets[entry_count]
byte  entry_payloads[]
```

Offsets are little-endian absolute file positions. `entry_offsets[0] == 4 + 4*entry_count`. Offsets are **non-decreasing**, not strictly increasing. Entry `i` occupies `[offset[i], offset[i+1])`; the final entry extends to EOF.

Duplicate adjacent offsets are valid zero-length entries. `RES1` contains 21 of them. `analyze_res.py` reconstructs all 13 archives byte-for-byte, including those duplicates.

Supplied corpus totals:

```text
archives             13
bytes         99,663,006
entries           12,562
EH entries        12,464
zero-length           21
non-empty non-EH      77
```

## EH envelope and CODE.2 dispatch

CODE.2 `2EB60..2EC72` reads the normal typed envelope as:

```text
byte   magic[2]          // "EH"
uint8  resource_type
word   type_header_size    // 16-bit field; CODE.2 sign-extends it
byte   type_header[type_header_size]
byte   payload[...]
```

CODE.2 `2EB8E` and `2EB9F` load `type_header_size` with `MOVSX`. The physical field is therefore a 16-bit word with **signed runtime interpretation**; preservation tools may store the raw word, but a faithful runtime must not silently zero-extend it.

The dispatcher at `2EDDA..2EEF7` maps types 1..7 exactly:

| Type | Loader | Supplied count | Role |
|---:|---|---:|---|
| 1 | `2FD9C` | 1,591 | image/sprite raster resource |
| 2 | `301A8` | 15 | font widths + type-1-compatible raster |
| 3 | `286C4` | 172 | XMIDI/XDIR music sequence |
| 4 | `28020` | 1,005 | Creative Voice File sampled sound |
| 5 | `374E0` | 0 | executable-supported structured record; absent from corpus |
| 6 | `37590` | 1,319 | first-person column-major texture raster |
| 7 | `37590` | 8,362 | compiled scene-shape column display list |

Types 6 and 7 intentionally share the same generic LZSS loader; higher-level meaning is established by their archive/consumer paths.

## Type 1 — image/sprite raster

All supplied type-1 resources use `type_header_size == 8`:

```text
+0x00 int16  count          // sign-extended by CODE.2
+0x02 int16  height         // sign-extended by CODE.2
+0x04 uint8  width_units       // width contribution = width_units * 8 bytes
+0x05 uint8  field_5           // conservative physical name
+0x06 uint8  compression
+0x07 uint8  field_7           // conservative physical name
```

CODE.2 computes decoded bytes per element as:

```text
height * width_units * 8
```

and processes `count` elements.

Compression selectors:

### Method 0 — raw

113 supplied resources. Payload size equals:

```text
count * height * width_units * 8
```

exactly in every instance.

### Method 1 — signed-control RLE

No supplied instance, but executable behavior is completely recovered from `2FF32` / `2FC48`:

```text
c = int8(read_u8())
if c >= 0:
    copy c + 1 literal bytes
else:
    repeat next byte 1 - c times
```

Important edge case: control `0x80` / `-128` repeats the next byte **129 times**. It is therefore not safe to substitute classic PackBits semantics, where `-128` is commonly a no-op.

The decoder state is continuous across the type-1 loader's internal element boundaries rather than reset for each element.

### Method 2 — error selector

Dispatches to the encoded resource-error path at `2FFA4`. No decoder exists behind this selector in the certified CODE.2 object and no supplied resource uses it.

### Method 3 — 4 KiB LZSS

1,478 supplied type-1 resources. CODE.2 `37700` uses:

- 4,096-byte history window;
- bytes `0x000..0xFED` initialized to `0xFE` by `37700`;
- bytes `0xFEE..0xFFF` are **not initialized by `37700`**;
- initial history cursor `0xFEE`;
- LSB-first flag bits;
- literal when flag=1;
- two-byte back-reference when flag=0;
- offset `lo | ((hi & 0xF0) << 4)`;
- length `(hi & 0x0F) + 3`.

The original corpus was previously certified for expected decoded sizes and exact compressed-payload exhaustion using a standalone decoder that prefilled all 4 KiB. After the 2026-10-06 executable re-audit, `analyze_res.py` models the final 18 bytes as undefined sentinels and fails if a stream reads one before it has been defined. Re-run that stricter validator against the canonical RES corpus before treating the partial-initialization edge as closed.

See `EXECUTABLE_ONLY_FORMATS.md` for method-1 and method-2 preservation details.

## Type 2 — font

Every supplied type-2 resource uses `type_header_size == 136 (0x88)`:

```text
+0x00 uint8 glyph_widths[128]
+0x80 int16  count          // sign-extended by CODE.2
+0x82 int16  height         // sign-extended by CODE.2
+0x84 uint8  width_units
+0x85 uint8  field_5
+0x86 uint8  compression
+0x87 uint8  field_7
```

CODE.2 `301A8` copies/retains the 128 glyph widths, points a temporary type-1 descriptor at `header +0x80`, and calls `2FD9C`. Thus font image data obeys the same type-1 decoded-size and compression rules.

All 15 supplied fonts have `count == 128`, `width_units == 1`, compression method 3, and exact compressed-stream exhaustion.

## Type 3 — XMIDI/XDIR sequence

All supplied type-3 resources use `type_header_size == 0`:

```text
uint32 blob_size
byte   blob[blob_size]
```

All 172 records match the stored length exactly and begin with `FORM ... XDIR`. Loader `286C4` belongs to the same music/sequence subsystem independently mapped through LEVELS/ANM/wordcode work.

## Type 4 — Creative Voice File

Normal supplied type-4 resources use `type_header_size == 0`:

```text
uint32 blob_size
byte   blob[blob_size]
```

All 1,005 blobs carry the `Creative Voice File\x1A` signature.

1,003 have exact declared/stored length equality. Two byte-identical shipped records preserve a genuine anomaly:

```text
RES11:4877
RES12:430
entry SHA-256 07db990a79ea8d02e89c150fec3d45a918a6efb563a99feb3a21f090315959eb
stored length field   0x11669
physical blob bytes   0x01669
difference            0x10000
```

CODE.2 reads the field as a 32-bit value, so tools must preserve the original bytes rather than silently truncate/repair it.

## Type 5 — executable-only structured record

No supplied Ravenloft archive contains type 5. Loader `374E0` nevertheless gives an exact physical contract.

It requires at least the following 28 header bytes:

```text
+0x00..+0x14  eleven uint16 words
+0x16         uint32 tail_span
+0x1A         uint8  unknown_byte_1A
+0x1B         uint8  tail_first_byte
```

The returned object contains those 28 bytes followed by `tail_span - 1` bytes copied from the payload. Total returned size is:

```text
0x1B + tail_span
```

`tail_span` therefore counts the variable tail beginning with header byte `+0x1B`.

A natural producer form would use `type_header_size == 28`, but because Ravenloft supplies no instance, only the loader's 28-byte minimum/read footprint is executable-proved. Field semantics remain `UNKNOWN`.

See `EXECUTABLE_ONLY_FORMATS.md`.

## Types 6 and 7 — shared LZSS wrapper

All supplied types 6 and 7 use `type_header_size == 0`:

```text
uint16 decoded_size
byte   compressed_data[]
```

Loader `37590` allocates `decoded_size + 2`, retains the size word, then invokes the same `37700` LZSS family described under type 1 method 3.

Exact validation:

```text
type 6   1,319 / 1,319 streams exact
type 7   8,362 / 8,362 streams exact
```

### Type 6 — column-major first-person texture

Renderer evidence proves one contiguous 128-byte vertical source column per column index.

Supplied decoded bodies are only:

```text
4,096 bytes = 32 columns * 128 texels
8,192 bytes = 64 columns * 128 texels
```

CODE.2 generates the column selector in `0x40` units, doubles it to a `0x80` byte stride, and `338C8` vertically resamples exactly 128 source samples into VGA memory.

See `TYPE6_RENDERER.md`.

### Type 7 — compiled scene-shape column display list

The supplied decoded body begins with a 10-byte geometry header:

```text
+0x00 uint16 source_width      // supplied: 64, 96, or 128
+0x02 int16  x_min
+0x04 int16  x_max
+0x06 int16  y_min
+0x08 int16  y_max
```

Immediately following is one relative 16-bit pointer for every source column `x_min..x_max`:

```text
uint16 column_list_offset[x_max - x_min + 1]
```

Each pointer targets a terminated list of 6-byte draw descriptors:

```text
int16 raster_stop_selector
int16 signed_adjustment
int16 raster_entry_selector
... repeated ...
int16 0                      // terminator
```

The first and third descriptor words select offsets in SINTAB's scale-specific executable rasterizer code. CODE.2 temporarily patches a selected stop point to `RET`, invokes the corresponding micro-rasterizer span, then restores the byte.

Corpus invariants across all 8,362 records:

- widths only 64/96/128;
- `x_min <= x_max < source_width`;
- `0 <= y_min <= y_max <= 128`;
- every column pointer starts after the header/table;
- every referenced list terminates in-bounds;
- selector fields collectively use all 129 even values `0x0000..0x0100`, exactly matching each SINTAB block's 129-entry code-offset table.

The type therefore represents a **compiled scene shape/display list coupled to external scale-specific rasterizer microcode**, not an ordinary bitmap sprite.

`INIT` partitions all 4,234 RES3 type-7 entries into four exact source banks:

```text
0..2168      NPC/creature family
2169..3315  placed object/item family
3316..4157  loose/pickable OBJECTS family
4158..4233  transient/moving-effect family
```

See:

- `TYPE7_RENDERER.md`
- `TYPE7_SOURCE_BANKS.md`
- `TYPE7_NPC_INDEX.md`
- `TYPE7_OBJECT_INDEX.md`
- `analyze_type7.py`
- `analyze_type7_banks.py`

## Non-EH records

There are 77 non-empty non-EH entries:

```text
RES1:381,427     two raw 4-byte 0x00000060 records
RES9:517..554    38 active interaction/display wordcode programs
RES12:370..406   37 ID-aligned alternate/incompatible programs
```

### Active RES9 wordcode

Physical container:

```text
uint16 word_count
uint16 program_words[word_count]
uint16 zero_bank[word_count]
```

The active loader `13F70` obtains raw archive 9 via `2EC74(9)`, reads the count, then reads **only** the first `word_count * 2` bytes into the VM buffer.

`INIT +0x1E == 103` and `INIT +0x22 == 414`, so:

```text
103 + 414 + program_id = RES9:517 + program_id
```

Program IDs 0..37 are therefore exactly `RES9:517..554`.

The complete active 42-opcode (`0x00..0x29`) wordcode VM is reconstructed and validates:

```text
programs                         38
program words                11,396
decoded instructions          4,237
parse errors                      0
invalid direct control targets    0
```

NPC types 34..70 map to programs 0..36 and NPC type 82 maps to program 37.

### Serialized zero bank

Every zero-bank word is zero, but the active loader stops after `program_words`. A whole-object call scan proves `13F76 -> 2EC74(9)` is the **only** direct raw-archive-handle request in certified CODE.2. No second reader later consumes the companion bank.

Thus it is serialized-but-runtime-unread in this executable. Its historical producer/editor purpose remains `UNKNOWN` and it must be preserved verbatim.

### RES12 alternate bank

RES12 has the same outer count/program/zero-bank container but an incompatible internal dialect, including inline character-template data where active CODE.2 expects a one-word template ID. No archive-12 raw-handle loader exists in the certified CODE.2 object.

Safe classification: **ID-aligned alternate/incompatible interaction-wordcode bank physically shipped on the same supplied CD**. The disc provenance is now known; no floppy/localization/prototype label or intended executable/build role is asserted without matching evidence.

### RES1 raw markers

`RES1:381` and `RES1:427` are four bytes `60 00 00 00`, each preceded by an empty entry. They cannot pass the normal EH loader, and the whole-object raw-handle audit finds no archive-1 raw reader. They are therefore inactive/unconsumed by established CODE.2 resource paths; historical producer meaning remains `UNKNOWN`.

See `NON_EH.md`, `WORDCODE_VM.md`, `WORDCODE_PROGRAM_INDEX.md`, `RES12_WORDCODE.md`, and `RES1_SENTINELS.md`.

## Validation ledger

The committed tooling verifies:

- 13 outer archives with byte-identical parse/reassembly;
- 12,562 total entries;
- 21 zero-length entries;
- 77 non-EH entries;
- 12,464 typed EH entries;
- all 113 supplied type-1 method-0 resources by exact raw size;
- all 1,478 supplied type-1 and all 15 type-2 method-3 streams by exact decode/stream exhaustion;
- all 172 type-3 resources by XDIR length/signature;
- all 1,005 type-4 resources by VOC signature and documented anomaly handling;
- all 1,319 type-6 and 8,362 type-7 LZSS streams by exact decode/stream exhaustion;
- all 8,362 type-7 display lists by geometry/pointer/list validation;
- all 38 active wordcode programs by instruction-boundary and control-target validation.

## Remaining evidence boundaries

No supplied Ravenloft RES entry remains structurally unparsed.

The intentional open questions are narrower:

- original producer/editor meaning of the unused wordcode zero bank;
- intended executable/build role of the CD-shipped RES12 alternate set;
- historical meaning of the two inactive RES1 `0x00000060` markers;
- source-level names for conservative type-1 descriptor fields `+0x05/+0x07`;
- semantic field names for executable-only type 5, which has no Ravenloft sample.

These are preservation `UNKNOWN`s. They do not block faithful parsing/repacking of the supplied RES archives or reconstruction of the certified executable's active runtime behavior.
