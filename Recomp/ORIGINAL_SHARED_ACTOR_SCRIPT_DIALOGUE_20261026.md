# Shared actor/combat, event scripting and dialogue source — 2026-10-26

**Branch:** `Recomp`. **Original game media:** strictly read-only on `DnD-Classics/MASTER`. All work targets common CODE.2 source functions, not individual level implementations. The 12-gate whole-game ledger remains **3/12 accepted**; no original DOS comparison, full combat certification or interactive dialogue certification is implied.

## Completed source-backed changes

### Animated actor → saving throw → party damage and cleanup

On the retained original guest, source `1BDAC` calls `0D550` for actor actions. Previously its **type 15** action unconditionally stopped at `ORIGINAL_1BDAC_0D550_SPECIAL_ACTION_PENDING`. That branch now executes the compiled original `0D550` and supplies real callbacks:

| Callback from original `0D550` | Source implementation / ABI |
| --- | --- |
| `0CEE4` saving throw, selected party slot | Real `code2::function_0cee4`; original `1C328` dice roll using original `30CA2` source RNG and guest class/level table |
| `0CD44` party damage | Real `code2::function_0cd44`, original time `19CC4`, source HP/status/death update, signed HP behavior and guest party-state updates |
| `0AD24` dying-actor cleanup | Real `code2::function_0ad24`, and original timed-effect `0893C` sweep where no unconnected timed-effect callbacks are needed |
| `14A60` active-party selection | Real `code2::function_14a60` and original string comparison `27C70` over validated guest strings, plus original `1C1DC` quest-bit query |
| `28110` actor SFX | Original source priority/request queue `code2::function_28110`, not fabricated audio-device completion |

**Safety boundaries:** Class/level tables, source sprite/record and guest string bounds are checked. A genuine death with still-live party members can run source `0CD44→0AD24`. When effects must expire and their callback families are missing, the source guard stops **before damage**. A would-be whole-party wipe is refused before the original `0CBC8` game-over compositor, which is not connected. Those guards are explicit unfinished capability, not completed death/UI handling. Original type-8 projectile actor actions and eligible-target non-type-15 near combat remain fail-closed and must be implemented from the corresponding actual ABIs.

**Independent source test:** `src/test_original_code2_1bdac_0d550_actor_source.cpp` calls canonical `function_1bdac` with a source actor (no host AI substitute), then exact `0D550/0CEE4/0CD44/0AD24` on synthetic party guest memory. It asserts both a saving-throw success and an authentic lethal hit/cleanup with a surviving other party member, including source timer, HP, flags and sound-request entry. This test has no original gameplay content or retail-DOS parity.

### Shared near-actor target exhaustion through original `0D550/0D4E8`

