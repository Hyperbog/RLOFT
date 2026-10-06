# INFO.DAT

Status: **physical format, installation/resource path, and active RES0..RES10 location selectors are proven in the supplied CODE.1/CODE.2 pair; lossless parse/encode verified. The two trailing selector positions remain preserved historical slots with no active consumer in either certified module.**

- Size: 17 bytes
- SHA-256: `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30`
- Exact bytes: `30 30 30 30 30 30 30 30 30 31 31 31 31 02 00 44 3A`.
- CODE.2 reader: logical `2E768..2E7EE`
- CODE.2 indexed resource opener: `2E7F0..2E9C3`

## Proven file layout

```text
+0x00  char  resource_location_digits[13]
+0x0D  int16 resource_path_length
+0x0F  byte  resource_path[resource_path_length]
```

For the supplied file:

```text
resource_location_digits = "0000000001111"
resource_path_length     = 2
resource_path            = "D:"
```

CODE.2 reads all 13 digit bytes into DGROUP `0xF054..0xF060` and subtracts ASCII `'0'` from each in place. The disk representation is therefore a decimal-ASCII selector vector; the runtime representation is numeric.

## RES archive location routing

Resource initialization calls the indexed loader with the literal base name `"RES"` and count **11**. For index `i = 0..10`, CODE.2 tests normalized selector `i`:

```text
selector == 0  -> open "RES" + decimal(i)
selector != 0  -> open resource_path + "\\" + "RES" + decimal(i)
```

Thus digits 0..10 are proved **RES0..RES10 location selectors**.

For this supplied installation:

```text
RES0  -> RES0
RES1  -> RES1
RES2  -> RES2
RES3  -> RES3
RES4  -> RES4
RES5  -> RES5
RES6  -> RES6
RES7  -> RES7
RES8  -> RES8
RES9  -> D:\RES9
RES10 -> D:\RES10
```

The executable checks zero versus nonzero; it does not require the normalized selector to equal exactly one.

Digits 11 and 12 are still read and normalized, but the proved CODE.2 RES setup uses count 11 and no separate consumer for addresses `0xF05F` or `0xF060` has been established. They remain raw/preserved. Do **not** automatically label them RES11/RES12 selectors merely because those archives exist in the supplied resource corpus.

See `FORMAT.md` and `analyze_info_config.py` for the exact executable evidence.

## Shared resource path

The trailing path is independently consumed by LEVELS event opcode 11's numbered interactive-resource path. That subsystem constructs:

```text
resource_path + "\\" + two decimal digits
```

so with the supplied path, numbered event resource 7 resolves to:

```text
D:\07
```

The field is therefore a shared installation/resource path prefix, not an arbitrary string.

## Tooling

- `FORMAT.md` — executable-backed physical and semantic format.
- `inspect_info.py` — lossless parser/encoder.
- `analyze_info_config.py` — immutable CODE.2 signature and RES-routing verifier.

Validation against the supplied files:

```text
INFO.DAT parse -> encode    PASS byte-for-byte
selector digits             0000000001111
RES selectors proved        0..10
resource path               D:
CODE.2 SHA/signatures       PASS
```

## Current frontier

INFO.DAT is at practical format closure. The only unresolved bytes are selector digits 11 and 12: they are structurally valid and must be preserved, but no runtime meaning is currently proved.

## CODE.1 and legacy-archive context

CODE.1 independently reads the same 13 selector bytes but initializes only 11 indexed RES groups, matching CODE.2's active RES0..RES10 set.

The shipped CD also contains RES11/RES12. Cross-archive analysis now identifies them as legacy/pre-split composite resource generations rather than unexplained foreign files. This strengthens the historical interpretation of INFO.DAT as a retained thirteen-slot format, but it does not prove active selector-11/12 semantics in the supplied executables.

See FORMAT.md and ../../Resources/RES/RES11_RES12_LINEAGE.md.
