# Level22 native combat: original rear-party suppression and NPC audio

**Date:** 2026-10-16 · **Canonical playable branch:** `demo-v1` · **Status:** C++ integrated/committed, Windows compile and gameplay acceptance not yet confirmed.

## Correct source game behavior in a real attack

The earlier real-hit integration entered the original E578 -> C80C -> 1C328 -> C910 -> E284 path but omitted a verified **E5F0:E621..E669** special target-clear condition. This allowed actors 2 and 3 to damage a valid Assassin in source level mode `FF` even with ineligible weapons.

The new strike invokes existing `CombatAttackTargetModel::choose()` **after** E578 invisibility/status cleanup but **before** the original hit d20 RNG:
- The actor's original party index and explicit original NPC record index are retained.
- The mode is the **current** `LevelState.header[0x290]`, equivalent to original DGROUP byte `258A8`.
- Only in mode `FF`, for actor index 2 or 3, the target is suppressed unless the actual equipped `OBJECTS WORD+35` low two bits equal `2`.
- The suppressed attack still clears the E578 source effects but spends **no hit/damage RNG**, changes no Assassin HP and does not create loot. This is an action without a resolved target, not a normal C80C miss.
- It does not invent or consult the missing automatic `DB94/DACC` projected cache. Only the source-supported explicitly selected NPC index is passed.

New native C++ regressions compare a suppressed rear-party attack against one with the original qualified weapon, including unchanged original RNG for the suppression and status cleanup after E578. The original normal hit, miss, held-NPC, creature-gate and exact four-relic lethal cases remain covered.

## Audible Assassin reaction and death, with exact INIT bank identities

The original NPC definition is already parsed from `NPCS.DAT` into its distinct fields:
- NPCS `+0x41`: surviving damage-reaction sound ID
- NPCS `+0x43`: death sound ID

The existing source NPC injury handler returns the correct kind and unmodified local ID. The new `OriginalNpcSoundModel::res6_id` looks up the authentic bank indices in the original, **actually installed `INIT`** asset:
- **Surviving hit**: `INIT+0x14` -> DGROUP `0x258C0` (shipped base 69)
- **Death**: `INIT+0x16` -> DGROUP `0x258BC` (shipped base 166)
- A zero death-sound ID **does not silence playback**: the original source requests the base death sample.

These offsets were checked independently against the immutable user-supplied `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92eb4d94806d835ad30a0bab3e265bd314`. Its x86 absolute-store instruction at physical offset `0x51E3A` is `A3 C0 58 02 00` and at `0x51E67` is `A3 BC 58 02 00`, immediately following the original INIT reads at `+0x14` and `+0x16`. The distinct bank indices also reconcile with the shipped RES6 count (266): `69+96=165` and `166+90=256` are both in bounds.

The real `win32_main.cpp` player-hit path now uses that RES6 ID to decode the actual original Creative Voice sample through the established Windows audio player after a successfully committed NPC injury. No sample plays for a miss or rear-target suppression. Missing or corrupt VOC audio is logged (when dev tracing is enabled) and does not roll back the already-valid HP/RNG/quest operation.

**Audio fidelity boundary:** This is direct native one-shot playback of the correct source sample. Original `28110/28168` DOS sound-channel priority, queuing, hardware timing and sample lifecycle are **not yet reconstructed**; do not describe the PC playback scheduler as bit-identical to DOS.

## Regression gates

- New `original_npc_sound_tests`: positive reaction, max source local reaction 96, death zero-ID fallback, max source death 90, overridden INIT bank words, missing/truncated INIT, and no-sound result.
- Expanded `combat_level22_strike_tests`: actor-2 party mode FF target suppression versus literal original item qualifier +35 low2==2.
- Existing combat source regression `verify_live_level22_weapon_strike.py` now guards the rear-party target decision before d20 RNG. It also fixes a discovered missing Python quote which had previously prevented the script from parsing.
- New `verify_original_npc_sound_pipeline.py` and eight deliberate negative mutation cases guard the correct INIT base offsets, zero-ID fallback, audio playback in the actual Win32 game, post-commit isolation and Windows CI registration.
- Windows quick CMake suite now builds/tests both `combat_level22_strike_tests` and `original_npc_sound_tests`, and runs their separate source contracts. The existing hand, quest, save and graphics regressions remain registered.

## Still unfinished

Original automatic target selection, most empty-hand attack pairs, Rod/spell-hand special effects, `24B48` action visual/cooldown and separate pauseable `A7E4` maintenance clock, class XP/level-up and monster AI/attacks still need reconstruction. The original mist video effect is also incomplete. A compiled source-matched Windows executable and user gameplay tests have **not** been verified for this patch; retain Pre-Alpha status.
