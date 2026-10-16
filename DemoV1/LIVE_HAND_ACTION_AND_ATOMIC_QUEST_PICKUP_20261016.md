# Native gameplay: source-backed hand clicks and atomic Level22 quest

**Date:** October 16, 2026 · **Branch:** `demo-v1` · **Status:** committed and source-audited, not a Windows field acceptance.

## 1. The actual player's hand tiles now dispatch to original action gates

The live Win32 exploration input previously ignored **primary clicks** on the four party members' two equipment-hand tiles. The native game now routes original CODE.2 DGROUP D684 hand hit rectangles through `CombatLiveHandRequestModel`, which composes the existing original:

- `214B8` active actor / status / busy selected hand,
- `24478` **both-hand** item arbitration,
- `26018` original item-category action routing.

The original `DGROUP:258A8` party/environment mode comes **directly from current `LevelState.header[0x290]`**, not from a hardcoded outdoor level or an invented global. This is the same byte used by the source environment and automap.

The native player now receives precise hand-action status text: candidate physical weapon attack, illegal actor, busy hand, pair veto, unsupported memory-backed unarmed combination, separate special-item action, category-E item or unrecognized action. The explicit status is traced via the existing optional F12 session diagnostics.

**Important limitation:** This is a decision-and-input-path integration. It does **not** apply damage, play original attack animation/audio or consume hit/damage RNG until the full original `E5F0` attack service has been connected. No synthetic HP decrement or fake monster target is introduced.

Most original one-weapon/empty-hand and both-empty-hand cases still require original `OBJECTS[-1]` prebase heap-adjacent memory bytes that are not in `OBJECTS.DAT`; the source-documented missing-data state is now a first-class value rather than a silent guess. Dual-equipped positive-item hands do pass genuine original item arbitration. A later compatibility mode may define explicitly enhanced unarmed rules, but must never mislabel them as original.

`combat_live_hand_request_tests` covers positive weapon eligibility and loser veto, one/both empty-hand refusals, inactive actor, action busy, hand-ready vs action-state distinction, category-E and special-item routing, invalid actor/hand input. All tests preserve the party image.

## 2. Fourth-relic pickup and complete Level23 transition now commit together

Previously the game executed `21704` and **committed removal of the fourth quest relic from Level22** before beginning the 22-frame mist effect. If frame presentation, Level23 media loading, application of the three F540 removals, or the host failed, the quest could become irrecoverably stuck: the player retained four relics but had no fourth pickup event to trigger it again.

New `ExplorationRuntime::pickup_and_advance_level22_at` stages a full `ExplorationRuntime` copy **before the original 223F4/21704 source hit test and transfer**, then:
1. selects and picks up the original family-C item **on the stage only**;
2. if it was not the final relic, commits that ordinary pickup;
3. on the true four-relic branch invokes the existing separately staged 22-frame original-order Level23 loader/consumer on the staged copy;
4. on any thrown handler, missing destination source, user-quit/cancelled frame, or failed post-frame action, preserves all pre-click live state (including the fourth floor item, cursor, quest BF44, RNG, level ID and visited-level history);
5. only if the entire event completes, commits the staged runtime once and refreshes real destination palette/music.

The normal Win32 player now calls this **single compound API**, not two separately committing operations. The event's temporary input lock attaches only if the fourth-relic playback actually starts and is lifted on success or rejection.

The original frame20 Level23 load, frame21 next-level presentation and consumption of only items `0x87`, `0x88`, `0x74` remain unchanged; parchment `0xE9` survives. The native 22-frame palette fade remains an **enhanced placeholder for the missing original DOS mist art**, not a claim of exact VGA effect parity.

## Source and Windows tests

The ordinary Windows quick build now compiles `combat_live_hand_request_tests` and runs it with prior game, source-render, Level22 quest and save tests. The new `verify_game_live_hand_dispatch.py` rejects 8 negative source mutations. The updated `verify_game_level22_transition.py` independently checks both rollback copies, source ordering, input isolation, original relic handling and native destination music using 15 negative mutations.

These are regression definitions and source integration validations; a new, source-matched Windows compile, CTest run and owner play capture are still **not confirmed**. The project remains **Pre-Alpha**.

## Highest-priority next passes

1. Complete normal live **E5F0** player attack with source-proven projected target selection, genuine 1d20, weapon/creature restrictions, original NPC reaction, class XP/level progression, and cooldowns tied to source A7E4 gameplay maintenance clock. Allow unsupported `OBJECTS[-1]` only after original-memory evidence or clearly marked enhanced rules.
2. Verify actual Assassin death/four drops and four player pickups on a compiled Windows build and then native Save→Quit→Continue in Level23.
3. Restore genuine `1CD48/2E510` 22-frame mist effects/artwork and optional 3D scene parity; continue source-authored quest and encounter expansion throughout the campaign.
