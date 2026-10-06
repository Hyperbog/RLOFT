# INIT UI/resource-base fields

Status: **CONFIRMED for the supplied Ravenloft build.** The first three 16-bit INIT words are resource-bank bases used by CODE.2. They are not generic unknown initialization constants.

The supplied values are:

```text
INIT +0x00 = 15   -> DGROUP 0x258DC
INIT +0x02 = 168  -> DGROUP 0x258D8
INIT +0x04 = 336  -> DGROUP 0x258E4
```

These fields are independent from the four RES3 scene-shape bases at `+0x06..+0x0C`.

## `+0x00 = 15`: RES0 UI-graphics base

CODE.2 repeatedly adds UI-relative graphic selectors to global `0x258DC` and supplies archive number 0 to resource accessor `2F29C` (directly or through the `2975C`/related drawing wrappers).

The supplied RES0 archive independently changes resource family at exactly this value:

```text
RES0 0..14   15 type-2 font resources
RES0 15..    type-1 UI/raster resources
```

Thus the safe field name is **`res0_ui_graphics_base`**. Some executable paths add offsets that are not exercised by the supplied active resource set, so this name describes the runtime base relationship rather than asserting that every mathematically possible offset is a valid shipped RES0 entry.

## `+0x02 = 168`: RES1 object/inventory UI base

Inventory/UI paths read `OBJECTS.DAT +0x2D` as the object's unaligned 16-bit UI graphic index, add global `0x258D8`, and request archive 1.

Across all 336 supplied OBJECTS definitions:

```text
minimum ui_graphic_index = 0
maximum ui_graphic_index = 167
```

Therefore the supplied object/UI namespace maps exactly to:

```text
absolute_RES1_id = 168 + OBJECTS.ui_graphic_index
RES1 range       = 168..335
```

The next INIT base is 336, so the serialized boundary independently closes this 168-slot namespace. The safe field name is **`res1_object_ui_base`**.

See `DatFiles/OBJECTS.DAT/FORMAT.md` for the independently reconstructed `ui_graphic_index` field.

## `+0x04 = 336`: RES1 character-face base

Multiple CODE.2 portrait/UI paths (`FF80..FFD0`, `16437..16469`, `1C7FA..1C829`, `1F581..1F5EE`) compute the archive-1 graphic index as:

```text
portrait_id = 336
            + int8(character.face_index)
            + uint8(character.sex) * 23

if (character.status_word & 0x0100)
    portrait_id += 46
```

The character fields are runtime record offsets:

```text
record +0x12  int8/byte face_index
record +0x13  uint8 sex
record +0x14  uint8 race
```

`sex` and `race` are independently proved by the indexed T text reader:

- CODE.2 `104B2..104D3` reads `T[0x1B + character.+0x13]`; supplied records `T[0x1B]` and `T[0x1C]` are `MALE` and `FEMALE`.
- CODE.2 `1046B..10483` reads `T[0x0F + character.+0x14]`; supplied `T[0x0F..0x14]` are the six race names `HUMAN`, `HALF-ELF`, `HALFLING`, `GNOME`, `DWARF`, `ELF`.
- `+0x12` is used as the per-sex face selector in every portrait resource calculation and as a face-index bit position in another character UI path.

Thus the safe field name for INIT `+0x04` is **`res1_character_face_base`**.

The gameplay meaning of status bit `0x0100` remains deliberately unnamed here. Its resource behavior is exact: it selects the second 46-slot portrait bank.

## RES1 portrait-bank closure

The 92 archive slots from 336 through 427 divide exactly into two 46-slot banks:

```text
normal status bank      RES1 336..381
status bit 0x0100 bank  RES1 382..427
```

Within either bank the local selector is:

```text
local = face_index + sex * 23
```

For the supplied resources:

```text
sex 0 / MALE    face 0..22  -> all 23 slots are typed type-1 graphics
sex 1 / FEMALE  face 0..20  -> 21 typed type-1 graphics
sex 1 / FEMALE  face 21     -> zero-length archive slot
sex 1 / FEMALE  face 22     -> raw 4-byte 60 00 00 00 marker
```

The same final empty/raw pair repeats in the alternate bank:

```text
normal bank      face slots 44/45 -> RES1:380 empty, RES1:381 60 00 00 00
alternate bank   face slots 44/45 -> RES1:426 empty, RES1:427 60 00 00 00
```

Every valid portrait resource in these banks uses the same type-1 descriptor (`count=80`, `height=1`, `width_units=6`, method 3), for 3,840 decoded bytes per graphic.

This explains the *placement* of the two raw RES1 markers. It does **not** reveal why the producer serialized literal `0x00000060` rather than another invalid-slot marker; that historical producer detail remains `UNKNOWN`.

See `Resources/RES/RES1_PORTRAITS.md` and `Resources/RES/RES1_SENTINELS.md`.
