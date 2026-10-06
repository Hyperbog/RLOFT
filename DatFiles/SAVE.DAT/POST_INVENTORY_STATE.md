# SAVE.DAT character post-inventory state (`+0x072..+0x1A5`)

Status: **executable reconstruction. Both spell matrices, post-spell combat timing fields, spell-UI persistence bytes, item-identification metadata, reversible effect modifiers, and the primary timed-effect core are structurally certified.**

## Dual spell preparation matrices

```text
matrix A: +0x072..+0x0C1 = runtime 0x2194A..0x21999
matrix B: +0x0C2..+0x111 = runtime 0x2199A..0x219E9
cell = matrix + bank*10 + entry
```

Both are 8 x 10 packed cells. High nibble is ready/current count; low nibble is selected/planned count; low nibble `0xF` means unavailable. Matrix A is the scribeable spellbook family; matrix B is the distinct second casting family.

## `+0x112`: class/level-derived combat target byte

Runtime byte `0x219EA` maps to record `+0x112`. Character setup computes this from class/level progression and returns `20 - progression`; later combat code reads it directly. Safe native label:

```c
uint8_t base_attack_target; // THAC0-style descending attack target
```

## `+0x113..+0x11A`: two hand/action ready deadlines

```c
uint32_t hand_ready_deadline[2]; // +0x113, +0x117
```

Maintenance loops exactly two entries, compares them with current game time, clears matured deadlines, and marks the character UI dirty. Hand/item-use code blocks the corresponding hand while its value remains nonzero. Producers store `current_time + duration`.

## `+0x11B..+0x11E`: secondary damage-absorption pool, directly produced by VAMPIRIC TOUCH

Runtime dword `0x219F3` maps to `+0x11B`.

The producer linkage is now decisive. In the object-use/combat path at physical `0x4988F..0x498A2`, object ID `0x7F` causes the resulting combat quantity in `ESI` to be stored directly at `0x219F3 + character*0x248`. OBJECTS.DAT object `0x7F` is `VAMPIRIC TOUCH`.

Incoming-damage processing at physical `0x47CE7..0x47D18` then consumes this dword before applying the remaining quantity to the character's hit-point word:

1. if the pool is zero, continue;
2. if incoming quantity is at least the pool, subtract the whole pool from the incoming quantity and zero the pool;
3. otherwise subtract the incoming quantity from the pool and reduce the remaining incoming quantity to zero;
4. only the remainder proceeds to the hit-point subtraction.

Thus this is a persistent **damage-absorption/temporary protection pool**. It is independently proven to be populated by VAMPIRIC TOUCH. Other combat calculations also add its current value into derived totals, so the native model should preserve the full signed dword rather than collapse it to a boolean.

Safe label:

```c
int32_t vampiric_touch_absorption_pool; // +0x11B
```

The label records the certified producer identity without asserting an original source-variable name.

## `+0x11F..+0x122`: primary-effect-0 damage-absorption pool

Runtime dword `0x219F7` maps to `+0x11F`.

Physical `0x45435..0x45454` is an effect producer path. It initializes primary effect timer slot 0 and obtains a generated protection quantity from helper `0x5DD64`; after adding 8 it stores the result into `0x219F7`.

Incoming-damage processing at physical `0x47C8D..0x47CB6` consumes this pool before the later protection pools and hit-point subtraction. When depletion reaches zero or below, the dword is clamped to zero and primary effect bit 0 is cleared (`AND 0xFE` on runtime byte `0x21A12`).

This certifies a second persistent damage-absorption pool tied specifically to primary timed effect index 0:

```c
int32_t primary_effect_0_absorption_pool; // +0x11F
```

The player-facing spell/effect name remains pending `T[588]`/producer catalogue closure. Do not substitute a guessed AD&D spell name.

### Damage-resolution ordering

The same damage path shows these persistent pools are layered. At physical `0x47CBC` another later-record pool (`+0x1F2`) is consumed first, followed by `+0x11B`, before the remaining quantity is subtracted from the hit-point word. `+0x11F` is handled immediately before that sequence and clears effect bit 0 on depletion. This establishes the fields as real protective resource quantities rather than generic combat accumulators.

## `+0x123..+0x12A`: reserved/unreferenced serialized subrange in current audit

