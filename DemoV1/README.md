# Ravenloft Demo Build v1


## Latest source-backed overlay predicates and sprite clipping (29414)

[Original CODE.2 29414/2AD18/2ADE0 checkpoint](LEVEL22_CODE2_29414_PREDICATES_CHECKPOINT_20261008.md)
implements the previously injected `2AD18` and
`2ADE0` object-type predicates, including the
real `1C1DC` world-state bitset at DGROUP
`22200`, and `29414` signed viewport clipping.
The 29414 model writes the original six-DWORD
(24-byte) scratch record at `EF74+EFD4*24`,
updates source `EF69/EF6B/EF6D/EF6F` WORDs,
and chooses full `2A380`, clipped `2A6B0`
or no raster (original EAX 2/1/0).
It calls `2E758` before the resource null check.

Adapters now install the genuine predicates beneath
`2AE30` and connect `29414` beneath the existing
`29804` draw-record stack. The shared original
`A6BC` and `D680` modes are synchronized
or sampled live instead of frozen as independent
overlay mouse and probe snapshots. An integrated
CTest exercises the full original callback chain
through the real overlay resource `880D`.

**105/105 original CODE.2 opcode checks matched**,
bringing the ledger to **1,109 named checks / 1,101
distinct addresses**. The two exact committed core
source blobs passed independent strict C++17 Debug,
Release and UBSan smoke tests. All three new
assertion-enabled CTests are registered, but
full current-HEAD Windows compilation and DOS
reference frame comparisons remain open.
The main remaining pixel backend is original
`2A6B0` clipped VGA rasterization.

Formal demo acceptance remains **1 of 5 gates**;
overall engineering completion is not a measured
single percentage.



## Original overlay pointer, hit-test and draw-record milestone

[Original 1A3B8 / 2AE30 / 29804 source checkpoint](LEVEL22_CODE2_POSITION_PROBE_DRAW_CHECKPOINT_20261008.md)
adds shared-engine native source models for the three remaining
original overlay entry services. `1A3B8` preserves independent
mouse-clamping modes, source-ordered `33C90/1F820`
callbacks and all three original position WORD outputs
(`9E31/9E33/9E35`). `2AE30` scans genuine
opposite-scene-page **22-byte records**, performs original
signed inclusive hit bounds, early type/frame filtering
and 0/1/2 classifications with authentic supplied
`2AD18/2ADE0` predicates. `29804` manages the
original `EFD4` nested drawing stack and
`EF74 + EFD4*0x18` record addresses before
calling real `29414`.

The three adapters compose with existing `1A288`
without replacing earlier four-plane VGA copying,
page selection, signed backing clipping or
reference-count behavior. **96 new CODE.2 opcode
checks** passed, raising the cumulative audit to
**1,004 named / 996 unique instruction sites**.
The exact source files passed isolated strict
C++17 Debug, Release and UBSan checks. Four new
assertion-enabled CTests are registered, but a
current-HEAD full Windows build and DOS frame
comparisons have not run.

The next original-code dependencies are `29414`
record rendering, `2AD18/2ADE0` predicates
and the `33C90/1F820` pointer services, followed
by authentic game runtime memory binding.
Formal demo acceptance is **1/5 gates**;
overall engineering completion is unmeasured.



## Original 2A114/2A178 four-plane overlay backing bytes

[Original VGA backing checkpoint](LEVEL22_CODE2_2A114_2A178_VGA_BACKING_CHECKPOINT.md)
adds native source-backed **actual four-plane VGA capture and
restoration** below the previously reconstructed `292BC`
and `2932C` signed overlay-clipping services. `2A114`
reads planes 0–3 into four contiguous blocks of original
DGROUP backing bytes while selecting graphics-controller
read-map index `04h`. `2A178` restores those blocks
using sequencer map masks `1,2,4,8` and finishes
with the exact original **`OUT 03C4h,FF02h`**.
The VGA state now preserves both raw `0xFF`
and effective lower-four-bit mask `0x0F`.

The original **zero-width restore** case is reproduced:
starting at column 3 still transfers one physical byte
per plane, whereas an aligned zero-width restore
writes no pixels but still updates hardware mask state.
An injected native binder supplies both raster services
to the existing clipped overlay chain, reusing live
`F001/EFED/EFF1/EFF9` rather than hardcoded geometry.

**53 new original CODE.2 signatures** raise the
byte-verification ledger to **908 named / 900
distinct instruction sites**. The exact committed
raster source passed isolated strict C++17
Debug/Release/UBSan tests against compatible VGA/DGROUP
interfaces. The two new registered CTests and full
Windows EXE have **not** yet been built at current HEAD.
Remaining fidelity work: original `1A3B8` coordinate
selection, `2AE30` hit testing, `29804` drawing,
live game-resource binding and DOS A/B frame/IRQ checks.

Formal demo acceptance is **1/5 gates**, distinct
from unmeasured overall engineering progress.



## Original 292BC/2932C backing-image clipping reconstructed

[Original backing clip checkpoint](LEVEL22_CODE2_OVERLAY_BACKING_CLIP_CHECKPOINT.md)
adds source-verified native clipping for the original
32×16 overlay capture/restore. `292BC` and
`2932C` use signed DWORD coordinates, signed
WORD EF3F/EF41 bounds and exact wrapping arithmetic.
They differ in a significant original quirk:
capture requires **positive** final column count,
while restoration can still call `2A178` when
the column count is **zero**.

The new reusable binding connects these rules to
the earlier `1A288/1AB20` overlay lifecycle;
the underlying VGA `2A114/2A178` raster backends
remain explicit, without invented backing pixels.
Of 30 sampled original instructions, 29 add new
auditor anchors (the 2939A return was previously
checked), bringing the executable audit to
**855 named checks at 847 unique addresses**.
The exact source and binder passed isolated strict
C++17 Debug, Release and UBSan checks.
Both new CTests are registered but await a
fresh full Windows build.

Formal release acceptance remains **1/5 gates**,
and the engineering completion percentage remains
unmeasured; this is additional reusable game-engine
behavior, not a newly accepted end-to-end demo gate.



## Original VGA page-stack integration (CODE.2 2E590 / 2E5D4)

[Source and integration checkpoint](LEVEL22_CODE2_VGA_PAGE_STACK_CHECKPOINT.md):
native overlay callbacks now execute the original DOS
`F0D0` **byte-depth VGA page stack** rather than
treating `2E590/2E5D4` as stub page changes.
The stack saves `EF4D` into `2B644`, preserving
its distinction from original `EFE9`, and
recalculates `F001` through `291B0/2AB6C`
with live `F005`. Original diagnostic calls at
depth 10/0, their continuation semantics and byte
wraparound are explicitly modeled.

