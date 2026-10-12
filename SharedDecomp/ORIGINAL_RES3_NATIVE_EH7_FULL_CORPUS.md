# Complete original RES3 ↔ compiled native EH7 per-shape differential

**Original file evidence now locally supplied:** \`RES+OTHER.zip\` has the
authentic 5,228,638-byte \`RES3\` with SHA256
\`44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36\`.
Independent Git binary blob SHA-1 matches
\`DnD-Classics/MASTER\` \`8bcae74169185139b08138a16814558879f6370d\`.
The accompanying \`INIT\` is 1,879 bytes, SHA256
\`cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276\`.

## Executed original corpus: actual C++ and independent Python

The NativeEngine C++ source \`5bffd3a3\` compiled cleanly with **20/20
CTests** and its \`rloft_native_res3_scene_audit\` program was run locally
against all authentic original INIT, RES3, NPCS.DAT, OBJECTS.DAT and
LEVELS.DAT. All **4,234 of 4,234** original EH7 entries passed strict
decompression, geometry/pointer/descriptor checks. Unlike previous source
work, this is **not a synthetic RES3 fixture or inferred archive count**.

A separate Python implementation of original CODE.2 LZSS and compiled
EH7 list validation, \`Resources/RES/analyze_res.py\` and
\`analyze_type7.py\`, independently decoded *all 4,234 entries* and
confirmed the exact same archive metrics:

| Original RES3 result | Independent Python | Compiled native |
|---|---:|---:|
| Shapes parsed | 4,234 | 4,234 |
| Source-width 64 | 4,158 | 4,158 |
| Source-width 96 | 40 | 40 |
| Source-width 128 | 36 | 36 |
| Unique column lists referenced | 123,294 | 123,294 |
| Raster selector descriptors counting column references | 278,085 | 278,085 |
| Source-class shape banks | 2169/1147/842/76 | 2169/1147/842/76 |

The independent original-source Python additionally measured
**7,600,070 decompressed EH7 body bytes** and **274,537 raster
descriptors when each unique pointer list is counted once** rather than
recounting duplicated column references.

## New permanent per-entry attestation gate

\`SharedDecomp/verify_native_res3_type7.py\` uses the canonical
independent original parser, not NativeEngine code or hashes synthesized
from native output. The native EH7 corpus report now emits **two SHA256
values for every original RES3 entry**, the exact original compressed
entry and its decoded EH7 body, plus the decoded length, source width,
source columns, unique referenced column-list count, descriptor count
including column references, and descriptor count over unique lists.

The validator separately parses original bytes and compares all
**4,234 × 2 = 8,468 source/body SHA256 values** plus **4,234 × 7
remaining typed fields**; it reports the earliest mismatch by entry
index and field. It also compares aggregate byte counts, resource class
bank lengths, widths and original INIT/RES3 SHA256 identities. No
source geometry/pixel buffers are emitted or checked into CI. Only
original-free synthetic fixtures run in GitHub Actions; the actual
licensed source files remain local.

The v1 schema \`rloft-native-res3-type7-shapes/v1\` was extended
additively with \`total_decoded_body_bytes\` and \`shapes[]\`, without
changing existing typed counters.

### Reproduce with privately held originals

\`\`\`sh
rloft_native_res3_scene_audit /private/original/install \
  > /private/native-res3-corpus.json
python3 SharedDecomp/verify_native_res3_type7.py \
  --res3 /private/RES3 --init /private/INIT \
  --native-report /private/native-res3-corpus.json \
  --report build/research/original-res3-native-diff.json
\`\`\`

**Boundary:** A complete original EH7 **source format** decode, even
with exact source/body byte hashes, is NOT visual parity. The
raster descriptors still point to unexecuted original SINTAB x86
scale-specific code. Original screen projection, 3D object scale,
palette, EH6 texels, 16.16 camera/raycast traversal, animation,
actor AI, event VM, original save compatibility and Windows playable
engine acceptance remain open. No Windows release workflow was
triggered and no DemoV1 gameplay was linked into NativeEngine.
