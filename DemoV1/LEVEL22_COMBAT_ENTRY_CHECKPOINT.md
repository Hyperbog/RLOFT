# Level 22 combat entry and target-selection checkpoint

## Private-repository build policy

**Windows GitHub Actions is manual-only while the repository remains private.** The workflow `.github/workflows/demo-v1-build.yml` no longer triggers on source changes or documentation commits. The last published binary and 102/102 CTest result are from earlier run #631; the new source described below **has not been compiled or run by Windows CI**. Do not describe that older executable as containing these changes. When the repository becomes public or a consolidated manual release is requested, run one explicit workflow and assess the new tests.

## Byte provenance

Verified against the supplied original DOS `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`, object-1 physical base `0x3AF24`. The checked-in standalone script `tools/verify_combat_entry_bytes.py` accepts the user's original `CODE.2` and now covers **33 exact executable-byte anchors** without GitHub Actions or a Windows build. Its original 18 anchors were previously checked, and the 15 newly added `24478/23EAC/23EE4/23E58` anchors were checked directly against the supplied archive. [The subsequent eligibility checkpoint](LEVEL22_HAND_PAIR_ELIGIBILITY_CHECKPOINT.md) provides the per-branch audit. The additions are not a C++ compile or gameplay certification.

### Critical original party-address correction

The base of the four 0x248-byte CHARS actor structures is DGROUP `0x218E0`. Earlier native code incorrectly subtracted the actor's status address `0x218F0` when calculating the hand deadline offset, introducing a 16-byte displacement. Correct offsets are:

| DOS global address | Character-relative | Meaning |
| --- | --- | --- |
| `0x21A7E + actor*0x248 + hand*4` | `+0x19E + hand*4` | item-swap hand-ready dword explicitly zeroed at `215E9` |
| `0x219EB + actor*0x248 + hand*4` | `+0x10B + hand*4` | **different** action-state dword checked for nonzero at `26037` |
| `0x21A0B + actor*0x248` | `+0x12B` | item-known/identified 32-bit bitfield |

`InventoryRuntimeState::swap_hand_slot` now resets only `+0x19E/0x1A2`. Its regression asserts that `+0x10B/0x10F` are untouched and the opposite hand's ready word remains unchanged.

### `214B8` and `26018` action preparation

- `214B8:214D9` rejects actor status bit `0x8000`; use/attack is separately rejected when `status & 0xF240` at `216D4`.
- `214B8:216E0` invokes two-hand eligibility routine `24478(actor,hand)` and only on approval calls `26018(actor,hand,-1)`. Full `24478` is complex and **not yet ported**.
- `26018:26037` checks the action-state dword at actor `+0x10B + hand*4`. If nonzero it returns before item-category dispatch.
- `26018:26045` reads signed `item[14+hand]`. When `-1`, it explicitly sets unarmed category `0x0A` at `26064`.
- `26018:26069` invokes predicate `23F50`, which identifies hard-coded item IDs `0x2E,0x2F,0x41,0x57,0x59,0x5A` and item categories `0x0D/0x0F` as special; these have separate action/effect paths.
- `26018:260B2..260D6` sends ordinary categories `9,10,11` to `E5F0` player attack orchestration. Category `0x0E` dispatches to an item-specific table; other categories continue through the remaining original action branches.

The new **side-effect-free** `CombatHandDispatchModel::inspect` reads the exact actor bytes, reports definitively blocked actor/hand action states and classifies the candidate action. Its result ALWAYS requires `24478` approval and cannot itself damage an NPC, schedule attacks, consume a spell or trigger the new UI. Synthetic regression tests cover normal weapons, unarmed hand, hardcoded special IDs, categories `0x0D/0x0E/0x0F`, status bits, and independence of the two dword families.

### Original `DB94` target priority and `DACC` candidate scanner

- `DB94` uses only the lowest bit of an input preference flags word. Bit 0 clear -> ask `DACC(group=0)` first; bit 0 set -> ask `DACC(group=1)` first. If the first scan returns anything except `-1`, accept it without scanning the other group; otherwise request the other group exactly once.
- `DACC` traverses projected 22-byte records by **descending draw rank** (record `+0x0A`) rather than their physical array order. It rejects nonzero record `+0x08`, live actor flag byte `[0x14D8B+actor_id*0x16] & 0x02`, and actors lacking flag `0x10`.
- Its horizontal group split is `record+0x00 <=0x80` for group 0 and `>0x80` for group 1. Its vertical visibility cutoff is `((u16(record+0x02) - 0x7D00) arithmetic_right_shift 3) >=0x3B`. The effective positive threshold is `0x7ED8`.
- Once eligible, the returned value is `u16(record+0x06)`, the original live actor index. If no candidate qualifies, return `-1`. A malformed rank sequence in the native decoder is rejected before attempting an out-of-bounds record read.

The new `CombatTargetPriorityModel` and `CombatVisibleTargetModel` implement the **separate ordered selection primitives**, with synthetic boundary/priority tests. The current presentation pipeline has NOT yet supplied a canonical 22-byte DACC cache/actor-flag mapping for live use. Connecting projected records to original world geometry and the attack orchestrator remains pending.

## Source deliverables

| File | Change |
| --- | --- |
| `src/inventory_runtime.cpp` | corrected `+0x19E` hand-ready dword write |
| `tests/inventory_runtime_tests.cpp` | exact readiness/action state isolation regression |
| `include/rloft/combat_hand_dispatch.hpp` / `src/combat_hand_dispatch.cpp` | nonmutating `214B8/26018/23F50` candidate preflight |
| `tests/combat_hand_dispatch_tests.cpp` | actor status, busy hands, unarmed/category/special dispatch |
| `include/rloft/combat_target_priority.hpp` / `src/combat_target_priority.cpp` | exact `DB94` two-group fallback |
| `tests/combat_target_priority_tests.cpp` | parity, first-group success, fallback, missing targets |
| `include/rloft/combat_visible_target.hpp` / `src/combat_visible_target.cpp` | ranked `DACC` projected actor filtering |
| `tests/combat_visible_target_tests.cpp` | original group split, rank, flags, projection cutoff, fallback |
| `tools/verify_combat_entry_bytes.py` | portable offline 33-opcode signature and offset verification |
| `CMakeLists.txt` | all three new test targets registered for future manual Windows CI |

## Acceptance / release boundary

- [x] Disable Demo v1 push-triggered GitHub Actions builds while private.
- [x] Audit original relevant entry-point bytes and two distinct per-hand dwords.
- [x] Correct known native hand-ready 16-byte displacement and extend tests.
- [x] Implement pure hand-preflight and visible/priority target selection; register deterministic tests.
- [ ] Execute C++ build and new CTest targets on an intentionally requested consolidated run.
- [ ] Integrate live 24478/26018/DB94/DACC/E5F0 with original RNG, damage rules, cooldown and pointer events, then perform DOS-versus-native acceptance.

**This bounded combat-entry research/source checkpoint is 67% (4/6 gates)**; both remaining gates are substantial. There is no claim that the source is compiled or that attacks are playable. The overarching five-major-gate faithful-demo acceptance remains **20%**. The prior hand-swap executable, which passed 102/102 CTests, predates the 16-byte correction and must not be treated as proof of this latest branch.
