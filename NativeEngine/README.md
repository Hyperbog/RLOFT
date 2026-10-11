# NativeEngine — standalone installation-backed Ravenloft reimplementation

**Owner branch:** `native-engine`. **Product target:** `rloft_native.exe` (planned, **not yet an independently built executable**).  
**Purpose:** a fully native, source-faithful replacement engine that runs *Ravenloft: Strahd's Possession* from the player's original installed resources, in the same *installation-backed engine* sense as OpenMW. This is **not** Demo v1, an asset viewer, a level preview, a Windows wrapper around DOSBox, or a modernization branch.

## Windows x64 is the active build target

**[Windows-first build and acceptance checkpoint](WINDOWS_FIRST_BUILD_AND_ACCEPTANCE_20261010.md)** supersedes Linux-focused development priorities. The independent `rloft_native_bootstrap.exe` Win32 application source now uses the actual inherited `GameSource` to autodetect or select an owned installation, inspect original resource availability, and present an explicitly **synthetic** indexed-frame display diagnostic. A separate `rloft_install_probe.exe` supports Unicode Windows install paths. **Neither is a playable game.** They do not copy the Demo V1 binary and do not modify original assets.

Build with **MSVC x64, CMake and Python** using [`Windows/Build-NativeEngine-Windows.ps1`](Windows/Build-NativeEngine-Windows.ps1). The separate [manually invoked Windows GitHub Actions workflow](https://github.com/Hyperbog/RLOFT/actions/workflows/native-engine-windows.yml) is registered on the default branch for the Actions UI, checks out the actual `native-engine` source, runs Windows tests and would produce an **asset-free Windows ZIP with executable SHA-256 and exact source revision**. **No Windows run or produced ZIP has yet been independently verified**: source/workflow registration is not a successful build. The workflow has no Linux job and no automatic triggers. The existing portable game-core code is retained, but Linux compatibility and packaging are not active release goals.

## Ownership and continuity

The branch was forked at the exact existing `demo-v1` source revision `f0880727c312be4ea30f8ab6032ff413432f5fe4` **only to retain the already developed and verified underlying code**. The inherited `DemoV1/` tree is the **source-integration starting point**, not the shipping product identity. Future product code, build/packaging and acceptance documents belong under `NativeEngine/`; keep `demo-v1` developer previews and experiments separate. Reuse verified `GameSource`, asset decoders and runtime systems without duplicating reverse-engineering evidence or rewriting proven routines unnecessarily. Shared implementations may later be extracted into a common library after source/ABI/tests are checked.

The [original-game fidelity charter](../DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) remains binding: recreate the **entire original game** before any new game features or modern gameplay changes. The original game files and DOS behavior are the final authority. The initial milestone below tests progress; it **does not** replace the full-game goal or waive the Level 22 source-faithful acceptance requirement before implementing independent Level 23 gameplay.

## Platform/framework architecture review (2026-10-10)

[**Framework research RFC-001**](FRAMEWORK_RESEARCH_RFC_001_20261010.md) recommends a **headless portable C++ game core** plus **SDL3 platform/input/audio** and a **renderer-independent GPU API**, with **bgfx as the provisional favorite** against SDL3 GPU and Godot GDExtension. It also specifies early layered mod VFS, versioned content/data schemas, sandboxed Lua interfaces and optional editor tooling, with source-faithful original execution kept separate from future expanded content. **No renderer selected definitively or dependency installed yet; no prototype benchmark has been run.** [Architecture decision/benchmark issue #10](https://github.com/Hyperbog/RLOFT/issues/10) owns objective comparison before engine implementation settles on a renderer. This is distinct from [M-ENGINE-01 original-install first playtest #9](https://github.com/Hyperbog/RLOFT/issues/9).

## Current checkpoint — Proof C (supersedes earlier engineering test counts)

**[Framework Proof C — original GameSource + installation inspector + transactional mod mounting](FRAMEWORK_PROOF_C_INSTALL_AND_MOD_TRANSACTION_20261010.md)** now verifies the **actual inherited original-media reader**, SHA-matched to the repository, rather than only pure callbacks. A separate **`rloft_install_probe` console executable** builds under Linux and detects loose files and supported GOG/ISO image layouts; it is **NOT** the planned playable `rloft_native.exe`. Synthetic original-style install fixtures cover loose source, ISO2048, raw ISO2352 offsets 16 and 24, auto locate, and failure/no-write behavior. Mod mount dependency ordering and atomic *in-memory* directory-plan rollback are compiled and tested. The full independent preflight passed **10/10 CTests in both local Debug and Release**; the original reader CLI and mod transaction separately passed strict ASan/UBSan. **No Windows exe, actual purchased-game install field test, SDL GPU/bgfx/Godot backend, playable normal game boot, or DOS parity accepted.** The older Proof A/B paragraphs below are historical checkpoints, not the latest test count.

**[GPU backend acceptance specification](GPU_BACKEND_ACCEPTANCE_SPEC_20261010.md)** now defines real SDL_GPU fence-delayed transfer readback, bgfx readback/frame constraints, offscreen exact RGBA8 readback, shader/palette correctness, and a separate SDL3 Renderer faithful-original display fallback. GPU selection remains provisional and must be decided by actual controlled measurements. M-ENGINE-01 [GitHub issue #9](https://github.com/Hyperbog/RLOFT/issues/9) is still open.

## Framework proof A — compiled architecture contracts

[**Proof A audit**](FRAMEWORK_PROOF_A_CONTRACTS_20261010.md): An independently compilable **C++17 renderer-neutral indexed-frame contract** and **read-only synthetic VFS overlay with original-data precedence** are now committed in `NativeEngine/include/` with standalone `NativeEngine/CMakeLists.txt`. The isolated renderer/VFS CTest passed **1/1** locally in both default and Release configurations; the additional standalone CPU RGBA reference test also compiled/passed locally (normal, Release, ASan/UBSan), giving a byte-defined comparison baseline for later GPU readbacks. There are now **three registered CTests**, not yet run together on a full current native-engine checkout. An adapter to the **existing actual** `GameSource` loader and its loose-install synthetic fixture are **authored but not yet locally built against a full clone**. All repo source blobs match compiled local code. **SDL3/bgfx/SDL_GPU/Godot backends are not built or benchmarked; no user-facing native engine or Windows Full build exists yet.** This is infrastructure only, without modded gameplay or revised fidelity claim.

## Framework proof B — actual read-only mod folders

[**Framework Proof B audit**](FRAMEWORK_PROOF_B_DISK_MOD_MOUNTS_20261010.md) extends the original-source VFS boundary with **lazy on-disk read-only mod folders**, last-mounted-wins priority, collision/source provenance, case-normalized paths and protected runtime-file checks. These folder contracts passed **3/3 CTests locally in both Debug and Release**, and the disk-mount suite passed ASan/UBSan. The real `GameSource` installation + disk override adapter test was extended, but is **not yet built** in the partial local workspace; a full Git checkout will register the fourth CTest and test the inherited original loader. This is not a hostile-mod sandbox, a GPU benchmark, or a playable native engine. No actual game rules have been modified. SDL3/bgfx/SDL_GPU/Godot selection remains open.

## GPU comparison evidence contract (without real GPU results yet)

[**Framework Proof B follow-up**](FRAMEWORK_PROOF_B_DISK_MOD_MOUNTS_20261010.md) now includes an executable **fail-closed backend evidence comparator** for the future bgfx, SDL_GPU and Godot trials. It requires byte-equal RGBA GPU readback to a common CPU reference, same source/asset identities, and real measurement metadata; it does **not** fabricate GPU results or rank disparate devices. The synthetic Python verifier is registered as a CTest. **Current locally compiled partial tree: 4/4 CTests in Debug and 4/4 in Release**, excluding the original `GameSource` adapter fixture until a full Git checkout is present (then 5 registered with Python). No candidate GPU library installed/tested locally; actual renderer selection and original-install GUI build remain open.

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
