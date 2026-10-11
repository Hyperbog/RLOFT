# DOSDis — compact current checkpoint

**Checked:** 2026-10-10. **Authoritative live source:** HEAD of
`disassembler-foundation` (resolve GitHub HEAD on each new turn;
**do not assume the commit that last edited this file remains HEAD**).
This is a navigation handoff, not a substitute for the full roadmap,
normative evidence requirements or a Git status check.

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
- **Next:** Original MZ **visual File Intelligence and post-analysis disassembly are now evidenced**. Request only the remaining interaction evidence: Go/CS:IP navigation to another backed address, References/Hexadecimal tab synchronization, optional text export, and one original LE file's project workflow. Prioritize future GUI layout improvements (truncated Explorer entry, Evidence strings and File Intelligence fields, space reserved for LE-only comments) without asserting functions that the current analyzer has not established. **G2 code-accuracy certification stays pending.**
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

**Guardrails:** canonical GitHub Actions remains
`workflow_dispatch`-only; temporary PRs #6 and #8 are never merged. Source-only work
and commits do not imply permission to build/test/dispatch CI. Always
verify these facts against live GitHub state before acting. Do not
commit original proprietary game bytes or private manifest excerpts.

**Maintenance:** Update this small handoff on every *substantial*
checkpoint (or gate-state change), after verifying live HEAD/build
run evidence. Keep the precise historical record in the roadmap;
avoid duplicating work logs here. Do not change the last compiled
SHA until a newer authorized run demonstrably succeeds.
