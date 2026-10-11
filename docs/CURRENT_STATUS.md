# Current cross-workstream status — 2026-10-11

This page states the **latest independently supported baseline at this checkpoint**. Workstream-specific current documents are authoritative for their own subsystem; their contents change without an automatic update to this page. Check live Git HEAD and the corresponding branch before citing a test result.

| Stream | What is established | What is *not* established |
| --- | --- | --- |
| Native game (`demo-v1`) | Manual Windows Full [#658](https://github.com/Hyperbog/RLOFT/actions/runs/38085278540) passed **214/214** tests on **earlier commit** `ba3dc29e42d93e87a147b4b909c52167dbd3b362`; original RES4[655] terrain verified in 64/64 **native** open-field captures; source-derived nearest 22220 and FF50 probes authored | No current-checkout Windows CTest run after #658; no authenticated DOS pixel/palette/IRQ/RNG equivalence or complete game. Provisional native engineering readiness **49.8% is not game completion** |
| CODE.1 / CODE.2 | All original `CODE.2` object-1 bytes accounted for; source-verified corrections `0139C`, `2975C`, `F790` applied; shared current corrected `Code2.cpp` blob `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e` | Full semantic certainty, DOS execution equivalence and acceptance do **not** follow from byte accounting or compiler success |
| Full decomp/recomp (canonical source) | [Windows source compile/link test #38171590171](https://github.com/Hyperbog/RLOFT/actions/runs/38171590171) **passed** on checkout `22a2499cbbcbf1bc472edaafe7368a88276d81ce`: canonical CODE.1 and CODE.2 C++ compiled to code and linked into one executable, one CTest passed exercising both reconstructions and CODE.2 entry-stub callback edge. [Build target](../Recomp/README.md) | This is a **source-link and leaf-execution gate**, **not the DOS game**: original DOS startup beyond the 353A0 trampoline, installed data sections, DOS/DPMI services, character creation and gameplay remain unwired/unexecuted. Earlier syntax-only verification is no longer the whole-build boundary. |
| DOSDis | [Authorized cross-platform G2 build #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405) reported green synthetic suites; a bounded independent x86-16 original-code sample is recorded in the [G2 validation record](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/docs/G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md) | No claimed universal accuracy, broad game-title validation, or full DOS reachability proof |
| Extractor | WPF and Python extraction tooling exists in its own owner workstream; original-format audits preserved | Tool build/packaging identity and source fidelity must be checked against a specific release, not the presence of a committed EXE |
| Original media | Original game collection is kept under `DnD-Classics:MASTER/` | No implied redistribution rights or automatic source CI dependency on these assets |

**Immediate shared dependency:** original CODE.2 `F790` makes a stack-local `T[0x3F]` read via `1C368` **before** its empty-item check. The verified correction and accompanying independent source-byte script are synchronized on native, decompilation and canonical disassembler branches. See [source register](SHARED_SOURCE_REGISTER.md). Older G2 review branches and historical media/backup branches are intentionally not rewritten.

## 2026-10-11 first canonical recompilation build result

[Source-linked Windows C++ execution run #38171590171](https://github.com/Hyperbog/RLOFT/actions/runs/38171590171) passed on `code1-decompilation` commit `22a2499cbbcbf1bc472edaafe7368a88276d81ce` using MSVC 14.44.35207 (Windows 2022). Both complete canonical translation units compiled, were linked into `rloft_recomp_link_gate.exe`, and one CTest passed. The build emitted one C4834 warning in CODE.1 at line 1187; this did not fail compilation. The executable is retained as a seven-day Actions artifact; it is a **developer linker/leaf runtime witness**, not the game.

**Next real blocker:** construct faithful CODE.1/CODE.2 data objects and connect original entrypoint 353A0 to reconstructed startup 35418 and its DOS/4GW interfaces; source probes currently observe only the incoming callback edge and two known-safe memory getters. Do not count this as a completed DOS boot. The historical Watcom 10.0a five-function source-byte experiment is a separate compiler-fingerprinting gate; no native-engine or demo Windows Full was dispatched.

## Current verification vocabulary

- **Authored/registered:** source or test exists, but is not necessarily compiled.
- **Locally built:** compiler/tests executed on a stated platform with recorded revision; not a Windows/DOS acceptance substitute.
- **Windows Full green:** exact manual Actions run and tested checkout, not current HEAD by default.
- **DOS accepted:** explicitly matched original-runtime behavior at matching original state with original media and reference evidence. No such completed whole-game gate is claimed here.

Consult [native integration contract](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md), [CODE.2 continuity](../Code2/CODE2_CONTINUITY_ADDENDUM.md), [DOSDis project documentation](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md) for the subsystem frontier.
