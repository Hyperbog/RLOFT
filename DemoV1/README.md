# Ravenloft: Strahd's Possession — native engine

**Active development branch:** `demo-v1`  
**Goal:** faithfully recreate **the complete original DOS game** as a native executable before any modernization. Level 22 is the current integration testbed, **not** the scope of the finished project.

This is the active project entry point. Historical development updates have been preserved, without deletion, in [the original full development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md). That archive contains old checkpoint statuses and must **not** be read as a current test or release report.

## Current evidence and limits (2026-10-11)

| Gate | Evidence / current limit |
| --- | --- |
| Original executable | `CODE.1` / `CODE.2` reconstructed and byte-accounted; **not exhaustively semantically verified**, and post-accounting corrections remain possible |
| Native Windows Full build | [#660](https://github.com/Hyperbog/RLOFT/actions/runs/38192487862) **245/245 CTests passed** on exact source `5b0b00e8`; Windows ZIP/EXE SHA-256 verified and committed. This is not DOS gameplay parity. |
| Current source test catalog | Run the [read-only preflight](tools/research_preflight.py) for current CMake declarations; source registration and passing Windows CTest are separate gates. |
| Level 22 viewport | Original `RES4[655]` full-open ground confirmed in 64/64 native capture frames; source-accurate partial clipping and DOS framebuffer equivalence remain pending |
| Original DOS acceptance | No accepted full indexed-frame/palette/IRQ/RNG/complete-game parity; do not infer fidelity from synthetic tests or a successful native build |
| Working score | Provisional **49.8% engineering readiness** is **not** 49.8% original-game completeness; preserve independent DOS parity gates |

The latest native source includes a read-only, provenance-checked source `22220/FF50` field projection. It does **not** yet use partial-depth calculations for gameplay rendering. Original `F790` stack-local text pre-read and callback identity have been synchronized from the verified decompilation branch; this is a **source correction**, not a passing end-to-end game test.

**October 11 build follow-up:** [Windows Full #659](FULL_WINDOWS_BUILD_CHECKPOINT_20261011_RUN659.md)
compiled but failed 2 source-only documentation assertions (243/245);
the missing `42-way` source-audit phrase was repaired. The
[user-authorized #660 rerun](FULL_WINDOWS_BUILD_CHECKPOINT_20261011_RUN660.md)
then passed **245/245**, produced a new Windows artifact, and committed its
verified executable. This closes the Windows regression checkpoint, **not**
the M-PLAY-01 normal-play field test or original DOS equivalence.

## Player field observation: prototype versus original DOS runtime

**October 11:** the owner reported that the bundled normal-trace launcher
still opens the old Level 22 native demo. This is consistent with the
source: `ravenloft_demo_v1.exe` is an incomplete C++ reconstruction
using original asset data, **not the original DOS R.EXE or original
CODE.1/CODE.2 execution**. Existing `CHARS` immediately enters its
Level 22 native engine. The field milestone M-PLAY-01 is **not accepted**.

The NEXT authorized Windows build will update the diagnostic command to
`--dev-trace --choose-install` so the source installation is selected
explicitly (instead of silently reusing a cached one). This **only
clarifies provenance**; it does not make the complete game playable.
Build #660 remains the last Windows-verified executable. Do not
represent this source-only revision as built or DOS-faithful.

## Current player field-test checkpoint

**[M-PLAY-01 — First native boot-to-blocker playability assessment](MPLAY01_FIRST_NATIVE_BOOT_TO_BLOCKER_20261010.md)** is now tracked in [GitHub issue #7](https://github.com/Hyperbog/RLOFT/issues/7). Its purpose is to test an exact-source Windows build through the **ordinary intro/menu/character creation/Level 22 path**, determine the farthest genuinely playable action, and preserve a reproducible first blocker. The existing developer-only Level 22 launcher remains a **separate diagnostic**, not proof of normal game progression. The milestone is defined, not yet field-accepted; the Windows Full #660 compilation/test gate is accepted; independent DOS parity is unchanged. No build is dispatched by creating this checkpoint.

The verified #660 Windows artifact includes
`Trace-Normal-Launch.cmd`, which starts ordinary gameplay with opt-in
non-mutating native tracing instead of the Level 22 diagnostic shortcut.
It preserves native game exit status and distinguishes failure before
exploration from a recorded field session. Existing save files must be
protected using a disposable runtime copy. Build #660 is published; normal gameplay and DOS acceptance remain open.

The [M-PLAY-01 offline trace-integrity tool](tools/audit_mplay01_trace.py)
now verifies an existing diagnostic ZIP's original indexed-byte hashes,
palettes, capture events and build provenance without unzipping or modifying
gameplay. Its synthetic tests do not replace the unaccepted current Windows
Full build or normal-path field test.

## Mandatory development methodology

1. [Original execution fidelity charter](ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) — binding goal, immutable-game source verification and **pre-commit** proof requirements.
2. [Active game integration contract](CONTINUOUS_GAME_INTEGRATION_CONTRACT_20261009.md) — up-to-date blockers, branch ownership, manual CI requirements and source-to-native gates.
3. [Subsystem readiness and acceptance](PROGRESS_SUBSYSTEM_REPORT_20261009.md) — tracked engineering readiness; keep it distinct from DOS equivalence.
4. [Preservation and integration roadmap](PRESERVATION_INTEGRATION_ROADMAP.md) — full-game scope and inter-system dependencies.
5. [Research iteration playbook](RESEARCH_ITERATION_PLAYBOOK.md) — repeatable preflight, faster original-byte oracles, safe concurrent commits and measurable process corrections.

**Source priority:** the original version-identified game binaries/files control; `Code1/Code1.cpp` and `Code2/Code2.cpp` are the primary reconstructed implementation references, never substitutes for independent executable-byte and DOS-behavior validation. Do not fill unknown behavior with invented gameplay or placeholder artwork.

## Where to work

| Area | Description |
| --- | --- |
| `include/rloft/`, `src/` | Portable native engine and original behavior adapters |
| `tests/` | C++17 standalone behavioral regressions |
| `tools/` | Offline Python original-byte/source contract tests and field probes |
| `Windows/` | Windows executable host, supporting assets and launchers |
| `../Code1/`, `../Code2/` | Reconstructed original executable code (maintained across divergent branches) |
| [Level22 nearest-depth/FF50 projection](LEVEL22_NATIVE_GROUND_22220_FF50_FIELD_PROJECTION_20261010.md) | Recent source-backed native diagnostic and remaining cache/overlay limitations |
| [F790 decompilation proof](../Code2/verify_f790_stack_local_preread.py) | Original `T[0x3F]` read-order regression; 21 original E8 sites |

## Build and verification

The project uses CMake, C++17, Python regression tools, and a Windows host. Portable checks can be built locally with an available C++ compiler:

```sh
cmake -S DemoV1 -B build/native -DCMAKE_BUILD_TYPE=Release
cmake --build build/native
ctest --test-dir build/native --output-on-failure
```

Run these from the **repository root**. The Windows GUI executable, DOS runtime fidelity, shader/pixel timing, and resource-backed gameplay require separately authorized and independently verified Windows/original-game testing. Passing `ctest` is not a DOS equivalence claim. Tests must retain active assertions under Release, as configured in the CMake suite.

**Windows Full GitHub Actions builds are manual and require explicit user authorization for each dispatch.** Committing a source change or documentation does not authorize CI execution. Original copyrighted game assets are required for game execution and should not be added to source commits.

## Research history

The [archived development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md) preserves every prior entry from the former 126 KB README, including historical, superseded test counts and failures. Independent original-byte audit and subsystem documents remain in place for reproducibility. Do not retroactively reinterpret old `certified` headings as a guarantee of semantic parity.
