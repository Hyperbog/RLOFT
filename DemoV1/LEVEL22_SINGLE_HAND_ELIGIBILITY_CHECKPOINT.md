# Original CODE.2 240A8 single-hand candidate — nonnegative-item reconstruction

## Purpose and scope

Continue the original `24478` two-hand legality audit without shortcutting its `240A8` per-hand candidate initializer. The native port now reconstructs **the instruction-accounted nonnegative-object-ID paths** through `240A8`, `33FC4`, and `34060`. This is one candidate calculation, **not complete two-hand permission**, combat execution, or correct hand-use timing. Attacks remain disabled.

The original shipped `CODE.2` SHA-256 is `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; its object-1 file base is `0x3AF24`. Source evidence was obtained by disassembling logical offsets `240A8..243DD` (virtual objdump addresses `340A8..343DD`), together with the helper functions `33FC4` and `34060`. Newly checked: **21 exact original instruction-byte anchors**, all matching the supplied original. The offline CODE.2 verifier now registers **54 anchors total (33 previous + 21 additional)** without GitHub Actions.

## Original contract: `240A8(actor,item,&classification)`

1. At `240B8..240D9`, the routine calls `23E58(item)`; if false, calls the different `23F50(item)`. It writes the resulting Boolean through the out-parameter. It initializes an independent **eligibility** return value to true.
2. If classified and item is not `-1`, `240EC..241E3` performs **signed** actor-class tests via original `23EAC`, with class codes `0,1,3,4,5`. Class 4, without classes 0/5/1, requires `34060(item)`. Class 4 + class 5 without 0/1 requires `34060(item) OR 33FC4(item)`. Class 5 without classes 0/1/3 requires `33FC4(item)`. Independently, class 3 with an OBJECTS byte `+0x2F` category 10 vetoes eligibility.
3. A classified item enters `2420D..24281` party-state logic. Only when global byte `0x258A8 == 0xFF`, actor index `>=2`, and the first two party status words are **not both exactly `0x8000`**, the item additionally needs `OBJECTS[ID].byte[+0x35] & 3 == 2` **or** `23F50(item)`. Otherwise the prior candidate remains unchanged. The literal byte+35 low two bits are now exposed by `ObjectDefinition::raw_byte_35_low2`; the remaining bits are not given invented semantics.
4. If *not classified*, `2428A..243D7` tests two different item-ID ranges. `0xA7..0xC9` requires any class 4 or any class-5 lane whose **corresponding signed level word** at actor offsets `+0x20/+0x22/+0x24` is `>=10`. `0xCA..0xE4` or the **single** ID `0x12D` requires class 3 or an eligible level-10 class-5 lane. Other nonclassified positive IDs return the original initial true candidate. Negative class bytes and levels remain signed, not normalized.
5. `33FC4` and `34060` are *different helper exception sets*, both additionally recognize `23EE4`'s six pair-exception IDs. `34060` also accepts `0x31,0x5B,0x26,0xF8,0xF9`. `33FC4` accepts special `23F50` cases and `0x30,0x29,0xFE,0xFF,0x28,0xFC,0xFD,0x27,0xFA,0xFB,0x26,0xF8,0xF9`. Both accept pseudo IDs `0x7E..0x86`. They must not be replaced by one undifferentiated “usable item” classification.

### Original empty-hand boundary deliberately not guessed

The original `24478` **does** invoke `240A8` with `item=-1`. `240A8` calls `23E58` and possibly `23F50` *before* testing item `-1`. Both helper routines can perform raw OBJECTS-base-relative memory reads using the signed negative ID; the exact memory immediately preceding the original loaded OBJECTS pointer can matter. A faithful emulator has to recover that raw-pointer context and special `3420D/34248` empty-hand branch behavior. A generic array index or invented “unarmed category 10” assumption is **not** sufficient here.

Therefore the current `CombatSingleHandEligibilityModel::evaluate` explicitly rejects **negative item IDs**; it does **not** claim to reproduce the original unarmed path. This is an intentional open reconstruction dependency, not a statement that the original game disallows unarmed attacks.

## Committed source and deterministic tests

- `src/combat_single_hand_eligibility.cpp` and public header implement `240A8`'s **nonnegative-item** candidate/out-flag branch logic, including the two actual exception lists, all class gates, party-state conditions, spell ranges, and corresponding signed class-level words.
- `include/rloft/object_catalog.hpp` and `src/object_catalog.cpp` now decode only the verified OBJECTS record `+0x35 & 3` bits; no invented name/meaning is assigned to its upper bits.
- `tests/combat_single_hand_eligibility_tests.cpp` contains synthetic regression cases for class combinations and vetoes, helper-set OR, exact thresholds `0xA7,0xC9,0xCA,0xE4,0x12D`, class-5 level boundary 9/10, global `0x258A8` gating, exact `0x8000` first-party statuses, item qualifier `2`, input validation and party nonmutation.
- `DemoV1/CMakeLists.txt` registers this model and tests for a **later manual** CMake/CTest run. They are source-registered, **not compiled/tested with MSVC yet**; do not cite the earlier 102/102 Windows pass as their verification.
- `tools/verify_combat_entry_bytes.py` registers 54 exact original opcode anchors. The **21 newly added** byte sequences were checked directly against original `CODE.2` and passed; this verifies controlling bytes, not new C++ execution.

## Unfinished pair eligibility and future integration

1. Recover the original *loaded* OBJECTS base and preceding memory bytes at the relevant call sites so `23E58(-1)` and `23F50(-1)` can be reproduced without fabricated memory.
2. Complete translation of `24478`'s two-hand reconciliation, including out-flags, `+0x35` low-two-bit priority, `+0x30` item weight, class checks, spell pseudo-items, party override and the **separate** per-hand action-state busy checks.
3. Use `240A8` results only as inputs to `24478`. Do not approve actual attacks until the rest of `24478` is instruction-accounted and temporal eligibility/cooldown ordering matches the original.
4. Connect `26018` and `E5F0` when target/collision/weapon category/damage/audio/animation/timing dependencies are ready; test original vs native **time series**, not only final HP.
5. Keep GitHub Actions manual-only while private and batch later verification into one intentional build.

**Completion accounting:** Bounded **nonnegative-item `240A8` branch source checkpoint ~75%** (original-byte branch audit, native model and tests, source integration complete; compiled/runtime comparison open). Full `24478` two-hand decision remains unfinished. **Overall faithful-demo release acceptance remains 20% (1/5 gates).**
