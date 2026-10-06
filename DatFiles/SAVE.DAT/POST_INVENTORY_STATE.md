# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; the 80-byte matrix at `+0x0C2..+0x111` is persistent spell memorization/preparation state with planned/selected and currently-ready copy counts now distinguished, alongside an executable-accessed dword at `+0x12F` and a 32-bit flag field at `+0x13A`.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. Sparse quick-start data must not be mistaken for padding.

## Spell memorization/preparation matrix at `+0x0C2..+0x111`

Runtime address `0x2199A` maps to record offset `+0x0C2`:

```text
0x2199A - 0x218D8 = 0x0C2
```

CODE.2 consumers prove an **80-byte array arranged as 8 banks of 10 bytes**:

```text
record offset = 0x0C2 + bank * 10 + entry
bank          = 0..7
entry         = 0..9
extent        = +0x0C2..+0x111 inclusive
runtime       = 0x2199A..0x219E9
```

The executable's nearby UI/text corpus identifies the subsystem as spell memorization/preparation: the same CODE.2 image contains the `memorize` command text and spell-casting/scribing messages, while these routines initialize class/level-dependent banks, maintain paired counts, and consume prepared copies during spell use.

### Certified nibble roles

Each byte contains two 4-bit quantities:

```text
bits 7..4  currently memorized/ready copies
bits 3..0  selected/planned memorization count
low = 0xF  unavailable / non-selectable sentinel
```

The distinction follows from combined executable behavior rather than AD&D terminology alone:

1. The direct spell-use routine at physical `0x3F3F4..0x3F53D` addresses one matrix element from character, bank, and entry indices. It rejects low nibble `0xF`, rejects a zero high nibble, invokes the associated action/cast path, and on successful use decrements **only the high nibble by one**, preserving the low nibble. Thus the high nibble is the currently usable/prepared copy count.
2. Matrix-management code at `0x3EBFA..0x3ECF9` sums the low and high nibbles independently against per-bank limits. It can reduce the low nibble to satisfy the bank allocation limit and separately reduce the high nibble. It also enforces `high <= low` for an individual entry. Thus the low nibble is the persistent requested/selected count against which the ready count is bounded.
3. Class/level-dependent initialization and availability routines at `0x3ED08..0x3EDC7` and `0x3EDC8..0x3EE98` switch entries between usable numeric state and `0xFF` sentinel state according to spell access. They seed newly enabled cells with paired `1/1` state in specific positions.
4. Because casting consumes the high count without reducing the low count, the low count survives as the memorization plan while the high count represents copies presently ready for use. Subsequent preparation/rest logic can therefore restore ready copies toward the selected count without losing the player's selection.

This gives the native representation:

```c
struct PackedSpellPreparation {
    uint8_t selected_count : 4; // low nibble; 0xF means unavailable
    uint8_t ready_count    : 4; // high nibble; consumed by successful use
};
```

The bitfield syntax above is explanatory only; portable code should decode the nibbles explicitly rather than rely on compiler bitfield layout.

The ten-byte bank width is storage capacity. Executable per-bank entry-count tables determine how many cells in each bank are meaningful. The eight bank identities/level numbering should remain table-derived until their exact UI labels are cross-referenced.

### Fixture consequence: `+0x10A` is an ordinary matrix element

`+0x10A` lies inside this certified array:

```text
0x10A - 0x0C2 = 0x48 = 72
72 = 7 * 10 + 2
therefore +0x10A = bank 7, entry 2
```

EHLIANNA's `+0x10A = 0x0F` has `selected_count = 0xF`, the unavailable sentinel, and `ready_count = 0`. ANVIL DAWN's `+0x0C2 = 0x11` has one selected copy and one currently-ready copy at bank 0, entry 0.

## Executable-accessed dword at `+0x12F..+0x132`

The former fixture-only `+0x131` anchor is not an isolated byte. CODE.2 accesses a dword beginning at runtime `0x21A07`:

