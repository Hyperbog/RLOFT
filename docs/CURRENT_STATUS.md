# Current cross-workstream status — 2026-10-10

This page states the **latest independently supported baseline at this checkpoint**. Workstream-specific current documents are authoritative for their own subsystem; their contents change without an automatic update to this page. Check live Git HEAD and the corresponding branch before citing a test result.

| Stream | What is established | What is *not* established |
| --- | --- | --- |
| Native game (`demo-v1`) | Manual Windows Full [#658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) passed **214/214** tests on **earlier commit** `ba3dc29e42d93e87a147b4b909c52167dbd3b362`; original RES4[655] terrain verified in 64/64 **native** open-field captures; source-derived nearest 22220 and FF50 probes authored | No current-checkout Windows CTest run after #658; no authenticated DOS pixel/palette/IRQ/RNG equivalence or complete game. Provisional native engineering readiness **49.8% is not game completion** |
| CODE.1 / CODE.2 | All original `CODE.2` object-1 bytes accounted for; source-verified corrections `0139C`, `2975C`, `F790` applied; shared current corrected `Code2.cpp` blob `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e` | Full semantic certainty, DOS execution equivalence and acceptance do **not** follow from byte accounting or compiler success |
| DOSDis | [Authorized cross-platform G2 build #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405) reported green synthetic suites; a bounded independent x86-16 original-code sample is recorded in the [G2 validation record](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/docs/G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md) | No claimed universal accuracy, broad game-title validation, or full DOS reachability proof |
| Extractor | WPF and Python extraction tooling exists in its own owner workstream; original-format audits preserved | Tool build/packaging identity and source fidelity must be checked against a specific release, not the presence of a committed EXE |
| Original media | Original game collection is kept under `DnD-Classics:MASTER/` | No implied redistribution rights or automatic source CI dependency on these assets |

**Immediate shared dependency:** original CODE.2 `F790` makes a stack-local `T[0x3F]` read via `1C368` **before** its empty-item check. The verified correction and accompanying independent source-byte script are synchronized on native, decompilation and canonical disassembler branches. See [source register](SHARED_SOURCE_REGISTER.md). Older G2 review branches and historical media/backup branches are intentionally not rewritten.

## Current verification vocabulary

- **Authored/registered:** source or test exists, but is not necessarily compiled.
- **Locally built:** compiler/tests executed on a stated platform with recorded revision; not a Windows/DOS acceptance substitute.
- **Windows Full green:** exact manual Actions run and tested checkout, not current HEAD by default.
- **DOS accepted:** explicitly matched original-runtime behavior at matching original state with original media and reference evidence. No such completed whole-game gate is claimed here.

Consult [native integration contract](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md), [CODE.2 continuity](../Code2/CODE2_CONTINUITY_ADDENDUM.md), [DOSDis project documentation](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md) for the subsystem frontier.
