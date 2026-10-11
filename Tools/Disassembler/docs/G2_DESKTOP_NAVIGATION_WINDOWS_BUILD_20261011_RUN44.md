# DOSDis Windows desktop navigation repair — authorized build #44

**Date:** 2026-10-11. **Owner authorization:** “proceed with that Windows build.”
**Result:** COMPILED / SYNTHETIC VERIFIED / ONE WINDOWS DESKTOP ZIP PUBLISHED.
**Not verified:** Interactive original CHECKSYS.EXE desktop navigation, LE GUI regression,
runtime game execution or comprehensive independent x86-16 accuracy.

## Exact source and build identity

- **Canonical source / tests:** `9e18238c2c4dbb1ebfb13948e07ef6efd82b7a9f` on
  `disassembler-foundation` at the moment of this owner-approved gate.
- **Temporary test branch:** `dosdis-windows-navigation-gate-20261011-9e1823`.
- **Temporary draft PR:** [#13](https://github.com/Hyperbog/RLOFT/pull/13);
  its workflow-only head commit is
  `fb54de74c2874d48fca8b705dc138e439adf2005`.
- **Actual GitHub Actions PR-merge checkout:**
  `6626ce4e17d589b16a458e9b5950f910f19791ee`.
  Independent GitHub commit comparison with the pinned canonical
  source reported **only one changed path**:
  `.github/workflows/disassembler-core.yml`, four lines.
  The compiled desktop/core/tests therefore match canonical source;
  the only temporary difference is the PR-trigger workflow.
- **Successful Actions run:** [#44, ID 38143277116](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116);
  Windows job `114481322988`. Overall conclusion **success**.

## Verified compile, smoke and package results

| Step | Verified result |
| --- | --- |
| Release build: DosDis.Desktop | PASS — 0 .NET warnings, 0 errors |
| Release build: DosDis.Smoke | PASS — 0 .NET warnings, 0 errors |
| Release build: DosDis.ProjectSmoke | PASS — 0 .NET warnings, 0 errors |
| DosDis.Smoke source-only synthetic suite | PASS |
| DosDis.ProjectSmoke synthetic persistence suite | PASS |
| New direct-16-bit navigation fixture: CALL, conditional taken branch, unresolved indirect, foreign/ambiguous targets | Included in passing synthetic smoke suite |
| Windows self-contained desktop publish | PASS |
| Windows desktop ZIP upload | PASS |
| Actual WPF click / layout automation | NOT RUN |
| Real original CHECKSYS GUI navigation | NOT RUN |
| Real original LE project GUI regression | NOT RUN |
| DOS executable runtime or G2 independent original ISA validation | NOT RUN |

The smoke suite includes the new independent direct-edge navigation
rules, but its successful synthetic tests do **not** prove that WPF
double-click events, text scrolling, tab state or all-references
checkbox behave correctly under real mouse interaction.

## Desktop-only distribution

- **Artifact name:** `DOSDis-Desktop-win-x64-preview`
- **GitHub artifact ID:** `11703189013`
- **ZIP archive size:** `66,906,804` bytes
- **GitHub artifact ZIP SHA-256:**
  `da73be2d543171f956307fa0eb51463e8528ad2f219154ee6b32bbcac55dc8f4`
- **Direct browser download:** https://github.com/Hyperbog/RLOFT/actions/runs/38143277116/artifacts/11703189013
- **Run page:** https://github.com/Hyperbog/RLOFT/actions/runs/38143277116

The SHA is of the uploaded ZIP archive, not of its contained
executable. The package is only the Windows desktop; **no CLI release**
or original proprietary executable was uploaded or published.

## Windows owner acceptance checklist

Use the unchanged exact-original Dungeon Hack `CHECKSYS.EXE` whose
SHA256 is `60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22`.

1. Launch the **new #44 desktop**, not older #43. Open CHECKSYS, then Analyze.
2. In Disassembly, **double-click `0000:0025 call 017Dh`**.
   It should select the resolved same-segment direct destination
   **`0000:017D`** in Disassembly. If navigation fails, retain
   screenshot/status and do not claim WPF acceptance. No far or
   indirect target must be fabricated.
3. With Hexadecimal selected, enter `0000:0025` and click Go.
   Position readout should show **`original file+0x00000225`**
   (first displayed row `file+0x00000220`).
   Enter `0000:017D` and repeat: physical **`0x37D`**
   (aligned first displayed row `0x370`).
   Repeated Go to an unchanged selected address must still
   refresh its position and scroll.
4. Use `file:0x225` to verify absolute original-file mode,
   and an invalid/unbacked CS:IP to check rejection.
5. References should initially list **all candidate graph edges**
   with Source, Destination, Kind and Evidence plus total count.
   The prior source-bound owner JSON had **394 edges**; counts
   may change only if analysis input/algorithm changes.
   Double-click a resolved reference: prior working behavior
   must remain functional. Tick **Selected instruction only**
   and confirm local incoming/outgoing subset. Unresolved
   dynamic/far destinations remain unknown.
6. Regression-check an original LE object's Go/Hex, references,
   Analyze, project save/reopen and export; MZ LE-only project
   storage remains deliberately disabled.

## Scope and follow-up

Earlier #43 WPF source-bound original
`CHECKSYS.EXE` MZ Analyze/JSON Export and static screenshots
were verified separately. This new gate addresses owner-reported
**interaction/navigation issues**, not verified control-flow
coverage beyond the entry-segment candidate graph.

All proprietary originals and private disassembly remain
owner-local. The canonical `.github/workflows/disassembler-core.yml`
must remain **`workflow_dispatch`-only**, with PR #13 **closed and
unmerged** after the build.

**Final scope:** WINDOWS DESKTOP BUILD + SYNTHETIC PASS; REAL MZ/LE GUI
NAVIGATION AND G2 INDEPENDENT ACCURACY **PENDING**.
