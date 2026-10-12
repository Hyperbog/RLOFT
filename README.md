# Ravenloft: Strahd's Possession — reconstruction and preservation

This repository preserves and reverse-engineers the original DOS game and develops a **complete faithful native game implementation**. The current objective is to recreate the whole original game before modernization or gameplay alterations. This is a research/engineering project, **not** a finished original-DOS-equivalent release.

## Project documentation

**[Central documentation index](docs/INDEX.md)** · [Current cross-workstream status](docs/CURRENT_STATUS.md) · [Branch ownership](docs/BRANCH_OWNERSHIP.md) · [Shared reconstructed source register](docs/SHARED_SOURCE_REGISTER.md) · [Project-specific documentation policy](docs/DOCUMENTATION_POLICY.md)

| Workstream | Owning project documents |
| --- | --- |
| Shared research infrastructure | [Deterministic source index and project ownership](ResearchInfra/README.md); [branch cleanup audit](docs/BRANCH_CLEANUP_AUDIT_20261011.md) |
| Canonical recompilation | [Executable linker/startup and original LE-image integration](Recomp/README.md) |
| Reconstructed executable CODE.1 | [`Code1/README.md`](Code1/README.md) |
| Reconstructed executable CODE.2 | [`Code2/README.md`](Code2/README.md) |
| Native game engine (`demo-v1`) | [`DemoV1/README.md`](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md); [original-game fidelity charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) |
| Standalone installed-game engine (`native-engine`) | [Engine product contract](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/README.md); [M-ENGINE-01 first native playtest](https://github.com/Hyperbog/RLOFT/issues/9) — separate executable, user installation assets, first playable-state assessment; **not built/accepted yet** |
| DOSDis disassembler (`disassembler-foundation`) | [`Tools/Disassembler/README.md`](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md) |
| Asset extractor (`extractor-wpf-fluent`) | [`Tools/anm/README.md`](https://github.com/Hyperbog/RLOFT/blob/extractor-wpf-fluent/Tools/anm/README.md) and [`Tools/README.md`](Tools/README.md) |
| DAT, resource and runtime format research | [`DatFiles/`](DatFiles/README.md), [`Resources/`](Resources/README.md), [`Runtime/`](Runtime/README.md) |
| Original-game files (`DnD-Classics`) | [Preserved `MASTER/` game folders](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) |

**Project-specific documents remain in their existing source directories and owner branches.** Central pages link to them; they do not override, duplicate, move or erase original reverse-engineering evidence. The project-specific charter controls game-facing source verification.

## Verification and distribution

Complete **original instruction-byte accounting is not complete semantic or runtime equivalence**. Keep compiler/test pass status separate from independently accepted Windows builds, original indexed VGA/palette, RNG/IRQ, full-level events and original DOS comparisons.

**Build authorization (updated 2026-10-11):** The owner has expressly lifted the per-build manual-approval restriction. Automated source-triggered CI and on-demand Windows/native/disassembler builds may run whenever required for development, on the appropriate owning branch and with the existing workflow prerequisites. A successful build is still distinct from original-DOS or gameplay parity. Original copyrighted game files remain evidence/preservation media and are not automatically redistributable or publishable in Actions artifacts.

See the [current status](docs/CURRENT_STATUS.md) for dated acceptance boundaries.
