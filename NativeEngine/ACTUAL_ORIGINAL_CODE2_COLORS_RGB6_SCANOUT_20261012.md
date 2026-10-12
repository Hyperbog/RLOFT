# Original CODE.2 level palette → COLORS DAC6 → native VGA RGB6 scanout

**Checkpoint (2026-10-12).** This closes the bounded, separately verified original indexed-VGA-pixel → selected DAC6-RGB6 frame conversion. It **does not** certify full DOS framebuffer equality, original live DAC updates/fades, camera traversal, dynamic lighting, actor sprites, or a playable engine.

## Exact compiled NativeEngine acceptance

- Exact native implementation: \`native-engine:7b1f30b98997ac45adedc5c14f12823565b2ed0e\`
- [Source-only Linux Actions #38257306292](https://github.com/Hyperbog/RLOFT/actions/runs/38257306292): **28/28 CTests passed**.
- New source \`NativeEngine/include/rloft_native/native_vga_rgb6_scanout.hpp\`, \`src/native_vga_rgb6_scanout.cpp\`, \`tests/native_vga_rgb6_scanout_tests.cpp\` and \`src/native_vga_rgb6_scanout_audit_main.cpp\`.
- Source CODE.2 object-2 physical file base **0x75F24**, original level display table **+0x81AC**. The first byte of each of **32 contiguous three-byte tuples** selects one of original COLORS **15 palette banks**; the other two original bytes are preserved as metadata, not invented as RGB channels.
- Exact original 96-byte display-tuple table SHA256:
  \`7370bd2fbc47895d0f81826ff98933710d1477b7f36d5882f9ea111e053f8e93\`
- Complete original source identities are pinned: original CODE.2 SHA256 \`853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314\`; COLORS SHA256 \`e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5\`; RES4 SHA256 \`d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46\`.
- All **32 original palette tuples** agree with previously extracted CODE.2 mapping \`native_world_palette_bank\`. Original world palette distribution is bank0: **1** level, bank2: **8**, bank3: **11**, bank4: **12**. Source tuple's other two fields are preserved, including the original byte-zero patterns in some levels.
- Original COLORS contains **15 × 256 × RGB** exact DAC6 values (0..63). The scanout returns interleaved **unscaled** DAC6 components rather than pretending a host 8-bit gamma rule is proven. No original DOS hardware access or x86 executed.
- Native 4-plane VGA read follows original planar \`x&3\`, \`x>>2\`, 80-byte row stride and +32768 page-1 offset; supports explicit \`320×200\` or \`320×400\` frame requests, both pages. The active original display page is a caller input, not inferred from DOS CRTC state.
- The native CTest verifies all 32 authored selectors, synthetic 6-bit palette roundtrip, page-0/page-1 addressing, last pixel of page1, frame length, 6-bit bounds, invalid palette/color/page/height/width inputs. It does **not** claim the source-DAC mapping reproduces original dynamic palette fades/animations.

## Original input versus independent implementation

The precise GitHub-built \`rloft_native_vga_rgb6_scanout_audit\` at \`7b1f30b9\` was executed locally against user-owned originals. Another implementation in \`/mnt/data/rloft_original_vga_rgb6_independent.py\` separately reads original CODE.2/COLORS/RES4 bytes, decompresses original EH6 resource ID 62 with its own CODE.2 LZSS 4K implementation, draws three **explicitly diagnostic** source-backed texture columns (heights 128,64,32; 224 original texel reads) and eight intentionally synthetic source **indexed** palette-probe values \`246..253\` (8 pixel writes), then converts the same four VGA planes to unscaled DAC6 colors for all 32 level palette choices (page0) and level0/page1. Source-supplied resource 62 is real game data; selected ray/height/shade/page conditions are diagnostic host operands.

The eight synthetic indexed swatches are essential for a strong regression gate: the **four original world palette banks differ only for indexed colors 246..253**. The real source texture-only sample never reached those indices and all four outputs falsely appeared identical before the swatches were added. With the explicitly diagnostic swatches, the independent oracle reports **four distinct** level palette RGB6 frame SHA256s. All 32 level-by-level RGB6 frame hashes, the page1 frame SHA256, the original CODE.2 display table hash, the palette distribution and the complete 4×65536-byte source framebuffer SHA256 match the compiled native binary exactly; **zero differences**.

Source-side test evidence (metadata only, original game bytes excluded):

\`\`\`
Original display tuple SHA256:
7370bd2fbc47895d0f81826ff98933710d1477b7f36d5882f9ea111e053f8e93

Three original-source strips + eight synthetic probe indexes,
complete four-plane buffer SHA256:
f097abe852a672a028a6a75d13768b453837b094cd23571642f95eae6bd00fbb

Original level count:                32 / 32
Original COLORS palette bank count:  15 / 15
Unique source-selected world banks:   4
RGB6 level page-0 frame hashes:      32 / 32
RGB6 original level0 page-1 hash:     1 / 1
Native/Python compared fields:       zero differences
\`\`\`

Authoritative original-private compiled evidence:
- \`/mnt/data/rloft_original_rgb6_compiled_7b1f30b9.json\`
- \`/mnt/data/rloft_original_vga_rgb6_python_independent_7b1f.json\`
- \`/mnt/data/rloft_original_vga_rgb6_compiled_independent_verification.json\`
- Exact source-only GitHub Linux binary \`RLOFT-native-RGB6-COLORS-palette-probe-7b1f30b9.zip\`. These contain **no original executable, RES4 texture bytes or COLORS payload**.

Canonical independent source-only original reference \`code1-decompilation:SharedDecomp/verify_native_vga_rgb6.py\` and 10 original-free synthetic regressions \`SharedDecomp/test_verify_native_vga_rgb6.py\`, source commit \`31cae2807895eedbadae45812d76d987190188ef\`. [Research Actions #38257972447](https://github.com/Hyperbog/RLOFT/actions/runs/38257972447) **210/210 tests PASS** (39 ResearchInfra and 171 SharedDecomp). Production comparator SHA-pins original CODE.2/COLORS/RES4 and checks every per-level frame fingerprint and source-bound byte field. Synthetic fixtures never include licensed originals or user-owned media in Actions.

## Next real engineering frontier

- Use source-proven CODE.2 camera/ray walker to supply original wall path, material orientation, projected height and shade, then compose real scene wall and EH7 sprite draw calls in the right depth order.
- Integrate the original *live* CRTC draw/view page state and dynamic DAC palette modification/fades before claiming DOS palette-frame match.
- Compare whole 320×400 original DOS indexed screenshots and DAC6 frames from reproducible original save/playback traces. DOS whole-frame parity and playable gameplay are **still open**.
- Keep the independent NativeEngine separate from DemoV1 and code1-decompilation executable reconstruction; do not trigger manual-only Windows release Actions. No licensed media to Git/CI.
