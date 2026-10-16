# Ravenloft — gameplay-first engineering reset

**Date:** October 16, 2026 · **Branch:** `demo-v1` · **Status:** Pre-Alpha
**Controlling policy:** [Enhanced PC Port Charter](ENHANCED_PC_PORT_CHARTER.md) and [opening/Level22 acceptance contract](INTRO_LEVEL22_PRODUCT_ACCEPTANCE_20261015.md).

## One product, no more parallel game rewrites

The player product is the continuous, C++17 native Windows `Ravenloft.exe`, built from `DemoV1/`. Keep the source trees and original game media on their owning branches; reuse their **verified implementations**, not a second native game. The historic `native-engine` prototype and exact DOS recompilation are references, not independent release prerequisites. Windows is the initial target; keep non-platform game logic portable.

Maintain an authoritative simulation (party, 32 original levels, NPCs, objects, inventory, scripts, combat, quests, save state) distinct from an interchangeable renderer, input host and audio output. Continue original RES/ANM/DAT asset decoding, with future validated, mod-forward resource IDs. Do not alter the original assets, story, core gameplay, or player saves. No placeholders are allowed to impersonate absent canonical game content.

## Locked delivery sequence

1. **Boot and menus:** DreamForge/SSI/TSR screens, original animated introduction, original four title actions, genuine CODE.1 character creation, correct startup from `Ravenloft.exe` or the helper, and responsive sound and animation. Resolve the current CINE00/CINE12 versus stand-alone logo **visual ordering discrepancy** with actual game-frame evidence.
2. **Visually correct, playable Level22:** genuine RES8 sky/backdrop, RES4 ground/walls, RES3 scenery/trees and Assassin, true depth/occlusion, usable HUD/inventory/map/top menus, paced animations and interactive Assassin combat. Source-derived projection is required; neither code accounting nor scene-record counts proves visual fidelity. Preserve the indexed software scene as default, make 3D improvements optional.
3. **Native saves and original transition:** save → quit → relaunch → Continue must restore changed party/NPC/inventory/scripts/level state. Complete the real Assassin relic event and CODE.2 22-frame mist with the original level handoff; no invented Level23 warp.
4. **Whole game:** expand the exact same gameplay engine across authored world transitions, scripts, monsters, equipment, spells, puzzles, quests and canonical ending. Then refine high-resolution presentation, FPS/vsync, input, audio, accessibility and safe mod resource overlays.

A milestone is **implemented** when code exists; **compiled** when a source-pinned Windows run passes; **field verified** only when real Windows play/video/screenshots demonstrate intended visuals, gameplay and audio. Do not equate these states. Never call the whole game accepted because the startup passes.

## First concrete renderer correction after reset

Source change [`def4c19c2b1d36ded0ffa8b6c95f022de4f01b2a`](https://github.com/Hyperbog/RLOFT/commit/def4c19c2b1d36ded0ffa8b6c95f022de4f01b2a) replaces a single arbitrary **0.9-unit GPU billboard width** for every sprite with its decoded RES3-width / 128-row relationship (two-unit height). Texture alpha, original per-object placement, authoritative simulation and original software presentation remain unchanged. The optional GPU view is still an approximation pending screenshot parity.

Also restore the source-indexed world **on the same frame** when optional GPU geometry raises, clearing left-over client-array state before compositing. Without this recovery, the world had already been cleared and could show a blank failed frame. The next frames fall back to the preserved software renderer. Added portable C++ width tests and negative source-integrity mutations; these are not evidence of full graphical correctness.

**Next user-facing defect to close:** compare new direct-executable Level22 screen captures against authentic original gameplay at the same party pose. Check original source panorama, RES3 location/height/masks, indexed palette and SINTAB, OpenGL sprite depth order, and currently missing sky/ground; fix the first reproduced scene defect, not a guessed coordinate. Windows gameplay and owner acceptance remain open at this checkpoint.

## Technology policy

Keep C++ and the current working Win32/OpenGL + software fallback while repairing gameplay. Move to a modern portable window/input/audio abstraction and newer OpenGL core / alternative backend only behind the same verified simulation and resource decode interfaces. DirectX 12 is an optional later rendering backend, **not a condition for completing Level22**. No rewrite simply to change graphics APIs.
