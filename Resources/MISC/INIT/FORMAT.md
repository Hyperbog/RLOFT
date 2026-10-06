# INIT format reconstruction

Status: **the supplied 1,879-byte `INIT` file is structurally reconstructed end-to-end.** CODE.2 consumes an 18-word fixed header, two variable-count 7-byte scene-descriptor tables, and two environment-profile backdrop/panorama tables. `analyze_init.py` parses every byte and reserializes the file byte-identically.

## Top-level layout

```text
0x0000  uint16 init_word[18]                         // 0x24 bytes
0x0024  uint16 class1_descriptor_count = 175
0x0026  scene_descriptor class1[175]                // 7 bytes each
0x04EF  uint16 class3_descriptor_count = 12
0x04F1  scene_descriptor class3[12]                 // 7 bytes each
0x0545  uint16 environment_profile_count = 2
0x0547  environment_profile profiles[2]             // variable length
0x0757  EOF
```

No bytes remain outside those structures in the supplied file.

## Fixed 0x24-byte initialization table

The supplied header contains:

```text
offset  value
+00      15
+02     168
+04     336
+06       0
+08    2169
+0A    3316
+0C    4158
+0E     600
+10      49
+12      49
+14      69
+16     166
+18     192
+1A     266
+1C      32
+1E     103
+20      48
+22     414
```

CODE.2 `16D04..17049` opens `INIT` and reads these words individually from offsets `0x00,0x02,...,0x22` into engine globals.

## `+0x06..+0x0C`: complete RES3 scene-shape bank partition

Four consecutive words become the source-bank table at DGROUP `0x27100..0x27106`:

```text
INIT +0x06 =    0 -> source class 0 RES3 base
INIT +0x08 = 2169 -> source class 1 RES3 base
INIT +0x0A = 3316 -> source class 2 RES3 base
INIT +0x0C = 4158 -> source class 3 RES3 base
```

Common helper `2B2F4` adds the source-class-selected base to the bank-local shape ID before geometry/render consumers request archive 3 through `2F29C`.

The 4,234-entry supplied RES3 archive is partitioned exactly:

```text
class 0     RES3 0..2168       2169 entries
class 1     RES3 2169..3315    1147 entries
class 2     RES3 3316..4157     842 entries
class 3     RES3 4158..4233      76 entries
                                   ----
                                   4234
```

The proved source families are:

```text
class 0  LEVELS family A / NPCS.DAT NPC-creature scene shapes
class 1  LEVELS family B / first INIT 7-byte scene-descriptor family
class 2  LEVELS family C loose/pickable items / OBJECTS.DAT definitions
class 3  second INIT 7-byte transient/moving-effect descriptor family
```

See `Resources/RES/TYPE7_SOURCE_BANKS.md` for the detailed renderer mapping.

## Scene-descriptor tables

After the fixed header, CODE.2 continues reading from the same open INIT stream.

### Source-class-1 table

Routine `1686C` reads:

```text
INIT +0x0024  uint16 descriptor_count = 175
INIT +0x0026  descriptor[175]          // 7 bytes each
```

The table occupies `0x0026..0x04EE`, and its count matches family-B type IDs `0..174`.

### Source-class-3 table

Routine `16934` then reads:

```text
INIT +0x04EF  uint16 descriptor_count = 12
INIT +0x04F1  descriptor[12]           // 7 bytes each
```

Both tables use the same physical descriptor:

```text
+0x00 uint16 local_shape_base
+0x02 uint16 spatial_extent_or_fixed_point_field
+0x04 uint8  shape_stride
+0x05 uint8  variant_count
+0x06 uint8  scene_influence_step
```

Executable evidence:

- class-1 emitter `2BC0A..2BC53` and class-3 emitter `2C055..2C09D` compute `local_shape_base + live_variant * shape_stride`;
- both pass `shape_stride` and source class to `2B2F4` for view/state-dependent subframe selection and RES3 base translation;
- update paths `1BF05..1BF36` and `1C07D..1C0A3` increment the live three-bit selector modulo `variant_count`;
- both scene emitters pass byte `+0x06` to common spatial-influence routine `2B088` when nonzero;
- class-1 collision/selection code consumes `+0x02` as a symmetric spatial extent when greater than one. Its historical/editor label remains conservative.

Corpus closure is exact:

```text
class 1 max(local_shape_base + shape_stride * variant_count) = 1147
class 3 max(local_shape_base + shape_stride * variant_count) =   76
```

These exactly match the two RES3 bank sizes derived independently from the fixed header.

