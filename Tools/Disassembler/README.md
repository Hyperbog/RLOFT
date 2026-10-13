# DOSDis — DOS Game Disassembler (research prototype)

Evidence-first DOS executable disassembly and reverse-engineering workstation. **The Windows WPF desktop is the only user-facing DOSDis application and release deliverable.** Its reusable .NET core provides separate x86-16 MZ and x86-32 LE analysis paths. A CLI remains in the repository exclusively as an **internal research/independent-validation harness**; it is not shipped or supported as a separate end-user app. Original game files are read-only, and candidate decoding is not proof of executed game behavior.

**CURRENT (2026-10-12) — CODE.1 classification benchmark / Windows #60 PACKAGE VERIFIED:** DOSDis now has an independently sourced *translated-vs-excluded byte-classification* benchmark for original Strahd CODE.1 **LE object 1 only** (70,402 bytes: 69,361 audited translated-code bytes, 1,041 audited excluded bytes across 282 ranges). A privacy-safe manifest contains original/object SHA-256 fingerprints and classification offsets, not any original proprietary bytes. The source-bound core scorer reports candidate-byte precision, translated-byte recall, excluded-code candidates and missed translation spans; the Windows WPF **Benchmark…** control imports a local manifest and opens disagreements in physical Hexadecimal view. The [automatic Windows desktop #60 build](https://github.com/Hyperbog/RLOFT/actions/runs/38343078919) compiled successfully with **zero warnings/errors**, both C# synthetic suites passed, and CI explicitly verified that the original-byte-free 282-range manifest was present in the published [Windows desktop-only ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38343078919/artifacts/11787004768) at compiled code SHA `e97b85e8e538eb1445563776b0d3876de8493dc5`. **No original-game score, independently verified instruction boundaries, WPF benchmark interaction test or complete disassembly certification is claimed.** [Benchmark source and acceptance details](docs/CODE1_CLASSIFICATION_BENCHMARK_P0_20261012.md); [current checkpoint](docs/CURRENT_CHECKPOINT.md).

