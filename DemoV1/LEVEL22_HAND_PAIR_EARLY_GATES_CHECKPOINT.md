# CODE.2 24478 empty-hand and pseudo-hand early gates

**Source-only checkpoint.** No GitHub Actions job, Windows EXE, or general combat permission is generated here. The original DOS machine-code authority is the supplied `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical file base `0x3AF24`. The original `24478` routine uses several conditionally bypassed paths **before** the ordinary qualifier/weight arbiter starts at `246E9`.

## Important correction: two *different* party-status tests

Prior exploratory notes accurately reported that `240A8:24228..24242` checks whether **both first-party status words equal `0x8000`**. An independent audit of **`24478:244EC..24506`** has now proven that a *different*, later branch checks whether **both zero-extended first-party status words are `>= 0x4000`**. This `0x4000` threshold reappears at `24566`, `245D1`, `24616`, `2466E` and `246C4`. In the special party mode `0x258A8 == 0xFF` for actor index `>=2`, any first-party status below `0x4000` vetoes or clears candidates as described below.

**Do not replace the first function's exact `0x8000` equality test with the later function's `0x4000` threshold, or vice versa.** They are separately executed original machine-code branches.

## Original `244CE..246E9` branch ledger

Original source was checked directly at the logical addresses below. `23F34` is itself an unsigned `0x7E..0x86` pseudo-hand ID test, so signed `-1` does **not** count as pseudo.

| PC range | Condition | Exact candidate effect |
| --- | --- | --- |
| `244CE..2451D` | Both hand item IDs are `-1` | Keep the two prior `240A8` eligibility results and jump directly to the two hand action-state busy checks at `24A58`; if special party mode `0xFF`, actor >= 2 and either first-party status is below `0x4000`, clear **both** before that jump |
| `24522..24597` | Both hand IDs are within `0x7E..0x86` | Force **both** candidates to 1; under the same disallowed special party condition force **both** to 0; jump directly to `24A58`, bypassing general actor `0x7240` and later item arbitration |
| `2459C..245B9` | Neither early-return case happened | Actor's own status word `& 0x7240 != 0` clears both candidates, without returning yet |
| `245B9..24641` | One hand is `-1` | Under the disallowed special party condition, clear that *specific* empty hand candidate; continue (do **not** early-return like the both-empty case) |
| `24641..24693` | First hand `0x7E..0x86` | Override the *first* candidate with the party gate's allowed result, even after the earlier actor-status veto |
| `24693..246E9` | Second hand `0x7E..0x86` | Override the *second* candidate likewise |
| `246E9..24A56` | General two-hand arbitration | Still compares the original **classification flags** returned through `240A8` out-pointers and may read OBJECTS qualifiers `+0x35` and weight `+0x30`; **not** covered by this early-gate component for empty/pseudo combinations |
| `24A58..24A90` | Every preceding path | Apply both independent `+0x10B/+0x10F` action-state dword vetoes and return requested hand's candidate |

### Correctness examples

- With first-party statuses `0x4000` and `0x4000`, special mode `0xFF` and actor 2, **both empty hands** keep their prior candidates, whereas statuses `0x3FFF,0x4000` clear them. A pair of statuses `0x8000,0x4000` passes the **later** gate even though it does not make the **earlier** `240A8` equality predicate true.
- Two pseudo-hand IDs (e.g. `0x7E,0x86`) force both candidate flags true when allowed **even if the actor's own status has a `0x7240` bit set**. This is possible because the original immediately jumps past that status mask.
- One pseudo-hand with a normal second item does **not** take the paired-pseudo short circuit. Its own candidate may be forced true/false after the general status check, while the other remains under normal rules.

## New bounded native model and integration

- `include/rloft/combat_hand_pair_early_gate.hpp`, `src/combat_hand_pair_early_gate.cpp`: immutable input `PartyImage`, party actor, both item IDs, original `0x258A8` byte, and the **two separately computed initial eligibility values**. Output contains both post-gate candidates and a Boolean `skip_object_arbitration` for the original short circuits.
- The existing `CombatHandPairEligibilityModel` now invokes the shared early gate instead of duplicating the actor `0x7240` veto. It also **implements the complete both-empty and both-pseudo early-return paths** through the final `+0x10B/+0x10F` action-state vetoes, without attempting the bypassed OBJECTS qualifier/weight arbitration. Both-empty requires the caller-supplied raw `OBJECTS_POINTER-0x12` category byte for each `240A8(-1)` preflight. Its ordinary positive-item qualifier/weight algorithm is unchanged.
- `tests/combat_hand_pair_early_gate_tests.cpp` covers `0x3FFF/0x4000/0x8000` boundaries, both-empty preservation/veto, both-pseudo shortcut and bypass of `0x7240`, independent pseudo overrides after `0x7240`, one-empty differences, pseudo boundaries `0x7D/0x7E/0x86/0x87`, state nonmutation and invalid inputs. The updated `combat_hand_pair_eligibility_tests.cpp` also exercises **both full short circuits** including `240A8` classification, final busy-state dwords, actual-prebase-byte requirement and the difference between the two status tests. These tests are registered in `DemoV1/CMakeLists.txt`, but **not yet compiled or executed** under private-repository no-auto-build policy.
- The offline `tools/verify_combat_entry_bytes.py` now has **87 exact original-code signature anchors** (72 prior + 15 additional directly verified `244CE..246E9` instructions, including the critical `cmp 0x4000` bytes). They verify source bytes, not unit-test execution, frame pacing, or the original DOS runtime.

**Do not let either preliminary early-gate output or the newly closed short-circuit pair results authorize a live attack:** both-empty explicitly requires an actual observed `240A8(-1)` heap-adjacent byte. **All two-nonnegative-item cases, including mixed pseudo+ordinary**, now proceed through the appropriate original `246E9` arbitration after early gate overrides. Mixed **`-1` + nonempty** pairs can now also be calculated [from explicit observed original heap bytes](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md). They remain **unavailable to runtime combat** until those values are captured rather than guessed; action/cooldown, damage, targeting and original frame/tick ordering are also unfinished.

## Remaining work and completion

- [x] Original `244CE..246E9` instruction audit and threshold correction.
- [x] Native deterministic early-gate model with original short-circuit indication.
- [x] Focused original-boundary tests added and CMake registration.
- [x] Shared early gate integrated into normal positive-item pair model.
- [ ] Full `24478` **mixed** empty/pseudo/ordinary composition with observed heap memory and original runtime traces; the two pure short circuits are now modeled through the final busy-state checks.
- [ ] Execute compiled test suite on a consolidated user-authorized build, then verify DOS-vs-native timing and gameplay actions.

**Bounded early-gate source checkpoint: 67% (4/6 gates)**. **Overall faithful-demo acceptance remains 20% (1/5 major release gates)**. No Windows build or new EXE has been generated.
