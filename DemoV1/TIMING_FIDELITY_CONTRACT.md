# Timing fidelity contract — game-world motion, input, animation, combat, and transitions

**Project rule:** A correct-looking screenshot or semantically correct state mutation is *not* complete fidelity unless its **observable timing, ordering, and user-response cadence** agree with original *Ravenloft: Strahd's Possession*. This is a cross-cutting release acceptance criterion for every new gameplay feature, not a final polishing phase.

**Authority:** immutable shipped CODE.1/CODE.2 instructions and their timing data; [Preservation Archive Chapter 17](../PreservationArchive/17_TIMING_INTERRUPTS_AND_FRAME_CADENCE.md); [Chapter 16, ANM timeline](../PreservationArchive/16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md); verified original DOS runs and captures. Native Win32 frame pacing is a host implementation, **not** itself proof of original gameplay cadence.

## Separate timing domains — do not merge these clocks

| Domain | Original evidence | Port constraint |
| --- | --- | --- |
| VGA presentation | CODE.1 `AF64` and `AF34` coordinate page switching with vertical retrace | Preserve present/palette ordering and smooth retrace delivery; never infer movement or AI tick duration from the host's display refresh alone |
| Game IRQ ticks | CODE.2 `30708` increments `F58C`; `30700` reads it | Preserve tick-based deadlines, unsigned comparison details and input/service work while waiting; the physical tick rate varies with driver/PIT/vector behavior and is **not certified globally** |
| Common game loop | CODE.2 `201A7..2022B` calls `2E608` to anchor current game tick, processes world/input/render/audio work, then `2E614(2,0)` | Preserve the original **anchored two-game-tick target** and update/service ordering; it is not automatically the VGA refresh interval or fixed real-time FPS |
| Pauseable gameplay maintenance | `19C80` separately increments `A7E4` while enabled; `1E75C` consumes it | Keep gameplay maintenance and effect deadlines distinct from display frames, wall time and raw IRQ ticks |
| World clock and save/restore | `CF4C` is 16.16 game-world time; save also persists the other independent tick state | Rest/fast-forward, scripted time, reloading and display must preserve each original clock and timer rather than recompute from host elapsed time |
| Animation/ANM | `13834..139AA`; ANM `default_wait` vs alternate value on chunk 3/8 image updates | Decode and honor each authored wait and packet order; do not make all frames uniform or force one animation frame per display refresh |
| Input and control | `307F0`, `1FC14`, `2E614` cooperative waits | Preserve key-hold integration, repeat/turn response, queued mouse events, interruption and modal pause rules across animation phases |
| Combat and NPC actions | `214B8`, `24478`, `26018`, `E5F0`, distinct hand-state/readiness dwords | Authenticate both hit/damage *and* eligibility delays, per-hand recovery, actor scheduling, creature reactions, animation/audio ordering |
| Authored visual transitions | CODE.1 card flip; CODE.2 original 22-frame Level 22 mist event | Preserve intermediate images, frame ordering, timing changes and input behavior; do not jump between endpoints simply because the final image/state is correct |

### High-priority risk in the present native host

`DemoV1/src/win32_main.cpp` uses `kVgaRefreshHz = 70.0863` as a nominal **presentation** cadence and currently runs persistent exploration input integration against that host loop. Chapter 17 records original game-loop `2E608 -> 2E614(2,0)` and independent IRQ/gameplay clocks. **We must measure and reconcile movement and turning speeds instead of assuming 70.0863 simulation ticks per second are original.** The standalone animation host also offers high-resolution waitable timers, but timer *precision* does not establish timer *authenticity*.

No universal movement speed in milliseconds, frames per attack, or ANM frame rate is asserted without original DOS runtime evidence. CODE.1 startup's 54,925-us BIOS-tick interpretation and VGA retrace pacing may be separately correct for their specific code paths; that must not be extrapolated to every driver/scene.

## Rules for all new feature work

