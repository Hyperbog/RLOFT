# DOSDis G2 authorized Windows/Linux build and synthetic verification — run #42

**Owner authorization:** "You may build" (2026-10-10).
**Scope:** Windows WPF/CLI, Linux CLI, .NET 10 C# smoke suites,
synthetic-only Python research/preflight tests and preview artifacts.
**Status:** COMPILED / SYNTHETIC VERIFIED (scoped). **Not** an
original-game independent relocation comparison, runtime reachability
check, Windows GUI-user acceptance or complete G2 accuracy acceptance.

## Exact source/build identity

- **Canonical application code/test SHA verified at this gate:**
  `3956f036fee44c0b6f07361811e4f0857dbea916`.
- **Temporary PR #8, never merge:**
  https://github.com/Hyperbog/RLOFT/pull/8
- **PR head (temporary verification workflow):**
  `e9239b7e820517f8798ac73f4616287802c38bb1`.
- **Actual Windows/Linux GitHub checkout / ephemeral PR merge SHA:**
  `dd251ba7479fe802e17155a2740d3519cf23847b`.
- **Source equivalence verified:** comparing the canonical SHA to the
  exact PR merge reports just ONE changed path:
  `.github/workflows/disassembler-core.yml`, temporary
  `pull_request` trigger plus Linux/Python jobs. All actual C#/Python
  application and test source bytes in the compiled checkout are
  canonical-code/test-equivalent.
- **Successful run:**
  https://github.com/Hyperbog/RLOFT/actions/runs/38096831427
  (`run_number=42`, `run_id=38096831427`,
  2026-10-10 Eastern local date, completion 2026-10-11 00:00 UTC).
- **Superseded failed run:**
  https://github.com/Hyperbog/RLOFT/actions/runs/38096753652
  (`run_number=41`, `run_id=38096753652`).

## Failed gate #41 and exact correction

Both Linux and Windows .NET compilation succeeded in #41, but the C#
synthetic suite failed in `Dos16ExternalOracleSmoke.Run` at
`sample identity hashes actual original bytes, never forged oracle hex`.

The `wrongStored` fixture incorrectly reduced a TWO-window original
instruction sample to ONE benchmarked expected instruction while
asserting that the sample byte digest should remain unchanged.
The changed sample membership **must** change its digest; this was
a malformed negative-control assertion, not demonstrated decoder
failure. The test now keeps both original-backed instruction windows,
but changes only the first expected original hex claim from `EB02`
to `EB03`. Its independently defined expected sample digest remains
valid with equal sample membership and source bytes. The repair was
committed to canonical `3956f036` and copied byte-identically into
the temporary PR branch. #41 was **not a pass** and no artifacts were
published by its failed jobs.

## Successful gate #42 evidence

| Verification component | Result |
| --- | --- |
| Windows .NET 10 desktop, CLI, DosDis.Smoke and DosDis.ProjectSmoke | **PASS**, zero build warnings/errors |
| Linux .NET 10 CLI, DosDis.Smoke and DosDis.ProjectSmoke | **PASS**, zero build warnings/errors |
| Windows DosDis.Smoke and DosDis.ProjectSmoke synthetic suites | **PASS** |
| Linux DosDis.Smoke and DosDis.ProjectSmoke synthetic suites | **PASS** |
| Linux Python `python3 -m unittest discover -s research -p 'test_*.py' -v` | **PASS**, **17 tests** |
| Windows self-contained CLI publish | **PASS** |
| Windows self-contained desktop publish | **PASS** |
| Linux self-contained CLI publish | **PASS** |
| GitHub Actions artifact uploads | **3 / 3 PASS** |
| Proprietary original DOS file execution or independent original-file comparison | **NOT RUN** |
| Manual Windows GUI owner acceptance | **NOT RUN** |
| Independent human accuracy review and G2-wide accuracy acceptance | **PENDING** |

`Dos16ExternalOracleSmoke` reports its synthetic-only oracle
comparison contract PASS on both platforms. The new SHA-bound
source-map/relocation comparison utilities and synthetic independent
report reconciliation are included in the 17 passing Python cases.
This validates testability and contracts under synthetic byte cases
only, not independent original-game ISA/runtime behavior.

## Published GitHub artifacts and archive fingerprints

Artifact IDs, sizes and ZIP archive SHA256 values are from the
run's GitHub artifact metadata and successful upload job logs.
These are **GitHub artifact ZIP archive digests**, not hashes of
each contained executable; do not confuse them with code/test
source SHAs.

| Artifact | ID | ZIP bytes | GitHub artifact-upload ZIP SHA-256 |
| --- | ---: | ---: | --- |
| `DOSDis-Desktop-win-x64-preview` | `11686885378` | 66,889,850 | `c70c048fde95c0f08386af52a2b0278aa39230d1404ff8c82bb07d12ea5c8127` |
| `DOSDis-CLI-win-x64-preview` | `11686550805` | 33,613,883 | `efd3b7889e409dd7b3b1884c2d9522edaacffe1305ef679cf777ab027ed85b03` |
| `DOSDis-CLI-linux-x64-G2-local-validation` | `11686021352` | 33,575,602 | `98159cdf78a365bed82451149489445ae45f3464a658c93fcb15d18933766091` |

Artifacts: https://github.com/Hyperbog/RLOFT/actions/runs/38096831427

This is a **temporary PR-only** verification mechanism. It must
remain **closed and unmerged**, with canonical
`.github/workflows/disassembler-core.yml` left `workflow_dispatch`-
only. Node.js 20 action deprecation warnings were emitted during
workflow housekeeping by `actions/checkout@v4`,
`setup-dotnet@v4` and `upload-artifact@v4`; they did not fail
the .NET compilation or regression checks.

## Next owner-authorized gate

The exact SHA-256 original Dungeon Hack `CHECKSYS.EXE` is available
in the `DnD-Classics/MASTER` collection. A future
**separately authorized** owner-local independent comparison should:

1. Use the exact original SHA256 and a **hypothetical, explicitly
   shared** MZ load-segment assumption (for example 2000h).
2. Run the independent Python SHA/geometry/fixup preflight and
   the compiled DOSDis `mz-relocate --load-segment 2000 --max 0`
   on the identical original input; compare with the strict
   five-field reconciliation script and record discrepancies.
3. Independently decode predeclared original instruction windows
   using a separate ISA decoder; do not use DOSDis candidate
   output as a ground-truth oracle.
4. Keep original game bytes and private original-byte reports
   out of hosted CI, public commits and preview packages.
5. Obtain the required second evidence class and separate
   human discrepancy review before claiming **G2 INDEPENDENT
   VERIFIED** for any complete format scope.

**Final gate classification:** COMPILED and SYNTHETIC VERIFIED
for these named Windows/Linux builds and synthetic suites at
the exact checkout above. G2 independent accuracy remains
**PENDING**.
