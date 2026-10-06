# SAVE.DAT late character-record state (`+0x1BE..+0x247`)

Status: **active executable reconstruction. The nine-entry secondary timer subsystem is identified as temporary spell-created hand-attack/item state; the terminal 74 bytes have now been bounded as serialized but directly unreferenced state in the audited CODE.2 image.**

This note continues the character-record reconstruction after the 26-entry timed-effect table and the three paired timed slots documented in `EFFECT_FLAGS.md` / `TIMED_SLOTS.md`.

## `+0x1BE`: Stone Giant Gauntlets saved Strength value

Runtime `0x21A96` equals record offset `+0x1BE`:

```text
0x21A96 - 0x218D8 = 0x1BE
```

CODE.2 physical `0x4CF50..0x4D048` ties this dword directly to equipped object ID `0x54`. OBJECTS.DAT record `0x54` is `STONE GIANT GAUNTLETS`.

When item slot 20 (`record +0x70`, runtime `0x21948`) contains object `0x54`, the ordinary current/effective Strength byte at `+0x3B` is not used directly. Instead:

- Strength-adjustment routine `0x4CF50` reads/modifies the dword at `+0x1BE`, clamped to 20;
- Strength getter `0x4CFBC` returns `+0x1BE` while the gauntlets are equipped;
- equipment-transition helper `0x4CFE0` copies the low byte of `+0x1BE` back to current Strength when object `0x54` is removed;
- when object `0x54` is equipped, it saves the previous current Strength into `+0x1BE` and writes effective current Strength `20` into `+0x3B`.

Thus `+0x1BE` is persistent auxiliary state for the Stone Giant Gauntlets Strength override. Preserve the full dword exactly even though the transfer helper uses its low byte.

## `+0x1C2..+0x1C5`: currently unreferenced dword

Runtime `0x21A9A` corresponds to record `+0x1C2`. An exhaustive direct-address scan of the audited CODE.2 disassembly finds no instruction operand referencing `0x21A9A` or an interior byte of this dword. This is materially different from the adjacent `+0x1BE` and `+0x1C6` fields, both of which have explicit executable consumers.

The field remains part of the serialized `0x248` record and must be preserved. Current evidence supports **unreferenced/reserved persistent dword**, not a gameplay semantic name. Computed or indirect access cannot be excluded solely from absence of a literal address, so this classification is intentionally conservative.

## `+0x1C6`: reversible effect modifier

Runtime `0x21A9E` equals record `+0x1C6`. The 26-effect expiration handler reads this dword for effect index 23, negates it, and passes it to the same character-adjustment helper at physical `0x4CF50`.

A producer at physical `0x61FEB` stores the return value of a `0x4CF50(character, 20)` adjustment into `+0x1C6` before scheduling associated state. Therefore this field stores the actual reversible adjustment applied by that effect, rather than a generic timer.

The exact player-facing effect name remains pending resource/caller identification.

## `+0x1CA`: temporary hand-effect bitset

Runtime `0x21AA2` equals record `+0x1CA`. CODE.2 treats it as a bitset with exactly **nine** timer entries:

```text
+0x1CA..+0x1CD   uint32 temporary_hand_effect_bits
+0x1CE..+0x1F1   uint32 temporary_hand_effect_expiry[9]
```

Maintenance routine physical `0x43860..0x438EE` checks the bitset, iterates indexes 0 through 8, compares each nonzero absolute deadline against current game time, clears the matured bit/timer, and calls expiration helper `0x435E8(character,index)`.

Producer physical `0x44A30..0x44A62` independently confirms the pairing: it ORs a caller-provided mask into `+0x1CA` and stores an absolute future game-clock value at `+0x1CE + index*4`.

### Exact index-to-object identity

The expiration helper resolves the gameplay family. For the expiring index it reads a signed 16-bit object ID from executable table `0x67CA[index]`, compares that ID against both hand slots (`item[14]` and `item[15]`), and removes the matching hand object through `0x5F9C0`.

The nine-entry table is physically present in CODE.2 at file `0x7C6EE` and is the contiguous sequence:

```text
0x007E 0x007F 0x0080 0x0081 0x0082 0x0083 0x0084 0x0085 0x0086
```

Cross-checking those IDs against OBJECTS.DAT gives the exact secondary-effect identities:

```text
index  bit mask    object   OBJECTS.DAT name
0      0x000001    0x007E   CHILL TOUCH
1      0x000002    0x007F   VAMPIRIC TOUCH
2      0x000004    0x0080   CLAWS OF THE UMBER HULK
3      0x000008    0x0081   CAUSE LIGHT WOUNDS
4      0x000010    0x0082   CAUSE SERIOUS WOUNDS
5      0x000020    0x0083   CAUSE CRITICAL WOUNDS
6      0x000040    0x0084   HARM
7      0x000080    0x0085   FLAME BLADE
8      0x000100    0x0086   LICH TOUCH
```

This closes the semantic identity of the entire nine-entry subsystem: these timers track **temporary spell/effect-created hand attack objects**. When the associated duration expires, the engine removes the corresponding temporary object if it is still present in either hand.

