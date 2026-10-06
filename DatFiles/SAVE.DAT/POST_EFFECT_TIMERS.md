# SAVE.DAT post-effect timer state (`+0x19E..+0x1B5`)

Status: **executable-certified structural reconstruction normalized to runtime character base `0x218E0`; exact gameplay label pending.**

The region immediately following the 26 primary effect expiry dwords contains three independently indexed 32-bit deadlines and a parallel three-dword auxiliary array.

## Serialized/runtime mapping

```text
runtime 0x21A7E = record +0x19E
runtime 0x21A82 = record +0x1A2
runtime 0x21A86 = record +0x1A6
runtime 0x21A8A = record +0x1AA
runtime 0x21A8E = record +0x1AE
runtime 0x21A92 = record +0x1B2
```

Therefore:

```c
uint32_t timed_slot_deadline[3]; // +0x19E..+0x1A9
uint32_t timed_slot_aux[3];      // +0x1AA..+0x1B5
```

## Producer evidence

Physical CODE.2 `0x49959..0x49980` obtains the pausable gameplay tick counter, computes a future value, and writes through a slot index scaled by four. The same slot indexes the parallel auxiliary array. Surrounding combat/object logic derives duration and modifies it in the presence of selected primary effect bits.

## Initialization and expiration

Character initialization at physical `0x51AAE..0x51ACE` zeros all three runtime deadline dwords `0x21A7E`, `0x21A82`, and `0x21A86`.

Physical `0x593C4..0x59456` walks exactly three deadlines for each active party character. A matured nonzero deadline is cleared and the character is marked for UI/state refresh. The parallel auxiliary dword is not cleared by this sweep.

## Direct consumer

Physical `0x59C5B..0x59CC8` selects one of the three slots, validates its deadline against current time, and consumes the corresponding auxiliary dword while the state remains active. This confirms one-to-one pairing:

```text
deadline[0] <-> aux[0]
deadline[1] <-> aux[1]
deadline[2] <-> aux[2]
```

## Reconstruction rule

Zero deadline means inactive; nonzero values are absolute gameplay-tick deadlines. Preserve both arrays exactly and do not merge them with the adjacent primary effect system:

```text
primary effect bits       +0x132..+0x135
primary effect expiry[26] +0x136..+0x19D
timed deadlines[3]        +0x19E..+0x1A9
timed auxiliaries[3]      +0x1AA..+0x1B5
```

Earlier `+0x1A6` / `+0x1B2` serialized starts were based on obsolete runtime base `0x218D8` and are superseded.