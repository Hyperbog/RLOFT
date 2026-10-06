# RES type-7 compiled scene-shape display lists

Status: **the supplied type-7 resources are executable-confirmed first-person scene-object shape records. Each decoded body contains a 10-byte geometric header, a per-source-column relative-pointer table, and zero-terminated lists of 6-byte rasterizer descriptors. CODE.2 uses the geometry for projected bounds and the lists to drive preloaded renderer microcode.**

The immutable RES bytes and original CODE.2 object are controlling evidence.

## Loader

Type 7 shares the type-6 LZSS loader `37590`:

```text
uint16 decoded_size
byte   compressed_data[]
```

The runtime allocation preserves the size word followed by the decoded body. Renderer consumers call archive 3 through `2F29C` and add 2 to reach the body.

All 8,362 supplied type-7 streams decode to their declared size and consume the compressed stream exactly.

## Geometric header

CODE.2 `21C1D..21C55` reads the first five signed/unsigned words of the decoded body while constructing projected scene-record bounds:

```text
+0x00 uint16 source_width
+0x02 int16  x_min
+0x04 int16  x_max
+0x06 int16  y_min
+0x08 int16  y_max
```

The routine uses:

```text
half_width = source_width / 2
x_span     = x_max - x_min
y_span     = y_max - y_min
```

and scales these values into the scene record's projected rectangle fields at `+0x0E..+0x14`.

Across the complete supplied corpus:

- `source_width` is only 64, 96, or 128;
- `source_width` is always positive and even;
- `0 <= x_min <= x_max < source_width`;
- `0 <= y_min <= y_max <= 128`.

The distribution is:

- width 64: 8,210 resources;
- width 96: 80 resources;
- width 128: 72 resources.

This is direct shape/sprite geometry used for first-person projected bounds, not merely metadata inferred from payload appearance.

## Per-column pointer table

Immediately after the 10-byte header is one little-endian 16-bit relative pointer for each source column in the inclusive x range:

```text
uint16 column_list_offset[x_max - x_min + 1]
```

Each offset is relative to the start of the decoded body.

CODE.2 `36CEC` splits traversal around `source_width / 2`:

- the left side is walked from the center toward `x_min` using the table in reverse;
- the right side is walked from the center toward `x_max` using the table forward.

The pre-decrement/pre-increment control flow means the table begins exactly at body `+0x0A`; the fifth geometry word at `+0x08` is **not** overlapped with the table.

Exhaustive validation of all 8,362 resources proves:

- table length is exactly `x_max - x_min + 1` words;
- every column pointer is at or after the end of the header/table;
- every pointer is inside the decoded body;
- duplicate column pointers are permitted and occur, allowing columns to share a draw list.

## Column draw lists

Each table entry addresses a zero-terminated list:

```text
repeat:
    int16 raster_stop_selector
    if raster_stop_selector == 0:
        end list
    int16 signed_adjustment
    int16 raster_entry_selector
```

Thus each active descriptor is exactly 6 bytes, and a list ends with a single zero 16-bit word.

The pointer itself may be odd; CODE.2 performs unaligned word reads, and many supplied records rely on that. A faithful parser must not impose word alignment on list addresses.

Across all supplied resources, every reachable list terminates before or exactly at decoded EOF. No pointer/list overrun occurs.

### Selector fields

CODE.2 `369D8` uses `raster_stop_selector` and `raster_entry_selector` as byte offsets into a preloaded renderer table/code block selected for the current scale. Both selectors are even in every supplied descriptor.

For `raster_stop_selector`, the engine:

1. reads a 16-bit relative target from the renderer block;
2. locates one byte at that target;
3. saves the original byte;
4. temporarily writes `0xC3` (`RET`).

For `raster_entry_selector`, it resolves another 16-bit relative target and calls that target through helper `369C0`.

After the generated/microcoded rasterizer returns, the saved byte is restored. This temporarily inserts a return point into the selected renderer microcode so the draw operation executes only the span represented by this descriptor.

`signed_adjustment` is added to the current renderer value before the micro-rasterizer call and passed in a register. It is preserved as a signed adjustment until the exact source-level visual parameter is independently named.

## Scale-dependent renderer microcode

`36CEC` derives a scale index from its projected-size argument and selects a pointer from the table at `0x18FE0`.

That pointer family is loaded during renderer initialization (`3838` / `39F0..3A50`) from external renderer data. The selected block contains both:

- offset tables used by the type-7 descriptors;
- executable rasterizer byte sequences patched/restored by `36C10`, `36C44`, `36C90`, `36CBC`, and `369D8`.

Type 7 therefore does not store ordinary row-major or column-major pixel bytes. It stores **shape geometry plus display-list descriptors that select spans of scale-specific renderer microcode**.

## Scene-object renderer use

`36CEC` is called repeatedly from the first-person scene renderer (`2C4E1`, `2C524`, `2C5AB`, `2C5FF`, `2C632`, `2C665`, `2C6B6`). Its second argument is derived from the 0x16-byte scene record's resource field and is passed directly as the archive-3 resource index.

The same type-7 resource is also loaded by `21BF0`, which scales the header geometry into the scene record's projected bounding rectangle. This independently connects the format to visible scene-object shape rendering.

The safe project name is therefore **compiled scene-shape column display list**. “Sprite” is a useful informal description of the visible result, but the on-disk payload is not a conventional bitmap sprite; it is a compact shape/display-list representation coupled to DreamForge's scale-specific software rasterizer.

## Structural validator

The RES validator can safely verify, for every type-7 decoded body:

1. body is at least 10 bytes;
2. source width is positive/even;
3. x/y bounds are ordered and within the proved supplied domains;
4. the complete per-column pointer table fits in the body;
5. each pointer starts after that table and lies within the body;
6. each referenced list consists of zero or more 6-byte descriptors followed by a zero word;
7. each selector word used as a renderer-table byte offset is even;
8. no list read exceeds decoded EOF.

These checks hold for all 8,362 supplied resources.

## Remaining semantic frontier

- assign stronger names to the three 6-byte descriptor fields only after the scale-rasterizer instruction family is fully reconstructed;
- map archive-3 IDs to `NPCS.DAT`, `OBJECTS.DAT`, and level scene-resource selectors;
- correlate the 64/96/128 source-width families with named game objects and animation groups;
- document the external scale-rasterizer data loaded by `3838` as its own preserved format/component.
