# SAVE.DAT post-effect timer state (`+0x1A6..`)

Status: **executable-certified structural reconstruction; exact gameplay label pending.**

The region immediately following the 26 timed-effect expiry dwords is not opaque padding. CODE.2 accesses the first twelve bytes as three independently indexed 32-bit timestamps, with a parallel three-dword auxiliary array twelve bytes later.

## Serialized/runtime mapping

The character runtime base is `0x218D8`, so:

```text
runtime 0x21A7E = record +0x1A6
runtime 0x21A82 = record +0x1AA
runtime 0x21A86 = record +0x1AE
runtime 0x21A8A = record +0x1B2
runtime 0x21A8E = record +0x1B6
runtime 0x21A92 = record +0x1BA
```

This yields two parallel three-entry dword arrays:

```text
+0x1A6..+0x1B1  uint32 timed_slot_deadline[3]
+0x1B2..+0x1BD  uint32 timed_slot_aux[3]
```

The names are intentionally structural. The exact player-facing system using these three slots is not yet canonicalized.

## Producer evidence

Physical CODE.2 `0x49959..0x49980` obtains the current game clock through `0x54BE8`, computes a future value, and writes it through an index scaled by four:

```text
record + 0x1A6 + slot*4 = future timestamp
record + 0x1B2 + slot*4 = associated value
```

The index is therefore a true three-slot selector rather than byte-oriented state. The surrounding routine derives the duration from the acting object/attack data and applies additional duration reduction when character effect bits `0x00000010` or `0x01000000` are active. That establishes interaction with combat/effect state but does not by itself justify a disease, poison, recovery, or cooldown label.

## Initialization

Character initialization at physical `0x51AAE..0x51ACE` explicitly zeros all three deadline dwords:

```text
0x21A7E = 0
0x21A82 = 0
0x21A86 = 0
```

This independently proves the three-entry width.

## Expiration sweep

Physical `0x593C4..0x59456` obtains the current game clock and, for every active party character, walks exactly three dwords beginning at runtime `0x21A7E`. For each nonzero entry whose timestamp has matured, it:

1. marks that character for UI/state refresh through `0xCF5A[character]`;
2. clears the expired dword to zero.

Equivalent structural behavior:

```c
for (unsigned character = 0; character < 4; ++character) {
    for (unsigned slot = 0; slot < 3; ++slot) {
        uint32_t *deadline = &party[character].timed_slot_deadline[slot];
        if (*deadline != 0 && now >= *deadline) {
            character_refresh[character] = 1;
            *deadline = 0;
        }
    }
}
```

The sweep does not clear the parallel auxiliary values at `+0x1B2`; those values are consumed elsewhere.

## Direct consumer

Physical `0x59C5B..0x59CC8` selects one of the three deadline slots, checks it against current time, clears an already-past value, and otherwise consumes the parallel auxiliary dword at `record + 0x1B2 + slot*4` while rendering/processing the still-active timed state.

This confirms the one-to-one pairing:

```text
deadline[0] <-> aux[0]
deadline[1] <-> aux[1]
deadline[2] <-> aux[2]
```

## Reconstruction rule

A native SAVE decoder should preserve at least:

```c
uint32_t timed_slot_deadline[3]; // +0x1A6
uint32_t timed_slot_aux[3];      // +0x1B2
```

Treat a zero deadline as inactive. Nonzero values are absolute game-clock deadlines, not durations stored relative to save time.

Do not merge this subsystem with `active_effect_bits` / `effect_expiry[26]` at `+0x13A..+0x1A5`. They are adjacent persistent timer systems with different indexing and consumers.

## Next target

Trace the producer at physical `0x498xx..0x499A5` back to its object/combat caller and identify what the three selector values represent. Then continue through the remaining post-effect fields beginning at `+0x1BE`.