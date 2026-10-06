# RES3 type-7 source-bank partition

Status: **the complete 4,234-entry `RES3` archive is now partitioned exactly into four first-person scene-source banks. The four bank starts come directly from `INIT +0x06..+0x0C`; CODE.2 adds the source-class-selected start to each scene record before archive-3/type-7 lookup.**

The immutable `INIT`, `RES3`, `NPCS.DAT`, `OBJECTS.DAT`, and original CODE.2 bytes are controlling evidence.

## Exact RES3 partition

CODE.2 `16D04..17049` reads the first 0x24 bytes of `INIT` as eighteen little-endian words. Four consecutive words become the source-bank base table at DGROUP `0x27100`:

```text
INIT +0x06 =    0 -> 0x27100 -> source class 0
INIT +0x08 = 2169 -> 0x27102 -> source class 1
INIT +0x0A = 3316 -> 0x27104 -> source class 2
INIT +0x0C = 4158 -> 0x27106 -> source class 3
```

`2B2F4` finishes scene-record shape selection at `2B6B5..2B6C4` by indexing this four-word table with the scene source class and adding the selected base to scene-record word `+0x04`.

`RES3` contains exactly 4,234 entries, so the four starts partition it without gap or overlap:

```text
source class 0     RES3 0..2168       2169 entries
source class 1     RES3 2169..3315    1147 entries
source class 2     RES3 3316..4157     842 entries
source class 3     RES3 4158..4233      76 entries
                                         ----
                                         4234
```

This equality is exact: the final bank ends at `RES3` EOF.

## Runtime translation

The common scene-building rule is:

```text
absolute_RES3_type7_id = source_bank_base[source_class]
                       + bank_local_shape_id
```

The resulting scene-record word `+0x04` is later consumed by `21BF0` and `36CEC`. Both pass it to resource accessor `2F29C` with archive number **3**, proving that the translated value is an absolute `RES3` entry ID.

This translation explains why unrelated static databases can reuse local shape index zero without collision.

## Source class 0: NPCS.DAT family A

Family-A/NPC scene emission `2B6D0..2BB9E` copies `NPCS.DAT +0x14` into scene-record `+0x04`, applies NPC animation/state offsets, and calls `2B2F4` with source class `0`.

Because class-0 base is zero, the existing numeric NPC shape IDs are also their absolute `RES3` IDs in this build:

```text
absolute = 0 + npc_local_shape
```

The important structural correction is conceptual: `NPCS.DAT +0x14` belongs to the class-0 bank and becomes an archive-3 ID through the same bank-translation mechanism as every other scene source. The supplied value zero for the class-0 bank makes the translation numerically invisible.

All 83 supplied NPC local bases lie inside `0..2168`; the largest is 2149.

See `TYPE7_NPC_INDEX.md` for the NPC-definition ledger.

## Source class 1: INIT family-B scene descriptors

Family-B scene emission `2BBA0..2BDF0` passes source class `1` to `2B2F4`. Its type ID indexes a 7-byte descriptor table loaded from the `INIT` tail into runtime pointer `0x255DC`.

After reading the fixed 0x24-byte INIT header, CODE.2 `1686C` reads:

```text
INIT +0x0024  uint16 descriptor_count = 175
INIT +0x0026  descriptor[175]          // 7 bytes each
```

The renderer-relevant descriptor layout is:

```text
+0x00 uint16 local_shape_base
+0x02 uint16 spatial_extent_or_fixed_point_field
+0x04 uint8  shape_stride
+0x05 uint8  variant_count
+0x06 uint8  scene_influence_step
```

Executable evidence:

- `2BC0A..2BC53` computes the initial bank-local shape value as `local_shape_base + live_variant * shape_stride`;
- `2BD5C..2BD84` passes `shape_stride` and source class `1` to `2B2F4` for view/state-dependent subframe selection and bank-base translation;
- `1BF05..1BF36` increments the live three-bit variant selector modulo descriptor byte `+0x05`, proving `variant_count`;
- `2BDA4..2BDC8` passes byte `+0x06` to the common scene-cell influence routine `2B088`;
- collision/selection paths including `2CCDA..2CD37` use the word at `+0x02` as a symmetric spatial extent when its value is greater than one. Its exact historical/editor name remains conservative.

The allocation expression

```text
local_shape_base + shape_stride * variant_count
```

never exceeds the 1,147-entry class-1 bank. The highest supplied descriptor ends at **1147 exactly**, independently matching the next INIT bank boundary at absolute `RES3:3316`.

The descriptor count 175 also matches the supplied family-B type-ID domain `0..174`.

## Source class 2: OBJECTS.DAT family C

