# Level 22 original combat damage — E5F0 dice dispatch and C910 weapon gate

**State:** source-only instruction-accounted reconstruction committed to `demo-v1`. **No Windows EXE, automatic GitHub Actions run, damage-enabled input, or current-source CTest pass is claimed.** GitHub Actions remains manual-only while this repository is private. The original source is the supplied immutable `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical file base `0x3AF24`. Logical CODE.2 offsets below differ from objdump's virtual-address display by `+0x10000`.

## Boundary and sequencing

Only after original `E5F0` obtains a real target and `C80C` returns a nonzero hit does it enter the damage branches `E6C9..E963`. The original calls `1C328` dice (which itself calls `30CA2` RNG once per die), may emit hit effects through `38110`, then processes target/item-specific effects, calls the general `C910` damage gate for non-Rod-of-Smiting IDs, and passes the resulting value into `E284` NPC injury. It then schedules the two different attack deadlines and hand action presentation documented in [the earlier timing checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md).

**Never roll dice during hand inspection, target preference, an unsuccessful C80C check, or a speculative UI hover.** The new damage models return pure plans and NEVER advance RNG, mutate an NPC, start an animation or apply HP loss. Live orchestration remains pending.

## Original ordinary weapon dice and stat bonus

- `E5F0:E86E..E88F` reads OBJECTS record **`+0x38`** as dice count, **`+0x39`** as sides, and **`+0x3A`** as bonus **per die**. It passes a *fourth* original character-derived final bonus computed earlier at `E6C9..E768` to `1C328(count,sides,per_die_bonus,final_bonus)`. The native `ObjectDefinition` now exposes exactly those three serialized bytes.
- The earlier final bonus uses actor **`+0x33`** raw strength and **`+0x3F`** exceptional-strength percentage. When raw strength is **18** and the signed class lanes include any code **1, 5, 0 or 2**, the exceptional percentage gives **3** at 0–75, **4** at 76–90, **5** at 91–99, **6** at 100+. The class tests are original `23EAC`, not guessed from displayed race/class strings.
- Every other case uses the **original signed-byte lookup at DGROUP `0x684B + strength`** (`E733`). The authored signed entries for Strength scores **0–25** have since been recovered exactly from CODE.2 object 2 (physical `0x7C76F`), and `CombatDamageStatBonusModel` now uses those actual bytes. For scores above 25 the original indexed machine instruction reaches adjacent DGROUP, so the native helper requires an observed byte. See [newer checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md).
- `CombatDamageDiceModel` likewise requires the genuinely computed final bonus for a non-pseudo item. It preserves the item+3A per-die value and does not invent a generic damage modifier.

Sample original OBJECTS.DAT records, not distributed by Git:

| Item ID and name | Category | Dice count (+38) | Sides (+39) | Bonus/die (+3A) |
| --- | ---: | ---: | ---: | ---: |
| `0x2E` SPEAR | 10 | 1 | 6 | 0 |
| `0x2F` THROWING KNIFE | 10 | 1 | 3 | 0 |
| `0x30` COMPOSITE BOW | 0 | 1 | 6 | 0 |
| `0x31` SLING | 0 | 1 | 4 | 1 |
| `0x59` DAGGER OF THROWING | 11 | 1 | 3 | 2 |
| `0x4A` ROD OF SMITING | 11 | 1 | 8 | 3 |

These are byte evidence for equipment data; not every item in the table is necessarily dispatched as an ordinary direct `E5F0` attack by `26018`.

## Nine-entry pseudo-hand jump table

The actual original table at logical `E5CC` contains **nine 32-bit CODE.2 jump addresses**, mapped by index `item-0x7E`. Each pointer and each relevant instruction was byte checked; it is **not** safe to infer the case order from English spell names.

| ID | Original pseudo-hand item | Original `1C328` dice args | Other original consequence |
| --- | --- | --- | --- |
| `0x7E` | CHILL TOUCH | `(1,4,0,0)` | Standard gate continues |
| `0x7F` | VAMPIRIC TOUCH | `(N,6,0,0)` | `N` now comes from **`22E40(class=4, actor)`**, which returns first matching signed class level or fallback 1, followed by original signed halving and +1; no guessed count |
| `0x80` | CLAWS OF THE UMBER HULK | `(2,6,0,0)` | Hit effect code `0x0C` |
| `0x81` | CAUSE LIGHT WOUNDS | `(1,8,0,0)` | Standard gate continues |
| `0x82` | CAUSE SERIOUS WOUNDS | `(2,8,1,0)` | Hit effect code `0x09` |
| `0x83` | CAUSE CRITICAL WOUNDS | `(3,8,3,0)` | Hit effect code `0x09` |
| `0x84` | HARM | `(1,4,0,0)` | If NPC HP exceeds the roll, first lower HP to the roll; set outgoing pre-`C910` injury to zero; hit effect `0x24` |
| `0x85` | FLAME BLADE | `(1,4,4,2)` | Standard gate continues |
| `0x86` | LICH TOUCH | `(1,10,0,0)` | Standard gate continues |

`CombatDamageDiceModel` reports these arguments, original effect discriminators and the separate HARM special branch. For `0x7F`, the original `22E40` result can now be computed directly with `CombatDamageDiceModel::inspect_for_actor` using the actual actor's signed class levels; the original standalone `inspect` still rejects a missing caller-supplied helper result. Its signed-divide behavior is reproduced if that input is provided; counts outside the existing native `OriginalRandom::dice` u8-count API are rejected rather than silently truncated. More instruction work may be required to connect the full `22E40` semantics.

**HARM source closure:** the original `E815..E839` changes target HP directly before the general gate; `C910` then enforces **minimum-one resulting injury**, followed by a second HP subtraction in `E284`. The later [`CombatHarmPreludeModel` checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md) now represents this exact order and has a composed cross-model regression. It does **not** yet roll RNG, apply HARM in live player combat or certify presentation timing.

## C910 original weapon/creature damage gate

The original `C910` receives damage, NPC definition pointer (DWORD **`+0x31`**), and item ID:

1. Empty hand is classified as original **category 9** *inside C910*, distinct from `26018`'s category 10 for an unarmed attack.
2. For nonnegative item IDs, use OBJECTS **`+0x2F`** category unless enchantment byte `+0x37` is nonzero, in which case effective category is **11**.
3. Effective category **9**: NPC flag DWORD `0x8000` zeroes provisional damage. Effective category **10**: NPC flag DWORD `0x4000` zeroes it. Effective category **11**: NPC flag `0x20000` zeroes it under the shipped executable's actual signed comparison; NPC flag `0x40000` does not produce an additional veto with the same zero-extended enchantment operand.
4. **Executable quirk, preserved:** `C975` loads the enchantment byte with `MOVZX`, then `C979` compares that nonnegative 32-bit register against **signed -2**. No zero-extended 8-bit value 0..255 is less than -2, so the `JL` branch is never taken. The symmetrical `C988..C98F` zero-extended value is always `>= -2`. This must not be replaced with imagined signed-enchantment semantics.
5. Finally `C993..C99D` clamps **any provisional damage below one to one** (including negative values, HARM's pre-gate zero, or values suppressed by creature flags).

The new read-only `CombatWeaponDamageGateModel` separately reports the original creature restriction being triggered and whether the final minimum clamp occurred, even if the input damage already equaled one. `E5F0` **excludes hardcoded Rod of Smiting `0x4A` from the normal C910 call**, instead entering original `E8BB..E955`. The later `C80C` comparison to 20 actually receives **only a Boolean return 0/1**; the apparent charged critical-hit branches are unreachable. Reachable monster type `0x17/0x1F` behavior makes an **extra `1C328(2,8,6,0)` roll**, while other types retain initial damage. The separate [Rod model](LEVEL22_CLASS_LEVEL_ROD_INJURY_CHECKPOINT.md) now exposes this outcome without consuming RNG.

## Tests and acceptance

- Committed `combat_damage_dice.hpp/.cpp`, `combat_damage_stat_bonus.hpp/.cpp` and `combat_weapon_damage_gate.hpp/.cpp` with deterministic regression programs registered in `DemoV1/CMakeLists.txt`.
- Tests exercise all nine pseudo IDs, exact original per-die and final-bonus ordering, exceptional-strength 75/90/99 thresholds, signed unobserved-table rejection, mandatory 22E40 input for Vampiric Touch, HARM/rod tagging, creature flags `0x4000/0x8000/0x20000/0x40000`, zero-extended enchantment behavior, and minimum-one effects.
- **30 more E5F0 original instruction/data signatures** (including the complete nine-entry jump table) and **17 more C910 instruction signatures** matched the immutable supplied CODE.2 byte-for-byte. The cumulative `tools/verify_combat_entry_bytes.py` now contains **173 signatures** (126 previous + 47 additional).
- **No new C++ compiler/CTest execution is claimed**. These deterministic tests are only source-registered until a deliberately requested consolidated build. The historical #631 Actions pass predates all these additions.

## Next faithful integration stages

1. Original DGROUP `0x684B` signed Strength table scores **0–25 are recovered and byte verified**, as is `22E40` Vampiric Touch. Only out-of-range Strength scores still need adjacent loaded bytes; see [later checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md).
2. Rod of Smiting's reachable target-type branch and **unreachable charged-crit path** are audited; HARM HP clamp has now been **staged through C910 and E284 in source**. Complete the later creature-specific `E284` type/death/audio effects and live timed attack integration.
3. Reconstruct the precise `E5F0` event/visual call sequence and NPC damage handoff `E284`, including hit-roll check, original `1C328` RNG draw count and `38110` effect timing. Only the initial E284 prelude is now reused by existing Level 22 Assassin damage; the rest of creature-specific death and live player-attack integration remains open.
4. Capture original DOS RNG, action-clock and animation/input traces before claiming combat temporal fidelity.
5. Keep Windows GitHub Actions manual-only and defer executable publication until a meaningful consolidated, explicitly authorized verification point.

**Bounded E5F0 dice, stat bonus, C910 source checkpoint: 67% (4/6 gates): machine-code/asset audit, exact source models, regression fixtures, and CMake registration complete; current-source compiled verification and original-world integration/runtime matching remain open.**

**Overall five-major-gate faithful-demo acceptance remains 20% (1/5).** Source reconstruction is not yet functional combat.
