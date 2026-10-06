# RES executable-supported formats absent from the supplied corpus

Status: **loader mechanics reconstructed from the certified CODE.2 object.** These paths matter to engine preservation even though the supplied Ravenloft RES archives do not instantiate them. Physical behavior is documented separately from unproved semantics.

## Type-1 compression method 1: signed-control RLE

Type-1 resources use the 8-byte image descriptor documented in `FORMAT.md`. Descriptor byte `+0x06` selects compression method `0..3` through the four-entry jump table at CODE.2 `2FD8C`:

```text
method 0 -> 2FEBC   raw byte stream
method 1 -> 2FF32   signed-control RLE
method 2 -> 2FFA4   encoded error path
method 3 -> 2FFAB   4 KiB LZSS decoder family
```

No supplied Ravenloft type-1 or type-2 resource uses method 1, but its decoder `2FC48` is completely recoverable.

### Byte grammar

The compressed stream consists of control groups. Interpret each control byte as signed int8.

```text
control >= 0:
    literal_count = control + 1       // 1..128
    copy literal_count following bytes

control < 0:
    repeat_count = 1 - control        // 2..129
    value = next byte
    output value repeat_count times
```

Equivalent pseudocode:

```text
while output_needed:
    c = int8(read_u8())
    if c >= 0:
        n = c + 1
        repeat n:
            emit(read_u8())
    else:
        n = 1 - c
        v = read_u8()
        repeat n:
            emit(v)
```

The implementation buffers one decoded run internally and returns one byte per `2FC48` call. Type-1 loader `2FF32..2FFA2` calls it until each element receives exactly:

```text
height * width_units * 8
```

bytes, repeating for `count` elements.

### Important non-PackBits edge case

This resembles PackBits but is **not byte-for-byte PackBits semantics**.

For control `0x80` / signed `-128`, CODE.2 executes 8-bit `NEG`, then `INC`, producing `0x81` / 129. It therefore repeats the following byte **129 times**.

Classic PackBits reserves `-128` as a no-op. A faithful Ravenloft/DreamForge implementation must preserve the executable's 129-byte behavior instead of substituting a generic PackBits library.

### Decoder state

`2FC48` keeps the current decoded run in static state (`0xF4DC`, `0xF4E0`, temporary bytes at `0xF4E4`). The type-1 method-1 branch does not reset that state at every image element; a run can therefore continue across the loader's internal element boundary.

No Ravenloft sample exists to establish producer conventions such as whether encoded groups were deliberately aligned to element boundaries. Port code should reproduce the continuous decoded-byte stream rather than resetting RLE state per element.

## Type-1 method 2

Compression selector 2 dispatches directly to `2FFA4`, which reports the encoded resource error through `2F580`. There is no decoder behind this method in the certified CODE.2 object.

No supplied resource uses method 2.

## Resource type 5 loader contract

Resource type 5 dispatches to CODE.2 `374E0`. No supplied RES archive contains an `EH` resource of type 5, so semantic field names cannot be validated from Ravenloft data. The loader's physical contract is nevertheless exact.

The normal `EH` reader `2EB60` has already read `type_header_size` bytes into memory before calling the type-specific loader. `374E0` assumes at least 28 bytes are available at that header pointer and consumes:

```text
header +0x00  uint16 unknown_word_00
header +0x02  uint16 unknown_word_02
header +0x04  uint16 unknown_word_04
header +0x06  uint16 unknown_word_06
header +0x08  uint16 unknown_word_08
header +0x0A  uint16 unknown_word_0A
header +0x0C  uint16 unknown_word_0C
header +0x0E  uint16 unknown_word_0E
header +0x10  uint16 unknown_word_10
header +0x12  uint16 unknown_word_12
header +0x14  uint16 unknown_word_14
header +0x16  uint32 tail_span
header +0x1A  uint8  unknown_byte_1A
header +0x1B  uint8  tail_first_byte
```

The loader then allocates:

```text
allocation_size = tail_span + 0x1B
```

copies those first 28 bytes verbatim to the returned object, and reads:

```text
tail_span - 1
```

additional bytes from the resource payload into returned offset `+0x1C`.

Thus, in the returned object, `tail_span` is exactly the number of bytes beginning at offset `+0x1B`:

```text
returned +0x1B  tail_first_byte       // already present in type header
returned +0x1C  remaining tail bytes  // read from payload
```

and total returned size is:

```text
0x1B + tail_span
```

### Physical on-disk contract

If the producer uses the natural header size of exactly 28 bytes, the record is:

```text
"EH"
uint8  type = 5
uint16 type_header_size = 28
byte   fixed_header[28]
byte   remaining_tail[tail_span - 1]
```

The executable itself does not explicitly compare `type_header_size` with 28 in `374E0`; it simply assumes those 28 header bytes exist. Therefore **28 is the loader-required minimum/read footprint**, while `type_header_size == 28` remains an executable-derived natural form rather than a corpus-validated universal rule.

`tail_span` must be at least 1 for the loader's `tail_span - 1` copy length to be sensible.

## What remains unknown for type 5

Without a supplied instance or a proved higher-level consumer that assigns meaning to the returned fields, the following remain `UNKNOWN`:

- source/editor class name;
- meanings of the eleven leading words;
- meaning of byte `+0x1A`;
- semantic content of the variable tail;
- whether another DreamForge title uses exactly the same producer grammar.

These are not required to reproduce Ravenloft's shipped runtime because no Ravenloft archive entry selects type 5. They are retained here so a future cross-title comparison or recovered producer asset can be tested against an exact executable loader contract rather than rediscovered from scratch.

## Preservation boundary

For a faithful engine reconstruction:

- implement method-1 RLE exactly, including the `-128 -> 129-byte repeat` behavior;
- keep method 2 as an unsupported/error selector unless later executable evidence says otherwise;
- retain a type-5 loader model matching `374E0`, but do not fabricate field semantics or synthetic Ravenloft resources;
- treat absence from the supplied corpus as evidence about this data set, not evidence that the code paths never existed.
