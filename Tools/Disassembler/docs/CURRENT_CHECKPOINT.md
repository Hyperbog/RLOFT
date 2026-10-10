# DOSDis — compact current checkpoint

**Checked:** 2026-10-10. **Authoritative live source:** HEAD of
`disassembler-foundation` (resolve GitHub HEAD on each new turn;
**do not assume the commit that last edited this file remains HEAD**).
This is a navigation handoff, not a substitute for the full roadmap,
normative evidence requirements or a Git status check.

## What is proven

- **Last owner-authorized build:** [GitHub Actions run #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405),
  Windows and Linux builds plus synthetic suites **PASS**.
  Exact temporary PR merge checkout:
  `5eaa1fb0798007dfca73e648f91bc1add42409e9`;
  equivalent canonical code/test source:
  `7fa76f822e0aa56a4bf1c611076bc50e5e0a5389`.
  Published Windows desktop/CLI and Linux CLI are preview artifacts,
  **not** a new GUI-accepted or full game-accuracy-certified release.
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

## Current SOURCE ONLY / NOT YET EXECUTED

- Report fields `mappedRegionSha256` and
  `benchmarkedInstructionBytesSha256`, plus C# synthetic fixtures
  to expose repeated original code samples across containers,
  overlays and different relative CS mappings.
- `research/original_source_preflight.py` and its synthetic
  `test_original_source_preflight.py` suite: source-authored,
  **not yet run**, independently checks file SHA and MZ/COM geometry
  without executing game code or using Iced/DOSDis.
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
  authored. **None are compiled or executed; no original game
  relocation comparison has occurred.**
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
  This comparator is **SOURCE ONLY, NEVER EXECUTED**; agreement
  must remain `metadata-agreement-not-certification`.
  Reproduction and the synthetic test-discovery command are in
  [the multigame G2 record](G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md).
- Project-side iteration protocol in
  [CONTINUOUS_IMPROVEMENT_PLAYBOOK.md](CONTINUOUS_IMPROVEMENT_PLAYBOOK.md):
  instructions to prevent repeated compile defects, circular oracles,
  duplicated source samples and unnecessary file reuploads.

## Next highest-information steps

1. Next approved exact-SHA G2 gate should first compile all
   current source and run both synthetic suites, including
   `python3 -m unittest discover -s research -p 'test_*.py'`.
   Then compare independent Python versus DOSDis relocation metadata
   for the exact-hash Dungeon Hack CHECKSYS MZ at a common hypothetical
   load segment, retaining all discrepancies and status limitations.
2. Until that gate is explicitly authorized, continue *source-only*
   independently researched and predeclared x86-16 COM near-transfer
   / MZ relocation-bearing original windows; never seed expectations
   from new DOSDis output.
3. Cover original-source mapping/rejection and 16-bit conditional,
   indirect and signed/unsigned guard behavior, retaining unknown
   paths and exact predeclared denominators.
4. When the owner next **explicitly authorizes** a compiled test
   gate, build at one exact SHA, run synthetic negatives and the
   independent-source comparisons locally, without proprietary
   originals in hosted CI. Record disagreements and preserve
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

**Guardrails:** canonical GitHub Actions is
`workflow_dispatch`-only; PR #6 was closed/unmerged. Source-only work
and commits do not imply permission to build/test/dispatch CI. Always
verify these facts against live GitHub state before acting. Do not
commit original proprietary game bytes or private manifest excerpts.

**Maintenance:** Update this small handoff on every *substantial*
checkpoint (or gate-state change), after verifying live HEAD/build
run evidence. Keep the precise historical record in the roadmap;
avoid duplicating work logs here. Do not change the last compiled
SHA until a newer authorized run demonstrably succeeds.
