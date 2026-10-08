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
