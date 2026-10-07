# Ravenloft: Strahd's Possession — Preservation Engineering Archive

## 17 — Timing, Interrupt Scheduling, and Frame Cadence

**Status:** FOUNDATION — original CODE.2 interrupt, tick and PIT mechanisms traced; live device-dependent cadence not yet certified  
**Working branch:** `code1-decompilation`  
**Frozen original-evidence baseline:** `373c7f10e6fc0f29c1e805125ef048150fcc7ba7`  
**Evidence:** original `CODE.2` SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`; object-1 file start `0x3AF24`, size `0x3A852`. All addresses below are original **object-1 logical offsets** unless explicitly marked runtime data addresses.

---

## 1. The game has multiple timekeeping layers

The original code separates a **game-visible IRQ-driven counter**, a **cooperative wait/service loop**, and a **PIT-programming callback scheduler**. A host port must preserve the resulting deadlines, servicing and input interruptions without assuming that an ANM "frame" equals a fixed host refresh interval.

```text
IRQ 8 game hook 30708                 optional timer scheduler 35FC6
  -> increment [F58C]                  -> service active callback slots
  -> update gameplay timers 19C80     -> reprogram PIT channel 0 as needed
  -> chain saved IRQ 8 handler         -> retain old timer handler callback
           |
           v
30700 reads current game ticks
2E608 records an optional tick anchor
2E614 waits until deadline while calling 2F868
           |
           v
ANM frame loop 13834..139AA
```

This diagram shows relationships, **not a proven simultaneous installation order for every audio-driver configuration**.

## 2. Game clock ownership

`30700` returns the 32-bit value at runtime `0xF58C`. Interrupt handler `30708..30735` saves registers and segment selectors, establishes its data context through `3566C`, increments that value at `3071C`, calls `19C80`, invokes the saved IRQ-8 vector at `[0xF582]` through a far call, restores state and uses `IRET`. It does not unconditionally send its own end-of-interrupt in this path; it chains the previous handler.

Routine `19C80` consumes the same clock: if `[0xA79C]` is enabled and the current tick exceeds `[0xA790]`, it dispatches `22CE0` and clears the pending flag. When `[0xA7E8]` is nonzero, it increments a separate tick-derived counter `[0xA7E4]`. Consequently `30708` supports game scheduling beyond ANM playback.

## 3. Interrupt installation and cleanup

Initialization `2F2EC`, called from CODE.2 `2044B`, invokes `30BB4` at `2F323`. That routine installs three vectors through DOS-compatible wrappers `3798D` (get old) and `37958` (set):

| Interrupt | Installed code | Preserved old vector |
|---|---|---|
| 9 (keyboard) | `0x308F8` | `0xF576/0xF57A` |
| 8 (timer) | `0x30708` | `0xF582/0xF586` |
| 0 (CPU exception) | `0x30B54` | `0xF57C/0xF580` |

The single-byte state at `[0xF575]` guards repeat installation. `30C38` restores the saved vectors and clears that state; verified cleanup calls include `2F535`, `31033`, and `33C79`. Do not replace these distinct vector ownership contracts with a global "initialize timer" flag.

## 4. Anchored wait semantics

`2E608` stores `30700()` in `[0x2B640]`. `2E614(duration, flags)` selects either `[0x2B640] + duration` or `30700() + duration` as its deadline, then repeatedly:

1. calls cooperative service `2F868`;
2. checks optional input branches when enabled by the flags;
3. re-reads `30700` and continues while the unsigned current tick is below its deadline.

On normal expiry the anchor is reset to zero and return value is zero. The flag-1 path queries `30AD8` and consumes input with `30AEC`; the flag-2 branch uses `2E340/33CB0` and can return `0x1F8 + input_code`. A flag-4 branch exists in the original routine and must remain instruction-accurate; its broader caller contract is not asserted here.

The ordinary ANM frame wait passes flags **zero**, so it is deadline-driven but still services work. This is not a blind sleep.

## 5. Separate programmable timer scheduler

Another IRQ-8 installer appears at `360E8..36136`: it saves the previous vector in `[0x10366/0x1036A]` and installs handler `35FC6`. The timer-callback registration path `3632D` calls that installer upon first registration, creates a reserved callback at slot 16 with period `0xD68D`, and enables it. The corresponding final unregistration `363DD` calls `3616F(0)` and `36137` to restore the old IRQ-8 vector.

Handler `35FC6` iterates 17 slot states (`0..16`) with a reentrancy guard `[0x10272]`. Active state `2` slots accumulate elapsed timer units at `[0x102DA+4*i]`, compare against per-slot interval `[0x1031E+4*i]`, and invoke the stored callback at `[0x10274+4*i]` when due. The normal handler exit acknowledges the interrupt at PIC port `0x20`. Its slot-16 callback `3607B` uses the captured previous IRQ vector, preserving a path for the preexisting timer handler.

## 6. PIT programming and variable cadence

`3616F` programs PIT channel 0 using command `0x36` at I/O port `0x43`, followed by low and high divisor bytes at `0x40`. `3619D` converts the scheduler's input interval by the **encoded integer expression**:

```c
uint32_t divisor = (interval < 0xD68D)
    ? (interval * 10000u) / 0x20BCu
    : 0u;