1. **Record a timing contract** for each state transition: source CODE offset; authoritative tick/counter; deadline or duration unit; frame/step ordering; input interruption rules; relevant sound/palette updates. If unknown, label unknown rather than substitute a convenient constant.
2. **Keep deterministic simulation separate from presentation**. Host refresh rate, window size, monitor Hz and rendering cost must not change game-coordinate movement per original simulation tick, enemy cadence, or number of steps in an authored animation.
3. **Preserve original paced sequences**, including intermediate card-flip frames, turn/stride progression, idle and NPC reactions, attack windups and recovery, effect lifetimes, rest fast-forward and transitions. No instantaneous endpoint swaps unless the original explicitly does so.
4. **Preserve pause/resume and cooperative servicing**: modal screens, menu clicks, audio service, input queues, anchored waits, timed interruptions, background-window throttling and loading must not cause unintentional simulation fast-forward.
5. **Do not silently “fix” original timing quirks** (integer wrap/truncation, exact strict comparison, rollover, wait anchoring, effect cancellation, accumulated fractional movement). Fidelity comes before generalized engine smoothing.
6. **Render smoothly without rewriting original behavior**. If display interpolation is needed for modern 60/120/144-Hz monitors, it must be presentation-only, opt-in/independently testable, never advancing original game state extra times or changing collision/targeting.
7. **Treat audible pacing as part of timing fidelity**: animation, UI highlight, sound start/stop, palette fades and event response must remain synchronized to the original authored sequence.
8. **No completion claim without temporal acceptance**. A model with correct static bytes and endpoints, or a single good screenshot, remains unverified until a time-series comparison is available.

## Original CODE.2 combat cadence — audited requirement

The native `E5F0` source audit now proves **two independent hand time consequences** using the pauseable original `A7E4` maintenance counter:

- A nonzero `C80C` hit writes `actor+0x19E+4*hand = first_A7E4 + floor(action_interval/2)` and stores damage at `actor+0x1AA+4*hand`; a miss does **not** perform those writes.
- The next callable routine—CODE.2 logical `24B48`—performs action graphics and then, **when the action interval is nonzero, even on a MISS or without a target**, reads `A7E4` again and writes `actor+0x10B+4*hand = second_A7E4 + full_action_interval`. The two clock reads can differ because presentation comes between them.
- For normal item IDs the interval is `9 * ((OBJECTS WORD +0x35 >>2)&15)`; for empty or pseudo hands it is 18. Original actor effect DWORD at `+0x132 & 0x01000010` halves that interval (integer truncation).
- Before hit resolution, CODE.2 logical `E578` services pending effect flags across all four actors and the attacking actor's additional state, setting original HUD dirty flags. Such service must not be silently deferred until after the hit or skipped because it is not visible in a still screenshot.

This is a **positive behavioral rule** for all combat implementation and regression acceptance. A missed swing is *not* instantaneous availability for another swing; conversely the hit-ready dword is not a replacement for the full action-busy dword. Neither may be implemented as a generic Windows frame delay. The source-only [combat target and timing checkpoint](LEVEL22_ATTACK_TARGET_AND_TIMING_CHECKPOINT.md) and original [CODE.2 continuity archive](../Code2/CODE2_CONTINUITY_ADDENDUM.md) record exact PC and machine-byte evidence; actual DOS tick calibration and native temporal traces remain required.

## C80C return-value fidelity and Rod RNG branching

An exact original CODE.2 audit proves `C80C` returns a **Boolean DWORD** (0/1), not a raw d20. `E5F0` stores that exact return and later Rod of Smiting checks it against 20, making its charged critical branch **unreachable in the normal attack path**. For NPC types `0x17` and `0x1F`, a successful Rod attack first consumes its normal OBJECTS dice, then additionally consumes **two d8 rolls with +6 per die**, instead of a single combined damage roll. Other target types retain only the initial dice result. Never reintroduce charged critical effects by comparing a host raw roll to 20; never omit the initial Rod RNG draw before the extra two.

The first part of original `E284` NPC injury now also reproduces reaction-state/countdown and special type-8/type-15 HP arithmetic. The Level 22 Assassin (type9) shares this prelude. All resulting HP, animations, reaction frames and actual game-time effects still require compiled native and original DOS comparison.

