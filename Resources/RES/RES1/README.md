# RES1

- Bytes: 354,310
- SHA-256: `c13b8ae42f7d3b65c06d8413627f40a2a155a7404da44abfc158b38a4e5efd19`
- Source upload: `RES+OTHER.zip`
- Family: RES archive

RES1 is structurally covered by the common RES archive parser/round-trip tooling.

The archive contains 405 typed `EH` resources, 21 intentional zero-length entries, and two raw four-byte records:

```text
RES1:381  60 00 00 00
RES1:427  60 00 00 00
```

A CODE.2 access-path audit shows that these raw records are not compatible with the normal typed loader and that archive 1 has no direct raw-handle consumer in the supplied executable. Their historical producer-side meaning remains unknown, so they are preserved exactly rather than assigned a speculative resource type.

See `../RES1_SENTINELS.md` for the evidence and preservation rule.
