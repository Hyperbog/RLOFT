# DemoV1 Full Windows #654 — C++17 + source-width F790 repairs accepted (2026-10-10)

**Outcome: SUCCESS.** [GitHub Actions run #38079988991](https://github.com/Hyperbog/RLOFT/actions/runs/38079988991), Full Windows, completed 2026-10-10 19:34:49 UTC.

| Checkpoint | Verified evidence |
| --- | --- |
| Run number / ID | **#654 / 38079988991** |
| Run event | Explicitly user-authorized one-shot temporary push trigger |
| Actual build checkout / embedded source SHA | `848ae8ba9e3cddde31bb5dd5ab8dbd26f97fc223` |
| Gameplay/test source immediately before temporary trigger metadata | `88091ad1956b2a5425887906d4fda9e6f58e90c3` |
| Full Windows C++ compilation | **PASS** |
| Canonical CODE.2 C++17 syntax preflight | **PASS** |
| Windows full CTest | **210/210 PASS, 100%** |
| Earlier failing F790 runtime-description regression | **PASS** |
| Windows artifact | `Ravenloft-Demo-v1-Windows-x64` |
| Artifact ID / ZIP size | `11680725331` / **819,160 bytes** |
| Artifact ZIP SHA-256 | `34d06984c763dafc96318090ab2d1567c833a43184114e994bf681015df1fc29` |
| Native EXE SHA-256 | `93c2902cd4410559ff420cccb3fe572d8b93486be0a98851a58dab4df9b0a1c6` |
| Windows EXE publication commit | `b10cf456d7b26429e6c93a84468e2742398d843c` |
| Workflow after run | Original **manual-only** blob `7c8e5720f38c0690d01457ea3daf556124725ef2`; trigger marker removed |
| Linux job | Skipped deliberately |

## What this actually validates

Windows #652 failed before CTest because F790 standalone targets did not inherit C++17; `CMAKE_CXX_STANDARD=17` fixes those target-language semantics.

Windows #653 then compiled successfully and passed 209/210 tests; its only failing test erroneously expected the F66C first ribbon tile at original x=`0x44` from a synthetic 160-pixel `AA` test when the mutable POUCH item message with one-pixel glyphs actually has original computed first tile at **x=`0x84`**. The corrected test measures the actual source text and checks frame-zero and empty F790 saved-page restoration at the same original-calculated tile position without touching shared game RNG.

Windows #654 completes that sequence: all **210 tests** belonging to the exact checkout passed. This includes earlier source-only F790 batch dispatcher, one-shot 112EC capture entry, feedback/selected polling, resource contracts, and the pre-existing whole Level22 native regression suite. Compilation and CTest success do **not** prove the source F790 adapter is wired into live Win32 input or that DOS pixels/IRQ/event timing match.

## Subsequent branch changes: do not conflate with this build

Additional source research and a **new Python-only original 11AD8 actor-toggle F790 oracle** were committed on `demo-v1` after the #654 build checkout. Current HEAD now registers **211 CTests with Python**, so the additional actor-toggle oracle was **not run inside #654's 210/210 Windows CTest**. That new evidence was separately original-binary checked in its own audit, but no subsequent Windows Full run is claimed for it.

The build artifact's embedded source SHA remains `848ae8...`; the checked-in executable was refreshed at `b10cf456...`. Subsequent docs/tooling commits do not make the published binary a build of later HEAD.

## Next user-assisted field test

Download the successful artifact from the [run page](https://github.com/Hyperbog/RLOFT/actions/runs/38079988991) and extract it. Run `Diagnose-Level22.cmd` with the original installed DOS game available through the previously used game-source mechanism. Check entry, movement, party/inventory art, multi-actor columns, encumbrance, open containers, click regions and automap. Press F12 at visible mismatches. The launcher packages bounded diagnostic events/indexed frames into a local ZIP under `%TEMP%\\RLOFT-Demo-Diagnostics`; inspect/upload that ZIP and comparison screenshots only when deliberately requested. The original files remain unmodified.

**Do not expect the new F790 source-event ribbon to be fully live yet**: the event batch/entry adapters have compiled and passed isolated tests, but no full Win32 source-event hookup or native-vs-original DOS pixel/IRQ acceptance has been established. The actor-toggle global-mode branch and item/event fanouts remain additional source-proven live-integration work.

**Readiness remains 49.8% provisional weighted; original-DOS acceptance remains 0% at current revision.** Finish fully faithful Level22 before any independent Level23 gameplay or modernization. CI builds remain user-authorized and manual-only.
