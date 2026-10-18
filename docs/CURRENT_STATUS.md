# RLOFT — engineering status and acceptance boundary

**Reviewed:** 2026-10-17. **Live GitHub branch/CI/PR data:** [LIVE_STATUS.md](LIVE_STATUS.md), refreshed automatically by [the documentation workflow](../.github/workflows/documentation-health.yml). **Historical snapshots:** [October 11–13 report](CURRENT_STATUS_HISTORY_20261017.md).

## Player-facing game — `demo-v1`

**Pre-Alpha.** The native C++/Win32 enhanced port owns the one player application, opening/title/party flow and Level 22 implementation. Its controlling requirements are the [enhanced port charter](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/ENHANCED_PC_PORT_CHARTER.md) and [release gates](https://github.com/Hyperbog/RLOFT/blob/demo-v1/DemoV1/RELEASE_GATES.json).

Last verified Windows package: [Actions run 38982609266](https://github.com/Hyperbog/RLOFT/actions/runs/38982609266), **67/67 CTests passed**, compiling source `8982fcc1f0ca68781cf0a55c4af98f4f15f0fa1c`. The latest branch commit may differ because it includes post-build packaging/housekeeping. **The owner has not accepted the latest 3D display, automap, inventory, menus, audio, or full campaign as working.** Level 22→23 mechanics and asset caching have source/tests, but player-visible end-to-end verification remains open.

**Next acceptance gate:** run the actual Windows player build through opening → title → character creation → Level 22, confirm visual/audio/menu/automap/combat/save behavior, then expand authored campaign progression. Do not replace a field test with a source assertion.

## Other workstreams

- **`code1-decompilation`**: original x86 CODE.1/CODE.2 evidence and reconstructed human-readable C++; [Decomp overview](../Decomp/README.md). Verified instruction accounting does not prove semantic accuracy.
- **`recomp`**: separate source-recompilation runtime, linker/startup and CTests; [Recomp overview](https://github.com/Hyperbog/RLOFT/blob/recomp/Recomp/README.md). Historical R01–R03/12 milestones and R04 memory work are research, not a runnable DOS or native game.
- **`DosDis`**: Windows WPF disassembler with DOS/NES/SMS work; [tool docs](https://github.com/Hyperbog/RLOFT/blob/DosDis/Tools/Disassembler/README.md). Candidate code classification is not independent execution evidence.
- **`Ravenloft-Asset-Extractor`**: WPF/Python extractor; [tool docs](https://github.com/Hyperbog/RLOFT/blob/Ravenloft-Asset-Extractor/Tools/README.md).
- **`native-engine`**: separate, experimental original-asset-backed GPU prototype; [engine docs](https://github.com/Hyperbog/RLOFT/blob/native-engine/NativeEngine/README.md). Not integrated into the canonical gameplay executable.
- **`DnD-Classics`**: original game media are preserved; no source/CI release may silently redistribute those files.

## Status rules

1. The [automatically refreshed page](LIVE_STATUS.md) reports GitHub's observed refs, timestamps and CI outcomes. A successful Actions run is not a playable-game acceptance.
2. Update this page only after concrete source-, build- or field-backed progress, with exact branch and commit or test evidence. Keep historical notes in their owning subsystem's evidence files.
3. The formerly controlling DOS-parity release specification is historical. The enhanced-port charter has precedence for the native game's Alpha/Beta/Final objectives.
4. Never mark automated percentages, R-gates, or player-facing release milestones complete merely because generated prose says so.
