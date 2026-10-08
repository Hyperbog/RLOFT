# Original CODE.2 24478 pair arbitration — ordinary equipped hands

## Scope and safety

**Source checkpoint only; no GitHub Actions builds.** This implements the normal two-**nonnegative**-item hand arbitration of original `24478`. It specifically excludes item `-1` and pseudo-hand object IDs `0x7E..0x86`, which use other original branches and, for `-1`, heap-adjacent reads. The model reports eligibility *candidates*, not executable attacks. CODE.2 `214B8` and `26018` action routing, original 30CA2 dice, E5F0 damage, 24478 complete approval and timing are still separate dependencies.

Original source: supplied `CODE.2`, SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`. CODE.2 object-1 logical offsets below; file base `0x3AF24`. Machine-byte signatures for matching qualifier, weight, unequal qualifier, single-classification, and two `-1` conditions were appended to the offline verifier. The verifier now contains **72 original instruction signatures** (54 previous + 11 pair-arbitration + 7 OBJECTS loader/memory allocation anchors).

## Exact ordinary 24478 candidate flow

1. At `24489/24490`, read actor's signed `item[14]` and `item[15]`. Call reconstructed `240A8` once for each hand to collect its **eligibility return** and separately written **classification Boolean**; these are distinct values.
2. For regular nonnegative IDs outside `0x7E..0x86`, original `245A2..245B9` clears **both** candidates when `actor.status & 0x7240`. Individual `240A8` class, level, special-item and party-state vetoes are already applied before pair arbitration.
3. At `246E9`, when **both** hands are classified, compare the literal OBJECTS `+0x35 & 3` qualifiers:
   - Both equal `2`: cancel both hands.
   - Equal, other qualifier: use OBJECTS byte `+0x30` weight, favor lower weight, favor first hand on a tie, *unless* the actor belongs to any signed class code `0,2,5,1` **and both IDs** pass original `23EE4` pair-exception; then preserve both preliminary candidates.
   - Unequal and either `2`: cancel both.
   - First qualifier > second and class `0/2/5/1` present: when second weight is **less** than first, preserve both candidates; otherwise veto second. Without that class group, veto first if second lighter, otherwise veto second.
   - First qualifier < second and class group present: **different** weights preserve both candidates, while equal weights veto first. Without the class group, veto first if first lighter, otherwise veto second.
   - When the class group is present and *both* IDs meet `23EE4`, preserve both for unequal qualifiers that are neither `2`.
4. When **only first** is classified, its qualifier `2` vetoes the **second** candidate. When **only second** is classified, qualifier `2` vetoes the **first**. When neither classified, leave initial candidates unchanged. These are the original `24A0A..24A56` cases.
5. Finally `24A5E/24A6F` independently vetoes each hand if its **action-state** dword at character offsets `+0x10B/+0x10F` is nonzero; `24A7A` selects the requested hand result. The item-swap readiness dwords at `+0x19E/+0x1A2` are different and must not be substituted.

These conditionals are asymmetric. A generic “highest-stat weapon wins” or “only one hand active” calculation would be wrong. Ordinary native logic and regression cases are in `combat_hand_pair_eligibility.*` and `combat_hand_pair_eligibility_tests.cpp`; test source is registered in CMake but the C++ tests have not been executed since the private-repo CI pause.

## Original empty-hand load provenance

The unresolved negative item has a more precise memory provenance:

- Original CODE.2 `268C8..2692C` loads object definitions. At `268C9`, it asks `412A0` to load resource `0x704`; `268D7` reads four bytes into count `0x258E8`. It then allocates **`count*0x41` bytes** with `4104C`, stores its returned pointer at `0x255D8` in `2690C`, and loads those definitions into the returned buffer with `412EC`. Object `0` begins at this returned pointer.
- Original memory wrapper `4104C` calls low-level allocator `47B34` with **requested size + 10**, stamps dword `0x12345678` at low-level allocation `+6`, and returns **`base+10`** at `410A2`.
- Both `23E58(-1)` and `23F50(-1)` compute `OBJECTS_POINTER + (-1 * 0x41) + 0x2F = OBJECTS_POINTER - 0x12`. With the observed wrapper, this is **8 bytes before the underlying low-level allocation begins**. It is neither a serialized OBJECTS.DAT record nor part of the 10-byte native wrapper prefix.
- Pair-arbitration accesses to fictional `item[-1].byte[0x30]` or `item[-1].word[0x35]` would fall **7 and 2 bytes before** the low-level allocation, respectively. Those are likewise not object-file fields. The bytes potentially depend on the allocator/neighboring heap state or the memory environment.

This explains why one cannot infer the empty-hand result from OBJECTS.DAT alone. The updated `CombatSingleHandEligibilityModel` can now evaluate original `240A8(-1)` when **the actual raw memory byte at OBJECTS_POINTER-0x12 is supplied**; it correctly treats original category 9/10/11/13/15 classification and the exact `0x258A8` actor/party gate. It rejects absent evidence. `CombatHandPairEligibilityModel` still rejects any `-1` and pseudo-item pair rather than guessing those additional `24478` branches.

**Important:** This does *not* prove that the original game always reads a consistent specific heap byte across boots. DOS heap allocation traces and real runtime control flow remain necessary before an authentic unarmed attack can be enabled.

## Regression, timing, integration

- Native source isolates `240A8` initial candidate calculation, `24478` ordinary pair arbitration, class-conditional exception behavior, raw object qualifier/weight and per-hand busy-state checks. It writes **no** party, world or NPC state, starts **no** combat animations, and applies **no** damage.
- New tests cover equal/unequal qualifier 2, tie-breaking and asymmetric weight comparisons, class group changes, both exception IDs, one-classified and neither-classified cases, busy state vs hand swap readiness, party status mask, negative/pseudo rejection, and party nonmutation.
- Original `240A8(-1)` tests now supply controlled raw pre-base category bytes to verify the unarmed classification/gating logic without manufacturing runtime data.
- **Temporal fidelity:** eligibility checks in the original hand-use call chain are tied to state-bearing tick/deadline fields. Actual gameplay action and cooldown/animation timing cannot be certified from a side-effect-free predicate, even if the Boolean decision matches. See `TIMING_FIDELITY_CONTRACT.md`.

## Remaining acceptance gates

- [x] Original branch ledger and source byte audit for the supported ordinary two-positive-item arbitration path.
- [x] Bounded native pair arbiter with preserved separate candidate and classification flags.
- [x] Deterministic test targets registered with boundary and conflict cases.
- [ ] Resolve the original `-1` and `0x7E..0x86` `24478` paths using runtime/heap evidence.
- [ ] Compile/run full native regression suite in one user-authorized consolidated build, then compare original DOS action/eligibility traces and timing.

**Ordinary pair arbitration subcheckpoint: 60% (3/5 gates). Overall faithful-demo acceptance: 20% (1/5 major release gates).** No GitHub Actions were launched.