A new binder connects page push/pop to `1A288/1AB20`
and synchronizes `EFE9/F001/F005` with the actual
`2A078` VGA-copy model. No hardcoded Level 22
page number was used. **23/23 original instruction
checks passed**, raising the executable audit to
**826 named / 818 distinct sites**. The exact committed
core source and binding passed isolated strict
C++17 Debug, Release and UBSan checks; the two
registered checked-in CTests have not yet run in a
complete Windows build on current `demo-v1`.

The next source-backed graphics boundary is
`292BC/2932C` 32×16 VGA overlay backing-image
capture/restore (including their *different*
zero-width clipping gates) and its lower-level
`2A114/2A178` four-plane operations.
Formal demo acceptance remains **1 of 5 gates**;
the development completion percentage is unmeasured.



## Original 1A288/1AB20 overlay activation and restoration checkpoint

The [original overlay source checkpoint](LEVEL22_CODE2_OVERLAY_1A288_1AB20_CHECKPOINT.md)
closes another shared-engine behavior boundary: the original
`1A288` first-activation and nested-reference increments,
`1AB20` last-reference backing restoration, signed WORD
wrap, and conditional `2AE30/29804` mode selection.
`CombatOriginalOverlayModel` operates on the SAME live
overlay globals used by `1A8C8` and can attach its
callbacks to the existing authentic `2A078` VGA
copy and `30CD4` backing-transfer services.

**49 new x86 opcode signatures matched** owned CODE.2,
raising the audit to **803 named / 795 distinct sites**.
The source-equivalent branch model passed isolated strict
C++17 Debug, Release and UBSan tests. A CMake-registered
integration CTest also exercises the full source-order
`1A8C8 → 1A288 → VGA copy → 30CD4 → 1AB20`
callback sequence, but it has **not** been run in a
fresh complete Windows build.

Original page stack `2E590/2E5D4`, actual
`292BC/2932C` overlay backing pixels,
`1A3B8/2AE30/29804` graphic/mouse behavior
and DOS frame/IRQ comparisons remain outstanding.
Formal demo acceptance remains **1/5 gates**;
engineering completion is tracked separately.


## Latest CODE.2 VGA latch copy and overlay backing transfer

[Original 2A078 / 30CD4 checkpoint](LEVEL22_CODE2_2A078_30CD4_COPY_CHECKPOINT.md)
now implements the shared screen compositor's **actual four-plane
VGA rectangle copy**. `2A078` uses VGA graphics-controller
write mode 1 and source-read hardware latches, with the original
`0x0F` sequencer plane mask, temporary destination page selection,
and exact forward `REP MOVSB` semantics. This is **not**
a packed indexed-pixel copy or `memmove`: overlapping VGA
copy regions can overwrite later source bytes.

A distinct `30CD4` native model now reproduces the original
unsigned-overlap direction choice and `0x400`-byte
DGROUP overlay backing transfer. Both routines are connected
to the prior `1A8C8` compositor through reusable
callbacks. Its direct path can write actual VGA pixels;
its buffered path can also move captured backing bytes
when real `1A288/1AB20` collaborators are supplied.
The original `1C258` message callback can invoke this
VGA copy before reading the independent `30700` IRQ time.

Original code audit: **70/70 new instruction signatures matched**
the original `CODE.2`, increasing the total to
**754 named checks at 746 distinct code locations**.
The exact committed 2A078 and 30CD4 sources passed
independent strict C++17 Debug, optimized Release and
UndefinedBehaviorSanitizer tests against compatible local
memory interfaces. The checked-in new CTest targets are
registered but **have not received a complete Windows build**.

Next: reconstruct actual `1A288` overlay suspension and
`1AB20` overlay restoration; connect real DOS resource
buffers and verify frame/IRQ traces against DOS. The earlier
130/130 Windows CI success belongs to its older source
SHA, not to this newer code. Formal release acceptance
remains **1 of 5 gates**; engineering completion is
tracked separately without an invented percentage.


## Current original 1A8C8 compositor checkpoint — signed row/column overlap

The [1A8C8 original compositor checkpoint](LEVEL22_CODE2_COMPOSITE_1A8C8_CHECKPOINT.md)
adds a reusable native **original source-order compositor wrapper**
with direct/no-overlay, direct/outside-overlay and
buffered/overlap paths. It preserves the original signed
32-row × 16-column overlay intersection, full WORD
reference counter, temporary 9E39 active page, A6BA
busy writes, and original 1,024-byte backing-buffer
transfer through `30CD4`. An adapter connects the wrapper
to the previously reconstructed `1C258` timed-message
renderer without sampling 30700 IRQ time early.

The actual `2A078` VGA graphics-controller plane copy
and nested `1A288/1AB20` original overlay services
remain injected, **not** replaced with guessed pixels.
An additional **31/31 executable instruction checks**
passed against owned `CODE.2`; the cumulative ledger is
**684 named checks at 676 distinct code addresses**.
An isolated C++17 source-equivalent smoke regression
passed Debug, optimized Release and UBSan; the new
checked-in CMake/CTest target has **not** yet run on Windows.
Prior Windows 130/130 CTest certification belongs to its
earlier pinned source SHA only.

**Project progress:** 1/5 formal demo acceptance gates
remain completed, while implementation progress is
tracked separately and is not assigned an unsupported
overall engineering percentage. See the
[progress scorecard](DEMO_PROGRESS_SCORECARD_20261008.md).


## Progress reporting — acceptance is not development completion

**Formal release acceptance: 1 of 5 major demo gates (20%).**
This is a coarse count of **accepted milestones**, not a
measure of implementation work, engineering hours, original
instruction coverage, or the percentage of the full game
completed. Substantial cross-cutting source reconstruction
and subsystem tests have been added since the first gate
was accepted, without yet earning another end-to-end gate.
Do **not** read historical “20%” checkpoint labels in
this README as “only 20% of development is complete.”

The [current development-versus-acceptance scorecard](DEMO_PROGRESS_SCORECARD_20261008.md)
lists the completed/partial subsystems and actual blockers.
The overall development percentage remains **unmeasured**
until an effort-weighted scope audit. Windows build #634
passed 130/130 CTests at an earlier pinned commit; newer
source requires a fresh consolidated test run, and original
DOS runtime/frame/timing equivalence remains to be proved.


## Latest original VGA sprite machine-state and asset-reader bridge

The [sprite machine-state checkpoint](ORIGINAL_SPRITE_MACHINE_BINDING_CHECKPOINT_20261008.md)
extends the completed original `2A380/2A584` pixel-copy
models to their missing DOS-observable side effects: exact
32-bit shader-pointer writes to both original executable
operands, the persistent VGA sequencer plane-mask register
(index `02h` at `03C4h`), and transparent-row global
`F009`. The shaded patch occurs before sprite-memory
reader callbacks, matching original `2A590/2A595` order.