## Respawn RNG and IRQ-clock ordering inside NPC death

Original CODE.2 logical `E0F8`, invoked *only if* a matching
`E284:E50F` tracked NPC type is found, uses its own original
`30700` IRQ-clock read, calls `290D0`, and draws
**two original 30CA2 RNG results per candidate coordinate**.
It allows at most three placement attempts, rejecting occupied
map cells and candidates too near the party. When a slot is
successfully initialized, it performs an **additional original
1C328 NPC HP dice roll**, then reports success; otherwise it
returns zero. Such success changes whether the original corpse
flag is written at `E539`.

This sits **between death-presentation `38110` and corpse
marking `E539`**, and is not interchangeable with the separate
`A7E4` hand recovery timebase or a host-managed respawn timer.
Do not consume placeholder random numbers, skip failed-placement
RNG attempts or normalize respawn to a fixed delay. Native source
currently keeps the result as an externally observed dependency;
see the [E284/E0F8 checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md).

## NPC death follow-on: original event and presentation order

Original CODE.2 `E284:E31F..E577` branches on the **signed**
resulting NPC HP only *after* the `3ACDC` reaction flag update.
For a positive-HP NPC, immediately request the original `38110`
presentation with `NPC_DEF WORD+0x41 + global 0x258C0`
and return. A dead NPC instead runs its type-specific script
and drops **before** the common death presentation with
`NPC_DEF WORD+0x43 + global 0x258BC`.

The Assassin type `0x09` creates four exact loose items through
`3195C` in order and writes story `BF44=1` after the fourth,
then enters common death presentation. The later
`E0F8`-conditioned three-slot respawn check controls whether
`NPC+0x0B` bit02 is set, followed by `1DDA4(DEF WORD+0x23)`.
Do **not** move a drop after its death graphic, attach the
nonlethal `+0x41` asset to a corpse, invent an unconditional
respawn result, or model these as a single instantaneous host
event. TYPE `0x18` may return earlier after a separate
world-state/RNG sequence.

The [E284 death-follow-on checkpoint](LEVEL22_E284_DEATH_FOLLOWON_CHECKPOINT.md)
contains the original PC ledger and byte checks. Native source
currently plans this order and supplies type9 drop definitions;
the `38110/1DDA4` temporal hooks and DOS frame/sound reference
are outstanding release gates.

## HARM staged HP writes and NPC reaction sequencing

The original spell-hand HARM branch (`E5F0:E815..E83B`) is
not one damage assignment. **After a successful C80C result**
it consumes its real `1d4` RNG draw; only then does it compare
signed NPC HP and *conditionally* write the rolled value directly
to `npc+0x12`. It emits/queues effect `0x24` and explicitly
zeroes the provisional injury, **then** invokes general `C910`
which enforces minimum-one damage, and **then** calls `E284`
to subtract another HP point. For target HP20 and roll3, the
staged values are **HP3 → HP2**, not HP3 as a final result.
A 1-HP target whose HARM roll exceeds 1 is not healed and can
die after the second injury. Every stage must remain ordered
relative to sound/visual service and the two independent
`A7E4` action deadlines.

In `E284:E306..E324`, original `3ACDC` NPC classification
updates high NPC runtime-state bits **before** the positive-HP
test. This update runs even on nonlethal hits; moving it into
the death-only handler would change AI/display reactions.
The Level 22 Assassin path now preserves it in source, but
original DOS audiovisual and tick-trace comparison remains
a separate release gate. See
[Strength/HARM/E284 checkpoint](LEVEL22_STRENGTH_HARM_E284_CLASSIFICATION_CHECKPOINT.md).

## Original combat RNG and damage-effect ordering

The original `E5F0:E6C9..E963` branch is reached **only after C80C hit success**. Once that branch executes, its item-specific original `1C328` dice recipes call `30CA2` RNG once per die, preserving the original count, per-die modifiers, and final modifier ordering. An attack that misses the initial hit check **must not roll or advance the damage RNG** merely to stage an effect, while its **nonzero-duration action-state lockout still runs later** through `24B48`.

