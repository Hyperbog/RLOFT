# RES resource archives

This directory contains one analysis workspace per supplied `RES0` through `RES12` archive.

## Confirmed outer container

All 13 supplied archives use the same directly verified little-endian container:

```text
int32 entry_count
int32 entry_offsets[entry_count]
byte  entry_payloads[...] 
```

`entry_offsets[0] == 4 + 4 * entry_count`. Offsets are absolute file offsets and monotonically delimit entries. Entry `i` has size `offset[i+1] - offset[i]`; the last entry extends from its offset to EOF.

This agrees with the extraction/regeneration logic in the community `LoftRES` utility, but the Ravenloft files themselves are the controlling evidence.

## Inner resources

Secondary tooling identifies many entry payloads as `EH`-prefixed resources, with type 1 used for image/sprite records and type 2 for font records, plus compression methods including 0, 1, and 3. Those inner semantics are useful leads but remain subject to direct verification against CODE.1/CODE.2 and the supplied entries before being promoted to confirmed format documentation.

## Next work

- inventory every entry in every RES archive;
- classify `EH` and non-`EH` payload families;
- map resource indices to CODE.2 loaders/renderers;
- verify image/font dimensions, palette selection, and compression algorithms;
- build lossless extraction and manifest tooling;
- require byte-identical reassembly before declaring RES generation faithful.
