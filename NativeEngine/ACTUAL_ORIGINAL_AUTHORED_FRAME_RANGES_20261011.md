# 6,204 original authored scene objects → bounded RES3 frame allocations

**Date:** 2026-10-11 Eastern / 2026-10-12 CI UTC.
**Scope:** CODE.2 original scene source/type/variant/resource geometry staging.
This checkpoint **does not attest the live DOS frame selector, animation,
depth ordering, palette, original camera or full-frame graphical parity**.

## Actual compiled source and original-media acceptance

- NativeEngine source: **\`88653be0729ff9d4651a2a50d6f824e626be422a\`**
- [Source-only native Linux Actions #38196555130](https://github.com/Hyperbog/RLOFT/actions/runs/38196555130): **23/23 CTests PASS** (new \`native_scene_frame_ranges_tests\` plus the already-proved world, EH7, SINTAB and CODE.2 VGA tests).
- The exact GitHub Actions artifact's \`rloft_native_scene_frame_ranges_audit\` was downloaded and executed **locally** on authentic user-owned original \`LEVELS.DAT\`, \`INIT\`, \`NPCS.DAT\`, \`OBJECTS.DAT\`, \`RES3\`. All five source SHA-256 digests matched the independently pinned retail identities. **None of these copyrighted original files was committed to the repository or uploaded to CI.**
- A separate Python implementation reading the original file bytes (not the native output) independently reconstructed every family and definition allocation, signed type normalization, all per-level fingerprints, and decoded the distinct referenced EH7 resources. It agreed on **32/32 original per-level frame-range hashes, the aggregate frame-range digest, all allocation counts, and source widths, with zero differences**.
- Permanent source reference: \`SharedDecomp/verify_native_scene_frame_ranges.py\` and 10 synthetic original-free negative tests. This verifier requires the original retail hashes in production runs.

## Source-backed findings

The prior stage proved all 6,204 authored compact source records and all 4,234 original RES3 EH7 entries. This milestone advances their **join** from base shape to **strictly owned candidate source-frame ranges**:

| Real original source family | Authored records | Selection envelope |
|---|---:|---|
| Family A, NPCS.DAT | **1,258** | Original base frame ONLY (live NPC animation still unresolved); additionally retain original NPCS definition bytes +0x18..+0x1A |
| Family B, INIT | **4,324** | Original \`base + live_variant * shape_stride + view_subframe\`; validates \`variant < variant_count\` and \`view_subframe < stride\` |
| Family C, OBJECTS.DAT | **622** | Original base + explicit \`subframe < stride\`; zero-stride definitions remain base-only; do not guess additional live-selector spans |

The original INIT class-one 7-byte descriptor records have source strides distributed as **8×88, 16×38, 1×35, 2×11, 4×3** with variant-counts **1×156, 3×12, 4×5, 2×1, 5×1**. INIT class-three transient/effect descriptors exist and are source-decoded but **are not minted as authored LEVELS records**. Live events or object constructors must supply transient records later.

The original NPCS.DAT 83 definitions now retain all three byte fields directly from original record \`+0x18..+0x1A\`, corresponding to source animation stride, cycle divisor, and animation span. Unlike an unsupported \`NPC base + (frame*stride)\` shortcut, a base-only envelope is used here until the live CODE.2 NPC animation/relative-view selection is proven.

The original OBJECTS.DAT 336 definition strides include **278 stride-8**, **26 stride-0**, **17 stride-2**, **15 stride-1** definitions. The legal envelope for a placed item is the single original definition-owned stride starting at its translated absolute class-two RES3 base. Even if code later proves multiple live states, **the bridge never allows a speculative \`state*stride\` to cross source-class or source-subrange ownership**.

All authored records retain archived type ID and CODE.2-translated live type ID separately, including the one original family-C \`0x54 → 0x58\` compatibility alias.

### Exhaustive authentic source-resolved scene frame catalog

The actual compiled C++ artifact walked every authored record and every supported source-owned subrange and called \`select_native_authored_res3_frame\` on each candidate. Each reached real RES3 entry was decoded by the native EH7 decoder, once per distinct resource.

| Measured from authentic original media | Result |
|---|---:|
| Original authored instances | **6,204** |
| Source-legal shape-slot references validated | **42,429** |
| Distinct referenced RES3 EH7 entries structurally decoded | **1,723** |
| Distinct width-64 original shapes | **1,721** |
| Distinct width-96 original shapes | **1** |
| Distinct width-128 original shapes | **1** |
| Missing/out-of-range/invalid referenced resources | **0** |

The independently calculated per-level hashes match **all 32/32**, as does the combined source-domain SHA256:

\`\`\`text
RLOFT-native-authored-frame-ranges-v1
frame_ranges_sha256:
a16781d0daeb1ab3aaa1a9f1b8a9a479fc5f5a52ad27ad5311c07a07d23bcf42
\`\`\`

The original source-instance linkage hash remains
\`aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c\`.
This is not an inferred resource mapping: original \`INIT\`,
\`NPCS.DAT\`, \`OBJECTS.DAT\`, \`LEVELS.DAT\` and \`RES3\` are all independently ingested
by the native C++ and Python implementations.

## Permanent code and verification boundaries

Native code:
- \`native_scene_frame_ranges.hpp/.cpp\`: immutable authored ranges,
  explicit caller-supplied variant/subframe safe selector, domain-separated
  original-derived digest and per-level 32 fingerprints.
- \`native_type7_scene_shape.hpp/.cpp\`: now retains the actual three NPC
  animation metadata bytes from each 0x45 source definition.
- \`native_scene_frame_ranges_audit_main.cpp\`: actual-original
  whole-corpus check including **every** candidate RES3 resource's EH7 shape
  integrity; metadata-only JSON output.
- Native CTest covers 32 synthetic original levels, all three class
  owners, zero-stride items, original-to-runtime type alias, legal
  variant/subframes, illegal bank crossings, NPC metadata mutation,
  per-level hash divergence and unsupported viewpoint operations.

Research code:
- \`SharedDecomp/verify_native_scene_frame_ranges.py\` independently
  parses the original five-file data source, recomputes the record
  series and 32 digest domains, decodes every distinct referenced EH7
  image and compares to the native source audit.
- \`SharedDecomp/test_verify_native_scene_frame_ranges.py\`: original-free
  corrupted schema, false fidelity flags, changed original NPC
  parameters, OBJ stride, INIT variant allocation, missing fields,
  per-level mismatch and CLI validation cases.

## Open rendering/gameplay frontier

This is a meaningful **join from original world objects to real RES3
shape resource allocations**, not a frame compositor. The original
\`2B2F4\` view correction, \`2B6D0..2C22E\` actor/fixture/effect live
state, \`2C294..2C6B6\` screen/depth ordering, and \`36CEC\` actual
projected size/scale and side selection must still be recovered and
integrated. The currently tested CODE.2 36B14/369D8 VGA dispatcher
already supports safe plane masks and EH7 list writes; this new
allocation API will supply its RES3 input only **after** source-proven
live variant/pose/view state is available.

EH6 texture sampling, COLORS/VGA palette, camera, actor AI/events, original
SAVE/LEVnnn compatibility, full DOS-frame pixel comparison, and a
current-source Windows playable game remain **unverified**.
The independent NativeEngine must not link DemoV1 gameplay or assume
the old Level22 diagnostic host raycast is the authentic DOS render
algorithm. Owner-controlled Windows release workflow remains manual-only.
