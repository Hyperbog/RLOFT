# Demo v1 — Full Windows Build #648 (2026-10-09)

**Verification status:** SUCCESS. [GitHub Actions run #37998467852](https://github.com/Hyperbog/RLOFT/actions/runs/37998467852).

| Item | Verified value |
| --- | --- |
| Workflow | `.github/workflows/demo-v1-build.yml` |
| GitHub Actions run number | **648** |
| Run identifier | `37998467852` |
| Event | Explicitly authorized, one-time marker-only push |
| Build checkout/source SHA | `fb0912346a831dad21d2960e99863cc8f446173a` |
| Windows MSVC compilation | **SUCCESS** |
| Original CODE.2 C++17 syntax check | **SUCCESS** |
| Full CTest suite | **178/178 passed** (100%) |
| Linux | Not requested, skipped |
| Build verification mode | Full |
| Windows x64 artifact | `Ravenloft-Demo-v1-Windows-x64`, artifact `11648122516` |
| Published build artifact size | 798,738 bytes compressed |
| Native EXE SHA-256 | `b2d868b35f9e283c24746aa96b1aa94d671377be81008b1433277ef359468f16` |
| Release binary in Git | `DemoV1/Windows/ravenloft_demo_v1.exe` |
| Release binary checksum in Git | `DemoV1/Windows/ravenloft_demo_v1.exe.sha256` |
| Verified build completion | 2026-10-09 22:22:52 UTC |

## Fidelity interpretation

This build compiled all registered reconstruction sources and passed all **178** native CTests, including the new Level22 original pickup, staged mist timeline, original event flag and continuity regression targets. It proves the current source compiles and its native assertions pass on Windows.

It **does not** establish that Level22 is completely playable. The player attack pipeline, actual original VGA pickup ribbon draw, all source-authored gameplay interactions, complete original 22-frame mist presentation, and original DOS state/pixel/IRQ acceptance remain open. Current-head DOS acceptance remains **0%**; the existing weighted readiness scoreboard remains **49.8%**, 20 systems/122 subsystems.

The build's source SHA includes the one-time workflow marker and transient workflow edit, which affect **CI trigger configuration only**, not game source. Both were removed/restored immediately after the authorized run was queued. The published EXE was committed to the latest `demo-v1` branch by the successful workflow at commit `146a5a89975c40ed5c32347d80af0b4b5f1b87fc`.

## Build policy unchanged

`.github/workflows/demo-v1-build.yml` has been restored **byte-for-byte** to the previous **manual-only** workflow. The single marker file `DemoV1/.run-full-windows-once-20261009` has been removed. Ordinary Git commits do **not** dispatch a build. Future builds require explicit authorization.

## User validation

Download the `Ravenloft-Demo-v1-Windows-x64` artifact from [run #648](https://github.com/Hyperbog/RLOFT/actions/runs/37998467852), or use the committed `DemoV1/Windows/ravenloft_demo_v1.exe` and its SHA-256 record. The bundle contains `Explore-Level22.cmd`, `Capture-Level22.cmd`, `Probe-Level22-Ground.cmd`, `Audit-Starting-Equipment.cmd` and related source probes. Launch from a correctly configured original Ravenloft game installation. Do not treat a successful developer scene launch as original-DOS feature acceptance without comparison.

**Project priority:** Finish fully playable, source-faithful Level22 **through the original mist ending**; defer independent Level23 gameplay.
