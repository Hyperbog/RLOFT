# Independent original RES3 scene-source instance oracle

The first-person renderer's source instances are now **data-backed** across
five authenticated original files, not guessed sprites. The separate
NativeEngine typed-world scene adapter and this source-only Python oracle
independently resolve all three compact \`LEVELS.DAT\` 11-byte families to
the relevant definition tables and four INIT source-class RES3 banks.

## Exact source and algorithm

\`SharedDecomp/verify_native_scene_instances.py\` validates the original
retail SHA256 identities for \`LEVELS.DAT\`, \`INIT\`, \`NPCS.DAT\` and
\`OBJECTS.DAT\`. It **independently parses** original scene records,
source bank starts +0x06..+0x0C, the 175 family-B descriptors and 12
moving-effect descriptors, NPCS 0x45-definition stride/local shape
+0x14 and OBJECTS 0x41-definition stride/local base +0x28 and
stride +0x2A.

For all 32 levels the oracle encodes, without exporting source records,
the **original compact family data**, original and CODE.2-normalized
runtime definition IDs, source class, ordered record index, signed
16.16 X/Y positions, source cell, full compact byte +0x0A,
NPC low-three-bit facing for family A only, and the **absolute RES3
source base** translated through INIT for that family's definition
namespace. One original family-C compact type 0x54 is migrated to
runtime OBJECTS type 0x58; its original type is still retained.

Two independent SHA-256 domains are used:

\`\`\`text
RLOFT-compact-world-scene-families-v1
RLOFT-native-scene-source-bases-v1
\`\`\`

The first detects mutations to raw authored 11-byte records. The second
checks **every source-class/position/type/base-ID resolution result**,
and additionally emits 32 independent per-level hashes to localize
the first original/native divergence. This is materially stronger than
matching three world-family totals.

## Real-original verified milestone

The compiled source at \`native-engine:5bffd3a3e53a06842d27fbe5c63d574acbc9babd\`
passed [Linux source-only CI #38191784044](https://github.com/Hyperbog/RLOFT/actions/runs/38191784044)
with **20/20 CTests**. Its asset-free
\`rloft_native_scene_instances_audit\` executable was then downloaded
and run locally on the actual four original files, including the authentic
1,879-byte INIT fetched from \`DnD-Classics/MASTER\` and
**independently verified against Git blob hash**
\`aeb2e1575879047d64d6cc961c5ea59043b555a4\`.

A separate Python implementation independently hashed all 6,204
original scene-placement and resolved-source records, confirming:

\`\`\`text
32 levels; 1,258 NPC, 4,324 placed, 622 loose item = 6,204 total
legacy family-C type 0x54->0x58 aliases = 1
32 / 32 per-level resolved-source SHA256 checks MATCH
retained compact scene original SHA256 MATCH
complete resolved-source ledger SHA256 MATCH
original source-base resolution ledger:
aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c
\`\`\`

The authenticity and basis of that result are separate: the
\`original_res3_archive_present=false\` flag in the actual local
compiled report documents that **full original RES3 bytes were not
available locally**. Source ID translation uses the proven 4234-entry
original RES3 bank directory cardinality and is verified against
INIT/NPCS/OBJECTS/LEVELS actual bytes. This DOES NOT establish
that all original EH7 streams successfully decoded in the new native
C++ decoder, nor any original graphics/pixel/gameplay parity.

## Reproduce using licensed owner-originals

\`\`\`sh
rloft_native_scene_instances_audit /private/original/install \
    > /private/native-authored-scene.json

python3 SharedDecomp/verify_native_scene_instances.py \
    --levels /private/LEVELS.DAT --init /private/INIT \
    --npcs /private/NPCS.DAT --objects /private/OBJECTS.DAT \
    --native-report /private/native-authored-scene.json \
    --out build/research/scene-instance-source-resolution.json
\`\`\`

Synthetic original-free unittest regressions run under the canonical
source-only ResearchInfra and SharedDecomp GitHub Actions checks. No
copyrighted original file bytes enter the repository or CI.

**Still unproved:** exact scene-view subframe, SINTAB x86 rasterizer
semantics, EH7 output pixels, original occlusion/order, textures/palette,
player/actor runtime movement, script VM, SAVE/LEVnnn round trip,
current-source Windows executable and completed/playable gameplay.
Original Windows release workflow remains manual-only.
