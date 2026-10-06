# SAVE.DAT timed character effects

Status: **high-effort executable-audited structure, normalized to runtime character base DGROUP `0x218E0`; individual player-facing effect names remain under reconstruction.**

Each `0x248` character record contains a persistent 26-entry timed-effect subsystem:

```text
+0x132..+0x135  uint32 active_effect_bits
+0x136..+0x19D  uint32 effect_expiry[26]
```

For effect index `i` in `0..25`, bit `i` corresponds to `effect_expiry[i]`.

## Timer scan

CODE.2 physical `0x43957..0x43988` tests runtime `0x21A12` and walks effect indexes. For an active effect with a positive expiry dword, the engine compares the stored value with current game time and invokes physical `0x43640` when the expiry is reached.

The expiration handler removes `1 << effect_index` from runtime `0x21A12` and zeros the corresponding timer at runtime `0x21A16 + effect_index*4`.

```c
uint32_t active_effect_bits;  // record +0x132
uint32_t effect_expiry[26];   // record +0x136
```

## Expiration text resources

The handler maps the effect index directly to the common `T` archive:

```text
expiration message T index = 0x24C + effect_index
```

Thus effect 0 uses `T[588]` and effect 25 uses `T[613]`. Decoding those records remains the preferred route to exact player-facing identities.

## Re-audited expiration dispatch

A high-effort instruction-by-instruction recheck of physical `0x436EF..0x4385A` corrected an earlier index-10/index-11 transcription error. The executable dispatch is:

```text
index  1  -> invokes shared external-state update via 0x6CE5C(0xF91C)
index  6  -> decrements runtime 0x21AD2 / record +0x1F2
index 10  -> invokes the same shared external-state update as index 1
index 11  -> clears runtime 0x21ACA / record +0x1EA
index 12  -> reverses Strength adjustment at runtime 0x21A03 / record +0x123 through 0x4CF50
index 13  -> subtracts low byte of runtime 0x21A07 / record +0x127 from current Dexterity at +0x035
index 17  -> decrements global dword 0xFD08
index 18  -> decrements global word 0xF03C
index 20  -> decrements global word 0x6DAE and dirties all four party display slots
index 21  -> clears the corresponding shared effect state across all four party records and dirties all four display slots
index 23  -> reverses modifier at runtime 0x21A9E / record +0x1BE through 0x4CF50
index 25  -> decrements global word 0xF03E
```

Indexes not listed still receive the common bit-clear, timer-clear, message, and refresh processing.

### Correction history

An earlier revision incorrectly assigned the `0x21ACA` clear to effect index 10 and described index 11 as Strength-related cleanup. Direct branch decoding proves instead that indexes 1 and 10 converge on the same external-state update at physical `0x4376C`, while index 11 branches to physical `0x4378A` and clears `0x21ACA`. That older interpretation is superseded.

The prior offset-base correction remains valid: runtime `0x21A03` is serialized `+0x123`, not the obsolete `+0x12B` derived from base `0x218D8`.

## Producer-side cross-checks

The generic application path around physical `0x45394` ORs a caller-supplied mask into `active_effect_bits`, computes the absolute expiry, and stores it in the matching timer slot.

Producer behavior independently confirms several dispatch identities:

- mask `0x00000040` / index 6 accumulates state at runtime `0x21AD2` / `+0x1F2`;
- mask `0x00000800` / index 11 accumulates state at runtime `0x21ACA` / `+0x1EA`, matching the index-11 expiration clear;
- physical `0x458B9..0x45940` sets overall bit 12, timer index 12, and stores the reversible Strength adjustment at `0x21A03` / `+0x123`;
- physical `0x45A49..0x45AD7` sets overall bit 13, timer index 13, stores the Dexterity adjustment at `0x21A07` / `+0x127`, and applies it to current Dexterity `0x21915` / `+0x035`;
- physical `0x61FEB..0x62002` stores a Strength adjustment at `0x21A9E` / `+0x1BE`, sets overall bit 23, and schedules timer index 23.

A separate producer at physical `0x46A61` sets bit `0x00000002` and the corresponding timer at runtime `0x21A1A`, independently confirming bit/timer index pairing.

## Reconstruction rule

A native port must preserve all 26 timers exactly, including inactive or unusual values. Cancellation and expiration must synchronize the bit and timer and reproduce index-specific reversal/cleanup behavior. Runtime addresses are the executable anchors; all serialized offsets here use corrected character base `0x218E0`.