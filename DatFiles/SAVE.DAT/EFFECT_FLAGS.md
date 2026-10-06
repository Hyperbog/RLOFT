# SAVE.DAT timed character effects

Status: **executable-certified structure, normalized to runtime character base DGROUP `0x218E0`; individual gameplay names remain under reconstruction.**

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

Thus effect 0 uses `T[588]` and effect 25 uses `T[613]`. Decoding those records remains the preferred route to exact player-facing effect identities.

## Effect-specific expiration side effects

Corrected serialized offsets below are derived from runtime addresses using `record_offset = runtime_address - 0x218E0`:

```text
index  1  -> invokes a shared external-state update
index  6  -> decrements character dword at runtime 0x21AD2 / record +0x1F2
index 10  -> clears character dword at runtime 0x21ACA / record +0x1EA
index 11  -> expiration branch participates in Strength-effect cleanup; see producer audit before assigning identity
index 12  -> reverses the Strength adjustment stored at runtime 0x21A03 / record +0x123 through helper 0x4CF50
index 13  -> subtracts low byte of runtime 0x21A07 / record +0x127 from Dexterity current/effective at record +0x35
index 17  -> decrements global dword 0xFD08
index 18  -> decrements global word 0xF03C
index 20  -> decrements global word 0x6DAE and dirties all four party display slots
index 21  -> clears the corresponding shared effect state across all four party records and dirties all four display slots
index 23  -> reverses the modifier at runtime 0x21A9E / record +0x1BE through helper 0x4CF50
index 25  -> decrements global word 0xF03E
```

The earlier documentation associated the `0x21A03` Strength modifier with record `+0x12B`; that was the obsolete `0x218D8` base. Its corrected serialized offset is `+0x123`, while its executable runtime address is unchanged.

Indexes not listed still receive common bit/timer/message expiration processing.

## Producer-side evidence

The generic effect-application path at physical `0x45394` ORs a caller-supplied mask into `active_effect_bits`. When duration is nonzero it computes an absolute expiration and stores it in the timer array selected by the same effect/mask path.

A separate direct producer at physical `0x46A61` sets bit `0x00000002` and stores a computed absolute timer at runtime `0x21A1A`, independently confirming bit-index/timer-index pairing.

Producer-side audits also establish primary effect index 12 as the effect using the reversible Strength adjustment at runtime `0x21A03` / record `+0x123`, and index 23 as the effect using runtime `0x21A9E` / record `+0x1BE`.

## Reconstruction rule

A native port must preserve all 26 timers exactly, including inactive or unusual values. Cancellation/expiration must synchronize the bit and timer and reproduce effect-specific reversal behavior. Runtime addresses are the executable anchors; all serialized offsets in this document use the corrected character base `0x218E0`.