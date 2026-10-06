# SAVE.DAT timed character effects

Status: **executable-certified structure; individual gameplay names remain under reconstruction.**

Each `0x248` character record contains a persistent 26-entry timed-effect subsystem:

```text
+0x13A..+0x13D  uint32 active_effect_bits
+0x13E..+0x1A5  uint32 effect_expiry[26]
```

For effect index `i` in `0..25`, bit `i` of `active_effect_bits` corresponds to `effect_expiry[i]`.

## Timer scan

CODE.2 physical `0x43957..0x43988` first tests the active-effect dword and then walks effect indexes. For an active effect with a positive expiry dword, the engine compares the stored value with the current game-time value and invokes physical `0x43640` when the expiry is reached.

The expiration handler computes `1 << effect_index`, XORs that mask out of runtime `0x21A12`, and zeros the corresponding timer at runtime `0x21A16 + effect_index*4`.

Portable model:

```c
uint32_t active_effect_bits;  // record +0x13A
uint32_t effect_expiry[26];   // record +0x13E
```

## Expiration text resources

The same handler maps the effect index directly to the common `T` text archive:

```text
expiration message T index = 0x24C + effect_index
```

Therefore the 26 expiration messages occupy contiguous `T` records:

```text
effect 0  -> T[0x24C] = T[588]
effect 1  -> T[0x24D] = T[589]
...
effect 25 -> T[0x265] = T[613]
```

The handler loads the indexed text through the already certified common reader at physical `0x5728C`. Decoding `T[588..613]` will provide a direct independent route to the player-facing names/messages for the effect indexes.

## Effect-specific expiration side effects

The expiration routine is not merely a bit/timer clear. Several indexes undo additional state. The following index-level behavior is executable-certified without assigning speculative spell names:

```text
index  1  -> invokes a shared external-state update
index  6  -> decrements character dword at runtime +0x1FA (record +0x1FA)
index 10  -> clears character dword at runtime +0x1F2 (record +0x1F2)
index 11  -> reverses a value using record +0x12B through helper 0x4CF50
index 13  -> subtracts low byte of record +0x12F from current/effective Dexterity +0x3D
index 17  -> decrements global dword 0xFD08
index 18  -> decrements global word 0xF03C
index 20  -> decrements global word 0x6DAE and dirties all four party display slots
index 21  -> clears bit 0x20 in byte +2 of the effect dword for all four party records and dirties all four party display slots
index 23  -> reverses a value using record +0x1C6 through helper 0x4CF50
index 25  -> decrements global word 0xF03E
```

Indexes not listed here still receive the common bit/timer/message expiration processing; no additional side effect has yet been certified for them in this switch.

## Producer-side evidence

The generic effect-application path at physical `0x45394` ORs a caller-supplied mask into `active_effect_bits`. When duration is nonzero it computes an absolute expiration from current game time and stores it in the timer array selected by the same effect/mask path.

Some masks have extra application behavior. For example, mask `0x40` updates character state at record `+0x1FA`, and mask `0x800` updates state at record `+0x1F2`; their expiration-side counterparts are visible in the handler above. These relationships are structural evidence only until the corresponding effect names are tied to spell/action resources.

A separate direct producer at physical `0x46A61` sets bit `0x00000002` and stores a computed absolute timer at runtime `0x21A1A`, which is `effect_expiry[1]`. This independently confirms the bit-index/timer-index pairing.

## Resource dependency

The exact gameplay identities should be recovered by decoding the supplied `T` archive records `588..613` and cross-checking each message against its producer and expiration side effects. Do not assign AD&D spell names from mask behavior alone.

## Reconstruction rule

A native port should preserve all 26 timers exactly, including inactive or unusual values. The timer array is persistent serialized state, not a runtime cache. Effect cancellation/expiration must keep the bit and corresponding timer synchronized and must reproduce effect-specific reversal side effects where applicable.
