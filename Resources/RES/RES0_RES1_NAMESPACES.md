# INIT-driven RES0/RES1 resource namespaces

Status: **CONFIRMED for the supplied Ravenloft build.** Three leading words in `INIT` are direct graphics namespace bases consumed by CODE.2 and independently close against the supplied RES0/RES1 archive boundaries.

## Fixed INIT words

CODE.2 `16D32..16DA6` reads the first three `INIT` words and stores them as:

```text
INIT +0x00 =  15 -> DGROUP 0x258DC
INIT +0x02 = 168 -> DGROUP 0x258D8
INIT +0x04 = 336 -> DGROUP 0x258E4
```

These are not generic counts. Their consumers add local graphic IDs before archive-specific resource loads.

## `+0x00 = 15`: RES0 raster-graphics base

The supplied RES0 archive contains exactly 141 entries:

```text
RES0:0..14    15 type-2 font resources
RES0:15..140 126 type-1 raster resources
```

CODE.2 graphics/UI paths repeatedly add local image numbers to global `0x258DC` and render the resulting archive-0 resource. Therefore `INIT +0x00` is safely the **RES0 type-1/raster graphics base**: it skips the complete 15-font prefix and lands exactly on `RES0:15`.

## `+0x02 = 168`: RES1 OBJECTS inventory/UI graphics base

`OBJECTS.DAT` contains 336 fixed 0x41-byte definitions. Its unaligned word at `+0x2D` is independently proved as the inventory/UI graphic index.

CODE.2 `10C58..10C63`, `10D31`, and related inventory/equipment presentation paths compute:

```text
absolute_RES1_id = INIT[+0x02] + OBJECTS[object_id].ui_graphic_index
                 = 168 + local_ui_id
```

Across all supplied OBJECTS definitions:

```text
local_ui_id minimum  0
local_ui_id maximum 167
```

Therefore the complete object UI namespace is:

```text
RES1:168..335  168 serialized slots
```

Several referenced slots are intentionally zero-length in the supplied archive. They remain valid archive entries and must not be compacted or renumbered.

## `+0x04 = 336`: RES1 character portrait base

Character presentation code at `FF80..FFB4`, `16437..16469`, and `1C7FA..1C829` computes:

```text
portrait_id = INIT[+0x04]
            + 23 * character.sex_index
            + int8(character.face_index)

if (character.status_word & 0x0100)
    portrait_id += 46
```

The character fields are independently named by the indexed `T` text reader:

```text
character +0x12  int8  face/portrait index
character +0x13  uint8 sex index
character +0x14  uint8 race index

T[27] = MALE
T[28] = FEMALE

T[15] = HUMAN
T[16] = HALF-ELF
T[17] = HALFLING
T[18] = GNOME
T[19] = DWARF
T[20] = ELF
```

CODE.2 `104B2..104CE` reads `T[27 + sex_index]`; `1046B..1047E` reads `T[15 + race_index]`. The supplied populated SAVE records use sex indexes 0/1 and race indexes 0..5.

### Physical portrait banks

RES1 closes around the formula exactly:

```text
normal namespace, 46 slots
  RES1:336..379  44 typed portrait resources
  RES1:380       empty
  RES1:381       raw 60 00 00 00

status-0x0100 namespace, 46 slots
  RES1:382..425  44 typed portrait resources
  RES1:426       empty
  RES1:427       raw 60 00 00 00
```

Every typed entry in both banks has the same type-1 descriptor tuple:

```text
count=80, height=1, width_units=6, field_5=255, compression=3, field_7=1
```

The `+46` executable adjustment therefore selects an exactly parallel second serialized namespace. The gameplay meaning of status bit `0x0100` is not assigned here; only its portrait-bank effect is proved.

The two raw `0x00000060` records remain runtime-unconsumed by the established CODE.2 RES loaders. Their location is now structurally explained as the same final local slot in each 46-slot portrait namespace, but their historical producer/editor meaning remains `UNKNOWN`.

## Reproducible validation

`Resources/MISC/INIT/analyze_resource_bases.py` checks the immutable INIT/RES/OBJECTS/T/SAVE inputs and proves:

- `INIT +0x00/+0x02/+0x04 == 15/168/336`;
- RES0 has exactly 15 fonts before 126 type-1 graphics;
- every OBJECTS UI index lies in `0..167`, resolving into RES1 `168..335`;
- both 46-slot portrait namespaces have the identical `44 typed + empty + raw marker` geometry;
- all 12 populated supplied SAVE character records resolve through the portrait formula to typed resources in both portrait namespaces;
- the `T` labels establish the sex and race selector mappings above.

## Porting rule

Keep the serialized local indexes local. A native port may expose resolved resource handles internally, but legacy import/export must preserve:

- OBJECTS `+0x2D` as a local `0..167` UI graphic index;
- character face/sex/race bytes as their original numeric codes;
- both empty portrait slots and both raw four-byte markers;
- runtime base addition rather than rewriting source data to absolute RES1 IDs.
