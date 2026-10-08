# Original combat closure — Strength table, HARM sequence, and E284 NPC classification

**Checkpoint scope:** immutable original CODE.2 instruction/data evidence, source-only HARM implementation and exhaustive stat tests, plus live Level 22 Assassin integration of a newly recovered E284 status transition. **This does not enable player combat.** All current C++ changes remain uncompiled since Windows GitHub Actions became manual-only; no preview EXE, native CTest success or original DOS timing equivalence is claimed.

## A. The original Strength damage table is recovered

E5F0 at logical `E733..E73D` loads the Strength score from the original character byte `actor+0x33`, then executes:

```asm
E733  mov   eax, [esp]
E736  movsx eax, byte ptr [eax+0x684B]
```

Original executable file evidence: supplied CODE.2, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical data start `0x3AF24`; original object-2 DGROUP starts at physical `0x75F24`. Thus DGROUP `+0x684B` is physically at **`0x7C76F`**. Reading 26 signed bytes for authored Strength indexes `0..25` yields:

```text
07 FC FE FF FF FF 00 00 00 00 00 00 00
00 00 00 01 01 02 07 08 09 0A 0B 0C 0E
```

| Strength score | Original signed damage modifier |
| --- | ---: |
| 0 | +7 (literal shipped byte; do **not** normalize or invent a rule) |
| 1 | -4 |
| 2 | -2 |
| 3–5 | -1 |
| 6–15 | 0 |
| 16–17 | +1 |
| 18 | +2 |
| 19 | +7 |
| 20 | +8 |
| 21 | +9 |
| 22 | +10 |
| 23 | +11 |
| 24 | +12 |
| 25 | +14 |

Original E5F0 **still takes its separate exceptional-strength branch** when score is exactly 18 and the original class-code probe finds signed class 1, 5, 0 or 2: percentile `0..75 → +3`, `76..90 → +4`, `91..99 → +5`, `>=100 → +6`. For other classes at 18, it uses the literal +2 from the table, not one of the percentile bonuses.

The native `CombatDamageStatBonusModel` has now embedded exactly those 26 **game-mechanical** signed coefficients with an original-file provenance comment; `combat_damage_stat_bonus_tests.cpp` tests every index, the special class exception and an optional caller-observed data byte mismatch. **The original executable has no bounds check** on `strength+0x684B`: for corrupted/unusual scores `26..255`, it can read subsequent DGROUP content. The bounded native model therefore requires a caller-observed adjacent memory byte for these scores; it does *not* extend the table with assumed tabletop values.

Offline source verification includes both E733/E736 exact instruction bytes **and a distinct immutable object-2 data signature** at `DATA_OBJECT_START+0x684B`. This is not a network fetch or a Windows build.

## B. HARM's two HP mutations cannot be combined

After `C80C` succeeds, original E5F0 item `0x84` takes its hardcoded `E815..E83B` branch:

1. **E815:** execute actual original `1C328(1,4,0,0)` RNG damage roll. No roll occurs on a miss.
2. **E825:** load the current NPC **signed 16-bit HP** at `npc+0x12`. At `E831` compare signed current HP to the rolled signed DWORD. If HP is **greater**, `E835` directly writes the die result's low 16 bits into the NPC HP word; if HP is less/equal it leaves HP unchanged. It does **not** heal a target below the roll.
3. **E829/E839:** emit/record effect `0x24` and set provisional outgoing injury to **zero**.
4. **E947:** because HARM is *not* Rod of Smiting `0x4A`, the attack calls original `C910` after any hit effect. Its universal `C993` minimum transforms provisional zero injury into **one**.
5. **E963:** call `E284` NPC injury with that resulting damage. The target can therefore lose another HP point after HARM's direct clamp and can die when its HP reaches zero.
6. Subsequent `E5F0/24B48` effects and **separate** `A7E4` hand deadlines still run as previously audited. This exact HP→effect→damage-gate→injury→timing ordering is part of the timing fidelity contract.

For example, target 20 HP and actual HARM roll 3 gives **3 HP after direct clamp**, then **2 HP after C910/E284**. Target 1 HP and roll 4 is **not healed** to 4; C910/E284 can reduce it to zero. These are real executable implications, not inferred tabletop descriptions.

