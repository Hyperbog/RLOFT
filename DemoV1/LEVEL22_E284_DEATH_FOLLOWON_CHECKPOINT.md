# CODE.2 E284 NPC death follow-on — type dispatch, presentation, and respawn markers

**State: source-only original-byte reconstruction; Level 22 Assassin drop specialization connected.** No new Windows executable, automatic Actions run, complete player-initiated combat, or current-HEAD CTest verification is claimed. The existing `E284..E31F` injury and `3ACDC` classifier are documented in [the preceding checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md). This follow-on extends the instruction ledger through original logical `E577`.

## Immutable original evidence and branch ordering

Original source: user-supplied CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. CODE.2 object-1 physical base `0x3AF24` and disassembler virtual address = logical + `0x10000`.

- `E31F..E324`: **test signed NPC HP after all preceding original injury/status writes**. Positive HP jumps directly to `E553`, skipping every death-type branch. Nonpositive HP enters `E32A`.
- `E553..E577`: on a surviving hit, emit the presentation/effect request `38110(WORD[NPC_DEF+0x41] + DWORD[0x258C0], 0xFF, 0, 1)` and return. No corpse drops, `1DDA4` callback, generic death-resource base, or respawn marker.
- `E32A..E4C9`: ordered **death-only type branch dispatch**: type `0x18` with a conditional external `C1DC(0x3B)`, current-level `0x19`, `E0BC`, original `1C328(1,3,0,0)`, resource/script state and possible early return; type `0x21` calls `E028(npc)`; type `0x12` calls `C124(0x20)`; type **`0x09`** makes four original `3195C` object spawn calls and sets `DWORD BF44=1`; type `0x50` calls `C124(0x43)`; type `0x08` invokes `C124(0x51)`, spawns `0x74` at seed `0x80`, calls `102A8(0x1F)`, `102A8(0x20)`, `102A8(0x21)`, then invokes original `38DCC`, `38EF0`, `38E58` presentation/state sequence. Other types fall through to common death presentation.
- The original Assassin **four object/seed pairs** appear in the exact ordered `3195C` call arguments: `(0x88,0x6E)`, `(0x87,0x78)`, `(0xE9,0x82)`, `(0x74,0x8C)`, with original coordinate/placement handling in `3195C`. A global story `DWORD BF44` is set to 1 after the fourth call; this global story-state write is **not yet wired** into the current native host.
- `E4C9..E50B`: common death presentation selects `WORD[NPC_DEF+0x43]` and adds `DWORD[0x258BC]` when nonzero, or uses the base when zero, then invokes original `38110(...,0xFF,0,1)`. This is not the same `+0x41/0x258C0` resource expression used on nonlethal reactions. Do not infer a wall-clock audio delay from either resource argument.
- `E50F..E539`: scan exactly three **signed WORDs** starting at `DGROUP:0x258AA` (offsets `+0,+2,+4`), comparing with the NPC's **zero-extended** unsigned type. Only a match invokes `E0F8(external_context)`. If any *matching* call returns nonzero, skip the `BYTE NPC+0x0B OR 0x02` dead marker. Otherwise the original sets that bit (native `runtime_flags |= 0x02000000`). Signed `-1` tracker entries never match unsigned NPC type `0xFFFF` through this comparison.
- `E53D..E54B`: on the ordinary common-death path invoke original `1DDA4(WORD[NPC_DEF+0x23])`. The native definition labels `+0x23` as the experience-award field, but the `1DDA4` observable award/state semantics remain a separate integration dependency.
- **Type `0x18` early-exit caveat:** its conditional original branch can return before the common presentation/respawn/award stage. The new source plan explicitly marks this external prerequisite and reports common-stage arguments as *conditional*, rather than claiming a complete type-18 runtime result.

## Native source integration

A new `CombatNpcDeathFollowOnModel` is a read-only original branch plan. It distinguishes signed HP-positive and death paths, actor type scripts, exact drop data and story marker, original presentation resource base/definition offset and the call to `1DDA4`. It separately accepts **observed** original `0x258AA` tracker words and actual `E0F8` callback success results to calculate the final runtime flag without inventing a three-slot table.

