# Canonical independent authored scene-frame-range oracle

This verifier is part of the **source-only decompilation/research
branch**, not a replacement game engine. It independently recomputes
NativeEngine original-data-authenticated object → source frame
allocations using **user-owned** original \`LEVELS.DAT\`, \`INIT\`,
\`NPCS.DAT\`, \`OBJECTS.DAT\` and \`RES3\`.

## Source proof

The executable-reconstructed CODE.2 routines establish four
non-overlapping original RES3 source classes:

\`\`\`text
family A NPCS          source class 0  RES3 0..2168
family B INIT fixtures source class 1  RES3 2169..3315
family C OBJECTS       source class 2  RES3 3316..4157
runtime effects INIT   source class 3  RES3 4158..4233
\`\`\`

Original source definitions have independent field layouts:

- \`NPCS.DAT\`: 83 records of 0x45 bytes; base source ID at +0x14;
  original animation stride/cycle divisor/animation span at
  +0x18/+0x19/+0x1A. These bytes are retained but NPC live view/
  animation frame selection is **not** inferred.
- \`INIT\`: 175 family-B seven-byte records with base +0, stride +4,
  variant count +5, plus 12 structurally matching runtime-effect
  descriptors. Frame selection \`base + variant * stride + subframe\`
  requires *explicit* caller-provided variant and subframe, each
  strictly bounded to the descriptor's original allocation.
- \`OBJECTS.DAT\`: 336 records of 0x41 bytes, shape base +0x28 and
  stride +0x2A; only the single source-owned stride is considered.
  A zero-stride definition retains its original base-only shape.
  Do **not** infer an unsupported \`8 * stride\` animation frame set.
- Original compact family C type \`0x54\` resolves to runtime type
  \`0x58\` without changing the archive byte identity.

All of these source rules are drawn from the original instructions
documented in \`Resources/RES/TYPE7_SOURCE_BANKS.md\`, not inferred
from modern sprite formats.

## Actual compiled-original acceptance, not synthetic only

The exact GitHub-built NativeEngine source
\`88653be0729ff9d4651a2a50d6f824e626be422a\`
passed [portable Linux Actions #38196555130](https://github.com/Hyperbog/RLOFT/actions/runs/38196555130)
with **23/23 CTests PASS**, including the independent original-free
\`native_scene_frame_ranges_tests\`.

The C++ \`rloft_native_scene_frame_ranges_audit\` was downloaded and
executed locally on the owner's authentic original five files. It
validated every source-authored legal subrange reference to the real
RES3 archive, structurally decompressing each unique referenced EH7
entry. Independent Python source bytes comparison verified every
source-defined 22-byte canonical frame record and all 32 independent
per-level SHA-256 digests, and separately read/decompressed original
referenced EH7 entry headers.

\`\`\`text
32/32 per-level original-to-native frame-range SHA256 MATCH
source-authored instances                   6,204
source-legal candidate shape references    42,429
unique original referenced EH7 entries      1,723
width-64 / width-96 / width-128            1,721 / 1 / 1
original reference decoding failures        0

RLOFT-native-authored-frame-ranges-v1
a16781d0daeb1ab3aaa1a9f1b8a9a479fc5f5a52ad27ad5311c07a07d23bcf42
\`\`\`

This is a **new** proof beyond the previous 6,204 BASE-ID ledger
(\`aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c\`);
the new digest also covers original variant counts, shape strides and
NPC metadata.

Full source-only unit-test gate at
\`code1-decompilation:bdaaa53e5377a86ea6e6dd580021dfcdd751b26d\`:
[Research Actions #38196899337](https://github.com/Hyperbog/RLOFT/actions/runs/38196899337),
**179/179 Python tests PASS** (39 ResearchInfra + 140 SharedDecomp,
including 10 new frame-range mutation/fidelity tests). Initial
fixture/report-limit test failures were corrected without suppressing
tests or changing source validation policy.

## Reproduce privately

\`\`\`sh
rloft_native_scene_frame_ranges_audit /private/original/RLOFT \
  > /private/native-frame-range-report.json
python3 SharedDecomp/verify_native_scene_frame_ranges.py \
  --game-dir /private/original/RLOFT \
  --native-report /private/native-frame-range-report.json \
  --out build/research/native-authored-frame-range-comparison.json
\`\`\`

The verifier SHA-pins all five original retail input files, parses all
three 11-byte world families, original definition allocations, validates
all referenced EH7 source bytes via the independent Python archive
decoder, and compares the C++ report's total/32 per-level hashes,
counts, source SHA identities and class shape-width totals. It rejects
duplicate JSON fields, corruption and any claimed original-frame,
actor-animation or depth-order parity.

**Boundary:** This proves original source resource ownership and legal
drawable frame allocations, **not** which animation, perspective frame,
depth-order or shaded pixels the DOS game would display in play. The
original event VM, actor state updates, SINTAB 3D draw context, palette/
texture composition and DOS-frame comparison remain future work.
No copyrighted media enters Git or CI; no Windows release workflow
was dispatched.
