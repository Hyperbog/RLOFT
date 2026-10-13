# DOSDis — compact current checkpoint

## P0 CODE.1 scored-byte workbench foundation — Windows #59 PASS

- Review of Claude's project critique identified the largest
  validation gap: x86 opcodes had narrow independent sampling,
  while **code discovery versus embedded data** was not scored
  against existing detailed decompilation research.
- Imported the independently curated `code1-decompilation`
  **CODE.1 LE object 1** classification ledger into the
  machine-readable `benchmarks/strahd-code1-object1-classification.json`
  (**282 exact excluded ranges, 1,041 nontranslated bytes,
  69,361 translated machine-code bytes; object size 70,402**).
  Original complete file SHA256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`;
  extracted object SHA256 `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203`.
  The manifest has **NO game bytes**.
- Implemented SHA-bound, source-byte-checked classification
  scoring in `CodeClassificationBenchmark`: candidate-on-
  translated, candidate-on-excluded, unclassified-translated
  and unclassified-excluded, candidate-byte precision and
  translated-byte recall. Abstentions are NOT decoder
  true-negative proof. Overlapping, forged or unbacked
  instructions and mismatched reference source/object hashes
  fail closed. Disagreements are bounded and physically
  navigable. Real instruction-start precision requires a
  separately extracted independent boundary catalog later.
- The Windows desktop **Benchmark…** button (LE graph only)
  loads a local JSON reference and displays source-bound
  precision/recall and navigable disagreement spans.
  The pinned original-free manifest ships under
  `benchmarks/` in the desktop package; the original file
  never leaves the owner's computer. Double-clicking a
  disagreement opens its stored physical Hexadecimal view
  without speculatively classifying new executable code.
- Automatic [Windows build #59](https://github.com/Hyperbog/RLOFT/actions/runs/38342771655)
  **SUCCESS**, compiled app/test source SHA
  `2f3a02f59c6e8db9c5842d2f945f8bd994de4bcf`.
  Windows WPF and both C# test projects compiled **0
  warnings/errors**. DosDis.Smoke, including NEW separate
  `CodeClassificationBenchmarkSmoke` and the pinned
  manifest-accounting test, **PASS**; SQLite persistence
  smoke **PASS**. Exactly one win-x64 WPF desktop ZIP:
  artifact `11787113883`, **66,931,033 bytes**, SHA256
  `6e1663fbae60ba0b051d8fd50c8e13272ecab3b7b1e041cf787e7b2f9d561c71`.
  [Download](https://github.com/Hyperbog/RLOFT/actions/runs/38342771655/artifacts/11787113883).
  No game originals/CLI distribution in GitHub CI.
- **Still PENDING:** actual original CODE.1 owner-local
  measured byte precision/recall; verified independent
  instruction starts/lengths; WPF user interaction with
  Benchmark dialog; held-out game coverage and trace
  evidence; faithful reassembly. Avoid claiming original
  score from synthetic tests or counting valid NOP padding
  as ISA decoding errors by default.
- [Full original-free reference, scoring contract and local
  acceptance checklist](CODE1_CLASSIFICATION_BENCHMARK_P0_20261012.md).
  **Next:** Have owner open exact original CODE.1, Analyze,
  select packaged manifest under Benchmark…, and report
  metrics, disagreements and first GUI screenshot. Then
  derive a separately audited start-boundary reference,
  triage false positives, improve indirect discovery, and
  implement analyst-controlled classifications/labels.

## Repository branch cleanup (2026-10-12)

- The owner requested one **current/main DOSDis branch** and deletion
  of redundant verification branches. Verified exact branch tips,
  closed/unmerged pull requests, and compared source with
  `disassembler-foundation` before deletion.
- Preserved **`disassembler-foundation`** as the sole DOSDis
  development branch. Deleted **`disassembler-g2-build-20261010-54247e`**
  (closed PR #8, unique branch changes temporary workflow only;
  original C# oracle fixture blob matched canonical byte-for-byte)
  and **`dosdis-windows-byte-search-gate-20261011-1f8b6b`**
  (closed PR #14, unique changes temporary workflow only).
- A one-time ref-deletion Actions job verified each expected SHA
  and PR closed/unmerged status before deleting; [run
  #1](https://github.com/Hyperbog/RLOFT/actions/runs/38337508053)
  **SUCCESS**. Subsequent GitHub branches query: **16 branches**,
  **one DOSDis branch**, the remaining **15 unrelated branches
  preserved**. The temporary maintenance workflow was
  subsequently deleted. No game source or DOSDis code was
  modified by this cleanup; canonical automatic desktop build
  workflow remains in place.
- Repository default branch remains `code1-decompilation`
  because other project work depends on it; this cleanup does
  **not** rename or replace GitHub's repository default.
  DOSDis's canonical development branch remains
  `disassembler-foundation`.


**Checked:** 2026-10-10. **Authoritative live source:** HEAD of
`disassembler-foundation` (resolve GitHub HEAD on each new turn;
**do not assume the commit that last edited this file remains HEAD**).
This is a navigation handoff, not a substitute for the full roadmap,
normative evidence requirements or a Git status check.

## CURRENT — Automatic Windows desktop CI enabled; build #49 PASS

- Owner expressly lifted the former DOSDis manual-build restriction:
  the assistant may compile, run original-free synthetic tests and
  publish **Windows WPF desktop-only** builds as needed, without
  separate per-build approval. Canonical
  `.github/workflows/disassembler-core.yml` now runs on scoped
  pushes to `disassembler-foundation`, scoped PRs targeting it,
  and manual `workflow_dispatch`; unrelated project/doc-only
  edits do not trigger Windows builds. Original proprietary game
  execution and independently sourced accuracy review remain
  separately gated. CLI remains internal, no public CLI artifact.
- The **canonical automatic [run #48](https://github.com/Hyperbog/RLOFT/actions/runs/38193637125)**
  allocated a Windows runner and exposed one real C# desktop
  compiler error `CS0136` (duplicate `fileOffset` local);
  fixed in canonical `15abeee277b2729028654c0b00865bb4d0c81b74`.
- The next automatic [Windows run #49](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386)
  **PASSED** WPF/Core/Persistence and both synthetic C# smoke
  suites and published exactly one Windows desktop artifact,
  no CLI. Source compiled: `15abeee277b2729028654c0b00865bb4d0c81b74`.
  Artifact `11722213441`, ZIP 66,915,474 bytes,
  ZIP SHA256 `bf88b54660579ae4bfd038cd2f8a8a8622858d3abed97809b286f614f904a2d1`.
  [Download](https://github.com/Hyperbog/RLOFT/actions/runs/38193734386/artifacts/11722213441).
- The new Enter/Go/Find, source-physical per-byte Hex click,
  repeated byte search, and cross-tab highlight source is
  **COMPILED + SYNTHETIC VERIFIED at #49**. Actual WPF user
  interaction on CHECKSYS original and LE regression **PENDING**.
  Do not imply build/synthetic success is an independent
  original-game decoder or DOS runtime certificate.
- **Current authoritative [build/policy record](DESKTOP_AUTO_BUILD_WIN49_20261011.md)**.
  Older #45–#47 failure report below is historical: those
  temporary PR jobs never started and are superseded for
  compiled/synthetic evidence by successful canonical #49.

## Historical temporary PR #45–#47 runner-allocation failures

- Owner reports that the previous #44 navigation fixes work in their
  Windows application. New request: pressing **Enter** in Go To
  should execute search; searched addresses/byte values should
  be highlighted in Disassembly, Hexadecimal and References; clicking
  one hex byte (`CD` in `09 FF CD 21`) should select the
  individual original-file byte rather than only allowing text drag.
- **SOURCE-AUTHORED, NOT YET BUILT:** Go/Find Enter handler,
  `CS:IP` / LE object / original file addresses retained, plus
  bounded `CD`, `CD 21`, `hex:46 04 CD 21` original-file
  byte-pattern queries; repeated Enter finds next match with wrap.
  Invalid byte syntax and unbacked `??` cells are rejected.
- `HexByteNavigation` maps click/text spans in bounded raw/LE
  hex previews to true physical source offsets; the read-only
  TextBox remains copyable with drag-select. Clicking one
  original-byte cell marks exactly its two hex digits and shows
  original physical offset and value. Cross-tab status,
  disassembly selected row AND exact hex substring,
  References source/destination row, and Hex selection
  use the same original-byte selection without guessing code.
  Undecoded data searches switch to Hex rather than
  fabricating an instruction/reference.
- WPF assembly rows are now **stable-identity classes** with
  mutable byte highlight properties and change notification:
  this avoids the earlier risk of record equality changing when
  highlighting the selected row. Synthetic-only hex token,
  search, malformed-input, no-match, next-match, wrap, LE
  unbacked and nonzero window regression fixtures were authored.
- Owner **authorized Build** on 2026-10-11. An isolated
  temporary PR #14 attempted Windows build [#45](https://github.com/Hyperbog/RLOFT/actions/runs/38193004540),
  Windows re-run, Linux cross-target [#46](https://github.com/Hyperbog/RLOFT/actions/runs/38193208259)
  and branch-scoped Linux push [#47](https://github.com/Hyperbog/RLOFT/actions/runs/38193253571).
  **ALL STOPPED BEFORE RUNNER ALLOCATION:** jobs `steps=[]`,
  `runner_id=0`; 0 Windows billed milliseconds; no
  .NET build, smoke execution or artifact. This does NOT
  prove a compiler or code failure. The GitHub web UI
  runner/allocation/billing/permissions message must be checked.
  The canonical manual-only workflow remained unchanged.
- Current preview #44 **does not include these additions**.
  Source still NOT COMPILED and GUI feature NOT ACCEPTED.
  [Full blocked-build #45–#47 record](DESKTOP_BYTE_SEARCH_WINDOWS_BUILD_20261011_GATE45_BLOCKED.md).
- [Full Enter-search/clickable-hex scope and acceptance checklist](DESKTOP_BYTE_SEARCH_AND_HIGHLIGHT_20261011.md).
  Prior build #44 remains Windows compiled/synthetic verified;
  original G2 decoder accuracy and DOS runtime unchanged/pending.

## CHECKSYS navigation #44 Windows BUILD/SYNTHETIC PASS — owner GUI acceptance pending

- Owner tested the original-MZ Windows #43 WPF preview: double-click
  `0000:0025 call 017Dh` did nothing; Go while on Hexadecimal
  did not visibly reposition the pane; References double-click
  **did navigate correctly**, but References showed only a few
  items for the selected instruction. This refines, rather than
  invalidates, the earlier Analyze/JSON Export success.
- Source cause: Disassembly DataGrid had NO double-click handler,
  References was restricted to a selected-row index, and Hex Go
  relied on `SelectionChanged` (which does not fire when selecting
  the same instruction again). Hex also lacked a physical-position
  label and scroll reset.
- **NOW BUILT / SYNTHETIC VERIFIED by owner-authorized Windows
  [run #44](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116)
  (original GUI interaction NOT YET TESTED):** direct same-segment graph
  call/branch navigation on Assembly double-click, Hex Go to any
  backed module-relative MZ position with explicit original physical
  offset and scroll reset, References default ALL graph edges
  with Source/Destination/Kind/Evidence/count and optional
  selected-row filter; preserves existing linked-reference navigation.
  Synthetic negative controls reject indirect, far
  and ambiguous destinations; an LE project without analysis
  clears stale graph-reference state. Both C# smoke suites
  **PASSED** on Windows #44, with zero .NET warnings or errors.
- Explicit original mapping: `0000:0025` CALL stored at
  `file+0x225`; encoded `E8 55 01` resolves within the
  same segment to `0000:017D`, stored `file+0x37D`.
  This is file-backed candidate navigation, not a runtime trace.
- [Desktop navigation owner field report and next Windows test
  checklist](G2_DESKTOP_NAVIGATION_FIELD_REPORT_20261010.md).
- **Latest successfully built Windows app is #44**: canonical
  application/test SHA `9e18238c2c4dbb1ebfb13948e07ef6efd82b7a9f`;
  actual ephemeral PR #13 merge checkout SHA
  `6626ce4e17d589b16a458e9b5950f910f19791ee`
  differs ONLY by temporary pull-request workflow. One
  Windows desktop-only artifact `11703189013`, ZIP bytes
  `66906804`, archive SHA256
  `da73be2d543171f956307fa0eb51463e8528ad2f219154ee6b32bbcac55dc8f4`.
  [Download](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116/artifacts/11703189013).
  NO user-facing CLI package and NO proprietary originals in CI.
  **Source compiled and synthetic tested, not yet owner GUI verified.**
  Full [#44 gate record](G2_DESKTOP_NAVIGATION_WINDOWS_BUILD_20261011_RUN44.md).

## CHECKSYS original MZ screenshot + JSON — partial owner GUI acceptance

- User tested the run #42 Windows WPF executable with exact-original
  Dungeon Hack `CHECKSYS.EXE` and reported only **RAW HEX VIEW**;
  Analyze and Export were disabled. This is an integration defect:
  WPF only loaded LE images despite x86-16/MZ core support.
- **Owner-authorized [Windows build #43](https://github.com/Hyperbog/RLOFT/actions/runs/38098035861) PASSED.** Exact temporary PR #11 merge checkout `88c2131d155a7c532dce37a6e9965c25bec42c69`, application/test code equivalent to canonical `244718cc7f86ba7ee2815cfc1df2d27f9be271b0`; temporary workflow is the only changed path. Windows WPF, core and project synthetic tests, original-free MZ analysis/export regressions, and self-contained desktop publish **PASS** with zero .NET warnings/errors.
- **Only one artifact**: `DOSDis-Desktop-win-x64-preview`, GitHub artifact ID `11686312055`, 66,903,050 ZIP bytes, ZIP SHA256 `313653140fb6fb2e322e13e7e7a7c0d0f7bbed37fafd8c4b9454e3e103700812`, [download](https://github.com/Hyperbog/RLOFT/actions/runs/38098035861/artifacts/11686312055). No user-facing CLI package.
- Standalone MZ WPF loader/preview, bounded x86-16 entry-segment
  analysis, typed **CS:IP** navigation/references/physical-hex view,
  source-bound JSON/text candidate export and MZ File Intelligence
  passed **compilation plus original-free synthetic validation**.
  The owner subsequently supplied an original-source-matched
  `dosdis-mz16-candidate-analysis.json` with **336 x86-16
  candidate instructions, 394 edges and 28 unresolved diagnostics**
  from exact-SHA Dungeon Hack CHECKSYS.EXE. This now evidences
  a real-original MZ Analyze/JSON Export workflow. **The owner
  then supplied a Windows GUI screenshot** visually confirming
  standalone MZ recognition, CS:IP instruction listings, the
  File Intelligence entry/relocation fields, and the same
  336/394/28/14,544 analysis status. No screenshot of an actual
  navigation/References/Hex action or real LE GUI regression
  is supplied. No independent decoded-code accuracy or
  real DOS execution claim.
- Current SQLite graph and note schema is LE-object-only: project
  save/open/notes/backup are disabled in MZ mode, while direct
  MZ candidate export is available. The exporter checks full source
  SHA, original image geometry, candidate source bytes and duplicate
  addresses before publication. New synthetic core regression cases
  passed the Windows #43 synthetic regression gate.
- **Owner product requirement:** only the **Windows WPF desktop** is
  distributed. CLI remains INTERNAL research/validation harness code;
  local Windows build script and canonical manual-only CI now publish
  **desktop only**, no CLI artifact.
- Exact root cause and Windows acceptance plan:
  [G2_DESKTOP_MZ_UI_HANDOFF_20261010.md](G2_DESKTOP_MZ_UI_HANDOFF_20261010.md).
- **Next:** Owner should test the NEW #44 artifact with exact original
  CHECKSYS: direct `0000:0025 call 017Dh` double-click -> `0000:017D`;
  Hex Go at both addresses with source physical file+0x225 and
  file+0x37D plus scroll/first-row readout; all References by default
  and selected-only filter while retaining linked-reference navigation.
  Check one real LE file's project workflow and optional MZ text export.
  Continue visual layout improvements separately. **G2 original-code
  independent accuracy acceptance remains pending.**
- Exact first original-file export evidence and private-audit summary:
  [G2_DESKTOP_MZ_WINDOWS_BUILD_20261010_RUN43.md](G2_DESKTOP_MZ_WINDOWS_BUILD_20261010_RUN43.md).
  Do NOT check in the owner's exported JSON or proprietary disassembly.

## What is proven

- **Previous owner-authorized cross-platform build (historical):** [GitHub Actions run #42](https://github.com/Hyperbog/RLOFT/actions/runs/38096831427) **SUCCESS**, Windows desktop/CLI and Linux CLI compile/publish, Windows+Linux C# smoke and project suites, **17 Python synthetic** tests passed. Exact temporary PR #8 merge SHA `dd251ba7479fe802e17155a2740d3519cf23847b`; source-equivalent canonical C#/Python code/test SHA `3956f036fee44c0b6f07361811e4f0857dbea916`. See [full gate record](G2_BUILD_GATE_RECORD_20261010_RUN42.md). Windows GUI-owner acceptance, new original-file comparison and broad accuracy certification are **NOT** included.
- **Failed/diagnostic run #41:** Windows/Linux compilation passed, but an incorrect negative test removed one sampled original instruction while asserting a two-instruction SHA. Corrected the synthetic-only manifest membership at canonical `3956f036`; rerun #42 passed. Both run IDs are retained.
- **Last verified original-code scope:** owner-local original Strahd
  `CODE.2` outer 16-bit MZ launcher, independently decoded with
  GNU objdump 2.44: **28/28** selected instruction starts,
  **15/15** structured operands, **5/5** near branch/call edges.
  **8/8** adversarial negative controls had expected outputs.
  Other 6,512 candidate instruction starts were unbenchmarked in
  those two windows. Results are bound to the run #40 compiled SHA,
  not to the present unbuilt source.
- **Correction:** Strahd `CODE.1` and `CODE.2` share the
  same 62,532-byte outer DOS MZ launcher; they do **not** provide two
  independent x86-16 code samples even though whole-file SHA differs.
- **New independent source preselection:** four DOS executable files
  in authenticated `DnD-Classics/MASTER` have individually verified
  source archive SHA/byte size:
  Dungeon Hack `SBLASTER.COM` and `CHECKSYS.EXE`,
  Pool of Radiance `POOLRAD/START.EXE`,
  Curse of the Azure Bonds `START.EXE`. Gold Box entries share the
  first **283 stored bytes**, which are not 283 decoded instructions.
  Original files do **not** currently need reupload for these cases.

## Earlier cross-platform G2 synthetic verification; original accuracy pending

- Report fields `mappedRegionSha256` and
  `benchmarkedInstructionBytesSha256`, plus C# synthetic fixtures
  to expose repeated original code samples across containers,
  overlays and different relative CS mappings. **Synthetic verified
  Windows/Linux at #42; original sampled-code coverage still pending.**
- `research/original_source_preflight.py` and its synthetic
  `test_original_source_preflight.py` suite: synthetic verified
  in Linux #42; the preflight has **not** been applied to new
  owner-original binaries by this build gate.
- Multigame original selection/relocation geometry records:
  **source hash/header verified only**; no new external-disassembler
  / DOSDis original-file comparison for the four other game files.
- Source-only security and independent relocation audit:
  `describe()` now rechecks the requested original SHA instead of
  trusting a passed string. The independent Python preflight can
  simulate original-backed DOS MZ 16-bit modular relocations at a
  **caller-assumed** load segment and return only a transformed-image
  SHA256, count and overlap metadata. DOSDis's separate C#
  `DosMzRebasedView.Summary.SimulatedFileBackedImageSha256`
  exposes the matching copied-image fingerprint in `mz-relocate`
  and `mz-analyze --load-segment` output. Synthetic positive,
  overlap, wrap, malformed-SHA and invalid-segment cases were
  authored and **synthetically verified by #42**. No original
  game relocation comparison has occurred.
- Source-only evidence-reconciliation hardening:
  C# synthetic MZ relocation tests no longer depend on opaque
  prewritten SHA256 strings; their expected 480-/464-byte image
  buffers are independently constructed from original synthetic
  opcode/fixup arithmetic and hashed. Python has matching independently
  built resident-image fixtures. The x86-16 benchmark digest fixture
  similarly derives expected SHA from separately specified
  offset/length/byte input, not a magic digest string.
- New `research/compare_mz_relocation_reports.py` and synthetic-only
  `test_compare_mz_relocation_reports.py` enforce exact pinned
  original SHA and five metadata comparisons (assumed load segment,
  file-backed byte count, fixup count, overlap indicator, relocated
  image SHA256). Malformed/duplicate JSON, unsupported simulation
  statuses and self-contradictory overlap claims fail closed.
  `mz-relocate` and `mz-analyze --load-segment` now expose
  `OriginalFileBackedBytes` for an independent length check.
  This comparator's synthetic cases **passed Linux #42**; no
  actual original-game comparison was run. Agreement must remain
  `metadata-agreement-not-certification`.
  Reproduction and the synthetic test-discovery command are in
  [the multigame G2 record](G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md).
- Project-side iteration protocol in
  [CONTINUOUS_IMPROVEMENT_PLAYBOOK.md](CONTINUOUS_IMPROVEMENT_PLAYBOOK.md):
  instructions to prevent repeated compile defects, circular oracles,
  duplicated source samples and unnecessary file reuploads.

## Next highest-information steps

1. The #42 Windows/Linux and synthetic Python/C# gate is now
   **PASS**. Next separately authorized exact-SHA original-file
   validation should compare independent Python versus compiled DOSDis
   relocation metadata for the verified Dungeon Hack CHECKSYS MZ at
   the same hypothetical load segment. Retain discrepancies, negative
   controls, sample denominators and all status limitations.
2. Until that gate is explicitly authorized, continue *source-only*
   independently researched and predeclared x86-16 COM near-transfer
   / MZ relocation-bearing original windows; never seed expectations
   from new DOSDis output.
3. Cover original-source mapping/rejection and 16-bit conditional,
   indirect and signed/unsigned guard behavior, retaining unknown
   paths and exact predeclared denominators.
4. When the owner next **explicitly authorizes** a compiled test
   original-comparison gate, use the exact #42 compiled binary
   and separately researched expected values; keep proprietary
   originals out of hosted CI. Record disagreements and preserve
   authorizations/artifact SHA.
5. Do not call **G2 INDEPENDENT VERIFIED** until all required
   feature/format samples, second evidence class and human reviewer
   adjudication pass. Current **G2-wide certification: PENDING**.

## Where to read deeper (in priority order)

1. [Continuous-improvement procedure](CONTINUOUS_IMPROVEMENT_PLAYBOOK.md)
2. [Current roadmap, normative build gate and work log](ROADMAP_BUILD_GATES.txt)
3. [G2 independent scope](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md)
4. [Multigame original source selection](G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md)
5. [Strahd executed sample evidence](G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md)
6. [Normative independent validation policy](INDEPENDENT_VALIDATION_POLICY.md)

**Guardrails:** canonical DOSDis Windows desktop CI is
**AUTOMATIC on scoped code/test pushes and PRs, plus manual dispatch**;
the owner permanently authorized DOSDis builds/tests as needed on
2026-10-11. Temporary PRs #6, #8, #11, #13, #14
remain closed without merging. Do not commit proprietary original
game bytes, private owner-local manifests, or unsupported decoder
certifications. Original-game execution and independent accuracy
claims remain separate explicit evidence/safety gates.

**Maintenance:** Update this small handoff on every *substantial*
checkpoint (or gate-state change), after verifying live HEAD/build
run evidence. Keep the precise historical record in the roadmap;
avoid duplicating work logs here. Do not change the last compiled
SHA until a newer authorized run demonstrably succeeds.