Family-C loose/pickable-item scene emission `2BDF4..2BFDC` indexes `OBJECTS.DAT` and constructs its bank-local shape value from:

```text
OBJECTS.DAT +0x28  local shape base
OBJECTS.DAT +0x2A  shape stride / subframe span
```

The code at `2BE70..2BEA8` forms `base + live_three_bit_selector * stride`, then `2BF27..2BF4F` passes stride and source class `2` to `2B2F4`. The common helper may add a further view/state-dependent subframe before adding class-2 base `3316`.

Thus the safe physical names are:

```text
+0x28 uint16 scene_shape_base_local
+0x2A uint8  scene_shape_stride
```

rather than treating `+0x2A` as a universally simple heading field. The live selector is part of the common 22-byte world-record state and can have family-specific meaning.

Across all 336 supplied OBJECTS definitions:

- every local base lies inside the 842-entry class-2 bank;
- every `base + stride` lies at or below 842;
- the maximum is **834 + 8 = 842 exactly**.

Therefore the shipped OBJECTS shape namespace closes exactly at the class-2 bank boundary:

```text
absolute RES3 object shape = 3316 + bank-local class-2 shape
```

Repeated OBJECTS definitions deliberately reuse local bases; this is visual-resource sharing, not a parser error.

## Source class 3: transient/moving-effect scene family

Immediately after the 175 class-1 descriptors, `16934` reads a second count and 7-byte table from the same open INIT file:

```text
INIT +0x04EF  uint16 descriptor_count = 12
INIT +0x04F1  descriptor[12]           // 7 bytes each
```

The table is loaded to runtime pointer `0x22234`. Scene emitter `2BFEC..2C22E` indexes it and passes source class `3` to `2B2F4`.

Its renderer-relevant 7-byte layout is the same as class 1:

```text
+0x00 uint16 local_shape_base
+0x02 uint16 spatial_extent_or_fixed_point_field
+0x04 uint8  shape_stride
+0x05 uint8  variant_count
+0x06 uint8  scene_influence_step
```

`1BFA2..1C0A7` independently cycles the live three-bit selector modulo byte `+0x05`, proving the same `variant_count` role. `2C1CB..2C206` routes `+0x06` through `2B088`.

All 12 descriptor allocations fit the 76-entry class-3 bank. The last supplied allocation ends at **76 exactly**, matching `RES3` EOF:

```text
absolute = 4158 + local_shape
```

The higher-level identity is now executable-backed. Scene emitter `2BFEC..2C22E` walks the same 0x16-byte live-world-object family used by the transient/moving-effect constructor `08A80`; both use record word `+0x0E` as the effect/type value, and the emitter uses that value to index this 12-entry INIT class-3 descriptor table. LEVELS event opcode 3 independently calls `08A80` to create these records. The safe project classification is therefore **transient/moving-effect scene family**; the historical DreamForge source identifier remains unknown.

## Why `shape_stride` is safer than `heading_stride`

The common renderer extracts a three-bit live-record selector, multiplies it by the source definition's stride, then `2B2F4` can add another view/state-dependent subframe within the same stride. For class 1 and class 3, a separate descriptor byte explicitly bounds the three-bit selector as `variant_count`.

Therefore the stride is certainly a **shape-subrange stride**. Calling it a universal heading stride conflates two distinct stages:

1. selecting a state/animation/orientation subrange;
2. selecting a view-dependent shape within that subrange.

The original source/editor terminology is not known, so project documentation should prefer `scene_shape_stride` / `shape_stride` unless a narrower family-specific behavior is proven.

## Reproducible validation

`analyze_type7_banks.py` verifies against the supplied binaries that:

1. `RES3` has 4,234 entries;
2. the four INIT starts are monotonic and partition all 4,234 entries exactly;
3. INIT contains 175 class-1 and 12 class-3 seven-byte descriptors at the executable-consumed offsets;
4. every class-1/class-3 `base + stride * variant_count` stays inside its bank and the maximal allocation closes the bank exactly;
5. every OBJECTS class-2 base/span fits its 842-entry bank and the maximal `base + stride` closes it exactly;
6. every NPCS class-0 local base lies inside the 2,169-entry bank.

## Safe implementation boundary

A faithful port/editor should preserve both local and absolute identities:

```text
source_class
bank_local_shape_id
absolute_RES3_id = bank_base[source_class] + bank_local_shape_id
```

Do not flatten static `NPCS.DAT`, `OBJECTS.DAT`, or INIT descriptor bases into absolute IDs on disk. The original engine deliberately stores bank-local IDs and performs the source-class translation at scene-build time.
