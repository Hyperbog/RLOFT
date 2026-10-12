# NativeEngine — Active developer/agent instructions

## Current controlling original world-to-RES4 surface binding gate (2026-10-12)

[Authentic source material binding and rendered-column milestone](ACTUAL_ORIGINAL_WORLD_RES4_MATERIAL_BINDING_20261012.md). Native commit `06c821e2570b6300fb8b894fe992c84a3ccf3377`, [source-only Linux Actions #38206444852](https://github.com/Hyperbog/RLOFT/actions/runs/38206444852): **25/25 CTests PASS**. Exact CI-built C++ audited original LEVELS.DAT, INIT and RES4 locally: **32/32 levels, 8,192/8,192 tile-code×candidate-bank source bindings, 452 distinct referenced original wide EH6 textures decoded**, 2,894 nonzero + 5,298 zero-valued valid candidate source IDs. The original fifth 64-word header bank is a constant **0xB5B1** and is **NOT** a resource-ID bank. Independently implemented Python original-byte parser/LZSS/4-plane sampler confirmed **all 32 level SHA256 and one real-original diagnostic indexed framebuffer SHA256**, zero differences. Combined domain SHA `565ab170002aa1fcbbf706e6fb3c64429e0d6778f8651f72769639cf6df0a8a5`.

The new `native_world_materials` module joins source-backed original mutable tile/resource variants to actual RES4 EH6 decoding and `native_type6_column_sampler`. Choosing one of the four map material banks, source texture column, camera-projected height, shade, and destination VGA coordinates still requires original CODE.2 camera/view logic; explicit diagnostic draw operands are **not** DOS frame fidelity. Remaining: authentic projection/ray traversal/material selector, wall/sprite occlusion, original palette/frame equality, actor/events/save and playable Windows. No licensed media committed, no DemoV1 gameplay linked, Windows release actions stay manual-only.

## CONTROLLING full RES4 667-texture fixed-point sampler checkpoint (2026-10-12)

[Full original RES4 16.16 texture-column verification](ACTUAL_ORIGINAL_RES4_TYPE6_667_TEXTURE_STRIPS_20261012.md). NativeEngine source ca50db31029311cf8e97a1f48408576b82ffd30d compiled and passed [source-only Linux Actions #38202663907](https://github.com/Hyperbog/RLOFT/actions/runs/38202663907) **24/24 CTests**. New independent native implementation native_type6_column_sampler.hpp/.cpp reconstructs CODE.2 338C8's 128-texel EH6 source column, 0x800000 / projected-height integer 16.16 stepping, byte-index subtraction, 80-byte VGA plane-row writes, page 0/page 1 and transactional draw batches. Original CODE.2 camera, wall material, shade/height operands are EXPLICIT diagnostic inputs, never guessed.

The exact compiled Linux binary was executed locally against authentic original RES4, INIT and COLORS. **All 667 original EH6 resources** (600 wide 64x128, 67 narrow 32x128) and **all 15 COLORS VGA palettes** validated; **320,160 source texel reads and 320,160 masked plane writes** matched a separate independent original-file Python decoder. All **4,019/4,019 per-resource/aggregate typed values matched (zero divergences)**, including every original compressed-entry SHA256, decoded row-major SHA256 and full four-plane indexed-output SHA256. Combined diagnostic digest: cfdbe7d5731a3b0e9f9828f0fb9a5cded21b3d565d103c9f901c34e583da2524. Original media NEVER committed or uploaded to CI.

Canonical research comparator SharedDecomp/verify_native_type6_columns.py introduced in code1-decompilation:cd3b85c020628f64cdb81be049072221eb948495; [research CI #38203008405](https://github.com/Hyperbog/RLOFT/actions/runs/38203008405) **189/189 tests passed** (39 ResearchInfra + 150 SharedDecomp). This is **source/diagnostic texture-pixel parity**, NOT reconstructed CODE.2 live raycast, texture/frame selector, actual camera projected heights, depth/occlusion, screen-composite color output, DOS frame fidelity, native actor/events/save, or playable Windows build. Next integrate source-proven camera and world material/RES4 ID into this sampler and the established EH7+SINTAB+CODE.2 VGA pipeline, then validate complete DOS reference frames. Keep NativeEngine independent of DemoV1 and Windows release workflow owner-manual.

## CONTROLLING source-authored frame allocation milestone — 42,429 valid original RES3 references

[Original authored RES3 frame allocation audit and open fidelity limits](ACTUAL_ORIGINAL_AUTHORED_FRAME_RANGES_20261011.md). Native source **88653be0729ff9d4651a2a50d6f824e626be422a** compiled in [GitHub Linux Actions #38196555130](https://github.com/Hyperbog/RLOFT/actions/runs/38196555130) with **23/23 CTests PASS**. The exact source-only GitHub-built \`rloft_native_scene_frame_ranges_audit\` was run locally against the original \`LEVELS.DAT\`, \`INIT\`, \`NPCS.DAT\`, \`OBJECTS.DAT\` and \`RES3\`: **6,204 authored sources produced 42,429 legal RES3 frame references; all 1,723 distinct referenced original EH7 entries decoded and validated** (1,721 width64, one width96, one width128), with no invalid references. The source-owned family A NPC animation metadata +0x18..+0x1A, family B INIT variant/stride, family C OBJECTS stride/zero-stride, and exact owner-bank boundaries are retained. Native selection requires explicit variant/subframe inputs and never guesses live DOS animation/frame state.

Independent original-byte Python recomputation matched **32/32 per-level hashes and the combined range SHA256** \`a16781d0daeb1ab3aaa1a9f1b8a9a479fc5f5a52ad27ad5311c07a07d23bcf42\` with ZERO differences. Canonical \`code1-decompilation:SharedDecomp/verify_native_scene_frame_ranges.py\` plus negative tests passed [Research Actions #38196899337](https://github.com/Hyperbog/RLOFT/actions/runs/38196899337), **179/179** (39 ResearchInfra + 140 SharedDecomp). Original media never entered Git or Actions.

**Next engineering:** decode original live NPC/view subframe and \`2C294..2C6B6\` depth ordering + frame selection; integrate camera/geometry into existing safe VGA compositor, wall textures and palette, then certify real DOS screenshot/frame output. Range ownership is now verified but original live pose, depth sort, pixels and playable-game parity are NOT. Do not conflate host DDA with DOS raycast or link DemoV1 gameplay. Native Windows Actions remain manual-owner-controlled.

## CONTROLLING original CODE.2 36B14/369D8 VGA dispatch milestone — 22/22 native CTests and original-byte parity

[Complete verified source evidence and limits](ACTUAL_CODE2_TYPE7_VGA_DISPATCH_20261011.md). NativeEngine exact compiled source \`a7220cd0961a55584dc07ba955be18132589f801\` passed [Linux Actions #38194778519](https://github.com/Hyperbog/RLOFT/actions/runs/38194778519) **22/22 CTests**, with the new independent native code \`native_type7_vga_dispatch.*\` and a new actual-source scene audit CLI. The actual GitHub-built binary was run locally against original **CODE.2/SINTAB/RES3**. Its **28/28 original original-DOS VGA mask triplets** and real EH7/SINTAB scale-126 grouped/individual diagnostic modes match an independent Python original-source interpreter: **101/101 source/display-plan check values, zero differences**. Both original-diagnostic modes yielded exact independent VGA plane digest \`01d87427b3ddbcfef2d107eecb9edff0e7c1244aa50f94ebf6db4b9151760a5a\`, with 3 vs 7 source-pass calls, 12 vs 28 masked stores and 28 actual plane writes each. The equal pixel digest applies only with explicit diagnostic ECX=0. Native nonzero-ECX test fixtures verify grouped/individual mode differences and all-or-nothing rollback.

The independent research comparator \`code1-decompilation:SharedDecomp/verify_native_type7_vga_dispatch.py\` at commit \`d80752689851e55ce781b7c41c89dda6ab90c305\` passed [Research CI #38195105393](https://github.com/Hyperbog/RLOFT/actions/runs/38195105393), **169/169 source-only synthetic regressions**. Original media remains outside CI and Git. The full original DOS camera/depth/scale/frame selection, EH6 texture/PAL VGA compositing, actual screen-frame fidelity and game loop/AI/events/save/Windows current build **remain open**. Next: original \`2C294..2C6B6\` emission/visibility/draw-call-to-resource binding and 3D scene projection, then original full DOS frame reference comparison. Do not link DemoV1 game runtime, repeatedly default to the obsolete Level22 demo, or trigger Windows release Actions without proper owner approval.

## CONTROLLING original SINTAB native raster milestone — source and isolated pixel output verified

[Authenticated native SINTAB / EH7 safe raster verification](ACTUAL_ORIGINAL_SINTAB_SAFE_RASTER_VERIFIED_20261011.md). The exact NativeEngine source **a3bc4e24fb18d0c7988dbe1ea261276def6df42d** compiled in [source-only Linux Actions #38194123968](https://github.com/Hyperbog/RLOFT/actions/runs/38194123968) with **21/21 CTests passing**. Source-only SharedDecomp `verify_native_sintab_raster.py`, commit **0a7290fcf2832dfbf8693874b7e3663d1a4da908**, passed [research Actions #38193878447](https://github.com/Hyperbog/RLOFT/actions/runs/38193878447) with **159/159 tests**.

The GitHub-built C++ source was run locally against the owner's authentic original **SINTAB** and **RES3**, without importing bytes into CI. An independent original-source Python implementation reproduced **127/127 code blocks**, **1,282/1,282 block/aggregate typed values**, **834,255/834,255 sample-scale EH7 selector plans**, the complete six-opcode source grammar, and all 11,587 reads / 23,910 VGA store opcodes. **Zero differences**. The bounded portable host raster interpreter additionally consumed an authentic source shape-0 first-column descriptor at scale 126 and produced **one source read and four masked plane stores**, matching a separate Python interpreter's complete VGA-plane SHA256 **e0b7360b0fd1615974122a5a451993b248978affed73333c000bf394ec613ddf**. No DOS x86 is executed.

**Boundary:** Verified original source microcode parsing, selector plans and one diagnostic indexed-pixel execution; NOT original full-frame raycaster/scene projection, shadow/ECX register setup, original SINTAB patch-jump paths, VGA grouping, color palette/EH6 texels, sprites in the live compositor, DOS frame-pixel parity or playable gameplay. Next: hook the proven CODE.2 view-call setup and original resource selection into the safe interpreter, validate complete DOS-reference frames. No DemoV1 game runtime linkage; Windows release Actions remain manual-only.

## CONTROLLING RES3 full-original EH7 corpus milestone — 4,234/4,234 source shapes

[Full authenticated original RES3 validation chapter](ACTUAL_ORIGINAL_RES3_EH7_4234_VERIFIED_20261011.md). The owner-provided RES+OTHER.zip now supplies the complete authentic RES3, confirmed by SHA256 and Git blob identity against DnD-Classics/MASTER. Exact compiled native source **5b50e948b526f54f70c10d6cb5020d09ca13caa0**, [Actions #38192524579](https://github.com/Hyperbog/RLOFT/actions/runs/38192524579) **20/20 portable CTests passed**. That exact GitHub-built C++ executable was run locally against ALL **4,234 original EH7** shape entries: **4,234/4,234 LZSS/geometry/pointer/descriptor decodes passed**. An independent Python decoder confirmed 4,234/4,234 compressed source SHA256 and 4,234/4,234 decoded-body SHA256, **38,106/38,106 per-shape typed fields with ZERO differences**; 123,294 unique referenced column lists and 278,085 raster descriptor occurrences. The complete source is 7,600,070 decoded bytes. No original bytes were committed to Git or uploaded to CI.

Canonical independent source verifier SharedDecomp/verify_native_res3_type7.py at code1-decompilation SHA **e24e8b2587fbbd068fd48ffc856ca5489af34ff2**, [research Actions #38192758031](https://github.com/Hyperbog/RLOFT/actions/runs/38192758031) **149/149 original-free tests passed**.

**Exact completion boundary:** original RES3 4,234-entry native format decoder/data identity VERIFIED; original renderer color/shape projection/frame parity, EH7 SINTAB rasterizer behavior, CODE.2 view subframes, authentic EH6 texture/palette rendering, actor/event gameplay and current Windows/MSVC demo NOT verified. Next work is the source-proved portable renderer and DOS reference frame trace, **not** more source-file acquisition. The original Windows release workflow remains owner-manual. Keep independent DemoV1 game runtime unlinked.

## CONTROLLING authored scene-source milestone — 6,204/6,204 authentic original source bases

[Original-world source instance verification chapter](ACTUAL_ORIGINAL_SCENE_6204_RESOLVED_20261011.md).
NativeEngine source \`5bffd3a3e53a06842d27fbe5c63d574acbc9babd\`
compiled with [Linux Actions #38191784044](https://github.com/Hyperbog/RLOFT/actions/runs/38191784044)
**20/20 CTests passing**; shared research source
\`47cc267de4ca5ea250c8cc9082f1a0dc189811e8\`
passed [source-only Actions #38192139467](https://github.com/Hyperbog/RLOFT/actions/runs/38192139467)
**139/139 regressions**. The real original \`INIT\` (exact Git-blob
verified from DnD-Classics/MASTER), plus original \`LEVELS.DAT\`,
\`NPCS.DAT\`, \`OBJECTS.DAT\`, was read locally by **the actual
GitHub-built native C++ executable**. Independent Python produced
**32/32 exact matching per-level resolved-source SHA256** and the
entire original-source scene-candidate digest:
\`aeb7e334ba23bc114d5444617451495c4bda238d772343f597a9e3b1755d348c\`.
This verifies 1,258 NPC + 4,324 placed + 622 loose original world
records, their CODE.2 source-class-resolved RES3 **BASE** IDs,
positions, and the single original type 0x54→0x58 item alias.

The C++ native EH7 display-list decoder and INIT four-bank source
catalog are compiled and synthetic CTest-verified, but **actual original
RES3 content has not yet been passed through that new binary**; RES3
is present in DnD-Classics Git but not local test media. That complete
4,234-entry live decoder run is the next exact-file acceptance gate.
Neither original view/animation subframe nor SINTAB x86 rasterized
pixels are verified. The host raycast diagnostic must not be called
the finished DOS engine. Do not link DemoV1 game runtime, commit
original payload data, or trigger manual-only Windows release workflow
without owner authorization.

## CONTROLLING verified native scene view/resource-header milestone — 288/288 original sections (2026-10-11)

[Evidence chapter](ACTUAL_SCENE_HEADER_288_AND_PROBE_20261011.md).
Exact compiled Linux SHA \`0850d312a76a52715eec459063a47b393396382d\`;
[Actions #38190025508](https://github.com/Hyperbog/RLOFT/actions/runs/38190025508),
**18/18 CTests PASS**. New independent NativeEngine now retains **all
676 original header bytes per map** (five 64-entry resource mapping
tables), resolves conservative original-backed resource candidates,
and has a bounded camera movement / 320-column raycast **diagnostic
host probe**. The latter uses original level topology and proven
normal collision but **is not the original DOS traversal, projection
or renderer**; its artificial RGB debug image is *not game artwork*.

The actual CI-built native executable was executed locally against
the owner's authentic \`LEVELS.DAT\`. Independent original byte parser
and actual native SHA-256 output match **288/288 map section digests**
(32 maps × complete header, cells, 10-byte controls, events, automap,
annotations and each A/B/C record family). New header-only source
SHA-256 matched **32/32**. This required a real-world malformed JSON
separator fix; the fixed compiled executable now emits valid JSON.
The canonical comparator is
\`code1-decompilation:SharedDecomp/verify_native_world_fingerprints.py\`,
which now requires the header digest too; [research CI
#38190191412](https://github.com/Hyperbog/RLOFT/actions/runs/38190191412)
passed **129/129** synthetic source-only tests.

**Next controlling work:** replace host DDA with original CODE.2-proven
orientation/traversal, bind validated RES3/EH7 and SINTAB microcode
semantics safely to the renderer, EH6 textures/VGA palettes, then
integrate animation, true party motion, NPC/event VM and SAVE
compatibility. No original media is committed; current Windows/MSVC
build and field/gameplay/visual parity remain unverified, and Windows
Actions stay manual-only. Keep independent \`demo-v1\` game runtime
unlinked.

## CONTROLLING native persistent-world milestone — actual original 32-level runtime verified on Linux

[Compiled source and full 32-level normal-collision evidence](ACTUAL_ORIGINAL_RUNTIME_32_LEVELS_VERIFIED_20261011.md).
At exact native source `20543d8fd3a6c61aad7c886d6d1699f1fbf07005`,
[Linux CI #38186427291](https://github.com/Hyperbog/RLOFT/actions/runs/38186427291)
passed **17/17 portable CTests**. New `NativeWorldRuntime` keeps 32 level
states, mutable 16-bit cell/automap and original ten-byte control records,
source-hash-bound snapshots, evidence-backed ordinary/dynamic collision,
kind-1 animation tick and bounded renderer-input marked-cell inventories.
The **actual compiled GitHub Linux runtime auditor** was independently
checked against the owner's original `LEVELS.DAT`, and exact source-level
control/normal-collision outcomes match all **115584 cells** (87983
normally passable, 26857 ordinary blocked, 744 special blocked; 745 special
centers, 324 kind-1 dynamic records). Python independently reproduced the
**full retained-world source SHA-256**
`a8232782ff8654f9b6c6651300b3fbb2235e58d2fadf61fe2221e11dbbc8bdcd`,
including cells, controls, A/B/C records, all event slots, automap and
annotations. Canonical `SharedDecomp/verify_native_world_runtime.py`
adds reusable cross-language gates, with [research CI
#38186651456](https://github.com/Hyperbog/RLOFT/actions/runs/38186651456)
**124/124 source-only tests passing**.

**Completion boundary:** fully decoded original world + native
state/collision/animation primitives YES; **not** proven player traversal,
original raycaster, graphics projection/occlusion, event VM, actor AI, save
compatibility, Windows/MSVC current build, or playable full game. The
Windows release workflow remains manual-only. Keep separate `demo-v1`
out of independent native runtime linkage. Next priority: first-person
ray traversal/RES3-SINTAB/EH6 renderer and the authenticated original
event/movement behavior, not additional generic JSON audit tools.

## CONTROLLING SOURCE-ONLY MILESTONE — actual NativeEngine byte round-trip VERIFIED on Linux (2026-10-11)

**Exact compiled C++ source:** `4676ebf50668480512861c9fd981d6e36402da82`.
**GitHub Actions Linux source-only run [#38185185120](https://github.com/Hyperbog/RLOFT/actions/runs/38185185120): 16/16 CTests PASS**, including the native bank and new world-payload tests. The full portable engine sources compile. A prior Linux SIGILL was traced to an EH2 font validator falling off a `bool` function; the explicit non-inert return repaired genuine undefined behavior, without disabling the test. The manually authorized Windows/MSVC workflow remains unchanged and **has NOT** built this newer revision.

The two GitHub-built Linux auditors from **that exact source** were downloaded and executed locally on the owner's real `LEVELS.DAT` and `SINTAB`, each pinned to its SHA-256. NativeEngine produced a complete 32-level world report (115,584 cells, 1,659 controls, 394 event slots / 555 commands, 127 SINTAB scale blocks), and its native typed-world payload round-trip returned `decoded_section_roundtrip_equal=true`. A separately implemented Python original-byte oracle independently verified **256/256 source section SHA-256 hashes**: all 32 levels × packed cells, controls, event slots, automap, annotations and the A/B/C compact families. Full original `LEVELS.DAT` SHA also matched between the independent reader and native executable. This supersedes the prior unbuilt source-only parity note for **world data preservation**.

The local isolated source directory included copies of the authentic `LEVELS.DAT` and `SINTAB`, plus **empty synthetic INIT, RES0 and T signature files solely to satisfy NativeMedia directory recognition**. This is *not* a validated playable original installation; no game input was uploaded to CI, shipped in the executable ZIP, or committed. The verification evidence is retained as the local user-facing report `rloft_native_original_sections_256_verified.json`; source and binary SHA/provenance details are recorded in [the milestone documentation](ACTUAL_ORIGINAL_WORLD_256_LINUX_VERIFIED_20261011.md).

**Project completion boundary:** Compiled Linux content decoder and byte-exact source preservation YES; current Windows/MSVC executable validation, original DOS runtime execution, 3D renderer/texture parity, AI/NPC/event VM and playable independent native game NO. The independent `demo-v1` gameplay implementation must never be linked into NativeEngine.

## New typed-world SHA-256 round-trip gate — source only, not yet a Windows build

[Full report contract and implementation](WORLD_FINGERPRINT_COMPARISON_20261011.md):
\`native_world_fingerprints\` now reconstructs the **full 32-level typed
world source sections**, verifies each decoded section byte-for-byte
against the user-owned original image, and emits **256 section SHA-256
hashes** across cells, controls, all event-slot bytes, automap,
annotations, and the three eleven-byte families. The new
\`rloft_native_payload_audit\` is a fourth internal C++ executable
configured in CMake, not yet a certified/packaged Windows binary.
Existing \`native_world_payload_tests\` now include three standard SHA-256
vectors and structural mutation rejection. **Current native-engine
HEAD has not been compiled or CTest-certified on Windows.**

The independent canonical \`SharedDecomp/verify_native_world_fingerprints.py\`
compares these native hashes against independent original-file parsing.
On **actual owner-supplied original bytes**, an isolated locally compiled
C++17 SHA implementation and independent Python \`hashlib\` agreed on
**385/385 hashes** (256 per-level sections, 127 SINTAB blocks, two full
original input files); four separate local byte-mutation controls were
all detected. This validates SHA/corpus evidence, **not** the exact
\`native-engine\` source compile or native gameplay. Canonical research CI
passed **106/106 source-only tests** including native-shaped synthetic
fingerprint comparison. Windows Actions remain manual-only, requiring
explicit owner authorization. Next is compiling the exact native source
and independently comparing the real \`rloft_native_payload_audit\` output,
then integrating verified world state into first-person simulation/rendering.

## Typed original-world payload continuation — SOURCE ONLY, UNBUILT

[Original-world payload retention](WORLD_PAYLOAD_RETENTION_20261011.md) at native
commit `19ada509fe03c5718a49cc6ce08b2154f5c233e2` adds
`native_world_payload`: all 32 original packed cell grids, column-owned
10-byte controls, complete 11-byte world families, full event-slot data and
automap/annotations, plus isolated mutable cell/automap copies. The standalone
`native_world_payload_tests` is registered as the **17th CTest**. This is a
real implementation step toward native simulation, not full gameplay or DOS
execution parity. **No build/CTest of this newer source has been run**;
the last certified MSVC artifact remains pinned to original
`44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`. Do not trigger native
Windows Actions without explicit owner approval. Independent source-byte
fingerprints are available under canonical
`code1-decompilation/SharedDecomp/original_world_oracle.py`; native decoded
payload bytes are not yet compared against those hashes.

## NEW controlling milestone — original WHOLE-CONTENT CERTIFIED, not native gameplay complete (2026-10-11)

Read [WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md](WHOLE_CONTENT_WINDOWS_CERTIFICATION_20261011.md). **Windows MSVC source SHA `44cc2ccc950aa686d6563b9eda6cd804b1b08cf8`**, [actual Actions run #38174139897](https://github.com/Hyperbog/RLOFT/actions/runs/38174139897), **16/16 CTests PASS**, all owned `DnD-Classics` original RES0–RES12 **12,562/12,562 typed/empty/raw records PASS**, all 8,362 EH7 source geometry/display-list records pass, all 1,319 EH6 textures and all 1,005 VOC containers PASS, all 32 LEVELS.DAT levels and 127 SINTAB raster-code blocks structurally accepted on real game bytes. Original world totals **115,584 cells, 1,659 controls, 394 active event slots / 555 commands**. This supersedes older small-step 9/9–14/14 build notes without invalidating their narrower historical evidence. Native content system has *three* independent Windows executables now: GUI, native whole-bank audit, and native whole-world+SINTAB audit; output ZIP is original-asset-free and independently SHA256 verified.

**DO NOT CONFUSE WITH GAMEPLAY:** the new C++ parsers handle original full source data but do not execute SINTAB x86 code, simulate NPCs/events or draw original 3D scenes end-to-end. **Next priority: integrate existing typed all-bank + all-level + SINTAB readers into a persistent native scene/world simulation and renderer**, including texture shading, CODE.2 geometry semantics, VGA palette context, event VM, ANM/audio/save. Avoid recurring one-EH-record preview releases; the owner explicitly requested large, integrated engineering strides. The separate `demo-v1` recomp remains untouched and must never become the new native engine's gameplay runtime dependency. Documentation-only HEAD after the above compiled SHA is not an executable build.


## RES archive contents: do not treat all records as EH1 raster images

The owner field-tested native RES3/4/5/6/10 and found "no additional EH type1", which is **correct for the old EH1-only viewer** but not a broken install. [RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md](RES_ARCHIVE_TYPES_AND_EH6_WINDOWS_20261011.md) records exact source types: RES3 4,234 EH7 scene shape/display-list records; RES4 667 EH6 world textures; RES5 EH3 XMIDI music; RES6 and RES10 EH4 VOC audio. RES9 contains 517 EH1 graphics and 38 script records. New native engine source includes a reusable original LZSS4K, standalone EH6 32x128/64x128 column-major texture decoder/preview, RES9 browsing and informative type-specific non-image errors. Real Windows build [#38172310617](https://github.com/Hyperbog/RLOFT/actions/runs/38172310617) compiled **source `744036693e84619d136d69dab157e9de25a2086e`**, passed **14/14 CTests** and uploaded checked asset-free ZIP. Owner actual GOG *EH6 pixel appearance* remains pending; do not mark visual parity passed. Next: compare texture orientation and original CODE.2 runtime shading, and reconstruct independent SINTAB/EH7 scene renderer. Treat audio archives as audio, not corrupted graphics. Keep DemoV1 recomp separate.


## Proven palette context is NOT per RES archive (user field finding, 2026-10-11)

The owner found multiple incompatible palettes *inside* the same RES file, matching existing original CODE.2 palette-routing research. [ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md](ORIGINAL_PALETTE_CONTEXT_ROUTING_20261011.md) is now the controlling checkpoint for native asset display. Current Windows source has `native_palette_context`: **exact callsite evidence, optional active-world-level palette, canonical PREVIEW ONLY, explicitly UNRESOLVED**. The 32 original level display banks and mixed RES0 palette IDs are tested, and Windows GUI defaults to `AUTO (source context)` with manual 0–14 override. Never assign one COLORS palette to a whole RES archive or assume each raster has a permanent correct bank; the eventual native renderer must carry mutable VGA DAC/level/modal/fade context. A fixed bank for a resource is evidence only for the original callsite that was observed.

Real Windows [run #38170601091](https://github.com/Hyperbog/RLOFT/actions/runs/38170601091) compiled source SHA `b1892e8448734f3ad8018a0a3911fe9ed59d582c`, **13/13 CTests PASS**, verified asset-free ZIP. **New AUTO palette appearance on owned GOG is NOT yet field-accepted**. Previous GOG ISO indexing 19/19 remains valid. No DemoV1 runtime linkage or original files packaged. Documentation HEAD after this source SHA is not what was compiled.


## Wrong-looking native RES colors — corrected source and MSVC verified, field comparison pending

[WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md](WINDOWS_PALETTE_CONTEXT_CORRECTION_20261011.md) records the owner's observation: native and separate decomp-demo renderers can show inverted-looking sprite colors. The confirmed native cause is a **single hardcoded COLORS bank 12 for every original RES resource** and omitted CODE.2 menu cursor **19-index remap**. Real game uses multiple bank contexts: menu 12, character creation 8, opening logos 14/9, etc. Correct independent colorizer in `src/native_raster_colors.cpp` takes the decoded palette explicitly and performs original DAC6 RGB→Windows BGRA without blind channel swapping/inverting. Windows asset UI has 0–14 bank selection, a Title cursor mode and raw RES browsing.

[Windows MSVC run #38169819411](https://github.com/Hyperbog/RLOFT/actions/runs/38169819411) compiled exact Git SHA `1c9440303a5c83ff6834a3f87cfdf9afc8dbfa19`, passed **12/12 actual CTests**, source-identified archive hashes and no original game assets. **The corrected visuals have not been user-field-verified** on the owned GOG install. Await a screenshot with the archive ID, record number and active bank before declaring DOS palette parity. The separate DemoV1 recomp project was NOT modified. Later docs-only HEADs are not the compiled revision.


## Verified independently built original RES browser (2026-10-11)

[WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md](WINDOWS_NATIVE_RES_BROWSER_VERIFIED_20261011.md) records actual verified Windows MSVC x64 execution: separate EH1 decoder build [run #38168263623](https://github.com/Hyperbog/RLOFT/actions/runs/38168263623) initially found a malformed test lambda capture, then passed **10/10** CTests after fix; full original RES image browser [run #38168898735](https://github.com/Hyperbog/RLOFT/actions/runs/38168898735) compiled source SHA `d29914503fb6d6bb3a65954257d0ac145f0ceaf0`, passed **11/11**, and produced asset-free AMD64 GUI EXE, with exact ZIP and executable SHA-256 independently verified. The new browser selects RES archive/record and navigates type1 images without flattening/repacking the install; other EH types are explicitly not decoded yet. **New raster output against user-owned GOG has NOT YET been field verified**; earlier 19/19 real GOG resource inventory belongs to a previous compiler SHA. Do not call documentation-only later HEAD compiled. Keep DemoV1 full decomp/recomp separate and do not resurrect gameplay source linking. Next resource work should inspect real images, then typed EH/ANM/DAT formats.


## Real original GOG asset loading accepted; EH1 raster next unbuilt gate

The owner provided a **Windows screenshot from the independent `rloft_native.exe` asset-loader** opening their original GOG `game.gog`. [Field ledger](WINDOWS_REAL_GOG_ASSET_FIELD_TEST_20261011.md) records **ISO2048, 69 indexed files, 19/19 known resources structurally validated, 0 missing, 0 invalid and original palette12 displayed**. This closes the *real source-media indexing and structural inventory* gate of the earlier compiled `fd314b89` binary. It does **not** establish deep decoding or gameplay.

[New native EH1 decoder and cursor visual proof](NATIVE_EH1_RASTER_MILESTONE_20261011.md) is source-committed: `src/native_raster.cpp` independently parses raw/LZSS EH type-1 RES graphics, original `INIT[0..1]+0x2F` cursor resource, transparent key, bounded raster sizes; native GUI now optionally displays its actual sprite under palette12, and `--verify-raster` has strict original-media acceptance. New `native_raster_tests` is an expected **10th Windows CTest**. **No MSVC run or user-owned GOG field test has been performed for this new revision.** Preserve demo-independent build and exact Git source-revision claims; the earlier 9/9 Windows build does not cover new code. No new Windows workflow run without user authorization.


## First verified independent Windows asset build — new accepted state

[WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md](WINDOWS_NATIVE_ASSETS_VERIFIED_BUILD_20261011.md) is the exact execution evidence. **MSVC x64 compiled the independent `rloft_native.exe` from SHA `fd314b89e96db07c739176d25f1fc9be94ecaa19`**, **9/9 native Windows tests passed**, source-identified no-assets ZIP uploaded and its actual bytes downloaded/verified. [Windows Actions run #38164976411](https://github.com/Hyperbog/RLOFT/actions/runs/38164976411). This replaces earlier **no native asset Windows compilation** claims; those were historical. The **real user-owned GOG installation has NOT yet been opened by this independent asset engine**, so field compatibility is still pending. Historical decomp bootstrap's original-menu screenshot does not verify this parser. Do not call any documentation-only later HEAD compiled. No new automated push triggers on main development branches; one authorized isolated build was used.


**Source of truth:** [ASSET_RUNTIME_ARCHITECTURE_20261011.md](ASSET_RUNTIME_ARCHITECTURE_20261011.md). Owner explicitly corrected architectural confusion on October 11, 2026.

## Two different products — DO NOT MERGE

- **`demo-v1` branch:** complete source-traced **decomp/recomp** workstream. Its intro cinematics, menu, character creation, DOS timing/animations and game controllers remain its own executable/project. It is allowed as **research and DOS-fidelity evidence**, not as a production dependency of `NativeEngine`.
- **`native-engine` branch:** wholly new, standalone replacement engine akin to **OpenMW/KEX**. Its executable must load original user-owned file data and independently implement rendering, gameplay, physics/world, audio, scripting and saves. **Do not link `DemoV1/src/win32_main.cpp`, 176-file game core, BootFlow/CharacterCreationFlow or any original DOS module** into `rloft_native.exe`. Do not copy/rename a decomp demo EXE or construct another old-code wrapper.

**A previous attempt to compile the decomp demo as native was mistaken and retired.** The modified DemoV1 CMake/host/media files were restored to previously verified blobs. Old docs `DEMO_BOOT_PIPELINE_REUSE_20261011.md` and `LIVE_ORIGINAL_RUNTIME_INTEGRATION_20261011.md` are historical, **superseded**. The previous Windows bootstrap (15/15 actual CTests; original GOG screenshot) is valid archival evidence but used decomp resource parsers; **its results do not certify the new independent native asset reader**.

## Active platform and file I/O decisions

- **Windows x64/MSVC first**. Do not spend new effort on Linux porting, Linux CI or Linux packages. Keep C++17 renderer/gameplay boundaries portable by design only where that does not delay Windows.
- **Read original media directly, lazily, read-only.** Mount original DOS loose files or GOG `game.gog` ISO9660; never modify installed files, never bundle game assets, never run CODE.1/CODE.2 executables or game.gog under DOSBox. Full installation unpack/repack is NOT a runtime requirement.
- **Native original asset path:** `native_media.hpp/.cpp` (ISO sector indexing/loose media/unique DOS paths), `native_assets.hpp/.cpp` (RES index/entry and COLORS 6-bit palette), `win32_native_assets.cpp` (independent Windows UI and user-owned GOG validation). `NativeEngine/CMakeLists.txt` contains **no DemoV1 source paths** and builds `rloft_native.exe` from these NEW sources.
- **Future asset cache** (optional): separate user-owned profile, disposable, keyed by source-byte hash + independent codec revision + resource ID; never replace the original ISO and never commit derived copyrighted resources. Mod VFS must layer above the immutable base, no silent save-file fallback to purchased installation.
- **Type/format validation**: bounded reads, case-insensitive DOS names, no path traversal, duplicate resolution fails closed. Tests must use synthetic copyrighted-free ISO2048 and raw 2352/offset16/24 as portability baselines, and a **REAL original owned GOG installation on Windows** for acceptance. Tests that only run old decomp source do not count toward native asset validation.
- Decode original assets as independent typed data: RES graphics/texture/scene/audio, `COLORS` palette, ANM video, DAT world/objects/NPC, music; add original gameplay semantics in new runtime once data is proven. Do not fake data, convert original game code into game loop, or overwrite retail saves.

## Builds, progress and release honesty

Current **only active Windows native-engine workflow**: [native-engine-assets-windows.yml](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-assets-windows.yml), **manual-only** on default and native-engine branches. [Build-NativeAssets-Windows.ps1](Windows/Build-NativeAssets-Windows.ps1) builds with VS2022/MSVC x64, requires **at least 9** source-independent native CTests, verifies ZIP and EXE SHA256 and packages only four engine-owned files. Source output marks `engine_stage=NATIVE_ASSET_LOADING`, `gameplay_verified=false`.

**Current validation state:** independent source implementation and tests committed; **NO new Windows/MSVC compile or user-owned original installation field test has run for this new engine.** The old 15/15 Windows result is for the retired bootstrap. Do not increment gameplay completion, represent original ANM/DAT decode as implemented, or reuse old binary/artifact. Windows CI runs require an explicit new user instruction; `continue` does not automatically authorize a new run.

For any substantial task: inspect live HEAD/branch differences, original Code1/Code2 archaeology, applicable continuity, and source data; use separate self-contained native source/CTest improvements, commit with `[skip ci]`, report actual evidence and remaining blockers; preserve decomp/demo changes on its own branch.

**Next exact gate:** build native asset engine under Windows; have owner select real `game.gog`, validate 19 named original sources/RES entry tables/palette12; then implement genuine independent EH sprite/ANM/DAT decoders and rendering. Focus on original-data fidelity before graphical modernization or mod extensions.
