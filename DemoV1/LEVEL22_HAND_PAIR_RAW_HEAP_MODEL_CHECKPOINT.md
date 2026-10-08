# CODE.2 24478 complete pair-decision model, conditioned on original raw memory

## Completed source path

The native `CombatHandPairEligibilityModel::evaluate` now covers **all combinations of two valid original item IDs** supported by the CODE.2 `24478` branch ledger:

- Ordinary + ordinary (including two qualified items, asymmetric weight precedence, class-group and `23EE4` exceptions);
- Temporary pseudo spell-hand `0x7E..0x86` + ordinary item, either hand order, through the original `24641/24693` candidate override and then `246E9` qualifier/weight arbitration;
- Two pseudo spell-hand objects, through original `24522` short circuit, skipping the `0x7240` general actor-status filter and qualifier/weight arbitration;
- Two empty hands (`-1,-1`), through original `244CE` short circuit **only when the original `240A8(-1)` category read is supplied**; no ordinary weight/qualifier read is made;
- Mixed `-1` + any valid nonnegative item, **only when the complete observed original pre-OBJECTS category/weight/qualifier fields are supplied**.

The original `24478` final action-state dword checks for both hands at actor `+0x10B/+0x10F` are performed on every branch, including both early short circuits. These results still do not execute combat or confer permission to bypass `214B8` / `26018` input ordering, animation or recovery timing.

## Explicit raw-memory dependency

The original object-loader `268C8..2692C` returns `OBJECTS_PTR = low_level_heap_allocation + 10` via allocator wrapper `4104C`. A signed object ID of `-1` makes source routines read **outside** OBJECTS.DAT:

| Original expression | Offset relative to OBJECTS_PTR | Offset relative to low-level allocation start | Required representation |
| --- | ---: | ---: | --- |
| `item[-1].category`, record `+0x2F` | `-0x12` (-18) | `-8` | Required for every `-1` hand, including both empty |
| `item[-1].weight`, record `+0x30` | `-0x11` (-17) | `-7` | Required for mixed `-1` + nonempty pair |
| `item[-1].qualifier`, word `+0x35 & 3` | `-0x0C` (-12) | `-2` | Required for mixed `-1` + nonempty pair |

A new `CombatEmptyHandRawFields` type carries these values; the last two are optional. This is deliberate: **both-empty jumps directly to final busy checks**, so its code does not read weight/qualifier and does not require inventing them. Any mixed pair lacking either field is explicitly rejected. The qualifier must be the already-masked two-bit value `0..3`.

**This is a parameterized, original-byte-faithful control-flow reconstruction, not a claim about the actual DOS heap bytes.** The engine cannot generate these fields from OBJECTS.DAT or assume a stable value across original boots. Original allocator trace or DOS runtime instrumentation remains necessary before feeding authentic empty-hand data to playable combat.

## Original pseudo-item object data

The original supplied `OBJECTS.DAT` includes **nine** `0x41`-byte definitions at IDs `0x7E..0x86` (`CHILL TOUCH`, `VAMPIRIC TOUCH`, `CLAWS OF THE UMBER HULK`, `CAUSE LIGHT/SERIOUS/CRITICAL WOUNDS`, `HARM`, `FLAME BLADE`, `LICH TOUCH`). All nine ship with `+0x2F` category 0, `+0x30` weight 0, and `+0x35 & 3` qualifier 0. These records explain why positive pseudo+ordinary arbitration can use *real* object fields. No original assets are distributed in the repository.

## Important different early status predicates

- `240A8:24228..24242` exact `status0 == 0x8000 && status1 == 0x8000` test is used for that helper's classified-item special party-mode gate.
- `24478:244EC..24506` and its repetitions use `status0 >= 0x4000 && status1 >= 0x4000` to determine the **separate** empty/pseudo hand early-branch override when `mode == 0xFF && actor >= 2`.
- Do not collapse these comparisons. With statuses `0x4000,0x4000`, `240A8` may veto a classified `-1` while `24478`'s later early gate permits candidates to persist; the initial veto remains in effect for both empty hands.

## Implementation and evidence

- `src/combat_hand_pair_eligibility.cpp` and its header now use the shared `CombatHandPairEarlyGateModel`, preserved `CombatSingleHandEligibilityModel` classifications, the original object category/weight/qualifier fields, `23EE4` class exceptions, status masks, and final separate busy-state fields.
- `tests/combat_hand_pair_eligibility_tests.cpp` covers paired empty/pseudo, mixed pseudo+ordinary in both orders, `240A8 ==0x8000` vs `24478 >=0x4000` boundary, explicit `-1` category-only requirement for both empty, additional mandatory raw weight and qualifier for mixed pairs, weight priority reversal, equal/unequal qualifier vetoes, invalid qualifier and actor, and mutation-free probing.
- `tests/combat_hand_pair_early_gate_tests.cpp` isolates every earlier branch including whether it bypasses object arbitration.
- 15 additional original `244CE..246E9` signatures were validated directly against the supplied immutable CODE.2; the consolidated `tools/verify_combat_entry_bytes.py` holds **87 original instruction anchors**.
- All new C++ sources/test targets are registered in `DemoV1/CMakeLists.txt`, but **have not been compiled or executed since GitHub Actions was changed to manual-only to conserve private-repository minutes**. The prior 102/102 test result and EXE are from earlier source.

## What is still missing for real combat

1. Capture/replicate the original heap-adjacent three bytes across real DOS runs, determine their stability and lifetime, and supply them to the actual native attack state without writing guessed serialized values.
2. Integrate the complete `24478` result into `214B8` action eligibility and `26018` dispatch at the exact original point, honoring per-hand busy state and original timing/order.
3. Complete `E5F0` attack target selection, original RNG/damage, `C910` weapon gate, NPC state mutations, visual/audio action frames and hand recovery.
4. Compare input→tick→world-state→render/sound timelines against original DOS before claiming functional combat accuracy.
5. Run one intentionally requested consolidated Windows build/test after a meaningful overall functional milestone, not repeated old previews.

**Bounded `24478` decision-model checkpoint: 67% (4/6 evidence/source gates; authentic heap-state input and compiled/original-runtime validation remain open).** Overall faithful-demo acceptance remains **20% (1/5 major release gates)**. No GitHub Actions build is needed for source-only progress.
