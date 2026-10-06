# CODE.1 / CODE.2 Linear-Executable Object Map

Status: **CONFIRMED STRUCTURAL BASELINE**

This checkpoint reconstructs the protected-mode image map needed to convert raw file offsets into runtime addresses. It supersedes the earlier caution that the `CHARS` file offsets could not yet be translated safely.

## 1. Bound-image chain

Both shipped files begin with the same DOS/4G binding/runtime prefix. The outer MZ header reports `e_cblp = 0x0044` and `e_cp = 0x007B`, placing the end of the first MZ image at file offset:

`(0x7B - 1) * 0x200 + 0x44 = 0xF444`

At exactly `0xF444`, both files contain the Rational `BW` signature. This explains the previous `file(1)` classification as a `BW collection for MS-DOS`.

The actual Ravenloft protected-mode application image is a later **LE (Linear Executable)** image. In both files the unique structurally valid LE header begins at:

- `LE header file offset = 0x27BDC`
- signature = `LE`
- byte order = little-endian
- CPU = `2` (386)
- target OS = `1` (OS/2-compatible LE encoding used by the extender)
- object count = `2`

The LE header location is independently supported by the header fields and by a complete, internally consistent object/page map; it is not inferred from the incidental ASCII letters `LE` elsewhere in the binaries.

## 2. CODE.1 LE map

LE header: `0x27BDC`

Header fields relevant to mapping:

| Field | Value |
|---|---:|
| Module pages | `0x18` (24) |
| Initial EIP object | `1` |
| Initial EIP | `0xCC1C` |
| Initial ESP object | `2` |
| Initial ESP | `0x9E90` |
| Page size | `0x1000` |
| Object table offset | `0xC4` |
| Object count | `2` |
| Object page-map offset | `0xF4` |
| Data-pages offset | `0x6A00` |

The data-pages offset is relative to the LE header, giving:

`0x27BDC + 0x6A00 = 0x2E5DC`

Objects:

| Object | Virtual size | Relocation base | Flags | First page | Pages |
|---|---:|---:|---:|---:|---:|
| 1 | `0x11302` | `0x10000` | `0x2005` | 1 | 18 |
| 2 | `0x09E90` | `0x30000` | `0x2003` | 19 | 6 |

The 24 LE page-map entries are sequential physical pages 1 through 24. Therefore an address in stored page `n` maps through the owning object's relocation base plus its object-relative page index and in-page displacement.

### CODE.1 `CHARS`

The certified string occurs at file offset `0x3E7B8`.

Relative to the first stored data page:

`0x3E7B8 - 0x2E5DC = 0x101DC`

This is object 1, zero-based object page 16, displacement `0x1DC`.

Therefore:

**CODE.1 `CHARS` runtime linear address = `0x201DC`.**

This address is now suitable for machine-code/data-reference xref work.

## 3. CODE.2 LE map

LE header: `0x27BDC`

| Field | Value |
|---|---:|
| Module pages | `0x4E` (78) |
| Initial EIP object | `1` |
| Initial EIP | `0x353A0` |
| Initial ESP object | `2` |
| Initial ESP | `0x2DD10` |
| Page size | `0x1000` |
| Object table offset | `0xC4` |
| Object count | `2` |
| Object page-map offset | `0xF4` |
| Data-pages offset | `0x15200` |

Stored data begins at:

`0x27BDC + 0x15200 = 0x3CDDC`

Objects:

| Object | Virtual size | Relocation base | Flags | First page | Pages |
|---|---:|---:|---:|---:|---:|
| 1 | `0x3A852` | `0x10000` | `0x2005` | 1 | 59 |
| 2 | `0x2DD10` | `0x50000` | `0x2003` | 60 | 19 |

All 78 page-map entries are sequential physical pages 1 through 78.

### CODE.2 `CHARS` occurrences

First occurrence, file `0x76634`:

`0x76634 - 0x3CDDC = 0x39858`

This is object 1 page index 57 with displacement `0x858`, giving:

**runtime linear address `0x49858`.**

Second occurrence, file `0x77064`:

`0x77064 - 0x3CDDC = 0x3A288`

This is object 1 page index 58 with displacement `0x288`, giving:

**runtime linear address `0x4A288`.**

Both values fall inside object 1's declared virtual extent (`0x10000` through `0x4A851` inclusive), providing an additional consistency check.

## 4. Architectural consequence

The CODE binaries are not opaque flat byte streams. They are bound DOS/4G executables containing a shared DOS/extender prefix followed by a two-object 386 LE application image. For Ravenloft reverse engineering, raw application file offsets must therefore be translated through the LE page map before they are compared with runtime/decompiler addresses.

For the current handoff investigation the three certified anchors are now:

- CODE.1 `CHARS`: `file 0x3E7B8` -> `linear 0x201DC`
- CODE.2 `CHARS` #1: `file 0x76634` -> `linear 0x49858`
- CODE.2 `CHARS` #2: `file 0x77064` -> `linear 0x4A288`

## 5. Validation notes

The mapping was reconstructed directly from the shipped LE headers, object table, and page map. RetroLinker's open implementation of Rational BW parsing was used only to corroborate the structure of the preceding bound `BW` image; the Ravenloft LE values above come from the game binaries themselves.

This also explains why generic GNU `objdump` was not a safe basis for the earlier xref attempt: the relevant application image is embedded behind the DOS/4G binding layer.

## Next checkpoint

Use linear addresses `0x201DC`, `0x49858`, and `0x4A288` as certified data anchors; enumerate immediate/pointer references to those addresses in the reconstructed application objects; classify the surrounding functions as open/read/write/create/test operations; then correlate those paths with the launcher return-status producers.