> **Decompilation workstream (currently `code1-decompilation`).** New decompilation and original-machine analysis work lives here; see [Decomp/README.md](Decomp/README.md). **Recompilation development is separate:** [`recomp` branch → `Recomp/`](https://github.com/Hyperbog/RLOFT/tree/recomp/Recomp). The requested `Decomp` branch rename requires a GitHub administrator action; the automated attempt was denied (403). This split retains shared original-source inputs while removing the old recompilation build subtree from the default branch. The playable enhanced game remains on [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1).

# Ravenloft: Strahd's Possession — enhanced PC port and original-game preservation

**Owner direction, October 13, 2026:** develop a **complete enhanced native PC port** of the game. Original files and all reverse-engineering/decompilation/recompilation work remain valuable research and compatibility resources, but exact DOS x86, VGA, IRQ, mixer, timing and memory/save ABI parity are **not release prerequisites**. Players seeking the unmodified game may use their original copy in DOSBox. The native port remains Pre-Alpha; the authored campaign must still be fully playable and preserve its recognizable content, story and mechanics.

**Universal mod-forward methodology:** [Shared engineering standard](MOD_FORWARD_METHODOLOGY.md) for replaceable game resources, stable content IDs, extensible menus, safe mod packages and editor-ready data across DOS/NES/SNES/SMS/Genesis ports. For RLOFT it is a design policy, **not** evidence that mod loading or editors are already implemented. Existing [workstream branch ownership](docs/BRANCH_OWNERSHIP.md) and original-game preservation rules remain authoritative.

## Project documentation

**[Central documentation index](docs/INDEX.md)** · [Current cross-workstream status](docs/CURRENT_STATUS.md) · [Branch ownership](docs/BRANCH_OWNERSHIP.md) · [Shared reconstructed source register](docs/SHARED_SOURCE_REGISTER.md) · [Project-specific documentation policy](docs/DOCUMENTATION_POLICY.md)

| Workstream | Owning project documents |
| --- | --- |
| Shared research infrastructure | [Deterministic source index and project ownership](ResearchInfra/README.md); [optional REA/Ghidra analysis for all projects](ResearchInfra/REA.md); [branch cleanup audit](docs/BRANCH_CLEANUP_AUDIT_20261011.md) |
| Canonical recompilation | [Executable linker/startup and original LE-image integration](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/README.md) |
| Reconstructed executable CODE.1 | [`Code1/README.md`](Code1/README.md) |
| Reconstructed executable CODE.2 | [`Code2/README.md`](Code2/README.md) |
| Native game engine (`demo-v1`) | [`DemoV1/README.md`](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md); **[active enhanced PC port charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ENHANCED_PC_PORT_CHARTER.md)**; [release milestones](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/RELEASE_MILESTONES_ALPHA_BETA_FINAL.md) |
| Standalone installed-game engine (`native-engine`) | [Engine product contract](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/README.md); [M-ENGINE-01 first native playtest](https://github.com/Hyperbog/RLOFT/issues/9) — separate executable, user installation assets, first playable-state assessment; **not built/accepted yet** |
| DOSDis disassembler (`DosDis`) | [`Tools/Disassembler/README.md`](https://github.com/Hyperbog/RLOFT/blob/DosDis/Tools/Disassembler/README.md) |
| Asset extractor (`Ravenloft-Asset-Extractor`) | [`Tools/anm/README.md`](https://github.com/Hyperbog/RLOFT/blob/Ravenloft-Asset-Extractor/Tools/anm/README.md) and [`Tools/README.md`](Tools/README.md) |
| DAT, resource and runtime format research | [`DatFiles/`](DatFiles/README.md), [`Resources/`](Resources/README.md), [`Runtime/`](Runtime/README.md) |
| Original-game files (`DnD-Classics`) | [Preserved `MASTER/` game folders](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) |

**Project-specific documents remain in their existing source directories and owner branches.** Central pages link to them; they do not override, duplicate, move or erase original reverse-engineering evidence. The active enhanced-port charter controls new native gameplay and release decisions; historical strict-DOS research contracts remain preserved but superseded as game release requirements.

## Verification and distribution

Complete original instruction-byte accounting does not by itself make a working game. Prioritize independent Windows gameplay field tests for visuals, audio, controls, Level 22 and start-to-ending campaign progression. DOS indexed VGA/palette, RNG/IRQ and code-byte comparisons remain optional tools for researching the intended rules and assets.

**Build authorization (updated 2026-10-11):** The owner has expressly lifted the per-build manual-approval restriction. Automated source-triggered CI and on-demand Windows/native/disassembler builds may run whenever required for development, on the appropriate owning branch and with the existing workflow prerequisites. A successful build is still distinct from actual native Windows gameplay acceptance. Original copyrighted game files remain evidence/preservation media and are not automatically redistributable or publishable in Actions artifacts.

See the [current status](docs/CURRENT_STATUS.md) for dated acceptance boundaries.
