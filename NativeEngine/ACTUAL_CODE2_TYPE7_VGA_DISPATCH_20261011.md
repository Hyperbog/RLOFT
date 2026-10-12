# Actual CODE.2 VGA plane dispatch + multi-descriptor native EH7 raster

**Date:** 2026-10-11 Eastern / 2026-10-12 UTC. Source-only renderer plumbing milestone, not full DOS frame parity or playable game.

## Verifiably compiled revisions and integration

- **Native source:** \`a7220cd0961a55584dc07ba955be18132589f801\`
- [Linux source-only native Actions #38194778519](https://github.com/Hyperbog/RLOFT/actions/runs/38194778519): **22/22 CTests PASS**. New \`native_type7_vga_dispatch_tests\` exercises original plane-mask lookup, grouped/individual mode, synthetic real-opcode raster output, second descriptor precedence, and rollback on late malformed data.
- **Research source:** \`d80752689851e55ce781b7c41c89dda6ab90c305\`
- [Source-only Research Actions #38195105393](https://github.com/Hyperbog/RLOFT/actions/runs/38195105393): **169/169 Python tests PASS** (39 ResearchInfra, 130 SharedDecomp). Original-free tests independently reconstruct mask tables and source EH7 spans, reject mismatched output digests and invalid fidelity claims.
- No original copyrighted code/data committed or used in CI. The Linux native program is a read-only auditor. DemoV1 gameplay is not linked into the independent native library.

## Real original CODE.2, SINTAB, RES3 executed through compiled safe native model

Original source identities, independently SHA-pinned:

\`\`\`text
CODE.2 SHA256 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
SINTAB SHA256 19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87
RES3   SHA256 44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36
\`\`\`

The exact GitHub-built \`rloft_native_type7_vga_audit\` binary was executed locally with these **authentic** original files, mounted in an original-media directory that contains original RES0/INIT/T plus the resources needed for the scene. The new \`NativeType7VgaMasks\` reads **three 32-byte mask arrays directly from CODE.2 original LE object 2**, whose original on-disk object start is \`0x75F24\`; object offsets are \`0xC668,0xC688,0xC6A8\`. It also retains original mutable-in-game *initial* flags at \`0xC029=1\` and \`0xEF4D=0\`. All **28** legal phase/width-1..7 mask triplets match independently derived byte/bit coverage exactly.

The new portable \`plan_native_type7_vga_dispatch\` mirrors original \`36B14/369D8\` source-level dispatch:

- **Grouped C029==0:** one original \`369D8\` call per clipped run of 1..7 columns, mask-array slot \`(column & 3)*8+width\`, first VGA pass always issued and second/third conditional on nonzero mask, byte address incremented by +0/+1/+2 across passes. CODE.2 low ECX word bytes are swapped after dispatch.
- **Individual C029!=0:** each column is a width-1 \`369D8\` call, swaps low ECX bytes for odd destination columns and restores caller lookup afterward. This supports certified runs wider than seven pixels *without* inventing a grouped-width-8 mask.
- Original sequencer writes alter separate VGA planes of a 65536-byte-per-plane A0000 aperture. Original page-1 base is +32768; x maps to destination byte x/4 and plane x&3.
- The \`draw_native_type7_vga_dispatch\` compositor processes **each grouped destination invocation's original EH7 descriptors in order**, with every pass run in its observed order. It delegates to the already-verified bounded SINTAB instruction interpreter for individual original spans. The **entire dispatch** is staged into a separate four-plane buffer, then committed only after all passes and later descriptors succeed; malformed later source bytes cannot leak partial writes.

### Authentic RES3/SINTAB indexed-output comparison

Exact original sample operands are deliberately diagnostic:

\`\`\`text
original source: RES3 entry 0, column 0, first descriptor
SINTAB: scale block index 126
viewport run: x=6, width=7
video page: original CODE.2 initial page 0
renderer row/row-byte offset: 0/0
source base: decoded EH7 body + 0
ECX: diagnostic 0 (not claimed DOS shade)
\`\`\`

The authentic shape's one source-list descriptor is resolved to original scale-code entry/stop targets. The native result was compared against a separately written Python original CODE.2/SINTAB/RES3 decoder and four-plane interpreter (no C++ function reuse), with **101 independent source and output fields checked and ZERO differences**.

| Source-backed probe | Grouped mode | Individual mode |
|---|---:|---:|
| Original VGA plane passes | 3 | 7 |
| Invoked source descriptors | 1 | 7 |
| Safe original-code span calls | 3 | 7 |
| Original-source index reads | 3 | 7 |
| Masked write opcodes | 12 | 28 |
| Actual plane writes | 28 | 28 |
| Four-plane SHA256 | Same as below | Same as below |

Both returned the independently reproduced 4×64KiB plane digest:
\`01d87427b3ddbcfef2d107eecb9edff0e7c1244aa50f94ebf6db4b9151760a5a\`.

The equal output in this **specific** source-data call is expected: ECX=0 disables the parity-dependent byte-swap difference, and the masks for both execution modes cover the same seven pixels. Copyright-free native unit tests use **nonzero ECX** to confirm **different expected pixels** and low-byte swap across alternating columns; they also test grouped x=6 width=7 masks 12/15/1, original page 1, a second descriptor, and all-or-nothing rollback. Do not misstate the equal hash as proof of identical general original modes.

Original-data local evidence:
- \`rloft_native_original_type7_vga_dispatch_a7220.json\`: actual compiled C++ output with all 28 original mask triplets.
- \`rloft_original_CODE2_VGA_dispatch_native_independent_verification.json\`: independent Python source file comparison.
- \`rloft_vga_original_independent.py\`: local independent original-byte calculator, with no compiled code or original payload bytes embedded.

Canonical portable source verifier:
\`code1-decompilation:SharedDecomp/verify_native_type7_vga_dispatch.py\`. It independently reads original CODE.2, SINTAB and RES3 bytes, validates pinned SHA256, enumerates original bit-mask triplets, recomputes original generated-code output for the two diagnostic modes, compares native output hashes and counters and identifies the first differing field. It refuses unsupported game/renderer parity flags. **No original media is uploaded to CI.**

## Accurate completion boundary and next engineering work

**VERIFIED:**
- All 4234 original EH7 entries preserved, type-7 structure and 127 original SINTAB programs decoded (earlier gates).
- Native safe original span interpretation with source-data pixel writes (earlier).
- Authentic CODE.2 28 mask-triplet table, two \`36B14\` modes, original pass ordering and 4-plane VGA memory semantics, with **a real original 7-pixel 3-pass and 7-pass diagnostic case**, independently attested.
- Independent compiled Linux C++ and Python synthetic mutation regressions.

**OPEN:**
- Original \`2C294..2C6B6\` scene emission, depth ordering and CODE.2 16.16 camera angle/projection/viewpoint; original actor perspective and animation frame selection; SINTAB per-scene scale choice and original shading/ECX runtime value; original width clipping and original VGA row/page offsets beyond the supplied diagnostic; full scene-source 2D sprite draw/occlusion, EH6 textures, VGA COLORS palette, compositing with level walls/floor/ceiling, DOS frame pixel comparisons.
- Native engine game loop, event VM, NPC/AI, SAVEDAT compatibility, current-source Windows/MSVC compile and user-accepted playable game.

The new renderer is **a real source-data-backed native VGA compositor primitive**. It does not yet feed a complete original 3D scene frame, and is not a faithful playable reimplementation by itself. Do not resurface old Level22 DemoV1 as the default test path. Native Windows release Actions remain owner-manual under the native project's existing release policy.
