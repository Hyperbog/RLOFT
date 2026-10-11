# DOSDis Windows desktop-only MZ GUI build gate — #43

**Owner authorization:** "You may proceed" in response to the proposed
Windows desktop-only build and regression gate. 2026-10-10 local date
(2026-10-11 UTC completion).

**Gate status:** **WINDOWS DESKTOP COMPILED, SYNTHETIC REGRESSIONS PASS,
ONE SELF-CONTAINED WPF PACKAGE PUBLISHED.** The earlier owner-reported
standalone DOS MZ raw-only UI problem has a newly built candidate fix,
but has **not been interactively rechecked on original CHECKSYS.EXE**.
Neither real DOS execution nor G2-independent original-code accuracy
certification follows from this milestone.

## Exact source and hosted build identity

- Canonical branch: `disassembler-foundation`
- Canonical application, source and tests submitted to this build:
  `244718cc7f86ba7ee2815cfc1df2d27f9be271b0`
- Temporary owner-authorized PR [#11](https://github.com/Hyperbog/RLOFT/pull/11)
  with branch `disassembler-mz-desktop-build-20261010-244718`
- PR source head, containing **only** temporary workflow trigger:
  `e5f520aa07e9394a11a2269ea2abc7323e50d3a4`
- Actual ephemeral PR merge SHA checked out by Windows Actions:
  `88c2131d155a7c532dce37a6e9965c25bec42c69`
- Canonical-vs-merge comparison verified **one changed path only**:
  `.github/workflows/disassembler-core.yml`, three YAML line
  changes for temporary `pull_request` enablement. No application
  source, test source, packages or original files differ.
- Windows build [run #43](https://github.com/Hyperbog/RLOFT/actions/runs/38098035861),
  GitHub Actions run ID `38098035861`, result **SUCCESS**.
  Windows `.NET 10` release build, self-contained win-x64 publish.

## CI / synthetic verification

| Verification | Result |
| --- | --- |
| Windows WPF desktop project build | **PASS**, 0 warnings/errors |
| C# core DosDis.Smoke build | **PASS**, 0 warnings/errors |
| C# DosDis.ProjectSmoke build | **PASS**, 0 warnings/errors |
| DosDis.Smoke synthetic original-free checks | **PASS** |
| DosDis.ProjectSmoke SQLite/project synthetic checks | **PASS** |
| New MZ capability and 16-bit entry analysis synthetic checks | **PASS** |
| MZ JSON/text default-no-original-byte and opt-in-byte checks | **PASS** |
| Forged SHA/bytes, duplicate entry and wrong MZ segment rejection | **PASS** |
| Windows x64 desktop-only self-contained publish | **PASS** |
| GitHub artifact upload | **PASS** |
| End-user Windows interactive GUI test on CHECKSYS original | **NOT RUN** |
| LE original-file GUI regression | **NOT RUN** |
| G2 independent original-code / runtime accuracy | **PENDING** |

The synthetic suite confirms that the C# core supports MZ semantics
and source-bound reporting; it does **not** exercise actual WPF button
clicks, file dialog interaction, a proprietary original executable,
real project persistence in MZ mode, or DOS emulator execution.
SQLite project snapshots remain LE-only while MZ source-backed
candidate export is available independently.

## Exact distributable

**Only one end-user package published; no CLI artifact:**

- Artifact name: `DOSDis-Desktop-win-x64-preview`
- Artifact ID: `11686312055`
- GitHub Actions ZIP archive size: `66903050` bytes
- GitHub artifact archive SHA-256:
  `313653140fb6fb2e322e13e7e7a7c0d0f7bbed37fafd8c4b9454e3e103700812`
- Direct authenticated browser artifact:
  https://github.com/Hyperbog/RLOFT/actions/runs/38098035861/artifacts/11686312055
- Run overview:
  https://github.com/Hyperbog/RLOFT/actions/runs/38098035861

The digest is **for GitHub's uploaded artifact ZIP**, not a guarantee
of any individual contained executable's digest.

The canonical repository workflow remains
`workflow_dispatch`-only; the temporary PR verification trigger
must remain **closed/unmerged**.

## Owner Windows acceptance: exact original CHECKSYS.EXE

Use SHA-pinned original from Dungeon Hack:
- Original length: **15,882** bytes.
- SHA256: `60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22`.
- Expected stored MZ: 512-byte header, 15,370 file-backed image bytes,
  entry `0000:0000`, 43 header relocation entries.

1. Unzip and run the **new desktop** from #43, not the previous #42
   preview. Open the unchanged CHECKSYS.EXE.
2. Verify title/header now shows **DOS MZ** and initial module-relative
   `0000:0000`, File Intelligence gives 43 original MZ fixups,
   and **Analyze** is enabled (instead of RAW HEX ONLY).
3. Run Analyze and verify candidate x86-16 instructions, control-flow
   evidence and references appear. Unknown bytes must not be
   advertised as confirmed code.
4. Use the `CS:IP` address box (`0000:0000`), choose other
   backed positions, and confirm displayed physical hex mapping.
   Check LE object navigation in a separately supported LE original.
5. Click Export. Save JSON and text with **No** at the
   original-byte excerpt prompt. Verify source SHA, original
   file-backed length and relocation count, and that no full
   executable/source path is included. Optional **Yes** contains
   only bounded original candidate instruction bytes. Exports may
   not replace the original binary.
6. Confirm existing LE project save/reopen and export remain functional.
   MZ project saving/annotation is deliberately disabled until MZ
   address namespaces are supported by the persistence schema.

**Acceptance threshold:** a positive WPF UI owner report/screenshot
of the actual updated CHECKSYS workflow plus a real LE regression,
distinct from this compile/synthetic gate. No complete G2 source
accuracy or runtime-flow claim without independently compared
original-code evidence.

**Final classification:** BUILD/SYNTHETIC PASS; ORIGINAL WPF UI
ACCEPTANCE **PENDING**.
