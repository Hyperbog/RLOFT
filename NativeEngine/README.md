# NativeEngine — standalone installation-backed Ravenloft reimplementation

**Owner branch:** `native-engine`. **Product target:** `rloft_native.exe` (planned, **not yet an independently built executable**).  
**Purpose:** a fully native, source-faithful replacement engine that runs *Ravenloft: Strahd's Possession* from the player's original installed resources, in the same *installation-backed engine* sense as OpenMW. This is **not** Demo v1, an asset viewer, a level preview, a Windows wrapper around DOSBox, or a modernization branch.

## Ownership and continuity

The branch was forked at the exact existing `demo-v1` source revision `f0880727c312be4ea30f8ab6032ff413432f5fe4` **only to retain the already developed and verified underlying code**. The inherited `DemoV1/` tree is the **source-integration starting point**, not the shipping product identity. Future product code, build/packaging and acceptance documents belong under `NativeEngine/`; keep `demo-v1` developer previews and experiments separate. Reuse verified `GameSource`, asset decoders and runtime systems without duplicating reverse-engineering evidence or rewriting proven routines unnecessarily. Shared implementations may later be extracted into a common library after source/ABI/tests are checked.

The [original-game fidelity charter](../DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) remains binding: recreate the **entire original game** before any new game features or modern gameplay changes. The original game files and DOS behavior are the final authority. The initial milestone below tests progress; it **does not** replace the full-game goal or waive the Level 22 source-faithful acceptance requirement before implementing independent Level 23 gameplay.

## Evidence-based starting assets, not completeness claims

Existing source contains `DemoV1/include/rloft/game_source.hpp` and `DemoV1/src/game_source.cpp`: `GameSource::open(path)`, autodiscovery, loose-directory media and `game.gog` ISO access. `RavenloftBootAssets` already includes RES archives, palettes, text, sound and other source readers; `BootFlow` models original CODE.1/CODE.2 startup. Those APIs are **reuse candidates**, not proof they cover every original asset or satisfy a stand-alone engine integration.

Do **not** rename or republish `ravenloft_demo_v1.exe` as `rloft_native.exe`. A genuinely distinct entrypoint, app identity, build target, install-path loading and installed-asset gameplay wiring must first exist and pass their separate tests. In particular, do not use `--explore-level22`, synthetic parties, debug teleports or hardcoded scene substitutes as evidence of ordinary game completion.

## M-ENGINE-01 — original-install boot → first gameplay blocker

**Acceptance objective:** a user can launch an independently built `rloft_native.exe`, select or autodetect their legally owned original installation, start the **normal original** game flow, and report the farthest real player-driven action without developer shortcuts. It is acceptable if gameplay stops quickly; identifying that precise limit is the purpose of this first assessment.

| Gate | Observable requirement |
| --- | --- |
| **E0 — Independent host** | Distinct native executable/Windows package; embedded exact source SHA and binary SHA-256. No original copyrighted assets bundled. No DOS binary execution or DOSBox dependency. |
| **E1 — Installation binding** | Select install path, detect loose original files or supported `game.gog` image, validate required media and report missing/incorrect files and version. Load real `INIT`, `RES`, `DAT`, `ANM`, `SINTAB`, etc. only when required by the actual startup/game path. Record which resources were opened without exporting proprietary data. |
| **E2 — Normal boot** | Use original assets for intro/title/menu and genuine CODE.1 character creation, or read a legitimate existing `CHARS`. Cross into original CODE.2 Level 22 startup. No synthetic characters or skip flags in this run. |
| **E3 — Actual interaction** | Player tries ordinary controls, camera, collision, HUD, inventory, automap, interactions, combat and original story events **only as accessible**. Log first reproducible blocker and the farthest unassisted progress, not an invented completion score. |
| **E4 — State integrity** | Prefer a separate writable native runtime copy/profile; back up `CHARS`, `SAVE.DAT` and other mutable state before any write. Test save/restore only if supported, and document omissions. Never silently overwrite the source installation. |
| **E5 — Evidence and fixes** | Provide exact source/build/media version, install validation result, stage-by-stage pass/fail, screenshot/video/optional trace, first blocker and original-code/resource evidence for the next correction. Distinguish compiled, Windows-field-tested and original-DOS-matched outcomes. |

### First test protocol

1. Build the **separate NativeEngine target**, record the source checkout, Windows compilation/CTest and ZIP/EXE hashes. A successful build is not a gameplay/parity result.
2. Extract the **asset-free** package in a clean folder. Point the engine to the player's installed original game (or a protected copy). Prefer an explicit path for reproducibility; test discovery separately.
3. Make one cold-start attempt without `CHARS` and one legitimate existing-party attempt only if safe and available. Do **not** bypass the original menu/creation path to artificially improve the score.
4. Play until the **first reproducible blocker**, noting every stage reached. Classify **P0** boot failure/crash, **P1** progression blocker, **P2** non-blocking behavior/presentation mismatch.
5. Compare observations with original CODE.1/CODE.2 and matching DOS behavior. Fix the **earliest real blocker** in the dedicated engine tree; re-test the same installed media after each source-verified correction.

**Milestone complete when:** an independently built asset-free native executable, a documented install-source verification result, a user playthrough attempt, the maximum genuinely reached gameplay stage and the first actionable defect are all recorded. This is **a measurement milestone, not full-game completeness or DOS fidelity certification**.

**Build authorization:** Windows Full CI runs remain manual and require explicit user approval per execution. Creating the branch, milestone or issue **does not** build/release the engine. No original game media or private saves belong in Git.