`CombatHarmPreludeModel::inspect(previous_signed_hp, actual_1d4_result)` is a **pure plan** for the direct clamp/FX/provisional damage. It refuses fabricated rolls outside 1..4 and leaves RNG and NPC memory untouched. New `combat_harm_prelude_tests.cpp` explicitly composes the result with existing `CombatWeaponDamageGateModel` and `CombatNpcInjuryPreludeModel` to check the correct *sequence*, including 1 HP lethal behavior and reaction countdown. This fixture does not itself initiate a player attack.

## C. E284 post-subtraction type classifier and the Assassin

Another original dependency inside `E284` is resolved. Once the initial type-`0x08`/`0x0F` HP rules execute, original `E306..E31F` calls `3ACDC(npc_type)`. Direct disassembly of `3ACDC:2ACE8..2AD14` shows:

```text
type < 0x22   → classifier true
type == 0x50  → classifier true
otherwise     → classifier false
```

If true, `E317` clears bits `0x30` from **BYTE `npc+0x0B`**, then `E31B` ORs bit `0x10`. Because the native record exposes original DWORD `npc+0x08` as `LiveNpcRecord::runtime_flags`, this is:

```cpp
runtime_flags = (runtime_flags & ~0x30000000u) | 0x10000000u;
```

This transition occurs **whether resulting HP is positive or nonpositive**: the signed HP gate at `E31F..E324` is *after* the classifier.

The Level 22 Assassin is NPC type **`0x09`**, which satisfies `<0x22`. `CombatNpcInjuryPreludeModel` now reports this transition along with HP, reaction packed word and 1/2 action countdown. The existing `Level22EncounterModel::damage_assassin` writes these original results before its already implemented type9 damage/death branch; the unchanged later logic still handles the **four item drops `0x88,0x87,0xE9,0x74`**, XP and death/reaction audio.

Extended tests assert that NPC runtime flag bit changes occur on **nonlethal** hits, persist through death, preserve unrelated bits, and follow the original `3ACDC` classification boundaries `0x21/0x22/0x4F/0x50/0x51` without erroneously marking every NPC.

**Important limitation:** E284's subsequent type-specific death, other NPC special events, `38110` visual feedback, `1DDA4` game systems and full original scene update remain open. The source model does not claim full general NPC combat compatibility.

## D. Evidence, build policy and acceptance gates

- Six exact new original opcodes for the E733/E736 Strength lookup and E815 HARM mutation, plus **eight** for the E306/3ACDC classifier and HP-positive gate, were checked directly against the supplied executable: **14/14** pass.
- The separate **26-byte DGROUP table** also matched byte-for-byte: **1/1** pass. The original SHA-256 was confirmed.
- The offline `tools/verify_combat_entry_bytes.py` now registers **226 machine-code anchors** (212 prior + 14 new) plus **one object-2 data anchor**. This is an offline evidence audit, **not** a native C++ compile or emulator comparison.
- New source: `include/rloft/combat_harm_prelude.hpp`, `src/combat_harm_prelude.cpp`; CMake target `combat_harm_prelude_tests`. Updated source: `combat_damage_stat_bonus.cpp`, `combat_npc_injury_prelude.cpp`, and `level22_encounter.cpp`, with corresponding regression updates.
- **GitHub Actions remains manual-only** while private. No new preview EXE, CI run or current-source Windows CTest success is claimed.

### Remaining in execution order

1. Reconstruct the rest of original **E284** beyond its now accounted initial type classifier: type-specific death handling, other NPC cleanup, presentation, XP, drops and special events, with the correct dependencies.
2. Reconstruct the **original RNG/1C328 draw sequence and E5F0/24B48 frame/audio cadence**, connecting HARM's pre-injury clamp, C910 and E284 at the right time.
3. Finish input-driven `214B8/26018` attack orchestration and capture actual DOS clock/RNG/NPC snapshots; add a true live attack to the Level 22 Assassin only when all essential gates are known.
4. Build/test the **consolidated** current-HEAD native code at the user-authorized checkpoint or after publication; never silently use repeated private-repo Windows minutes.

**Bounded Strength/HARM/E284 classifier checkpoint: 67% (4/6 gates).** Original bytes, source reconstruction, deterministic tests and scoped encounter wiring are complete; current-head compiled verification and original-DOS runtime/timing comparison remain open.

**Overall faithful-demo acceptance remains 20% (1/5 major release gates).**
