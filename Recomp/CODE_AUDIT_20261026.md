# Recomp source audit — 2026-10-26

## Purpose and verified baseline

Scope: current `Recomp/` C++17 source, private guest/file adapters, CMake target ownership, seven compiled `*.inc` fragments, and the Linux/Windows source regression gates. Original `CODE.1`, `CODE.2`, binary assets, research notes and guest ABIs were **not** automatically rewritten or removed.

Verified audit implementation:
- [Signed DOS seeks and fail-closed overlay allocations](https://github.com/Hyperbog/RLOFT/commit/8ddf7fb33781920bf5e72ce8e95b18dc88c47511).
- [Source inventory audit](https://github.com/Hyperbog/RLOFT/commit/045fe252946723f4e9e902c15cf54ccf3c6e3560), with six synthetic negative/positive tests; the CMake input scan detected an actual orphaned palette test.
- [Restore missing palette-selection CTest](https://github.com/Hyperbog/RLOFT/commit/0ecb002a82277dcdd0c634c4ee83ada61f56fd67). This preserves valuable tests instead of deleting apparently unused sources.
- The one omitted bound in the initial new regression test was corrected in commit `255df59abc75`.

**Verification:** Linux [run #40267364210](https://github.com/Hyperbog/RLOFT/actions/runs/40267364210) and Windows source-link [run #40267364193](https://github.com/Hyperbog/RLOFT/actions/runs/40267364193) both passed **93/93 CTests**, including the restored palette test. Both passed the new source inventory; six inventory unit tests passed. The audit counts 50 registered C++ inputs and seven included source fragments. No duplicate CMake executables or CTest names and no orphan inputs/fragments remain in the audited set.

## Changes made

1. Shared the signed two's-complement 32-bit DOS file displacement algorithm in `src/original_code2_checked_dos_seek.hpp`. The private mutable `WORK/LEV` adapter may seek beyond EOF up to its bounded capacity; the source `SAVE.DAT` loader cannot seek past EOF. The distinction is unit-tested, including invalid origins, negative offsets, and no mutation on rejected seeks.
2. `src/original_code2_live_level_files.hpp` now catches map and vector allocation exceptions inside its `noexcept` guest-file ABI, sets `failed`, and returns an error instead of terminating the process. The writer checks its existing map entry rather than allocating implicitly.
3. Added `tools/audit_recomp_source_layout.py` and `test_audit_recomp_source_layout.py` to Linux and canonical Windows source-link CI. An unregistered C++ input, broken/orphan `.inc`, duplicate CMake target/CTest or fragment include cycle now fails CI. Larger source files are **warnings**, not automatic deletion targets.
4. Restored `tests/scene_palette_selection.cpp` as a compiled, media-free CTest. Never assume a file is dead code merely because an entry was missing from CMake.

## Remaining refactor risks, prioritized

| Priority | Source area | Verified risk or maintainability issue | Required next action |
|---|---|---|---|
| P1 (mitigated) | `src/original_guest_continuation.hpp` | Cooperative cancellation is explicit and synthetic tests verify shutdown, late-reply rejection, finite host waits and exception propagation. Arbitrary uncooperative original code cannot be safely preempted in-process. | Keep strict process-level watchdogs for live original CODE.2 probes; never detach a guest owning borrowed memory |
| P1 (resolved) | `src/original_code2_live_07938_restore.inc` | Prevalidated Level 23 saved-world snapshots are installed through a bounded `noexcept` transaction; synthetic checks include duplicate, oversized, active-writer and injected allocation failures. | Preserve the failure gate while implementing original `07938 → 181B4` sentinel semantics |
| P1 | `src/original_code2_live_level_transition.inc` | The genuine original `07938→181B4` dispatch reports prior level `0xFFFFFFFF`, while the transition adapter rejects it. | Implement only the authenticated no-departing-level sentinel branch and validate the unchanged original guest state; strict native frame acceptance remains **open** |
| P2 | `src/code2_startup_gate.cpp` (251 KB) | Startup, DOS services, guest state, live callbacks and test diagnostics share one large translation unit. | Extract pure input/validation and source-labeled transport types first. Split source address callbacks only when tests and ownership invariants permit |
| P2 | `src/code2_original_res_startup.cpp` (100 KB), `code2_original_world_frame.cpp` (92 KB), `original_code2_live_full_frame.inc` (85 KB) | Four source files exceed 64 KiB; difficult to review and reason about independently. | Separate by guest-owned subsystem and read-only ABI boundaries, not arbitrary line counts. Preserve original source call order |
| P2 | `.github/workflows/recomp-milestone-windows.yml` | Large source/media test workflow with many diagnostic phases, some intentionally `continue-on-error`. | Extract test phases into focused mandatory CI jobs while retaining independent real-media verification and original provenance |
| P3 | Host diagnostic controls | Magic guest offsets and internal `0xFFF*` test commands recur in callbacks and Win32 host. | Introduce named, contract-tested constants for private-only commands; never treat them as retail scan codes |

**Not in scope:** remove `Code1/`, `Code2/`, original game media, historical source-address research files, or `.inc` modules; auto-accept R01–R12 milestones; claim DOS frame/audio parity from CTest success.

## Forward rules

- Run `python Recomp/tools/audit_recomp_source_layout.py --root .` before adding a new C++ file. Add the file to CMake or a real `#include` owner. Do not silence missing dependencies by deleting tests.
- Keep source `0xNNNNN` provenance beside callback implementations.
- Keep owner originals read-only; all save snapshots stay private.
- All new `noexcept` guest callback paths must avoid unguarded allocations and filesystem exceptions.
- Every native strict acceptance uses source-specific success markers **and** failure exit codes; green aggregate CI does not prove cross-level rendering.


## Guest lifecycle and authentic Level 23 snapshot hardening — verified follow-up

Changes:
- [`966b8a88`](https://github.com/Hyperbog/RLOFT/commit/966b8a88a0884511febaff819ff60798a26bf988) implements `Worker::request_stop()`, `stop_requested()`, `finished()`, `join()`, and `await_pause_for()`. Existing `stop()` remains request-plus-join for backwards compatibility and owner lifetime safety. `resume()` now rejects a reply after shutdown is requested. A bounded timeout does not synthesize an original DOS service or free live guest RAM.
- The guest worker regression now asserts: one retained source stack across two host replies, sequence-guarded replies, cancellation of an actual paused callback, refusal of a late reply, a bounded wait while source code is running, a cooperative guest-side shutdown checkpoint, and propagation of the original guest exception.
- The private `Overlay::install_saved_snapshot()` commits a validated `LEV023.DAT` vector before invoking compiled original `07938`. A failed allocation or validation returns an explicit error and cannot escape the `noexcept` CODE.2 callback or overwrite another saved snapshot. Authentic game state is still set only by source routines.
- [`f52be0d2`](https://github.com/Hyperbog/RLOFT/commit/f52be0d2eaaf10542b2548263a88efd5fd2ffb52) adds deterministic allocation-exception injection to the **same insertion helper** used by production; it verifies the source snapshot bytes are not consumed during the simulated allocation failure.

**Acceptance evidence (exact revision `f52be0d2`):** [Linux run #40270213344](https://github.com/Hyperbog/RLOFT/actions/runs/40270213344) and [Windows source-link run #40270213355](https://github.com/Hyperbog/RLOFT/actions/runs/40270213355) passed 93/93 CTests each, with all source inventory tests. [Windows engineering package run #40270213358](https://github.com/Hyperbog/RLOFT/actions/runs/40270213358) also succeeded.

**Lifecycle scope limitation:** C++17 does not provide safe forced termination of a thread running arbitrary original CODE.2 instructions. An indefinitely running guest that never checks its shutdown flag or reaches a host callback remains a supervised-process boundary; `join()` intentionally does not detach such a thread and leave guest pointers dangling. The strict native CI already supervises and kills the *process* on a bounded timeout. No original source callback or DOS input response was invented to conceal this limit.

**Next recompilation step, separate from the audit:** original `07938` legitimately presents prior-level sentinel `C6C8=0xFFFFFFFF` to `181B4(23)`. It must be allowed only on the authenticated original saved-game restoration path, with correct absence of a departing-world save. After a verified retained-save restore marker, continue the original Level 23 → 26 type-8 portal and require native `1FC14` post-scene VGA frame acceptance. The audit has **not** accepted that gameplay gate.
