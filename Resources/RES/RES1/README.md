# RES1

- Bytes: 354,310
- SHA-256: `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19`
- Source upload: `RES+OTHER.zip`
- Family: RES archive

RES1 is structurally covered by the common RES archive parser/round-trip tooling.

The archive contains 405 typed `EH` resources, 21 intentional zero-length entries, and two raw four-byte records.

## Executable-backed namespaces

INIT and CODE.2 now prove two important RES1 base relationships:

```text
INIT +0x02 = 168  object/inventory UI graphic base
INIT +0x04 = 336  character-face graphic base
```

Every supplied `OBJECTS.DAT +0x2D` UI graphic index lies in `0..167`, so object/UI graphics resolve into archive slots `168..335` exactly.

Character portraits occupy the remaining 92 slots `336..427` as two 46-slot banks. CODE.2 selects:

```text
local = face_index + sex * 23
id    = 336 + local
if status_word & 0x0100:
    id += 46
```

`sex` is independently proved as `0=MALE`, `1=FEMALE` through the T text archive. See `../RES1_PORTRAITS.md`.

## Terminal raw records

The portrait-bank ends contain:

```text
RES1:380  empty
RES1:381  60 00 00 00
RES1:426  empty
RES1:427  60 00 00 00
```

These correspond to female face slots 21 and 22 in the two parallel 46-slot banks. All male face slots 0..22 and female face slots 0..20 contain normal typed portrait graphics.

The raw slots cannot pass the normal `EH` loader, and archive 1 has no direct raw-handle consumer in the certified CODE.2 object. Their runtime slot role is therefore resolved as invalid/reserved portrait positions, while the historical reason for the literal marker value `0x00000060` remains unknown.

See:

- `../RES1_PORTRAITS.md` for portrait indexing and character-field evidence;
- `../RES1_SENTINELS.md` for loader reachability and preservation rules;
- `../../MISC/INIT/UI_RESOURCE_BASES.md` for INIT base-field cross-links.
