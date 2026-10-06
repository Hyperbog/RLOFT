# CODE.1 / CODE.2 Application Object Map

Status: **CORRECTED / CONFIRMED AGAINST CANONICAL DECOMPILATION BASELINES**

This document records the physical-to-logical mapping required for runtime/data xref work. It supersedes the earlier revision of this file, which incorrectly treated the LE `data-pages offset` as the physical start of object 1. The canonical CODE.1/CODE.2 decompilation records already contain the certified application extraction boundaries and remain controlling evidence.

## 1. Bound-image structure

Both shipped files are DOS/4G-bound executables containing the protected-mode Ravenloft application after the extender/binding material. The application metadata describes two objects: executable object 1 and data object 2.

The important rule is that a raw file offset cannot be converted by subtracting the LE-header-relative data-pages field alone. The physical page stream must be interpreted using the executable's actual page organization. The project's already-certified extraction baselines provide the authoritative object starts.

## 2. CODE.1 certified map

Canonical CODE.1 records establish:

- file size: `0x43D1B`
- object-1 virtual/executable extent: `0x11302`
- object-1 file start: `0x2C724`
- object-1 virtual base used by the application map: `0x10000`
- object 1 occupies 18 x `0x1000` physical page slots
- object-2 virtual base: `0x30000`

Therefore object 2 begins physically at:

`0x2C724 + (18 * 0x1000) = 0x3E724`

### CODE.1 `CHARS`

`CHARS` occurs at file offset `0x3E7B8`.

Object-2 displacement:

`0x3E7B8 - 0x3E724 = 0x94`

Correct mapped address:

**CODE.1 `CHARS` = object 2 + `0x94` = linear `0x30094`.**

The previous value `0x201DC` was incorrect and must not be used for xref work.

## 3. CODE.2 certified map

Canonical CODE.2 records establish:

- file size: `0x88661`
- object-1 virtual size: `0x3A852`
- object-1 file start: `0x3AF24`
- object-1 virtual base: `0x10000`
- object 1 occupies 59 x `0x1000` physical page slots
- object-2 file start: `0x75F24`
- object-2 virtual base: `0x50000`
- object-2 virtual size: `0x2DD10`

The object-2 physical start is independently reproduced by:

`0x3AF24 + (59 * 0x1000) = 0x75F24`

### CODE.2 `CHARS` #1

File offset: `0x76634`

Object-2 displacement:

`0x76634 - 0x75F24 = 0x710`

Correct mapped address:

**CODE.2 `CHARS` #1 = linear `0x50710`.**

### CODE.2 `CHARS` #2

File offset: `0x77064`

Object-2 displacement:

`0x77064 - 0x75F24 = 0x1140`

Correct mapped address:

**CODE.2 `CHARS` #2 = linear `0x51140`.**

The previous values `0x49858` and `0x4A288` were incorrect and must not be used for xref work.

## 4. Why the correction matters

The erroneous mapping placed all three strings in executable object 1. The corrected map places all three in **data object 2**, which is exactly the expected location for filename/string data referenced by application code.

This also explains why a search of the executable instruction stream for the former absolute addresses produced no valid references.

## 5. Certified xref anchors

Use only these values going forward:

- CODE.1 `CHARS`: file `0x3E7B8` -> object-2 `+0x94` -> linear `0x30094`
- CODE.2 `CHARS` #1: file `0x76634` -> object-2 `+0x710` -> linear `0x50710`
- CODE.2 `CHARS` #2: file `0x77064` -> object-2 `+0x1140` -> linear `0x51140`

## 6. Evidence hierarchy

The correction was made by reconciling the shipped binaries against the project's certified decompilation baselines rather than extending an unverified header interpretation:

- `Code1/DECOMPILATION_CONTINUITY.md`: object-1 file start `0x2C724`, extent `0x11302`
- `Code2/README.md` / `FINAL_CERTIFICATION.md`: object-1 file start `0x3AF24`, object-2 file start `0x75F24`, object-1 extent `0x3A852`

The shipped CODE bytes remain controlling evidence.

## Next checkpoint

Search the executable object-1 streams for references/fixups resolving to data-object offsets `0x94`, `0x710`, and `0x1140`; classify each reference by operation and function; then correlate the resulting paths with launcher return statuses 10 and 12.