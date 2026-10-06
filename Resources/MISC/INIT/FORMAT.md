# INIT format reconstruction

Status: **the leading 0x24-byte table is directly consumed by CODE.2 as eighteen little-endian initialization values. Four of those words exactly partition the complete RES3/type-7 scene-shape archive, and the immediately following INIT tail contains the 175-entry source-class-1 and 12-entry source-class-3 scene descriptor tables. Interaction/text/audio base fields are also executable-resolved. Later INIT structures remain under reconstruction.**

## Fixed 0x24-byte initialization table

The supplied `INIT` begins with:

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

CODE.2 `16D04..17049` opens `INIT` and reads these words individually from file offsets `0x00,0x02,...,0x22` into engine globals.

## `+0x06..+0x0C`: complete RES3 scene-shape bank partition

Four consecutive words are stored at DGROUP `0x27100..0x27106` and are indexed by the first-person scene source class in common helper `2B2F4`:

```text
INIT +0x06 =    0 -> source class 0 RES3 base
INIT +0x08 = 2169 -> source class 1 RES3 base
INIT +0x0A = 3316 -> source class 2 RES3 base
INIT +0x0C = 4158 -> source class 3 RES3 base
```

`2B6B5..2B6C4` doubles the source-class index, reads one of these words, and adds it to scene-record word `+0x04`. The resulting value is later passed to `2F29C(..., archive=3)` by both geometry/bounds consumer `21BF0` and renderer `36CEC`.

Because supplied `RES3` contains exactly 4,234 entries, the four starts partition the entire archive without gap or overlap:

```text
class 0     RES3 0..2168       2169 entries
class 1     RES3 2169..3315    1147 entries
class 2     RES3 3316..4157     842 entries
class 3     RES3 4158..4233      76 entries
                                   ----
                                   4234
```

The scene databases therefore store bank-local shape IDs. CODE.2 translates them to absolute RES3 IDs only while building first-person scene records.

The currently proved source identities are:

```text
class 0  LEVELS family A / NPCS.DAT definitions
class 1  LEVELS family B / first INIT 7-byte descriptor table
class 2  LEVELS family C loose/pickable items / OBJECTS.DAT definitions
class 3  second INIT 7-byte descriptor table; higher gameplay identity unresolved
```

See `Resources/RES/TYPE7_SOURCE_BANKS.md`.

## INIT tail scene-descriptor tables

After CODE.2 reads the fixed 0x24-byte header, it does **not** close the file. It continues sequentially into two variable-length 7-byte descriptor tables.

### Source-class-1 table

Routine `1686C` reads at the current file position:

```text
INIT +0x0024  uint16 descriptor_count = 175
INIT +0x0026  descriptor[175]          // 7 bytes each
```

The table occupies `0x0026..0x04EE`, and its 175 descriptors match the supplied family-B type-ID namespace `0..174`.

### Source-class-3 table

Routine `16934` then reads:

```text
INIT +0x04EF  uint16 descriptor_count = 12
INIT +0x04F1  descriptor[12]           // 7 bytes each
```

The renderer-relevant descriptor layout shared by the class-1 and class-3 tables is:

```text
+0x00 uint16 local_shape_base
+0x02 uint16 spatial_extent_or_fixed_point_field
+0x04 uint8  shape_stride
+0x05 uint8  variant_count
+0x06 uint8  scene_influence_step
```

Evidence:

- class-1 emitter `2BC0A..2BC53` and class-3 emitter `2C055..2C09D` compute `local_shape_base + live_variant * shape_stride`;
- both pass `shape_stride` and their source class into `2B2F4` for view/state-dependent subframe selection plus bank-base translation;
- update paths `1BF05..1BF36` and `1C07D..1C0A3` increment the live three-bit selector modulo byte `+0x05`, directly proving `variant_count`;
- scene emitters pass byte `+0x06` to common spatial influence routine `2B088` when nonzero;
- class-1 collision/selection code consumes `+0x02` as a symmetric spatial extent when greater than one. The exact historical source name remains unresolved.

Corpus closure is exact:

```text
class 1 max(local_shape_base + shape_stride * variant_count) = 1147
class 3 max(local_shape_base + shape_stride * variant_count) =   76
```

Those values are exactly the two RES3 bank sizes derived independently from the fixed header.

## RES interaction/audio fields

```text
INIT +0x1C = 32  -> global 0x258C8
INIT +0x1E = 103 -> global 0x258CC
INIT +0x20 = 48  -> global 0x25614
INIT +0x22 = 414 -> global 0x7FCC
```

### `+0x1E`: archive-9 interaction resource base

Wordcode drawing paths use `0x258CC` as the base for typed archive-9 resources. The supplied value is 103, so the interaction resource region begins at `RES9:103`.

### `+0x22`: archive-9 wordcode displacement

Raw wordcode loader `13F70` seeks archive-9 entry:

```text
0x258CC + 0x7FCC + program_id
```

With the supplied INIT values:

```text
103 + 414 = 517
```

which exactly equals the first non-EH wordcode entry `RES9:517`. Thus `+0x22` is safely the displacement/count from the interaction resource base to the program bank in the supplied data.

### `+0x1C`: sound-effect base

Wordcode opcode `0x1C` adds its encoded sound ID to global `0x258C8` before direct playback through `27F24`. The supplied base is 32.

### `+0x20`: second sound/index base

A dialog/audio path adds program global `0x7FF8` and a local sound ID to `0x25614` before direct playback. The supplied base is 48. A stronger historical name is deferred until all consumers are reconciled.

## Remaining INIT tail

After the 12 class-3 descriptors, parsing has reached file offset `0x0545`. CODE.2 immediately continues into additional variable-length structures through `16990`; those later structures are not yet fully reconstructed and must remain preserved verbatim.

The fixed-header words not assigned above are still executable-proved initialization values, but they should not receive semantic resource-family names merely from numeric coincidence.

## Safe implementation boundary

A faithful INIT reader may now expose:

- the fixed 18-word header without normalizing unknown words;
- the four RES3 source-bank starts and their exact class partition;
- the 175 class-1 and 12 class-3 seven-byte descriptor tables;
- descriptor shape base/stride/variant-count/influence fields, while retaining the `+0x02` word under a conservative physical label;
- the proved interaction/audio bases at `+0x1C..+0x22`.

Do not convert bank-local shape bases stored in INIT, NPCS.DAT, or OBJECTS.DAT to absolute RES3 IDs on disk. The source-class addition is a runtime translation performed by the original engine.
