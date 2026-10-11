# RLOFT verified branch and workflow consolidation — 2026-10-11

**Source:** Live GitHub tree/commit compare of all 21 refs, verified before deletion. [Successful guarded GitHub Actions run #38179888132](https://github.com/Hyperbog/RLOFT/actions/runs/38179888132). The action rejected changed SHAs, protected heads, open PR heads, or any branch with a unique non-workflow source change. It archived and re-read all fourteen tags **before** deleting any branch. Original-media and current owner branches were not touched.

## Final branch inventory (7)

\`code1-decompilation\`; \`demo-v1\`; \`native-engine\`; \`disassembler-foundation\`; \`extractor-wpf-fluent\`; \`DnD-Classics\`; \`disassembler-g2-build-20261010-54247e\` (unique review candidate).

## Exact fourteen retired refs

The recovery tag for every row is:
\`refs/tags/archive/retired-20261011/<retired-name>\`.

| Retired name | Preserved commit SHA prefix | Proof of redundancy |
| --- | --- | --- |
| \`native-engine-win64-build-20261011-0647\` | \`d71823f20c55\` | one workflow-only commit beyond historical canonical base |
| \`native-assets-win64-20261011-1415-once\` | \`bb2e29f5a952\` | one workflow-only commit |
| \`native-browser-win64-20261011-1610-once\` | \`11865739a908\` | one workflow-only commit |
| \`native-colors-win64-20261011-1730-once\` | \`14593e03e240\` | one workflow-only commit |
| \`native-complete-content-win64-20261011-once\` | \`2341129ef41a\` | one workflow-only commit |
| \`native-eh6-world-textures-win64-20261011-once\` | \`180f59a2a1b4\` | one workflow-only commit |
| \`native-fullbank-win64-20261011-once\` | \`e8b38433dc55\` | one workflow-only commit |
| \`native-palette-router-win64-20261011-once\` | \`91f8231b3d94\` | one workflow-only commit |
| \`native-raster-win64-20261011-1530-once\` | \`c10064f5e150\` | one workflow-only commit |
| \`disassembler-mz-desktop-build-20261010-244718\` | \`e5f520aa07e9\` | one workflow-only commit |
| \`dosdis-windows-navigation-gate-20261011-9e1823\` | \`fb54de74c287\` | one workflow-only commit |
| \`disassembler-g2-build-20261010\` | \`2945b1474289\` | fully ancestor of active disassembler |
| \`disassembler-g2-validation-20261010\` | \`5754331f4423\` | fully ancestor of active disassembler |
| \`preservation-backup-code1-2026-10-07\` | \`373c7f10e6fc\` | fully ancestor of canonical source |

**Important:** A build branch can contain only an additional workflow *even though the build output is unique*. This is why its exact original checkout SHA and Actions run are preserved as tags rather than discarded from Git history. Do not confuse these temporary branches with the independent \`native-engine\` or \`demo-v1\` products.

## Five obsolete historical CODE.1 workflows retired from each active development branch

Paths removed on \`code1-decompilation\`, \`demo-v1\`, \`native-engine\`, \`disassembler-foundation\`, \`extractor-wpf-fluent\`:

- \`.github/workflows/integrate-d23a.yml\` — archived one-off CODE.1 close verifier tied to historical count
- \`.github/workflows/integrate-d573.yml\` — referenced deleted \`Code1/integrate_d573.py\` and still had write permissions
- \`.github/workflows/code1-audit-snapshot.yml\` — historical two-day source snapshot uploader
- \`.github/workflows/residual-normalized-all-history.yml\` — historical all-history residual research run
- \`.github/workflows/residual-audit.yml\` — obsolete 69,360-byte assertions versus current 69,361-byte closure

This removes **25 tracked workflow files**, five on each owner branch. Actual source code and historical audit documents were not deleted. Workstream cleanup commits: \`demo-v1:200f8dd44e8d\`, \`disassembler-foundation:bec1316921a1\`, \`extractor-wpf-fluent:29de5adc73af\`, \`native-engine:59f2331b4cc5\`, \`code1-decompilation:d3b6a4ee7b38\`.

## Remaining review hold

\`disassembler-g2-build-20261010-54247e\` was not retired. Against \`disassembler-foundation\`, it has **two unique commits**, touching \`.github/workflows/disassembler-core.yml\` and \`Tools/Disassembler/tests/DosDis.Smoke/Dos16ExternalOracleSmoke.cs\`. Reconcile the genuinely different test semantics and G2 evidence before promotion or archival.

## Build/acceptance boundaries

No game, DAT, RES, CODE.1/CODE.2 source, asset extractor, native renderer, or disassembler implementation was changed by this housekeeping batch. No current-SHA Windows Full or original DOS parity test follows from removing historical workflows.
