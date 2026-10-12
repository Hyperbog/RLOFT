# DOSDis #45 Win-x64 desktop Build Gate — blocked before runner allocation

**Owner approval:** “Build” (2026-10-11 local EDT).
**Disposition:** **BLOCKED INFRASTRUCTURE / NO BUILD STARTED / NO ARTIFACT**.
No new compiled/synthetic test checkpoint or source-accuracy claim.
The latest actually verified Windows desktop binary remains
[#44](https://github.com/Hyperbog/RLOFT/actions/runs/38143277116).

## Exact canonical source under test

- Canonical pinned snapshot:
  `1f8b6b9d4a8f58965c9cd4979809e49db6558bf6`
  on `disassembler-foundation` at gate start.
- Compared against the prior byte-search source milestone
  `bc56763ab9fe3c7b3e02673b586e6332ebee3ca5`:
  **three subsequent repository commits affected unrelated
  Code1/Code2 sources and other workflows only**, not
  `Tools/Disassembler`. No external original game bytes
  were uploaded to hosted CI.
- Isolated temporary branch:
  `dosdis-windows-byte-search-gate-20261011-1f8b6b`.
- Temporary PR: [#14](https://github.com/Hyperbog/RLOFT/pull/14),
  **DO NOT MERGE**. Workflow modifications on temporary
  branch were ONLY for attempting this explicitly
  owner-authorized gate. Canonical workflow should remain
  `workflow_dispatch`-only and publish only Windows desktop,
  never CLI.

## Failed attempts — runner never allocated

| Run | Host / event | Observation |
| --- | --- | --- |
| [#45](https://github.com/Hyperbog/RLOFT/actions/runs/38193004540) | Windows / isolated PR, initial attempt | FAILURE; `windows-core` job had `steps=[]`, `runner_id=0`; no compiler or tests |
| Same #45, second attempt | Re-run failed jobs after transient allowance | FAILURE; replacement Windows job again `steps=[]`, `runner_id=0`; GitHub reported 0 billable Windows milliseconds |
| [#46](https://github.com/Hyperbog/RLOFT/actions/runs/38193208259) | Linux / isolated PR, Windows-targeted WPF cross-build fallback | FAILURE; Linux job had `steps=[]`, `runner_id=0`; no compiler/test invocation |
| [#47](https://github.com/Hyperbog/RLOFT/actions/runs/38193253571) | Linux / branch-scoped temporary push-triggered fallback | FAILURE; Linux job had `steps=[]`, `runner_id=0`, terminated ~3s after start; no compiler/test invocation; 0 artifacts |

For #45 the log download returned GitHub storage HTTP 404
`BlobNotFound` because the job had no steps/log to retrieve.
All structured job results returned failure immediately with
no runner, and no .NET step executed. This cannot establish
a C#/XAML failure or an accuracy regression; **the code is
unbuilt and untested** at this gate.

Recent unrelated repository Linux push jobs (for example,
`38192758031`, Research Infrastructure) succeeded shortly
before these blocked attempts. The actual root cause of
the runner-allocation failures was **NOT available in the
read-only GitHub job API**; possibilities include GitHub
Actions billing, permission/approval policy, organization/
repository limits or runner provisioning. Do not assert a
specific cause without looking at the run's GitHub UI banner,
billing and Actions settings.

The Linux fallback workflow attempted only platform-neutral
original-free smoke suites and a Win-x64 WPF cross-target
build using `EnableWindowsTargeting=true`; whether WPF
can cross-publish in this environment was NOT tested,
because it never got a Linux runner. Never call its failure
a WPF cross-target compiler failure.

## Next steps and gate reopening

1. User opens the failed [#47](https://github.com/Hyperbog/RLOFT/actions/runs/38193253571)
   or [#45](https://github.com/Hyperbog/RLOFT/actions/runs/38193004540)
   run in GitHub's web UI and reviews the top-level
   failure/runner allocation message. Account owners can
   check GitHub Actions billing/usage and the repository's
   Actions execution permissions; ask for the displayed
   banner if root cause needs assistance.
2. After runner availability is restored, authorize
   rerunning the **canonical Windows WPF desktop-only
   build workflow** at a fresh pinned SHA; use an isolated
   temporary gate only if needed. Do not merge temporary
   triggers or publish any CLI package.
3. Require Windows WPF, both original-free C# synthetic
   test suites and one Win-x64 desktop publish to pass.
   New `HexByteNavigation` synthetic cases cover
   click-map original offsets, LE `??`, malformed hex
   patterns and next/wrapped find.
4. User verifies Enter-to-Go/Find, clicking individual
   `CD` or `09` byte tokens, copy selection,
   exact source offsets, highlight in Disassembly, Hex
   and References, and preserved old MZ/LE navigation.

**Current status:** Source authored and committed;
Build #45-#47 **BLOCKED BEFORE RUNNER START**;
no new ZIP; original independent G2 accuracy pending.
