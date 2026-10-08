# Original CODE.2 24478 two-hand eligibility research — bounded native predicates

**State:** original bytes audited and a full **conditional** `24478` pair-decision source model committed, including pseudo and `-1` branches when the original heap bytes are supplied. **No authentic heap snapshot, connected real combat, fresh C++ test run or DOS timing validation yet.** GitHub Actions is manual-only while the repository is private.

## Why this matters

The CODE.2 `214B8` use/attack path requests `24478(actor,hand)` before dispatching `26018`. The general input actor/hand status and action-state checks alone do not establish permission to attack. `24478` looks at **both hands, classes, item-definition fields, special effects and active-hand state**; a native attack based merely on “weapon present” would be unfaithful. Every operation is tick-sensitive because permitting, rejecting, switching hands or starting cooldowns changes the game's action cadence.

## Direct original-byte trace

Source: immutable supplied `CODE.2`, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 physical base `0x3AF24`. The following are **original object-1 logical offsets**:

1. `2447F..24497`: read actor input and two signed character item words at global `0x2193C + actor*0x248` (hand 0, slot 14) and `0x2193E + actor*0x248` (hand 1, slot 15).
2. `2449B..244CC`: invoke `240A8` **twice**, supplying actor and the corresponding item and a pointer to separate local state/flags. Its actual class/category-dependent logic must be ported before any final approve/reject result is returned.
3. `244CE..2451D`: special both-hands-empty branch: global `0x258A8`, actor >= 2, and the first two actors' status words (at `0x218F0` and `0x21B38`) influence whether both-hand candidates are cleared. This is an **original conditional rule**, not a generic “empty hand always usable” simplification.
4. `24522..246E9`: spell/pseudo-hand item ID range `0x7E..0x86` and character status mask `0x7240` affect each hand's candidate; separate class/effect helper `23EAC` is consulted during later two-item reconciliation.
5. `246E9..24A56`: reconcile the two candidates according to original item-definition word `+0x35` low two bits, class-based tests through `23EAC`, special ID tests `23EE4`, and each item's byte `+0x30` (priority/weight field). A pair can suppress either hand or both; the decision is **not** just a cooldown predicate.
6. `24A58..24A7A`: independently clear hand 0 candidate if `dword [0x219EB + actor*0x248]` is nonzero; clear hand 1 if `dword [0x219EF + actor*0x248]` is nonzero.
7. `24A7A..24A9B`: choose the remaining candidate for requested hand `0` or `1` and return the saved selection. **A successful preliminary hand classification does not bypass the final busy-state filter.**

### Bounded helper predicates now reconstructed

| Original routine | Verified contract | Native implementation |
| --- | --- | --- |
| `23EAC` | Compare the requested **signed** class code to any of actor bytes `+0x15, +0x16, +0x17`; sign-extend each before equality | `CombatHandPredicates::has_class` |
| `23EE4` | True only for exact IDs `0x2B,0x59,0x68,0x9F,0x102,0x103` | `CombatHandPredicates::is_pair_exception` |
| `23E58` | True for object IDs `0x7E..0x86`; otherwise actual OBJECTS category `9,10,11`, or exact object ID `0x30` | `CombatHandPredicates::is_attack_hand_item` |
| `240A8` | Supplies per-hand candidate and auxiliary flags to `24478`, with class, spell-range and party-state branches | [Nonnegative branches and explicit-prebase-byte `-1` classification](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md) implemented; actual allocator-adjacent memory value unresolved |
| `24478` | Combines candidates, classes, definitions, special cases, both action-state dwords and requested hand | [Conditionally complete pair-decision model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md), including empty/pseudo paths; authentic `-1` runtime heap-byte capture and timed combat integration pending |

**Important distinction:** `23EE4` (six pair-exception IDs) and `23F50` (six *different* effect-special IDs plus category D/F) are **separate original predicates**. Neither may be silently replaced by the other's list. The existing `CombatHandDispatchModel` remains only a read-only candidate classifier and still explicitly requires `24478` approval.

## Regression / evidence status

- Native `src/combat_hand_predicates.cpp` and header plus `tests/combat_hand_predicates_tests.cpp` implement and test the exact three helper decisions, including signed class `-1`, category/ID disjunction, all six hard-coded exceptions and input validation.
- `CMakeLists.txt` registers the C++ target for a later **manual** test run.
- `tools/verify_combat_entry_bytes.py` now holds **33 machine-byte anchors** (18 earlier + 15 newly verified) covering original hand, combat and target entry points.
- The **15 new original opcode signatures** were checked against the original local `CODE.2` bytes and passed. Their success verifies static machine bytes, not C++ unit-test execution.
- A fresh Windows C++ compile/CTest and native-vs-original game-time trace **have not been run** since the Actions push trigger was removed. Do not treat the historical 102/102 result as validating this new code.

## Required next implementation steps

1. The original OBJECTS allocation pointer is now traced. Recover the actual **heap-adjacent byte values at runtime** for `240A8(-1)` and the companion `24478` `+0x30/+0x35` pre-base fields; the parameterized `240A8(-1)` path is implemented and tested in source, not compiled.
2. The [original `24478` pair model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md) now covers positive, pseudo, both-empty and mixed `-1` combinations when the actual raw heap fields are supplied. **Acquire those fields in an original DOS run**, then validate branch-equivalent native execution and original timing before returning unrestricted attack authorization.
3. Integrate approval only at the original `214B8` flag-`01h` point and preserve action-state gating, `26018` dispatch and original timing before damage.
4. Then implement exact `E5F0` target/hit/damage feedback orchestration with `DB94/DACC` matching the *live projected NPC buffers*, animation and sound ordering, and original cooldown/deadline units.
5. Run one consolidated requested CI build after a meaningful implementation milestone or after moving the repo public. Runtime playtest and temporal DOS comparison remain release gates.

**Overall faithful-demo acceptance: 20% (1/5 major gates)**. The earlier *helper predicate* milestone (50% of its original four gates) is superseded by the [bounded full source model checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md), currently **67% (4/6)**: original byte and source gates are closed; authentic runtime heap values and compiled/DOS timing validation remain open.