pit_write_low_then_high((uint16_t)divisor);
```

The input multiplication is a widened x86 operation and the hardware write takes only the low sixteen bits. Do not normalize zero or round the quotient differently without a compatibility test. `361CE` scans active timer slots to find the shortest interval and calls `3619D` when the chosen interval changes.

The effective game-clock tick duration depends on these scheduling and chaining decisions. **A single absolute millisecond rate is not yet certified for every configuration or cinematic.**

## 7. ANM dependency and fidelity checklist

Chapter [16](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md) proves that each frame uses the ANM `default_wait`, or the alternate `[0x12C10]` value when chunk 3 or 8 updates the image. This chapter supplies the timer source and wait semantics; keep packet dispatch and page/palette presentation ordered before the wait.

Regression requirements: test registration and teardown of both IRQ-8 owners; saved-vector chaining; tick rollover boundaries; anchored versus unanchored waits; flag-specific early exits; cooperative servicing while waiting; the 17 callback-slot states; changing shortest intervals and emitted PIT bytes; final timer-vector restoration; and variable ANM packet mixes. These are proposed tests, **not executed gameplay certifications**.

## 9. Timer installation ordering across audio initialization

The primary CODE.2 initialization path at `2044B` enters `2F2EC`. Inside it, `2F323` calls `30BB4` **before** either sound-driver initializer. After SOUND.CFG loading and the configuration checks, `2F407` calls `27D20` with driver filename at config `+0x24` and four config words at `+0x0A..+0x10` **only when** word `+0x14` is nonzero. `2F426` calls `282BC` with filename at `+0x16` and words `+0x00..+0x06`. In the supplied SOUND.CFG these correspond respectively to the digital-sound and MT-32 music driver selections.

Therefore the verified **attempted** order in this entry path is:

```text
30BB4 game IRQ-8 hook (30708)
  -> 27D20 digital driver, conditional on SOUND.CFG +0x14
  -> 282BC music driver
  -> driver-managed callback registration, if requested
