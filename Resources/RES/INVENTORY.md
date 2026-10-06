# Supplied RES archive inventory

This inventory is generated from the immutable `RES0`..`RES12` files. Counts describe the supplied Ravenloft corpus, not universal format limits.

| Archive | Bytes | Entries | Zero-length | Non-EH | EH type distribution |
|---|---:|---:|---:|---:|---|
| `RES0` | 2,186,991 | 141 | 0 | 0 | type 1: 126; type 2: 15 |
| `RES1` | 354,310 | 428 | 21 | 2 | type 1: 405 |
| `RES2` | 63,736 | 38 | 0 | 0 | type 1: 38 |
| `RES3` | 5,228,638 | 4,234 | 0 | 0 | type 7: 4,234 |
| `RES4` | 1,975,542 | 667 | 0 | 0 | type 6: 667 |
| `RES5` | 1,510,960 | 98 | 0 | 0 | type 3: 98 |
| `RES6` | 6,210,899 | 266 | 0 | 0 | type 4: 266 |
| `RES7` | 211,133 | 62 | 0 | 0 | type 1: 62 |
| `RES8` | 58,772 | 73 | 0 | 0 | type 1: 73 |
| `RES9` | 8,061,481 | 555 | 0 | 38 | type 1: 517 |
| `RES10` | 46,060,354 | 219 | 0 | 0 | type 4: 219 |
| `RES11` | 15,589,639 | 5,114 | 0 | 0 | type 3: 74; type 4: 260; type 6: 652; type 7: 4,128 |
| `RES12` | 12,150,551 | 667 | 0 | 37 | type 1: 370; type 4: 260 |
| **Total** | **99,582,006** | **11,895** | **21** | **77** | type 1: 1,591; type 2: 15; type 3: 172; type 4: 1,005; type 6: 1,319; type 7: 8,362 |

## Compression inventory

Type 1:

- method 0: 113 resources, all exact raw-size matches;
- method 3: 1,478 resources, all exact LZSS stream matches;
- methods 1 and 2: absent from supplied files.

Type 2:

- method 3: all 15 resources, all exact LZSS stream matches.

Types 6/7 use the shared outer LZSS wrapper rather than the type-1 header compression byte:

- type 6: 1,319 exact streams;
- type 7: 8,362 exact streams.

## Type-6 decoded-size distribution

| Decoded bytes | Count |
|---:|---:|
| 4,096 | 134 |
| 8,192 | 1,185 |

`RES4` contains 67 / 600 respectively; the type-6 subset in `RES11` also contains 67 / 585 respectively.

## Type-2 decoded bitmap distribution

| Decoded bytes | Count |
|---:|---:|
| 10,240 | 1 |
| 11,264 | 1 |
| 15,360 | 13 |

All have 128 glyph-width entries, descriptor count 128, width unit 1, and compression method 3.

## Known shipped anomaly

The same type-4 VOC entry occurs at `RES11:4877` and `RES12:430`. It declares `0x11669` bytes while only `0x1669` bytes are present before the next archive entry. The records are byte-identical and have SHA-256:

`07db990a79ea8d02e89c150fec3d45a918a6efb563a99feb3a21f090315959eb`

The `0x10000` discrepancy is recorded exactly; no correction is applied by the analysis tooling.
