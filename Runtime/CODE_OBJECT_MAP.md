# CODE.1 / CODE.2 Application Object Map

Status: **CORRECTED / CONFIRMED AGAINST IMMUTABLE BINARIES AND CANONICAL DECOMPILATION BASELINES**

This document records the physical-to-logical mapping required for runtime/data xref work. It supersedes the earlier revision of this file, which incorrectly treated the LE `data-pages offset` as though it were directly relative to the outer bound file. The canonical CODE.1/CODE.2 decompilation records remain controlling evidence, and the bound-module geometry below has now been re-derived directly from the immutable files.

## 1. Bound-image structure

Both shipped files are DOS/4GW Professional bound executables. Direct byte comparison shows that CODE.1 and CODE.2 share the same outer bound-loader image through file `0x25D23`, and both embed the Ravenloft linear-executable module beginning at file `0x25D24`.

The nested application module begins with an MZ header at:

```text
module origin = 0x25D24
```

Inside that nested MZ image, the dword at nested offset `+0x3C` is:

```text
e_lfanew = 0x1EB8
```

Therefore:

```text
0x25D24 + 0x1EB8 = 0x27BDC
```

and file `0x27BDC` is the LE header in both CODE files.

This is consistent with the DOS/4GW Professional architecture documented by Watcom/Tenberry: the Professional extender can be **bound to the application program file**, while DOS/4GW applications themselves use the Linear Executable format. Official references:

- Open Watcom Programmer's Guide, “Linear Executables”: https://openwatcom.org/ftp/manuals/current/pguide.pdf
- Open Watcom Programmer's Guide, DOS/4GW Professional differences: same manual, §14.2.
- IBM *OS/2 16/32-bit Object Module Format and Linear eXecutable Module Format*, Revision 10: https://openwatcom.org/ftp/devel/docs/lxomf.pdf

The IBM linear-executable specification defines `DATA PAGES OFFSET` as relative to the beginning of the EXE file containing the linear module. For these bound Ravenloft images, that origin is the nested module at physical file `0x25D24`, **not byte zero of the outer DOS/4GW Professional bound file**.

Accordingly:

```text
physical first data page =
    bound-module origin + LE DATA PAGES OFFSET
```

This is the missing structural explanation behind the previously certified object starts.

## 2. Shared bound-loader evidence

Direct comparison of immutable CODE.1 and CODE.2 establishes:

```text
outer/bound module origin       0x25D24
nested LE header               0x27BDC
first byte difference          0x27BF0
                               = LE header + 0x14
```

Thus the two files are byte-identical through the LE header's initial fixed fields and first diverge at the LE `MODULE # OF PAGES` field.

The shared outer image contains, among other DOS/4G/DOS/4GW strings:

```text
DOS/4GW Professional Protected Mode Run-time
Copyright (c) Rational Systems, Inc. 1990-1994
Feb 17 1994 11:54:25
1.96
DOS/4GW Professional
LINEXE_LOADER
RATIONAL DOS/4G
```

Those strings are direct shipped-byte evidence for the bound extender identity. They should not be confused with Ravenloft application strings or DreamForge-authored application logic.

## 3. CODE.1 certified map

Immutable CODE.1 identity:

```text
file size       0x43D1B / 277,787
SHA-256         fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed
module origin   0x25D24
LE header       0x27BDC
```

Relevant LE header fields:

```text
MODULE # OF PAGES       0x18
EIP OBJECT              1
EIP                     0xCC1C
ESP OBJECT              2
ESP                     0x9E90
PAGE SIZE               0x1000
OBJECT TABLE OFFSET     0xC4
OBJECT COUNT            2
DATA PAGES OFFSET       0x6A00
AUTO DATA OBJECT        2
```

Object table:

```text
Object 1:
    virtual size        0x11302
    relocation base     0x10000
    flags               0x2005
    first page index    1
    page-map entries    0x12 / 18

Object 2:
    virtual size        0x9E90
    relocation base     0x30000
    flags               0x2003
    first page index    0x13
    page-map entries    6
```

The first physical data page is therefore:

```text
0x25D24 + 0x6A00 = 0x2C724
```

This exactly reproduces the certified object-1 start.

Object 1 occupies 18 physical 4 KiB page slots, so object 2 begins at:

```text
0x2C724 + (18 * 0x1000) = 0x3E724
```

The fact that object 2 has virtual size `0x9E90` but only six page-map entries is not a contradiction: the LE object can contain a larger virtual region than the physically stored pages; the remaining virtual extent is loader-provided non-file-backed state such as zero-fill/BSS capacity.

### CODE.1 `CHARS`

`CHARS` occurs at file offset `0x3E7B8`.

Object-2 displacement:

