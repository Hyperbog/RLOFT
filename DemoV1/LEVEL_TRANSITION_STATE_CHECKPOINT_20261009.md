# Part 9: General level-continuity engine checkpoint

**Branch:** `demo-v1`  
**Compilation:** Source-only, new tests not yet run. Builds remain user-authorized.

## Engine changes

- Added `LevelContinuityStore` to retain mutable `LevelState` and expanded `LevelWorldState` for previously visited level IDs; world objects, NPC HP, cell and automap state survive in-memory return visits.
- Added `ExplorationRuntime::transition_to_level(source, target)`: returns a remembered mutable destination, or uses the established original `LEVnnn.DAT`/ `LEVELS.DAT` loader and NPC/world expansion for a first visit.
- The target must be supplied from a separately verified original event/exit. No arbitrary Level22-to-Level23 teleport or made-up entry pose is implemented.
- Global party actor bytes, carried inventory/containers, original event flags and world clock remain on the SAME `ExplorationRuntime` session; archive NPC initialization RNG only advances if a new destination enters successfully.
- Target level/geometry/16.16 position/facing preflight occurs before session change. Original RES4 INIT+header selector is rebound per destination, decoded RES3 scene cache is cleared, and old hit-frame revision invalidated.
- Inventory-only SAVE backing is dropped on transition, preventing an invalid multi-world SAVE export. This is not a full game SAVE/LOAD bridge.
- Open hand-container modal state explicitly blocks transition until original close/cancel behavior is integrated.

## Evidence and pending tests

`level_continuity_tests` is registered in CMake and asserts rejected transitions do not modify live state, preserving party/clock/flags, NPC HP, removed loose items, cell edits and automap caches through a roundtrip. Registration increases the expected total from **172 to 173 CTests**; the Quick subset stays 19. All newly authored tests remain **uncompiled/unexecuted** until an authorized build. The last verified Full Windows build was #647 (172/172), source `fb4a0145`.

**Still open:** original CODE.2 authored exit/event routing, exact transition cutscenes/palette/music/IRQ, complete level-local event effects, full-world SAVE.DAT serialization and reload, live native Level22→actual next event route→return acceptance, DOS pixel/timing/state comparison. Only an in-memory engine bridge exists; it must not be described as a complete original cross-level lifecycle.

## Next original-code checkpoint (still source-only)

The [CODE.2 original mist timeline](LEVEL22_ORIGINAL_MIST_FRAME_SEQUENCE_20261009.md) adds an independently testable **source-order** event plan for 21F70..223F0. The storyboard does not display synthetic mist or execute a guessed duration. Registered CTests increase to **174**, Quick stays **19**. The 22-frame event and current level-continuity source are unbuilt and not DOS-accepted.

## Part 9 continuation — original exit data now established

The CODE.2 Level22 mist caller/source order is verified from original bytes; the [frame timeline](LEVEL22_ORIGINAL_MIST_FRAME_SEQUENCE_20261009.md) and [staged camera](LEVEL22_ORIGINAL_MIST_STAGING_20261009.md) models preserve its original operations. [LEVELS.DAT metadata](ORIGINAL_LEVEL23_DESTINATION_AUDIT_20261009.md) confirms the stage destination is a 64×64 original Level23. **No invented portal, timed mist renderer, or manually triggered level skip has been added.** 175 CTests registered; current source remains unbuilt.

## Original numbered event dispatcher continuation

[CODE.2 1CD48 event gate](ORIGINAL_NUMBERED_EFFECT_1CD48_AUDIT_20261009.md) is now modeled with the original event+0x80 persistent bit semantics. Mist event2 uses bit0x82; it is not set on a synthetic pickup shortcut. Total registered CTests **176** (Quick19). Live original effect execution and DOS parity are still open.
