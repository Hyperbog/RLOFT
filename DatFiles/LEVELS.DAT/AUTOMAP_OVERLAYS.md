# LEVELS.DAT automap export overlays

Status: **the base-map buffer, placed-record overlay, annotation-label overlay, merge precedence, and annotation legend are instruction-proven from certified CODE.2.**

This document specializes the exporter discussion in `AUTOMAP.md`.

## Three row buffers

CODE.2 `026E8..02A33` allocates three buffers, each `map_height` bytes long, and rebuilds them for every outer map coordinate. In the transform used by the exporter, each buffer corresponds to one output row/column slice.

The buffers are:

1. base automap glyphs derived from cache `0x1760`;
2. placed-record/object overlay;
3. automap annotation-label overlay.

When producing the merged map, buffer 2 replaces non-space positions in the base buffer and buffer 3 then replaces non-space positions after that. Annotation labels therefore have final precedence over placed-record symbols at the same output position.

The non-merged output path writes the three buffers as separate lines before emitting the note legend.

## Placed-record overlay

For each non-`0xFF` map cell, the exporter calls `00700(column,row,start_index)` repeatedly. In the active level path, `00700` scans the 22-byte record family at runtime `0x22238`, bounded by header count `0x258A6` (family B), and returns records whose stored cell coordinates match the requested cell.

The exporter reads each matching record's type word at runtime `+0x0E` (`0x22246 + index*22`). It writes:

```text
type 0x3C..0x41  -> ASCII '*' (0x2A)
all other types  -> 0xFE
```

If multiple matching records occupy one cell, later matches overwrite the same output byte; therefore the final overlay character reflects the last returned matching record.

### OBJECTS.DAT cross-check

CODE.2 `168C8` opens the string at object-2 offset `0x704` (`OBJECTS.DAT`), reads its count, allocates `count * 0x41`, and loads the records to runtime pointer `0x255D8`.

The supplied `OBJECTS.DAT` is exactly:

```text
uint32 count = 336
336 * 0x41-byte records
```

Its entries `0x3C..0x41`, the exact range singled out by the exporter, are:

```text
0x3C  POTION OF EXTRA HEALING
0x3D  POTION OF FIRE RESIST
0x3E  POTION OF GIANT STRENGTH
0x3F  POTION OF HEALING
0x40  POTION OF SPEED
0x41  OIL OF FIERY BURNING
```

Family-B type IDs in all supplied levels range from `0..174`, entirely inside the 336-entry object table. Other family-B consumers test IDs that correspond to recognizable `OBJECTS.DAT` entries; for example type `0x9B` is `BLOOD KEY`, and CODE.2 `256C4` scans/removes visible family-B records of exactly that type.

Taken together, these executable and corpus relationships support describing family B as a **placed object/item record family**. Family C also uses IDs within the object database range, so the exact B-versus-C gameplay distinction remains open.

The byte `0xFE` is a literal exporter overlay value. Its final displayed glyph/category is intentionally left unnamed until the downstream character/font semantics are independently closed.


## Exact export traversal and output modes

A direct CODE.2 re-disassembly of `026E8..02A33` closes the physical text-output ordering.

The routine allocates three temporary buffers, each `map_height` bytes long. The outer loop advances across `map_width`; for each outer coordinate the inner loop advances across `map_height` and computes the source cell as:

```text
cell_index = inner_coordinate * map_width + outer_coordinate
```

The exported text is therefore a column-oriented/transposed traversal of the native row-major cell array: each emitted text line corresponds to one outer X-like map coordinate while positions within the line walk the row/Y-like coordinate.

The three buffers remain:

```text
buffer 1  resolved base-map CP437 bytes
buffer 2  placed-object overlay bytes
buffer 3  annotation-label overlay bytes
```

### Merged file output

In merged mode the routine combines them in this exact precedence order:

```text
result = base

if object_overlay != ' ':
    result = object_overlay

if annotation_overlay != ' ':
    result = annotation_overlay
```

It then writes the complete merged line followed by the encoded two-byte CR/LF line ending.

Thus:

```text
annotation > placed object > base terrain
```

when two layers occupy the same exported cell.

### Printer/stream output uses carriage-return overprinting

In the non-merged stream mode, `026E8` does **not** merge the three buffers in memory.

For each map line it writes:

```text
base buffer
CR

placed-object buffer
CR

annotation-label buffer
CR LF
```

The single carriage returns after the first and second layers return the print head to the beginning of the same physical line without advancing paper. The three textual layers are therefore designed to overprint on a DOS printer/compatible stream. The final CR/LF advances to the next physical map line.

