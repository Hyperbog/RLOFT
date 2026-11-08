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