**CURRENT (2026-10-11) — Automatic Windows desktop CI and first verified Enter/Find build:** The owner explicitly lifted the previous DOSDis manual-build restriction. The canonical `.github/workflows/disassembler-core.yml` now auto-builds/test/publishes on relevant pushes to `disassembler-foundation` and PRs targeting that branch; `workflow_dispatch` remains. No CLI release or proprietary game bytes are included. [Automatic Actions run #49](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386) **PASSED** Windows WPF/Core, both C# synthetic smoke suites and one self-contained desktop publish at exact code SHA `15abeee277b2729028654c0b00865bb4d0c81b74`. [Download current Windows desktop ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386/artifacts/11722213441) (ZIP SHA256 `bf88b54660579ae4bfd038cd2f8a8a8622858d3abed97809b286f614f904a2d1`). The new Enter-go/byte search, single-byte clickable Hex and cross-tab source highlights are compiled and synthetically verified; **owner Windows GUI behavior and G2 independent game-decoder accuracy are pending**. [Exact current policy/build record](docs/DESKTOP_AUTO_BUILD_WIN49_20261011.md). Earlier sections describing a manual-only workflow or an unbuilt search feature are historical and superseded by this checkpoint.

**Desktop CHECKSYS.EXE integration — WINDOWS BUILD #43 PASS, owner GUI acceptance pending:** The run #42 Windows application accepted Dungeon Hack's valid DOS MZ `CHECKSYS.EXE` only as raw bytes, disabling Analyze/Export despite existing 16-bit core support. The Windows GUI now recognizes validated standalone MZ, shows module-relative x86-16 entry disassembly, executes bounded candidate control-flow analysis, supports CS:IP navigation and source-backed JSON/text exports. MZ SQLite project snapshots/annotations remain disabled until their address schema supports CS:IP. The canonical manual-only workflow and local Windows script now build/publish **desktop only**, not a CLI package. **The new MZ desktop code compiled and passed synthetic regressions in [Windows build #43](https://github.com/Hyperbog/RLOFT/actions/runs/38098035861); owner screenshots and an original-source-matched JSON report now confirm MZ recognition, candidate analysis display and JSON export; navigation/Hex interaction and independent code accuracy remain unverified.** [Download the desktop-only preview](https://github.com/Hyperbog/RLOFT/actions/runs/38098035861/artifacts/11686312055) and see [exact Windows #43 build evidence](docs/G2_DESKTOP_MZ_WINDOWS_BUILD_20261010_RUN43.md). [Root cause, source fix and next Windows acceptance gate](docs/G2_DESKTOP_MZ_UI_HANDOFF_20261010.md). The previous cross-platform #42 build predates this GUI fix; #43 is Windows desktop-only.

**Windows CHECKSYS.EXE navigation follow-up — WINDOWS BUILD #44 PASS, real GUI test pending:** Owner testing of #43 confirmed original-file MZ Analyze/JSON Export but found inactive direct CALL double-click, Hex Go not visibly repositioning and a References tab limited to the selected instruction. The corrected WPF source now follows source-backed direct candidate call/branch edges, reports original physical file offsets while resetting Hex scroll, and shows all graph references with an optional selected-only filter. Existing resolved-reference double-click behavior is retained. [Owner-authorized Windows Actions #44](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116) **PASSED** .NET desktop/core compilation and both C# synthetic suites (0 build warnings/errors), including direct-target regression cases. [Download desktop-only #44 ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116/artifacts/11703189013). Temporary PR #13 closed without merging; manual-only canonical CI preserved. **Actual WPF mouse/Go/References interaction on original CHECKSYS and one LE regression still require owner testing.** [Full build #44 record](docs/G2_DESKTOP_NAVIGATION_WINDOWS_BUILD_20261011_RUN44.md) and [original bug/interaction checklist](docs/G2_DESKTOP_NAVIGATION_FIELD_REPORT_20261010.md).

**Windows desktop byte search and exact-source highlighting — COMPILED + SYNTHETIC VERIFIED #49, owner GUI test pending (2026-10-11):** Owner confirms the #44 navigation updates work and requests Enter-to-search, highlighting of searched source addresses/hex bytes across Disassembly, Hexadecimal and References, and single-click addressable original hex bytes (`CD`, `FF` etc.) rather than only whole-text selection. The new UI source wires Enter and Go/Find, validates bounded hex byte patterns, supports repeated next-match search, makes each original-file-backed hex cell individually clickable with a physical offset readout, and highlights the matching instruction-byte substring and actual reference rows where present. Undecoded bytes show a Hex selection, not invented code. Normal drag-to-copy remains. Synthetic byte mapping/search fixtures **passed the Windows #49 synthetic smoke suite**; real WPF mouse/keyboard interaction has **not** been owner-tested. [Technical scope and Windows acceptance checklist](docs/DESKTOP_BYTE_SEARCH_AND_HIGHLIGHT_20261011.md). The latest verified desktop preview is [#49](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386), which includes these additions.

**Resume future work here:** [Compact current checkpoint](docs/CURRENT_CHECKPOINT.md) — most recent verified build and source-only differences, actual next G2 work and original-file availability; [Continuous-improvement playbook](docs/CONTINUOUS_IMPROVEMENT_PLAYBOOK.md) — defect-to-regression loop, independent evidence safeguards, batch/build efficiency and retrospective lessons. Read these before the full historical roadmap. They do **not** authorize new builds or CI.

**Latest owner-authorized cross-platform G2 build (2026-10-10):** [Windows/Linux Actions run #42](https://github.com/Hyperbog/RLOFT/actions/runs/38096831427) **PASSED** .NET 10 Windows desktop/CLI, Linux CLI, both C# synthetic suites on both platforms and **17/17 Python synthetic tests** on Linux. All .NET builds had **zero warnings/errors**. Actual ephemeral PR-merge checkout: `dd251ba7479fe802e17155a2740d3519cf23847b`; canonical application/test source-equivalent SHA: `3956f036fee44c0b6f07361811e4f0857dbea916`. Three self-contained preview artifacts published. An initial run #41 compiled successfully but exposed a misconstructed x86-16 benchmark identity negative fixture; this was corrected at canonical `3956f036` and verified by full successful #42. The temporary verification PR #8 is **not to be merged**; canonical CI remains manual-only. [Full gate record](docs/G2_BUILD_GATE_RECORD_20261010_RUN42.md). No new original-game comparison or human G2 accuracy acceptance was performed by #42.

**Previous run #40 (historical):** [Windows/Linux Actions run #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405) passed cross-platform .NET 10 CLI, Windows WPF and both C# synthetic suites at source-equivalent SHA `7fa76f822e0aa56a4bf1c611076bc50e5e0a5389`. The independently compared Strahd original CODE.2 instruction samples below remain specifically tied to the #40 compiled binary, not automatically to #42.

**First exact-original independent x86-16 comparisons (2026-10-10):** The compiled Linux CLI was run locally against SHA-bound Strahd `CODE.2` outer DOS MZ launcher bytes (never uploaded to CI). GNU binutils `objdump` 2.44 independently supplied two nonoverlapping windows: **28/28 instructions**, **15/15 structured operand expectations** and **5/5 expected direct branches/calls** agreed, with no missing/conflicting/not-observed findings and no truncation. **8/8 local negative controls** returned their expected discrepancies or controlled error status. See [G2 original validation evidence record](docs/G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md). The other **6,512 candidate starts were not benchmarked** by these samples. Manual Windows GUI interaction, runtime reachability, broad cross-title accuracy and independent human reviewer acceptance remain **PENDING**. This result does **not** close G2 accuracy certification.

**Latest source-only G2 provenance audit (2026-10-10):** Reinspection of separately uploaded original Strahd `CODE.1` and `CODE.2` showed that, despite different full-file SHA-256 hashes, both embed an **identical 62,532-byte DOS MZ launcher** and the same `CS=01BD` 54,900-byte entry-region. The **28 original x86-16 instructions compared in run #40 belong to one shared byte-identical launcher**, not to two independent code paths. Report source now adds `mappedRegionSha256` and `benchmarkedInstructionBytesSha256` to expose reused sample evidence and avoid inflated denominators; synthetic negative fixtures were added. **These changes are now compiled and synthetically verified in run #42; no new original Strahd comparison was performed at #42.** See [the exact original-source record](docs/G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md) and [the comparison contract](docs/X86_16_EXTERNAL_ORACLE_COMPARISON.md).

**G2 multigame original-source expansion (2026-10-10):** SHA-checked four original DOS executable blobs directly from `DnD-Classics/MASTER` against the original collection inventories: Dungeon Hack `SBLASTER.COM` (explicit COM), Dungeon Hack `CHECKSYS.EXE` (MZ with 43 bounded relocation entries), and Pool of Radiance / Curse of the Azure Bonds `START.EXE` (different CS/physical entries but 283 identical source bytes at entry). These supply new independently selected **original files**, not yet new original DOSDis accuracy comparisons. A standalone read-only Python source/mapping/hash preflight and its synthetic-only fixtures were compiled where applicable / **synthetically exercised in Linux run #42**; the other-game originals have not yet been compared against DOSDis. [Full source selection, hashes, independent mapping checks and local preflight](docs/G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md). No file upload is currently needed for these four. G2 independent review and broad correctness acceptance remain pending.

**Latest source-only G2 safety and relocation-oracle work (2026-10-10):** Closed a direct-helper SHA-256 identity bypass in the independent Python DOS preflight; added an optional, privately computed MZ relocation digest at an *explicitly assumed* 16-bit load segment. DOSDis's copied MZ relocated-image summary and CLI now expose their own digest for a future cross-implementation SHA comparison. Synthetic cases cover altered SHA, modular wraparound, overlap and zero fixups. **All of these changes are synthetically verified in run #42**; source SHA and relocated-copy fingerprint work still needs an independent comparison on exact-hash original game bytes. No original-game relocation comparison or human accuracy acceptance has occurred. See [multigame original-source and relocation comparison plan](docs/G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md) and [compact checkpoint](docs/CURRENT_CHECKPOINT.md).

**Historical G1 verification note (2026-10-10):** The Windows desktop and synthetic suites passed at G1 build #35 (source SHA `9f605e36581b2c6db72a6a47248ea5af31611506`), with owner-reported desktop acceptance. The newer R2/LE/MZ analysis changes are **source-only and not yet G2-verified**. The canonical, current evidence states and gates are in [ROADMAP_BUILD_GATES.txt](docs/ROADMAP_BUILD_GATES.txt); older historical sections below reflect development at the time they were written. See also the [Strahd CODE export feedback handoff](docs/LE_STRAHD_CODE_JSON_HANDOFF_20261010.md) for a title-qualified x86-32 validation target.

The project intentionally does not certify automatic full code coverage, verified runtime control flow or complete decompilation.

## Required independent accuracy validation

**Independent validation is a binding acceptance requirement** for accuracy/format
claims at pending and future G2–G8 gates and before the final release. Passing
the project's synthetic tests or using Iced.Intel to decode instructions does
**not** independently verify DOSDis's loader, CFG, relocation, instruction
classification or function discovery. Each supported scope must be compared
at the **exact compiled commit and original binary SHA** against predeclared
outside evidence (such as a separately implemented decoder, applicable
format/ISA specifications, independently checked original-byte audits or
reproducible execution traces), with reviewed mismatches and explicit blind
spots. Strahd, Stone Prophet and Menzoberranzan share `CODE.1`/`CODE.2`
filenames but need **separate** source identities and comparison records.

Read the [normative independent validation policy](docs/INDEPENDENT_VALIDATION_POLICY.md)
and [evidence record template](docs/INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md).
The [roadmap/build gates](docs/ROADMAP_BUILD_GATES.txt) define required
acceptance per phase. The [predeclared G2 independent test scope](docs/INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md)
specifies future x86-16 oracle comparisons without claiming they have run.
An optional [external x86-16 oracle comparison interface](docs/X86_16_EXTERNAL_ORACLE_COMPARISON.md)
is source-complete for `com-analyze`, `mz-analyze` and `overlay-analyze`.
It checks original mapped bytes, flow/direct edges and optional
structured mnemonic/register/immediate facts and a restricted
16-bit ModR/M memory-address subset (at most two explicit operands),
with **separate semantic assertion denominators**. It does **not**
execute an independent decoder, model 32-bit address overrides or
implicit memory effects, verify the claimed oracle author, infer
runtime segment values, or certify G2.
This policy adds **no automatic build or test run**:
owner authorization is still required. As of 2026-10-10 the current HEAD has
**not** passed the new independent verification gate.

## Requirements

- .NET 10 SDK (see `global.json`)
- NuGet package `Iced` pinned to `1.21.0` (MIT)

## Commands

From `Tools/Disassembler`:

```sh
dotnet run --project src/DosDis.Cli -- inspect /path/to/CODE.2
dotnet run --project src/DosDis.Cli -- disasm /path/to/CODE.2 --object 1 --offset 0x2975C --count 16
dotnet run --project tests/DosDis.Smoke
dotnet run --project tests/DosDis.Smoke -- /path/to/CODE.1 /path/to/CODE.2
```

**Windows-only, manual CI:** Only `workflow_dispatch` starts the Windows x64 job. Pushes and pull requests do not trigger the disassembler build. No Linux jobs or releases are produced. These are experimental CLI previews, not production certifications.

`inspect` emits JSON with immutable source SHA-256, LE module/object/page mappings, entry point, and evidence-tagged File Intelligence fields. `disasm` emits structured instruction records (original bytes, object offset, assembly, flow-control category).

Input is limited to 128 MiB in this prototype as an explicit safety bound. The stored image is copied into memory and never written back. Use an isolated authorized copy of any proprietary original game files; do not add them to this repository.

## Scope and intentionally unsupported features

- Format probe accepts an outer MZ with exactly one validated nested-or-top-level LE module.
- Object/page parser supports ordinary file-backed LE pages (`flags=0`) and represents unfurnished virtual tails as **unmapped**. Iterated/compressed/invalid page types fail closed.
- Relocation **records are not yet decoded or applied**; instruction bytes are original file-backed bytes, not a simulated relocated image.
- Initial assembly decode targets x86 **32-bit LE executable objects** only. The broader 16-bit DOS path needs a dedicated real-mode/segmented loader and execution context.
- CLI listing is *linear disassembly of a researcher-selected address*, not a verified automatic code/data or function discovery pass. It must not be treated as proof that selected bytes were executable.
- File Intelligence fields are identified by full SHA-256 where available; known embedded extender strings are reported as strings, not a proved minimum DOS version.
- The original Part 14 baseline had no SQLite project, WPF interface or CFG. Parts 16–18 add candidate CFG, reference recovery, a SQLite source implementation and an initial WPF source shell. VM decoding, Ghidra import, and binary comparison remain future milestones.

## Evidence boundary

The repository `Runtime/CODE_OBJECT_MAP.md`, `Code1/FINAL_BINARY_AUDIT.md`, `Code2/FINAL_CERTIFICATION.md`, and the immutable original files control the Ravenloft fixtures.

Verified baseline constraints:

| Field | CODE.1 | CODE.2 |
|---|---:|---:|
| File bytes | 277787 | 558689 |
| Nested MZ start | `0x25D24` | `0x25D24` |
| LE header | `0x27BDC` | `0x27BDC` |
| Object 1 start | `0x2C724` | `0x3AF24` |
| Object 1 virtual length | `0x11302` | `0x3A852` |
| Entry object/offset | `1:0xCC1C` | `1:0x353A0` |

The RLOFT original CODE files **must not be committed** to source control or public release artifacts.

## Development and verification

- `tests/DosDis.Smoke` generates small unencumbered synthetic executable fixtures. Public CI executes those fixtures.
- Optional original-binary tests are activated by supplying original CODE.1 and CODE.2 paths locally.
- The entire Part 14 implementation is isolated under `Tools/Disassembler`; its CI workflow is under `.github/workflows/disassembler-core.yml`.
- Builds are reproducible only after dependency resolution and lock-file creation in the actual .NET build environment. The prototype currently pins the sole third-party package by exact version; a NuGet lock file and package auditing are future gates.

See [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) for contracts and remaining acceptance work.

## Part 15: internal LE relocations and loaded-memory view

The loader now parses supported internal 32-bit LE relocation records, including source lists, additive offsets and negative source offsets crossing page boundaries. Original file bytes remain untouched. `fixups` enumerates records and `memory` shows the reconstructed memory view. `disasm --relocated` is an explicit opt-in; ordinary `disasm` still shows original bytes.

```sh
dotnet run --project src/DosDis.Cli -- fixups /path/to/CODE.2 --max 10
dotnet run --project src/DosDis.Cli -- memory /path/to/CODE.2 --object 1 --offset 0xBA7 --count 4
dotnet run --project src/DosDis.Cli -- disasm /path/to/CODE.2 --object 1 --offset 0x2975C --count 16 --relocated
```

**Scope limitation:** This milestone is not a universal LE relocation engine. Unsupported imports, selectors, far-pointer fixups, chaining and nonstandard pages fail explicitly. See [docs/LE_FIXUP_CHECKPOINT.md](docs/LE_FIXUP_CHECKPOINT.md).

## Part 16: recursive control-flow discovery (candidate evidence)

The first recursive 32-bit x86 control-flow analyzer is implemented. It follows original LE entry points or explicitly requested seeds; emits instructions, basic blocks, direct call/jump/conditional branches, unresolved indirect edges, and candidate functions; and retains original-file-backed gaps as unknown rather than forcing code/data classifications. Analyst-supplied protected data ranges prevent accidental speculative decoding. Shared return blocks can belong to multiple candidate functions.

```sh
dotnet run --project src/DosDis.Cli -- analyze /path/to/CODE.2 --seed 1:0x2975C --max-instructions 3000 --max-output 100
dotnet run --project src/DosDis.Cli -- analyze /path/to/CODE.2 --max-output 100
dotnet run --project src/DosDis.Cli -- analyze /path/to/CODE.2 --seed 1:0x2975C --data 1:0x3A7F8:0x3A7FE
```

`analyze` produces JSON with provenance, candidate instruction bytes, typed flow edges, basic blocks, function candidates, classification ranges, and diagnostics. `--relocated` opts into decoding relocated memory while preserving original instruction bytes. This is **bounded candidate discovery, not a certified automatic complete disassembly**. The tool does not yet resolve jump tables, far transfers, 16-bit code, or full code/data ownership. See [docs/CFG_ANALYSIS_CHECKPOINT.md](docs/CFG_ANALYSIS_CHECKPOINT.md).

Earlier sections of this README describe the Part 14 baseline; Part 15 and Part 16 sections supersede baseline statements that a feature was not yet present.

### Part 16 original-game checkpoint

The earlier Part 16 evidence archive includes an authorized original-binary run from before the Windows-only CI decision. The original files were not uploaded to CI. Entry-seeded static analysis identified 10,997 candidate CODE.1 instructions and 41,720 candidate CODE.2 instructions, with no instruction-budget truncation at 100,000. The known CODE.2 shared-return test passed. These counts **are not complete original-game instruction inventories** and are not behavior-fidelity percentages. See [Part 16 checkpoint](docs/CFG_ANALYSIS_CHECKPOINT.md).

## Part 17: indirect control-flow and data cross-references

The core now recognizes LE relocation-backed absolute indirect calls/jumps and indexed 32-bit pointer tables (at least 2 consecutive valid entries; max 128), plus object-relative xrefs from absolute memory operands and source fixups embedded in code. All findings are **candidates**. Ambiguous unbounded indirect control flow remains unresolved; candidate-only instructions carry separate provenance labels. `analyze` JSON contains `DataReferences` and `JumpTables`. See [Part 17 details](docs/INDIRECT_REFERENCE_CHECKPOINT.md).

**AI assistance remains deferred until after the finished application's final release and explicit authorization.** No model or provider code is added in this milestone. See the repository's archival AI research document for non-active future reference.

## Part 18: SQLite research projects and Windows desktop source preview

The first Windows-only WPF source shell and versioned SQLite `.ddproj` project storage have been added. Save candidate CFG snapshots and analyst comments without altering original bytes; reopen requires matching original artifact SHA-256. See [Part 18 implementation note](docs/PERSISTENCE_AND_WINDOWS_DESKTOP.md) for limitations and verification instructions. **These additions have not yet been compiled in a Windows environment.**

**GitHub Actions is now opt-in, `workflow_dispatch` only.** No push/PR triggers; do not trigger hosted CI unless explicitly requested. Use `build-windows.ps1` for manual local builds and optional authorized Ravenloft binary tests. No AI work before final release.

## Part 19: Windows UI/project consistency fixes (source checkpoint)

Part 19 fixes loss of the analyzed assembly listing when navigating to an out-of-list address: a bounded, **unverified** supplemental preview now coexists with known candidate rows. Opening a project with no saved graph clears any previous project's graph and rows. The desktop offers an explicit **Backup project** action using SQLite's transactional backup API, refusing existing backup destinations and leaving the original file untouched. Project smoke tests now cover a WAL-consistent backup, schema mismatch rejection, and optional CODE.1/CODE.2 bounded analysis save/reopen checks.

Build and synthetic tests are included in [the manual Windows build script](build-windows.ps1). `.github/workflows/disassembler-core.yml` remains `workflow_dispatch` **only** but, when explicitly authorized, now builds the WPF desktop and exercises the persistence tests as well as the CLI. **No CI was run for Part 19; no Windows executable has been verified for this source checkpoint.** See [Part 19 acceptance gate](docs/WINDOWS_DESKTOP_VERIFICATION_GATE.md) and do not distribute the WPF preview as production-ready before a real Windows 11 build and test.

## Part 20: cancellable analysis and project restore validation

The core recursive analyzer now accepts an optional `CancellationToken`. The Windows WPF source exposes **Cancel analysis** and blocks shutdown until active file/database operations finish. Cancelled traversal does not publish partial findings. The SQLite project loader verifies its stored instruction count and truncation marker against the decoded payload; a mismatch is rejected rather than silently displayed as reliable research.

**Overall completion estimate: 30% toward the full first final Windows release.** This is a planning estimate, not original-game behavior parity or code coverage. The Part 20 C# additions have **not yet been compiled or run on Windows**; only offline structural checks were performed. See [Part 20 verification checkpoint](docs/PART20_CHECKPOINT.md). The build and all GitHub Actions remain manual-only; no AI integration is planned before final release.

## Part 21: local Windows release gate and backup publication safety

The first WPF desktop executable is **still awaiting a real Windows 11 .NET 10 build**. This source checkpoint makes SQLite backups atomic (verify private staging copy, then non-overwriting same-volume rename) and preserves the previous visible CFG if a new analysis cannot be saved. The Windows local build helper accepts optional `-LocalLogDirectory 'C:\DOSDisLogs'` to capture a private PowerShell build transcript. **Review logs for personal file paths before sharing.** No hosted Actions run or Linux build is needed.

Part 21 adds regression assertions against backup staging residue and accidental overwrite. It does not implement crash reporting, session replay or the analysis debugger; those remain scheduled for later. The overall **30% estimated final-release completion** is unchanged until compilation/runtime acceptance gates pass.

## Part 22: Windows GUI reference fixes and local package creation

This source-only checkpoint repairs the WPF reference viewer's incorrect `FlowEdge.Target` property access (correct: `FlowEdge.Destination`), indexes references for fast navigation, and enables double-click navigation on resolved references. The manual-only Windows workflow no longer duplicates desktop builds/tests/publication. `build-windows.ps1 -Publish` produces `dist/DOSDis-Windows11-x64-preview.zip` plus its SHA-256 manifest after a successful local Windows build. See [Part 22 verification note](docs/PART22_CHECKPOINT.md).

**No successful Windows GUI compilation is claimed yet.** Estimated completion remains **30%**. No automatic CI, Linux builds, or AI integration.

## Part 23: source-backed verification of restored instruction evidence

SQLite snapshots are now cross-checked against the opened SHA-256-matched original `LeImage` before the desktop displays restored analysis. The verifier checks object bounds, executable-object attribution, unique instruction starts, stored-byte count and original bytes. This does **not** upgrade candidate instructions to verified executable code; it only verifies their **source bytes**. The synthetic project suite now tests valid original-backed restore and rejects tampered cached machine bytes and duplicate starts. See [Part 23](docs/PART23_CHECKPOINT.md).

**Windows GUI compilation/runtime testing is still pending; overall first-release completion remains 30%.** Builds and GitHub Actions remain Windows-only and manual-only; AI is deferred until post-final release. No additional diagnostic/debugger feature has been implemented in Part 23.

## Part 24: first successful Windows build (verified CI)

The .NET 10 WPF desktop, CLI, core, persistence and synthetic regression projects have **compiled and passed both test suites on GitHub's Windows runner**. The successful desktop and CLI preview archives are available from [Actions run 37841978786](https://github.com/Hyperbog/RLOFT/actions/runs/37841978786). The WPF desktop is **not yet interactively tested** on Windows 11 and the CODE.1/CODE.2 proprietary binaries were excluded from CI. The Windows workflow has been returned to `workflow_dispatch` only. Updated first-final-release planning progress: **35%** (was 30%). See [Part 24 checkpoint](docs/PART24_CHECKPOINT.md).

## Part 25: offline evidence-preserving analysis export (source preview)

The WPF workspace now has a user-initiated **Export…** action to write a candidate analysis report in JSON or text format. The CLI has a `report` command with explicit `--out` and optional `--include-bytes`. The schema-versioned output retains artifact SHA-256, object-relative addresses, uncertainty/provenance, control-flow/diagnostic details and candidate tables, but omits the full executable, original file paths, project comments and machine-byte excerpts by default. Byte excerpts require an explicit opt-in; inspect free-text provenance before sharing. Exports cannot silently overwrite existing reports. See [Part 25 checkpoint](docs/PART25_CHECKPOINT.md).

**Part 25 has passed offline structural checks only; no new Windows compilation or GUI runtime test has been run.** GitHub Actions remains `workflow_dispatch` only. The first-final-release estimate stays at **35%** until Windows verification.

## Part 26: first live CODE.1 Windows GUI review and dark-theme corrections

The project owner's real Windows screenshot confirms the Part 24 WPF executable **launches and displays original CODE.1** metadata, candidate disassembly and function listings. The screenshot does not verify interactive project save/reopen, correctness of all candidate code classifications or other navigation features.

Review identified native WPF controls rendering bright white in the otherwise dark interface (disabled actions, tabs and table headers), low-density contrast and hard-to-read fixed-width File Intelligence values. The source now defines explicit dark control templates/states, larger row/column typography, dynamic metadata column sizing and evidence-detail/tooltip behavior while preserving the existing event wiring, column resize grippers, virtualization and original binary byte handling. **No original-game analysis logic changed.**

**Overall first-final-release planning progress is 37% (was 35%)**, credited to verified Windows GUI startup and CODE.1 listing, not to uncompiled theme work. The Part 26 visual improvements have **not yet been rebuilt or user-tested**; the Part 24 Windows artifact still has the earlier UI. See [Part 26 UI review](docs/PART26_WINDOWS_GUI_REVIEW.md). CI remains manual-only and Windows-only, and AI is postponed until after final release.

## Part 27: verified Windows build of theme and exports

The Part 26 dark-theme revisions and Part 25 analysis-report exports have now **compiled successfully on a Windows .NET 10 runner**. Core and SQLite synthetic regression tests passed, including candidate JSON/text report export and machine-byte opt-in safeguards. Windows desktop and CLI x64 ZIP packages were published by [successful run 37855935875](https://github.com/Hyperbog/RLOFT/actions/runs/37855935875). The temporary one-time build trigger was removed and GitHub Actions is manual-only again. **Visual acceptance of this newer executable, actual CODE.1/CODE.2 interactive behavior and GUI project workflows remain pending.** First-release estimated completion: **39%** (up from 37%). See [Part 27 test and build checkpoint](docs/PART27_WINDOWS_BUILD_CHECKPOINT.md).

## Part 28: keep selected tab and inspect arbitrary raw files (source preview)

Address navigation from the function Explorer now preserves the active Disassembly, Hexadecimal, or References tab. Unsupported file formats (including game resources) open in a bounded, **read-only physical-file-offset hex view** instead of appearing blank. Use `file:0xOFFSET` to seek in raw source bytes; `object:0xOFFSET` remains for recognized LE object mapping. File Intelligence explicitly marks when code analysis is unavailable. This is **not** a general resource parser or 16-bit MZ disassembler. Synthetic smoke source covers raw bytes and EOF bounds, and no original game bytes are changed. See [Part 28 checkpoint](docs/PART28_TAB_AND_RAW_HEX_CHECKPOINT.md).

**Status: code committed but not yet rebuilt on Windows**, pending UI validation; first-release progress **39%** (unchanged). GitHub Actions is manual-only; AI remains post-final-release.

## Part 29: verified Windows build of raw-file hex viewer

The Part 28 navigation fix and raw-file hexadecimal fallback have now **compiled and passed both synthetic .NET 10 suites on Windows**: [successful GitHub run 37858020058](https://github.com/Hyperbog/RLOFT/actions/runs/37858020058). Self-contained Windows desktop and CLI preview artifacts were published and integrity-checked. The new UI still requires a real Windows 11 interaction test: confirm Explorer function selection retains the current Hexadecimal/References tab and arbitrary DAT files open to `file+0x...` physical bytes. The workflow was immediately reverted to `workflow_dispatch` only and the temporary trigger removed. Completion estimate: **40%** overall (was 39%). See [Part 29 verification note](docs/PART29_WINDOWS_BUILD_AND_RAW_INSPECTION.md).

## Part 30: private, bounded diagnostic session capture (source checkpoint)

The initial diagnostic roadmap now has source-level implementation: a **bounded per-process event ring buffer**, synthetic privacy/report tests, an opt-in WPF **Diagnostics…** JSON export action, and best-effort local incident reports for unhandled .NET failures. The event schema stores controlled stage names, timestamps, source SHA-256 and numeric context; it **does not collect original bytes, analyst comments, local paths or exception messages**. This first iteration is an *operation history*, not a decoder-decision debugger or replay. See [Part 30 checkpoint](docs/PART30_DIAGNOSTIC_SESSION_CHECKPOINT.md).

**Part 30 source is not yet Windows-compiled**. The user confirmed Part 29's tab-state/raw-file issues resolved; estimated first-final-release completion **41%** (40% previously, no credit for unbuilt Part 30 code). GitHub Actions remains manual-only, Windows-only, and AI remains postponed until after final release.

## Part 31: Windows-verified diagnostic session reports

The Part 30 bounded operation history, opt-in `Diagnostics…` JSON export and best-effort local incident hooks have now compiled on the Windows .NET 10 runner and **passed both synthetic regression suites**: [successful build 37861762919](https://github.com/Hyperbog/RLOFT/actions/runs/37861762919). Published desktop and CLI artifacts were downloaded and their ZIP contents verified. **Interactive diagnostic-button behavior and actual crash incident capture are still untested on Windows**. The temporary one-time workflow trigger was removed and Actions remains strictly manual-only. Estimated first-final-release completion: **43% overall**, increased from 41% for verified testing. See [Part 31 checkpoint](docs/PART31_WINDOWS_DIAGNOSTIC_VERIFICATION.md).

## Part 32: bounded evidence-linked analyzer decisions (source checkpoint)

The optional `AnalysisDecisionTrace` records typed, bounded analyzer decisions, addresses and candidate provenance without original bytes, local paths or private comments. **Not yet Windows-built**; the completion estimate stays **43%**. See [Part 32](docs/PART32_ANALYZER_DECISION_TRACE.md). GitHub Actions remains manual-only.

Part 32 source audit: branch edge trace outcomes now distinguish fresh versus previously seen targets and non-executable versus unbacked virtual destinations. Invalid seed/range requests no longer consume the single-use trace. Static review only; no build was requested or run.

## Part 33: opt-in offline analyzer trace export (source checkpoint)

The CLI `analyze` command can now request a bounded decision trace as a
**separate local JSON file**, without embedding original game bytes or analyst
comments. This is source-only, not a verified build:

```sh
dotnet run --project src/DosDis.Cli -- analyze /path/to/CODE.2 --trace-out decisions.json --trace-capacity 1024
```

Default capacity is 512 events (range 8–4096); older records are dropped with
an explicit count. The export is limited to 8 MiB, uses same-directory staging,
and refuses to replace an existing file. Without `--trace-out`, analysis does
not allocate a decision trace and stdout retains its existing JSON output.
The new CLI only exposes static-analysis decisions, not live game execution.
See [Part 33 source checkpoint](docs/PART33_TRACE_EXPORT_SOURCE.md).
**No build or test execution has been requested or performed.**
First-release completion estimate stays at **43%**.

## P0: guarded literal jump-table candidates (source checkpoint)

The CFG analyzer can now infer a **candidate** contiguous literal-dword jump
table when an immediately adjacent unsigned `cmp index,N; ja default;
jmp [index*4+absolute]` sequence provides a bounded entry count and **every**
pointer uniquely maps to backed executable LE code. LE relocation-backed
tables remain preferred. Literal entries are labeled
`no-relocation-proof` and the unresolved runtime edge is preserved.
Synthetic seven-entry positive and malformed/signed-guard negative cases are
added; no original Menzo executable was verified here. Source-only:
**no build or tests run**, and completion remains **43%**.

## Stone Prophet regression intake — P0 control-flow correctness (source checkpoint)

Stone Prophet `CODE.1` export measurements supplied by the analyst: 13,615
candidate instructions, 42 unresolved indirect calls, five unresolved indirect
jumps, zero recovered tables, zero diagnostic entries; a 23,761-byte unexplored
interval in executable object 1 was also reported. These are **external report
claims**; original Stone bytes have not been independently rechecked here.
The source audit confirms current recovery excludes `cs:`-overridden indexed
jumps and computed register-indexed calls. They remain P0.

Unresolved indirect calls/jumps are now surfaced as bounded CFG diagnostics
and counted explicitly in JSON, text and CLI summary exports even when some
candidate destinations were found. Additive fields do not change the
`candidate-unverified` analysis classification. New synthetic assertions
were added but **not run**; no build was requested. See
[Stone/Strahd intake and gap priorities](docs/P0_STONE_CROSS_GAME_REGRESSION_INTAKE.md).
First-release estimate remains **43%**.

## Strahd CODE.1 three-game regression intake (source checkpoint)

The supplied Strahd `CODE.1` candidate export reports 16,711 instructions,
331 function candidates, 1 relocation-backed **indirect-call table**, 19
unresolved indirect calls, five unresolved jumps, and one rejected overlapping
decode at `1:0xB04B`. These are exported candidate counts, not independently
certified executable coverage. The identity hash matches the separately
archived Strahd CODE.1 binary, which has a **70,402-byte object-1** audit with
**69,361 verified instruction bytes and 1,041 proven non-code bytes**.

The former `jumpTableCandidates` serialized list now preserves explicit
`DispatchKind` labels for indirect CALL versus JMP (legacy name retained for
readers of older reports); JSON counts distinguish both. Overlap diagnostics
now carry proposed and retained assembly, rejection reason, and unresolved
status when an alternative decode is rejected. Both have new synthetic checks,
**not compiled or run**. See [Strahd/Stone/Menzo baseline](docs/P0_THREE_GAME_VALIDATION_BASELINE.md).
Original Strahd research is a separate reference, never automatically promoted
to Menzo/Stone facts. Overall estimate remains **43%**, Windows only, no
build requested.

## P0: first source-bound known-answer validator (source checkpoint)

The independent Strahd CODE.1 audit is represented in a **data-only,
source-hash-gated** manifest at
`tests/fixtures/strahd-code1-known-answer-v1.json`. It contains no original
game bytes other than a single separately documented opcode expectation.
`KnownAnswerValidator` compares explicitly documented instruction entries,
non-code intervals, direct edges and LE fixups against a selected original
and the candidate graph; it reports `match`, `conflict`, `missing`,
`not-observed`, and `not-classified` rather than silently teaching
Strahd's verified classifications to the analyzer.

Example local CLI invocation (source only; **not compiled or tested**):

```powershell
DosDis.Cli.exe analyze "C:\\Games\\Strahd\\CODE.1" --known-answer tests/fixtures/strahd-code1-known-answer-v1.json --validation-out strahd-validation.json
```

The validator requires the **full** SHA-256 and byte length to match;
Menzo/Stone are rejected when matched against the Strahd fixture. The manifest
does not speculate about an unavailable original archive/internal path.
The comparison result is local, machine-readable and non-overwriting, with no
game-specific source names copied into another executable. An incomplete
manifest does not establish negative/exhaustive instruction coverage.
Version-to-version diff and comprehensive independently checked function
boundaries remain future work. Overall progress remains **43%**.

## P0 provenance: game / SHA-256 namespace (source checkpoint)

All new candidate JSON and text reports include a source-provenance record.
CLI `analyze` and `report` accept `--game-id`, `--module`,
`--source-archive` and `--internal-path`. Local absolute paths are
**never** captured automatically. The exact original SHA-256 remains the
identity anchor; a game label supplied for an unrecognized hash stays
`analyst-declared-game-unverified`. Verified Strahd CODE.1/2 fingerprints
reject contradictory game names. An absent game ID is `unassigned`.
Research namespaces contain **both** game ID and complete source SHA:
`game-id/64-character-sha`. Provenance on saved graph-only reports marks
unknown source length honestly. No cross-game routine names are inferred.
See [P0 provenance checkpoint](docs/P0_PROVENANCE_NAMESPACES.md).
No build requested; static source only.

## P0 executable object gap ledger (source checkpoint)

`ExecutableGapAnalysis` now accounts for every executable LE object byte as
candidate code, analyst-protected data, unclassified file-backed bytes or
unbacked virtual memory. Ranked unknown spans link to original file offsets,
relocation sources and targets, incoming candidate edges, data references and
analyst seeds. Each gap stays **unknown**, even when references are present.
Output is bounded and optional; JSON export refuses overwrite.
See [P0 gap ledger](docs/P0_EXECUTABLE_GAP_LEDGER.md).
No builds or test execution requested; completion estimate remains 43%.

## Expanded P0 source-only batch — provenance, gap recovery, CS dispatch and namespaces

The branch now contains independent game/SHA-256 provenance, bounded gap
analysis with CLI and WPF report integration, a conservative CS-relative
indexed dispatch path, and SQLite V2 project symbol namespaces with migration
logic. The Strahd known-answer fixture now contains **282 verified non-code
ranges totaling 1,041 bytes**, plus the independently documented callback at
`1:0xFC97`. A byte-accounting audit verified these range lengths against
`Code1/RESIDUAL_LEDGER.md`; the software regression suite has **not** run.
See [expanded P0 checkpoint](docs/P0_EXPANDED_SOURCE_CHECKPOINT.md).
**Completion stays 43%** until Windows compilation, synthetic regression
execution and original-binary comparisons are expressly requested.

## Format-generalization checkpoint (source-only)

`BinaryFormatDetector` now reports MZ, LE/LX/NE/PE wrapper signatures,
FBOV/TPOV overlay signatures, AESOP/16, GFFI, and unknown/headerless data.
These are *detections*, not silently validated executable mappings. The new
`DosMzImage` validates standalone MZ headers, relocation positions and
original-file offsets; its explicit x86-16 linear decode returns only
candidate instructions. `mz` and `mz-disasm` CLI modes expose these
facts separately from the current 32-bit LE analyzer.

`FileIntelligence` no longer embeds any game-specific source fingerprints.
Use the optional `--identity-catalog catalog.json` for external SHA-256
and size-bound file identification. The catalog never changes code discovery.
See [format-support matrix](docs/FORMAT_SUPPORT_MATRIX.md) and
[architecture contract](docs/GAME_AGNOSTIC_ARCHITECTURE_CONTRACT.md).
No builds, test execution or proprietary original-file regressions were
performed. First-release completion remains **43% pending verification**.

## Explicit headerless 16-bit COM mode (source-only checkpoint)

`DosComImage` offers a manual, one-segment load origin (default `0x100`)
for `com` and `com-disasm` CLI operations. Normal `inspect` does not
silently accept a `.COM` suffix as a confirmed executable format.
MZ and COM use the same original-byte-backed `X86RealModeDecoder`.
No PSP bytes, runtime relocation, executable reachability, or driver
interface semantics are fabricated.

Examples for a future **manual** Windows build:

```powershell
DosDis.Cli.exe inspect \"SOUND.COM\"
DosDis.Cli.exe com \"SOUND.COM\" --origin 100
DosDis.Cli.exe com-disasm \"SOUND.COM\" --origin 100 --offset 100 --count 32
DosDis.Cli.exe mz \"MAIN.EXE\" --max 100
DosDis.Cli.exe mz-disasm \"MAIN.EXE\" --count 32
```

All offset/origin parameters shown are hexadecimal. These commands are
source implementation examples, **not verified executable results**.
The original-file loader policy and signature detection remain format-driven.

## Paired FBOV/TPOV resident directory recovery (source-only checkpoint)

The `overlay` CLI command inspects an FBOV/TPOV envelope without decoding
container bytes. `overlay-map FILE.OVR --resident PROGRAM.EXE` now attempts
a **strictly unambiguous, gap-free Turbo Pascal-style overlay directory**
using trap descriptors in a separately supplied standalone MZ executable.
It returns original EXE descriptor offsets, source OVR code/fixup byte extents,
procedure-trap counts, and **two independent SHA-256 identities**. This
recovery is limited to TPOV and 8-byte FBOV envelopes; the distinct 16-byte
FBOV variant is not interpreted using the Turbo Pascal descriptor layout.

The existing `overlay-disasm --regions manifest.json --region ID` requires a
separately authored, source-hash-bound region map; it returns only
unverified 16-bit instruction candidates. The directory parser **does not**
automatically promote its code extents to verified functions or infer runtime
segment locations. No Gold Box game receives any special-case logic.

See [overlay envelope contract](docs/P0_FBOV_TPOV_OVERLAY_ENVELOPES.md) and
[resident overlay directory recovery](docs/P0_TP_RESIDENT_OVERLAY_DIRECTORY.md).
**No builds or tests were run.** Progress remains 43% pending verification.

## P0 generic 16-bit recursive control flow and overlay storage audit

The source now contains `Dos16ControlFlowAnalysis`, operating identically
on bounded `Dos16CodeRegion` hypotheses produced by validated DOS MZ,
analyst-selected COM, or a source-bound overlay code-region manifest.
Near CALL/JMP/conditional transfers receive candidate edges; far/indirect
calls and jumps, interrupts, rejected overlapping decodes and out-of-region
targets retain explicit unresolved evidence. The ledger separately counts
decoded and unknown file-backed bytes. The analyzer **never assigns a game-
specific function, callback address, runtime segment or relocation**.

The new `DosOverlayFixupAudit` verifies paired overlay directory code/fixup
extents, reports SHA-256 fingerprints of each stored unit's code and **opaque**
relocation payload, and distinguishes the paired EXE and OVR identities.
It does **not** decode fixup semantics or feed fixup blobs to x86.

```powershell
DosDis.Cli.exe mz-analyze MAIN.EXE --max-instructions 20000
DosDis.Cli.exe com-analyze SOUND.COM --origin 100 --entry 100
DosDis.Cli.exe overlay-analyze GAME.OVR --regions map.json --region entry
DosDis.Cli.exe overlay-fixups GAME.OVR --resident START.EXE
```

See [16-bit CFG](docs/P0_X86_16_RECURSIVE_CFG_SOURCE.md),
[overlay fixup audit](docs/P0_OVERLAY_FIXUP_STORAGE_AUDIT.md) and the
[format matrix](docs/FORMAT_SUPPORT_MATRIX.md). These are source-only CLI
examples. **Compilation, smoke tests and original DOS file regressions have
not been run.** Build execution remains user-requested only.

## P0 source checkpoint: DOS MZ relocation and far candidate traversal

The new `DosMzRebasedView` models DOS MZ segment-word fixups on a **copy**
of original file-backed bytes, in exact relocation-table order. The caller
supplies an assumed load segment; each write retains original/pre-write/
post-write values, source-file offsets and duplicate/overlap evidence.
Source bytes and runtime placement are never modified or inferred.

`DosMzFarTargetRecovery` annotates candidate far `CALL`/`JMP` destinations
only when their unprefixed 16:16 immediate selector has **exactly one
resident MZ relocation** and a nonwrapped, file-backed destination under
the assumed load segment. The ordinary CFG's unresolved far edge is retained.
`DosMzCrossSegmentAnalysis` follows those candidates in independent,
bounded x86-16 region graphs and records conflicting interpretations of
shared physical file bytes rather than automatically merging code aliases.

```powershell
DosDis.Cli.exe mz-relocate MAIN.EXE --load-segment 2000 --max 100
DosDis.Cli.exe mz-analyze MAIN.EXE --load-segment 2000
DosDis.Cli.exe mz-cross MAIN.EXE --load-segment 2000 --max-entries 16 --per-entry-instructions 1024
```

The hexadecimal load segment is a **simulation assumption**, not a
measured DOS runtime address. No DOS PSP, self-modifying code, overlay
manager or far-indirect pointer resolution is implied. All source remains
game-agnostic. See [MZ relocation model](docs/P0_MZ_RELOCATION_SIMULATION.md)
and [cross-segment candidate review](docs/P0_MZ_CROSS_SEGMENT_CANDIDATES.md).
Synthetic tests have been added but **not executed**; compilation remains
manual-only by user request.

## P0 16-bit indirect control-flow evidence (source only)

`Dos16IndirectPointerRecovery` now reports source-backed candidate targets
for a narrow, explicit-CS direct memory-pointer CALL/JMP family.
`Dos16DispatchTableRecovery` additionally validates bounded CS-indexed
near/far pointer tables supplied in a **full-SHA-256-bound research
manifest**. Far candidates require separate MZ relocation and
assumed-load-segment evidence; index bounds and runtime table values are
never inferred. Neither mechanism replaces unresolved CFG edges or
automatically seeds code. The CLI options `--indirect` and
`--table-map research.json` are available for MZ, explicit COM and
analyst-mapped overlay analysis. See
[indirect recovery constraints](docs/P0_X86_16_INDIRECT_RECOVERY.md).
No builds or tests have run; the release workflow remains manual-only.

## P0 source-only: automatic unsigned BX table bounds

`Dos16AutomaticDispatchRecovery` now recognizes a **narrow**
unsigned `CMP -> JA/JAE -> optional MOV BX,r16 -> SHL BX,stride ->
CS:[BX+positive_disp16]` chain when all original bytes and locally
unique CFG predecessor edges corroborate it. It supports near-pointer
stride 2 and FAR-pointer stride 4 and validates **all** source-backed
entries using the independent table verifier; FAR entries still require
MZ selector fixups and an explicitly assumed load segment. A rejected
entry or code/data overlap exposes no partial table. No decoded target
is added to the CFG and runtime mutation remains unknown.

The CLI has opt-in `--auto-tables` for `mz-analyze`, `com-analyze`
and `overlay-analyze`. This is a small, source-only register/bounds
propagation pass, not a claim of complete indirect-flow recovery.
See [automatic guard requirements](docs/P0_X86_16_AUTOMATIC_GUARDS.md).
**Manual-only builds; synthetic tests written but not run.**

## Nested MZ/LE detection from original-byte evidence (source-only)

Signature-first detection now recognizes bounded MZ-bound extended
headers, including nested LE modules behind DOS launcher stubs, and
reports ambiguous multiple modules. A signature match does not validate
LE pages or grant permission for standalone 16-bit MZ code recovery.
Both original Strahd CODE files supplied to this Project reproduce
the nested-header geometry. Tests were extended without being run.
See [nested format checkpoint](docs/P0_NESTED_MZ_ORIGINAL_EVIDENCE.md).

## P0 source checkpoint — shared extended module scanner and DOS launcher

`MzExtendedModuleLocator` now drives both extended format detection
and the LE loader's unambiguous module selection. `inspect` exposes
all recognized MZ-bound nested candidate modules by original-file
offset, without attempting to decode other formats.

For outer DOS launcher research, opt into `DosMzImage.LoadLauncherStub`
through the `--dos-stub` MZ CLI option. It validates the outer MZ
header, relocation table, real-mode entry and the separation from
later independently MZ-bound modules. A direct LE/LX/NE/PE module is
**not** accepted as a launcher. Original CODE.1 and CODE.2 bytes
confirm a 62,532-byte independently declared DOS MZ with 78 fixups,
preceding their nested LE image. These are external regression facts,
not analyzer special cases.

```powershell
DosDis.Cli.exe inspect CODE.1
DosDis.Cli.exe mz CODE.1 --dos-stub
DosDis.Cli.exe mz-analyze CODE.1 --dos-stub --max-instructions 1024
```

Source-only CLI examples: **not compiled, tested, or executed**.
See [nested original-byte evidence](docs/P0_NESTED_MZ_ORIGINAL_EVIDENCE.md).

## Desktop file-picker usability — source checkpoint

Open Binary now remembers its last successfully loaded directory across
restarts via a small local preference. Existing filenames in **Save
project**, first-comment creation, **Backup project**, analysis export,
and diagnostic export now prompt to replace, keep both with `(1)`,
`(2)` suffixes, or cancel. New project and backup replacement occurs
only after explicit confirmation and staged SQLite integrity checks;
live journal sidecars prevent an unsafe overwrite. The original
opened game file cannot be overwritten by these workflows.

See [desktop file-dialog behavior](docs/P0_DESKTOP_FILE_DIALOGS_AND_CONFLICTS.md).
**Source changes and synthetic tests are written; a new Windows build
and manual UI retest remain pending.**

## P0 source checkpoint — bounded multi-block 16-bit guarded tables

The opt-in `--auto-tables` mode now accepts local **uniquely backed**
NOP / `JMP rel8` / `JMP rel16` bridges between a supported unsigned
CMP/JA/JAE index guard, optional register copy, BX scaling, and
CS-indexed indirect CALL/JMP. A maximum of four transparent
intermediates is used for each connection. New near-table stride
alternatives include `ADD BX,BX` (`01 DB` or `03 DB`).

Unknown or competing predecessor paths, stale original byte evidence,
register-changing intermediates and incomplete CFGs remain unresolved.
The existing CLI option is unchanged; runtime call targets stay
**candidate-only** and are never enqueued automatically.

See [guard and unique-path contract](docs/P0_X86_16_AUTOMATIC_GUARDS.md).
The new regressions are source-only; **no new build has been run**.

## P0 source checkpoint — unsigned in-range dispatch selection

`--auto-tables` can now recognize a second common bounded 16-bit
dispatch idiom: `CMP BX,N; JB/JBE in_range`, where the **taken**
path enters the checked table block. The existing `JA/JAE`
out-of-range rejection pattern remains supported. Literal operands,
unsigned entry counts, both branch edges, original code bytes and
uniquely backed NOP/JMP bridges are checked. Extra or falsified
edges cause rejection; dynamic or unprovable targets remain unresolved.

[Updated guard proof and failure modes](docs/P0_X86_16_AUTOMATIC_GUARDS.md).
Added synthetic regression cases are **not yet executed**. The last
published Windows preview predates these source changes.

## P0 source checkpoint — accumulator CMP and far index scaling

Generic 16-bit automatic table recovery now also recognizes the exact
accumulator `CMP AX,imm16` (`3D iw`) paired with a validated
`MOV BX,AX` guard. Far pointer arrays accept either `SHL BX,2`
or two separate source-backed `BX *= 2` operations using
`SHL BX,1` / `ADD BX,BX`, provided the selected path is unique.
Every FAR selector still requires a matching MZ relocation and
an explicit simulated load segment; runtime transfer paths remain
candidate-only. Additional positive and negative synthetic cases
have been added, **not run**.

[Technical guard and stride evidence](docs/P0_X86_16_AUTOMATIC_GUARDS.md).

## Development roadmap and manual build gates

The canonical plain-text milestone and validation policy is
[ROADMAP_BUILD_GATES.txt](docs/ROADMAP_BUILD_GATES.txt).
Routine `Continue` work means **source, documentation and synthetic
fixture creation only**. Windows compilation, synthetic test execution,
GitHub Actions and executable packaging occur **only on explicit user
authorization**, ideally after several related checkpoints reach an
integration gate. Each report distinguishes latest source HEAD from
last successfully built HEAD.

## Optional shared REA research adapter

[Optional REA/Ghidra independent candidate analysis](REA_ANALYSIS.md) is available through the canonical `code1-decompilation:ResearchInfra/` checkout. It is local, read-only static analysis and **does not** install REA into this application's runtime, alter original game media, or certify original DOS/gameplay parity.
