# Part 20 — Enhanced port Alpha build path and authored Level 22 music

**Effective goal:** complete playable enhanced Windows PC port.  
**Branch:** `demo-v1` — **Pre-Alpha, owner field acceptance still open.**

## Windows build no longer blocked by historical DOS research

The previous push workflow required **all** registered original-DOS/CODE.1/CODE.2 source research CTests and a separate canonical reconstructed CODE.2 syntax gate before it published the native Windows game. That's the wrong release scope after the owner's October 13 product change.

The enhanced PC build now does this:

1. Every `demo-v1` source push configures the real Windows x64 game.
2. It compiles the native executable, Level 22 rendering and motion regressions, character transfer and inventory tests, the native 128-row RES3 sprite compositor and scene planner, and the **enhanced world-music cue tests**.
3. The focused CTest selection includes the real continuous-launch media-safety check, previously working frontend source hashes/animation clocks, native Level 22 layering, source-preservation release ledger, and the active build-pipeline/music wiring checks.
4. Only after those checks succeed does the workflow stage and publish `Ravenloft-Continuous-Alpha-Windows-x64` with the normal intro→character creation→Level 22 launcher and executable SHA-256/source SHA.
5. The **historical full suite** and original CODE.2 faithful-source syntax audit remain available through an explicit **manual Full workflow dispatch**. They are useful research, not blockers for a native-player Alpha. The optional Linux run is unchanged.

**Traceability fix:** the bot's checked-in `DemoV1/Windows/ravenloft_demo_v1.exe` and matching `.sha256` now also get `DemoV1/Windows/build-source-sha.txt` containing the **exact checkout revision compiled into the executable**. Later commits can move branch HEAD while the executable is building; do not misidentify that binary as automatically compiled from the newest branch HEAD.

`DemoV1/tools/verify_enhanced_port_build_pipeline.py` adds a public, source-only CTest with six deliberate regression mutations. It catches required port-build conditions accidentally reverting to DOS research gating, missing native renderer coverage, missing provenance or suppression of the continuous-player artifact.

## Gameplay audio is now integrated

Previously `run_exploration` received the same `Win32MidiPlayer` and `Win32AudioPlayer` used by the working CINE/character creator, but did **not** request a background sequence on normal Level 22 entry.

Original asset data is already represented by `LevelState::music_sequence`, decoded from the fixed level header at offset **+0x29C**. The supplied source `LEVELS.DAT` level 22 has ID **0**, which is a **valid authored music resource ID**, not an absence sentinel. `RES5` stores XMIDI/XDIR type-3 music entries. Existing `RavenloftBootAssets::music_sequence(index)` and `parse_xmidi_sequence(...,1)` / `Win32MidiPlayer::play` were already used by the intro and character creator.

At the **real CODE.1→CODE.2 gameplay boundary**, the Windows host now:

- Selects the **current level's own authored sequence ID** from `runtime.session().level.music_sequence` and starts the existing native XMIDI backend with a 2500ms volume fade from 0 to 100.
- Uses `EnhancedWorldMusicState` to request a track once, not every 60Hz render. A level transition or an authored default-sequence change requests a new track.
- Invalidates the active cue if the original CINE08 event takes over audio; on the next live gameplay frame, starts the level's own track again.
- Reports resource-decode errors through Win32 debug output/opt-in trace and continues to draw gameplay rather than crash the scene because music isn't available. Lack of an installed host MIDI output device is also not assumed to prove successful playback.
- Does **not** change the earlier CINE00/CINE12 sound/voice or character-creation music, sound cues, lady/cat timing or card flipping.

`enhanced_world_music_tests` is a standalone C++17 regression for 600 stable frames, valid ID 0, changing cues, changing level and cinematic resume. `enhanced_world_music_pipeline_contract_tests` adds six source-level mutation guards against restoring silent gameplay and changing existing character-creation audio. Both are part of focused Windows builds. A locally compiled equivalent of the portable cue model passed GCC14 with `-Wall -Wextra -Wpedantic -Werror -O2 -DNDEBUG`.

**Soundbank boundary:** RES5 has two 49-entry banks. A historical alternate-bank switch is tied to period hardware-driver mode; that hardware predicate has not been re-created in the native port. Initial default uses the direct first-bank RES5 resource index through the existing host XMIDI player, as the intro already does. Do **not** claim specific MT-32 vs General MIDI bank/timbre parity; listen to Level 22 and verify the intended arrangement before accepting the audio gate.

This change supplies the **Level 22 background music integration**, not the full remaining encounter, action, NPC, ambient or positional SFX system. Those native sources still require real game-event playback wiring.

## Current status and next acceptance

The source snapshot and core wiring passed an **independent 16/16 integration check**, but this does **not** establish a successful Windows build, the correct rendered sky/ground/trees/assassin, available MIDI output, or actual sound synchronization.

1. Obtain the **current-source** successful GitHub Actions `Ravenloft-Continuous-Alpha-Windows-x64` artifact. Check its `build-source-sha.txt` and SHA-256; don't test a stale checked-in EXE as if it contains the newest changes.
2. Copy legitimately owned game assets into its `Game/` folder, then run only `Play-Ravenloft-Alpha.cmd`. It launches normally without a folder picker and isolates the source game files.
3. Verify the original opening screens, audio, lady/cat/card effects and creation; Level 22 must then show actual scenery and assassin with stable 60Hz presentation **and audible authored background music**.
4. Check normal movement, mouse/keyboard and interactable gameplay. Record a first-scene capture and any error before promoting Alpha gates.

**Classification: Pre-Alpha; all ten Alpha gates remain open.** Build success alone cannot change that. The enhanced native PC port's campaign/save systems and other Level 22 sound effects remain subsequent integration work.
