# Level 22 combat hit-check checkpoint — CODE.2 C80C

## Purpose

The next incremental normal-gameplay component is **player attack accuracy**, distinct from the existing `Level22EncounterModel::damage_assassin` NPC hit-point/death/drop mutation. No viewport click is converted to damage until the exact original weapon/touch/unarmed attack-command, damage gate, cooldown, and feedback order is implemented.

## Direct controlling bytes

Re-verified original immutable CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314` against logical `C80C..C90C` (objdump virtual `1C80C..1C90C`):

- `C818..C827`: live NPC packed byte `+8 & 0x08` → immediate automatic hit; **no RNG**.
- `C82C..C859`: weapon ID `-1` yields zero enchantment; otherwise OBJECTS byte `+0x37`. When the caller enables Strength adjustment, subtract `C7C4`'s result.
- `C85B..C87A`: `FE44` burden rank `0..4`, penalties `0,0,1,2,4`.
- `C87B..C8A0`: `character +0x10A` base attack target minus signed NPC armor class `+0x21` plus weapon/Strength/burden accumulator. Subtract four when live NPC byte `+8 & 0x80` (Slow).
- `C8A1..C8D5`: Bless (character `+0xF3` bit 0), Aid (bit 3), and equipped slot 20 object `0x54` give modified die bonuses `1+1+3`.
- `C8D6..C908`: dice helper receives `1d20 + bonus`. Compare **modified** result `>=20` first (hit), then `==1` (miss), otherwise `>=threshold` (hit). This is *not* natural-d20-first tabletop behavior.

The full original orchestration is `E5F0..EA81`, including `DB94` target choice, `E578` stealth-effect removal, `C910` damage gating, `E284` NPC death and hand readiness. Those are separate future integration tasks.

## Native source

- `include/rloft/combat_hit_check.hpp`, `src/combat_hit_check.cpp`: side-effect-free `CombatHitCheckModel::resolve`, returning hit/miss, held-target auto-hit status, threshold, burden penalty, die bonus, and adjusted d20.
- The input contract uses original fields **or** outputs from already distinct original helper routines; it does *not* substitute guessed strength/encumbrance thresholds or generate RNG internally.
- `tests/combat_hit_check_tests.cpp`: threshold boundary, adjusted 20 hit from natural 19+Bless, natural 1 becoming a valid adjusted 2, raw 1 miss, held-target no-roll, armor, Strength gate, burden, Slow, Aid/gauntlets and invalid inputs.
- CMake and assertion-enabled CTest registration are included in the normal Windows build.

## What remains

1. Connect original player combat action command `214B8`/hand timing to the original attack orchestrator `E5F0` (not the world-viewport pickup primary-click route).
2. Decode current character base THAC0, Strength/exceptional-strength bonus, burden `FE44`, active-effect bits, equipment and the original `30CA2` dice helper without changing evaluation order.
3. Resolve actual world target via original `DB94`, preserve no-target/miss feedback, and check the NPC weapon-category gate `C910`.
4. Calculate weapon/unarmed/touch damage, special effects and cooldown *before* calling the already existing `Level22EncounterModel::damage_assassin` mutation.
5. Run deterministic original DOS trace comparisons plus real input playtest before enabling player attacks.

**Scope acceptance:** original hit-check arithmetic is implemented with synthetic regression tests; fresh CI and original DOS combat trace equivalence have not yet been certified. This alone does not close the major combat release gate.

**Completion:** bounded combat-hit arithmetic milestone **67% (2/3: opcode reconstruction and test registration; fresh CI unverified)**. Overall five-gate faithful demo **20%** pending end-to-end gameplay acceptance.
