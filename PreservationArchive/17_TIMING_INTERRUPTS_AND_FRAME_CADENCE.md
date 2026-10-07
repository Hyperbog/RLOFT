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

## 8. Remaining unknowns

The complete dynamic initialization order for all original audio drivers; timer scheduling under driver failure/reinitialization; platform-dependent ISR latency; precise physical elapsed time per cinematic tick; and behavior across wraparound at real runtime remain **OPEN**. Do not infer these solely from nominal IBM PC PIT constants or a modern emulator.

**Next:** resolve timer registration and chaining along the actual default audio configuration, then enumerate the other tick-controlled gameplay loops and correlate counter ownership with the save/world state architecture.

## Source ledger

- [CODE.2 certification](../Code2/FINAL_CERTIFICATION.md)
- [CODE.2 faithful reconstruction](../Code2/Code2.cpp)
- [Chapter 16 — ANM timeline](16_ANM_CINEMATIC_ENGINE_AND_TIMELINE.md)
- [Chapter 15 — audio driver boundary](15_AUDIO_MUSIC_SFX_AND_DRIVER_INTERFACE.md)
- [Chapter 07 — UI/input](07_INPUT_UI_MENU_AND_INVENTORY_ARCHITECTURE.md)
- [Runtime audio dependency audit](../Runtime/AUDIO_INTERFACE_DEPENDENCY_AUDIT.md)
