# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; packed-nibble state at `+0x0C2` and a 32-bit flag field at `+0x13A` are now certified.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. Sparse quick-start data must not be mistaken for padding.

## Certified packed-nibble state at `+0x0C2`

Runtime address `0x2199A` maps to record offset `+0x0C2`:

```text
0x2199A - 0x218D8 = 0x0C2
```

CODE.2 has repeated consumers that address bytes at `0x2199A + character_stride + index`. They independently split each byte into low and high nibbles with masks/shifts `0x0F` and `0xF0`, compare nibble values with `0x0F`, rewrite either nibble while preserving the other, and in some paths set/clear the entire byte to `0xFF`/`0x00`.

This proves that `+0x0C2` is the beginning/anchor of a **packed 4-bit-value character array/state region**. The exact array length and gameplay label remain unresolved; no spell-slot name is assigned yet.

Representative physical CODE.2 consumers include `0x3EBFA..0x3ECC8`, `0x3ED60..0x3EE7A`, `0x3F414..0x3F4DA`, and `0x5E232..0x5E662`.

## Certified 32-bit state/flags field at `+0x13A`

The previous pass conservatively described runtime `0x21A12` as a live byte. Full instruction-width tracing now proves that it is a **32-bit little-endian flags/state word** beginning at record `+0x13A`:

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

The executable also ORs caller-supplied dword masks into the field, XORs dynamically selected bits, tests dynamically selected bits, compares the full dword against zero, and iterates individual bit positions. This is therefore not a single boolean/status byte.

Representative physical consumers:

```text
0x4366C  XOR dynamic dword bit
0x43957  compare full dword with zero
0x43B91  WORD AND 0xFDDF
0x4539E  OR caller-supplied dword mask
0x457AF  set 0x20
0x45CA1  test dynamic dword mask
0x46A61  set 0x02
0x47907  load dword; test low-byte 0x40
0x47CAF  clear 0x01
0x4842F  test 0x20
0x494A0  test 0x20
0x494B0  clear 0x20
0x49939  test dword mask 0x01000010
0x4A042  load dword; test 0x01
0x4D65E  compare full dword with zero
0x4D837  test caller/dynamic dword mask
0x56630  test 0x20
0x5A589  compare full dword with zero
0x6CDDE  WORD test 0x0402
```

Individual gameplay names for these bits remain intentionally unassigned until their surrounding effect/action paths are semantically traced.

## Reference-fixture survey

The two populated active characters are sparse:

```text
EHLIANNA
  +0x10A = 0x0F
  +0x131 = 0x02
  +0x13A..+0x13D = 00 00 00 00

ANVIL DAWN
  +0x0C2 = 0x11
  +0x10A = 0x0F
  +0x131 = 0x01
  +0x13A..+0x13D = 00 00 00 00
```

The `0x11` value at ANVIL DAWN `+0x0C2` is consistent with the newly proved packed-nibble representation: both nibbles contain value `1`. This consistency is supporting fixture evidence, not a gameplay label.

All ten saved-roster records in this fixture contain only `0x00`/`0xFF` values throughout the post-inventory range. Unused active-party records are predominantly `0xFF`; therefore occupancy must not be inferred from these fields.

## Remaining candidate anchors

```text
record +0x10A -> runtime base 0x219E2
record +0x131 -> runtime base 0x21A09
```

These remain exact differential-save anchors but do not yet have certified semantics.

## Reconstruction rule

Do not collapse `+0x072..+0x247` into reserved padding. At minimum it contains a packed-nibble state region anchored at `+0x0C2` and a 32-bit flags word at `+0x13A`. A native decoder must preserve the entire tail losslessly until every substructure is decoded.

## Tool support

`tools/analyze_character_records.py` reports non-fill bytes in the post-inventory range and now decodes `+0x13A..+0x13D` as a 32-bit little-endian flags value.

## Next target

Determine the packed-nibble array boundaries around `+0x0C2`, trace the semantics of its consumers, and then identify the exact gameplay meanings of the certified `+0x13A` flag masks. After that, continue with candidate anchors `+0x10A` and `+0x131`.