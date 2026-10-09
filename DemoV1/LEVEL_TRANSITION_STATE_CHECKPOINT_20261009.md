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