This explains why the object and note buffers contain spaces everywhere they do not intend to replace the base layer: spaces leave already printed base characters visually intact on ordinary impact/text printers.

## SAVE and PRINT callers

The map-file SAVE path is now executable-proven.

CODE.2 contains the mutable path template:

```text
AUTOMAPS\\AUTO???.MAP
```

at object-2/DGROUP offset `0x5FDA` (physical CODE.2 file offset `0x7BEFE` in the supplied executable).

Routine `02A34` patches the three question-mark bytes, creates/opens the resulting file through the runtime file wrappers, calls `026E8` in merged-output mode, and closes the stream.

The digit stores must be preserved in their encoded order. The supplied routine writes:

```text
first placeholder   hundreds digit
second placeholder  ones digit
third placeholder   tens digit
```

so the filename generator has a machine-proven tens/ones ordering quirk. For level 22 this still yields `AUTO022.MAP`; values with unequal tens/ones can expose the difference. A compatibility implementation should reproduce the encoded order rather than silently replacing it with generic `%03d` formatting.

The PRINT caller at `03321` invokes the same exporter in non-merged mode with fixed FILE-like runtime object `0xF764`.

CODE.2 holds five consecutive Watcom FILE objects at `0xF6F4 + n*0x1C`; `0xF764` is the fifth. Open Watcom's DOS C runtime documents the startup standard streams in the order:

```text
stdin
stdout
stderr
stdaux
stdprn
```

Therefore the fifth object is the standard printer stream, `stdprn`. The CR-overprint behavior above is consequently the actual Ravenloft automap PRINT path, not merely a generic text-stream hypothesis.

Open Watcom C Library reference:
<https://openwatcom.org/ftp/manuals/1.5/clib.pdf>

## Annotation-label overlay

Routine `23B8` walks the variable-length list at DGROUP `0x5760`. Each in-memory entry begins:

```text
+0x00  int16 raw_x
+0x02  int16 raw_y
+0x04  int16 payload_length
+0x06  payload[payload_length]
```

A zero `payload_length` marks the in-memory terminator.

For each entry, `23B8` converts the stored coordinates using signed truncation toward zero:

```text
map_x = raw_x / 8
map_y = raw_y / 5
```

and compares them with the requested map coordinate. A matching entry causes the exporter to place its sequential annotation label into the third row buffer.

Labels begin with single-letter `A` through `Z`, represented as a two-byte field whose second byte is initially space. After `Z`, the routine advances into two-letter labels using the executable's own increment ordering. The corresponding legend routine `2474` uses exactly the same label progression, so map labels and legend entries remain synchronized without needing to impose a modern numbering convention.

## Serialized annotation blob

The level loader `18B39..18B69`:

1. zeroes `0x800` bytes at `0x5760`;
2. reads the common-tail 32-bit blob length;
3. if the length is below `0x7FF`, reads that many bytes into `0x5760`.

The writer calls `1C28`, which walks the `{6-byte header + payload}` entries until the zero-length terminator and returns the serialized byte length preceding that terminator. It writes that uint32 length and then exactly that many bytes from `0x5760`.

Therefore the common tail previously described generically as `text_length + text` is specifically the **automap annotation-list blob**:

```text
uint32 annotation_blob_length
uint8  annotation_blob[annotation_blob_length]
```

The terminating zero-length header is implicit in the zero-filled runtime buffer and is not included in the serialized length.

All 32 supplied archive records and `LEV022.DAT` have annotation length zero, so the supplied samples contain no saved player annotations.

## Annotation legend

Routine `2474` walks the same `0x5760` list. For each record it builds a line beginning with the same label plus `':'`, then copies the payload bytes. Carriage-return bytes split/flush legend output according to the encoded writer path.

This proves that the third exporter layer is a user-facing map annotation/note system rather than another terrain or object database.

## Safe implementation boundary

A faithful tool may now:

- parse and serialize the final annotation blob as variable records;
- expose signed raw annotation coordinates and payload bytes;
- reproduce the `/8` and `/5` map-coordinate matching rule;
- reproduce label generation using the encoded order;
- reproduce object-overlay `'*'` selection for family-B types `0x3C..0x41` and preserve literal `0xFE` for other records;
- merge overlays in the original precedence order;
- reproduce the transposed/column-oriented export traversal;
- reproduce merged-file CR/LF output;
- reproduce printer-layer CR overprinting;
- preserve the encoded AUTO filename digit-order quirk when strict compatibility is required.

Do not reinterpret annotation payload encoding beyond byte-preserving text behavior until the text/font path is separately required.