```text
0x21A07 - 0x218D8 = 0x12F
record +0x12F..+0x132 = executable-accessed 32-bit field
```

At physical `0x45AD1`, CODE.2 writes a computed dword to `0x21A07`; the immediately following instruction adds the low byte of that computed value to runtime `0x21915`, the already-certified Dexterity-related current/effective byte. A corresponding path at physical `0x437B0` reads the low byte of `0x21A07` and subtracts it from `0x21915`.

This proves a direct relationship between the **low byte** of the `+0x12F` dword and a temporary/equipment-style Dexterity adjustment path. It does **not** yet prove that the whole dword can be named simply as a Dexterity modifier: the reference fixture stores nonzero upper bytes while the low byte is zero.

Reference values are:

```text
EHLIANNA    +0x12F..+0x132 = 00 00 02 00 -> 0x00020000
ANVIL DAWN +0x12F..+0x132 = 00 00 01 00 -> 0x00010000
```

Therefore the earlier notation `+0x131 = 0x02/0x01` remains a true byte observation but is structurally incomplete and must not be modeled as a standalone field. Preserve the complete dword until its upper-byte semantics are traced.

## Certified 32-bit state/flags field at `+0x13A`

Runtime `0x21A12` is a **32-bit little-endian flags/state word** beginning at record `+0x13A`:

```text
0x21A12 - 0x218D8 = 0x13A
record +0x13A..+0x13D = uint32_le state_flags
```

CODE.2 performs byte, word, and dword operations on the same field. Confirmed masks/operations include:

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

The executable also ORs caller-supplied dword masks into the field, XORs dynamically selected bits, tests dynamically selected bits, compares the full dword against zero, and iterates individual bit positions. Individual gameplay names remain unassigned until their surrounding effect/action paths are semantically traced.

Representative physical consumers include `0x4366C`, `0x43957`, `0x43B91`, `0x4539E`, `0x457AF`, `0x45CA1`, `0x46A61`, `0x47907`, `0x47CAF`, `0x4842F`, `0x494A0`, `0x494B0`, `0x49939`, `0x4A042`, `0x4D65E`, `0x4D837`, `0x56630`, `0x5A589`, and `0x6CDDE`.

## Reference-fixture survey

The two populated active characters are sparse:

```text
EHLIANNA
  spell matrix bank 7, entry 2 (+0x10A) = 0x0F
    selected = unavailable (0xF), ready = 0
  +0x12F dword = 0x00020000
  +0x13A flags = 0x00000000

ANVIL DAWN
  spell matrix bank 0, entry 0 (+0x0C2) = 0x11
    selected = 1, ready = 1
  spell matrix bank 7, entry 2 (+0x10A) = 0x0F
    selected = unavailable (0xF), ready = 0
  +0x12F dword = 0x00010000
  +0x13A flags = 0x00000000
```

All ten saved-roster records in this fixture contain only `0x00`/`0xFF` values throughout the post-inventory range. Unused active-party records are predominantly `0xFF`; occupancy must not be inferred from these fields.

## Reconstruction rule

Do not collapse `+0x072..+0x247` into reserved padding. At minimum it contains the certified 80-byte spell-preparation matrix, the executable-accessed dword at `+0x12F`, and the 32-bit flags word at `+0x13A`. A native decoder must preserve the entire tail losslessly until every substructure is decoded.

## Tool support

`tools/analyze_character_records.py` reports the spell matrix as eight ten-byte banks, decodes every byte into high/low nibbles, reports non-fill bytes elsewhere in the post-inventory range, and decodes the dwords at `+0x12F` and `+0x13A` as little-endian values.

## Next target

Map the eight bank identities and their executable entry-count/slot-limit tables, then finish the upper-byte semantics of `+0x12F` and assign individual `+0x13A` flag bits to gameplay effects. Continue outward into unresolved regions `+0x072..+0x0C1`, `+0x112..+0x12E`, `+0x133..+0x139`, and `+0x13E..+0x247`.
