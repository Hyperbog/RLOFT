# RLOFT branch ownership and lifecycle (verified 2026-10-11)

## Seven retained branches

| Branch | Sole owning project / purpose | Treatment |
| --- | --- | --- |
| \`code1-decompilation\` | Default; CODE.1, CODE.2, Recomp, shared ResearchInfra, original DAT/RES/runtime research and central docs | **Active canonical source** |
| \`demo-v1\` | \`DemoV1/\` complete faithful native-game recreation | **Active native fidelity project** |
| \`native-engine\` | \`NativeEngine/\` independent install-backed native executable | **Active standalone product** |
| \`disassembler-foundation\` | \`Tools/Disassembler/\` Windows desktop DOSDis and independent validation | **Active disassembler** |
| \`extractor-wpf-fluent\` | \`Tools/\` GUI extraction and research implementations | **Active extractor** |
| \`DnD-Classics\` | Original preserved \`MASTER/\` and D&D game collection | **Immutable media; no housekeeping rewrites** |
| \`disassembler-g2-build-20261010-54247e\` | Distinct G2 validation candidate with two unmerged commits (workflow plus independent smoke-test changes) | **Review hold; do not delete or assume merged** |

**There are seven branches, not seven completed products.** Exact branch HEAD, source blob SHA, CI checkout and original-source acceptance remain separate. For original-game fidelity, retain the [game charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md).

## 2026-10-11 consolidation

[Audited branch retirement run #38179888132](https://github.com/Hyperbog/RLOFT/actions/runs/38179888132) succeeded: **14 obsolete branch refs retired, each first preserved as an exact SHA lightweight tag**. The old 21-branch tree is now seven branches. Recovery tags are \`refs/tags/archive/retired-20261011/<original-branch-name>\`; tags preserve historical commit/tree/CI provenance. Details and complete retired names are in [the exact branch cleanup audit](BRANCH_CLEANUP_AUDIT_20261011.md).

Across five **active development** branches (canonical, demo, native engine, disassembler, extractor), five historical CODE.1-only workflows were retired from each because their one-off job or obsolete accounting conditions were no longer suitable for current CI. The original media branch and the independent G2 review branch were not rewritten. The active project workflows remain in place.

## Safe branch lifecycle rules

1. **Do not create a long-lived branch solely for a one-time Windows build.** Prefer a manual dispatch on the owning workstream where practical, or a temporary branch with a documented source SHA and explicit retirement step.
2. **Retire only after concrete comparisons**, not names: inspect current SHA, merge base, ahead/behind commits, every unique changed path, open PRs and branch protection. An unmerged source/test change is a stop condition, even on a build-named branch.
3. **Archive first.** Record an immutable source SHA and establish/re-read a recovery tag before deleting the branch ref. Never delete an original-media branch, source owner branch, or protected/active PR branch.
4. **Do not wholesale merge divergently owned trees.** Synchronize only bounded, independently proven shared source changes and record exact blob identities in [the shared source register](SHARED_SOURCE_REGISTER.md).
5. **Keep owner-specific files in their own directory/branch.** The shared [Research Infrastructure](../ResearchInfra/README.md) is an evidence index, not a second copy of each project. Use the [project registry](../ResearchInfra/project_registry.json) for machine-readable ownership.
6. **Evidence is graded.** Parser findings are not original-byte proofs; compilation is not DOS equivalence; Windows/native CI does not establish playable original game acceptance. Preserve the exact accepted checkout.
7. **No original media rewrites, release rewrite, auto-merge or new Windows Full run** is authorized by documentation or housekeeping alone.

Recreate an archived branch only when genuinely needed: \`git switch -c <new-name> archive/retired-20261011/<old-name>\`. Do not re-use retired one-shot branch names for ongoing development.
