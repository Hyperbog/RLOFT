# Ravenloft: Strahd's Possession — native engine

**Active development branch:** `demo-v1`  
**Goal:** faithfully recreate **the complete original DOS game** as a native executable before any modernization. The player-facing Level 22 walkaround is retired. The priority is the preserved original intro, title and character-creation sequence.

This is the active project entry point. Historical development updates have been preserved, without deletion, in [the original full development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md). That archive contains old checkpoint statuses and must **not** be read as a current test or release report.

## Part 17 — exploration preload first-blocker trace (2026-10-13)

[Source and instrumentation boundary](PART17_ALPHA_EXPLORATION_PRELOAD_TRACE_20261013.md).
The separate opt-in exploration diagnostic now starts **before** the
original Level22 initial session/world load, rather than only after its
presenters were constructed. Source-safe stage IDs distinguish initial
load failure, world-state initialization failure and presenter readiness.
The early trace pointer is detached on exceptions and ordinary launches
without `--dev-trace` remain non-instrumented. A source-only regression
guards the ordering and ten deliberate negative mutations. The isolated
field runner reports only the last whitelisted initialization checkpoint
rather than printing potentially private raw input events. The same
opt-in trace now distinguishes **first gameplay frame build, raster, and
host submission** using one-shot stage IDs; `present.submitted` is not
proof of a correctly displayed frame. **This is not a verified
current-HEAD Windows field test or original DOS parity; the release
remains Pre-Alpha.**

## Alpha → Beta → Final: release acceptance (2026-10-12)

**Current classification: Pre-Alpha.** See the
[release milestones, criteria and blocker order](RELEASE_MILESTONES_ALPHA_BETA_FINAL.md)
and the [machine-readable gate ledger](RELEASE_GATES.json).
Alpha is the **authentic opening** (CINE00/CINE12, splash/logo
screens, all title menu actions, CODE.1 creation and CHARS handoff)
through the **first playable Level22 scene and basic input**.
The full Level22 quest, its mist/Level23 transition and remaining
campaign belong to Beta. The retired Level22 developer warp is
not a release milestone. Beta is the entire original campaign
from title to ending. Final requires faithful
original DOS state, raster, time, sound, inputs and persistence.
No modernization before completing the original game.

Latest verified exact-checkout Windows Full:
[#676](https://github.com/Hyperbog/RLOFT/actions/runs/38346121349),
source `b17871c9ac2350afd0c4667de78a2aca059a5bad`,
**257/257 CTests passed**. This does not demonstrate the complete
normal original-game startup on Windows. Windows CI
passes do **not** establish native normal-play field acceptance or
original-DOS parity. The historical build/status paragraphs below
are retained as dated evidence, not automatically current.


## Isolated normal-game Alpha field test (2026-10-12)

The separate development-only CI artifact
`Ravenloft-Alpha-Field-Test-Windows-x64` provides
`Test-Alpha-Continuous-Game.cmd`. It selects a user's owned
original game installation and makes a unique **local temporary copy**
before running the ordinary `--dev-trace` path. The selected original
CHARS/SAVE.DAT files are never deleted or rewritten; a cold start may
delete CHARS **from the copy only**. A `--dev-trace` explicit temporary
installation also **does not change the saved Windows game-folder
preference**; normal non-diagnostic installation selection retains
its existing remembered-folder behavior. The copied data remains under
`%TEMP%\\RLOFT-Alpha-Field` until the tester elects to remove it.
Captured diagnostic ZIPs are local and may contain personal party names.
The opt-in `--dev-trace` field run now also creates a **separate startup
diagnostic ZIP** containing source-SHA-pinned stage IDs and native input
observations from before CINE00 through title/character creation.
**Raw key and scan codes are intentionally suppressed throughout CODE.1
character creation**, so entered actor names are not recoverable from
the diagnostic events.
This startup ZIP can exist even if the first Level22 runtime never
loads; a distinct exploration ZIP is created only after entering that
runtime. Neither is automatically uploaded or counts as DOS evidence.

**This is an opt-in Pre-Alpha field-test artifact, not an Alpha game
release and not DOS execution.** It does not restore the retired
Level22 warp or enter gameplay from the default read-only
`Play-Original-Intro.cmd` launcher. The main player-facing artifact
continues to contain only the read-only front-end showcase.

Source-only safety and packaging regressions are registered as
`alpha_sandbox_normal_launch_contract_tests`; Windows execution,
the first normal-game blocker, and original-DOS comparison remain
separate acceptance gates. See
[formal Alpha/Beta/Final criteria](RELEASE_MILESTONES_ALPHA_BETA_FINAL.md).

## Original intro / character-creation build (2026-10-11)

The owner has retired the standalone Level 22 demo as a player-facing
checkpoint. The existing code already contains the CINE00/CINE12 intros,
DreamForge/SSI sequence, title/menu animation and CODE.1 character creation.
The [verified #661 package](FRONTEND_RESTORATION_BUILD_661_20261011.md) includes `Play-Original-Intro.cmd` and uses owned original media,
forces the existing front end even when CHARS exists, and **does not modify
CHARS/SAVE.DAT or enter the incomplete Level 22 renderer**. The unfinished
title-menu options are explicitly reported as such. This is a native
front-end recreation, **not** the original DOS executable or a complete
playable game. Existing Level 22 game assets and reconstruction code are
preserved for eventual game continuity; old Level 22 demo launchers are no
longer distributed. Automatic Windows Full CI is authorized on public
`demo-v1` updates and publishes `Ravenloft-Original-Intro-Windows-x64`.

## Current evidence and limits (2026-10-11)

| Gate | Evidence / current limit |
| --- | --- |
| Original executable | `CODE.1` / `CODE.2` reconstructed and byte-accounted; **not exhaustively semantically verified**, and post-accounting corrections remain possible |
| Native Windows Full build | [#661](https://github.com/Hyperbog/RLOFT/actions/runs/38194351208) **246/246 CTests passed** on exact source `66046988`; [intro-and-character-creation artifact](FRONTEND_RESTORATION_BUILD_661_20261011.md) verified. This is not original DOS or end-to-end campaign parity. |
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

**Full Windows builds now run automatically on relevant `demo-v1` pushes**, by owner authorization after the repository became public. Passing CI remains distinct from end-to-end game fidelity. Original copyrighted game assets are required for game execution and should not be added to source commits.

## Research history

The [archived development log](DEVELOPMENT_LOG_ARCHIVE_20261010.md) preserves every prior entry from the former 126 KB README, including historical, superseded test counts and failures. Independent original-byte audit and subsystem documents remain in place for reproducibility. Do not retroactively reinterpret old `certified` headings as a guarantee of semantic parity.

## Optional shared REA research adapter

[Optional shared REA native-binary analysis](REA_ANALYSIS.md) is available through the canonical `code1-decompilation:ResearchInfra/` checkout. It is local, read-only static analysis and **does not** install REA into this application's runtime, alter original game media, or certify original DOS/gameplay parity.
