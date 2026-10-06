# RES1

- Bytes: 354,310
- SHA-256: `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19`
- Source upload: `RES+OTHER.zip`
- Family: RES archive

RES1 is structurally covered by the common RES archive parser/round-trip tooling.

The archive contains 405 typed `EH` resources, 21 intentional zero-length entries, and two raw four-byte records.

Executable/corpus cross-linking now establishes two major INIT-driven namespaces:

```text
RES1:168..335  OBJECTS.DAT inventory/UI graphics
RES1:336..381  character portrait namespace A (46 slots)
RES1:382..427  character portrait namespace B (46 slots; status bit 0x0100)
```

The portrait namespaces each end with one empty slot followed by the same raw marker:

```text
RES1:380 empty   RES1:381 60 00 00 00
RES1:426 empty   RES1:427 60 00 00 00
```

The raw marker's historical producer meaning remains unknown. CODE.2 cannot consume it through the normal `EH` loader and has no raw archive-1 access path, so it remains preserved as inactive serialized metadata/marker data rather than assigned a synthetic resource type.

See:

- `../RES0_RES1_NAMESPACES.md` for the INIT/object/portrait namespace mapping;
- `../RES1_SENTINELS.md` for marker reachability and preservation evidence.