## RES interaction/audio fields

```text
INIT +0x1C = 32  -> global 0x258C8
INIT +0x1E = 103 -> global 0x258CC
INIT +0x20 = 48  -> global 0x25614
INIT +0x22 = 414 -> global 0x7FCC
```

### `+0x1E`: archive-9 interaction-resource base

Wordcode drawing paths use `0x258CC` as the base for typed archive-9 resources. The supplied value is 103, so the interaction resource region begins at `RES9:103`.

### `+0x22`: archive-9 wordcode displacement

Raw loader `13F70` seeks archive-9 entry:

```text
0x258CC + 0x7FCC + program_id
```

With the supplied values:

```text
103 + 414 = 517
```

which exactly equals the first active non-EH program at `RES9:517`.

### `+0x1C`: sound-effect base

Wordcode opcode `0x1C` adds its encoded sound ID to `0x258C8` before direct playback through `27F24`. The supplied base is 32.

### `+0x20`: second sound/index base

A dialog/audio path adds the program sound sub-base and local sound ID to `0x25614` before playback. The supplied value is 48. A stronger historical source name remains deferred.

## Environment-profile tables at `+0x0545`

Routine `16990` consumes the remainder of INIT as:

```text
uint16 profile_count
repeat profile_count:
    uint16 res8_base
    uint16 stored_record_count
    placement placement[stored_record_count]

placement:
    int16 resource_selector
    int16 azimuth_degrees
    int16 draw_width_pixels
    int16 screen_y
```

The supplied file stores exactly two profiles:

```text
profile 0: res8_base=8,  stored_record_count=38
profile 1: res8_base=45, stored_record_count=27
```

Every list ends in one all-zero placement record and contains no earlier zero selector. Renderer `314B0` terminates on that zero selector even though `16990` also uses the serialized count for allocation/read size.

### Link to LEVELS.DAT

`LEVELS.DAT` fixed header byte `+0x290` is loaded to DGROUP `0x258A8` and has only values `0`, `1`, or `0xFF` in the shipped archive.

At `1F2B1`, any non-`0xFF` value is passed directly to `314B0(profile_id)`, which indexes the two INIT profile arrays loaded by `16990`. Thus profile IDs 0 and 1 correspond exactly to these two serialized environment-profile groups; `0xFF` takes the alternate/default level path.

### Link to RES8

`314B0` calls graphics helper `2986C`, which calls `2F29C` with archive number **8**.

For placement resources:

```text
if resource_selector >= 8:
    absolute_RES8_id = res8_base + resource_selector
else:
    absolute_RES8_id = resource_selector
```

All supplied active placement selectors are >=24. Resolved IDs are:

```text
profile 0 -> RES8 {32,33,35,36,37,38,39,40,41,42,43}
profile 1 -> RES8 {69,70,71,72}
```

All are valid inside the 73-entry supplied RES8 archive.

The same `res8_base` also selects the profile's phase-dependent backdrop resources before the placement walk, proving that it is the base of a profile-specific RES8 environment graphics range.

### Placement semantics

`resource_selector` is the archive-8 graphic selector described above. Zero is the list terminator.

`azimuth_degrees` is direct world/view angular placement: `314B0` subtracts current view angle `0xC658`, applies ±360-degree wrap, and converts the difference to screen X with the encoded projection arithmetic.

`draw_width_pixels` is halved for centering and used in horizontal clipping tests, directly proving a pixel width.

`screen_y` is passed directly as the draw helper's vertical coordinate.

See `ENVIRONMENT_PROFILES.md` for the full behavioral reconstruction.

## Structural closure and reproducibility

`analyze_init.py` validates that the supplied file has:

```text
18 fixed header words
175 class-1 descriptors
12 class-3 descriptors
2 environment profiles
38 / 27 stored environment records
EOF immediately after profile 1
```

It also reserializes the parsed representation byte-for-byte and can verify all resolved placement IDs against RES8.

## Safe implementation boundary

A faithful reader/repacker may expose:

- all 18 fixed header words while retaining conservative names for unresolved words;
- the four RES3 source-bank starts;
- the two 7-byte scene-descriptor tables and their proven shape/variant fields;
- the interaction/audio bases at `+0x1C..+0x22`;
- the two environment-profile groups and their RES8 base, angular placements, widths, and Y coordinates.

Do not convert bank-local scene-shape bases stored in INIT/NPCS/OBJECTS into absolute RES3 IDs on disk; the source-bank addition is a runtime translation. Preserve serialized list counts and terminal zero placement records exactly.
