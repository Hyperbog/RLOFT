# ANM cinematic resources

This directory contains one analysis workspace per supplied `CINE00.ANM` through `CINE12.ANM` source file.

## Initial confirmed structure

Direct parsing of the supplied files establishes the outer ANM grammar:

```text
uint16 frame_count
uint16 version
repeat frame_count times:
    uint16 chunk_count
    repeat chunk_count times:
        uint16 chunk_id
        int32  chunk_size
        byte   payload[chunk_size]
```

Initial cross-checks against certified CODE.2 cinematic paths establish that the engine handles multiple image packet forms rather than one universal frame representation. In particular, `CINE00.ANM` through `CINE11.ANM` heavily use fixed-size chunk type 8 image payloads, while `CINE12.ANM` uses compressed type-2/type-3 image packets and no type-8 full-plane packets.

CODE.2 also establishes a native 320-byte scanline decode path over 400 scanlines for the packed cinematic image path, and type-8 payloads are exactly `0x14000` bytes (`4 * 0x5000`) copied as four VGA planes beginning at framebuffer byte offset `0x0FF0`. External tools that export these images as 640 pixels wide are therefore presentation/export logic, not evidence that the encoded native framebuffer width is 640 pixels.

## Secondary references

`LaZar00/LoftANM` is useful secondary evidence for chunk decoding and import/export behavior, but its interpretations are not controlling. Every field and opcode used here must be reconciled against the supplied ANM bytes and CODE.2.

## Next work

- inventory every chunk ID/size per cinematic;
- map command chunks to CODE.2 dispatch cases;
- reconstruct type-2/type-3 decompression byte-exactly;
- identify palette/subtitle/audio/timing command semantics;
- build parser/extractor before any serializer;
- require byte-identical round trip before declaring regeneration support.
