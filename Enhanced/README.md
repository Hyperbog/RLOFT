# Ravenloft Enhanced — native game

This is the **one** player-facing application for Ravenloft. It restores the
October 16, 2026 enhanced native game (formerly `DemoV1/`) as first-class
source. The original implementation already contains cinematic presentation,
the title front end, character creation, exploration, inventory, combat and
native save infrastructure. Those features must be exercised and reviewed
together before declaring a playable milestone.

The source was restored from
[`8982fcc1f0ca`](https://github.com/Hyperbog/RLOFT/commit/8982fcc1f0ca68781cf0a55c4af98f4f15f0fa1c).
Preserving exact source blobs allows us to distinguish integration defects
from historical game defects.

## Build

Windows x64, Visual Studio C++ and CMake:

```powershell
cmake -S Enhanced -B build/enhanced -A x64
cmake --build build/enhanced --config Release --target ravenloft_demo_v1
```

The historical CMake target `ravenloft_demo_v1` is temporarily unchanged;
the product and project are **Ravenloft Enhanced**. CI publishes a source-only
`Ravenloft.exe` together with the original-media launcher and source hash.
Use only legally owned game files and do not overwrite original installations
or save folders.

For standalone source tests, configure the same CMake project and compile
the relevant gameplay test targets. CI first tests the critical path:
boot, character creation, title keyboard, exploration, inventory, save,
and Level 22 story/encounters.

## Product gates

1. **Recovered baseline:** reproduce the historical native binary's startup,
   title menu, character creation and Level 22 behavior on a current Windows
   runner. A successful compile is necessary, but a player review is required.
2. **Alpha:** uninterrupted cinematic/title → New Game → complete two-person
   character creation → authentic playable Level 22; no developer-only warp.
3. **First campaign transition:** interactions, assassin, relic, mist scene,
   persistent state and authored transition to Level 23.
4. **Beta:** complete original campaign, quests, spells, dialogue, combat,
   levels, safe save/load and ending.
5. **Final:** stability, UI, modern rendering/audio, accessibility, mod hooks,
   and reliable packaging with player-provided media.

The
[enhanced release ledger](RELEASE_GATES.json) and
[charter](ENHANCED_PC_PORT_CHARTER.md) govern the player experience.
[Original-source research](../Research/README.md) and
[reconstructed-source runtime tests](../Recomp/README.md) are supporting
reference work, **not** additional independent player applications.

## Shared DreamForge executable foundation

The long-term engine targets **Strahd's Possession**, **Menzoberranzan**, and
**Stone Prophet**, with one native player and per-title data/rules adapters.
Strahd remains the only pre-alpha playable target; the other two are not yet
implemented as gameplay profiles.

`include/rloft/original_executable.hpp` provides a reusable, read-only
DOS MZ / DOS/4G Linear Executable **structural inspector** and a verified
nine-executable source identity table. Its C++17 regression target is
`original_executable_tests`. The Menzo and Stone launchers, CODE.1 and
CODE.2 were independently checked byte-for-byte by SHA-256 against their
`DnD-Classics` source inventories on November 12, 2026. Original game bytes
are not embedded into this source.

**Important:** inspection of an original binary's container, image objects,
entrypoint and source hash is **not machine-code-to-C++ decompilation**.
Title-specific functions, ABI differences, asset formats, gameplay and campaign
logic still require original-byte analysis and native integration. Do not
relabel this static evidence as functioning Menzo/Stone gameplay.

### Original launcher and CODE module reconstruction

The shared C++17 component `include/rloft/original_launcher.hpp` is a
source-backed translation of the observed DOS child-process supervisor loops
of `R.EXE`, `M.EXE` and `SP.EXE`. All begin with CODE.2, treat status 10
as a CODE.1/2 handoff and status 12 as CHARS cleanup/restart; Menzo and Stone
also restart on status 13, while Stone additionally deletes CHARS after each
CODE.2 return. The native Strahd `BootFlow` now dispatches through this
common policy, without replacing its existing title-specific UI/game state.

`include/rloft/original_bound_image.hpp` extracts each DOS/4G LE code/data
object from the authenticated original CODE.1/CODE.2. The nested MZ origin
is `0x25D24` in all six originals. The LE headers are at `0x27BDC` in
Strahd/Menzo and `0x28774` in Stone; all six use sequential uncompressed
4 KiB page mappings. The C++ loader reproduces virtual object zero-fill.
The new `original_cross_game_runtime_tests` CTest runs without original
media; it can optionally test a local original CODE pair. A GCC and Clang
local source test passed for the original Strahd pair.

This is an executable-container and **bounded launcher-main reconstruction**,
not a completed machine-instruction decompilation of Menzo/Stone application
code. LE fixups, original OS service interfaces, game-specific rules, assets,
and campaigns remain unimplemented. Only Strahd has a pre-alpha player.

### Cross-game resource startup (Phase 4)

The shared native C++ source `original_resource_startup.cpp/.hpp` now
translates the **first actual application callees** in Menzo CODE.1
(`0x1044`) and Stone CODE.1 (`0x680C`, `0x6134`), plus the initial
`RES`/title/`CHARS` startup paths in both CODE.2 applications.
Untranslated DOS/game functions are explicit host callbacks, not guessed.
`original_resource_startup_tests` is registered in the existing CTest.

Strahd, Menzo and Stone `RES0` archives were independently verified to use
the same offset-table/EH container grammar, with 141, 175 and 174 entries.
The existing `ResArchive` remains the payload reader; the new read-only
`inspect_original_res0` validates original index and record structure.
See [Phase 4 evidence and limitations](docs/CROSS_GAME_RESOURCE_STARTUP_PHASE4.md).
The Menzo/Stone native gameplay profiles remain disabled until original
resource lifecycle and game rules are implemented and tested.

## Engineering rules

- Only `Enhanced/` owns the user-facing session, character party, world state,
  progression, save data, graphics and input dispatch.
- Original media and verified decompilation are behavioral references. Adopt
  original source algorithms where useful, without replacing one working game
  with a second competing guest runtime.
- No hardcoded per-level gameplay outcomes, fabricated artifacts, private save
  injection, or source test results counted as player acceptance.
- Regressions require a reproducible executable commit, exact source content,
  user-visible capture and a minimal test. Keep unknowns clearly open.
- **No DOSDis dependencies or additional active game implementations.**

## Preserved evidence

The dated history remains available in the original `DemoV1/` Git commit;
the active source directory intentionally contains only its verified code,
tests, launchers, primary charter and selected milestone descriptions.