`CombatOriginalSpriteRasterBinder` now links the existing
original normal/shaded sprite dispatcher to **actual planar
VGA writes** using injected **original DOS resource/shader
pointer readers**, with `2E758/F0C8` callback refresh
after rasterization. This binding is shared engine code:
no Level 22-only geometry, invented sprite bytes or
arithmetic tint rules were inserted.

Five newly added original-byte signatures matched CODE.2.
The audit now holds **653 named checks at 645 distinct
instruction sites**. Focused strict C++17 Debug/Release/
UBSan tests passed for matching source blobs, machine-state
patches against the original owned CODE.2, pixel writes,
callback sequencing and resource capture boundaries.
**No new full Windows build is claimed.** The last
green 130/130 Windows CTest belongs to the prior
pinned build source. DOS reference-screen compositor,
actual memory-resource provider and real-game rendering
acceptance remain open; faithful-demo progress is
**20% (1/5 major gates)**.


## Latest original sprite raster checkpoint — real four-plane VGA writes

The [2A380/2A584 raster source checkpoint](LEVEL22_CODE2_SPRITE_RASTER_CHECKPOINT.md)
extends the original `291B0 -> 2939C/293EC -> 2A380/2A584`
path beyond callback planning: the new native source **writes actual
opaque and transparent sprite bytes into `VgaPlanarMemory`** using
the original frame header, width/height, four-plane write mask,
`EFF9` row pitch, transparent sentinel and DWORD frame addressing.

A key disassembly correction: shaded `2A584` performs a lookup
through an **original 256-entry DOS byte table**, whose 32-bit
pointer is self-patched into two CODE.2 instructions. It does not
perform a generic tint or arithmetic subtraction. The model uses
only caller-captured table bytes, preserves sentinel checks before
lookup, and reports the code-patch pointer for future live-state
integration. Malformed inputs fail closed rather than inventing pixels.

The original-byte audit now includes **648 named instruction checks
at 640 distinct logical code sites**. All **71 new** normal/shaded
raster checks matched the supplied original `CODE.2`.
The **exact committed implementation source** passed isolated strict
C++17 Debug, Release and UBSan harness tests with a compatible
VGA memory interface, including all 320 starting columns. These
commits have **NOT** received a new full Windows CMake/CTest build.
The previous green 130/130 Windows run #634 applies to its
earlier pinned source SHA only.

The current blocker is **live DOS sprite/resource/shader memory
binding, original VGA sequencer and self-modifying-code state,
`1A8C8` composition, and pixel/timing comparison against DOS**.
Until then this is real source-backed component rasterization,
not a certified end-to-end playable HUD. Overall fidelity-demo
acceptance remains **20% (1/5 major gates)**.


## Current DOS message renderer frontier — exact glyph and sprite callbacks

