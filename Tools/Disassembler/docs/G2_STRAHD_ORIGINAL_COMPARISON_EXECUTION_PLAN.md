# G2 Strahd CODE.2 outer-MZ independent comparison — execution plan

**Historical pre-execution plan, subsequently EXECUTED in the owner-authorized G2 build gate (2026-10-10).** The original 11-instruction MZ entry scope was matched 11/11, and an independently predeclared 17-instruction branch/call window matched 17/17. See [executed comparison and negative-control record](G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md), exact tested source `7fa76f82`, [Windows/Linux run #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405). Remaining G2 independent accuracy and human reviewer acceptance are PENDING. This document preserves the original plan; its future-tense sections describe the protocol used, not today's completion state.

## 1. Frozen source/sample identity

- Game: **Ravenloft: Strahd's Possession**, not Stone Prophet or Menzo.
- Original file: `CODE.2`, 558,689 bytes. SHA-256:
  `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Decoder reference: owner-local GNU binutils `objdump` **2.44**,
  Intel syntax, `-mi8086`, from an original-byte entry window.
  The privately supplied JSON manifest and decoder transcript must
  remain OUTSIDE Git, CI and published executable archives.
- Address class: **outer DOS MZ launcher's 16-bit file-backed resident
  module**, *not* an LE object, protected-mode linear address, or
  simulated runtime segment.
- Outer MZ declared disk extent: 62,532 bytes; header: 512 bytes.
  Initial relative CS:IP: `01BD:2382`.
  Explicit entry-region mapping is MZ CS `01BD`, origin `0`,
  physical file start `7632 = 0x1DD0`, size `54900`.
  First referenced instruction: CS:IP `01BD:2382`, original physical
  file offset `16722 = 0x4152`.
- Scope preselected independently: **11 consecutive instructions**
  at the initial entry; **9 structured register/immediate** assertions.
  No direct branch/call-target assertion and no structured assertion
  for the two memory-writing instructions at the end of this window.
  Therefore this sample cannot certify guarded CFG, pointer targets,
  original relocations, arbitrary code discovery or MZ runtime state.

## 2. Exact built code baseline and source-only change warning

- Last successful owner-authorized Windows test: GitHub Actions
  [run #38](https://github.com/Hyperbog/RLOFT/actions/runs/38088066909);
  exact checkout: PR merge
  `d8c404c56fed675457f5497859f242002e863732`;
  equivalent canonical code/test source:
  `2945b1474289cb42c167000628255848b77b7896`.
- The build compiled CLI, Windows desktop and both synthetic test
  suites; **it did not execute original DOSDis-vs-objdump comparisons**.
- Subsequent JSON unknown/duplicate-field rejection patches are
  **SOURCE ONLY** and not covered by run #38. For a test of those
  patches, obtain separate owner authorization for a new Windows build.
  Record the actual new compiled SHA, never inherit #38's PASS.
- Keep canonical GitHub Actions `workflow_dispatch` only. Never add
  game originals, oracle byte windows or private source paths to CI.

## 3. Local execution command (requires separate authorization)

From `Tools/Disassembler` with an authorized owner-local copy and
the separately provided, non-repository oracle JSON:

```powershell
dotnet run --project src/DosDis.Cli -- mz-analyze "<PRIVATE>\CODE.2" --dos-stub --external-oracle "<PRIVATE>\strahd-code2-mz-objdump-oracle.json" --external-report "<PRIVATE>\strahd-code2-g2-compare-new.json"
```

Use a **new**, nonexisting JSON report path. The CLI export must
not overwrite any prior evidence. A prebuilt CLI preview may be
used only with its documented exact build/merge SHA. Do not use
the protected-mode `analyze` command for this outer-MZ sample.

## 4. Evidence review, not an automatic pass

1. Independently calculate the original file's SHA-256 and byte
   length; reject mismatch before interpreting any comparison.
2. Verify report region physical mapping, 16-bit CS/origin, entry IP,
   decoder identity/format, exact executed build SHA and CLI options.
3. Reconcile **all 11** declared instruction results. Preserve
   matched, conflicting, missing, not-observed, unsupported and
   truncation/unknown statuses separately. For the **9** structured
   assertions retain the separate operand denominator; missing
   operand expectations are *untested*, not automatically correct.
4. Review every disagreement against retained independent decoder
   output AND independently checked original-byte/ISA evidence.
   Adjudicate mistaken oracle expectations with visible correction
   history, not by silently changing expected data.
5. Record candidate instructions outside the predeclared sample as
   **NOT BENCHMARKED**. Match does not imply runtime execution.
6. Preserve additional negative cases: altered source hashes,
   wrong MZ/file mapping, duplicate/misspelled manifest keys,
   impossible memory addressing, contradictory instruction bytes,
   incorrect control-flow expectations and truncation.
7. Predeclare additional *independently decoded* original code windows
   containing direct branches, signed/unsigned guards and 16-bit
   memory forms before viewing their DOSDis results. Cross-title
   CODE.1/CODE.2 require separate SHA-bound records.

Use `INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md` for the eventual
discrepancy register and independent human review. One 11-instruction
window — even if fully matched — would support only its narrow
benchmarked scope and **cannot** close G2 or imply whole-executable
accuracy. Status stays **G2 INDEPENDENT VALIDATION PENDING** until
all mandatory scope/secondary evidence and review obligations pass.
