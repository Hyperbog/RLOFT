# Part 9 — original CODE.2 1CD48 numbered-effect gate

**Objective:** complete faithful original-game code recreation, without modernized event behavior.

## Binary-proven dispatch rule

The supplied immutable CODE.2 original object's `1CD48..1CE65` implements an event dispatcher with *persistent event history* in DGROUP `22200`.

- `1CD48..1CD59`: load original event number; if not `-1`, **add 0x80**. If it is `-1`, do not add the event-index offset.
- `1CD5E..1CD69`: call `1C1DC` to query the persistent bit corresponding to `event_number+0x80`. A previously seen event skips the effect payload.
- `1CD6F..1CE2B`: otherwise run the selected original effect branch. The Level22 mist prelude `222A6..222BD` passes **number 2, effect resource 0x21, variant 7**.
- `1CE2B..1CE63`: afterward, if this was numbered, calculate the original bit position from the shifted event number using quotient/remainder division by 32 and set that bit in DGROUP `22200`. This is performed even if the corresponding effect was already marked.
- Original `1C1DC:1C1E2..1C219` queries the same original bit without modifying other flags.

Therefore **the Level22 original mist effect is gated by persistent event bit `0x82`**, which is byte `0x10`, mask `0x04` of the existing 32-byte session event vector. This follows mechanically from original instruction arguments, not from a modern one-shot rule.

## Recreated code

`OriginalNumberedEffectFlags::inspect_1cd48(flags,event)` preserves whether the effect should actually be dispatched and the exact selected bit; `finish_1cd48` applies the original post-dispatch bit write. **No flags change merely on pickup/preview:** this new model is isolated until the live original audiovisual event dispatcher is implemented, since the source sets the flag after the original call chain.

Unnumbered `-1` bypasses the bit query and leaves the history vector untouched. Only the representable original 0..127 numbered events are currently supported in the 256-bit native vector. Out-of-range aliases into other DOS memory are deliberately unresolved, not invented.

The registered `original_numbered_effect_flags_tests` checks first-run/event repeat suppression, original 0x82 bit, event10 bit0x8A, preserved surrounding bits, -1 bypass and original vector boundary. **Unit tests are authored but unexecuted** until a user-authorized build.

## Required next reconstruction

Faithfully wire CODE.2 `1C924/1C77C` effect actions, `CF4C/CF4E` phase and `31F94` clock-dependent wait, original RES/VGA/cinematic rendering, 21F70 camera staging, exact level-23 switch at frame20, and the three F540 post-frame removals. Invoke the 1CD48 flag gate **at the original source point**, not early when last relic pickup occurs.

Current scorecard **49.8% provisional engineering readiness**, **20 systems/122 subsystems**, DOS current-HEAD acceptance **0%**. Registered CTests after this checkpoint: **176 total / 19 Quick**. Last manually authorized Windows Full run #647 verified **172/172** on older `fb4a0145`. No build was requested for this code.
