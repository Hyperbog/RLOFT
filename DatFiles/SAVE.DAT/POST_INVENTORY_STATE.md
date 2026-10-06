# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; two adjacent 80-byte spell-preparation matrices are executable-certified, followed by auxiliary state including a slot-aligned item-instance bitset and a 32-bit character effect/state field.**

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

This closes the formerly unresolved `+0x072..+0x0C1` region: it is not miscellaneous state or padding. It is a second spell-preparation matrix with the same eight-bank, ten-entry geometry and the same packed-nibble accounting rules as the previously certified matrix at `+0x0C2`.

### Executable proof of the parallel layout

The CODE.2 routine beginning at reconstructed object-1 linear `0x11BC0` iterates eight banks and accesses packed cells at runtime `0x2194A + character*0x248 + bank*10 + entry`. Its inner loop extracts both nibbles, treats low nibble `0xF` as unavailable, clamps aggregate counts against a per-bank limit, and enforces the high-nibble count not exceeding the low-nibble count.

The immediately following routine beginning at linear `0x11D4C` performs the same algorithm against runtime `0x2199A + character*0x248 + bank*10 + entry`. It likewise advances ten bytes per bank and terminates after eight banks.

The two bases differ by exactly `0x50` bytes, the size of one complete 8x10 matrix:

```text
0x2199A - 0x2194A = 0x50
```

The second routine additionally chooses between table roots `0x62B0` and `0x6350` according to the character class-related discriminator at record `+0x1D`, demonstrating that matrix availability/progression is class-sensitive.

### Certified nibble roles

For both matrices, each byte contains two 4-bit quantities:

```text
bits 7..4  currently memorized/ready copies
bits 3..0  selected/planned memorization count
low = 0xF  unavailable / non-selectable sentinel
```

The direct spell-use path requires a nonzero high nibble and decrements only that high nibble after successful use, preserving the low nibble. Matrix-management paths independently sum and clamp selected and ready counts and enforce `ready <= selected`.

Portable native code should decode explicitly:

```c
selected = packed & 0x0F;
ready    = packed >> 4;
unavailable = (selected == 0x0F);
```

Do not use compiler-dependent C/C++ bitfield layout for the serialized representation.

### Bank numbering and capacity

Each matrix has eight ten-byte banks. Bank index is zero-based internally and corresponds to displayed spell level `bank + 1`. The ten-byte width is storage capacity; executable table-driven entry counts determine how many cells in a bank are meaningful.

The UI paths currently traced render spell levels 1 through 6, while the persistent record reserves all eight levels. Banks 6 and 7 therefore remain real executable-managed storage even where the shipped UI path does not expose them directly.

### Matrix-family naming boundary

Matrix A has independently been tied to the scribeable spellbook family. Matrix B remains a distinct executable spell family; its exact display-family name is being kept conservative until its `T` resource records are decoded. See `SPELL_FAMILIES.md` and `SPELL_RESOURCE_LOOKUP.md`.

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

## Slot-aligned item-instance bitset at `+0x133..+0x136`

Runtime `0x21A0B` is a 32-bit bitset whose low 21 bits map one-for-one to the character's `item[21]` array:

```text
bit N of record +0x133 dword <-> item[N]
N = 0..20
```

CODE.2 helper physical `0x4A13C` computes `1 << slot`, masks the dword at `0x21A0B`, and returns that slot's bit. Companion helper `0x4A15C` first verifies that `item[slot] != -1`, then clears and rewrites exactly that slot bit from its caller-supplied boolean value.

This is not generic character state. Inventory transfer paths move the bit together with the item instance: they read the source-slot bit, move the signed 16-bit item ID, write the old bit into the destination slot, and clear the source bit. The same mechanism is used for ordinary inventory and equipment slots; direct consumers query equipment slots 14 and 15 as well.

Character initialization at physical `0x51AD4` zeros the complete bitset. Another executable path at physical `0x46E1C..0x46E2C` walks all 21 slots and attempts to set every occupied slot's bit to one. This strongly suggests an item knowledge/identification-style state, but the exact player-facing meaning remains deliberately unnamed until the responsible action/UI path is independently identified.

Rendering/action code also passes the per-slot boolean onward with the item ID, so native code must treat it as persistent **item-instance metadata**, not a property derivable from OBJECTS.DAT.

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

## Reference-fixture consequence

The two populated active characters are sparse, but both matrices and the auxiliary fields must be preserved. In particular, the previously noted `+0x10A` byte is matrix B, bank 7, entry 2. The first matrix occupies the full region immediately after inventory even when most cells are `0x00`/`0xFF` in the quick-start fixture.

All ten saved-roster records contain only `0x00`/`0xFF` values throughout much of this tail. That does not make the regions padding.

## Reconstruction rule

A native decoder should model at least:

```text
+0x072..+0x0C1   spell-preparation matrix A: 8 x 10 packed cells
+0x0C2..+0x111   spell-preparation matrix B: 8 x 10 packed cells
+0x112..+0x12E   unresolved
+0x12F..+0x132   executable-accessed auxiliary dword
+0x133..+0x136   uint32 slot-aligned item-instance bitset; bits 0..20 used
+0x137..+0x138   unresolved bytes
+0x139           class/casting capability selector/mask
+0x13A..+0x13D   uint32 character state/effect flags
+0x13E..+0x247   unresolved effect/auxiliary state
```

Preserve all unresolved bytes losslessly.

## Next target

Identify the action/UI consumer that gives the `+0x133` per-item bit its exact player-facing meaning, then assign individual `+0x13A` character-state flag bits to gameplay effects.