# Windows Full #660 — 245/245 passed, normal-launch artifact published

**Owner/branch:** `demo-v1` — Part 14 M-PLAY-01  
**Authorized:** 2026-10-11 Eastern  
**Windows Actions:** [Full #660](https://github.com/Hyperbog/RLOFT/actions/runs/38192487862) (run ID `38192487862`)  
**Exact compiled checkout / build-source-sha.txt:** `5b0b00e89bfdc783ee20c59ad1c546bfa0873311`  
**Windows run conclusion:** **success** (2026-10-12 UTC)  

## Verified CI and compiled source

| Acceptance component | Verified evidence |
| --- | --- |
| Original CODE.2 C++17 syntax gate | Passed |
| Windows x64 full CMake configure/build | **Passed** |
| Complete Windows CTest | **245/245 passed (100%)**, total CTest duration 7.30 s |
| New M-PLAY-01 normal-launch contract | **Passed**, CTest #10 |
| New native trace-integrity regression | **Passed**, CTest #11 |
| Original C050 source-only check | **Passed**, after documented missing `42-way` phrase repaired following failed #659 |
| Windows artifact publication | **Passed** |
| Commit compiled EXE and SHA-256 to Git | **Passed**, commit `9961e8e0c2f738952d1b853ca2072bdd340cae68` |
| Linux optional build | Skipped |
| Original DOS indexed-pixel/tick/IRQ/RNG parity | **Not accepted** |
| Ordinary original-game M-PLAY-01 field playthrough | **Not accepted** |

## Artifact identity independently checked

- Artifact: `Ravenloft-Demo-v1-Windows-x64`, artifact ID **11722062072**, downloadable from [GitHub run #660](https://github.com/Hyperbog/RLOFT/actions/runs/38192487862).
- Downloaded artifact ZIP: **831,435 bytes**; SHA-256 `edf00bd5d323b6166b50b696a85d4a41fec10e769f3c417d70afb2a4fbf23f20`; `zipfile.testzip()` returned no corrupt member.
- Native `ravenloft_demo_v1.exe`: **951,296 bytes**; SHA-256 **`f28f4165b4c4f47071536c4de4b63f12363e55efba1c10a57e5bf3c1297693de`**; identical to committed `DemoV1/Windows/ravenloft_demo_v1.exe.sha256`.
- Artifact `build-source-sha.txt`: exactly `5b0b00e89bfdc783ee20c59ad1c546bfa0873311`; `build-verification-mode.txt`: `full`.
- Artifact includes `Trace-Normal-Launch.cmd` (regular gameplay path, opt-in trace, NO `--explore-level22`), `Diagnose-Level22.cmd` (explicit developer-only shortcut), `Explore-Level22.cmd`, `Capture-Level22.cmd`, `Probe-Level22-Ground.cmd`, `Audit-Starting-Equipment.cmd`, and both source-probe EXEs.
- No original copyrighted game resources are packaged; use separately installed owned media.

## Workflow and provenance cleanup

Original manual-only workflow blob `296f27bb893994c2862b11a7cfc98a948899378c` was restored in commit `bbfd367ad7428eab3e615bfadd2dcfef4b5490d3` while run #660 was compiling. Temporary user-authorized marker `DemoV1/.authorized-full-build-trigger-20261011-part14-mplay01-retry660` is deleted at this documentation checkpoint. Future Windows Full runs still require separate user authorization; source/documentation commits do not trigger another build.

## Next acceptance gate

Use a disposable original-media/runtime installation copy to protect `CHARS`, `SAVE.DAT`, and any mutable state. Run the new **normal-play** `Trace-Normal-Launch.cmd`; report farthest observed ordinary progression and the first reproducible blocker (including a failure before exploration). A successful native build and 245 CTests are not proof that intro, character creation, Level 22, the rest of the game, or original DOS presentation/input behavior work end to end. Keep the developer-only `Diagnose-Level22.cmd` evidence separate. No independent Level 23 development is authorized until Level 22 completion acceptance.
