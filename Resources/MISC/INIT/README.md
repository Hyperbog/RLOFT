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

The fixed header is now substantially behaviorally mapped. Confirmed resource relationships include:

- `+0x00 = 15`: first RES0 type-1/raster graphic, immediately after all 15 fonts;
- `+0x02 = 168`: RES1 OBJECTS inventory/UI graphic base;
- `+0x04 = 336`: RES1 character portrait base;
- `+0x06..+0x0C`: four RES3/type-7 source-bank starts;
- `+0x0E = 600`: RES4 secondary/narrow type-6 texture-bank base (600 64-column textures followed by 67 32-column textures);
- `+0x10 = 49`: alternate RES5/XMIDI resource-bank offset used by the music loader for specific driver modes;
- `+0x12/+0x14/+0x16/+0x18/+0x1A = 49/69/166/192/266`: RES6 sampled-SFX namespace boundaries/bases, with 266 equal to the supplied RES6 entry count and used only by an executable-supported but uninstantiated wordcode direct-SFX path;
- `+0x1C = 32`: archive-9 graphics/resource base at DGROUP `0x258EC`;
- `+0x1E = 103`: archive-9 interaction-resource base at DGROUP `0x258CC`;
- `+0x20 = 48`: archive-10 interaction voice/SFX base;
- `+0x22 = 414`: displacement from archive-9 interaction base to the active wordcode bank.

This corrects an earlier documentation slip that associated `+0x1C` with DGROUP `0x258C8`: immutable CODE.2 loads **`+0x1A` into `0x258C8`** and **`+0x1C` into `0x258EC`**.

See:

- `FORMAT.md` for the complete serialized grammar and fixed-header relationships;
- `ENVIRONMENT_PROFILES.md` for the final two profile tables and their renderer semantics;
- `analyze_init.py` for byte-exact structural validation;
- `analyze_resource_bases.py` for cross-archive validation of the fixed-header namespace bases.
