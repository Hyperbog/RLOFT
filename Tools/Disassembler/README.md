# DOS Disassembler — foundation (Part 14)

Evidence-first DOS disassembly research application. **Early prototype:** command-line analysis only, not yet a desktop app or certified full disassembly. Source files are original-byte read-only; no language generation.

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
