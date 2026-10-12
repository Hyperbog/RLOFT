# Original RES4 EH6 fixed-point 338C8 renderer: full native/independent differential

The independent source oracle lives in \`SharedDecomp/verify_native_type6_columns.py\`. It never imports NativeEngine C++ and never emits original copyrighted pixel/texture data. User-supplied original files are read privately when the production original-data acceptance command runs.

## Actual compiled source

NativeEngine committed source **ca50db31029311cf8e97a1f48408576b82ffd30d**, [Linux source-only Actions #38202663907](https://github.com/Hyperbog/RLOFT/actions/runs/38202663907), **24/24 CTests passed**. Exact compiled original-media-independent audit binary \`rloft_native_type6_strip_audit\` was downloaded from the CI artifact and executed locally with the owner's original RES4, INIT and COLORS.

| Original source | SHA256 |
|---|---|
| RES4 | d8989c50fa58e317ff33465e7baec16e970f40586417d790fd8a1b9005c60f46 |
| INIT | cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276 |
| COLORS | e366986bf701ba832aa769bf99f0db90a76af658300ce3d02e7039a970a9baf5 |

**Real original-source results:** all **667 RES4 EH6 original resources** (600 of width64, 67 of width32; 128 texels per source column) pass original CODE.2 LZSS. All **15** original COLORS DAC6 banks valid. Four fixed diagnostic strips per texture — output heights 128/64/256/32 and shade bytes 0/7/255/3 — generated **320,160 indexed source reads, 320,160 independent VGA plane writes**. The source filename/index, physical encoded entry SHA256, decoded row-major SHA256, per-resource four-plane pixel SHA256, original width, and combined cross-corpus hash were recomputed with an independent original-file Python decoder without consulting the compiled result.

**4,019/4,019 original-vs-native fields matched, ZERO differences.** Combined domain-separated \`RLOFT-native-RES4-strips-v1\` fingerprint:

\`\`\`text
cfdbe7d5731a3b0e9f9828f0fb9a5cded21b3d565d103c9f901c34e583da2524
\`\`\`

This is an independent **data + portable sampler parity proof**, not an execution of CODE.2 x86. The actual game camera height/shading/material selection was **not** available to the four diagnostic strip calls, and the host 256-line-region centering cannot be declared a complete DOS renderer test.

## Reproducible permanent source oracle

The Python original-source verifier \`SharedDecomp/verify_native_type6_columns.py\` imports the original CODE.2 LZSS research implementation in \`Resources/RES/analyze_res.py\`; reads all 667 original EH6 archive entries; validates declared decoded size and compression consumption; reconstructs 128-byte column-major bodies then row-major SHA256; applies the original integer 16.16 source step \`0x800000 / projected_height\` and wraps shading subtraction at byte width; writes to separate 64KiB VGA planes at \`page*0x8000+(row*80+x//4)\`; and recomputes all source/plane fingerprints. The live original RES4/INIT/COLORS SHA256 are pinned by default and unbounded report keys are rejected.

\`\`\`sh
rloft_native_type6_strip_audit /private/original/RLOFT \
  > /private/compiled-original-RES4-strips.json
python3 SharedDecomp/verify_native_type6_columns.py \
  --res4 /private/original/RLOFT/RES4 \
  --init /private/original/RLOFT/INIT \
  --colors /private/original/RLOFT/COLORS \
  --native-report /private/compiled-original-RES4-strips.json \
  --out build/research/original-RES4-strips-diff.json
\`\`\`

The ten new original-free tests in \`SharedDecomp/test_verify_native_type6_columns.py\` use compressed synthetic 64x128/32x128 EH6 records and prove digest mismatch localization, altered original texel detection, palette DAC component bounds, INIT width-bank mismatch rejection, EH6 signature rejection, retail-hash gating, all false fidelity-claim flags, and duplicate native JSON key rejection. [Research source-only Actions #38203008405](https://github.com/Hyperbog/RLOFT/actions/runs/38203008405) passed **189/189 Python tests**: 39 ResearchInfra and 150 SharedDecomp, including all ten new texture tests at source **cd3b85c020628f64cdb81be049072221eb948495**.

The local evidence package \`RLOFT_original_RES4_type6_column_evidence.zip\` includes only original-source and compiled-native SHA256 ledgers, summary and methodology. Neither original media nor decoded original pixel/color arrays are committed or present in artifacts.

## Source compatibility boundaries

CODE.2 \`338C8\` 128-texel texture sampler is now implemented and byte-validated for the diagnostic projected-height inputs. Prior native milestones independently verified the original world/objects source ownership, 4234 EH7 scene-shapes, 127 SINTAB opcode programs, original CODE.2 plane dispatch and safe 4-plane writes. **Still open**: original CODE.2 \`32F93\` live source-column/wall material selection, camera and ray traversal, original spatial projected-height selection, real shading argument, live actor sprite frame/scene emission, depth ordering, pixel-accurate whole-DOS-frame parity, gameplay VM, saves and Windows acceptance.

A successful 667/667 texture render-data verification must **not** be reported as 100% original DOS renderer or a playable replacement engine. Do not mix in DemoV1 gameplay; Windows release workflows remain owner-manual.
