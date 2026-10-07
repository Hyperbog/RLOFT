# SAVE.DAT late character-record state (`+0x1B6..+0x247`)

Status: **high-effort executable reconstruction normalized to corrected runtime character base DGROUP `0x218E0`.** The temporary spell-created hand subsystem is semantically identified; the complete terminal 82-byte interval is serialized/initialized runtime record state with no direct character-field reference in the audited CODE.2 image.

## Corrected late-state map

```text
+0x19E..+0x1A9   uint32 combat_feedback_deadline[3]
+0x1AA..+0x1B5   uint32 combat_feedback_damage[3]
+0x1B6..+0x1B9   Stone Giant Gauntlets displaced/effective Strength auxiliary
+0x1BA..+0x1BD   serialized dword; no direct CODE.2 reference found
+0x1BE..+0x1C1   Potion of Giant Strength reversible Strength modifier (primary effect 23)
+0x1C2..+0x1C5   uint32 temporary_hand_effect_bits
+0x1C6..+0x1E9   uint32 temporary_hand_effect_expiry[9]
+0x1EA..+0x1ED   Aid temporary-hit-point/protection pool (primary effect 11)
+0x1EE..+0x1F1   recurring Strength-recovery deadline
+0x1F2..+0x1F5   Stoneskin remaining/accumulated state (primary effect 6)
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

## Stone Giant Gauntlets state

Runtime `0x21A96` maps to record `+0x1B6`. CODE.2 physical `0x4CF50..0x4D048` ties this dword to OBJECTS.DAT object `0x54`, `STONE GIANT GAUNTLETS`. Equipment-transition logic saves/restores displaced effective Strength here while the gauntlets force effective Strength to 20. Preserve the full dword.

The bracer/gauntlet equipment slot is item slot 20 at record `+0x068`, runtime `0x21948`. Strength base/current are record `+0x032/+0x033`.

## `+0x1BA..+0x1BD`: unreferenced serialized dword

Runtime `0x21A9A` maps to `+0x1BA`. Direct-address audit found no executable operand referencing it or an interior byte. Preserve it as opaque serialized state; absence of literal references does not exclude computed access.

## `+0x1BE..+0x1C1`: Potion of Giant Strength reversible modifier

Runtime `0x21A9E` maps to `+0x1BE`. The primary-effect expiration handler reads this dword for effect index 23, negates it, and passes it to Strength adjustment helper `0x4CF50`. Producer physical `0x61FEB..0x62002` stores the applied adjustment, sets overall effect bit 23, and schedules timer index 23.

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

## `+0x1EA..+0x1ED`: Aid protection pool, effect 11

Runtime `0x21ACA` maps to `+0x1EA`. Producer physical `0x45401..0x45417` adds a generated positive quantity. Damage/value processing at `0x47CBC..0x47CE5` consumes the pool before continuing, either exhausting it or reducing the incoming quantity to zero.

A high-effort re-audit of the primary effect dispatch corrected an older transcription error: **primary effect index 11**, not index 10, clears this dword on expiration at physical `0x4378A`. Generic producer logic also associates mask `0x00000800` (bit/index 11) with additions to `0x21ACA`, independently confirming the lifecycle relationship.

Raw `T[599]` and the producer catalogue identify primary effect 11 as **Aid**. The field is therefore the persistent Aid temporary-hit-point/protection pool consumed before ordinary HP damage.

## `+0x1EE..+0x1F1`: recurring Strength-recovery deadline

Runtime `0x21ACE` maps to `+0x1EE`. At expiry the engine applies `+1` through the Strength adjustment routine, clears the deadline, compares current Strength `+0x033` against base Strength `+0x032`, and reschedules if further recovery is needed.

## `+0x1F2..+0x1F5`: Stoneskin accumulated/remaining state

Runtime `0x21AD2` maps to `+0x1F2`. Producer code adds a generated positive quantity for primary effect bit/index 6; expiration decrements this dword. The closed primary-effect catalogue and raw text identify index 6 as **Stoneskin**, so this is the serialized Stoneskin accumulated/remaining-state quantity.

## `+0x1F6..+0x247`: terminal reserved/unreferenced region

The corrected character base and fixed `0x248` stride place the complete final 82 bytes at runtime `0x21AD6..0x21B27`.

Evidence now covers the entire interval:

1. Direct serializer/load operations move `0x920 = 4*0x248` bytes beginning at runtime `0x218E0`, proving the party image base and stride.
2. An exhaustive direct-address audit found no character-field operands in runtime `0x21AD6..0x21B1F` (`+0x1F6..+0x23F`).
3. A follow-up literal scan of `0x21B20..0x21B27` found no character-field access. The only literal collision is `0x21B24`, whose context at physical `0x5CAA7` is a code jump-table base (`jmp *%cs:0x21b24(,%eax,4)`).
4. Character initialization at physical `0x51A04..0x51A3A` explicitly computes `0x21B28 - 0x219EB` and zeroes `[0x219EB, 0x21B28)`. This directly initializes every byte through runtime `0x21B27`; `0x21B28 - 0x218E0 = 0x248` exactly.
5. Fixture evidence: all 12 populated quick-start characters store zero across `+0x240..+0x247`; unused active-party records retain `0xFF` fill there. A lossless decoder therefore must preserve stored bytes rather than synthesizing one universal fill value.

Safe classification:

```text
+0x1F6..+0x247   82-byte serialized reserved/unreferenced terminal region
```

This does not prove that original source code declared the interval as padding; computed/indirect consumers remain theoretically possible. It does prove serialization membership, initialization membership, and absence of direct character-field references in the audited executable.

## Reconstruction requirements

- Preserve all late-state dwords and absolute deadlines exactly.
- Keep the nine-entry temporary-hand subsystem separate from the 26 primary effects.
- Preserve exact temporary-hand index/object mapping `0x7E..0x86`.
- Associate `+0x1EA` lifecycle with primary effect index 11, not 10.
- Preserve opaque `+0x1BA` and complete `+0x1F6..+0x247` terminal region.
- Do not repurpose terminal bytes as native extension space.
- Derive serialized offsets from runtime addresses using corrected base `0x218E0`.