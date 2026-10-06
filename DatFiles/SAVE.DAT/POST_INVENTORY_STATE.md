# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; spell matrices, item-identification metadata, and the timed-effect flag/timer core are now structurally certified.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. Sparse quick-start data must not be mistaken for padding.

## Dual spell memorization/preparation matrices

```text
matrix A: runtime 0x2194A..0x21999 = record +0x072..+0x0C1
matrix B: runtime 0x2199A..0x219E9 = record +0x0C2..+0x111
matrix byte = base + spell_level_index * 10 + spell_entry
```

Both are 8 x 10 packed cells. High nibble is currently ready/memorized count; low nibble is selected/planned count; low nibble `0xF` is unavailable. Matrix A is the scribeable spellbook family. Matrix B is a distinct spell family whose display-family name remains conservative pending `T` resource decoding.

## Executable-accessed dword at `+0x12F..+0x132`

Runtime `0x21A07` maps to record `+0x12F`. One path writes a dword there and adds its low byte to the certified Dexterity-related current/effective byte at `+0x3D`; a corresponding path subtracts that low byte. The full dword remains unnamed because fixture values use upper bytes while the low byte is zero.

## Item known/identified-state bitset at `+0x133..+0x136`

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to `item[0..20]`:

```text
bit N <-> item[N], N = 0..20
```

CODE.2 helpers read/write individual bits, inventory transfers move the bit with the item, initialization clears the dword, a bulk action marks every occupied slot, and the inventory renderer consumes the bit beside the item ID. This certifies persistent per-item **known/identified state**.

Native code must move the corresponding bit when an item changes slots and clear it when a slot becomes empty. Do not derive identification state from OBJECTS.DAT.

## Class/casting capability selector at `+0x139`

Runtime `0x21A11` is initialized from class-related bytes and used as a whole-byte selector/mask by spell UI/setup paths. Setup code ORs `0x04` for class code 1, `0x01` for class code 3, and `0x02` when any class byte equals code 4. Exact class names remain tied to the separate class-ID reconstruction.

## Timed character-effect flags at `+0x13A..+0x13D`

Runtime `0x21A12` is a 32-bit effect/state bitset. Executable expiry processing now gives the field a stronger structural identity than the earlier generic `state/flags` label.

### 26 effect bits have parallel expiration dwords

Physical `0x43957..0x43988` performs the following for each active character:

1. if the dword at `0x21A12` is zero, skip effect-expiration work;
2. iterate effect index `0..25`;
3. inspect a dword beginning at runtime `0x21A16 + effect_index*4`;
4. when that dword is positive and the current game-time value has reached/passed it, call physical `0x43640(character, effect_index)`.

The timer base maps exactly to serialized record offset `+0x13E`:

```text
0x21A16 - 0x218D8 = 0x13E
```

and the loop covers 26 dwords:

```text
record +0x13E + effect_index*4
for effect_index = 0..25
```

Thus `+0x13E..+0x1A5` is a certified array of **26 per-effect expiration/time dwords** aligned with bits 0..25 of the `+0x13A` effect bitset.

### Expiration helper

Physical `0x43640` computes `mask = 1 << effect_index` and XORs that mask into `0x21A12`. It is reached by the expiration scanner only for an active character/effect timer that has matured. The routine also performs effect-specific follow-up/resource-message work indexed by the same effect number.

This establishes the core legacy model:

```c
uint32_t active_effect_bits;       // +0x13A
uint32_t effect_expiry[26];        // +0x13E .. +0x1A5

// bit i and effect_expiry[i] describe the same effect slot
```

The exact time unit/epoch is still being traced; preserve raw dword values rather than converting them prematurely.

### Confirmed masks and consumers

```text
0x00000001  bit 0; tested and explicitly cleared
0x00000002  bit 1; explicitly set; included in 0x0402 test
0x00000008  bit 3; affects a derived combat calculation
0x00000010  bit 4; included in dword test 0x01000010
0x00000020  bit 5; explicitly set/tested/cleared
0x00000040  bit 6; tested; one path invokes expiration/helper index 6
0x00000100  bit 8; affects the same derived calculation
0x00000200  bit 9; cleared together with bit 5 by WORD AND 0xFDDF
0x00000400  bit 10; included in WORD test 0x0402
0x01000000  bit 24; included in dword test 0x01000010
```

At physical `0x47907..0x47938`, bits 3, 5, 6, and 8 alter a common derived calculation; bit 6 additionally triggers helper `0x43640` with effect index 6. This further confirms that the dword is an effect registry rather than miscellaneous unrelated flags.

Physical `0x43B91` clears bits `0x20` and `0x0200` together while also zeroing associated character auxiliary dwords. Physical `0x4949C` separately walks the party, notices bit `0x20`, marks the character UI dirty, and clears that bit. These behaviors are retained as executable facts without assigning spell/effect names prematurely.

## Reconstruction rule

A native decoder should model at least:

```text
+0x072..+0x0C1   spell-preparation matrix A: 8 x 10 packed cells
+0x0C2..+0x111   spell-preparation matrix B: 8 x 10 packed cells
+0x112..+0x12E   unresolved
+0x12F..+0x132   executable-accessed auxiliary dword
+0x133..+0x136   uint32 item known/identified-state bitset; bits 0..20 -> item[0..20]
+0x137..+0x138   unresolved bytes
+0x139           class/casting capability selector/mask
+0x13A..+0x13D   uint32 active timed-effect bitset; bits 0..25 paired with timer slots
+0x13E..+0x1A5   uint32 effect_expiry[26]
+0x1A6..+0x247   unresolved effect/auxiliary state
```

Preserve all unresolved bytes and raw timer values losslessly.

## Next target

Trace the effect-specific producers and resource-message IDs for effect indexes 0..25. This should assign canonical gameplay names to individual `+0x13A` bits while simultaneously determining the time representation stored in `effect_expiry[26]`.