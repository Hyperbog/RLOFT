# Original DDA4 class XP in the native Assassin encounter

**Date:** October 16, 2026 · **Branch:** `demo-v1` · **Status:** **Windows MSVC build and 65/65 CTest checks PASSED** on compiled source `4625ccb358171072fea4816cfbf3c1e9c92dcff8`; hands-on gameplay acceptance remains open.

## Real game change

The live explicitly targeted Level22 Assassin now awards **the actual NPCS.DAT experience value** to the party when an equipped-weapon hit kills it. The game stages the monster injury, original four relic drops, original story BF44 mutation, attack RNG changes, and the class XP writes as **one complete ExplorationRuntime copy**. The live game commits only after the entire operation succeeds. Failure of a class/threshold preflight or a drop placement cannot leave partial XP, a corpse without relics, or consumed RNG.

The native attack path obtains progression thresholds from the **genuine shipped CODE.2 tables**, rejecting any unsupported replacement. The user-provided `CODE.2` file was checked locally: SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, progression-table FNV64 `0xA1AB0932C322B502`, six ordered class threshold arrays.

Source routine `1DDA4 / DDA4` is reconstructed in these steps:

1. Use the **original status eligibility mask `F240h`**, not party-slot occupancy, to select recipients.
2. For N qualifying actors compute original rounded share `(npc_award + N/2) / N`, enforcing the original minimum one.
3. For each actor count occupied class-track bytes at `+0x15 .. +0x17`, then split its share with the original rounded per-track minimum one.
4. Validate **all** resulting class/XP levels against the independently authenticated 6×20 progression thresholds **before writing a single live byte**. Empty party or unsupported class is not guessed.
5. Write each class-track DWORD to **original actor offsets `+0x26,+0x2A,+0x2E`** using x86 wrapping DWORD semantics, preserving all other original party state.
6. Return a distinct `level_change_services_pending` result when the new XP crosses a level threshold. Do **not** invent a new class level, health increment, spell slots, event text or IRQ/RNG activity.

The original follow-on services `50DC`, `3EA4/3DE4`, `3210F`, `1C21C`, `DC7C`, and derived attack-target recalculation still need a fully ordered live implementation. The project has existing models for several of those subroutines; they are not yet connected to this death. **XP is genuinely applied and saved**, but character level increases and their side effects are not falsely reported as complete. Players are notified if they earn XP that requires those pending callbacks.

When the original party/class records are invalid, the game rejects the encounter transaction rather than pretending XP was awarded. Previously supported **normal** non-level-changing Assassin kills now gain real saved experience.

## Save / Continue

The original actor image is already serialized verbatim by the game's own native `EnhancedGameSaveCodec`. No new save version or custom XP format is required. The extended native save regression checks that the `+0x26` class-track DWORD survives:
- an actual Level22 defeated-Assassin state with authentic quest relics,
- a Save→Continue round trip, and
- the staged Level22→23 world transition with the prior world preserved separately.

## Tests and limitations

New compiled `combat_xp_award_service_tests` covers authentic 2-actor and two-class splitting, correct rounded shares/XP writes, preserving all non-XP bytes, threshold-crossing pending services, invalid class rollback, all-blocked party rejection and DWORD wrap. The new `verify_live_assassin_xp.py` source contract guards exact class table loading, death/loot/XP atomicity, no fake level changes, native save XP continuity and the normal Windows build/test registration; it defines **13 negative mutation cases**.

Previous combat hand, strike, NPC sound, source GPU and Level22 quest test targets remain included. The original CODE.2 progression tables were independently checked against the local supplied archive. The actual Windows MSVC game and quick-test targets were **compiled successfully** by [GitHub Actions run 38955645652](https://github.com/Hyperbog/RLOFT/actions/runs/38955645652). CTest passed **65/65** checks, including the class-XP award, source-backed strike, native save, graphics and mist-transition suites. The run published a source-matched `Ravenloft-Enhanced-Game-Windows-x64` ZIP and committed the built EXE with an SHA-256 manifest (`af963ca899bcf4e11cca5d4ab54108232c960e73a94802eb0d73b418c60c8696`). Real Windows render/audio input and a player-driven Save→Continue playthrough still require manual acceptance.

The project remains **Pre-Alpha**. Priorities: complete source-ordered level-up callbacks, original A7E4/24B48 hand clock/animation, DOS automatic target selection, original empty-hand heap behavior, other enemies and remaining campaign content.