Temporary spell hands `0x7E..0x86` have independent original dice and some authoring-time effects. `0x7F` (Vampiric Touch) needs `22E40` to determine its dynamic die count; `0x84` (HARM) may first clamp an NPC's HP before `C910` sets minimum outgoing injury to one; literal weapon `0x4A` (Rod of Smiting) follows a separate target-type/hit-roll/charge branch rather than C910. **Do not normalize these into a common generic damage formula or reorder their visual/audio feedback, RNG calls, target HP writes and action-clock observations.**

The read-only [damage dice and C910 models](LEVEL22_DAMAGE_DICE_AND_WEAPON_GATE_CHECKPOINT.md) register the original parameters and branch distinctions, but **do not** execute them in a live combat timeline. Acceptance still requires actual DOS input/RNG/clock/render/audio traces, including missed attacks, spell-hand effects and creature-protected targets.

## Measurable timing acceptance and regression plan

- Original DOS **reference traces**: record a representative sequence of timestamps, original tick counts, player 16.16 coordinates/heading, action flags, displayed page/palette, animation frame IDs, sound cues and modal transitions under recorded input. Distinguish measured values from inferences and note DOSBox/hardware/audio configuration.
- **Movement:** hold forward, backward and turning inputs for controlled tick durations; compare position, heading, acceleration/remainders, stops at collision, diagonals, and input-release responsiveness at multiple sampling points—not only final position.
- **Animation:** record frame order, per-frame dwell/wait values and total elapsed sequence for card flips, character UI, intro, attack/use, NPC death and original Level 22 mist (22 frames). Include partial frames, interruptions, palette changes and sound cue timing.
- **Combat:** confirm actor/hand eligibility and recovery by original tick and action-state dword, target acquisition, delayed damage/feedback and opponent scheduling. Avoid applying damage on the mouse event merely because accuracy arithmetic returns hit.
- **Modal/pause/time:** test entering/exiting the automap, inventory, rest and quit; confirm exactly which clocks/queued events advance or stop, and behavior after a long host stall or window focus loss.
- **Variable-host display:** compare the same recorded simulation input on 60/70/120/144-Hz displays and under throttled rendering. The *original game-state tick trace* should match regardless of monitor refresh.
- **Tolerances and evidence:** establish frame/tick-accurate matching where original data proves discrete counts. For real-time variability, report observed distribution and hardware/driver configuration; do not invent a universal fixed tolerance or claim exact DOS timing where none has been measured.

## Latest original E0F8 model boundary

The [read-only E0F8 source model](LEVEL22_E0F8_RESPAWN_SOURCE_CHECKPOINT.md)
models 290D0's zero-low16 BIOS fallback and the exact original
coordinate/HP dice draw order. It does **not** observe a real IRQ tick,
update the shared game RNG, invoke native combat, or change world state.
Do not replace the 30700 IRQ observation with A7E4 maintenance time,
VGA refresh, or an invented fixed host clock. No time-series DOS/native
validation has been completed for this new model.

## Current implementation boundary and next actions

1. Preserve current original-derived event/state reconstruction; do not enable attacks until timing of `24478/26018/E5F0` plus hand state and damage/feedback order is modeled.
2. Audit the host's movement-update frequency, original IRQ tick scheduling, cooperative `2E614` wait and input handling; introduce a **separate original-game simulation clock** only after original timing phase and step mapping are validated.
3. Add deterministic source-level temporal regression fixtures for turn/stride progression, ANM packet waits, UI flips, cooldown/idle/NPC scheduling and world-clock pause/rest. (The final NPC spelling uses ordinary 'NPC'.)
4. Compare against DOS traces before claiming Level 22 playability/fidelity. Keep environment/surface frame equality and *temporal* equality as separate tests.
5. Keep GitHub Actions **manual-only** while the repository remains private. No new Windows previews merely to observe timing.

**Completion accounting:** This contract is a new cross-cutting release requirement, **not** evidence that timing fidelity is finished. The preexisting overall five-major-gate faithful-demo acceptance remains **20% (1/5)** until runtime behavior, including cadence, is verified.


