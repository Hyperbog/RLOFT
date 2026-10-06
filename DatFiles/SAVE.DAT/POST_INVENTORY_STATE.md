# SAVE.DAT character post-inventory state (`+0x072..+0x247`)

Status: **active executable reconstruction; an 80-byte packed-nibble array at `+0x0C2..+0x111`, an executable-accessed dword at `+0x12F`, and a 32-bit flag field at `+0x13A` are now structurally certified.**

This note isolates the 470 bytes after `item[21]` in each serialized `0x248` character image. Sparse quick-start data must not be mistaken for padding.

## Certified packed-nibble array at `+0x0C2..+0x111`

Runtime address `0x2199A` maps to record offset `+0x0C2`:

```text
0x2199A - 0x218D8 = 0x0C2
```

CODE.2 consumers prove that this is an **80-byte array arranged as 8 banks of 10 bytes**:

```text
record offset = 0x0C2 + bank * 10 + entry
bank          = 0..7
entry         = 0..9
extent        = +0x0C2..+0x111 inclusive
runtime       = 0x2199A..0x219E9
```

Each byte contains two independently manipulated 4-bit values. Consumers extract and rewrite the low/high nibbles with `0x0F` and `0xF0`, compare nibble values with `0x0F`, preserve one nibble while replacing the other, and sometimes replace the entire byte with `0x00` or `0xFF`.

The boundary proof is executable, not fixture-derived. In the loop at physical `0x3EBFA..0x3ECF9`, the outer index executes exactly eight times (`ebx < 8`) and advances the array offset by `0x0A` after each bank. The related initialization/update loops at `0x3ED2C..0x3EDBE` and `0x3EDDB..0x3EE8F` use the same ten-byte bank stride and eight-bank termination. Direct element access at `0x3F407..0x3F41E` computes `bank * 10` before adding an entry index.

The inner loops use a per-bank count rather than blindly touching all ten entries, so the ten-byte width is storage capacity, not proof that every entry in every bank is active.

### Certified nibble behavior

The two nibbles are not merely packed flags: CODE.2 treats them as small **counters/state quantities**.

At physical `0x3F3F4..0x3F53D`, direct `(character, bank, entry)` access first rejects a low nibble of `0xF` and then rejects a zero high nibble. After the associated action path succeeds, `0x3F4BA..0x3F4DA` decrements the **high nibble by exactly one**, masks it back to four bits, preserves the low nibble, and writes the packed byte back. This establishes the high nibble as a consumable/remaining quantity for the selected entry.

The normalization family at `0x3EBFA..0x3ECF9` compares high and low nibbles, accumulates them independently, clamps them against per-bank limits, and rewrites one nibble while preserving the other. The initialization/update families at `0x3ED2C..0x3EE8F` use `0xF` as an unavailable/sentinel low-nibble state and convert selected entries between `00`, `11`, and `FF`-style states according to class/level-driven tables.

Therefore a native reconstruction must preserve the two counters separately even though they share one byte. The exact gameplay names of the two counters are still withheld: the surrounding family is strongly spell-system-shaped, but the canonical SAVE documentation will not call these memorized/prepared spell counts until the table identities and action caller are closed independently.

### Character-class initialization cross-reference

The packed-array initialization helpers are invoked from character setup paths. For example, physical `0x4F8AB` calls the `0x3EDC8` initializer when the first stored class-code byte equals `1`; physical `0x4F915`, `0x4F93F`, and `0x4F969` call the `0x3ED08` initializer for class code `3` in any of the three class-code positions. These calls pass the character index and a class-related word from the character record.

This proves that availability within the 8×10 matrix is class/level dependent. It does not by itself assign AD&D class names to numeric class codes 1 and 3.

### Fixture consequence: `+0x10A` is no longer a separate unknown anchor

`+0x10A` lies inside this certified array:

```text
0x10A - 0x0C2 = 0x48 = 72
72 = 7 * 10 + 2
therefore +0x10A = bank 7, entry 2
```

EHLIANNA's `+0x10A = 0x0F` is therefore an ordinary packed-array element, not evidence of a separate field. ANVIL DAWN's `+0x0C2 = 0x11` is bank 0, entry 0 with both nibbles equal to 1.

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
  packed bank 7, entry 2 (+0x10A) = 0x0F
  +0x12F dword = 0x00020000
  +0x13A flags = 0x00000000

ANVIL DAWN
  packed bank 0, entry 0 (+0x0C2) = 0x11
  packed bank 7, entry 2 (+0x10A) = 0x0F
  +0x12F dword = 0x00010000
  +0x13A flags = 0x00000000
```

All ten saved-roster records in this fixture contain only `0x00`/`0xFF` values throughout the post-inventory range. Unused active-party records are predominantly `0xFF`; occupancy must not be inferred from these fields.

## Reconstruction rule

Do not collapse `+0x072..+0x247` into reserved padding. At minimum it contains the certified 80-byte packed-nibble array, the executable-accessed dword at `+0x12F`, and the 32-bit flags word at `+0x13A`. A native decoder must preserve the entire tail losslessly until every substructure is decoded.

## Tool support

`tools/analyze_character_records.py` reports the packed region as eight ten-byte banks, decodes every byte into high/low nibbles, reports non-fill bytes elsewhere in the post-inventory range, and decodes the dwords at `+0x12F` and `+0x13A` as little-endian values.

## Next target

Close the table identities and caller semantics behind the packed-array action path so the two nibble counters can receive gameplay names without inference. Then finish the upper-byte semantics of `+0x12F`, map individual `+0x13A` flag bits to gameplay effects, and continue outward into the unresolved regions `+0x072..+0x0C1`, `+0x112..+0x12E`, `+0x133..+0x139`, and `+0x13E..+0x247`.