```

The timer code does not prove that both driver loads always succeed or that every supported hardware/launcher configuration reaches the same post-initialization state. Neither driver filename should be taken as a recovered timer frequency.

## 10. Driver registration activates a separate callback layer

The digital loader `27D20` invokes scheduler-table initializer `36254` (`27DFE`), then asks `365DF` for a driver handle (`27E04`); the music loader `282BC` likewise obtains a handle (`283AD`) but initializes the shared tables only when `[0xEE40]==0` (`283A7`). Both loaders eventually invoke install dispatch `366C5`, at `27EB6` and `2847D` respectively.

The dispatch at `366C5..36779` calls the driver's resolved interface function, and **only if a non-sentinel callback address is supplied** does `3670C` register it through `3632D`. A successful registration returns a callback-slot handle and the driver supplies its desired interval via `3654E`. Thus the additional IRQ-8 installer `360E8` is reached through a concrete **driver-callback path**, not an unconditional gameplay startup call.

On the first registration, `3632D` clears the timer state through `3609B`, captures the then-current interrupt-8 vector, installs `35FC6`, and reserves timer **slot 16** with callback `3607B`, initial interval `0xD68D`, and active state 2. User/device callbacks occupy slots 0..15. Handler `35FC6` services due callbacks and uses the special slot-16 callback to transfer to the previously saved vector. Where the game hook was already installed, this provides the bridge back to `30708` and ultimately the prior system handler.

Driver release at `3678A` unregisters the owned timer slot; when the last slot is removed, `363DD` restores the prior interrupt vector using `36137` and returns the PIT divisor to zero using `3616F(0)`. This is a **conditional, counted teardown**, not an unconditional shutdown of the game clock.

## 11. Interval units, quantization and cadence boundary

At `3654E` the scheduler converts the frequency argument by unsigned division:

```c
period = 1000000u / frequency; // original: 0xF4240 / input
```

A separate entry `36581` converts a different time scale by multiplying the argument by `10000` then dividing by `0x2E9C`; zero is replaced by the reserved `0xD68D` value. `361CE` chooses the **smallest nonempty interval** across slots, changes the PIT divisor only when this minimum differs from `[0x10362]`, and resets all accumulated per-slot counts on a change. `3619D` converts the chosen interval into the PIT low/high divisor specified in §6.

The reciprocal `1000000/frequency` and reserved `0xD68D` are consistent with an intended microsecond-oriented interval scale and a conventional approximately 18.2-Hz old-timer callback. **That is a bounded interpretation of the arithmetic, not a measured cadence:** integer division, callback accumulation, PIT phase and audio-driver configuration can all affect observed timing. In particular, do not convert an ANM wait count into milliseconds by assuming it equals a newly programmed high-rate audio callback interval. The game-facing `0xF58C` counter only advances when its own handler `30708` runs.

## 12. Gameplay timer consumers beyond cinematics

The game hook's `19C80` callback supports a deferred event at `[0xA790]`. The corresponding producer at `19DDC..19E01` sets enabled byte `[0xA79C]`, captures two arguments in `[0xA794/0xA798]`, and schedules `deadline = 30700() + uint8[0xC025]`. On an IRQ tick with `now > deadline` (strict unsigned comparison), `19C80` calls `22CE0(arg1,arg2,2)` and clears the enabled byte. A competing path `19E08..19E32` may dispatch the stored event earlier with mode 8.

Separately, when `[0xA7E8]` is nonzero, `19C80` increments dword `[0xA7E4]` on each game-clock invocation. Routine `1E75C`, called from `2022B`, measures this **second** counter against two baselines `[0xBF50/0xBF54]`. At an interval difference of at least `0x438`, it enters a repeating party-status/HP regeneration pass, compares current versus maximum HP for four `0x248`-byte character records, and increments eligible HP by one. An additional branch checks `0x21C` ticks for a different periodic party-state update.

The underlying game-time counter can also be explicitly assigned by `1E74C`; it is therefore not merely an alias for `0xF58C`. Preserve both raw counters, their independent reset/set operations, their pause/enable gates, and their callers before choosing a host-side clock abstraction.

## 13. Correction C-38 — wait-flag 4 and unsigned deadline wrapping

Chapter §4 previously left flag `0x04` open because `2E614` contains a later conditional return `0x1F0` (`2E6A1..2E6B7`). Direct control-flow examination constrains this further: on entry `2E63A` tests that bit and immediately **clears it** with `and bl,0xFB` at `2E63F` before the servicing loop. The flag-1/flag-2 early-return paths do not fall through after changing `EBX`. Consequently the later `test bl,0x04` cannot normally succeed from a caller-supplied flag-4 bit. Preserve the encoded dead branch; do not advertise `0x1F0` as an observed reachable early-return result.

The deadline comparison at `2E6BD` uses **unsigned** `JB` after computing an absolute 32-bit deadline. There is no proved modular-difference correction at this comparison; a native implementation must retain this behavior or document a deliberate later compatibility deviation. Tests should cover near-wrap anchors instead of silently substituting a mathematically cleaner timer.

**Verification:** source CODE.2 SHA-256 matched the certified original, and 28 targeted byte/call assertions for these paths passed. This verifies the cited machine bytes and static edges, **not** timer hardware measured in execution.

## 14. Remaining unknowns

The complete dynamic initialization order for all original audio drivers; timer scheduling under driver failure/reinitialization; platform-dependent ISR latency; precise physical elapsed time per cinematic tick; and behavior across wraparound at real runtime remain **OPEN**. Do not infer these solely from nominal IBM PC PIT constants or a modern emulator.

**Next:** trace exact save/load ownership for the gameplay-time counter and its regeneration baselines; investigate driver callback activation for the supplied SOUND.CFG under executable observation; then audit additional frame/update loop callers and capture timing regression traces.

## Source ledger

- [CODE.2 certification](../Code2/FINAL_CERTIFICATION.md)
- [CODE.2 faithful reconstruction](../Code2/Code2.cpp)
- [Chapter 16 — ANM timeline](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md)
- [Chapter 15 — audio driver boundary](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md)
- [Chapter 07 — UI/input](07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)
- [Runtime audio dependency audit](../Runtime/AUDIO_INTERFACE_DEPENDENCY_AUDIT.md)