## DDA4 level-change event clock and HP service (source audit 2026-10-08)

Original DDA4:DF0E..DF86 writes a changed class level, invokes
`50DC(0,actor)`, conditionally calls `3EA4` or `3DE4`,
and **unconditionally** invokes `3210F`, `1C21C`, then
`DC7C`. Original `1C21C:1C21E` directly calls `30700`
and sets `CF54 = observed_irq_tick + 0x5A`; it also
queues the associated message and sets `CF59=1`.
The `DC7C` path calls original `1C328` dice before
modifying current/max HP. Thus even a level change whose
50DC predicate returns negative still has observable IRQ
timing, queued presentation and RNG/state consequences.
These steps **must not** be replaced with one immediate
XP/HP recalculation or a Windows frame-time clock.

The [new source-only 50DC service plan](LEVEL22_DDA4_50DC_SERVICE_ORDER_CHECKPOINT.md)
reports this ordering without executing the services; do
not confuse a read-only trace with live timing acceptance.



## DC7C level-up RNG ordering — original-source acceptance boundary

At original `DDA4:DF81`, the `DC7C` level-up HP routine
is entered **after** the mandatory `3210F` presentation and
`1C21C` IRQ-timed message queue. Its first gameplay action
is `1C328(1,class_die_sides,0,0)`, consuming **exactly one
30CA2 RNG draw before evaluating any class cap or Constitution
modifier**. Even when a level >=10 or >=11 causes the die
value to be discarded from the HP formula, the random draw
still advances the shared DOS RNG state.

The [DC7C source checkpoint](LEVEL22_DC7C_HP_PROGRESSION_CHECKPOINT.md)
reproduces the original six-way class dispatch, post-roll
state, HP arithmetic, multiclass division and two WORD additions
without mutating the game. Its 16,416 original-asset-backed
comparison cases pass. The native DDA4 timeline must **not**
execute this pure model out of order, substitute the Windows
clock, skip its RNG side effect on capped levels, or apply
HP to live characters before the 3EA4/3DE4 and 1C21C
services have been integrated.


## 1C21C IRQ-timed level-up message queue (2026-10-08)

Original CODE.2 `1C21C..1C257` calls **30700** and
writes `CF54 = full_IRQ_tick + 0x5A` with original DWORD
wrap. It copies a zero-terminated 3210F-produced message
to `14580` without clearing trailing bytes, then sets
`CF59=1`. The separate display/expiry stage checks
unsigned `observed_IRQ_tick > CF54` at `1C2D1..1C2D7`
(plain CMP/JBE, NOT a wrap-safe host elapsed-time helper).

The [source-only callback checkpoint](LEVEL22_DDA4_SPELL_IRQ_CHECKPOINT.md)
has a bounded read-only model of the queue and expiry
predicate. **Do not use Windows frame time, gameplay A7E4
time, timer approximations or synthetic retries** when this
is later connected to the live DDA4 event sequence.
Original 1C258..1C326 display effects and original 3210F
message bytes are separate remaining acceptance gates.


## DDA4 original message creation and IRQ display cadence (2026-10-08)

CODE.2 `DDA4:DF54` formats the original string `%s gains a
level.` through `3210F` from DGROUP `+548` to buffer
`+759C`, then `1C21C` queues it with the actual independent
`30700` IRQ read and a `+0x5A` deadline. The renderer
call at `1C2C4` enters `1C258` first and draws a pending
message (if `CF59!=0`), clearing the pending byte only after
the original compositing call `1A8C8` has returned.
**Only after this stage** does `1C2CC` call `30700`
to check expiration. The unsigned comparator checks
`current_IRQ_tick > DWORD[CF54]`. Erase writes the
`0xFFFFFFFF` deadline sentinel **before** the second
`1A8C8` compositor call.

This means a single update may both display and erase a
message when drawing took enough original IRQ ticks.
Do not sample time once at the start of a frame, substitute
A7E4 gameplay ticks, or normalize unsigned timer wrap.
See [source audit](LEVEL22_DDA4_MESSAGE_FORMAT_DISPLAY_CHECKPOINT.md).

