# Level22 quest-state safety — original Assassin and relic flow

**Date:** October 16, 2026 · **Game branch:** `demo-v1` · **Status:** source committed; Windows gameplay acceptance remains open.

## Scope

Preserve the actual original Level22 story event, not a synthetic attack or invented transition. The source-reconstructed `Level22EncounterModel` owns type-9 Assassin injury/death, the exact four item IDs and placement seeds, and the first-person pickup path. The user-facing attack dispatcher and 22-frame mist playback are **not complete** and must never be described as playable on the basis of these tests.

## Implemented

1. **Atomic original NPC injury/death.** Prepare HP/animation/flags, the NPC definition's source death response and all four authored drops in a copy of `LevelWorldState`; commit only once the complete sequence succeeds. If original placement/collision probing throws on a later drop, the real world retains the living NPC and its entire existing loose-item list. This also protects the original 300-loose-item eviction behavior from partial commits.
2. **Unique death rewards.** A second call against the dead type-9 NPC now fails without duplicating quest items or reporting another XP reward.
3. **Pickup safety.** Both the live runtime entry and the encounter model reject world pickup when an item is already held. The original 1A344 held-item swap is not implemented, so allowing it would silently overwrite an item. Validate OBJECTS identity, BF44 message predicates and remaining-three-relic story predicate before changing the live cursor/loose-item vector; only then commit a valid pickup. A failed query leaves source selection and state intact.
4. **Correct frame-revision timing.** `ExplorationRuntime` advances its scene revision only after accepted NPC damage or item pickup, not on a rejected attack/invalid item. Thus failed interactions do not spuriously invalidate the current frame's source target IDs.
5. **Save continuity tests.** Native save regression now round-trips a defeated Assassin with all four exact authored relic IDs, followed by a save with one of those items in the cursor and three left in the level. It verifies that native Load retains the corpse HP, source records, remaining items and story message state. No original DOS SAVE.DAT is touched.

## Regression coverage

- Third of four original placement probes throws: **no partial death or drops**; retry succeeds with the exact authored set
- Same failure at the 300-item loose-record ceiling: **no original item evictions**; subsequent successful death evicts exactly four oldest items
- A second hit on a corpse: **no duplicated drops**
- Pickup into occupied cursor or from an undefined OBJECTS ID: **no erased item, no lost cursor**
- Relics picked up in source order still trigger only after the fourth distinct item; normal inventory stashing preserved
- Native save/reload before and after the first relic pickup preserves authentic data and re-encodes byte-identically

The ordinary [Windows quick workflow](../.github/workflows/demo-v1-build.yml) now **builds and runs** the existing `level22_story_tests`, `level22_encounter_tests`, `level22_viewport_target_tests` and `exploration_runtime_tests`, which had previously been excluded from that focused Windows gate. The already-present `enhanced_game_save_tests` gains the quest snapshot regression.

## Verification boundary

The source-code invariants and GitHub branch updates were checked independently. **Do not claim the latest Windows CI finished or these tests ran successfully until a new source-matched run confirms it.** No in-game attack dispatcher, original audio feedback, authored mist frames, Level23 warp, visual correctness, actual disk-backed save/relaunch, or owner gameplay acceptance is established by this checkpoint.

## Next playable path

1. Integrate source-backed player hand attack initiation (214B8/24478/26018/E5F0) with actual actor eligibility, original RNG, NPC hit/weapon/damage rules, clock/cooldown and per-frame target selection. Never replace these with a fixed-damage click.
2. Wire authentic combat sounds, NPC reaction/death and the visible relic pickup feedback in the live Windows player.
3. Execute original Level22 mist staging and 22 frame actions, including the original frame-20 level transition and post-frame relic consumption, then run real save/quit/relaunch and player-visible Windows verification.
