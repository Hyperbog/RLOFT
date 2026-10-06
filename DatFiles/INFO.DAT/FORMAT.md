# INFO.DAT format reconstruction

Status: **complete physical layout proved; resource/install path semantics proved in both CODE.1 and CODE.2; selectors 0..10 decode as the active RES0..RES10 location selectors for the supplied executable pair; selector positions 11..12 are retained historical slots with no active consumer in either certified game module.**

## Controlling evidence

- `INFO.DAT`: 17 bytes
- SHA-256: `4ffa6ac5d5b5afd103bd67d84d3a372b401b3b19407b0c68c44bb0dfd9e32c30`
- certified `CODE.2`: SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- INFO reader: CODE.2 logical `2E768..2E7EE`
- indexed resource opener: `2E7F0..2E9C3`
- resource subsystem setup: `20440..2044F`, `2F2EC..2F464`

The executable bytes are controlling evidence. The supplied installation values are a corpus sample, not a source for field names.

## Physical layout

```text
+0x00  char  resource_location_digits[13]
+0x0D  int16 resource_path_length
+0x0F  byte  resource_path[resource_path_length]
```

The supplied file is:

```text
resource_location_digits = "0000000001111"
resource_path_length     = 2
resource_path            = "D:"
```

Exact bytes:

```text
30 30 30 30 30 30 30 30 30 31 31 31 31 02 00 44 3A
```

CODE.2 reads exactly 13 bytes into DGROUP `0xF054..0xF060`, then subtracts ASCII `'0'` from each byte in place. The serialized representation is therefore an ASCII decimal-digit vector, while the runtime representation is numeric.

The path length is loaded with signed extension. For a valid supplied-style file it must describe the exact remaining byte extent. CODE.2 stores the path beginning at DGROUP `0xF061` after first clearing an 80-byte buffer.

## RES archive location selectors

The high-level resource initialization path calls:

```text
2F2EC("RES", 11)
```

and `2F2EC` forwards that base name and count to `2E7F0`.

The static strings are binary-proved:

```text
DGROUP 0x1130 = "RES"
DGROUP 0x13B8 = "\\"
```

For each index `i = 0..10`, `2E7F0` tests normalized selector byte `0xF054+i`.

When the selector is zero, the opened filename is:

```text
"RES" + decimal(i)
```

When the selector is nonzero, the filename is:

```text
resource_path + "\\" + "RES" + decimal(i)
```

Thus selectors 0..10 are safely modeled as **RES archive location selectors**:

```text
0        -> open RESn from the current/local path
nonzero  -> open <INFO resource path>\RESn
```

The executable tests zero versus nonzero; it does not require the normalized value to equal exactly 1. The on-disk reader nevertheless expects decimal ASCII bytes and the supplied file uses only `0`/`1`.

### Supplied installation routing

For `0000000001111` with path `D:`:

```text
selector 0   RES0   -> RES0
selector 1   RES1   -> RES1
selector 2   RES2   -> RES2
selector 3   RES3   -> RES3
selector 4   RES4   -> RES4
selector 5   RES5   -> RES5
selector 6   RES6   -> RES6
selector 7   RES7   -> RES7
selector 8   RES8   -> RES8
selector 9   RES9   -> D:\RES9
selector 10  RES10  -> D:\RES10
```

This is executable-derived path construction, not an inference from where files happened to be copied for analysis.

## Selectors 11 and 12

The reader always loads and normalizes all 13 digits, but the proved CODE.2 RES-loader call uses count 11.

In the complete certified CODE.2 object:

- the selector array is referenced by its base `0xF054` for the 13-byte input and the indexed resource loop;
- the resource opener has one proved caller and receives count 11;
- no direct reference to selector addresses `0xF05F` or `0xF060` is present.

Therefore digits 11 and 12 must remain serialized/preserved, but **no runtime semantic role is currently proved for them**. Do not automatically label them `RES11`/`RES12` merely because the shipped resource corpus contains those archive names.

## Shared resource path use

The length-prefixed path is independently consumed by the LEVELS event subsystem. The numbered interactive-resource path constructs:

```text
resource_path + "\\" + two_decimal_digits
```

For the supplied `D:` path, event resource 7 becomes `D:\07`.

This establishes the field as an installation/resource path prefix shared by more than the RES0..RES10 loader.

## Preservation rules

A faithful reader/editor should:

1. preserve all 13 serialized ASCII digits, including selectors 11 and 12;
2. expose selectors 0..10 as zero/nonzero RES-location controls while retaining their exact numeric values;
3. preserve the signed 16-bit path-length representation and exact path bytes;
4. never synthesize meanings for selectors 11..12 until an executable consumer is proved;
5. reproduce the original 17 supplied bytes exactly on decode -> encode.

`inspect_info.py` provides the lossless physical parser/encoder. `analyze_info_config.py` verifies the resource-routing semantics against the immutable CODE.2 image.

## CODE.1 independent confirmation

Certified CODE.1 independently preserves the same thirteen-slot reader:

- function 8978 opens INFO.DAT, reads 13 selector bytes, subtracts ASCII '0', then reads the signed-length path payload;
- function 8A04 builds indexed resource filenames from those selector/path values;
- the resource bootstrap reaches the indexed loader with base name RES and group count 11.

Therefore both supplied game modules consume the same physical thirteen-byte selector vector but initialize only indices 0..10.

This is historically important because the CD also contains RES11 and RES12, now proved to be legacy/pre-split composite resource generations. It is consistent with a thirteen-archive lineage predating the final split layout, but no recovered executable currently proves that selector positions 11/12 were ever active. Preserve that distinction.

See Resources/RES/RES11_RES12_LINEAGE.md.