The current native 3210F and display-lifecycle models only
plan source-verified actions. Renderer services 291B0,
2939C, 3060C, and 1A8C8 and shared-global re-reads
must be integrated in their original observed temporal order
before declaring a timed level-up message implementation.


## Verified Windows build versus unconnected IRQ renderer services

Original 1C258/1C2C4 now has a C++ **execution bridge**.
It invokes injected 291B0, 2939C, 3060C and 1A8C8
collaborators synchronously and reads 30700 only AFTER
pending-message drawing has returned. Callback-side changes
to C050, 2222C, CF54 and the message buffer are re-read at
their real original call sites. Pending CF59 clears after the
first composite; expired CF54 becomes FFFFFFFF before
the second composite. The independently verified 291B0
page state uses original F005 stride, not a hardcoded Windows
stride. See [actual run history and source boundary](WINDOWS_BUILD_AND_IRQ_RENDERER_CHECKPOINT_20261008.md).

Windows CI #634 passed all **130 tests**, but the new
execution bridge was committed subsequently and is not
included in that pinned build. No real display service,
DOS IRQ observation or pixel-matching runtime claim has
been made by adding this bridge. The workflow is again
manual-only.


## Original signed glyph, shaded sprite and F0C8 refresh ordering

After 1C258 calls 3060C, original 303C8 selects font via
2F29C, stores the pointer at 2B67C and traverses each
**signed** DOS character. Every glyph re-reads F56C
to choose 2939C or shaded 293EC, and **after the
sprite renderer returns** re-reads 2B67C and the unsigned
advance byte at font_record+signed_character+4 to advance
2B680. The original code loops on live source bytes,
not an eagerly copied Windows text string.

Both 2939C and 293EC skip drawing entirely for a null
resource pointer; when nonzero they delegate to 2A380
or 2A584, then tail-call 2E758. The 2E758 conditional
callback pointer F0C8 is read **after** sprite rasterization,
which may change that pointer. The shaded sprite primitive
also self-patches the original shade operand locations.
These effects have explicit bounded, callback-driven source
models but the pixel rasterizers and refresh effect are not
yet implemented.

[Original glyph/sprite source checkpoint](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md)
records 48 additional verified x86 instruction signatures and
focused strict Debug/Release/UBSan tests. No later Windows
CI pass is claimed. Preserve the current 30700 post-draw
IRQ observation, independent of Windows frame time.


## 2AB38 variable video addressing and plane selection

The original sprite draw at 2A380/2A584 invokes 2AB38,
which reads mutable runtime `DWORD[EFED]`,
`DWORD[EFF1]` and `DWORD[F001]`; it computes
two sequential low-five-bit CL shifts from the row,
adds unsigned `column>>2` to its linear VGA page base
and returns `column&3` as VGA plane. The native pure
address model now matches those original instructions
and x86 DWORD wrapping. No fixed 80-byte scanline
or A8000 page should be inserted unless the **original
runtime** has selected that exact shift/page state.

See [source and tests](LEVEL22_ORIGINAL_GLYPH_SPRITE_RENDERER_CHECKPOINT.md).
This address projection is not the original plane-mask
pixel write itself, nor a new timing source. Original
303C8 per-glyph callbacks, F0C8 refresh, 1A8C8
composition and IRQ-after-render ordering are still
mandatory for faithful timed message output.


## Original 2A380/2A584 sprite mask, shade pointer and refresh order

Source-backed normal and shaded sprite raster loops now
write real four-plane bytes to native `VgaPlanarMemory`.
The complete call chain still orders
`2939C/293EC -> 2A380/2A584 -> 2E758/F0C8`,
and the IRQ is not sampled until all original rendering
callbacks return. The shaded `2A584` code first writes
the table pointer DWORD to original CODE.2 operands
`2A60B` and `2A672`, then indexes that 256-byte DOS
memory table with the original raw sprite color. The
sentinel branch skips both lookup and VGA write.

