# DOSDis desktop-only MZ support — CHECKSYS.EXE field report and source fix

**User-observed bug:** Windows WPF preview built by G2 run #42 opens
Dungeon Hack `CHECKSYS.EXE` (15,882 bytes; exact SHA256
`60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22`)
as an unexplained **raw file**, showing raw hex and "code analysis
unavailable"; **Analyze** and **Export** are disabled.

**Why:** The desktop `OpenBinary_Click` only invoked
`LeImage.Load` for 32-bit LE and otherwise populated a raw-only view.
`SetWorking` guarded Analyze with `_image != null` and Export with
`_graph != null`; `_graph` is an LE-specific
`FlowAnalysisResult`. The preexisting source-backed
`DosMzImage`, `X86RealModeDecoder` and
`Dos16ControlFlowAnalysis` were already implemented and checked
synthetically in the core/CLI, **but were not integrated into WPF**.
The MZ signature in File Intelligence therefore never enabled
the validated DOS MZ loader. This is a desktop capability/integration
bug, **not** demonstrated original-file corruption.

## New Windows desktop behavior — SOURCE ONLY / NOT YET BUILT

- **User distribution:** The **Windows WPF desktop is the only
  user-facing DOSDis application**. CLI is retained solely as an
  internal research/validation harness, not a distributed product.
  The local Windows `-Publish` script and canonical manual-only
  Windows CI workflow now build/package/publish **desktop only**.
  Neither GitHub push nor PR triggers CI.
- **Source-backed standalone MZ opening:** Try validated LE first,
  then, only if the standalone MZ signature is identified, call
  `DosMzImage.Load`. The successful MZ path retains the original
  artifact identity and shows header-derived **module-relative
  CS:IP**, stored relocation count and bounded x86-16 linear
  entry preview instead of falling back to raw hex.
  Invalid/unsupported MZ stays raw-only, never guessed as code.
  Unvalidated nested LE/NE/LX/PE wrappers remain outside the
  automatic standalone MZ path.
- **MZ Analyze:** Run `Dos16ControlFlowAnalysis.Analyze` on the
  validated original-file-backed **initial CS entry region** with
  a 20,000-instruction cap on a UI background task. The candidate
  instruction/edge/diagnostic summary retains unknown file-backed
  bytes, explicit unresolved far/indirect transfers and truncation.
  The 16-bit analyzer does not currently offer a cancellation
  token; its Cancel button remains disabled for this mode.
- **Addressing/interaction:** Render MZ locations as hexadecimal
  `CS:IP` (for CHECKSYS's entry, `0000:0000`), not LE
  `object:offset`. Resolved references and entry navigation remain
  in the same module-relative namespace. The hex pane uses the
  **original physical source offset** from `DosMzImage.ToFileOffset`;
  arbitrary `file:0xOFFSET` navigation still works.
  Out-of-backed-region addresses are rejected.
  WPF diagnostic events do not mislabel `CS=0000` as LE object 0.
- **Export:** Source-bound, schema-versioned original-backed
  x86-16 candidate JSON and text reports, with immutable source
  SHA256, original MZ geometry, instruction locations, candidate
  edges, uncertainty, diagnostics and unknown/truncated status.
  Original machine byte excerpts require explicit user opt-in.
  Reports exclude full binaries, private paths and project
  comments. The serializer validates original source SHA and
  instruction-byte identity, source geometry, duplicate starts
  and the initial entry namespace. Export still uses the desktop
  non-overwriting/atomic `ProjectFilePaths.PublishNewOrReplace`
  flow. The existing LE exporter remains unchanged.
- **SQLite caution:** The persisted project snapshot/annotation
  schema uses LE object addresses; MZ graphs use CS:IP. Project
  create/open/notes/backup controls are temporarily disabled while
  viewing MZ, rather than silently treating an MZ instruction as
  an LE object. MZ candidate reports can be exported independently.
- **File Intelligence:** Validated MZ entries now advertise actual
  16-bit analysis support, module-relative entry and original
  stored relocation count rather than the raw-only capability
  label in the supplied screenshot.

## Synthetic regression additions — NOT YET EXECUTED

`tests/DosDis.Smoke/Program.cs` now creates a standalone
synthetic MZ with two entry instructions (`MOV AX,1234h; RET`)
and one valid relocation. It checks:

- Standalone MZ File Intelligence reports validated x86-16 support,
  original entry `0000:0000` and fixup count; rejects foreign SHA.
- The existing 16-bit core analyzes two entry instruction candidates.
- Default desktop MZ JSON/text exports include exact source
  identity, module-relative addresses and two instructions but
  **no original instruction-byte excerpts** or original filename.
  Explicit byte opt-in includes bounded original instruction hex.
- Forged graph SHA, forged stored instruction bytes,
  duplicate instruction starts and incompatible CS all fail closed.
- Original MZ source bytes remain unchanged.

These tests **have not run**, and no new Windows desktop executable
has been produced after the source change. The last owner-authorized
compiled/synthetic checkpoint is G2 [run #42](https://github.com/Hyperbog/RLOFT/actions/runs/38096831427),
which **predates this GUI integration**. It must not be cited as proof
that the new WPF path compiles or works visually.

## Required acceptance steps

At the next independently **owner-authorized** Windows desktop-only
build gate:

1. Compile WPF/Core/Persistence and run C# synthetic + project smoke;
   inspect and repair any new compiler/fixture failures. Do not
   distribute CLI artifacts.
2. Publish **only** the self-contained Windows desktop preview,
   pinned to the exact canonical source SHA; record its artifact SHA.
3. On owner Windows PC, open SHA-matched original
   `CHECKSYS.EXE`. Verify the WPF no longer labels it RAW,
   `0000:0000` preview is present, MZ File Intelligence lists
   43 original relocations, **Analyze** is enabled,
   and candidate graph/references appear.
4. Select `0000:0000`, navigate to a file-backed CS:IP,
   verify source hex coordinates; export JSON and text using
   default no-instruction-bytes privacy, then optionally
   explicit byte excerpts. Verify original SHA, 43 stored fixups,
   no exported original path, nonoverwriting output, and that
   unchanged original file bytes match the independent hash.
5. Regression-check at least one real LE file to ensure its
   object-addressed flow analysis, project save/reopen,
   navigation and export remain operational.
6. No claim of proven runtime flow, correct physical DOS
   allocation, known far dynamic targets, complete function
   boundaries or G2 independent accuracy acceptance follows
   merely from opening/analyzing/exporting in the GUI.

**Current classification: GUI SOURCE FIX AUTHORED, BUILD AND INTERACTIVE
ACCEPTANCE PENDING.**
