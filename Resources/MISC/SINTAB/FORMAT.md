# SINTAB renderer table and scale-microcode package

Status: **the supplied `SINTAB` file is structurally reconstructed as the external trigonometric/projection and scale-rasterizer package loaded by CODE.2 renderer initialization. The complete byte-consumption grammar is reproduced, all 127 scale blocks validate, their stored payload-size sum closes exactly, and the generated code/control-table layout is independently consumed by the type-7 scene-shape renderer. Some fixed-prefix tables retain structural rather than source-level semantic names.**

## Controlling evidence

- supplied file: `SINTAB`
- bytes: `333,757` (`0x517BD`)
- SHA-256: `19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87`
- CODE.2 loader: logical `3838..3A63`
- compiled scene-shape consumers: `369D8`, `36B9C..36CBC`, `36CEC..372F3`
- runtime scale-code pointer table: DGROUP `0x18FE0`

The immutable supplied file and certified CODE.2 object are controlling evidence.

The executable also contains the literal filename `SINTAB`. The decisive format identification is the loader's exact sequential read grammar: the byte counts, stored values, dynamic table length, 127 length-prefixed blocks, and aggregate payload size all match the supplied `SINTAB` byte-for-byte. No filename-pointer relocation assumption is required for that structural proof.

## Exact physical layout

The supplied file parses as:

```text
+0x0000  uint32 sine_q16_signmag[450]         // 0x708 bytes
+0x0708  uint32 tangent_q16[900]              // 0xE10 bytes
+0x1518  uint16 projection_table_A[128]       // 0x100 bytes
+0x1618  uint16 projection_table_B[128]       // 0x100 bytes
+0x1718  uint32 renderer_constant[8]           // 0x020 bytes
+0x1738  int32  view_angle_table[256]          // 0x400 bytes
+0x1B38  uint32 scale_block_count              // 127
+0x1B3C  uint32 runtime_lookup_max             // 508
+0x1B40  uint32 renderer_aux_value             // 128
+0x1B44  uint16 runtime_lookup[509]             // (508 + 1) * 2 = 0x3FA
+0x1F3E  uint32 scale_block_payload_bytes      // 324,989 / 0x4F57D
+0x1F42  scale_block[127]
+0x515BD byte   zero_trailer[512]
+0x517BD EOF
```

The fixed offsets above are not inferred from convenient factoring: CODE.2 `3838..39C0` performs the corresponding reads in that exact order and computes the `runtime_lookup` byte length as `(runtime_lookup_max + 1) * 2`.

The loader then allocates exactly `scale_block_payload_bytes` bytes and reads `scale_block_count` length-prefixed blocks into that arena.

## First table: 450-entry signed-magnitude Q16 sine

The first `0x708` bytes are 450 little-endian dwords. Their low 31 bits reproduce an approximately Q16-magnitude sine table for integer degrees `0..449`:

```text
index   stored
0       0x00000000
30      0x00007FFF
45      0x0000B504
90      0x0000FFFF
180     0x80000000
225     0x8000B504
270     0x8000FFFF
360     0x80000000
449     0x0000FFF6
```

The high bit is a sign flag rather than ordinary two's-complement sign extension:

```text
indices   sign bit
0..179    0
180..360  1
361..449  0
```

Thus `0x80000000` is a negative-zero style sign/magnitude representation at 180 and 360 degrees. The magnitude agrees with `abs(sin(degrees)) * 65536`, clamped to 65535, to within one unit throughout the supplied table; these one-unit differences are retained exactly and are not normalized by tooling.

The safe physical name is `sine_q16_signmag`. The original DreamForge typedef/name is unknown.

## Second table: 900-entry Q16 tangent family

The next `0xE10` bytes are 900 unsigned dwords. They reproduce a Q16 tangent progression with two adjacent ranges sharing the 45-degree boundary:

```text
entries   represented range
0..449    approximately tan(0.1 .. 45.0 degrees) * 65536
450..899  approximately tan(45.0 .. 89.9 degrees) * 65536
```

Examples:

```text
index 0    114
index 449  65535
index 450  65536
index 899  37549325
```

All entries agree with the corresponding mathematical Q16 tangent to within two integer units. The duplicated 45-degree boundary is serialized data and must be preserved.

## Remaining fixed lookup tables

### Two 128-word monotone tables

`projection_table_A` and `projection_table_B` are loaded into two separate runtime allocations. In the supplied file both are monotonically increasing:

```text
A: 309 .. 6831
B: 293 .. 6489
```

They clearly participate in the same projection/renderer initialization subsystem, but the exact source-level distinction between A and B is not yet instruction-proven. They remain structurally named.

### Eight renderer constants

The eight serialized dwords are:

```text
256, 256, 127, 328, 328, 22272, 175, 179200
```

CODE.2 reads them individually into separate renderer globals. Their encoded values are preserved as independent fields rather than collapsed into a guessed geometry structure.

### 256-entry signed view-angle table

The following `0x400` bytes are 256 signed dwords. The supplied sequence descends smoothly from `359` through `0` at index 128 to `-359` at index 255. Its use belongs to the first-person projection subsystem, but the unit/source name is not asserted beyond `view_angle_table` until all direct consumers are closed.