The current raster source returns the pointer-patch value
and the final sequencer plane mask, but does **not yet
persist** these two live original state side effects.
An actual in-game renderer must write these values in
the original order and preserve post-raster F0C8 refresh
and `1C2C4` post-draw IRQ observation. Do not substitute
a guessed shade formula, modern framebuffer pitch, or
host frame clock. [Technical checkpoint](LEVEL22_CODE2_SPRITE_RASTER_CHECKPOINT.md).


## Original sprite self-patch and hardware state now preserved

The original shade pointer is stored to CODE.2 operands
2A60B/2A672 **before** 2A584 invokes 2AB38 or reads
sprite resource bytes. The native machine capture now
implements both opcode writes before DOS resource-reader
callbacks, with repeatable 32-bit little-endian patches
to writable original code memory.

Sprite plane loops set the original VGA sequencer index-02
mask **before** writing each plane. The last mask remains
observable after sprite rendering or framebuffer clearing.
Transparent branch 2A42B/2A651 writes F009 to the actual
observed source height after the initial sequencer OUT,
whereas opaque sprites leave F009 unchanged. All these
source-ordered updates are opt-in with a captured original
DOS machine state and do not invent the initial register.

See [current machine-state checkpoint](ORIGINAL_SPRITE_MACHINE_BINDING_CHECKPOINT_20261008.md).
The later 2E758/F0C8 refresh and 1C2C4 full IRQ read
are still separate temporal obligations; the current
binding executes source-order callbacks, not a synthetic
host-clock substitute. The newer source is not covered
by the previous green Windows runner.


## 1A8C8 original framebuffer compositor — wrapper verified, pixel copy open

`1C258` calls `1A8C8` **before** the independent `30700`
IRQ read in `1C2C4`. Original `1A8C8` conditionally
wraps `2A078` with signed **row/column** overlay
intersection and nested `1A288/1AB20` services. A
buffered overlap may decrement the 9E37 overlay WORD,
switch 9E39 page, call 1A288, copy via 2A078,
move 0x400 backing bytes with 30CD4, call 1AB20,
then increment the *live* refcount, restore 9E39
and clear A6BA. This whole chain completes before
`30700` observes the IRQ clock.

[Original 1A8C8 source checkpoint](LEVEL22_CODE2_COMPOSITE_1A8C8_CHECKPOINT.md)
adds a callback-driven reusable wrapper and a
timed-message callback adapter. The underlying VGA
2A078 graphics-controller port operations and
nested overlay routines still require source-backed
native effects and DOS trace comparison. Never
pre-sample IRQ time, omit the 1024-byte backing move
or change signed edge comparisons into ordinary
unsigned rectangle intersection.


## Original 2A078 VGA latch-copy versus DGROUP 30CD4 move

The `1A8C8` screen-compositor wrapper now connects to an
actual native `2A078` four-plane VGA **write-mode-1**
rectangle copy. `2A078` momentarily selects the destination
VGA page, computes its byte address via `2AB38`, restores
the original source `EFE9/F001` page via `2AB6C`,
sets graphics controller 05h to `0x41`, selects
sequencer plane mask `0x0F`, then performs the original
**FORWARD** `REP MOVSB` with full four-plane latches
on each source read. Mode 05h is set back to `0x40`
after copying, while the sequencer mask persists.
Overlapping VGA copies intentionally do **not** use
an overlap-aware memmove.

By contrast, original `30CD4` is an independent
DGROUP overlap-aware move that uses original unsigned
source/destination comparisons and may copy **backward**.
The native `1A8C8` backing callback can now execute its
exact 1024-byte memory move after the planar copy and
before the overlay restore call.

The complete `1C258 -> 1A8C8 -> 2A078 -> 30700`
order is preserved in the injectable native bridge.
Original `1A288` overlay suspension, `1AB20`
restoration, actual DOS resource state and pixel/frame
comparisons remain outstanding. No Windows host-clock
source has been substituted. See
[the original-code checkpoint](LEVEL22_CODE2_2A078_30CD4_COPY_CHECKPOINT.md).


## 1A288 and 1AB20 nested original overlay timing

