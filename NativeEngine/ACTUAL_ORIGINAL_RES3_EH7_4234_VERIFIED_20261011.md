# Verified complete original RES3/EH7 corpus — 4,234 / 4,234

**Date:** 2026-10-11 Eastern, 2026-10-12 UTC. **Scope:** complete original scene-shape data decoding, *not* graphics or gameplay parity.

## Original-media availability and authenticity

The owner uploaded RES+OTHER.zip with all 13 RES0–RES12 resources plus INIT, SINTAB, COLORS and T (17 entries). This removes the last RES3-original-availability blocker. The original RES3 file is **5,228,638 bytes**, SHA-256 **44955ebbed0a8ed8d874fae07b01710317044882ab3c7c95d9be54e883df9a36**. Independently recalculated Git blob SHA-1 **8bcae74169185139b08138a16814558879f6370d**, exactly matching the original game's DnD-Classics/MASTER blob. Original INIT: 1,879 bytes, SHA-256 cc48df4eb4c0710d45e7f5557e440129df44c5efb1af3ca3426ea9f9e3798276. Original SINTAB SHA-256 19b6bc15f1be616bb8ff49dc9029995a1a0413fae06addc9fd41ddef761d0e87 matches the earlier source.

Original game media were extracted locally for read-only tests and **NOT committed to Git, included in CI or distributed in binary artifacts**.

## Exact compiled source and all-original input execution

NativeEngine exact compiled source **5b50e948b526f54f70c10d6cb5020d09ca13caa0**, [Linux Actions run #38192524579](https://github.com/Hyperbog/RLOFT/actions/runs/38192524579): **20/20 CTests passed**. The artifact from that specific build was downloaded and executed locally with authentic original RES3, INIT, NPCS.DAT, OBJECTS.DAT, and LEVELS.DAT.

**Every one of all 4,234 original EH7 entries decoded without error.** Every stream consumed exactly its LZSS input and passed source geometry, column-pointer, signed-adjustment, selector range/parity and termination checks. Four INIT source banks matched 2,169 / 1,147 / 842 / 76 entries. The same run validated original NPC/object database record counts, namespace boundaries and all 6,204 authored world instance records.

| Independent original-data property | Actual result |
|---|---:|
| Original EH7 shapes accepted | 4,234 |
| Source widths 64 / 96 / 128 | 4,158 / 40 / 36 |
| Unique referenced column lists | 123,294 |
| Rasterizer descriptor occurrences across columns | 278,085 |
| Descriptors counted once per unique pointer list | 274,537 |
| Total decompressed EH7 body bytes | 7,600,070 |
| Four original RES3 class-bank sizes | 2,169 / 1,147 / 842 / 76 |

## Exhaustive independent C++ versus Python proof

The C++ auditor now outputs original compressed-entry SHA-256, decompressed EH7-body SHA-256 and typed geometry/list metrics **for each of the 4,234 entries**. The existing independent original CODE.2 research parser and a separately implemented raw-source Python decoder produced the same actual-original results without copying native output into expectations.

**4,234/4,234 original entry hashes matched. 4,234/4,234 independently decompressed body hashes matched. All 38,106 per-shape typed fields matched. Zero differences.** The complete comparison is saved as local metadata-only rloft_RES3_4234_original_vs_compiled_native_verified.json, with fuller source/compiled per-entry evidence in RLOFT_RES3_4234_Verification_Evidence.zip. These reports contain no original compressed or decompressed game bytes.

The canonical reusable oracle is SharedDecomp/verify_native_res3_type7.py on code1-decompilation. It uses independent Resources/RES/analyze_res.py and analyze_type7.py, rejects false parity claims, malformed original input and duplicate JSON, and diagnoses mismatches by entry number and field. Research source SHA **e24e8b2587fbbd068fd48ffc856ca5489af34ff2** passed [Actions #38192758031](https://github.com/Hyperbog/RLOFT/actions/runs/38192758031): **39 ResearchInfra + 110 SharedDecomp = 149/149 source-only tests passed**. Original copyrighted files are never part of these CI fixtures.

## Interpretation and next blockers

**VERIFIED:** All original RES3 EH7 source entries decode in independently compiled NativeEngine C++; actual compressed and decompressed bytes are fingerprint-identical to the independently decoded original; source geometry/list descriptors and original INIT bank partition are preserved and checked; 6,204 original authored world instances already have independent source-base identity evidence.

**NOT VERIFIED:** exact original DOS 3D frame pixels, SINTAB scale-code behavior (original x86 is deliberately never executed), EH7 draw list compositor, EH6 indexed texture application, VGA palette and scale/depth selection, original ray order/occlusion, source-specific perspective and animation frames, actor AI/event interpreter, save-game compatibility, or Windows current-source demo/game acceptance.

**Next integrated code:** recover original CODE.2 scene viewing/subframe/scale decision and translate SINTAB rasterizer spans into safe portable native geometry/pixel operations; bind original EH6 textures and palette; validate against actual DOS screenshots/traces. Do not equate the existing host DDA debug strips with original renderer. DemoV1 gameplay must remain separate from independent NativeEngine. Windows release Actions stay manual-owner-controlled.

This closes the original RES3 corpus decoder acceptance gate, **not the 100% project completion gate**.
