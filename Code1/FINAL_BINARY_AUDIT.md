# CODE.1 final binary residual audit

## Source identity

- Original file: `CODE.1`
- Size: **277,787 bytes (`0x43D1B`)**
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- Object-1 file start: `0x2C724`
- Object-1 length: **70,402 bytes (`0x11302`)**
- Extracted object SHA-256: `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203`

## Final residual byte evidence

| Logical range | Original file range | Bytes | Classification |
|---|---|---|---|
| `04977` | `3109B` | `90` | NOP alignment |
| `087C2..087C3` | `34EE6..34EE7` | `8B C0` | MOV EAX,EAX alignment |
| `08E1F` | `35543` | `90` | NOP alignment before embedded table |
| `0CC19..0CC1B` | `3933D..3933F` | `00 00 00` | zero alignment before entry `CC1C` |
| `0D8CE..0D8CF` | `39FF2..39FF3` | `00 00` | zero alignment |
| `0DDD8..0DDDF` | `3A4FC..3A503` | `00 00 00 00 00 00 00 00` | zero alignment before `DDE0` |
| `0E3B3` | `3AAD7` | `00` | zero alignment before `E3B4` |
| `0F742..0F74F` | `3BE66..3BE73` | fourteen `00` bytes | zero alignment before `F750` |
| `0F7B3..0F7B6` | `3BED7..3BEDA` | `20 20 00 00` | embedded signature literal loaded by following parser |
| `0F7B7` | `3BEDB` | `90` | NOP alignment before `F7B8` |
| `0F8BD..0F8C0` | `3BFE1..3BFE4` | `20 20 00 00` | embedded signature literal loaded by following loader |
| `0F8C1..0F8C3` | `3BFE5..3BFE7` | `8D 40 00` | LEA EAX,[EAX+0] alignment |
| `0FC97` | `3C3BB` | `C3` | executable one-byte RET callback |

The `F7B3` and `F8BD` literals are confirmed by encoded `MOV EAX,ds:F7B3h` and `MOV EAX,ds:F8BDh` references in the immediately following routines.

## FC97 LE fixup proof

The LE header begins at original file `0x27BDC`. Its fixup page table is at header-relative `0x15D` and its fixup record table is at header-relative `0x1C1` (file `0x27D9D`). The record at fixup-table relative `0x4757` / original file `0x2C4F4` is:

`07 00 28 03 01 97 FC`

Interpreted using the IBM/Open Watcom LX/LE fixup record definition: `07` is a **32-bit offset source fixup**, source offset is `0328h`, target object is **1**, and target offset is **FC97h**. Thus `FC97` is an addressable executable callback and is represented as a one-byte no-op C++ function corresponding to its sole `RET`.

## Closed accounting

- Verified translated instructions: **69,361 bytes**
- Proven non-code/data: **1,041 bytes**
  - embedded tables/data: **204 bytes**
  - alignment/zero/padding: **575 bytes**
  - arithmetic-closed generic non-code: **262 bytes**
- Unresolved: **0 bytes**
- Total: **70,402 / 70,402 bytes**

This closes byte classification. It does not replace the separate final semantic audit of every translated instruction range against the original binary.
