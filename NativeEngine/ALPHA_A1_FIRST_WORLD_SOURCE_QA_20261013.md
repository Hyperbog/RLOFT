# Alpha A1 — first-world and startup acceptance repairs (2026-10-13)

**Canonical branch:** `native-engine`  
**Implementation checkpoint before this note:** `c1f1458e48eb4319b114e6dd452b453510997ca5`  
**Product state:** **Pre-Alpha.** Source corrections and focused local validation completed; **current-head Windows full build, original-data Actions result, actual Windows GUI session and DOS equivalence not independently observed**.

## Code corrections

- Corrected a definite native source build failure: `native_original_startup_audit_main.cpp` referenced `load_native_world_payload`, `NativeWorldRuntime`, `NativePartyStart` and `NativeCollision` without `rloft_native::` namespace qualification.
- `native_party_start_tests` now walks the real shared `BootFlow` state sequence CODE.2 cold start → startup choice 2 → R.EXE status 10 → CODE.1 → CHARS status 10 → CODE.2 import → Exploration, using explicit synthetic two-actor fixture data.
- Original title menu has working host keyboard Up/Down + Enter alongside source-backed mouse selection. Entry 3 now performs the original supervisor's status-0 termination. Entries 0/1 remain unimplemented CODE.2 handlers, without fake load/new-game behavior.
- Original CODE.1 statistics review has host keypad + / - mappings to the already-shared source roll/editor logic. Presentation and character generator are not duplicated.
- Failed CODE.1 source resource construction rolls the supervisor back to the live title menu rather than trapping it in a partially exited CODE.2 state.
- Native first-party GPU world session now returns its window handle. Enter after that window closes explicitly opens a **fresh** Level 22 session with retained CHARS rather than getting stuck; it is **not a save/resume feature**. If already open, focus the existing window. Party display no longer advertises unavailable debug noclip.
- Source Windows CI watches the new import, GPU, test and packaging sources and requires `native_party_start_tests` in the packaged Windows CTest set.

## Source Level 22 audit — independently inspected original owner data

The owner-supplied `LEVELS.DAT` (499,878 bytes) has a Level 22 source record with:

| Field | Original bytes |
|---|---:|
| Map grid | 30 × 15 |
| Entry cell (4,4) word | `0x0000` |
| 10-byte control records | 10 |
| Kind-1 dynamic controls | **0** |
| Other control kind | all 10 are kind 0 |
| Serialized 26-byte event slots | 50 |
| Nonempty event slots | **0** |
| Three 11-byte record families | **1 / 110 / 0** |

Those exact observations are now assertions in the native real-original `rloft_native_startup_audit` and in Windows CI. **Do not synthesize a kind-1 door or normal map-event trigger for Level 22.** The absence of map-local event-slot bytes does *not* mean the level lacks events: original CODE.2 executable-driven scripts and the original 11-byte family records still require runtime reconstruction. These findings prevent premature Level 22 → 23 level-change work.

## Validation actually performed locally

An isolated C++17 replica of the small new CHARS import module (the retrieved original implementation with comments removed) was independently **compiled and run** with both local `g++ -Wall -Wextra -Wpedantic -Werror` and `clang++` using a synthetic two-actor CHARS fixture, plus invalid-portrait rejection. The owner-supplied `LEVELS.DAT` archive was independently parsed with Python; the dimensions, 10 kind-0 controllers, all-zero 50 event slots, family sizes and passable entry cell assertions **passed**.

**Limits:** The isolated importer compile is **not** a full native CMake build or exact-GitHub-checkout Windows x64 compile. Current-head Actions job IDs/logs are not available through the connected GitHub action interface. Do not infer green CI or GUI acceptance from source inspection, older Actions results or these module-level tests.

## Next work

1. Obtain/build and inspect exact current-HEAD native Windows CI test output; fix specific MSVC, linking or source-data audit failures.
2. Implement original CODE.2 initial Level 22 hard-coded gameplay scripts (including source-authored 11-byte records, assassin sequence) and authored RES3 scenery without inventing conventional map triggers.
3. Integrate in-world party inventory, interactions and event state; distinguish mutable original data from a separate native profile.
4. Build legitimate profile save/restart/load, then qualify the first fully playable Alpha route. Music/VOC/cinematic timing and remaining title-menu handlers are also open.

**Do not call this milestone playable Alpha, Beta or Final.**
