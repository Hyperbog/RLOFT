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