`Level22EncounterModel::damage_assassin` now uses the shared model's **type-9 ordered drop data** for the existing `spawn_drop` routine. Existing `Level22Story::assassin_death_drops()` remains part of the quest identity/pickup inventory code, but is no longer a separate hardcoded source for the death drop-loop arguments. The native host still defers `38110` presentation resources, global `BF44` story state and source-driven `1DDA4` semantics to their proper original runtime service layer.

`combat_npc_death_followon_tests.cpp` is CMake-registered and covers positive-vs-zero/negative HP, exact four Assassin drops/seeds, all nontrivial type branches, type8 drop, death/versus-hit resource sources including zero offsets, signed tracker/mismatch behavior, early successful `E0F8` suppression and exhausted tracker death marking. **This test has not been compiled/executed in the current native build.**

## Byte verification, timing requirements and remaining gates

**32/32 new original executable opcode signatures** for `E32A..E577` matched the supplied CODE.2, including the four original 3195C item IDs, `BF44` write, the split damage/death resource bases, three-slot E0F8 scan, `+0x0B` death marker and `1DDA4` call. The cumulative offline verifier `tools/verify_combat_entry_bytes.py` now contains **258 CODE.2 opcode anchors plus one separate 26-byte Strength table data anchor**. Static original-byte checks are not current-source compilation or runtime acceptance.

Next:
1. Resolve the **type-18** original special condition and early-return effects only with original `C1DC/E0BC` context and true `1C328` RNG sequencing.
2. Recover actual `0x258AA` respawn tracking values in the Level 22 world and reproduce `E0F8` dynamic logic. Do **not** pass placeholder zeros or silently treat callback success as false.
3. Map `38110` presentation, the original `BF44` story-state write and `1DDA4` progression consequences to the existing world/actor state in CODE.2 order. Compare **input → C80C RNG → damage dice → E284 injury → drops/state → presentation → A7E4 hand deadlines**, preserving the pauseable gameplay clock rather than Windows display timing.
4. Execute a consolidated source compilation/CTest **only on an authorized manual run**, or locally without private GitHub Actions if a complete project checkout becomes available.

**Bounded E284 death-follow-on source checkpoint: 67% (4/6 source/evidence gates).** Original byte evidence, typed source plan, focused tests, and Level 22 drop integration are complete; compiled validation and original runtime event/timing verification remain open. **Overall five-major-gate faithful demo acceptance remains 20% (1/5).** No Windows EXE was produced.

## New E0F8 respawn body audit — exact RNG/time boundary

A follow-up direct disassembly of CODE.2 logical `E0F8..E281`, reached from the **matching** `E284:E51F` tracker slot, proves `E0F8` is **not** a static Boolean test:

1. `E0FE` calls original `30700` (game IRQ tick read) and `E109` calls `290D0` with its result. The timing domain is **not** automatically the pauseable `A7E4` attack-recovery counter.
2. Each placement attempt invokes original `30CA2` **twice** at `E11B` and `E134` to choose coordinates, followed by the original cell `0x40/0x3F` occupancy masks and two signed-distance thresholds of `0x0F` at `E185/E19C`.
3. `E1D6..E1E2` permits **up to three** candidate attempts; after three failures it returns literal **0** at `E279`. A successful candidate writes a 22-byte-stride original NPC slot and its signed coordinates, state `0x8000`, destination NPC type and other definition-derived fields.
4. **Only on successful placement**, the routine calls original `1C328` at `E233` to roll the new NPC's HP from the NPC definition's hit-dice byte and six-sided dice (other two damage bonuses zero), then returns literal **1** at `E26D`.
5. Therefore `E0F8` can advance the game's original RNG stream and mutate the NPC array *after* death presentation but *before* `E539` decides whether to mark the dead NPC. Calling a modern respawn callback without reproducing those draws and checks would alter subsequent attack dice, game-world state and timing.

**15/15 more original CODE.2 opcode matches** were verified at `E0F8` (including its IRQ tick call, both `30CA2` draws, obstruction/distance thresholds, max-attempt branch, actor record writes, HP roller, success and failure return values). The cumulative offline verifier now has **273 original x86 instruction signatures plus one 26-byte Strength data signature**. This additional research does **not** claim to implement E0F8 or supply the three original `258AA` tracked slot values. The pure `CombatNpcDeathFollowOnModel::death_runtime_flags` still requires the caller to supply observed tracker types and **actual** E0F8 success/failure results; no synthetic resurrections or RNG draws are enabled.
