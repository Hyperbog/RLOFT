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

## New retained-host dependency closure (2026-10-27 UTC)

Code commits [`62e70ec1`](https://github.com/Hyperbog/RLOFT/commit/62e70ec10e47a4e4aeac0ce7294baef6d9017484), [`a3aba341`](https://github.com/Hyperbog/RLOFT/commit/a3aba341e1b27c38093e5f92f07b02845185d6ff), [`d7bc3a37`](https://github.com/Hyperbog/RLOFT/commit/d7bc3a37e40c9bdecb1dc39af484c2894293f100) and [`fc5e6ff2`](https://github.com/Hyperbog/RLOFT/commit/fc5e6ff234c7a9cca99a6fd9223bbfd6a0609913) create a real retained-machine manifest for **10 of the 20 original callback contracts**: source DOS heap alloc/free (`3104C/310B0`), source planar VGA destination-page strip (`2A078`), source page select (`291B0`), original RES0 artwork (`2E510`), source HUD/scanout (`1EA04/1EA78`), source `2E608` clock, original `30CA2` RNG, and source `1A1F4` true INT33 mouse button query. This does **not** complete their composed production choreography or establish sound-driver parity.

The new source-VGA test `src/test_original_code2_game_over_vga_strip.cpp` boots original CODE.2 VGA mode and verifies genuine latches copy a four-pixel `0CADC/2A078` rectangle from the source plane to the second hardware page and preserve `EFE9`. Unlike the earlier `0CADC` synthetic callback test, its destination bytes are checked in a real bounded `OriginalVgaHost`. [Windows canonical #40437545135](https://github.com/Hyperbog/RLOFT/actions/runs/40437545135) passed **103/103 registered CTests**.

**Still disconnected:** `1F1C8` world frame and `1F494` party frame in the game-over ABI, active source `28168/28C54/27F24` sound/music with AIL-device guarantees, original nonblocking `2E614` timing, `1A9F8/1AACC/1A288` cursor, and original `205B4` device teardown/exiting. The source actor now reads the actual partial manifest and reports the first missing callback **before lethal HP mutation**. Even a future non-null manifest must not automatically certify device or DOS parity. There is no playable live game-over frame yet.

## Source cursor, IRQ8 timing, and compiled shutdown (2026-10-27 UTC)

Commits [`4414823e`](https://github.com/Hyperbog/RLOFT/commit/4414823eebe62cd2fd807280edb16ef59c9768c7), [`19e87b00`](https://github.com/Hyperbog/RLOFT/commit/19e87b00a1f0247c4fa9e8f586352bba9eef8de5), [`a5ec7410`](https://github.com/Hyperbog/RLOFT/commit/a5ec74101c3e66a812e6046c384575701113be7c) and MSVC fix [`b559b643`](https://github.com/Hyperbog/RLOFT/commit/b559b643be99ec48d95763e982146fe2a974400f).

The actual **retained guest callback manifest** now includes original `1A9F8` mouse/page reset, `1AACC` cursor-backdrop restoration and `1A288` cursor redraw. Their complete `Application33/34/35Boundary` instances reuse the existing retained mouse IRQ33 state, original page stack/copy/capture callbacks and actual planar VGA host. They do not fake pointer movement.

A separate compiled-source CTest `test_original_code2_2e614_irq8_timing.cpp` executes original `2E608→2E614`, with the **compiled** `interrupt_30708` invoked by the test scheduler during each original `2F868` event pump. The test proves 18 source IRQ ticks to the first deadline plus four additional IRQ ticks to a new deadline. This **does not** certify a live timer/event loop; assigning a synthetic clock to production was explicitly avoided.

`test_original_code2_0cbc8_source_sequence.cpp` now routes the compositor exit through **compiled original `205B4`**, asserting the exact sequence of callback requests to mouse disable, two guest heap frees, stream cleanup, game-resource cleanup and exit status `0x0C`; source `205B4` correctly skips video reset for that status. These final boundary callbacks are test observers only, not a working retained operating-system shutdown.

Windows source-link [#40438921665](https://github.com/Hyperbog/RLOFT/actions/runs/40438921665) **PASS 104/104 CTests**. Canonical static census **985 source function bodies, 671 edges, 1,933 ABI mentions, 489 live callback assignments**. The whole-party live `0CBC8` guard remains active because original `1F1C8/1F494` scene/party composition, `28168/28C54/27F24` active AIL audio, live `2E614` IRQ progress, and `205B4` native shutdown are not validated end to end.

## Retained scene, party and source-audio expansion (2026-10-27 UTC)

Code commits [`12e6452d`](https://github.com/Hyperbog/RLOFT/commit/12e6452de8b5bd08eb04966b8597426f8faf82df), [`26e91839`](https://github.com/Hyperbog/RLOFT/commit/26e91839df9300db0de6b0b3988cd27c8c6cf838), [`db0c9631`](https://github.com/Hyperbog/RLOFT/commit/db0c96311f1caaf33eb91466d7e6415660cf42b3), [`0ac087d0`](https://github.com/Hyperbog/RLOFT/commit/0ac087d00c95f43e623c54aff20b6b3e67afa294), [`47283fe0`](https://github.com/Hyperbog/RLOFT/commit/47283fe0aeb6b30d877292efcc94d2fa9b9fb646), and [`977d56dd`](https://github.com/Hyperbog/RLOFT/commit/977d56ddbfa0b7844e75f2211a648588bc90e420).

The retained `0CBC8` manifest now supplies **compiled-original** `1F1C8` world-scene and `1F494` party-frame callbacks. They reuse the existing `original_full_configure_scene` and source VGA/RES equipment/portrait routes, rather than constructing bespoke game-over pixels. The world wrapper refuses invocation without its original scene trace, exterior trace, and full source polygon/actor callback graph. Source regression `test_original_code2_0cbc8_source_sequence.cpp` additionally executes genuine `1F494` for all four high-bit characters and verifies original dirty flags and death-art IDs, and directly executes `1F1C8` over synthetic guest geometry with recorded original order (page, sky/ground, polygon/actors, 14 HUD source requests, event service and coordinate writes). These *synthetic device observers* do not certify whole-frame retail parity.

Queued `28168` and direct `27F24` SFX have real source execution paths using original resource packet lookup, strict length/alias bounds, and the preflighted existing experimental WinMM waveOut bridge. The direct sample does not invent a new sound-queue entry or consume the two source priority slots. `28C54` active music **still stops** until a true driver service is connected; invoking the disabled-driver source return is not evidence of music playback. The appended `test_original_code2_28168_audio_service.cpp` case confirms direct `27F24` source copy, sample submission callback and unchanged source queue.

**Fail-closed production boundaries:** source `2E614` requires continuous authentic IRQ8/event servicing; `205B4` needs working native/guest teardown and DOS exit; active Miles AIL music is not reproduced by WinMM; the whole-party `0CBC8` entry is still blocked before mutation despite more non-null callbacks. [Canonical CI #40439986244](https://github.com/Hyperbog/RLOFT/actions/runs/40439986244) **104/104 PASS**. **500 static callback assignments**, no measured dynamic coverage; acceptance **3/12**.

## Host elapsed-time IRQ8 scheduler checkpoint (2026-10-27 UTC)

The new `src/original_code2_irq8_elapsed_scheduler.hpp` converts elapsed
`std::chrono::steady_clock` time into a bounded number of interrupt
dispatches; it does **not** mutate guest tick `F58C` itself. The caller must
supply a separately verified interrupt frequency, invoke the compiled original
`interrupt_30708` on the same owning guest thread, and configure all relevant
interrupt callback services. Unarmed, invalid-rate, clock-reversal and
excessive-backlog cases fail closed, without inventing source IRQ8 ticks.

`src/test_original_code2_irq8_elapsed_scheduler.cpp` executes original
`30708` on a bounded source guest with a **test-only** injected monotonic
timeline, verifying eight original incremented ticks, source `19C80`
notifications and saved-vector far-chain callback ordering, CPU-state
restoration, rejection of a backwards timestamp, and no partial dispatch on
an excessive backlog. The test's 500 Hz input is **not** evidence that the
retail game's IRQ8 frequency is 500 Hz.

**Not completed:** authentic device-derived IRQ8 frequency and vector
chaining in the live host, real nonblocking modal event processing through
`2F868`, uninterrupted `2E614` service, active Miles music and native
`205B4` shutdown. No new production callback was armed; whole-party
`0CBC8` remains explicitly fail-closed. The new test is a bounded
scheduler/source-handler integration check, **not** an IRQ8 hardware,
gameplay, or retail-DOS timing parity acceptance.

**Hardware nomenclature:** Original `30BB4` installs handler
`30708` on DOS interrupt vector `08h` (normally the IRQ0 timer
vector), not the PC's separate hardware IRQ8/RTC vector `70h`.
The scheduler name follows the project's historical "IRQ8" shorthand
for interrupt *vector* 8. The original timer cadence has not been
authenticated against the running DOS host.
 
## Retained INT 08h vector-chain and source wait boundary (Part 41)

New `src/original_code2_int08_vector_preflight.hpp` verifies the original
`30BB4` vector-installed flag, all three retained IVT targets, original
saved INT 08h predecessor fields `F582/F586`, and non-null timer/chain
callbacks. It refuses a missing or recursive predecessor, mismatched vector,
or incomplete source interrupt callback graph.

The retained CODE.2 guest now owns a per-session elapsed scheduler and
polls it on its **existing owning guest thread** before `2F868`. The
game-over `2E614` callback is routed to genuine compiled source through
the same event pump, but explicitly refuses invocation when the scheduler
is not armed. No timer rate, previous DOS interrupt handler or PIC EOI
has been invented; no production arming call is present.

The new `test_original_code2_int08_vector_lifecycle.cpp` executes
compiled `30BB4 → 30708 → 30C38` on a bounded guest with **test-only**
previous-vector observers. It validates installation/idempotency,
retained memory writes, original IRQ tick changes, source hook then chain
order, CPU restoration, missing-service rejection and full vector restore.
Passing this test is not a DOS IRQ0/BIOs clock-equivalence receipt.

Full-party `0CBC8` remains guarded and retains the live-device parity
stop before lethal HP mutation. The scheduler still needs authenticated
timing input, non-fabricated previous-vector execution, active Miles
music, and native `205B4` teardown before game-over is playable.
