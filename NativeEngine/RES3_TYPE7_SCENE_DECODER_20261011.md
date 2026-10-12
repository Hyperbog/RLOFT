# Native RES3/EH7 first-person scene-shape decoding and original INIT bank mapping

## Architectural milestone

This implementation moves the native renderer's **real original source inputs**
beyond abstract cell codes and host DDA debug strips. It adds an **independent
C++17 original EH7 scene-shape display-list decoder** and the four original
\`INIT\` scene-source banks. It never executes the original 16-bit x86
rasterizer nor assumes EH7 is a bitmap.

The source evidence is documented on \`code1-decompilation\` in:
- \`Resources/RES/TYPE7_RENDERER.md\`: original CODE.2 routines
  \`21C1D..21C55\`, \`36CEC\`, \`369D8\` prove header geometry,
  relative column pointers, selector triples and temporary patching of
  scale-specific SINTAB microcode;
- \`Resources/RES/TYPE7_SOURCE_BANKS.md\`: original INIT +0x06..+0x0C
  source-bank starts, CODE.2 \`2B2F4\` source-class translation,
  class-1/class-3 descriptors and 7-byte field roles.

## Exact original EH7 representation

Original archive entry layout:

\`\`\`text
+0x00  "EH"
+0x02  uint8 type = 7
+0x03  uint16 header_span = 0
+0x05  uint16 decoded_body_size
+0x07  LZSS-4KiB compressed body (consumed exactly)

decoded body:
+0x00  uint16 original_source_width
+0x02  int16  x_min
+0x04  int16  x_max
+0x06  int16  y_min
+0x08  int16  y_max
+0x0A  uint16 column_offset[x_max-x_min+1]  (relative to decoded body)

at each column_offset:
  repeat:
    uint16 stop_selector
    if stop_selector == 0: terminate this list
    int16  signed_adjustment
    uint16 entry_selector
\`\`\`

Original data has widths 64, 96 and 128. Original compiled renderer
uses stop/entry selectors as even byte offsets into a selected SINTAB
rasterizer table. These ARE NOT pixels and no frame rendering is
certified merely by decoding their numeric values.

\`decode_native_type7_scene_shape\` uses the same independent,
already-tested native LZSS implementation as EH6, with consumed-stream
verification, geometry/range checks, bounded pointer-table validation,
unaligned list pointers, duplicate/sharing list pointers,
zero-termination enforcement, selector parity/range checks and safe
failure on a truncated or malformed descriptor. It returns the complete
per-source-column descriptor vector and unique-list totals without
materializing original x86 code as executable memory.

## Four original RES3 source-class namespaces

The original INIT file defines:

| class | owner | RES3 IDs | count |
|---|---|---:|---:|
| 0 | NPCS.DAT / family A | 0–2168 | 2169 |
| 1 | INIT / family B descriptor table | 2169–3315 | 1147 |
| 2 | OBJECTS.DAT / family C definitions | 3316–4157 | 842 |
| 3 | INIT / transient/moving effects | 4158–4233 | 76 |
| **Total** | | | **4234** |

\`decode_native_res3_scene_banks\` reads the four bases from original
INIT +0x06..+0x0C; verifies monotonic, in-range partition; retains
both original 7-byte descriptor tables at offsets +0x24 (175 records)
and +0x4EF (12 records); checks variant_count and the exact
class-1/class-3 allocation-end closure. Source-class-to-absolute-RES3
translation is explicitly bounds-checked; source-class 1 local ID 0 is
not confused with class 0 ID 0.

\`audit_native_res3_scene_corpus\` strictly validates every entry from
the \`NativeResArchive\` source object, counts original widths,
descriptors, reused pointer lists and per-bank shape counts. The
\`rloft_native_res3_scene_audit\` console diagnostic consumes a
**user-owned original install folder / ISO** read-only and emits
metadata-only JSON \`rloft-native-res3-type7-shapes/v1\` and input
SHA-256s. No copyrighted source textures/geometry are embedded in
the output or CI binaries.

## Verification boundaries and follow-up work

The native source-only regression fixture constructs EH7 LZSS
literal streams with shared **odd byte-aligned list offsets** and
signed negative adjustment, all four disjoint INIT bank ranges,
175+12 class descriptors and a fake RES3 archive. Negative tests
reject compressed truncation/trailing data, pointer overlap/outside
span, odd selectors, bad signed coordinates, incorrect INIT counts,
invalid bank boundaries and descriptor allocation overruns.

The local user-provided files have \`LEVELS.DAT\`, \`SINTAB\`,
\`NPCS.DAT\`, \`OBJECTS.DAT\` and CODE objects but **not RES3/INIT**.
The user's originals are present in \`DnD-Classics/MASTER\` Git history;
CI must NOT download game media to a source-only build. The actual
4,234-entry RES3 native-corpus executable run remains a distinct
acceptance gate until those original two assets are locally available
through the install/ISO. The exhaustive original-language archive
parser ledger on \`code1-decompilation\` is existing independent
research evidence, not a replacement for actual native C++ original
corpus execution.

**Not implemented here:** interpreting/executing SINTAB rasterizer
microcode; rendering accurate EH7 shapes; original viewpoint/perspective,
camera, lighting, EH6 sampling, palette composition, AI/events or a
playable game. No \`demo-v1\` gameplay is linked. Windows release
Actions stay manual-only.

## Original-definition binding added

The same native source now constructs \`NativeSceneDefinitionDirectory\`
from **actual bytes** of \`NPCS.DAT\` and \`OBJECTS.DAT\`, not from game-
agnostic object identifiers:

- \`NPCS.DAT\`: 83 × 0x45-byte records; local scene shape at record
  +0x14, and source class 0 absolute ID = base + local. Original
  corpus maximum local base is **2149**, under the **2169** class-0 size.
- \`OBJECTS.DAT\`: 336 × 0x41-byte records; source class 2 local shape
  base at +0x28, stride at +0x2A; require
  \`local + stride <= 842\`. Original corpus maximum allocation end is
  exactly **842**, the class-2 bank boundary.
- INIT class 1 and 3: the original 175+12 seven-byte descriptor tables
  retain base/extent/stride/variant count/scene-influence bytes.
  \`init_base_variant\` accepts an **explicit caller-supplied variant**
  and converts only a provably bounded starting subframe into an
  absolute RES3 ID. It cannot guess the current actor's facing, live
  animation phase or renderer-chosen perspective suffix.

The new corpus-audit command reads original INIT, RES3, NPCS.DAT and
OBJECTS.DAT read-only, validates all four sources together, and
emits separate SHA-256 source identities and definition counts. It
does not export scene geometry or game payload bytes.

**Independent original evidence checked:** the exact original
\`DnD-Classics/MASTER\` INIT (Git blob \`aeb2e1575879047d64d6cc961c5ea59043b555a4\`,
1,879 bytes) yields original bases \`[0,2169,3316,4158]\`,
175 class-1 descriptors reaching 1,147, and 12 class-3
descriptors reaching 76 with zero bad allocations. The supplied
local original NPCS.DAT and OBJECTS.DAT have 83/336 records and
maximum local ranges matching the archive study. Original RES3
Git blob \`8bcae74169185139b08138a16814558879f6370d\`
(size 5,228,638 bytes) is **known to be present** under the original
branch, but has not been materialized into this local runtime or
executed through the new decoder. C++ end-to-end RES3 acceptance
remains open.
