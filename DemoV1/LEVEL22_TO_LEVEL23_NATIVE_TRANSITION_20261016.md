# Fourth Assassin relic: live Level22 → Level23 transition

**Date:** October 16, 2026 · **Canonical playable game:** `demo-v1` · **State:** source committed and cross-file audited; unverified Windows game build and owner play acceptance.

## Implemented in the actual game

Original CODE.2 Level22 item callback `223F4` recognizes the fourth item of the four genuine type-9 Assassin drops (`0x88`, `0x87`, `0xE9`, `0x74`). The live `win32_main.cpp` now invokes `ExplorationRuntime::complete_level22_relic_transition` immediately after the original item-transfer routine confirms that fourth pickup.

### Exact gameplay/quest-state consequences

1. Before changing any live level data, the runtime checks that all **four items are actually in the cursor, equipped inventory or original containers**, not merely a synthetic quest flag or player-click count.
2. The original CODE.2 `21F9D` effect staging pose is applied to a **complete isolated copy** of the current runtime: level22 world at `(23.5,7.5)`, heading `0xD5`. The pending original movement accumulator is cleared on entry.
3. The synchronous 22-frame action loop is played against the copy. **Frame 20** is presented while on Level22. Only **after its presentation** does original event `181B4(23)` load real Level23 through `ExplorationRuntime::transition_to_level` and its existing direct-LEV/LEVELS archive precedence.
4. **Frame 21** presents the newly loaded Level23 with authentic source geometry, location `(9.5,58.5)`, facing `0xB4` (180 degrees). These values were rechecked against supplied `LEVELS.DAT`; Level23 is a valid 64×64 level.
5. Only after ALL 22 presentations have returned successfully does the original F540 epilogue remove `0x87`, `0x88` and `0x74` in that order. The Assassin's Parchment `0xE9` is NOT consumed.
6. Only after the above succeeds is the isolated runtime **committed** in one move. The live game refreshes its level DAC palette and source world music, discards old render/hit-test records and clears movement input. A failed level load, rejected frame, cancelled host or thrown inventory operation cannot strand a partially transitioned persistent world.

The original event lock prevents player input during playback. The native host temporarily detaches the exploration controller and clears old world clicks, reattaching it after success or a reported failure. No user input from the effect may leak into the new level.

### Presentation boundary

The original source specifies `1CD48(2,0x21,7)`, source frame resource base `258DC+0x67..0x7C`, original `2E510`/VGA effects, wait on `31F94` and several staging loops. Those exact original frames and audio/visual effects are **not yet reconstructed in the live Windows renderer**. Until they are, the PC port uses an explicitly **enhanced 22-frame palette blackout**, followed by a half-brightness first frame of original Level23. No commercial artwork is invented and this is never described as DOS-video faithful.

This is actual authored **game level progression**, not a preview shortcut or secondary game executable.

### Tests and gates

- New `level22_quest_playback_tests` checks 22 frame presentations, Level23 entry after frame20, frame21 in the new level, all three epilogue removals, parchment survival, and declines at **every one of the 22 frames**.
- Extended `enhanced_game_save_tests` round-trips a Level23 destination with the surviving `0xE9` parchment, full coordinate/facing state and separately preserved defeated Level22 Assassin world in the visited-level snapshot.
- New `verify_game_level22_transition.py` source integrity guard checks live host integration, atomic copy/commit ordering, original frame20/21 ordering, native music/palette refresh, original quest relic identity, input lock and continuous Windows CI registration. It includes **13 negative regression mutations**.
- The ordinary Windows quick workflow compiles and runs `level22_mist_timeline_tests`, `level22_quest_playback_tests`, `enhanced_game_save_tests` and the new source contract, in addition to existing Level22 source/encounter tests.

### Remaining acceptance and work

**Not yet verified:** a Windows CI run compiling this specific source checkout, a new player-matched `Ravenloft.exe`, rendered Level23 screenshots, actual original-effect artwork/sound, save→quit→relaunch at the transition, or direct owner field acceptance. The project remains **Pre-Alpha**.

Most importantly, the **normal player hand attack dispatcher is still not live**. Players cannot yet reliably defeat the Level22 Assassin through the original source-backed attack rules; that earlier blocker must be completed before this new transition forms a fully naturally reachable player quest. Do not claim that the first complete original Level22 encounter is playable merely because the fourth-relic event is now implemented.

The next development priority is `214B8 → 24478 → 26018 → E5F0` live melee and its original hit/damage/hand clocks, then proper original `1CD48/2E510` mist visuals and Level23 exits/AI.