### 509-word runtime lookup

`runtime_lookup_max` is `508`, so CODE.2 allocates and reads exactly 509 little-endian words. The supplied table uses 17 distinct values, ranges `0..2313`, and ends in a long zero region. `36CEC` indexes this table during projected scene-shape setup and clamps one resulting value against `0x808`.

The serialized name remains `runtime_lookup`; no stronger editor-facing interpretation is assigned yet.

## Scale block family

The file stores:

```text
scale_block_count = 127
```

Each block is physically:

```text
uint16 block_size
byte   block[block_size]
```

The 127 supplied `block_size` values range from 581 to 3025 bytes. Their sum is exactly:

```text
324,989 == stored scale_block_payload_bytes
```

so the aggregate allocation/read size at CODE.2 `39B3..3A48` is completely accounted for.

### Internal block layout

Every supplied block has the same prefix grammar:

```text
block +0x000  uint16 code_patch_offset[129]   // 0x102 bytes
block +0x102  uint16 scale_step[129]          // 0x102 bytes
block +0x204  byte   rasterizer_x86[]
```

Consequently:

```text
block_size >= 0x204
rasterizer_code_bytes = block_size - 0x204
```

All 127 generated code bodies end in x86 opcode `0xC3` (`RET`). Representative bodies disassemble as straight-line 32-bit x86 code that reads source texels through `ESI`, performs the renderer's color/adjustment arithmetic, and writes scaled pixels into VGA-oriented destination positions through `EDI`.

This is executable code stored in the data file, not compressed image data.

## `code_patch_offset[129]`

For every supplied block:

- all 129 words are monotonically non-decreasing;
- every value is inside the same block;
- every value is `>= 0x204`, therefore points into `rasterizer_x86`;
- first value is always `0x204`;
- the last value reaches the final generated code region.

CODE.2 helpers `36C10`, `36C44`, `36C90`, and `36CBC` receive the runtime code pointer, subtract `0x204` to recover this prefix, and use `code_patch_offset` to modify/restore generated instructions. Encoded operations include temporarily writing `0xC3` (`RET`) and short-jump opcode `0xEB` into selected code locations.

Thus the first table is safely identified as a **generated-code patch/entry offset table**.

## `scale_step[129]`

CODE.2 helpers `36B9C` and `36BD8` likewise subtract `0x204` from the code pointer, add `0x102`, then accumulate words from this second table when converting projected spans into micro-rasterizer limits.

The supplied table values are small nonnegative integers. Their sums increase strictly across all 127 blocks:

```text
block 0    sum =   4
block 1    sum =   8
...
block 30   sum = 124
block 31   sum = 129
...
block 126  sum = 511
```

This is therefore a scale/resampling step distribution table tied directly to each generated rasterizer. The project name `scale_step` is behavioral; the original source identifier is unknown.

## Runtime pointer transformation

The loader reads each complete block into one allocated arena. It then stores **`block + 0x204`**, not the raw block start, into the scale-code pointer family at `0x18FE0`. Entry zero is filled from the first generated-code pointer after loading, so the runtime table provides a valid low-end/default alias.

This explains the subtract-`0x204` behavior in the patch/span helpers: ordinary rendering operates from the executable-code address while helpers recover the two serialized prefix tables when needed.

`36CEC` selects from this pointer family according to projected size and the type-7 compiled scene-shape renderer eventually invokes/patches the generated code through `369D8` and neighboring helpers.

This closes the previously unresolved external micro-rasterizer component documented in `Resources/RES/TYPE7_RENDERER.md`.

## 512-byte zero trailer

After the 127th scale block, parsing reaches absolute offset:

```text
0x515BD / 333245
```

The physical file continues for exactly 512 additional bytes to `0x517BD`; every trailer byte is zero.

CODE.2 `3838..3A63` closes the stream immediately after the final length-prefixed block and does not read this trailer. It is therefore serialized trailing padding/reserved data in this build. A faithful round-trip must preserve it even though the current loader ignores it.

## Reproducible validation

`analyze_sintab.py` validates:

- immutable size/SHA;
- every fixed-prefix boundary;
- 450 sine and 900 tangent entries;
- the serialized count/max/aux fields;
- the exact 509-word runtime lookup length;
- all 127 scale-block lengths;
- equality of summed block sizes and serialized `scale_block_payload_bytes`;
- the two 129-word prefixes in every block;
- patch offsets monotonic and contained inside generated code;
- scale-step accumulation;
- all 127 code bodies terminating in `RET`;
- exact 512-byte all-zero trailer and final EOF.

## Safe implementation boundary

A preservation-quality port should not replace `SINTAB` wholesale with modern floating-point trigonometry merely because some prefix tables are mathematically recognizable. The file also contains renderer-specific projection data, lookup tables, and executable rasterizer code whose exact integer behavior is part of the original engine.

A faithful implementation may translate the generated micro-rasterizer semantics into safe portable code, but should retain the original tables/code as canonical evidence and validate the translated renderer against their encoded behavior.
