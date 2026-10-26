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

**Safety boundaries:** Class/level tables, source sprite/record and guest string bounds are checked. A genuine death with still-live party members can run source `0CD44→0AD24`. When effects must expire and their callback families are missing, the source guard stops **before damage**. A would-be whole-party wipe is refused before the original `0CBC8` game-over compositor, which is not connected. Those guards are explicit unfinished capability, not completed death/UI handling. Original type-8 projectile actor actions and non-type-15 near combat remain fail-closed and must be implemented from the corresponding actual ABIs.

**Independent source test:** `src/test_original_code2_1bdac_0d550_actor_source.cpp` calls canonical `function_1bdac` with a source actor (no host AI substitute), then exact `0D550/0CEE4/0CD44/0AD24` on synthetic party guest memory. It asserts both a saving-throw success and an authentic lethal hit/cleanup with a surviving other party member, including source timer, HP, flags and sound-request entry. This test has no original gameplay content or retail-DOS parity.

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

- [Canonical source-link #40411237378](https://github.com/Hyperbog/RLOFT/actions/runs/40411237378): **98/98 CTests passed**, including the new three compiled-source actor/script/dialogue tests and the established shared rendering, world and DOS-service tests.
- [Linux portable recomp #40411237198](https://github.com/Hyperbog/RLOFT/actions/runs/40411237198): **PASS**.
- [Retained native saved-portal #40411237304](https://github.com/Hyperbog/RLOFT/actions/runs/40411237304): **PASS** after the shared gameplay changes.
- [Windows player engineering build #40411237267](https://github.com/Hyperbog/RLOFT/actions/runs/40411237267): **PASS**.
- The original-media 555-command corpus workflow at the latest workflow source revision is a **separate gate**; its result must be checked before calling it passed. No independent original DOS game-session parity was supplied.

### Remaining subsystem dependency order

1. Expand the original `0D550` near-attack callback graph: `0D4E8` target selection, `0C9A0` hit/saving throws, original damage/status/effect paths, and `1BDAC` record cleanup. Handle type-8 `08A80` moving-record projectile and whole-party `0CBC8` game-over rendering by original source, not dummy returns.
2. Support archived `002A8` opcode-11 early/UI subtypes and fully source-backed `08A80` collision/reposition cases (no fabricated `22E90`); validate complete scripts *before* invoking the original interpreter.
3. Wire all modal `1C77C` and `1C40C` callbacks to retained original guest DOS-file, renderer, timed modal input/page and source resource operations. Keep actual blocking event behavior; no automatic dismissal of dialogues.
4. Complete original AIL sound device, DOS IRQ/input/timer and ordinary save/menu paths, then synchronized independent retail-DOS oracle comparisons.

Re-run `python Recomp/tools/audit_original_source_callbacks.py --root . --out build/original-source-callback-census.json` for a machine-readable source graph. Never promote static function counts or 32-world archive parser breadth to a finished-game percentage.
