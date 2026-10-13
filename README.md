# Ravenloft: Strahd's Possession — enhanced PC port and original-game research

**Current player-facing goal (owner decision, October 13, 2026):** a complete **enhanced native Windows PC port** preserving the original campaign, art, story and game rules while modernizing graphics presentation, frame pacing, sound, input and saves. People seeking unchanged DOS-era behavior can run their original game through DOSBox. Research into original code and assets continues where helpful, but exact DOS CPU/VGA/IRQ, timing or save-format parity is **not a prerequisite** to the port. The playable game remains Pre-Alpha.

## Shared project coordination

The [cross-workstream documentation index](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/INDEX.md), [current status](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/CURRENT_STATUS.md), [branch ownership](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/BRANCH_OWNERSHIP.md) and [shared CODE synchronization register](https://github.com/Hyperbog/RLOFT/blob/code1-decompilation/docs/SHARED_SOURCE_REGISTER.md) live on the default branch. **Original-game reverse-engineering records, binary provenance and historical fidelity documentation remain preserved in `DemoV1/`** as optional reference material, not as modern release gates.

## Start here

| Project area | Active documentation |
| --- | --- |
| Native executable and gameplay (`demo-v1`) | [Engine README](DemoV1/README.md), **[active enhanced port charter](DemoV1/ENHANCED_PC_PORT_CHARTER.md)** and [Alpha/Beta/Final milestones](DemoV1/RELEASE_MILESTONES_ALPHA_BETA_FINAL.md) |
| Reconstructed executable code | [CODE.1](Code1/README.md) and [CODE.2](Code2/README.md) |
| Binary formats and data investigation | [DatFiles](DatFiles/) and [Tools](Tools/) |
| Development history | [Full preserved native development log](DemoV1/DEVELOPMENT_LOG_ARCHIVE_20261010.md) |
| Historical reverse-engineering / subsystem research | [Engineering subsystem report](DemoV1/PROGRESS_SUBSYSTEM_REPORT_20261009.md) and [superseded original-fidelity charter](DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) |

**Repository branches are distinct workstreams.** `demo-v1` is the native game engine; `code1-decompilation` maintains executable reconstruction; `disassembler-foundation` maintains the disassembly tooling; `DnD-Classics` preserves original game data. Reconcile changes through small tested commits, actual Windows gameplay and original data/code research where relevant—**not by merging divergent branches wholesale**.

## Validation policy

- **Port acceptance is playable-game driven.** Native Windows field runs, a continuous full intro → character creation → visible Level 22, full authored campaign, safe native saves, and reliable graphics/audio/controls are the engineering goals.
- Windows builds run automatically on relevant `demo-v1` source commits; build success and CTests are separate from player-visible gameplay acceptance.
- Use original `CODE.1`/`CODE.2`, DAT/ANM/RES resources and DOSBox comparison to understand missing content or mechanics **when useful**. Exact hardware or instruction-order equivalence is optional, not a release blocker.
- Keep unlicensed original game assets out of public development and release artifacts; let players use their legitimately owned copy. Modern safety fixes and usability enhancements are authorized. Do not claim a gameplay feature is implemented before it is player-tested.

Consult the [engine README](DemoV1/README.md) for the latest acceptance boundary, build instructions and research links. Historical `certified` terminology in earlier reports denotes an archival milestone, **not complete semantic or runtime verification**.
