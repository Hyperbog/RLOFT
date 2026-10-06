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

Runtime byte `0x219EA` maps to record `+0x112`.

Character setup at physical `0x51AE6..0x51AF9` calls helper `0x48AF8(character)` and stores its byte result directly at `0x219EA`.

The helper is executable-readable enough to establish its role without relying on fixture values:

1. it counts the character's active class slots from class bytes `+0x1D..+0x1F`;
2. it scans the corresponding class-level words beginning at runtime `0x21900`;
3. it selects the greatest level and remembers the class slot that supplied it;
4. it uses two class-indexed progression-table bytes at DGROUP `0x6827/0x6828`;
5. it computes a progression quantity from level and class;
6. it returns `20 - progression`.

This is the characteristic descending attack-target/THAC0-style calculation used by the combat engine. Later combat code reads the byte directly. The safe native label is therefore:

```c
uint8_t base_attack_target; // +0x112; class/level-derived, 20 - progression
```

`THAC0` is an appropriate AD&D interpretation, but the structural documentation retains `base_attack_target` because that is what the executable calculation proves directly.

The supplied quick-start SAVE stores zero here for its populated characters; setup recomputes it. Do not infer field meaning from that sparse fixture value.

## `+0x113..+0x11A`: two hand/action ready deadlines

Runtime `0x219EB` maps to record `+0x113`. CODE.2 accesses it as exactly two dwords:

```c
uint32_t hand_ready_deadline[2]; // +0x113, +0x117
```

Evidence:

- initialization at physical `0x51A00` clears the serialized region beginning at `0x219EB`; explicit setup also zeros nearby combat state;
- maintenance physical `0x593C4..0x59456` obtains current game time and loops exactly two dwords at `0x219EB + hand*4`; a matured nonzero value is cleared and the character UI is marked dirty;
- hand/item-use logic physical `0x5F982..0x5F99E` checks the corresponding deadline and rejects/blocks the affected hand while it is nonzero;
- producer physical `0x5FAF4..0x5FB10` obtains current game time, adds a caller-supplied duration, and writes the resulting absolute deadline to `0x219EB + hand*4`.

Thus these are per-hand/action **absolute ready/cooldown deadlines**, not opaque dwords. Preserve their raw game-clock values for compatibility.

## `+0x11B..+0x122`: two independent combat/effect accumulators

Runtime dwords `0x219F3` (`+0x11B`) and `0x219F7` (`+0x11F`) are initialized to zero and are repeatedly consumed by combat/effect code.

`+0x11B` is added into several derived combat values and is decremented/consumed by elapsed or applied quantities. `+0x11F` is similarly decremented toward zero; when it reaches zero, one path clears primary effect bit 0. These accesses prove that both dwords are persistent combat/effect state rather than padding, but their exact player-facing labels are not yet safe.

Portable interim representation:

```c
int32_t combat_aux_0; // +0x11B
int32_t combat_aux_1; // +0x11F
```

Do not merge them with the hand-ready deadlines: their update semantics are different.

## `+0x123..+0x12A`

No direct absolute-address consumer was found in the current CODE.2 audit for this eight-byte subrange. It remains serialized opaque state and must be preserved losslessly. Indirect/computed access cannot be excluded solely from absence of direct references.

## `+0x12B..+0x12E`: reversible Strength modifier for primary effect index 12

Runtime `0x21A03` maps to record `+0x12B`.

Producer physical `0x458B9..0x45940` supplies the decisive linkage:

- it sets primary effect bit 12 (`0x00001000`) by ORing `0x10` into byte runtime `0x21A13`;
- it computes and stores the corresponding effect deadline at runtime `0x21A46`, which is `+0x13E + 12*4`;
- it computes a positive Strength adjustment from character state;
- it calls the certified Strength-adjustment helper `0x4CF50`;
- it stores that helper's actual returned adjustment in runtime dword `0x21A03` / record `+0x12B`.

Expiration helper physical `0x43799..0x437AF` reads `0x21A03`, negates it, and passes it back to `0x4CF50`, reversing the Strength change.

```c
int32_t primary_effect_12_strength_adjustment; // +0x12B
```

The player-facing effect name remains pending the primary-effect resource catalogue.

## `+0x12F..+0x132`: reversible Dexterity-related auxiliary dword

Runtime `0x21A07` maps to record `+0x12F`. One transition adds its low byte to certified Dexterity current/effective `+0x3D`; companion expiration/cleanup subtracts the same low byte. Preserve the full dword because fixture values use upper bytes even where consumers operate on the low byte.

## `+0x133..+0x136`: item known/identified-state bits

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]`. CODE.2 moves the bit with an inventory item, clears it with the slot, can mark all occupied items, and passes the state to item rendering.

## `+0x137`: selected spell bank

```c
uint8_t selected_spell_bank; // zero-based spell-level bank
```

Spell-list expansion multiplies this byte by ten and indexes both 8x10 matrices. Selecting another bank writes the new value and resets `+0x138`.

## `+0x138`: six-row spell-list scroll offset

```c
uint8_t spell_list_scroll;
```

Spell UI navigation changes it by exactly six; changing banks resets it to zero.

## `+0x139`: class/casting capability selector

Runtime `0x21A11` is initialized from class-related bytes and used as a selector/mask by spell setup and UI paths. Setup ORs `0x04` for class code 1, `0x01` for class code 3, and `0x02` when any class byte equals code 4. Exact class labels remain governed by class-ID reconstruction.

## `+0x13A..+0x1A5`: primary timed effects

```c
uint32_t primary_effect_bits;       // +0x13A
uint32_t primary_effect_expiry[26]; // +0x13E..+0x1A5
```

Bit `i` corresponds to timer `i`. Expiration processing iterates indexes `0..25`, compares nonzero positive deadlines against current game time, and dispatches effect-specific cleanup through the same index.

Expiration-message resources map contiguously:

```text
effect 0  -> T[588]
...
effect 25 -> T[613]
```

## Current reconstruction map

```text
+0x072..+0x0C1   spell matrix A, 8 x 10 packed cells
+0x0C2..+0x111   spell matrix B, 8 x 10 packed cells
+0x112           class/level-derived base attack target (THAC0-style)
+0x113..+0x11A   uint32 hand_ready_deadline[2]
+0x11B..+0x11E   persistent combat/effect accumulator 0
+0x11F..+0x122   persistent combat/effect accumulator 1
+0x123..+0x12A   opaque serialized state; no direct reference found
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

Use explicit legacy decoding. Preserve absolute hand-ready deadlines and all opaque bytes exactly. `+0x112` may be recomputed by original setup code, but a lossless SAVE round trip must still retain the serialized byte. Do not reinterpret `+0x11B/+0x11F` until their producer families are fully closed.

## Next target

Trace the producers and consumers of `+0x11B/+0x11F`, then determine whether `+0x123..+0x12A` is genuinely reserved serialization space or reached through computed addressing.