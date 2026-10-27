# Original CODE.2 game-over compositor closure — 2026-10-27 UTC

**Branch:** `Recomp`. **Original media:** read-only on `DnD-Classics/MASTER`. These regressions target common CODE.2 source and are not specific to any level.

## Source graph and original control flow

- `0CD44` applies authentic HP/status/timer/death writes. Its tail calls original `0CCF4`, which checks all four source party status words with mask `0xD040`. Only a true whole-party defeat enters `0CBC8`.
- `0CBC8` invokes original scene `1F1C8`, page `1EA78`, modal frame `1F494`, audio `28168`, mouse reset `1A9F8`, VGA page select `291B0`, animation pump `2E608`, artwork `2E510`, indexed page `1EA04`, music `28C54`, source delay `2E614`, cursor `1AACC`, sampled audio `27F24`, cursor redraw `1A288`, held/released mouse state `1A1F4`, and exit `205B4`.
- At its transition point `0CBC8` calls compiled original `0CADC`, which requests original allocation `3104C(0xE10)`, source RNG `30CA2`, VGA strip copy `2A078`, and source free `310B0`. This uses 400 original nine-byte animation records and is **not** a pre-rendered substitute.

## Verified source regressions

`src/test_original_code2_0cbc8_source_sequence.cpp` executes compiled `function_0cbc8` with test-only observer callbacks. It verifies the artwork IDs/order, page timing, music and sample requests, 400 original source particle records, 32,000 source strip-copy callback invocations, source RNG count, release-then-press input wait, and exact exit code `0x0C`.

The same regression separately executes compiled original `0CD44→0AD24→0893C→0CCF4→0CBC8→0CADC` for a synthetic full-party wipe. The four status words, HP state and cleanup are original guest-memory writes; the compositor callbacks are instruments, **not fake operational renderer/audio/input services**.

`src/original_code2_game_over_readiness.hpp` adds a read-only 20-dependency completeness check for the exact source callback contract, guest buffer size, and resource-ID arithmetic. It **does not** prove that callback implementations contain working host devices or image data. Returning `ready` in a synthetic test never authorizes production-game execution.

Canonical Windows [#40435549362](https://github.com/Hyperbog/RLOFT/actions/runs/40435549362): **PASS, 102/102 CTests**. Linux portable [#40435549213](https://github.com/Hyperbog/RLOFT/actions/runs/40435549213): **PASS**. Windows player engineering [#40435549094](https://github.com/Hyperbog/RLOFT/actions/runs/40435549094): **PASS**. The previous preserved-media/native workflow successes were at different tested revisions and do **not** establish the latest full game-over state.

## Remaining live integration, not certified

The retained live `original_full_actor_source_damage` deliberately still refuses full-party defeat before mutation when `0CBC8` host services are unavailable. Required live closure: (1) source-aware scene/VGA page and artwork callbacks, including real `2E510` resource handling and original `0CADC` page copies; (2) original game-over music/sample requests with active AIL/device handling; (3) proper allocation/free lifecycle for the `0xE10` source work buffer; (4) nonblocking host event servicing during true source timing and modal mouse release/press waits; (5) original application shutdown.

This is **source-control-flow coverage, not retail DOS-equivalent frame/audio/input parity**. Do not replace any original device callback with a silent no-op, synthetic input sequence, host-selected game result or a fabricated rendered frame. The whole-game acceptance ledger stays **3/12**; canonical function count **985**, static direct edges **671**, live callback assignments **462**. Their ratios are not dynamic gameplay completion.
