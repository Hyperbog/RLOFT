# Native Alpha A2 — Source-backed Level 22 inspection targeting

**Branch:** `native-engine`  
**Source revision checkpoint:** `070b874f9adc42717e84eb862df496ddf5a4fde1`  
**Date:** October 13, 2026  
**Status:** Source-integrated, new CTest and authentic-data acceptance added. **Not yet a passing current-HEAD Windows build, DOS-targeting reconstruction or playable Alpha.**

## Source-grounded functionality

The party-bound GPU Level22 window already consumed all **111** original authored compact records, using `LEVELS.DAT`, `INIT`, `NPCS.DAT`, `OBJECTS.DAT` and `RES3` without inventing sprite frames. This step adds the first in-world **source entity selection/inspection** path.

- `native_scene_target.hpp/.cpp` implements a portable, bounded read-only query over a supplied set of original `NativeAuthoredSceneInstance` records. The caller supplies the live native camera, original level, a 2.5-cell radius and a 0.45-radian half-cone by default.
- Candidate positions retain their **original 16.16** subcell coordinates. Identity, source family, original type ID, compact-record ordinal and exact RES3 base are unchanged. The provisional host query rejects malformed coordinates, invalid level scope, nonfinite camera parameters and excessive budgets; it checks normal source cell collision along the line of sight and never modifies inventory, controls, flags or source media.
- The best aligned candidate wins, with closer range breaking bearing ties and original source order as final stable tie. This is explicitly an inspection heuristic and **is not the original CODE.2 click/attack/use targeting**.
- In the party GPU window, press **F** while near a discovered source position to display the class (NPC/placed/loose), source type number, original RES3 base ID and source-record ordinal. No dialogue, scripted response, pickup, combat or world mutation is fabricated. The existing F5 marker overlay still only shows original placements in discovered cells.
- The new `native_scene_target_tests` CTest covers source fixed-point coordinates, angular and range rejection, original collision occlusion, blocked target endpoint, invalid source scope, corrupted coordinate metadata and read-only world checks. CI and the Windows packaging script now require this test.

## Authentic initial-level binary acceptance

The supplied original `LEVELS.DAT` was inspected independently, SHA-256:

`50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`

Level 22 contains original NPC type **9** as the sole 11-byte class-A source record:

- Raw record: `09000d00007c0700007802`
- X: `13 + 0x7C00/65536 = 13.484375`
- Y: `7 + 0x7800/65536 = 7.46875`
- Low-three-bit facing: `2`
- Original cell `(13,7)`: `0x0000`, ordinary-passable.

The original-media `rloft_native_startup_audit` now invokes the **actual native host target selector** on this verified NPC record from `(13.084375,7.46875)` facing 0 radians. Windows CI requires `original_first_level_npc9_inspection_target_verified`. The test only asserts source data acceptance; it does **not** establish that original gameplay puts the party at that viewpoint or that NPC9 is alive/active/hostile at that time.

## Bounds and remaining first-playable Alpha gates

No actual live sprite view-frame selector, EH7 projection/occlusion, action VM, original NPC encounter scheduling, inventory, combat, audio or profile persistence is implemented by this change. The 111 placed records remain **proven authored positions and shape bases**, not interactive entities with invented gameplay states.

The native Win32 path and all new source files are committed, but there is **no independently retrieved current-head Windows/MSVC compile, original-data Actions run or owner GPU screenshot/field-play confirmation**. Previous CI passes cover older source revisions only.

**Next priority:** confirm exact-source Windows build and test failures; reconstruct CODE.2 original RES3 frame/depth selection and initial executable-driven Level22 NPC/event state; wire source-validated targeting to real interaction/equipment/inventory and native profile save/load. Do not substitute visual debugging for playable-Alpha requirements or advance to Level23 prematurely.
