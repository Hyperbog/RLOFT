# Ravenloft: Strahd's Possession — project index

**Goal:** one enhanced, native Windows version of the complete original Ravenloft campaign, preserving authored content, with future modding support. **Current playable-game status: Pre-Alpha**; Windows build/tests do not establish that the graphics, menus, audio, saves or full campaign work for players.

## Workstreams

| Workstream | Branch | Start here |
| --- | --- | --- |
| Enhanced playable game | [`demo-v1`](https://github.com/Hyperbog/RLOFT/tree/demo-v1) | [Game README](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/README.md), [controlling charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ENHANCED_PC_PORT_CHARTER.md) |
| Original code decompilation | [`code1-decompilation`](https://github.com/Hyperbog/RLOFT/tree/code1-decompilation) | [Decomp](Decomp/README.md), [CODE.1](Code1/README.md), [CODE.2](Code2/README.md) |
| Source recompilation | [`recomp`](https://github.com/Hyperbog/RLOFT/tree/recomp) | [Recomp guide](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/README.md) |
| Standalone experimental engine | [`native-engine`](https://github.com/Hyperbog/RLOFT/tree/native-engine) | [Engine README](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/README.md) |
| Disassembler | [`DosDis`](https://github.com/Hyperbog/RLOFT/tree/DosDis) | [DOSDis README](https://github.com/Hyperbog/RLOFT/blob/DosDis/Tools/Disassembler/README.md) |
| Asset extraction | [`Ravenloft-Asset-Extractor`](https://github.com/Hyperbog/RLOFT/tree/Ravenloft-Asset-Extractor) | [Extractor README](https://github.com/Hyperbog/RLOFT/blob/Ravenloft-Asset-Extractor/Tools/README.md) |
| Preserved original files | [`DnD-Classics`](https://github.com/Hyperbog/RLOFT/tree/DnD-Classics/MASTER) | Original `MASTER/` media; do not modify |

**Branch naming:** `Decomp` was requested for the default `code1-decompilation` branch, but GitHub refused its automatic rename (HTTP 403). Use the actual branch name until an administrator completes the rename. `demo-v1` is the **playable native port**, not the decompilation branch.

## Status and documentation

- [Automatically refreshed GitHub status](docs/LIVE_STATUS.md) — branch commit IDs, latest completed Actions run per branch and open pull requests; **not** gameplay certification.
- [Current engineering acceptance boundary](docs/CURRENT_STATUS.md) — manually maintained, source-backed assessment of gameplay and research.
- [Documentation index](docs/INDEX.md), [branch ownership](docs/BRANCH_OWNERSHIP.md) and [documentation/automation policy](docs/DOCUMENTATION_POLICY.md).
- [Universal mod-forward methodology](MOD_FORWARD_METHODOLOGY.md) — a design goal, not proof of working mods.
- [Historical original/source register](docs/SHARED_SOURCE_REGISTER.md) and [preservation archive](PreservationArchive/) — retain address, SHA and source evidence.

Builds may run automatically on their owning branches. Do not commit or redistribute copyrighted original game files or private instruction-byte disassembly reports. Never infer DOS compatibility or player acceptance from a successful CI run.
