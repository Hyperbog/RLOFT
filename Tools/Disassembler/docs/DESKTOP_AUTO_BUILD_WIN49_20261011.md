# DOSDis automatic Windows desktop build policy and run #49

**User instruction (2026-10-11, local):** repository is public; remove
the prior DOSDis manual-only CI policy and build whenever development
requires. **This explicitly supersedes the earlier build authorization
restrictions for DOSDis.** It does not authorize writing proprietary
original game binaries into public CI/artifacts or treating a successful
build as original-game ISA accuracy certification.

## Permanent workflow update

Canonical workflow `.github/workflows/disassembler-core.yml`
changed at `dbf9abf9677addb618ba0206269150d47b78ffc5`:

- `workflow_dispatch` remains available.
- Automatic `push` builds on `disassembler-foundation` only, with
  `paths` limited to DOSDis source, tests, Windows build script,
  Directory.Build files, solution files and the workflow itself.
- Automatic `pull_request` builds targeting
  `disassembler-foundation`, restricted to those same paths.
- Concurrency cancels superseded runs per branch/ref.
- Windows `windows-latest` with .NET 10 compiles Windows WPF desktop,
  DOSDis.Smoke and DosDis.ProjectSmoke, executes both synthetic suites,
  publishes exactly one self-contained Windows x64 **desktop-only**
  artifact. The internal research CLI is NOT an end-user artifact.
- No original copyrighted game bytes, unredacted original disassemblies
  or owner-local original-byte manifests are allowed in hosted CI.

The GitHub repository was checked live and is **public**, with default
branch `code1-decompilation`, while DOSDis's canonical development
branch remains `disassembler-foundation`. Public visibility does
not by itself guarantee Actions runner assignment, quota or billing;
those need separate runtime confirmation.

## First automatic runs

### Automatic run #48 — initial compile failure

[Run #48](https://github.com/Hyperbog/RLOFT/actions/runs/38193637125),
GitHub run ID `38193637125`, automatically triggered from workflow
commit `dbf9abf9677addb618ba0206269150d47b78ffc5`.

- Windows runner **WAS allocated** and checkout and setup-dotnet succeeded.
- Core and Persistence compiled.
- Windows WPF compilation FAILED with exact
  `CS0136` at `MainWindow.xaml.cs:975`: a second local
  `fileOffset` pattern variable was declared within an enclosing
  scope that already used that name. **0 warnings, 1 error**.
- Smoke, project smoke, publish and artifact stages did NOT run.

The correction renamed only the shadowing inner Hex Go variable to
`hexFileOffset`, committed as
`15abeee277b2729028654c0b00865bb4d0c81b74`.
The source fix automatically triggered the next Windows run.

### Automatic run #49 — complete success

[Run #49](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386),
GitHub run ID `38193734386`, source SHA
`15abeee277b2729028654c0b00865bb4d0c81b74`:

| Verification stage | Evidence |
| --- | --- |
| Windows WPF / Core / Persistence compilation | **PASS** |
| DosDis.Smoke project compilation | **PASS** |
| DosDis.ProjectSmoke project compilation | **PASS** |
| Synthetic DosDis.Smoke | **PASS**: `ALL SMOKE TESTS PASSED` |
| Synthetic DosDis.ProjectSmoke | **PASS**: SQLite/snapshot/annotation persistence |
| Self-contained `win-x64` WPF desktop publish | **PASS** |
| One GitHub Actions artifact upload | **PASS** |
| Windows WPF GUI keyboard/mouse interaction | **NOT PERFORMED** |
| Independent original-game ISA/flow accuracy | **PENDING** |

No proprietary original game files were executed or packaged
in hosted CI. No CLI deliverable was published.

**Only new end-user downloadable package:**

- Artifact: `DOSDis-Desktop-win-x64-preview`.
- Artifact ID: `11722213441`.
- ZIP bytes: **66,915,474**.
- GitHub uploaded ZIP SHA256:
  `bf88b54660579ae4bfd038cd2f8a8a8622858d3abed97809b286f614f904a2d1`.
- [Download desktop ZIP](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386/artifacts/11722213441).
- SHA256 is for the **artifact ZIP**, not an individual contained EXE.

The previously documented temporary PR #14 and failed runs #45-47
remain historical evidence of failure to allocate a runner. They
do not supersede a successful canonical automatic build #49.

## Owner GUI verification still required

The build/smoke result establishes that the new source **compiles**
and its synthetic source-bound mapping/search tests pass. It does
not prove user-facing WPF mouse selection and cross-tab highlights.

On Windows, open SHA-pinned Dungeon Hack `CHECKSYS.EXE`; verify:
Enter executes the Go/Find query; `CD`, `CD 21`,
`hex:46 04 CD 21` find actual source file-backed bytes;
repeated Enter finds next; click one `CD` hex-byte token to
highlight exactly its two digits and source physical offset;
normal drag-select copying still works; matching disassembly
original-byte portions and existing reference rows highlight;
unknown bytes do not invent candidate instructions/references.
Check a supported LE file in addition to MZ and ensure prior
navigation and project functions remain operational.

**Classification:** #49 WINDOWS BUILD + SYNTHETIC VERIFIED; actual
GUI owner acceptance and independent original-code accuracy PENDING.
**Policy:** automatic path-scoped Windows desktop CI ENABLED; no
future per-build owner permission required for DOSDis, but separate
scope/accuracy/privacy constraints remain enforced.
