# Documentation ownership, cleanup, and automatic maintenance

**Effective:** 2026-10-17. Keep a clear separation between **current** guidance, **live GitHub observations**, and **historical source evidence**.

## Source of truth

- Root [README](../README.md): small branch/project landing page; updated when names or scope change.
- [INDEX](INDEX.md) and [BRANCH_OWNERSHIP](BRANCH_OWNERSHIP.md): human-maintained stable navigation and workstream policy.
- [CURRENT_STATUS](CURRENT_STATUS.md): manually reviewed *gameplay and reverse-engineering acceptance*, with named date/commit/test evidence.
- [LIVE_STATUS](LIVE_STATUS.md): **generated** by `docs/tools/update_live_status.py`; exact observed GitHub branch heads, latest completed workflow per branch, and open PRs. The generator cannot certify that game visuals/audio/controls work.
- `DemoV1/`, `NativeEngine/`, `Code1/`, `Code2/`, `Recomp/` and `Tools/`: local source documents and dated proof. Large old README checkpoint histories may be renamed to `README_HISTORY_YYYYMMDD.md` **within the same directory** to retain relative links.
- Original-game media and original-byte analysis remain outside public source artifacts, and `DnD-Classics` is never modified by housekeeping.

## Update cadence

The [documentation-health workflow](../.github/workflows/documentation-health.yml) runs on a daily UTC schedule, on-demand, and on changes to its own script/tests. It validates the generator/tests and writes `docs/LIVE_STATUS.md` only if content changed. Pushes use `[skip ci]` and a regular, non-force, fast-forward-safe Git push. Workflow failure must not delete a previous live report. New documentation checks must not trigger Windows Full, player builds, or original-media redistribution.

Owners must manually update `CURRENT_STATUS.md` and their relevant READMEs after meaningful source or acceptance changes; an automated green test is never evidence of player acceptance. Never add a dated status headline to the top of a README for every individual commit—put details in the relevant dated checkpoint/historical file instead.

## Cleanup rules

Delete only truly redundant, obsolete one-off status stubs after confirming the underlying link/run and history are retained. Preserve unique investigation evidence, original offsets, game logic, provenance, long-form audits and continuity. Do not auto-delete research documents using age, size, or filename patterns. Renaming/moving cross-branch documents requires updating their links and affected builds. Generated files are identified explicitly; do not manually edit them.

The requested default-branch rename to `Decomp` is pending a GitHub-admin action (automatic rename returned 403). Until completed, `code1-decompilation` is the live default. All new docs and CI updates must use real current branch names.