Runtime addresses `0x219FB..0x21A02` correspond exactly to these eight bytes. An exhaustive direct-address scan of CODE.2 found no aligned or unaligned absolute reference to any byte in that runtime interval, while the immediately adjacent fields at `0x219F7` and `0x21A03` have numerous direct references.

Fixture audit provides supporting but non-decisive evidence: this subrange is zero in the active quick-start characters and the ordinary populated roster records; one record image retains `0xFF` fill in this area, demonstrating that it must not simply be synthesized as zeros during lossless serialization.

Safe classification:

```text
+0x123..+0x12A  reserved/unreferenced serialized bytes in the current CODE.2 audit
```

This is not permission to discard the bytes. Computed/indirect access cannot be disproved from an absolute-reference scan, and legacy round-trip serialization must preserve them exactly.

## `+0x12B..+0x12E`: reversible Strength modifier for primary effect index 12

Runtime `0x21A03` maps to record `+0x12B`. The producer sets primary effect bit 12, schedules timer 12, applies a positive Strength adjustment through the certified Strength helper, and stores the actual returned adjustment here. Expiration negates the stored value and sends it through the same helper.

```c
int32_t primary_effect_12_strength_adjustment;
```

## `+0x12F..+0x132`: reversible Dexterity-related auxiliary dword

Runtime `0x21A07` maps to record `+0x12F`. One transition adds its low byte to certified Dexterity current/effective `+0x3D`; companion cleanup subtracts the same low byte. Preserve the full dword.

## `+0x133..+0x136`: item known/identified-state bits

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]`. CODE.2 moves the bit with an inventory item, clears it with the slot, can mark all occupied items, and passes the state to item rendering.

## `+0x137`: selected spell bank

```c
uint8_t selected_spell_bank;
```

Zero-based spell-level bank. Spell-list expansion multiplies it by ten and indexes both 8x10 matrices.

## `+0x138`: six-row spell-list scroll offset

```c
uint8_t spell_list_scroll;
```

Spell UI navigation changes it by exactly six; changing banks resets it to zero.

## `+0x139`: class/casting capability selector

Runtime `0x21A11` is initialized from class-related bytes and used as a selector/mask by spell setup and UI paths.

## `+0x13A..+0x1A5`: primary timed effects

```c
uint32_t primary_effect_bits;       // +0x13A
uint32_t primary_effect_expiry[26]; // +0x13E..+0x1A5
```

Bit `i` corresponds to timer `i`. Expiration-message resources map contiguously from `T[588]` through `T[613]` for effect indexes `0..25`.

## Current reconstruction map

```text
+0x072..+0x0C1   spell matrix A, 8 x 10 packed cells
+0x0C2..+0x111   spell matrix B, 8 x 10 packed cells
+0x112           class/level-derived base attack target (THAC0-style)
+0x113..+0x11A   uint32 hand_ready_deadline[2]
+0x11B..+0x11E   VAMPIRIC TOUCH-produced damage-absorption pool
+0x11F..+0x122   primary-effect-0 damage-absorption pool
+0x123..+0x12A   reserved/unreferenced serialized bytes; preserve exactly
+0x12B..+0x12E   reversible Strength adjustment for primary effect 12
+0x12F..+0x132   reversible Dexterity-related auxiliary dword
+0x133..+0x136   item known/identified bits
+0x137           selected spell bank
+0x138           spell-list scroll offset
+0x139           class/casting capability selector
+0x13A..+0x13D   primary active-effect bits
+0x13E..+0x1A5   primary effect expiry[26]
```

Later state beginning at `+0x1A6` is documented in `TIMED_SLOTS.md` and `CHARACTER_TAIL.md`.

## Native reconstruction rule

Use explicit legacy decoding. Preserve absolute hand-ready deadlines and all reserved bytes exactly. Model `+0x11B` and `+0x11F` as consumable signed protection quantities, not booleans. The fact that one pool is produced by VAMPIRIC TOUCH and the other belongs to primary effect index 0 is executable-certified; the player-facing identity of primary effect 0 remains pending resource closure.

## Next target

The former `+0x112..+0x12A` unknown block is now structurally closed. Continue with the early/core character fields (`+0x010..+0x045`) and primary-effect resource identities.