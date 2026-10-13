# Alpha A2 — first Level 22 source-authored placement integration

**Branch:** `native-engine`  
**Implementation checkpoint:** `fe775f86ee51ac95c02acfa3d2f9a2020fcea047`  
**Date:** 2026-10-13  
**Acceptance class:** Source-integrated diagnostic metadata, **not** a compiled/field-tested playable Alpha or DOS-authentic sprite renderer.

## What was connected

The original source-aware NativeEngine already had portable `derive_native_authored_scene_instances` and a complete 32-level `NativeAuthoredSceneDirectory`, with all compact 11-byte NPC/placed/loose records mapped through source `NPCS.DAT`, `INIT`, `OBJECTS.DAT`, and original `RES3` bank definitions.

This milestone connects that **existing** typed source ledger to the normal CODE.1 → 0x920 CHARS → CODE.2 level22 **party GPU session**:

- The party-bound (not debug-map-only) GPU `State` reads the original `RES3` record directory, `INIT` source bank layout, `NPCS.DAT` and `OBJECTS.DAT`, then derives and retains the complete original 32-level scene ledger. Any missing required archive or out-of-range source type fails rather than producing placeholder scenery.
- Original Level 22 must contain **111** authored records, sourced from the actual level sections, before normal party GPU launch proceeds.
- The existing host-session automap can optionally show **diagnostic position markers**, restricted to discovered cells, at the exact original **16.16** X/Y record positions. Marker colors distinguish type IDs; the source record's class, type, shape base and index remain unchanged. These are **not** sprites, combat actors, interactive items, map-event emulation, or verified original DOS visibility.
- Toggle the purely diagnostic original-placement layer using **F5**. Independent GPU inspector can still explore source geometry without constructing a party/scene ledger.
- Source provenance/fingerprint verification and discovered-position list construction occur on new exploration/visibility updates. The OpenGL repaint loop reuses the cached list; it does **not** recompute the 32-map SHA on every frame.

## Owner-supplied original-binary independent check

From `/mnt/data/LEVELS.DAT` (`SHA256 50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`), original archive level index 22 spans **4,744 bytes** and is a **30 × 15** map. Exact original compact records:

| Source class | Original type | Count |
|---|---:|---:|
| A, NPC | 9 | **1** |
| B, placed | 62 | **52** |
| B, placed | 63 | **42** |
| B, placed | 64 | **16** |
| C, loose | — | **0** |
| **Total** | | **111** |

All original record coordinates are in bounds. Source NPC 9 begins at **(13.484375, 7.46875)** with compact byte flags `0x02` (low-three-bit source facing 2). These are source *positions*, not confirmed runtime AI state.

Original level has **10 kind-0 controls, no kind-1 animated controllers, all 50 event slots empty**, and ordinary-passable first-party start `(4.5,4.5)` cell word `0x0000`.

The exact four-type distribution, source bounds and original RES3 base limits are now enforced by `rloft_native_startup_audit`. Source original SHA guards in Windows CI now include **RES3**, **NPCS.DAT**, **OBJECTS.DAT**, **LEVELS.DAT** and other startup assets.

## Regressions

`native_scene_instances_tests` now verifies the separate host discovered-cell mask is independent of original transient `0x0040` visibility; it tests original record order, first/second source class and fixed-point coordinates, invalid masks, oversized candidate result limits, invalid level IDs, tampered original source fingerprint and persistence of original discovered data after transient marks clear. Windows packaging requires `native_scene_instances_tests`.

**Not yet verified:** latest-HEAD complete MSVC Windows CI results, actual GPU render/marker screenshot, source frame parity, game interactions, real entity animation/AI, party inventory, native profile save and reloaded session. No original shipped archive bytes were committed to the Git branch.

## Immediate subsequent Alpha work

1. Obtain a passing current-HEAD MSVC/CTest + real original-data startup/scene ledger audit, then owner field testing on actual Windows GPU.
2. Resolve original CODE.2 **RES3 EH7 subframe selection, projection/depth/occlusion**, actual sprite drawing and original first-level 11-byte record lifecycle without synthesizing objects.
3. Reconstruct the executable-driven Level22 source NPC9 event/encounter path rather than inventing a populated map event table.
4. Integrate actual in-world events, usable inventory/interaction, source-authored gameplay and profile-only save/load. *Do not* jump to Level23 until the startup and Level22 playable Alpha gates pass.

**Status:** first-level source placement ownership integrated and checked against original record bytes; **still Pre-Alpha**.
