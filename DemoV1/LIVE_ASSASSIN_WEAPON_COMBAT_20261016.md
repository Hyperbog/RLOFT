# Level22 Assassin: real source-backed weapon hit and death

**Date:** October 16, 2026 · **Native game branch:** `demo-v1` · **QA:** source-committed, NOT yet a confirmed Windows gameplay build.

## Player-visible game change

In the actual unified Win32 game, click a **valid equipped hand tile**, then click the visible Level22 Assassin in the original source-projected world. The live input path verifies the original 214B8 and both-hand 24478 legality against current LEVELS header+0x290 and the equipped item. The second click resolves the authentic 223F4 class-0 NPC record, including render-rank and geometry bounds, and authorizes an **explicit** live family-A target in the original E5F0 player attack routine.

This is an **enhanced native two-click targeting interaction**, not the original DOS one-click auto-targeting implementation. The original E5F0(-1) automatic DB94/DACC projected-cache target lookup is still outstanding. It is deliberately NOT replaced with nearest-monster search or guessed OpenGL coordinates.

### Actual original damage order used

1. Require the current Level22 world scene snapshot, coordinate/facing, NPC type 9 and positive health, and recheck the hand against the actual party image. These checks precede mutation.
2. Stage the **complete native ExplorationRuntime**, including party, NPC world, CODE.2 table models and OriginalRandom state.
3. Preflight an ordinary positive item ID with category 9, 10 or 11. The Rod of Smiting 0x4A and spell-hand 0x7E..0x86 have extra actions and **fail closed**, not fabricated as ordinary weapons. Load original E6C9 Strength/class final damage modifier and the original OBJECTS +38/+39/+3A weapon dice.
4. Invoke **E578** attack-sensitive invisibility cleanup on the staged party. The original service affects all four radius-invisibility records plus the attacking actor's undead invisibility.
5. If the NPC has original held flag bit08, C80C succeeds **without** a d20 RNG draw. Otherwise consume **exactly** `OriginalRandom::dice(1,20)`, build the actual character/NPCS/OBJECTS/burden/Bless/Aid/gauntlets hit inputs, and run the original C80C threshold/modified-d20 check.
6. On a miss, do not roll weapon dice. Commit the original E578 effects and d20 RNG advance; keep monster HP unchanged. On a hit, roll the original weapon damage dice, then apply original C910 creature-type/category/enchantment gate and minimum-one quirk.
7. Feed that exact damage into the **existing original E284 type-9 Level22 injury/death/drop handler with reaction mode 0**, matching actual E5F0:E957..E963 machine code. A lethal hit creates the four original items `0x88,0x87,0xE9,0x74` with the authentic drop seeds and BF44 story mutation. The original 2D5D0 world placement probe is reused, so collisions are not fabricated.
8. Only after the full operation succeeds is the staged world/party/RNG committed. A stale target, missing original table, unrecognized special, invalid weapon or failed quest drop cannot leave a partly damaged enemy or consumed RNG in live play. New game-owned native saves already include world, party, RNG and BF44.

The supplied immutable CODE.2 SHA-256 is `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. A direct disassembly of object-1 E5F0 confirmed that `E963` calls `E284` after pushing **reaction mode 0**. Separately, auditing the supplied `OBJECTS.DAT` verified **35/35** normal category-9/10/11 shipped records have positive count and sides in their authored weapon dice.

## Regression coverage

- `combat_level22_strike_tests`: deterministic CODE.2 table fixture and OBJECTS/NPCS fixtures; d20 hit, natural-one miss, RNG state after a miss, held-NPC hit with no d20 draw, E578 effect clearing, C910 minimum-one despite creature gate, invalid hand/corpse and Rod rejection before RNG, and hit→E284 lethal Assassin→four genuine quest-item IDs.
- `verify_live_level22_weapon_strike.py`: 13 anti-regression mutations guarding attack effect order, hit-only weapon RNG, source NPC identity, staged commit, actual player input and Windows CTest/build registration.
- Windows quick CMake suite now includes the new compiled test and source guard alongside hand eligibility, NPC encounter, original Level22→23, native game save and GPU tests. Release test assertions remain enabled.

## Important remaining fidelity gates

**Original 24B48 animation, actor +10B busy/write expiry, +19E hand-ready deadline and maintenance clock A7E4 are not connected.** A player can presently issue another eligible weapon attack without the DOS cooldown. This is NOT claimed to be exact combat pacing. Missing source maintenance-clock semantics must be implemented rather than silently treating VGA refresh, BIOS IRQ clock or milliseconds as A7E4.

**XP/level-up is not yet committed to the party record.** The current E284 routine accurately returns the NPC definition's original experience award; original DDA4 class XP and level-change side effects must be connected before that award can truthfully become a leveled-up character. Do not display the awarded-XP amount as applied experience.

**Damage/death sound playback** through the original NPC sound resource mapping and exact attack animation feedback remain to be integrated. The new actual NPC HP, death and relics will be visible through the existing source-driven scene/world composition.

**Most one-equipped/one-empty and both-empty-hand cases remain blocked** by original 24478's reads of heap-adjacent `OBJECTS[-1]` bytes absent from OBJECTS.DAT. A two-**equipped**-weapon character can use the new bounded native attack. No made-up DOS heap bytes or fake unarmed category are used.

**Only the explicit Level22 Assassin target** uses this new execution route. Other enemies/level AI, Rod, spell-hands, NPC player attacks and generic auto-target selection are future campaign work. The subsequent fourth-relic pickup/22-frame Level23 event is now transactional, but its original video/effect resources are not yet faithfully rendered.

**Windows build, MSVC compilation, test execution, sound verification and owner visual/play acceptance have NOT been verified for this commit.** A successful GitHub source revision is not a verified Windows executable. The game remains **Pre-Alpha**.
