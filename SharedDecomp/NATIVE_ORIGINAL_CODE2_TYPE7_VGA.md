# Original CODE.2 36B14 / 369D8 Type-7 native VGA differential

This canonical source-only research gate compares the **actual original data** to the independent NativeEngine CODE.2 mask/VGA raster dispatcher and SINTAB span compositor.

## Controlling native checkpoint

Exact source \`native-engine:a7220cd0961a55584dc07ba955be18132589f801\`, [portable Linux source-only Actions #38194778519](https://github.com/Hyperbog/RLOFT/actions/runs/38194778519) **22/22 CTests PASS**.

The actual GitHub-built \`rloft_native_type7_vga_audit\` binary ran locally on user-owned original CODE.2, RES3 and SINTAB. It verified **all 28 legal VGA plane-mask triplets**, the two original CODE.2 dispatch modes, source selection of first original shape's first descriptor at SINTAB scale 126, and exact 4×64KiB indexed VGA output. An independent Python direct-original-byte decoder produced the **same 101 source/output values and zero divergences**, including both original diagnostic-mode framebuffer SHA256 values:

\`\`\`text
01d87427b3ddbcfef2d107eecb9edff0e7c1244aa50f94ebf6db4b9151760a5a
\`\`\`

Source original SHA256 identities:

\`\`\`text
CODE.2 853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
SINTAB 19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87
RES3   44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36
\`\`\`

The sample uses diagnostic ECX=0, source base +0, renderer row=0 and page=0, not reconstructed original game camera/shade state. Grouped mode submits **three** plane passes, individual mode **seven**; they use 12 vs 28 masked stores but both touch 28 actual VGA plane bytes, so the equal hash is expected here. Original VGA mode initial value \`CODE.2 object-2 +0xC029=1\`, initial page \`+0xEF4D=0\`. Original mask arrays come directly from object-2 offsets \`C668/C688/C6A8\` and use slot \`(x&3)*8+width\`. Native synthetic tests with nonzero ECX prove the two real original dispatch modes can produce different output.

## Independently reproducible comparator

\`SharedDecomp/verify_native_type7_vga_dispatch.py\` reads original raw CODE.2/SINTAB/RES3 files and checks *pinned retail SHA256*, actual original VGA masks against exact 28 original offset/coverage entries, decodes original RES3 first EH7 entry through the independent \`Resources/RES/analyze_res.py\` LZSS, parses original SINTAB scale-block 126 directly, interprets each raw original six-opcode raster span, computes both complete 4-plane diagnostic digests and source read/write/pass counts, and compares the compiled native JSON report. It rejects duplicate JSON keys, nonfinite JSON, unsupported original execution/frame/gameplay parity claims, missing and mismatched fields.

\`\`\`sh
rloft_native_type7_vga_audit /private/original/RLOFT > /private/native-original-vga.json
python3 SharedDecomp/verify_native_type7_vga_dispatch.py \
  --code2 /private/CODE.2 \
  --sintab /private/SINTAB \
  --res3 /private/RES3 \
  --native-report /private/native-original-vga.json \
  --out build/research/native-code2-vga-original-comparison.json
\`\`\`

Synthetic, licensed-original-free \`test_verify_native_type7_vga_dispatch.py\` builds its own fake CODE.2 mask globals/data tables and fake original-EH7/SINTAB opcode payloads. Its ten tests cover source-mode differences, mask/reference mutation, original source byte changes, native frame hash mismatches, missing or corrupt report fields, fidelity-flag lies and strict CLI JSON validation. [Source-only Research Actions #38195105393](https://github.com/Hyperbog/RLOFT/actions/runs/38195105393) passed **169/169** tests (39 ResearchInfra + 130 SharedDecomp) at source \`d80752689851e55ce781b7c41c89dda6ab90c305\`.

Local original-media evidence files, not committed original payloads:

- \`rloft_native_original_type7_vga_dispatch_a7220.json\` (actual C++ original-input output)
- \`rloft_original_CODE2_VGA_dispatch_native_independent_verification.json\` (independent original byte comparison)
- \`rloft_vga_original_independent.py\` (local independent original parser + interpreter, requires user-owned original game media)

**Verification boundary:** CODE.2 36B14/369D8 masks/dispatch and isolated original-data EH7 indexed-pixel execution are verified. The original camera/depth and 3D scene projection, scene object draw-order/frame-selection, live ECX shading and other state, EH6 textures and palette, original DOS entire framebuffer and a playable native Windows demo remain unverified. Original 16-bit x86 is never executed. Original media never enters CI or source commits. Do not present the host debug Level22 as a completed game.
