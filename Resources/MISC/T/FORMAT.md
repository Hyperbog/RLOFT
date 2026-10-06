# T indexed text resource

Status: **the supplied `T` file is structurally reconstructed as an offset-indexed, length-prefixed, bytewise-XOR-obfuscated text archive. CODE.2 routine `1C368` proves the active lookup/decode algorithm. All 1,073 supplied records validate against the file's active offset table. Unreferenced slack bytes are intentionally preserved and are not interpreted as text.**

## Controlling evidence

- supplied file: `T`
- bytes: `133,584`
- SHA-256: `a3f1e19e40f226c8cf150a43aacd9c378ee35a1294d2c9dd72870bfdf1e16315`
- CODE.2 indexed reader: logical `1C368..1C408`
- runtime file handle: DGROUP `0x22224`

The immutable file and original CODE.2 object are controlling evidence.

## Physical layout

The supplied file begins with:

```text
+0x0000  uint32 stored_count = 1073
+0x0004  uint32 offsets[1073]
          byte   unreferenced/slack bytes
          text records at offsets[]
```

The active 1,073-entry offset table occupies bytes `0..4295` (`0x0000..0x10C7`). The first referenced record begins at absolute file offset `4572` (`0x11DC`), leaving **276 unreferenced bytes** between the active offset table and the first text record.

The first payload offset is arithmetically equivalent to room for 69 additional dwords beyond the active table, but those 276 bytes are **not** treated as additional offsets. They contain unrelated/stale byte material and CODE.2's indexed reader never consults them as table entries.

The leading dword's value agrees exactly with the number of active offsets in the supplied file. The indexed reader itself does not read or bounds-check against that count when resolving an individual record, so a faithful low-level implementation should distinguish the serialized count from the reader's encoded behavior.

## Record lookup proved by CODE.2

Routine `1C368(destination,index)` performs:

```text
seek(T, 4 + index * 4)
read uint32 record_offset
seek(T, record_offset)
read uint16 encoded_length
read encoded_length bytes into destination
destination[encoded_length] = 0
for every payload byte:
    byte ^= 0xDC
```

Thus one referenced record is:

```text
record_offset:
    uint16 payload_length
    uint8  encoded_payload[payload_length]
```

The obfuscation is a direct bytewise XOR with `0xDC`; there is no compression step in this reader.

Instruction order matters for preservation: CODE.2 follows the offset table, not the end of the previous payload. Bytes between one record's encoded payload and the next indexed offset are therefore not part of either text string.

## Supplied-corpus structure

Direct validation of all 1,073 active offsets gives:

```text
file bytes                         133,584
stored count                         1,073
offsets strictly increasing          yes
first record offset                  4,572
active-table end                     4,296
pre-payload slack                      276 bytes
zero-length records                     25
maximum encoded payload              1,040 bytes
records ending at next offset        1,043
records followed by skipped slack       30
inter-record skipped slack           1,804 bytes
last record ends exactly at EOF        yes
decoded records containing NUL           0
preservation/patch round-trip exact
```

Zero-length records are valid indexed entries. Their offsets still participate in the table and must not be removed or collapsed.

## Unreferenced/slack bytes

There are two forms of bytes that the indexed reader does not consume as text:

1. the 276 bytes between the active offset table and record 0;
2. 30 gaps between the end of one length-prefixed record and the next indexed record offset.

The latter gaps total 1,804 bytes. They are highly nonuniform (`4..1012` bytes) and contain recognizable unrelated residue, including DOS-era file/directory strings and partial messages/table fragments. The largest gap follows text index 863 and is 1,012 bytes.

This evidence rules out treating the gaps as a required per-record alignment scheme. They are best preserved as **unreferenced serialized slack/residue**. A byte-faithful repacker must retain them exactly unless intentionally creating a normalized new file; a preservation tool must never decode or present them as active dialogue.

## Cross-system consumers

### RES9 interaction/display wordcode

Wordcode opcode `0x19` sets a per-program text-record base. Dialog operations then resolve text IDs relative to that base before calling the indexed reader.

The active 38-program bank has direct NPC identities (see `Resources/RES/WORDCODE_PROGRAM_INDEX.md`). Its text bases fall inside this `T` index namespace. Reused NPC/story variants deliberately share the same base, for example the outdoor/indoor forms of the same interaction character.

This is an executable-backed link among:

```text
NPCS.DAT identity
    -> RES9 wordcode program
    -> opcode-0x19 text base
    -> T indexed text records
```

### ANM cinematic subtitles/text

ANM chunk type 7 calls the same reader with:

```text
T index = encoded_chunk_word + 0x324
```

so cinematic subtitles/messages and interaction dialogue share this same indexed text archive.

### LEVELS.DAT event text

The executable-supported LEVELS event opcode 7 also calls `1C368` directly with its encoded text index. This independently establishes `T` as a game-wide indexed text/message resource rather than a wordcode-only dialogue file.

## Encoding and character interpretation

The physical transform proved by CODE.2 is only:

```text
plain_byte = encoded_byte XOR 0xDC
```

The reader appends a NUL in memory after decoding, but that terminator is **not stored in the file**. None of the 1,073 decoded supplied payloads contains an embedded NUL.

Higher-level character rendering belongs to the game's DOS text/font pipeline. Tools should preserve decoded byte values and avoid rewriting arbitrary bytes through Unicode unless they have an explicit reversible character-set policy.

## Reproducible validator

`analyze_t.py` verifies:

- stored count/table bounds;
- strictly increasing active offsets;
- each 16-bit length and payload staying before the next offset/EOF;
- XOR decoding;
- zero-length records;
- all unreferenced gap sizes;
- final-record EOF closure;
- byte-identical preservation/patch round-trip while preserving every slack byte.

## Safe implementation boundary

The validator starts from a preserved carrier (`bytearray(data)`) and rewrites the parsed active fields into it. Its exactness claim is therefore a **preservation/patch round-trip**, not an independent serializer rebuilt solely from an abstract record model.

A faithful reader/editor should model the file as **offset-addressed records plus preserved unreferenced regions**, not as a simple packed list.

In particular:

- preserve the leading dword and the 1,073 active offsets exactly when round-tripping the supplied file;
- use the table offset for every lookup rather than deriving record `i+1` from record `i`'s length;
- preserve all 276 pre-payload slack bytes and all 30 inter-record gaps unless deliberately normalizing into a new non-identical file;
- preserve zero-length records;
- XOR payload bytes with `0xDC` only;
- do not serialize the runtime NUL terminator;
- do not infer that stale/slack strings are game dialogue.
