# INIT

- Bytes: 1,879
- SHA-256: `cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276`
- Source upload: `RES+OTHER.zip`
- Family: MISC

## Current reconstruction status

The supplied `INIT` file is **structurally closed end-to-end** against CODE.2. `analyze_init.py` parses and byte-identically reserializes all 1,879 bytes.

Its serialized sections are:

```text
0x0000..0x0023  18 little-endian initialization words
0x0024..0x04EE  175 source-class-1 scene descriptors (7 bytes each)
0x04EF..0x0544   12 source-class-3 scene descriptors (7 bytes each)
0x0545..0x0756    2 environment-profile backdrop/panorama tables
```

Confirmed fixed-header relationships now include:

- `+0x00 = 15`: RES0 UI-graphics base; entries `0..14` are the 15 type-2 fonts and UI/raster consumers use archive 0 relative to this base;
- `+0x02 = 168`: RES1 object/inventory UI base; `OBJECTS.DAT +0x2D` values `0..167` map exactly to `RES1:168..335`;
- `+0x04 = 336`: RES1 character-face base; CODE.2 uses `face_index + sex*23` and adds 46 when status bit `0x0100` selects the alternate portrait bank;
- four RES3/type-7 source-bank starts at `+0x06..+0x0C`;
- archive-9 sound/resource/wordcode bases at `+0x1C..+0x22`;
- class-1 and class-3 scene shape descriptor tables;
- two environment-profile tables selected directly by `LEVELS.DAT header +0x290` values `0` and `1`;
- environment graphics resolved through archive 8 / `RES8` with azimuth-anchored placement records.

See:

- `FORMAT.md` for the complete serialized grammar and fixed-header relationships;
- `UI_RESOURCE_BASES.md` for the first three INIT words and RES0/RES1 UI/portrait mappings;
- `ENVIRONMENT_PROFILES.md` for the final two profile tables and renderer semantics;
- `analyze_init.py` for reproducible parsing/round-trip validation.

Some fixed-header words still lack historical source/editor names. Structural closure does not justify inventing labels for fields whose runtime meaning remains only partially characterized.