The [303C8/2939C/293EC original renderer checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
adds two more source-backed native execution layers after the
verified **130/130 Windows CTests at earlier pinned SHA**:

- `3060C -> 303C8` now uses the original `2F29C`
  font selection, signed DOS character indexes, per-glyph
  `F56C` normal/shaded choice, re-read font pointer `2B67C`,
  unsigned width byte and wrapping `2B680` column advancement.
  A `bind_display_3060c` adapter connects this sequence
  to the existing original IRQ-timed `1C258` callback driver.
- `2939C/293EC` now dispatch to the original normal/shaded
  sprite-raster callbacks with null-resource early returns
  and observe the optional `2E758/F0C8` refresh **after**
  rasterization, including callback-mutated F0C8 changes.

The immutable-code ledger contains **557 named x86 byte
checks at 549 distinct offsets**, a direct Strength data
reference, nine SHA256-checked DGROUP spans and one hashed
CS span. All **48 new original instruction sites matched**
the supplied `CODE.2`. Focused local source-equivalent
strict C++17 Debug, Release and UBSan tests passed.
These new CMake tests are **NOT** included in the older
green 130/130 Windows runner result.

Actual `2A380/2A584` planar sprite pixels, `2E758`
refresh effects and `1A8C8` composition remain to be
implemented and compared with DOS reference frames.
No Windows font API or approximate indexed-font drawing
has been silently substituted. Overall faithful-gameplay
acceptance remains **20% (1/5 gates)**.


## Latest verified Windows build — 130/130 CTests passed

The actual [Windows x64 GitHub Actions run #634](https://github.com/Hyperbog/RLOFT/actions/runs/37788168281)
**passed compilation, all 130 CTests, Windows EXE artifact
upload, and committed the refreshed EXE**.
The executable SHA-256 is
`09a3ce4cb5e5d804eee4647413915be7c871789f22cf26e72f13e3f71ff59b35`.
Initial run #632 revealed three outdated regression-test
compilation errors; #633 compiled but identified one
test fixture incompatible with the original `0x7240`
actor mask. Both were corrected without altering DOS
gameplay rules. Full chronology:
[2026-10-08 build and renderer checkpoint](WINDOWS_BUILD_AND_IRQ_RENDERER_CHECKPOINT_20261008.md).

**Source after this verified Windows run:** added an
independently tested original `1C258/1C2C4` renderer
callback execution bridge and `291B0` VGA-page calculation.
The bridge executes the original collaborators in temporal
order with a post-render IRQ observation, but does **not**
implement their underlying image/text/GDI/VGA pixel effects.
It adds 22 verified executable opcode anchors, bringing
the audit to **509 named checks / 501 distinct addresses**.
The newly added CTest has strict isolated C++17
Debug/Release/UBSan validation, **not** a second complete
Windows build. The green 130/130 result belongs to
the precise earlier source SHA `40e3f14b1072a7a0180ea1b0b4c1cea437137631`.
Faithful-demo DOS gameplay acceptance remains **20% (1/5)**.
GitHub Actions is restored to manual-only operation.


## Latest source checkpoint — DDA4 level-up text and IRQ display lifecycle

The [3210F / 1C258 / 1C2C4 checkpoint](LEVEL22_DDA4_MESSAGE_FORMAT_DISPLAY_CHECKPOINT.md)
closes the **original level-up message source**: CODE.2 DGROUP
+67E4 points to the verified format `%s gains a level.` at +548.
The original 3210F wrapper writes its final NUL in buffer
+759C and 1C21C copies it to the timed queue +14580.
A new read-only C++ model preserves the actor name's DOS
bytes, destination-buffer tail and unsupported out-of-bounds
source cases.

The original `1C258` display and `1C2C4` expiration sequence
now has a separately tested **ordered event model**. It records
the original render-service calls, the pending-flag clear
**after** drawing, and the later original IRQ observation.
An expired message may be displayed and erased in the same
call. No renderer, real-time clock or global state mutation
is substituted for the verified original calls.

Original executable audit: **487 named x86 byte checks at
479 distinct logical instruction addresses**, one direct
Strength table, nine hashed DGROUP data spans and one hashed
CS data span. The new 30 opcodes and two original format/data
hashes passed. The exact committed model sources and headers
passed focused strict Debug, optimized Release and UBSan
compilation/execution using original CODE.2. **No complete
repository CMake/CTest or Windows EXE build was executed**.
The five-main-gate faithful demo acceptance remains **20%**.


## Current original DDA4 callback checkpoint — spell state and IRQ queue

The [3DE4/3EA4/1C21C checkpoint](LEVEL22_DDA4_SPELL_IRQ_CHECKPOINT.md)
adds source-backed, **read-only** C++ projections for the two
different original level-dependent spell-state byte sweeps and
the independently timed `1C21C` message enqueue. All three
original spell/count tables are read from an owned CODE.2 and
fingerprint-checked; original copy/expiry behavior uses a
32-bit IRQ-tick observation and the exact +0x5A deadline,
without a substituted host clock.

The original-code ledger now contains **457 named opcode
checks, 449 distinct instruction addresses**, one directly
verified Strength table, seven hashed DGROUP tables and one
hashed CS jump table. The 50 new source-byte signatures and
three original table hashes passed. A separate strict C++17
Debug/Release/UBSan source-equivalent differential verifier
passed 960 original-table-driven actor-state cases and IRQ
deadline checks; the **full checked-in CMake test suite and
Windows application have NOT been rebuilt**.

These models do not yet execute the original live DDA4
level-up sequence, which additionally requires actual
3210F formatted output and 1C258..1C326 message
presentation/expiry behavior. Faithful-demo release
acceptance remains **20% (1/5 major gates)**.


## New DC7C level-up HP reconstruction — original CODE.2

The [DC7C HP checkpoint](LEVEL22_DC7C_HP_PROGRESSION_CHECKPOINT.md)
now includes a read-only source reconstruction of original
single-die class HP rolls, the two 19-byte signed Constitution
adjustment banks, all six CS jump-table branches, original
level-10/11 caps, the **clamp-before-multiclass** rule, one
30CA2 RNG draw per call and 16-bit current/max HP wrapping.
The tables are loaded from a user-owned `CODE.2` image
and fingerprint-checked, not embedded or redistributed.

The original-byte audit has grown to **407 named checks,
399 distinct logical instruction addresses**, plus one direct
Strength table, four hashed DGROUP tables and one hashed CS
jump table. The exact committed read-only model and regression
passed isolated strict C++17 Debug and Release tests against
the original CODE.2 data; the original-backed test caught
and corrected a mistaken fixture value for Constitution 15
before final validation.

**No live XP/HP changes, no original-timing emulation, and no
rebuilt Windows EXE are claimed.** The `3EA4`/`3DE4`
class services and `1C21C` IRQ message side effects must be
preserved before the full DDA4 sequence can mutate state.
Overall faithful-demo acceptance remains **20% (1/5)**.


## Latest DDA4 callback control — original 50DC and timed service sequence

The [50DC / DDA4 service-order checkpoint](LEVEL22_DDA4_50DC_SERVICE_ORDER_CHECKPOINT.md)
adds independently audited, read-only models of the original
class-selection predicate and its call sequence after a class
level changes. The original 50DC selector uses the status
mask `0xF240`, prefers class code `3` in zero mode,
has a class-`1` level-9 exception, and searches code `4`
in nonzero mode. A nonnegative result optionally calls
`3EA4` or `3DE4`. **Even with a negative result**, the
original still calls presentation `3210F`, timed message
`1C21C`, and RNG-consuming HP service `DC7C` in order.

The original executable audit now has **378 named byte
checks covering 370 distinct instruction offsets**, plus one
direct and two hashed original data references. The exact committed focused C++ sources/tests
passed isolated strict C++17 Debug and Release harnesses;
registered full CMake/Windows tests remain **unexecuted**.
This does **not** apply live levels, HP, XP, combat statistics
or IRQ timers. Full original game-behavior acceptance
remains **20% (1/5 major demo gates)**.


## Latest preservation checkpoint — original class levels and attack target

The [DDA4/DBD4 source checkpoint](LEVEL22_DDA4_THRESHOLDS_DBD4_CHECKPOINT.md)
adds bounded C++ models for six original class tables (20 XP thresholds each)
and the original class-specific DBD4 attack-target coefficients.
All source tables are loaded from the user-owned CODE.2 executable,
fingerprint checked and never embedded/distributed as asset dumps.
The CODE.2 verifier now has **347 original instruction signatures,
one direct Strength data anchor and two hashed original data tables**.
An isolated strict C++17 core test passed against original CODE.2 in
Debug and Release; the full Windows game has **not** been rebuilt.

Importantly, original level changes invoke 50DC/3EA4/3DE4/3210F/1C21C/DC7C
before DBD4 recalculates the attack target. These side effects are
**not** currently simulated and the new models are read-only.
Native XP, level-up and party-stat mutation remain gated on full
original instruction-order validation. Overall release acceptance
remains **20% (1/5 major gates)**.


## DDA4 original experience distribution — next source checkpoint

The [DDA4 experience checkpoint](LEVEL22_DDA4_EXPERIENCE_CHECKPOINT.md)
reconstructs eligible-party count (original status mask 0xF240),
rounded per-member experience, three class-track count/rounding,
minimum-one award rules and 32-bit class XP wrap. It returns a
**read-only projection**; level-up threshold scans, class-change
callbacks, and recalculated attack targets are NOT implemented.
The original-byte verifier now contains **318 code signatures plus one
Strength table**. A source-equivalent isolated C++17 test passed,
but registered CMake/Windows tests and end-to-end demo acceptance
have not been rerun. **Overall demo: 20% (1/5 gates).**


## New original-file result: Level 22 cannot invoke E0F8 for the Assassin

**The E284 `0x258AA` tracker is resolved for the supplied original Level 22
archive and mutable save.** `LEVELS.DAT` level 22 and `LEV022.DAT`
both store signed creature-type filters **(-1,-1,-1)** at fixed level-header
offsets `0x292/0x294/0x296`. Original CODE.2's `E50F..E539` scan
sign-extends each filter and compares against the zero-extended NPC type,
so Assassin type `9` cannot call `E0F8`. It reaches the original
corpse-marker branch **without E0F8 RNG draws or timer reseeding**.

The [updated E0F8 checkpoint](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md)
records original binary/hash verification and this important refinement
to the earlier research. `LevelState::respawn_npc_types()` now exposes
the actual saved header values; the Level22 encounter and its live
`ExplorationRuntime` caller pass the real level state, reject any
unsupported matching tracker before modifying the world, and use the
original no-match corpse-marker model. The Level22 C++ regression
fixtures now cover signed header parsing and matched-tracker rejection.
This is committed **source only**; no consolidated native/Windows tests
or updated EXE are claimed. Complete combat/presentation/timing
acceptance remains open and the overall five-gate demo stays at **20%**.


## Original E0F8 respawn source-model checkpoint

The [E0F8 source checkpoint](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md)
adds a **read-only** reconstruction of the original IRQ-tick/290D0 BIOS
fallback RNG reseed, two 30CA2 draws per candidate, low-byte map
obstruction, two separate 15-cell party-distance tests, live NPC cell
occupancy, three-attempt limit and exact 22-byte slot/HP mutation plan.
Its local isolated C++17 tests passed with Release assertions enabled.
The CMake test target is registered but the current branch has **not**
received a consolidated Windows build. On the original 30x15 Level 22
map the routine cannot find a candidate at least 15 y-cells from an
in-bounds party, but a *real invocation still consumes six RNG draws*;
the actual 258AA tracker and original IRQ observations remain unknown.
The offline verifier now has **293 CODE.2 instruction signatures plus one
Strength data table**. This **does not** connect E0F8 to live combat,
change shipped behavior or advance overall demo acceptance (20%, 1/5).


## Project-wide timing fidelity — mandatory

**Walking pace, turn response, animation frame dwell, card flips, combat
recovery, NPC reactions and story transitions must match the original game's
timekeeping**, not merely its final graphics or state. See the
[Timing Fidelity Contract](TIMING_FIDELITY_CONTRACT.md), grounded in
[Preservation Archive Chapter 17](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md).

The original main loop has an anchored two-game-tick target
(`201A7..2022B`, `2E608/2E614`). This must be reconciled with the native
host's nominal 70.0863-Hz **display** refresh; the two are not interchangeable.
No universal milliseconds-per-tick claim is made without measured original
runtime evidence. Every new gameplay subsystem requires explicit timing,
interruptibility, audio/animation ordering and regression acceptance.
Overall five-major-gate demo completion remains **20%**; cadence equality is
not yet certified.

## Private-repository development and build policy

**Automatic GitHub Actions Windows builds are disabled on `demo-v1`.**
`.github/workflows/demo-v1-build.yml` has **manual `workflow_dispatch`
only**, to conserve private-repository Actions minutes. Continue committing
faithful gameplay reconstruction, regression tests and archival research
without repeatedly building preview binaries. Re-enable push builds only
after explicitly deciding to do so when the repository becomes public.

The **last previously tested Windows executable** is from
[build #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
(102/102 CTests), but it **predates** the recently verified correction
of the hand readiness offset and the new combat-entry/target-selection
source. Do not treat its tests as validation of current HEAD.

The current source-only work includes
[original attack/target preflight and offline verification](LEVEL22_COMBAT_ENTRY_CHECKPOINT.md).
An original-asset offline byte audit now covers **273 original CODE.2
machine-code anchors plus one signed Strength data table**. Seventeen combat C++ test targets are registered,
but **have not been executed** since the private-repository build pause.
The [conditional 24478 hand-eligibility model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
preserves the original requirement for observed DOS heap-adjacent
bytes; it is not yet connected to live attacks.
This deliberate pause does not alter the **overall faithful-demo
acceptance: 20% (1/5 release gates)**.


## Playable Level 22 walkaround preview (opt-in)

**Windows x64:** use the newly added `Explore-Level22.cmd` packaged next
to `ravenloft_demo_v1.exe` in the latest Windows Actions artifact. It
loads the actual Level 22 without requiring the intro or character-creation
sequence. If CHARS is present it is used read-only; otherwise the preview
uses temporary in-memory EXPLORER/GUIDE characters. Your installed game files
are not saved or modified by this mode.

Use arrow keys or the numeric keypad to move and turn; press Esc to exit.
Alternatively run `ravenloft_demo_v1.exe --explore-level22` (optionally
followed by a quoted installation path). The normal no-flag boot flow is
unchanged. The original world/environment is loaded, but incomplete
main-wall rasterization can leave some foliage/walls/scene pixels absent.
The preview is not a full gameplay demo.

[Walkaround instructions, implementation and acceptance checklist](EXPLORATION_WALKAROUND_CHECKPOINT.md).
**Initial walkaround checkpoint: 100% (5/5 bounded gates)**:
[Windows build #602](https://github.com/Hyperbog/RLOFT/actions/runs/37716513471)
passed 101/101 CTests and published the direct-entry EXE with a launcher.
User-provided Windows screenshots subsequently confirmed original-asset outdoor
scenery, HUD, expanded top menu and automap. These captures also show an
unresolved black-background character panel and questionable automap/modal
background isolation. Locomotion fidelity, wall rasterization, complete
gameplay and original DOS indexed-frame equality remain unverified.
Overall faithful-demo acceptance is still **20% (1/5 major release gates)**.


Goal: reproduce the original shipped behavior from boot through character creation while reading assets from a legally installed/owned copy of *Ravenloft: Strahd's Possession*.

## E284 NPC death and event follow-on — type-specific source checkpoint

The [E284 death-follow-on checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md)
now reconstructs the **positive-HP versus corpse paths**, the
distinct original presentation-resource expressions for injury
and death, and the ordered special-death branches for NPC types
`0x08/0x09/0x12/0x18/0x21/0x50`. Level 22's Assassin
type `0x09` now uses the audited `E284` four-drop ID and
placement-seed plan (`88/6E,87/78,E9/82,74/8C`), removing
a second hardcoded death-spawn loop.

The original `E0F8` respawn routine is now **instruction-audited**:
it samples the independent `30700` IRQ tick, draws two `30CA2`
values for each of up to three placement attempts, checks map
collision/proximity, and can roll new NPC HP on success.
This source evidence **does not imply native E0F8 integration**.

The `0x258AA` three-entry signed type tracker and `E0F8`
respawn callback are modeled **only with observed caller inputs**;
type `0x18` still requires external condition/RNG evidence.
Original global story `BF44`, presentation/audio `38110`,
and `1DDA4` award consequences are not yet fully connected.
**32 original E284 death-branch opcodes** and **15 subsequent E0F8
respawn-body opcodes** have been verified, bringing the cumulative
offline verifier to **273 instructions plus one data table**.
The tests are source-registered and not yet compiled in Windows.

## Recovered Strength table and HARM/E284 event ordering

The [Strength, HARM and E284 classification checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md)
recovers the **exact signed DGROUP:684B modifiers for scores 0–25**
from immutable CODE.2 object-2 data. The original modifier for
Strength 18 without a qualifying exceptional-strength class is
**+2**, and the E5F0 percentile branch remains separate.
Unsupported scores 26–255 require actual adjacent DGROUP
bytes because the executable performs no index bounds check.

The new `CombatHarmPreludeModel` preserves the original
**1d4 → conditional direct HP clamp → provisional zero damage
→ C910 minimum-one injury → E284 second HP write** sequence.
It does not roll dice or mutate an NPC merely to inspect it.

The shared `E284` injury prelude now also applies the original
`3ACDC` NPC type classification to BYTE `npc+0x0B`, *before*
the HP-positive/death branch. This applies to the Level 22
Assassin on both nonlethal and lethal damage and preserves the
existing four-quest-item drop handling. The source-only offline
audit is now **226 x86 instruction anchors and one 26-byte
DGROUP data anchor**. No new Actions build has been run.

## Original 22E40, Rod and E284 injury reconstruction

The [class-level, Rod and E284 injury checkpoint](LEVEL22_CLASS_LEVEL_ROD_INJURY_CHECKPOINT.md)
closes three additional original combat source dependencies:

- **Vampiric Touch `0x7F`:** `22E40` returns the signed
  level of the *first matching class-4 lane* (`+0x15..+0x17`,
  levels `+0x20..+0x24`), defaulting to 1 when absent.
  The damage planner now derives its d6 count directly from
  the original `PartyImage`.
- **Rod of Smiting `0x4A`:** `E5F0` compares the Boolean
  return of `C80C` to 20, so its apparent charge-consuming
  critical path is **unreachable through that normal caller**.
  NPC types `0x17/0x1F` reach an extra `2d8+6 per die`
  roll; other types retain initial damage.
- **NPC injury `E284`:** preserves original reaction flags,
  type-`0x0F` subtraction exemption, type-`0x08` HP floor,
  and signed/word-wrap arithmetic. The existing Level 22
  Assassin type-`0x09` damage path now shares this original
  prelude before its established XP, death sound, and four drops.

**39 additional immutable CODE.2 opcode signatures** were checked
locally, bringing the audit to **212**. The source and CMake targets are committed but have not received
a new Windows build. The normal-score **DGROUP:684B Strength
table has since been recovered**; full E284 creature death effects,
actual RNG/tick traces and playable attack input remain open.

## E5F0 damage dice and C910 weapon restrictions — source checkpoint

The [damage-dice and creature-gate checkpoint](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md)
now reconstructs the ordinary original `OBJECTS.DAT` dice parameters
(`+0x38` count, `+0x39` sides, `+0x3A` bonus per die), the **actual
nine-entry `E5CC` temporary spell-hand jump table**, exceptional-strength
modifier thresholds, and the `C910` creature/weapon damage gate.

The model handles HARM's special pre-gate NPC-HP-clamp decision
without applying it prematurely, and distinguishes Rod of Smiting's
separate `0x4A` charge/type branch. Character modifiers that use
the **original DGROUP `0x684B` Strength modifiers for scores 0–25**
have since been recovered byte-for-byte from CODE.2 object 2;
Vampiric Touch's `22E40` class-level helper is also reconstructed.

The new models **do not roll dice, change NPC health, change hand
recovery timers, or launch an animation**. Three new C++ test targets
are registered for later intentional verification; the original
that earlier source stage added 47 verified signatures (then 173);
the current audit has **226 instruction anchors plus one data-table anchor**.
Overall faithful-demo acceptance remains **20% (1/5 major gates)**.

## Pre-attack status service E578 — original actor effects

The original CODE.2 logical `E578` executes **after initial target
selection but before target validation and the hit roll**. It clears
a pending `+0x132` effect bit for all four actors, then clears
selected-actor `+0x133` bit02h and its `+0x15A` state, marking
the original `CF5A` HUD redraw flags. The source model
`CombatAttackStatusService` and synthetic regression are committed,
but are not yet wired into live attacks or compiled under deferred CI.
See the [E5F0 timing and E578 checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md).

## Original E5F0 target selection and attack timing — source checkpoint

[Attack target and timing checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md)
now reconstructs the original explicit-target-vs-`DB94` preference
order, special rear-party `+0x35 & 3 == 2` restriction and action
recovery based on the **full original OBJECTS WORD +0x35 bits 2..5**.
The result is **9 × ((word >> 2) & 15)** gameplay maintenance ticks
for a regular item, or 18 for an empty/pseudo hand, optionally halved
by the original actor effect mask. On a successful hit the original
`E5F0` stores **A7E4 + half the interval** as the *hit-only*
hand-ready deadline and records the resulting damage. Its next call,
original CODE.2 logical **`24B48`**, presents the hand action and,
when the interval is nonzero, stores a **second, miss-inclusive**
action-busy deadline using the **later A7E4 reading + full interval**.
The two dwords and clock observations cannot be merged.

The new native routines are **side-effect-free**, consume the
**captured dispatched item ID** as original E5F0 does, and do not
change the character state, choose invented damage or run animations.
Timing may not be equated with VGA refresh or host milliseconds.
Both new C++ regression targets await a deliberate consolidated build;
21 additional original executable signatures passed local byte audit.

**Bounded E5F0 target/timing source checkpoint: 67% (4/6)**.
Overall faithful-demo acceptance remains **20% (1/5 release gates)**.

## Full 24478 pair decision — conditionally reconstructed

The [raw-heap-conditioned original hand decision model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
now accounts for ordinary, pseudo, both-empty and mixed negative-item
`24478` paths **as a side-effect-free candidate calculation**.
A negative item reads raw bytes before the original allocator base;
the model demands the actual observed original category alone for
both-empty, or category/weight/qualifier for a mixed pair.
This does not make unarmed attacks playable: raw DOS heap values,
attack/use event ordering, hit/damage, audio and recovery timing are
unverified.

**Bounded original pair-decision source checkpoint: 67% (4/6 gates)**.
Overall faithful-demo release acceptance remains **20%**.

## Original 24478 empty/pseudo short circuits

The [empty/pseudo early-gate checkpoint](LEVEL22_HAND_PAIR_EARLY_GATES_CHECKPOINT.md)
models original `244CE..246E9` and distinguishes its first-actor
`status >= 0x4000` threshold from `240A8`'s **separate**
`status == 0x8000` branch. The `24478` pair evaluator now
supports **both hands empty**, but only with the original observed
OBJECTS pre-base category byte, or **both hands holding pseudo
spell items `0x7E..0x86`**; both shortcut directly to their distinct
action-state busy checks. Mixed **positive pseudo+ordinary** pairs follow the normal
qualifier/weight arbiter after pseudo overrides. A mixed `-1`
with any nonempty hand can also be **modeled** when its actual
original allocator-adjacent category, weight and qualifier bytes
are supplied explicitly; absent evidence is rejected.
[Original heap-conditional pair checkpoint](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md)
documents the exact three byte offsets. The authentic DOS heap values
and temporal combat execution remain outstanding. No attack is enabled and current source
has not been compiled since private CI was paused.

## Original 24478 ordinary two-hand arbitration

The [ordinary two-hand arbitration checkpoint](LEVEL22_HAND_PAIR_ARBITRATION_CHECKPOINT.md)
originally modeled CODE.2 `24478` for **two ordinary positive equipped items**; the later [conditional full pair model](LEVEL22_HAND_PAIR_RAW_HEAP_MODEL_CHECKPOINT.md) now extends that source to the original pseudo and `-1` branches,
using the original independent `240A8` eligibility/classification results,
literal OBJECTS `+0x35 & 3` and `+0x30` weight fields, character class
groups, `23EE4` pair exemptions, and each hand's `+0x10B/+0x10F`
action-state busy gate. The source/tests are registered for an eventual
manual build; **there is no newly verified Windows binary or playable attack**.

The original loader at `268C8` allocates OBJECTS through a wrapper
returning a pointer 10 bytes into an allocation. `240A8(-1)` reads the
pre-table byte `OBJECTS_POINTER-0x12`, *outside* the allocated object
record and even before the low-level allocation base. The helper can
model the empty-hand decision only when that actual raw byte is supplied.
The full pair model now handles `-1` and pseudo items **conditional
on all raw original bytes actually read by the selected path**.
Original heap behavior and game-time action pacing remain separate
research gates.

**Bounded ordinary pair checkpoint: 60% (3/5 gates)**. Overall faithful
demo release acceptance remains **20% (1/5 gates)**.

## Original 240A8 candidate and 24478 hand legality — source-only progress

The latest [single-hand eligibility checkpoint](LEVEL22_SINGLE_HAND_ELIGIBILITY_CHECKPOINT.md)
implements original `240A8` for **nonnegative object IDs** and permits the original
empty-hand (`-1`) branch **only with the true OBJECTS pre-base category byte**,
including its
`33FC4/34060` exception sets, signed class and level gates, two original
spell-ID intervals, party-mode rules and the literal OBJECTS `+0x35 & 3`
qualifier. Its regression target is registered but not yet compiled.

The source deliberately **does not invent a default `240A8(-1)` unarmed category**:
the original helper reads memory before the allocated OBJECTS buffer.
The original loader's pointer arithmetic has now been recovered, but
the allocator-adjacent **runtime byte value** is still unknown. Conditional
`24478` arbitration now supports `-1` and pseudo hands; **capturing the real
original heap values and connecting timed attack input** remain prerequisites
to live attacks.
The [offline original-byte audit](tools/verify_combat_entry_bytes.py)
had reached **87 opcode anchors at that earlier checkpoint**; the [current verifier](tools/verify_combat_entry_bytes.py) contains **273**, including the 15 then-newly audited `24478` empty/pseudo short-circuit instructions (both original party-status checks remain distinct) matched
against original CODE.2. This is source evidence, not a new game build.

**Bounded nonnegative `240A8` checkpoint: 75%** (compiled/original-runtime
comparison pending). **Overall faithful-demo release acceptance: 20%**.

## Level 22 live HUD item handling (original CODE.2 214B8 / 211D8)

The [live HUD hand and actor-panel checkpoint](LEVEL22_HAND_INTERACTION_CHECKPOINT.md)
extends ordinary full-inventory swaps into the playable world UI. Supported
normal-gameplay **right-click hand tiles** exchange their held items, preserving
item identity, the identified bit and the original selected-hand ready-deadline
reset. A **right-click on the actor inventory panel** places an ordinary
held item in the actor's first free ordinary slot without opening the modal.
An inventory-full result leaves the item safely on the cursor. These native
right-click bindings are provisional pending DOS event-flag trace confirmation.

The original `0F238` known-bit early return and `211D8` direct-stash
known-bit non-update are preserved rather than replaced by generic inventory
semantics. Special equipment, container and spell-hand branches remain
explicitly unsupported. The original `214B8` separate use/attack path is
not yet enabled: original target selection, damage, cooldown and feedback
must be reconstructed first.

**Hand interaction checkpoint: 60% (3/5 gates) on current source**,
pending a deferred manual CI rerun and installed-game playtest. Historical
[Windows CI run #631](https://github.com/Hyperbog/RLOFT/actions/runs/37719712911)
passed 102/102 for the *earlier binary*, but did not validate the later
`+0x19E` readiness offset correction. Overall faithful demo: **20%**.

## Level 22 functional gameplay integration (ordinary inventory)

The next [inventory interaction checkpoint](LEVEL22_INVENTORY_INTERACTION_CHECKPOINT.md)
adds **original CODE.2 12E70 ordinary 3x4 slot clicks** and the bounded
`117A0` held-item/identified-bit exchange to the existing four-party inventory
modal. Players can now take a previously picked-up loose item from the held
cursor into an empty normal inventory slot, freeing the cursor for another
pickup. The model tests also cover the full sequence of three Assassin relic
pickups/stashes followed by the fourth item completing the original Level 22
possession predicate.

Temporary spell-hand objects, occupied container insertion, equipment and
hand action rules remain gated. Combat and the 22-frame Level 23 transition
are not yet enabled. Automated Windows compilation and an original-asset
inventory interaction playtest remain separate acceptance gates.

**Inventory subcheckpoint: 83% (5/6 bounded gates)** after Windows 102/102 CTests; real gameplay interaction remains unverified.
**Overall faithful-demo release acceptance: 20% (1/5 gates)**.

## Level 22 combat accuracy — original hit test

The separate [CODE.2 C80C combat hit-check checkpoint](LEVEL22_COMBAT_HIT_CHECKPOINT.md)
implements the original ordered player-to-NPC threshold calculation as a
bounded, deterministic native primitive: held-target auto-hit, weapon and
Strength adjustments, burden, Slow, Bless/Aid, Stone Giant Gauntlets and
the original **modified-d20** automatic-hit/miss rules. Synthetic tests
are registered in CMake, but the input command, hit target, RNG, damage,
weapon category gate, NPC reaction and hand cooldown have **not** been
connected. Attacks are intentionally not dispatched by arbitrary clicks.

**Hit-check arithmetic subcheckpoint: 100% (3/3 bounded gates)** after
Windows 102/102 CTests; full combat is still unimplemented and overall
faithful-demo acceptance remains 20%.

## Extended integration milestone — Level 22

The demo's original boot/character-creation milestone has been extended to
**all systems integrated for the Level 22 Assassin/relic sequence**.
The earlier boot/controller documentation below remains valid but no longer
describes the full current scope.

The original SINTAB 127-block rasterizer interpreter, original RES3 type-7
pixel execution path, and whole-scene VGA trace integrity are documented in
[TYPE7_RENDERER_CHECKPOINT.md](TYPE7_RENDERER_CHECKPOINT.md).
The separate original CODE.2 surface-prepass investigation and its
signed/unsigned SINTAB underflow boundary are tracked in
[SURFACE_PREPASS_CHECKPOINT.md](SURFACE_PREPASS_CHECKPOINT.md).
A **transactional SINTAB pixel executor is now connected to supported
RES4 surface prepass runs**, with per-run diagnostics and pending coverage.
Ordinary SINTAB main-pass wall rendering and DOS-frame equivalence remain open.
Viewport-selected Assassin damage and loose-item pickup bindings, including
snapshot-invalidating world state, are documented in
[LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md](LEVEL22_VIEWPORT_ACTION_CHECKPOINT.md).

**Do not label the demo finished yet.** Ordinary SINTAB wall rendering, final
original-frame equivalence, remaining host combat/inventory-input bindings,
and relic-transition integration remain open.
Development commits continue on `demo-v1`; the Windows CI workflow handles
native-source changes while Linux is manual opt-in.


## Certified preservation reference

The complete original-engine preservation documentation is mirrored as
[PreservationArchive/](../PreservationArchive/00_SCOPE_EVIDENCE_BASELINE_AND_OPEN_QUESTIONS.md)
from the pinned `code1-decompilation` source, including the 22 completed
static chapters and the separate 91-task future research register. See
[PRESERVATION_INTEGRATION_ROADMAP.md](PRESERVATION_INTEGRATION_ROADMAP.md)
for the original-code-to-native source map, open acceptance tests, and the
resolved two-routine CODE.2 reconstruction reconciliation across branches.
Both branches now share the original-byte-corrected `Code2/Code2.cpp` blob
`87f6638921c53a32bde59735515fbc175e2c96ee`; see
[the recorded CODE.2 errata](../Code2/POST_CERTIFICATION_ERRATA.md).
The historical 2026-10-05 source-certification hashes remain provenance
for the pre-correction revision, not the present source.

The native demo is a **separate** workstream. GitHub Actions run #572 passed
full Linux and Windows compilation/tests (99/99 Linux tests; Windows
executable published), but original-frame equivalence and the full Level 22
Assassin/relic/transition gameplay remain unverified. Automatic Linux jobs
are intentionally paused to save Actions minutes; Windows builds and tests
remain available for native changes.

## Asset policy

The demo does **not** bundle Ravenloft game data.

Normal use is:

1. locate an installed Ravenloft copy automatically;
2. identify the original media source;
3. read/extract only the files required by the native runtime;
4. preserve original resource IDs, palette indices, timing, and data semantics.

A pre-extracted resource folder remains supported for development and preservation testing, but it is not the intended end-user requirement.

## Installed-copy discovery

The native source layer currently supports:

- the GOG Ravenloft product registry entry (product ID `1432905053`) on Windows;
- `RLOFT_GAME_DIR` as an explicit override;
- common GOG Games / GOG Galaxy roots;
- common Steam roots;
- a directly supplied game installation directory;
- a directly supplied `game.gog` path;
- a loose directory containing Ravenloft `RES0`, `INIT`, and `T`.

Candidate installations are accepted only when Ravenloft-specific media anchors are present.

## game.gog

GOG's installed CD image can be consumed directly. The demo has a small read-only ISO-9660 reader and does not require 7-Zip or a manual extraction step.

Supported data-sector layouts:

- 2048-byte ISO sectors;
- 2352-byte raw Mode-1 sectors with 2048-byte user data;
- 2352-byte sectors whose ISO user data begins at offset 24.

ISO version suffixes such as `;1` are normalized during lookup. The indexer walks subdirectories so resource files do not need to be at the image root.

## Runtime-side files

For files that belong to the writable installed game rather than the CD resource image, lookup checks both:

- the installation root;
- the installation root's `RLOFT` directory.

This keeps the original distinction between installed runtime state and CD/resource media available to later compatibility work.

## Current boot milestone

The binary-proven transition implemented so far is:

```text
R.EXE supervisor
  -> CODE.2 startup
  -> CHARS-absent cold-start presentation
  -> CODE.2 four-entry startup menu
  -> menu index 2
  -> legacy result 10
  -> R.EXE launches CODE.1
  -> character-creation workflow
```

The native RES0/COLORS cold-start renderer and startup-menu presentation are implemented. The Windows host also composites the original Ravenloft cursor from the selected installation's RES0 data rather than using a host-system pointer.


## CODE.1 character-creation boundary

Direct re-disassembly of the certified CODE.1 object closes the first three
controller states:

```text
state 0  0x1CD0  choose one of the two original player-character slots
state 1  0x1DD8  sex selection
                  result 3 -> sex 0 / MALE
                  result 4 -> sex 1 / FEMALE
state 2  0x1E64  race selection
                  results 1..6 -> race codes 0..5
```

The state-0 presentation is now rendered from the installed game data.
CODE.1 loads COLORS palette 8 before the controller. Its first screen uses
RES7 graphics, including the two vertically emitted background strips at
RES7:24/25, the fixed RES7:30..38 control family, RES7:42, and the two
state-0 selector graphics.

The observable CODE.1 opening presentation at `0x45FC..0x4976` is implemented,
including its 70 displayed frames, palette reveal, opening music ramp, and
RES6 one-shot sound cues. The reconstructed controller then continues through
the character-creation workflow, its transition animations, stat generation
and editing, portrait selection, naming, final acceptance, CHARS serialization,
and the status-10 supervisor handoff back to CODE.2.

For Demo v1's requested cold-start-to-character-creation slice, the remaining
unimplemented CODE.2 behavior is outside the target path: the CHARS-present
startup/import branch and the full visible behavior of startup-menu entries 0
and 1.


## New original 2AB38 planar VGA pixel-address checkpoint

The [glyph/sprite rendering checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
also now includes a source-backed `2AB38` address/plane model:
the original combines two **mutable** row-shift globals
`EFED/EFF1`, unsigned `column>>2`, `column&3` as
the VGA plane, and the original `F001` linear page base.
It preserves wrapping x86 DWORD arithmetic and never
invents constant screen pitch or a default DOS video page.

**20 additional original signatures passed**, raising
the executable audit to **577 named checks at 569 distinct
addresses**. Local strict C++17 Debug, Release and UBSan
pixel-address model tests passed. This calculation is
registered in the native CMake/CTest source but is
**not included in the prior green Windows 130/130 run**.
The actual original sprite plane writes and display
compositor remain unintegrated; faithful gameplay
acceptance stays **20% (1/5 main gates)**.
