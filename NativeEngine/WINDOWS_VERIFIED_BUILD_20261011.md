# First Verified NativeEngine Windows x64 Build — 2026-10-11

**Milestone:** independently compiled Windows **bootstrap**, not the original game's native replacement.  
**Branch built:** `native-engine`  
**Exact source commit:** `1f8287f61346c1143b3e9348342a3691a487df4a`  
**Windows runner:** `windows-2022` / Visual Studio 2022 / MSVC x64 Release  
**Workflow:** [Run #38133169095, attempt 2](https://github.com/Hyperbog/RLOFT/actions/runs/38133169095)  
**Windows job:** `114452140303`, completed **success**  
**Artifact:** [RLOFT-NativeEngine-Windows-x64-BOOTSTRAP-20261011](https://github.com/Hyperbog/RLOFT/actions/runs/38133169095/artifacts/11699684083)

## Actual Windows evidence

- Windows CMake configured and MSVC compiled **both independent Windows executable targets**: `rloft_native_bootstrap.exe` (Windows GUI) and `rloft_install_probe.exe` (console media inspector).
- GUI bootstrap self-test completed on Windows, original-format loose/ISO media and Unicode-path fixtures exercised **the actual inherited GameSource**, and original RES0 cursor/static CODE.2 menu fixture tests passed.
- Complete **15/15 CTests passed** in **6.47 seconds**: original source readers, static original menu and cursor, renderer-neutral frame contract, mod mount/transaction, fail-closed simulated renderer evidence, GUI installation and synthetic ISO tests, and Windows package integrity.
- PowerShell packaging produced a ZIP; the new verifier returned `WINDOWS_BOOTSTRAP_PACKAGE_INTEGRITY_PASS`, and the GitHub artifact upload succeeded.
- Both Windows executables and the final ZIP were retrieved and independently inspected, **not merely trusted from the uploaded workflow log**. Both PE COFF machine fields are **0x8664** (AMD64); GUI subsystem 2 and inspector console subsystem 3.

| File | SHA-256 |
| --- | --- |
| `RLOFT-NativeEngine-Windows-x64-BOOTSTRAP.zip` | `82d62255f8b75e6147d39f47883e81f37651978b328e83215f06a372891f8784` |
| `rloft_native_bootstrap.exe` | `17205a69d06ac382aaaa5fd51f01d05388e30ed130c72b6eee7bb58a8f3e69c3` |
| `rloft_install_probe.exe` | `1be6072f30cd6214e108997c5f2dfa2435c2ee41218b3c26054d0ffd41aaab02` |
| GitHub outer Actions artifact ZIP | `59551006481126f14c4b6af3f8268351943af8bcf0566807eb98874d0eed1141` |

The inner source package contains **exactly five files**: the two EXEs, `README-WINDOWS.txt`, `SHA256SUMS.txt`, and `build-provenance.json`. It records `game_playable=false` and `original_assets_included=false`. Original copyrighted files and user game saves are **not** included.

## First run failure and fix

Attempt 1 genuinely built both executables, but only **13/15 tests passed**. The failures were:

1. `native_disk_mod_mount_tests` assumed case-distinct physical files `DUP` and `dup` could coexist on NTFS; on Windows the second write reused the first file. Corrected the fixture to test *canonical virtual-key collisions* unconditionally and expect physical case-only duplicate rejection only when the host can actually represent distinct files.
2. `native_windows_package_integrity_contract` could not resolve `Get-FileHash` in a Windows PowerShell CTest child. Replaced those calls with a deterministic .NET SHA-256 stream helper in all three Windows release/verification scripts, leaving all five-file allowlist, inner/outer SHA and provenance checks intact.

These changes were committed and the **same authorized build workflow's failed Windows job** was retried once. Attempt 2 compiled, passed 15/15 tests, packaged, verified and uploaded the artifact.

## Build-trigger isolation

The owner explicitly authorized the Windows build. The connected GitHub tool exposed **no `workflow_dispatch` call**, so a dedicated, isolated one-shot branch `native-engine-win64-build-20261011-0647` was created with a single narrowly filtered Windows-only push event for its own workflow-file creation. The normal `native-engine` workflow and its manual-only dispatch policy were **not changed**; no permanent push trigger was added to the main development/default branches. The one-shot workflow checked out the exact active `native-engine` tree. Attempt 2 is the verified result.

## What this milestone does NOT establish

- **No original purchased DOS/GOG/Steam installation has been exercised on the user's machine in this checkpoint.** All read-path integration tests are constructed from synthetic original-format fixtures.
- The bootstrap may display the **authentic static CODE.2 original-resource menu frame**, but it does not implement interactive menu navigation, full cold-start cinematic timing, original character creation, gameplay, original save state, or a playable game loop.
- No authentic original-game screenshot-by-screenshot DOS comparison or GPU backend benchmarking was conducted; GDI remains a temporary Windows bootstrap presenter.
- The Windows build/zip/CTest gate is accepted; **M-ENGINE-01 playable native game is not**. [M-WIN-01 #12](https://github.com/Hyperbog/RLOFT/issues/12) remains open for a user's installed-media GUI field test and first-playability blocker inventory.
- A later commit to documentation may advance `native-engine` HEAD beyond the executable's exact pinned source SHA. **Never describe a later HEAD as compiled** unless another Windows build verifies it.

## Next work

1. Have the owner extract the validated package on Windows 10/11, point the bootstrap at a **protected copy** of their original installation, and record the displayed media type, version-detection limits, whether the static menu or cursor fallback renders, and any load errors. The bootstrap is read-only.
2. Integrate source-proven original CODE.2 boot presentation/input and CODE.1 character workflow into a separately named **playable** Windows engine target. Keep DemoV1 unchanged.
3. Obtain original-game DOS reference captures and compare 320×400 indexed framebuffer/palette/timing/state, then investigate renderer extensions and mod profiles after original fidelity gates.

**Result:** The user-requested **Windows bootstrap build is delivered and verified**. Do not confuse this with shipping the complete native game.
