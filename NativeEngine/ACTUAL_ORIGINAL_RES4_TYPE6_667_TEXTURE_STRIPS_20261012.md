# All 667 original RES4 wall/surface textures — CODE.2 338C8 native 16.16 VGA strip proof

**Status (2026-10-12): native fixed-point column sampler IMPLEMENTED, compiled and independently tested on every original RES4 EH6 texture.** This is a source-owned *rendering primitive*, not a reconstructed camera/raycast, world texture-selector policy, depth compositor, or DOS framebuffer screenshot.

## Compiled implementation

- Exact original-free native source commit: [\`ca50db31029311cf8e97a1f48408576b82ffd30d\`](https://github.com/Hyperbog/RLOFT/commit/ca50db31029311cf8e97a1f48408576b82ffd30d).
- [GitHub Actions #38202663907](https://github.com/Hyperbog/RLOFT/actions/runs/38202663907): real Linux C++ build, **24/24 CTests PASS**, including new \`native_type6_column_sampler_tests\`. Native release Windows workflow was **not** manually dispatched.
- Source implementation: \`include/rloft_native/native_type6_column_sampler.hpp\`, \`src/native_type6_column_sampler.cpp\`. New audit program: \`rloft_native_type6_strip_audit\`. Compiled binary is present in the exact \`RLOFT-native-type6-fixed-point-CA50db31.zip\` Actions artifact, containing **no original game media**.
- The portable NativeEngine library remains independent of \`demo-v1\`.

The implementation is constrained to recovered CODE.2 **0x338C8** behavior:
- Original EH6 128-texel vertical source column, selected from 32×128 or 64×128 decoded RES4 texture. Original CODE.2 source selector advances \`0x40\`; after the explicit factor 2, one column is a **128-byte stride**.
- Integer 16.16 source phase increment exactly \`0x800000 / projected_height\` for explicit positive projected output height.
- Each output pixel reads \`source_column[phase >> 16]\`, subtracts the low byte of the caller-supplied shade (modulo 256), advances \`phase += step\`, and writes one indexed-color value at original **80-byte VGA plane-row stride** to lane \`x & 3\` at byte address \`x / 4\`.
- The caller explicitly supplies height in **1..256**, source column, destination x, 400-line VGA page, 256-line working-region row origin, and shade. The host API centers that bounded height in its region using integer division. **The DOS perspective and clipping policy is not inferred by this API**.
- Two physical 400-line draw pages are supported at 0 and +\`0x8000\` in the four independent 64-KiB VGA plane buffers.
- An **ordered batch of up to 1024 strips is fully transactional**: every source/height/page/address is checked before staging, and the staged four-plane framebuffer is published only after every strip succeeds.
- Original-free regression tests cover width32/width64, heights 1/32/64/128/256, source step, shading/subtraction byte wrap, page flipping, exact plane addressing, overlap semantics, malformed texture, outside source column/viewport/page, 257-pixel rejection, excessive draw-call count, and all-or-nothing rollback.

## Entire original-data corpus: authentic RES4/INIT/COLORS

Original original-owner resource files were read locally and **never committed to Git or uploaded to CI**:

\`\`\`text
RES4    1,975,542 bytes  SHA256 d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46
INIT        1,879 bytes  SHA256 cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276
COLORS     11,521 bytes  SHA256 e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5
\`\`\`

The exact Linux executable from successful CMake/CTest Actions run **#38202663907** was executed locally against the actual original RES4, INIT and COLORS files. Every RES4 entry was decoded as original EH6 LZSS, 15 original COLORS 6-bit DAC palette banks were validated, and the INIT original narrow texture start **600** was applied. Each texture was run through four explicit diagnostic test strips:

| Strip | Original source col | Projected height | Shade subtracted | Target |
|---|---|---:|---:|---|
| 1 | 0 | 128 | 0 | x=0, page 0 |
| 2 | width/2 | 64 | 7 | x=1, page 0 |
| 3 | width-1 | 256 | 255 | x=2, page 0 |
| 4 | width/2 | 32 | 3 | x=3, page 1, 256-line region beginning at row 144 |

These strip coordinates, heights, palette-index subtraction and 400-line page are **diagnostic operands** chosen to test the true source data and sampler, **not asserted to be original game camera output**.

| Authentic original decoded / rendered source property | Real verified value |
|---|---:|
| RES4 entries decoded in compiled C++ | **667/667** |
| Wide 64×128 original textures | **600** |
| Narrow 32×128 original textures | **67** |
| Original DAC palettes validated | **15** |
| Fixed-point source-texel reads | **320,160** |
| Individual four-plane VGA color-index stores | **320,160** |
| Original entry SHA256 per texture | **667/667 agree** |
| Original decoded row-major SHA256 per texture | **667/667 agree** |
| Resulting complete 4×64-KiB framebuffer SHA256 per texture | **667/667 agree** |
| Original-to-compiled metadata fields compared | **4,019/4,019** |
| Divergences | **0** |

An independent Python implementation read the authentic original RES4 binary from scratch, performed CODE.2 LZSS with the original 0xFEE ring-start semantics, transposed source data, and independently sampled the same diagnostic fixed-point strips into four 65,536-byte planes. Its per-entry compressed SHA256, decoded SHA256 and pixel-plane SHA256 matched the actual compiled C++ audit **for every texture**. The combined native and independently calculated fingerprint is:

\`\`\`text
RLOFT-native-RES4-strips-v1
cfdbe7d5731a3b0e9f9828f0fb9a5cded21b3d565d103c9f901c34e583da2524
\`\`\`

Metadata-only local files:
- \`type6_strip_audit/compiled_native_original.json\` (actual GitHub-built executable run with original media)
- \`type6_strip_audit/independent_reference.json\` (separate original-file decoder and indexed-plane interpreter)
- \`type6_strip_audit/original_vs_compiled_summary.json\` (4,019-value complete comparison result)
- \`RLOFT_original_RES4_type6_column_evidence.zip\` (source-independent metadata and no game media)

## Permanent source-only differential gate

Independent comparator \`SharedDecomp/verify_native_type6_columns.py\` on \`code1-decompilation\` parses the original RES4 archive using \`Resources/RES/analyze_res.py\` as a separate independent decoder. It computes all three per-entry digests and all 667 complete four-plane framebuffer fingerprints; source SHA identities of original RES4, INIT and COLORS are pinned by default. An explicit \`--allow-other-version\` mode exists **only for original-free synthetic unit fixtures**. It rejects false original DOS frame parity claims and duplicate JSON keys, and localizes the first differing texture/field.

\`SharedDecomp/test_verify_native_type6_columns.py\` supplies **10 new original-free synthetic regression tests** for compressed EH6, both source widths, each per-texture digest, corrupted original source, palette values, bad INIT division, original hash pin, unsupported screen-frame claims, CLI and duplicate JSON fields.

The research source at [\`cd3b85c020628f64cdb81be049072221eb948495\`](https://github.com/Hyperbog/RLOFT/commit/cd3b85c020628f64cdb81be049072221eb948495) passed [Research Actions #38203008405](https://github.com/Hyperbog/RLOFT/actions/runs/38203008405): **189/189 Python tests PASS** (39 ResearchInfra + 150 SharedDecomp). CI contains only synthetic original-free resources.

### Reproduce with private original media

\`\`\`sh
rloft_native_type6_strip_audit /private/original/RLOFT \
  > /private/native-res4-strip-audit.json
python3 SharedDecomp/verify_native_type6_columns.py \
  --res4 /private/original/RLOFT/RES4 \
  --init /private/original/RLOFT/INIT \
  --colors /private/original/RLOFT/COLORS \
  --native-report /private/native-res4-strip-audit.json \
  --out build/research/res4-type6-original-differential.json
\`\`\`

## Next actual native-renderer integration work

Use the now-proven EH6 sampler for source-decoded actual wall/surface pixel columns after CODE.2 **0x32F93/0x33647** supplies the original material/source-column and **0x338C8** supplies projected height, shade and destination plane address. Unresolved earlier world/raycamera computations must still supply these parameters faithfully. The authenticated EH7 native sprite compositor, complete CODE.2 36B14/369D8 VGA dispatch, 6,204 world object frame-range ownership and 15 COLORS palettes are available independently; binding their *live original draw-call context*, full scene occlusion, depth and color/presentation into a real frame is **not certified**.

**Do not equate this 667-texture source/pixel parity with a completed DOS renderer, authenticated raycaster, original whole-frame screenshot or playable game.** There was no Windows/MSVC current revision build. Keep manual-only Windows releases and no DemoV1 gameplay import.
