# Original CODE.2 combat closure — 22E40, Rod of Smiting, E284 injury prelude

**Checkpoint state:** Original executable bytes independently verified; native source/test fixtures committed; first E284 prelude reused in the existing Level 22 Assassin damage path. **No new current-HEAD C++ compilation, Windows Actions run, player attack integration, or original DOS time-series certification has occurred.**

Original file authority: supplied `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; CODE.2 object-1 file base `0x3AF24`. Addresses below are logical CODE.2 offsets; objdump's adjusted virtual addresses add `0x10000`.

## 1. 22E40 eliminates an open Vampiric Touch helper dependency

Direct original disassembly `22E40..22E8C`:

- `22E40`: compute original actor stride `actor*0x248`.
- `22E48/22E5D/22E72`: compare three **signed** actor class bytes at `+0x15,+0x16,+0x17`, in that order, against the requested signed class code.
- `22E55/22E6A/22E7F`: for the **first matching** class lane, sign-extend the corresponding original **16-bit class-level word** at `+0x20,+0x22,+0x24` and return immediately. The highest of multiple matching class levels is **not** used.
- `22E87`: when none match, return literal **1**, not zero.
- `E5F0:E845..E867` for Vampiric Touch item `0x7F` calls `22E40` with **class code 4 and the attacking actor index**, computes signed `SAR` / +1 dynamic die count, then calls the original `1C328(N,6,0,0)` damage roller. It does **not** call an unidentified generic caster-level routine.

Native `CombatClassLevelModel::for_class` implements the exact three lane probes and signed fallback. `CombatDamageDiceModel::inspect_for_actor` now uses this real helper to produce the `0x7F` dice count directly from a `PartyImage` snapshot, including the exact first-matching-lane rule. Its outputs remain **read-only**. No dice are rolled merely by requesting the plan. Tests cover no matching class, 3 possible lanes, duplicate classes, negative class/level, zero level, and a Vampiric Touch test whose dice count changes when the class lane changes.

This **closes the original 22E40 decoding dependency**, subject to a deferred native compile. Actual spells still require a live target, a C80C hit, correct `30CA2` RNG draw count, effects, and action timing.

## 2. Rod of Smiting 0x4A has unreachable critical/charge branches

Direct original analysis exposes a stronger fact than the previous notes:

- `E5F0:E6AB` calls the original `C80C` hit resolver and stores its **return value** at local `[esp+8]` (`E6B5`). Nonzero enters post-hit damage.
- `C80C:C81E` returns literal `1` for a held target; `C8EA` returns `1` for modified-d20 `>=20`; `C8F9` returns `0` for the modified-one miss; `C900..C908` returns only `SETGE` **0 or 1** for the remaining comparison.
- `E8D2` and `E912` compare that stored **Boolean return**, not the d20, with literal `0x14` (20). It cannot equal 20 on the original normal attack path.
- Thus `E8D9..E8FE` and `E919..E945` charge-based kill/triple-damage branches are **present in the executable but unreachable** from the normal `E5F0` → `C80C` path. No charged rod counter is read/decremented there, despite the apparent game-rule intent.
- For NPC type **`0x17` or `0x1F`**, the reachable `E900..E910` path makes an **additional** `1C328(2,8,6,0)` roll after the initial ordinary OBJECTS-weapon roll. This entails **two additional original RNG draws**, with +6 **per die**, and replaces the previous provisional damage.
- For other NPC types, the reachable `E912..E957` path keeps the previous provisional damage, consumes **no** further RNG and skips `C910`. Both cases pass the result to original `E284`.

This is a **shipped executable behavior**, not a recommendation to change the intended D&D rules. Native `CombatRodOfSmitingModel::inspect(type,c80c_return)` accepts the actual C80C Boolean (rejects a mistaken raw d20=20), returns the conditional extra dice recipe and explicitly reports no charge consumption. The original `OBJECTS.DAT` Rod of Smiting `0x4A` record has original `+0x31` charge index byte 0, but the unreachable branch must not be resurrected just because that index exists.

The existing `CombatHitCheckModel::resolve` now also exposes the **exact original EAX Boolean return** (`CombatHitCheckResult::original_eax_return`), separately from its calculated `modified_d20`. A direct regression chains a raw d20 of 20 through `C80C` to returned **1**, then into the Rod model, confirming the charge path stays dead. This prevents future integration from accidentally passing the wrong d20 field.

A future DOS runtime trace is still required to validate representative Rod attacks. When a legacy source comment calls the dead branch “crit on natural twenty,” **do not map a host raw roll to the comparison**; the original C80C returns only Boolean.

## 3. E284 NPC injury prelude reconstructed and reused by Level 22

Original `E284..E306` early mutation occurs after post-roll weapon/creature damage selection and **before** creature-specific death/drop/AI/audio processing:

1. Given original reaction-mode DWORD, write NPC `WORD+0x10 = 2` if zero, otherwise `1`. If original NPC definition `BYTE+0x1C` is nonzero, OR low packed state `WORD+0x14` with `0x0006`. Preserve unrelated bits.
2. NPC type `0x0F`: do **not** subtract incoming damage from HP; still service reaction state and continue original later type classification.
3. NPC type `0x08`: if high packed flags `WORD+0x14 & 0xC000 == 0xC000` OR signed HP `<=10`, skip HP subtraction. Otherwise perform the same 16-bit subtraction; if the new signed HP is `<=0`, clamp to literal **10**, rather than allowing death through that prelude.
4. Other NPC types (including Level 22 Assassin original type **`0x09`**): perform **16-bit** subtraction of `AX` from signed `WORD+0x12`, preserving original arithmetic wrapping, without a generic clamp.
5. **Historical scope through `E306`.** Later direct auditing has now decoded `E306..E324`'s `3ACDC` classification and `NPC+0x0B` state change, with implementation in the shared injury prelude and Level22 Assassin path. **The remainder of E284's creature-specific death/respawn/audio routines remains open**; see [newer checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md).

Native `CombatNpcInjuryPreludeModel::inspect` reports resulting reaction bits, countdown and signed HP without world mutation. New tests exercise types `8,9,15`, packed high-mask C000h, reaction zero/nonzero, HP floor, signed negative results and 16-bit wrapped damage. The existing `Level22EncounterModel::damage_assassin` now calls this common exact prelude for original type 9 **before** its existing Assassin-only death-sound, XP, and four quest-item drop logic. Existing encounter regression tests have been extended to assert preservation of unrelated packed-state high bits and countdown 2/1.

**This integration reuses existing Level 22 damage behavior; it does not enable new attack input or remove any original NPC death calls.** It has not yet received a fresh compiled Windows test.

## 4. Byte verification, source and next gates

The standalone offline verifier `tools/verify_combat_entry_bytes.py` now registers **212 CODE.2 machine-code signature anchors** (previous 173 + **24** for `22E40/C80C/Rod` + **15** for `E284`). All **39/39** newly added anchors independently byte-matched the original immutable `CODE.2` in a local audit. No Windows GitHub Actions minutes were consumed.

Sources/targets registered in `DemoV1/CMakeLists.txt`: `combat_class_level`, `combat_rod_of_smiting`, `combat_npc_injury_prelude`, with matching tests; updated `combat_damage_dice` and `level22_encounter` now call the appropriate bounded logic.

Remaining blockers in dependency order:

1. **Closed in subsequent source audit:** DGROUP `0x684B` signed Strength modifiers for scores `0..25` were recovered from CODE.2 object-2 physical `0x7C76F`, and the native damage model uses those actual bytes. Only unsupported scores above 25 still require adjacent loaded DGROUP evidence; see [newer checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md).
2. **Source-only HARM HP-clamp → C910 → E284 ordering is now reconstructed and cross-model tested**; actual RNG, HP effects and audiovisual timing in live player attacks, plus later E284 creature death/respawn, remain incomplete.
3. Finish player attack input `214B8/26018`, original `A7E4` hit/action deadlines and `24B48` animation/sound timing without substituting display ticks for gameplay-maintenance clocks.
4. Compare deterministic original DOS input, RNG, NPC HP, animation and clock traces to the native implementation. Run a **consolidated** manual Windows compile/CTest only when intentionally authorized or after the repo becomes public.

**Bounded class-level/Rod/E284-prelude source checkpoint: 67% (4/6 gates: source opcode audit, native models, regression registration and prelude integration complete; current-source compile and original-runtime comparison outstanding). Overall faithful-demo acceptance remains 20% (1/5 major gates).**
