# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; two adjacent 80-byte spell-preparation matrices are executable-certified, followed by auxiliary state including a slot-aligned item known/identified-state bitset and a 32-bit character effect/state field.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. Sparse quick-start data must not be mistaken for padding.

## Dual spell memorization/preparation matrices

Relocation-aware disassembly of CODE.2 object 1 exposes two structurally parallel 80-byte matrices inside every character record:

```text
matrix A: runtime 0x2194A..0x21999 = record +0x072..+0x0C1
matrix B: runtime 0x2199A..0x219E9 = record +0x0C2..+0x111

matrix byte = base + spell_level_index * 10 + spell_entry
spell_level_index = 0..7   (displayed spell level = index + 1)
spell_entry       = 0..9
```

The CODE.2 routine beginning at reconstructed object-1 linear `0x11BC0` iterates matrix A; the immediately following routine at `0x11D4C` performs the parallel algorithm against matrix B. Both use the same packed-nibble accounting rules.

### Certified nibble roles

```text
bits 7..4  currently memorized/ready copies
bits 3..0  selected/planned memorization count
low = 0xF  unavailable / non-selectable sentinel
```

The direct spell-use path requires a nonzero high nibble and decrements only that high nibble after successful use. Matrix-management paths independently sum and clamp selected and ready counts and enforce `ready <= selected`.

Portable native code should decode explicitly:

```c
selected = packed & 0x0F;
ready    = packed >> 4;
unavailable = (selected == 0x0F);
```

Each matrix has eight ten-byte banks. Bank index is zero-based internally and corresponds to displayed spell level `bank + 1`. Matrix A has independently been tied to the scribeable spellbook family. Matrix B remains a distinct executable spell family; its exact display-family name remains conservative until its `T` resource records are decoded. See `SPELL_FAMILIES.md` and `SPELL_RESOURCE_LOOKUP.md`.

## Executable-accessed dword at `+0x12F..+0x132`

CODE.2 accesses a dword beginning at runtime `0x21A07`:

```text
0x21A07 - 0x218D8 = 0x12F
record +0x12F..+0x132 = executable-accessed 32-bit field
```

One path writes a computed dword there and adds its low byte to the certified Dexterity-related current/effective byte at `+0x3D`; a corresponding path subtracts the low byte. The whole dword must not yet be named simply as a Dexterity modifier because the reference fixture stores nonzero upper bytes while the low byte is zero.

Reference values:

```text
EHLIANNA    +0x12F..+0x132 = 00 00 02 00 -> 0x00020000
ANVIL DAWN +0x12F..+0x132 = 00 00 01 00 -> 0x00010000
```

## Item known/identified-state bitset at `+0x133..+0x136`

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to the character's `item[21]` array:

```text
bit N of record +0x133 dword <-> item[N]
N = 0..20
```

CODE.2 helper physical `0x4A13C` computes `1 << slot`, masks the dword at `0x21A0B`, and returns that slot's bit. Companion helper `0x4A15C` first verifies that `item[slot] != -1`, then clears and rewrites exactly that slot bit from its caller-supplied boolean value.

Inventory transfer paths prove that the bit belongs to the item instance: they read the source-slot bit, move the signed 16-bit item ID, write the old bit into the destination slot, and clear the source bit. The same mechanism is used for ordinary inventory and equipment slots, including direct consumers of hand slots 14 and 15.

The remaining semantic ambiguity is resolved by combining the bulk-set and rendering paths:

1. character initialization at physical `0x51AD4` clears the complete bitset;
2. physical `0x46E1C..0x46E2C` walks all 21 slots and sets the bit for every occupied item;
3. inventory rendering at physical `0x4FB64..0x4FBC6` retrieves the per-slot bit with `0x4A13C` and passes it immediately beside the item ID to renderer `0x5C880`;
4. `0x5C880` stores that boolean as a dedicated rendered-item flag (`0x0400` in the transient render record);
5. equip/unequip and inventory-transfer routines preserve or clear the bit exactly as expected for knowledge attached to an item instance.

A command/effect that marks every carried object at once, combined with a renderer-visible boolean that follows the object rather than its slot, identifies this as the persistent **item known/identified state**. Native code should therefore model it as item-instance identification metadata. This state is separate from intrinsic object properties in OBJECTS.DAT and must survive inventory rearrangement.

Recommended representation:

```c
uint32_t item_known_bits;  // serialized at +0x133; bits 0..20 correspond to item[0..20]

bool item_is_known(unsigned slot) {
    return slot < 21 && ((item_known_bits >> slot) & 1u) != 0;
}
```

When moving an item between slots, move the corresponding known bit with it. When deleting/emptying a slot, clear its bit. Do not infer this state from the object ID.

The supplied quick-start fixture has this dword equal to zero for both populated active characters.

## Class/casting capability selector at `+0x139`

Runtime `0x21A11` is initialized from class-related bytes and used as a whole-byte selector/mask by spell UI/setup paths. Setup code ORs `0x04` for class code 1, `0x01` for class code 3, and `0x02` when any class byte equals code 4. Exact class names remain tied to the separate class-ID reconstruction.

## Certified 32-bit state/flags field at `+0x13A`

Runtime `0x21A12` is a 32-bit little-endian flags/state word beginning at record `+0x13A`.

Confirmed masks/operations include:

```text
0x00000001  tested; bit 0 also explicitly cleared
0x00000002  explicitly set; included in a 0x0402 word test
0x00000010  included in dword test 0x01000010
0x00000020  set, tested, and cleared
0x00000040  tested
0x00000200  cleared together with 0x20 by WORD AND 0xFDDF
0x00000400  included in WORD test 0x0402
0x01000000  included in dword test 0x01000010
```

The executable also ORs caller-supplied masks, XORs dynamically selected bits, tests dynamic bits, compares the complete dword with zero, and iterates individual bit positions. Gameplay names remain unassigned until the surrounding effect/action paths are semantically traced.

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
+0x13A..+0x13D   uint32 character state/effect flags
+0x13E..+0x247   unresolved effect/auxiliary state
```

Preserve all unresolved bytes losslessly.

## Next target

Assign individual `+0x13A` character-state flag bits to their gameplay effects, beginning with the directly set/tested/cleared masks `0x20`, `0x02`, and `0x40`.