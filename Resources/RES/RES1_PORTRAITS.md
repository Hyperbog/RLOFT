# RES1 character portrait banks

Status: **CONFIRMED for the supplied Ravenloft build.** RES1 entries `336..427` are two 46-slot character-face graphic banks selected from the serialized character identity fields and status word. The two formerly opaque raw `0x00000060` entries occupy the terminal invalid/reserved face slots in those banks.

## Executable index formula

INIT supplies:

```text
INIT +0x04 = 336 -> DGROUP 0x258E4
```

CODE.2 portrait paths at `FF80..FFD0`, `16437..16469`, `1C7FA..1C829`, and `1F581..1F5EE` all use the same calculation:

```text
local_face = int8(character[+0x12])
           + uint8(character[+0x13]) * 23

res1_id = 336 + local_face

if (character.status_word & 0x0100)
    res1_id += 46
```

The resource is then drawn through archive 1.

## Character identity fields

The adjacent serialized character bytes are now cross-proved:

```text
+0x12  face_index
+0x13  sex
+0x14  race
```

`+0x13` is directly linked to game text by CODE.2 `104B2..104D3`:

```text
T[0x1B + sex]
```

with the supplied T records:

```text
T[0x1B] = MALE
T[0x1C] = FEMALE
```

`+0x14` is likewise read through:

```text
T[0x0F + race]
```

where records `0x0F..0x14` are the six supplied race names. `+0x12` is the face selector used in the portrait index formula and another bit-indexed character-UI path.

This document records the resource-side proof without changing the concurrently maintained SAVE.DAT schema files.

## Exact bank geometry

The archive has exactly 92 slots from the face base through EOF:

```text
336..381  bank 0: status bit 0x0100 clear
382..427  bank 1: status bit 0x0100 set
```

Each bank reserves 23 face indexes for each of two sex values:

```text
local  0..22 = sex 0 / MALE,   face 0..22
local 23..45 = sex 1 / FEMALE, face 0..22
```

The supplied resources populate:

```text
MALE   face 0..22  -> 23 typed type-1 resources
FEMALE face 0..20  -> 21 typed type-1 resources
FEMALE face 21     -> zero-length archive entry
FEMALE face 22     -> raw 4-byte 0x00000060 record
```

Thus:

```text
bank 0
  336..358  male faces 0..22
  359..379  female faces 0..20
  380       female face slot 21: zero-length
  381       female face slot 22: 60 00 00 00

bank 1
  382..404  male faces 0..22
  405..425  female faces 0..20
  426       female face slot 21: zero-length
  427       female face slot 22: 60 00 00 00
```

No supplied SAVE character uses female face index 21 or 22. The active typed resource loader could not turn the corresponding slots into valid graphics: the zero-length slot aliases the following raw marker bytes, and the raw marker itself lacks the required `EH` envelope.

A malformed/custom character record could still mathematically calculate those indexes. Therefore the preservation claim is **invalid/reserved slots in the supplied content/runtime contract**, not a claim that arbitrary memory corruption could never request them.

## Portrait resource homogeneity

Every populated portrait slot is a type-1 resource with the same 8-byte descriptor:

```text
count        80
height        1
width_units   6
field_5      FF
compression   3
field_7       1
```

The common decoded size is:

```text
80 * 1 * 6 * 8 = 3,840 bytes
```

The existing type-1 LZSS validator exhausts each stream exactly.

## Status bit `0x0100`

Its **resource behavior** is proved: it selects the second portrait bank by adding exactly 46.

The bit has at least one non-rendering consumer elsewhere in CODE.2, so it should not be renamed merely from the portrait effect. Until its gameplay state is independently certified, use a behavioral label such as:

```text
status_0100 / alternate_portrait_bank_bit
```

rather than `dead`, `injured`, `unconscious`, or another speculative state name.

## Relationship to the two raw RES1 markers

The two raw records are therefore no longer positionally mysterious:

```text
RES1:381 = terminal female-face slot in bank 0
RES1:427 = terminal female-face slot in bank 1
```

Their placement is resolved. Their literal producer-side value `0x00000060` remains unexplained and must be preserved exactly.

See:

- `RES1_SENTINELS.md` for loader reachability and preservation evidence;
- `Resources/MISC/INIT/UI_RESOURCE_BASES.md` for the INIT base fields;
- `DatFiles/SAVE.DAT/CHARACTER_RECORD.md` for the serialized 0x248 character record.
