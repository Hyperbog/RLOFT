# SAVE.DAT late character-record state (`+0x1BE..+0x247`)

Status: **active executable reconstruction. Several late-record fields are now structurally or semantically certified.**

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

A faithful native implementation should model the transition, not merely recompute Strength from inventory after load, because the original record preserves the displaced value.

## `+0x1C6`: reversible effect modifier

Runtime `0x21A9E` equals record `+0x1C6`. The 26-effect expiration handler reads this dword for effect index 23, negates it, and passes it to the same character-adjustment helper at physical `0x4CF50`.

A producer at physical `0x61FEB` stores the return value of a `0x4CF50(character, 20)` adjustment into `+0x1C6` before scheduling associated state. Therefore this field stores the actual reversible adjustment applied by that effect, rather than a generic timer.

The exact player-facing effect name remains pending resource/caller identification.

## `+0x1CA`: second active-effect bitset

Runtime `0x21AA2` equals record `+0x1CA`. CODE.2 treats it as a bitset with **nine** timer entries:

```text
+0x1CA..+0x1CD   uint32 secondary_effect_bits
+0x1CE..+0x1F1   uint32 secondary_expiry[9]
```

Maintenance routine physical `0x43860..0x438EE`:

1. checks whether `secondary_effect_bits` is nonzero;
2. iterates indexes 0 through 8;
3. reads `secondary_expiry[index]` at runtime `0x21AA6 + index*4`;
4. compares a nonzero deadline with current game time;
5. clears the corresponding bit by XOR;
6. clears that timer dword;
7. calls expiration/action helper `0x435E8(character,index)`.

Producer physical `0x44A30..0x44A62` independently confirms the pairing: it ORs a caller-provided mask into `+0x1CA` and stores an absolute future game-clock value at `+0x1CE + index*4`.

This is therefore a second persistent timed-effect subsystem, distinct from the 26-entry `+0x13A/+0x13E` system.

Portable representation:

```c
uint32_t secondary_effect_bits;   // +0x1CA
uint32_t secondary_expiry[9];     // +0x1CE
```

Do not merge these nine states into the primary 26-effect table: the original executable uses separate masks, separate timer arrays, and a separate expiration helper.

## `+0x1F2`: persistent additive state

Runtime `0x21ACA` equals record `+0x1F2`. It is read and modified as a signed/additive dword by multiple gameplay paths. The primary 26-effect expiration handler explicitly clears it for effect indexes 10 and 11. Other paths add it into computed values and increment/decrement it.

This certifies it as persistent gameplay state, not padding. Its exact gameplay label remains unresolved.

## `+0x1F6`: independent deadline/state timestamp

Runtime `0x21ACE` equals record `+0x1F6`. Maintenance code at physical `0x438EE..0x43954` compares this nonzero dword against current game time, clears it when due, applies a character adjustment, then may schedule a new future value. Another producer at `0x487C3` writes it directly.

It is therefore an independent persistent deadline/state timestamp outside both the 26-entry and 9-entry effect arrays.

## `+0x1FA`: persistent counter

Runtime `0x21AD2` equals record `+0x1FA`. The primary effect expiration handler decrements it for effect index 6; producer-side code at physical `0x453F1` increments/adds to it. This is persistent counted state associated with that effect family, not padding.

## Current late-tail map

```text
+0x1A6..+0x1B1   timed_slot_deadline[3]
+0x1B2..+0x1BD   timed_slot_aux[3]
+0x1BE..+0x1C1   Stone Giant Gauntlets displaced/effective Strength auxiliary dword
+0x1C2..+0x1C5   unresolved
+0x1C6..+0x1C9   reversible modifier for primary effect index 23
+0x1CA..+0x1CD   uint32 secondary_effect_bits
+0x1CE..+0x1F1   uint32 secondary_expiry[9]
+0x1F2..+0x1F5   persistent additive effect/gameplay state
+0x1F6..+0x1F9   independent deadline/state timestamp
+0x1FA..+0x1FD   persistent effect-associated counter
+0x1FE..+0x247   unresolved
```

## Reconstruction requirements

A native loader must preserve every field above losslessly. In particular:

- retain the Stone Giant Gauntlets displaced Strength state across save/load;
- preserve the secondary nine-bit effect mask and all nine absolute deadlines;
- do not fold the independent `+0x1F6` deadline into either timer array;
- preserve unresolved bytes unchanged until their executable consumers are certified.

## Next target

Trace expiration helper `0x435E8` and the producers of `+0x1CA` to assign gameplay identities to the nine secondary effects, then continue through `+0x1FE..+0x247`.