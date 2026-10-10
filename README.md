# Ravenloft: Strahd's Possession — reconstruction and preservation

This repository preserves and reverse-engineers the original DOS game and develops a **complete faithful native game implementation**. The current objective is to recreate the whole original game before modernization or gameplay alterations. This is a research/engineering project, **not** a finished original-DOS-equivalent release.

## Project documentation

**[Central documentation index](docs/INDEX.md)** · [Current cross-workstream status](docs/CURRENT_STATUS.md) · [Branch ownership](docs/BRANCH_OWNERSHIP.md) · [Shared reconstructed source register](docs/SHARED_SOURCE_REGISTER.md) · [Project-specific documentation policy](docs/DOCUMENTATION_POLICY.md)

| Workstream | Owning project documents |
| --- | --- |
| Reconstructed executable CODE.1 | [`Code1/README.md`](Code1/README.md) |
| Reconstructed executable CODE.2 | [`Code2/README.md`](Code2/README.md) |
| Native game engine (`demo-v1`) | [`DemoV1/README.md`](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md); [original-game fidelity charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ORIGINAL_EXECUTION_FIDELITY_CHARTER.md) |
| DOSDis disassembler (`disassembler-foundation`) | [`Tools/Disassembler/README.md`](https://github.com/Hyperbog/RLOFT/blob/disassembler-foundation/Tools/Disassembler/README.md) |
| Asset extractor (`extractor-wpf-fluent`) | [`Tools/anm/README.md`](https://github.com/Hyperbog/RLOFT/blob/extractor-wpf-fluent/Tools/anm/README.md) and [`Tools/README.md`](Tools/README.md) |
| DAT, resource and runtime format research | [`DatFiles/`](DatFiles/README.md), [`Resources/`](Resources/README.md), [`Runtime/`](Runtime/README.md) |
| Original-game files (`DnD-Classics`) | [Preserved `MASTER/` game folders](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) |

**Project-specific documents remain in their existing source directories and owner branches.** Central pages link to them; they do not override, duplicate, move or erase original reverse-engineering evidence. The project-specific charter controls game-facing source verification.

## Verification and distribution

Complete **original instruction-byte accounting is not complete semantic or runtime equivalence**. Keep compiler/test pass status separate from independently accepted Windows builds, original indexed VGA/palette, RNG/IRQ, full-level events and original DOS comparisons.

Native Windows Full CI builds and disassembler builds remain manually owner-authorized. Source commits or documentation housekeeping do not dispatch them. Original copyrighted game files are evidence/preservation media, not automatically redistributable source assets.

See the [current status](docs/CURRENT_STATUS.md) for dated acceptance boundaries.