## `+0x1F2`: consumable additive pool/state

Runtime `0x21ACA` equals record `+0x1F2`. Producer physical `0x45401..0x45417` adds a generated positive value into this dword. A downstream value/damage-processing path at physical `0x47CBC..0x47CE5` consumes this field before continuing: if the incoming amount is at least the stored value, it subtracts the stored value from the incoming amount and clears the field; otherwise it subtracts the incoming amount from this field and reduces the incoming amount to zero.

A later computation at `0x484AB..0x484C5` adds `+0x1F2` to a signed 16-bit character value and another persistent dword before calling the same downstream processing routine. The primary effect-expiration handler also clears this field for selected effect cases.

This establishes a **persistent consumable additive pool/state** with depletion semantics. It is stronger than the previous generic `additive state` label, but the player-facing effect name remains unresolved until the producer's effect ID/resource identity is certified.

## `+0x1F6`: recurring Strength-recovery deadline

Runtime `0x21ACE` equals record `+0x1F6`. Maintenance code at physical `0x438EE..0x43954` proves the complete behavior:

1. if the dword is zero, no recovery event is pending;
2. once current game time reaches the stored deadline, the engine calls the certified Strength adjustment routine `0x4CF50(character,+1)`;
3. it clears the current deadline;
4. it compares effective/current Strength with the character's base Strength byte at `+0x3A`;
5. if they still differ, it schedules another deadline using current game time plus a randomized interval.

Another producer at physical `0x487C3` writes an absolute future game-clock value directly into this field.

This is therefore a persistent **recurring Strength-recovery deadline**: temporary Strength loss is repaired one point at a time, and another recovery event is scheduled until effective/current Strength again matches base Strength. This also independently reinforces the established base/current Strength pairing at `+0x3A/+0x3B`.

## `+0x1FA`: persistent counted effect state

Runtime `0x21AD2` equals record `+0x1FA`. Producer-side code at physical `0x453D9..0x453F1` generates a positive quantity and adds it into this dword. The primary effect-expiration handler decrements the field for effect index 6.

This certifies an accumulated/counting relationship to primary effect 6, but not yet its player-facing identity. Unlike `+0x1F2`, the currently traced expiration path decrements this field by one rather than clearing the entire value.

## `+0x1FE..+0x247`: serialized terminal reserved/unreferenced region

This final region is exactly 74 bytes long. Two independent observations now bound it:

1. **Fixture evidence:** every populated character record in the supplied quick-start SAVE.DAT stores all 74 bytes as zero. Unused active-party records retain their broader `0xFF` unused-record fill pattern instead, confirming that the zeros in populated records are deliberate initialized record contents rather than file-level padding.
2. **Executable evidence:** an exhaustive literal-address scan of the audited CODE.2 disassembly finds no direct operand references anywhere in runtime range `0x21AD6..0x21B1F`, which corresponds exactly to record `+0x1FE..+0x247`. The immediately preceding fields through `0x21AD2` do have direct consumers.

The save/load architecture nevertheless serializes the complete fixed `0x248` character image, so these bytes remain part of the persistent format.

The strongest safe classification is therefore:

```text
+0x1FE..+0x247   74-byte serialized reserved/unreferenced tail
```

This does **not** prove that every byte was intentionally designed as padding. A computed-address access not exposing a literal operand remains theoretically possible. For preservation and native-port work, retain these bytes losslessly and do not repurpose them without additional evidence.

## Current late-tail map

```text
+0x1A6..+0x1B1   timed_slot_deadline[3]
+0x1B2..+0x1BD   timed_slot_aux[3]
+0x1BE..+0x1C1   Stone Giant Gauntlets displaced/effective Strength auxiliary dword
+0x1C2..+0x1C5   serialized dword; no direct CODE.2 reference found
+0x1C6..+0x1C9   reversible modifier for primary effect index 23
+0x1CA..+0x1CD   uint32 temporary_hand_effect_bits
+0x1CE..+0x1F1   uint32 temporary_hand_effect_expiry[9]
+0x1F2..+0x1F5   consumable additive pool/state
+0x1F6..+0x1F9   recurring Strength-recovery deadline
+0x1FA..+0x1FD   accumulated/counting state for primary effect index 6
+0x1FE..+0x247   74-byte serialized reserved/unreferenced tail
```

## Reconstruction requirements

A native loader must preserve every field above losslessly. In particular:

- retain the Stone Giant Gauntlets displaced Strength state across save/load;
- preserve the temporary hand-effect mask and all nine absolute deadlines;
- preserve the exact association between secondary index and object ID `0x7E..0x86`;
- model `+0x1F6` as a recurring Strength-recovery deadline, separate from both effect timer arrays;
- preserve `+0x1C2` and the terminal 74-byte region even though no direct CODE.2 consumers are presently visible;
- never treat the terminal bytes as free extension space in a compatibility-preserving implementation.

## Next target

Return to the earlier unresolved character-record islands (`+0x112..+0x12E`, `+0x137..+0x138`) and synchronize the authoritative `CHARACTER_RECORD.md` with all post-inventory discoveries before assigning remaining primary-effect gameplay names.