Commit [`f20a9a22`](https://github.com/Hyperbog/RLOFT/commit/f20a9a223b656ab1852044c5ae4307a00980adbf) wires original retained `1BDAC→0D550→0D4E8→28110` when all four potential party slots are ineligible according to genuine reconstructed `0D4E8`. Source `30700` time, `30CC4` seed, optional `30CA2` RNG, and source priority sound requests remain original; no host-computed target or hit outcome is returned. New `original_code2_near_combat_preflight.hpp` checks guest memory, camera distance, source direction mapping and all four `0D4E8` predicates before entering a source branch that would otherwise mutate guest state.

New `test_original_code2_0d550_near_exhausted_source.cpp` exercises original four-target exhaustion and original paired selection, verifies actor state/timer, source RNG calls, sound queue and unchanged HP, and asserts that eligible targets and special type-8 projectiles remain blocked. This **does not** cover successful `0C9A0` hit checks, `0D1E0` level effects, `0E284` status cleanup, `08A80` projectiles, `0CBC8` whole-party rendering, or independent retail-DOS parity.

### Original near-combat eligible hits (new: 2026-10-27)

Code commits [`2b2446e`](https://github.com/Hyperbog/RLOFT/commit/2b2446e04d4899558b754d656a23adc1e7cc393b) and [`2798d2f`](https://github.com/Hyperbog/RLOFT/commit/2798d2f8cba9fef060ecfccb31bccdc7117ecc3b) extend the same retained `1BDAC` guest into **original `0D550→0C9A0` hit checks** and **original `0EFA0/0FE44` modifiers, `0CEE4` saves, `0CD44` nonlethal HP/time updates and `28110` sound queues**. Eligible target selection remains source-owned via `0D4E8` and `14A60/27C70`. A side-effect-free `eligible_hit` preflight in `original_code2_near_combat_preflight.hpp` predicts only the authentic reseed/d20 sequence and validates source ownership, item bounds, empty hands and absence of unconnected effects. It checks source `0EFA0` modifiers using compiled read-only originals; gameplay hits and HP changes are still performed by the originals, not by preflight.

The additional regression `test_original_code2_0d550_eligible_hit_source.cpp` calls compiled `1BDAC` then original `0D550`. It checks a natural d20=20 success with original failed-saving-throw status flags, a d20=10 hit using original `0EFA0` modifiers, original `0CD44` nonlethal HP/timer accounting, actual `28110` source audio queue, and rejection of a would-be lethal case. This is synthetic guest-memory source verification, **not** retail DOS parity. Missing original `3210F/1C21C` miss-description rendering, equipped-hand `24478`, nested `0F5F8` contents, nontrivial `0E284` effects, lethal `0AD24` in this branch and `0CBC8` game-over remain gated.

Windows canonical [#40428870100](https://github.com/Hyperbog/RLOFT/actions/runs/40428870100) passed **100/100 CTests**. Windows comprehensive milestone [#40428870076](https://github.com/Hyperbog/RLOFT/actions/runs/40428870076) also **PASS**. The full acceptance ledger remains **3/12**; static callback wiring is **457**, not dynamic gameplay coverage.

### Original combat miss, equipped modifiers and survivor death (2026-10-27 UTC)

New source commits [`d8254a45`](https://github.com/Hyperbog/RLOFT/commit/d8254a45c4651618fed9078c65e3fc8100c99702), [`e7258471`](https://github.com/Hyperbog/RLOFT/commit/e7258471d784eec94ab2911fddd3eee599c165e5), [`6dd76160`](https://github.com/Hyperbog/RLOFT/commit/6dd761608145d2076626970d62755312f0f3b5d6), [`bfd47c28`](https://github.com/Hyperbog/RLOFT/commit/bfd47c28aa5e55c7ad42fca9e90c1ded92dba7bf).

- **Equipment:** real `24478` hand-eligibility runs from both `0EFA0` source modifier callback sites after original guest item-table/record and both-hand checks. Mixed empty/equipped hands and nested containers remain guarded pending full `240A8/23E58` address-bound analysis.
- **Survivor death:** the already verified type-15 `0CD44` damage adapter was lifted into a common source callback and reused by near attacks. Original `0AD24→0893C` cleanup is now reachable on a lethal hit only after verifying other party survival, class/name pointers, no pending timed-status dependencies, and source `0CBC8` non-entry. Actual whole-party `0CBC8` artwork/page/audio/modal controls remain unconnected; do not count these as game-over completion.
- **Miss text:** original `0D550` calls reconstructed `3210F→385B0` using guest string bytes and the actor descriptor passed as its single variadic argument. A strictly validated bridge admits literal formatting and one unmodified `%s` only, and original `1C21C` stages the output and its time-based overlay flag. `%n`, width/precision, numeric/far and other `38BC1` format paths are not claimed. The source 385B0 parser/driver and 1C21C staging are compiled originals, with a bounded host ABI glue layer where the full printf conversion callback is still missing.

The comprehensive [Windows milestone #40433361604](https://github.com/Hyperbog/RLOFT/actions/runs/40433361604) also **PASS** after the source tests, including retained guest frame/input integration and original-media probes.

The new and amended CTests use **synthetic guest fixtures** to assert source party HP/status/cleanup, original hand callback counts, original miss text and overlay deadlines, sound requests and preflight refusal of unsupported game-over or conversions. **No retail DOS synchronized combat parity or complete UI-frame display is established.** [Canonical source-link #40433361448](https://github.com/Hyperbog/RLOFT/actions/runs/40433361448) passed **101/101 registered CTests**; measured live callback wirings **462** (static only); 3/12 whole-game gates remain.

### Complete compiled game-over control path; live host remains guarded

[Game-over compositor closure report](ORIGINAL_GAME_OVER_COMPOSITOR_20261027.md) verifies reconstructed `0CD44→0AD24→0893C→0CCF4→0CBC8→0CADC` on a synthetic party wipe and the entire 400-record indexed page/audio/input/exit source sequence. This includes the original 20 callback dependencies, with instrumented test-only services rather than live device substitutions. The retained game-over compositor remains fail-closed until its real scene, palette/artwork, audio, source memory allocation, modal input and shutdown callbacks are wired. Windows canonical [#40435549362](https://github.com/Hyperbog/RLOFT/actions/runs/40435549362) passes **102/102** source CTests; Linux [#40435549213](https://github.com/Hyperbog/RLOFT/actions/runs/40435549213) and Windows player [#40435549094](https://github.com/Hyperbog/RLOFT/actions/runs/40435549094) pass. Whole-game gates still **3/12**.

### Retained game-over host I/O integration (2026-10-27)

The partial **real guest/device** source callback manifest now contains `3104C` allocate, `310B0` free, `291B0` select, `2A078` actual planar VGA latch transfer, `30CA2` RNG, `2E510` source RES0 draw, `1EA04` HUD blit, `1EA78` true source page handoff, `2E608` time initialization and `1A1F4` real guest INT33 mouse-button query. The new `original_code2_video_boot.hpp` method and CTest verify transfer into the other actual VGA page, not merely observer counts. The retained actor `0CD44` calls the `original_code2_game_over_readiness.hpp` manifest guard before any fatal party-state mutation. Missing scene, party frame, active AIL/audio, timed pumping, cursor and `205B4` exit still block a true wipe. Never promote source-regression coverage to interactive game-over acceptance. [Current source CI #40437545135](https://github.com/Hyperbog/RLOFT/actions/runs/40437545135) passes **103/103**, static live callback wirings **478**; whole-game **3/12**.

### Shared `002A8` gameplay-script dispatcher

`src/test_original_code2_002a8_script_source.cpp` validates a five-command event descriptor using the same `original_code2_script_preflight.hpp` as the retained gameplay loop, then executes **compiled source `code2::function_002a8`**. It verifies original tile-word changes (opcode 4), original `1C124/1C1A0` bit updates (8/10), original `BF44` state (12), source `28110` sound dispatch (5), and all 50 script slot indices. A later unsupported opcode 7 is rejected **before** any earlier source opcode mutates the guest. Script opcode 7 modal UI and other side-effectful branches requiring additional callbacks stay blocked.

**Original retail data census:** Parsed untouched `LEVELS.DAT` from `DnD-Classics` (archive SHA-256 `50d0b1d13f81733a1ac55296d00fcba9aef002202bc00341f1062c37fd017d91`). All 32 indexed world records contain a combined **1,600 physical 26-byte script descriptors**, **394 active descriptors**, and **555 active commands**:

| Opcode | Active source commands | Opcode | Active source commands |
| ---: | ---: | ---: | ---: |
| 0 | 67 | 1 | 111 |
| 2 | 188 | 3 | 42 |
| 4 | 44 | 5 | 20 |
| 6 | 2 | 7 | 0 |
| 8 | 4 | 9 | 0 |
| 10 | 0 | 11 | 6 |
| 12 | 71 | — | — |

The six opcode-11 script commands have subtype bytes `0D, 0C, 07, 09, 0B, 02`. The current live-source preflight can only admit the original `0D/0C` early-return branches when their guest bit prerequisites are true; the remaining subtypes need a full original service/UI graph. These are content counts, **not** gameplay coverage percentages. No active opcode-7 in archive records does **not** imply conversations never happen: `1C77C` is invoked from other original gameplay/menu paths.

The `.github/workflows/recomp-saved-levels-linux.yml` world-loader matrix now checks this exact census by running the pre-existing source/data audit `DatFiles/LEVELS.DAT/analyze_event_scripts.py` on the owned original archive. This is independent of the synthetic CTest.

### Original `1C368` dialogue text file boundary

`src/test_original_code2_1c368_dialogue_text.cpp` calls the original `code2::function_1c368`: it indexes a real-style read-only DOS text file table, performs original `3131C` seeks, `312EC` local reads, `312EC` guest reads, copies the exact length-prefixed payload into original guest memory, and applies CODE.2's actual XOR-`DC` decoding. Two independent indexed records return correct decoded, NUL-terminated text, with **four original seek callback calls and six read calls**.

This verifies the **text decoding dependency** used by `1C77C`, **not** full modal dialogue. The original `1C77C` and `1C40C` presentation requires full resource/portrait, indexed-font and modal-input callbacks `13FD4`, `14118`, `1A1F4`, page/UI/audio/lifecycle and actor selector integration. The retained live `17A68→1C77C` callback remains fail-closed until these services are wired without omitting the original input wait or modal page transition.

## Verified workflows and limits

- [Canonical source-link #40426692610](https://github.com/Hyperbog/RLOFT/actions/runs/40426692610): **101/101 CTests passed**, including the new three compiled-source actor/script/dialogue tests and the established shared rendering, world and DOS-service tests.
- [Linux portable recomp #40426692479](https://github.com/Hyperbog/RLOFT/actions/runs/40426692479): **PASS**.
- [Retained native saved-portal #40426692565](https://github.com/Hyperbog/RLOFT/actions/runs/40426692565): **PASS** after the shared gameplay changes.
- [Windows player engineering build #40426692622](https://github.com/Hyperbog/RLOFT/actions/runs/40426692622): **PASS**.
- The original-media 555-command corpus workflow at the latest workflow source revision is a **separate gate**; its result must be checked before calling it passed. No independent original DOS game-session parity was supplied.

### Remaining subsystem dependency order

1. Expand the remaining eligible-target original `0D550` near-attack callback graph: `0D4E8` target selection, `0C9A0` hit/saving throws, original damage/status/effect paths, and `1BDAC` record cleanup. Handle type-8 `08A80` moving-record projectile and whole-party `0CBC8` game-over rendering by original source, not dummy returns.
2. Support archived `002A8` opcode-11 early/UI subtypes and fully source-backed `08A80` collision/reposition cases (no fabricated `22E90`); validate complete scripts *before* invoking the original interpreter.
3. Wire all modal `1C77C` and `1C40C` callbacks to retained original guest DOS-file, renderer, timed modal input/page and source resource operations. Keep actual blocking event behavior; no automatic dismissal of dialogues.
4. Complete original AIL sound device, DOS IRQ/input/timer and ordinary save/menu paths, then synchronized independent retail-DOS oracle comparisons.

Re-run `python Recomp/tools/audit_original_source_callbacks.py --root . --out build/original-source-callback-census.json` for a machine-readable source graph. Never promote static function counts or 32-world archive parser breadth to a finished-game percentage.
