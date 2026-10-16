# Native Level22 Combat: Scene-Locked Input and Global SFX Control

**Checkpoint:** October 16, 2026 · **Branch:** `demo-v1` · **Status:** committed source; Windows executable/play acceptance unverified.

## Player-facing combat safety

The enhanced-native explicit two-click attack adapter now uses a dedicated `EnhancedCombatTargetArming` state machine, rather than an unbound `optional<actor,hand>`. An authorized hand click records the **actual source-rendered viewport identity**: world scene revision, source level ID, both original 16.16 party coordinates, and source heading.

The next viewport target click is **always consumed once**, whether the game finds an Assassin, the player clicks empty space, the source view changed, or the attack is rejected. It cannot be passed to the separate original `21704` loose-item pickup handler. In particular, a failed/empty attack no longer unexpectedly picks up a relic or initiates its Level23 quest transition.

The adapter checks all five recorded fields on the target click. A different current level, camera pose, world state revision or source frame invalidates the selection **without altering NPC HP, game RNG, inventory or story**. Opening a native map, options screen or character panel clears pending combat selection; a top-edge menu click (including Save/Load) also clears it before any world transition or checkpoint restore. A freshly selected hand and source scene are required for a new attempt. The actual weapon and target are still independently revalidated by `ExplorationRuntime::attack_level22_assassin_at`.

This is enhanced **PC interaction correctness**, not a claim to have reconstructed original automatic `DB94/DACC` target lookup or action animation. DOS `24B48` action duration, `A7E4` maintenance clock, character XP/level-up, and other enemies remain unfinished and are not replaced with host frame time.

## Actual separate Music and sampled Sound options

The native indexed in-game options surface now has three distinct buttons: **Music**, **Sound**, and **Return To Game**. Music continues to control the existing authored RES5 XMIDI stream independently. Sound controls the shared `Win32AudioPlayer` that decodes and plays original Creative Voice samples. Turning Sound off stops all active PCM voices and suppresses subsequent `Win32AudioPlayer::play` calls; turning it back on re-enables them. This affects the Assassin's genuine surviving-hit and corpse sounds **and** all other sampled VOC paths using the shared player, including cinematics.

The new exploration session sets sampled effects enabled by default, so the previous session's mute does not leak into a new game. The option is an explicitly native PC overlay behavior; it does not alter original `GAME.OPT` serialized bytes or change the DOS sound hardware queue `28110/28168`. Existing audio hardware fallback and graceful VOC-decode failure behavior remain unchanged.

## Tests

- New `enhanced_combat_target_arming_tests` (C++17): correct once-only consumption, valid actor/hand, five independent stale-scene dimensions, absent frame, cancel-on-modal behavior, and invalid indices. Included in the standard Windows `rloft_boot_flow` CTest quick suite.
- Expanded `enhanced_options_presenter_tests`: three non-overlapping original-indexed screen buttons, edge/gutter click behavior, live framebuffer repaint and separate music/sound drawing states.
- New `verify_native_combat_interaction.py` source contract: authentic five-part scene identity, consumed-before-pickup ordering, menu/modal cancellation, SFX UI event, shared PCM mute implementation, sound-only toggle and Windows CI registration. **16 negative source mutation scenarios**.
- Updated `verify_original_npc_sound_pipeline.py` and `verify_enhanced_level22_ui.py` require the new sound-enabled gating while preserving the existing correct original sampled NPC resource selection and original automap/mouse input separation.

An independent cross-file source integration audit checked **19 required invariants**, all passed. The new C++ test targets and Python validators are committed in `demo-v1` CMake/Windows CI definitions. A successful MSVC build, complete Windows CTest execution, new matching `Ravenloft.exe`, sound output from a real device and gameplay screenshots have **not** been confirmed for this HEAD. Do not interpret a clean source audit as a compiled, tested Windows release.

## Remaining main game work

Source-accurate A7E4/24B48 attack cadence, XP and level-up side effects, original auto-target buffers, unsupported original OBJECTS[-1] empty-hand memory, other enemies, creature AI, exact DOS sound channel priorities, and original 22-frame mist graphics. Still **Pre-Alpha**, not a finished campaign port.