Original `1A288` does not always draw: if 85FC is
disabled it is a no-op; if 9E37 is nonzero, it only
increments the WORD and returns. On first activation
it runs 2E590, 1A3B8, 292BC (32×16 capture),
2AE30, optional mode-dependent 29804 calls,
then 2E5D4 **before** incrementing 9E37.

Original `1AB20` decrements 9E37 immediately when
85FC is enabled, then restores backing only when
the resulting WORD is zero: 2E590 -> 2932C
(32×16 at caller-supplied signed row/column)
-> tail-call 2E5D4. No increment follows.
The original 1A8C8 compositor already changes
these shared globals before and after its callbacks.
The integrated native adapter preserves the exact
callback, 16-bit counter and page restoration order.
All this work happens before the original 1C2C4
`30700` IRQ observation. See
[the original-overlay checkpoint](LEVEL22_CODE2_OVERLAY_1A288_1AB20_CHECKPOINT.md).


## 2E590/2E5D4 live VGA page stack and temporal page writes

Original CODE.2 `2E590` checks BYTE F0D0 for
depth **exactly 10**, invokes 2F580(1358h) on that
condition, then rereads the depth, saves original
BYTE EF4D at 2B644+depth, increments depth
modulo 256, and calls 291B0 on the requested
page low byte. The matching 2E5D4 tests
depth zero, calls 2F580(136Ch) if needed,
then **still decrements** F0D0 modulo 256,
reads the saved byte and calls 291B0.
The diagnostics are real original callbacks, not
a host-side clamp or exception in normal DOS flow.

The shared native binding exposes these exact
page transitions to the earlier `1A288/1AB20`
overlay timing callbacks. Critically it reads
the live video model's F005 **at the 2AB6C
call site** and updates the same EFE9/F001
registers subsequently consumed by 2A078.
The separate EF4D register is never conflated
with EFE9, which can change without EF4D.
Any original sprite/page/copy work and the
original `1C2C4` IRQ read must still run
in source order; no synthetic host clock is
introduced. See
[2E590/2E5D4 checkpoint](LEVEL22_CODE2_VGA_PAGE_STACK_CHECKPOINT.md).


## Original 292BC/2932C overlay backing clip sequence

The page-stacked original 1A288/1AB20 overlay
code now has source-backed signed backing-clip
wrappers. Both use signed DWORD row/column
positions, clipped first against zero and then
against signed WORD EF3F/EF41 limits, preserving
DWORD LEA/ADD wrap. They pass the original
unadjusted backing pointer to the actual next
`2A114` capture or `2A178` restore service.

Original `292BC` skips the backend when final
column count is zero (`JLE`), but original
`2932C` does **not** (`JL`). That restore
may therefore issue VGA sequencer writes
despite a zero logical column count.
This distinction must survive future pixel
rendering, page-stack callbacks and the
post-render `30700` IRQ observation; no
short-circuit modern graphics clipping should
replace it. [Checkpoint](LEVEL22_CODE2_OVERLAY_BACKING_CLIP_CHECKPOINT.md).


## Native 2A114/2A178 four-plane backing capture/restore

Original overlay `1A288` invokes `292BC -> 2A114`
to capture an area into raw DGROUP backing blocks,
with **each full VGA plane copied contiguously**
before the next read map. `2A114` writes VGA GC
read-map register 04h for planes 0 through 3 and
leaves plane 3 selected. `1AB20` invokes
`2932C -> 2A178` to restore all four blocks
via VGA sequencer masks `1,2,4,8` and finally
executes the exact **`OUT 03C4h,FF02h`**.
The native VGA state now retains the raw 0xFF
port data separately from the effective 0x0F
plane mask.

Even a clipped zero-column restore can issue
source-backed port effects or copy a physical
byte when its starting column plane is nonzero.
These occur **inside** the original
`1A8C8 -> 1A288/1AB20` callback chain, and
the independent `30700` IRQ clock is read only
after the entire draw/restore chain completes.
Do not eagerly flatten the backing into a
modern framebuffer or omit zero-width callbacks.
[Source checkpoint](LEVEL22_CODE2_2A114_2A178_VGA_BACKING_CHECKPOINT.md).
