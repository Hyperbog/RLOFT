# SAVE.DAT late character-record state (`+0x1B6..+0x247`)

Status: **executable reconstruction normalized to corrected runtime character base DGROUP `0x218E0`.** The temporary spell-created hand subsystem is semantically identified. The terminal runtime-addressed region and final eight serialized bytes are kept distinct pending independent serializer audit.

## Corrected late-state map

```text
+0x19E..+0x1A9   uint32 timed_slot_deadline[3]
+0x1AA..+0x1B5   uint32 timed_slot_aux[3]
+0x1B6..+0x1B9   Stone Giant Gauntlets displaced/effective Strength auxiliary
+0x1BA..+0x1BD   serialized dword; no direct CODE.2 reference found
+0x1BE..+0x1C1   reversible modifier for primary effect index 23
+0x1C2..+0x1C5   uint32 temporary_hand_effect_bits
+0x1C6..+0x1E9   uint32 temporary_hand_effect_expiry[9]
+0x1EA..+0x1ED   consumable additive/protection pool
+0x1EE..+0x1F1   recurring Strength-recovery deadline
+0x1F2..+0x1F5   accumulated state associated with primary effect index 6
+0x1F6..+0x23F   runtime-addressed tail with no direct CODE.2 references found
+0x240..+0x247   final eight serialized bytes; serializer/runtime role requires independent audit
```

## Stone Giant Gauntlets state

Runtime `0x21A96` maps to corrected record offset `+0x1B6`:

```text
0x21A96 - 0x218E0 = 0x1B6
```

CODE.2 physical `0x4CF50..0x4D048` ties this dword to OBJECTS.DAT object `0x54`, `STONE GIANT GAUNTLETS`. Equipment-transition logic saves/restores displaced effective Strength here while the gauntlets force effective Strength to 20. Preserve the full dword.

The bracer/gauntlet equipment slot is corrected item slot 20 at record `+0x068`, runtime `0x21948`. Strength base/current are corrected record `+0x032/+0x033`.

## `+0x1BA..+0x1BD`: unreferenced serialized dword

Runtime `0x21A9A` maps to `+0x1BA`. Direct-address audit found no executable operand referencing it or an interior byte. Preserve it as opaque serialized state; absence of literal references does not exclude computed access.

## `+0x1BE..+0x1C1`: primary-effect-23 reversible modifier

Runtime `0x21A9E` maps to `+0x1BE`. The primary-effect expiration handler reads this dword for effect index 23, negates it, and passes it to Strength adjustment helper `0x4CF50`. Producer physical `0x61FEB` stores the actual return value of an adjustment before scheduling the effect.

## Temporary spell-created hand effects

Runtime `0x21AA2` maps to `+0x1C2`:

```c
uint32_t temporary_hand_effect_bits;      // +0x1C2
uint32_t temporary_hand_effect_expiry[9]; // +0x1C6..+0x1E9
```

Maintenance physical `0x43860..0x438EE` iterates indexes 0..8, expires deadlines, clears bit/timer state, and calls helper `0x435E8(character,index)`. Producer `0x44A30..0x44A62` ORs the mask and stores the corresponding absolute deadline.

The executable table maps indexes exactly to temporary hand objects:

```text
0  0x007E  CHILL TOUCH
1  0x007F  VAMPIRIC TOUCH
2  0x0080  CLAWS OF THE UMBER HULK
3  0x0081  CAUSE LIGHT WOUNDS
4  0x0082  CAUSE SERIOUS WOUNDS
5  0x0083  CAUSE CRITICAL WOUNDS
6  0x0084  HARM
7  0x0085  FLAME BLADE
8  0x0086  LICH TOUCH
```

Expiration removes the matching temporary object from either hand if still equipped.

## `+0x1EA..+0x1ED`: consumable additive/protection pool

Runtime `0x21ACA` maps to `+0x1EA`. Producer physical `0x45401..0x45417` adds a generated positive value. Damage/value processing at `0x47CBC..0x47CE5` consumes the pool before continuing, either exhausting the pool or reducing the incoming quantity to zero. Exact player-facing effect identity remains unresolved.

## `+0x1EE..+0x1F1`: recurring Strength-recovery deadline

Runtime `0x21ACE` maps to `+0x1EE`. At expiry the engine applies `+1` through the Strength adjustment routine, clears the deadline, compares corrected current Strength `+0x033` against base Strength `+0x032`, and reschedules if further recovery is needed.

## `+0x1F2..+0x1F5`: accumulated effect-6 state

Runtime `0x21AD2` maps to `+0x1F2`. Producer code adds a generated positive quantity; primary effect index 6 expiration decrements it. The exact player-facing identity remains pending effect-resource closure.

## Terminal record region

The previous documentation classified `+0x1FE..+0x247` as one 74-byte runtime tail. That conclusion depended on the obsolete runtime base `0x218D8`.

With the corrected base, runtime range `0x21AD6..0x21B1F` maps to:

```text
+0x1F6..+0x23F   74 bytes
```

The existing exhaustive literal-address audit found no direct CODE.2 references in that runtime interval. Populated quick-start records store zeros in the audited area, while unused records retain their unused-record fill pattern. Preserve these bytes losslessly.

However, the fixed serialized record continues for eight more bytes:

```text
+0x240..+0x247
```

Those final eight bytes are **not covered by the previous runtime-address scan** and must not be silently folded into the same conclusion. They require an independent save/load serializer and fixture audit before being classified as reserved, runtime state, or non-runtime serialization data.

## Reconstruction requirements

- Preserve all late-state dwords and absolute deadlines exactly.
- Keep the nine-entry temporary-hand subsystem separate from the 26 primary effects.
- Preserve the exact index/object mapping `0x7E..0x86`.
- Preserve the opaque `+0x1BA` dword and `+0x1F6..+0x23F` region.
- Do not repurpose `+0x240..+0x247`; its role remains explicitly open.
- Derive serialized offsets from runtime addresses using corrected base `0x218E0`.