```text
0x3E7B8 - 0x3E724 = 0x94
```

Correct mapped address:

**CODE.1 `CHARS` = object 2 + `0x94` = linear `0x30094`.**

The obsolete value `0x201DC` is incorrect and must not be used for xref work.

Canonical CODE.1 source evidence:

- https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/DECOMPILATION_CONTINUITY.md
- https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code1/FINAL_BINARY_AUDIT.md

## 4. CODE.2 certified map

Immutable CODE.2 identity:

```text
file size       0x88661 / 558,689
SHA-256         853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314
module origin   0x25D24
LE header       0x27BDC
```

Relevant LE header fields:

```text
MODULE # OF PAGES       0x4E
EIP OBJECT              1
EIP                     0x353A0
ESP OBJECT              2
ESP                     0x2DD10
PAGE SIZE               0x1000
OBJECT TABLE OFFSET     0xC4
OBJECT COUNT            2
DATA PAGES OFFSET       0x15200
AUTO DATA OBJECT        2
```

Object table:

```text
Object 1:
    virtual size        0x3A852
    relocation base     0x10000
    flags               0x2005
    first page index    1
    page-map entries    0x3B / 59

Object 2:
    virtual size        0x2DD10
    relocation base     0x50000
    flags               0x2003
    first page index    0x3C
    page-map entries    0x13 / 19
```

The first physical data page is:

```text
0x25D24 + 0x15200 = 0x3AF24
```

This exactly reproduces the certified object-1 start.

Object 1 occupies 59 physical 4 KiB page slots, therefore:

```text
0x3AF24 + (59 * 0x1000) = 0x75F24
```

which exactly reproduces the certified object-2 physical start.

### CODE.2 `CHARS` #1

File offset: `0x76634`

```text
0x76634 - 0x75F24 = 0x710
```

Correct mapped address:

**CODE.2 `CHARS` #1 = linear `0x50710`.**

### CODE.2 `CHARS` #2

File offset: `0x77064`

```text
0x77064 - 0x75F24 = 0x1140
```

Correct mapped address:

**CODE.2 `CHARS` #2 = linear `0x51140`.**

The obsolete values `0x49858` and `0x4A288` are incorrect and must not be used for xref work.

Canonical CODE.2 source evidence:

- https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/README.md
- https://github.com/Hyperbog/RLOFT/blob/373c7f10e6fc0f29c1e805125ef048150fcc7ba7/Code2/FINAL_CERTIFICATION.md

## 5. Why the correction matters

The original mapping mistake treated an LE offset without first establishing the origin of the **nested application module inside the bound DOS/4GW Professional image**.

The corrected model is:

```text
outer physical CODE file
    |
    +-- DOS/4GW Professional bound runtime
    |
    +-- 0x25D24 : nested application MZ/module origin
                    |
                    +-- +0x1EB8 -> LE header at 0x27BDC
                    |
                    +-- +DATA_PAGES_OFFSET -> physical data-page stream
```

This produces the certified object starts directly from the shipped bytes and explains why the prior false mappings put `CHARS` strings into executable object 1.

## 6. Certified xref anchors

Use only these values going forward:

- CODE.1 object-module origin: file `0x25D24`
- CODE.1 object 1: file `0x2C724`, vbase `0x10000`
- CODE.1 object 2: file `0x3E724`, vbase `0x30000`
- CODE.1 `CHARS`: file `0x3E7B8` -> object-2 `+0x94` -> linear `0x30094`
- CODE.2 object-module origin: file `0x25D24`
- CODE.2 object 1: file `0x3AF24`, vbase `0x10000`
- CODE.2 object 2: file `0x75F24`, vbase `0x50000`
- CODE.2 `CHARS` #1: file `0x76634` -> object-2 `+0x710` -> linear `0x50710`
- CODE.2 `CHARS` #2: file `0x77064` -> object-2 `+0x1140` -> linear `0x51140`

## 7. Evidence classification

- Bound-module origin `0x25D24`, nested MZ header, `e_lfanew=0x1EB8`, LE location `0x27BDC`, LE fields, object-table values, shared DOS/4GW strings: **BYTE-PROVEN** from immutable CODE.1/CODE.2.
- Physical object starts and virtual mappings: **CROSS-CORROBORATED** by direct header/object-table parsing and the independently certified decompilation extraction boundaries.
- Meaning of DOS/4GW Professional binding and general LE field terminology: **OFFICIAL-DOCUMENTED** by the Open Watcom/Tenberry and IBM format documentation cited above.
- Gameplay meaning of `CHARS`: established separately in `Runtime/CHARS_XREF_RECONSTRUCTION.md`; this map records only its physical/logical locations.

The shipped CODE bytes remain controlling evidence.
