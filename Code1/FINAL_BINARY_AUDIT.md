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

## End-to-end semantic audit checkpoint 1 — `00000..01463`

The first final-audit region was regenerated directly from the immutable object with `objdump -D -b binary -m i386 -M intel` and reconciled against the current `Code1.cpp`. The complete `0x1464`-byte span closes as **5,193 translated instruction bytes + 27 alignment bytes = 5,220 bytes**, with no gaps or overlaps.

Instruction-accounted routines checked: trap stub `0000..0002`, accumulation helper `0010..0048`, indexed record reader `004C..00EA`, text/layout primitive `00EC..035B`, hit-test `035C..043A`, input/event poller `043C..04AF`, UI painter `04B0..0648`, slot renderer `064C..07E1`, transition renderer `07E4..10A1`, and presentation routine `10A4..1461`.

The audit rechecked direct call targets and argument order, signed comparisons/divisions, modulo/overflow behavior, DGROUP widths and offsets, the `831C` double-poll path, the reverse XOR record loop, the transition counter termination at `[last-1] == 0Bh`, `9C44` slide-call argument order, and the signed interpolation arithmetic in `10A4`. No semantic discrepancy was found.

Directly reverified non-code in this region: `0003..000F` NOPs, `0049..004B` LEA alignment, `00EB` NOP, `043B` NOP, `0649..064B` LEA alignment, `07E2..07E3` MOV EAX,EAX, `10A2..10A3` MOV EAX,EAX, and `1462..1463` MOV EAX,EAX. Coverage/accounting is unchanged by this semantic audit.


## Semantic audit checkpoint 2 — `1464..38A3`

Direct re-disassembly from the immutable original `CODE.1` found and corrected semantic mismatches in the already-counted C++ without changing byte coverage. Corrections include: encoded `954C` argument order in `1464`; exact `1464` caller slot/label placement in `1CD0`, `1DD8`, `1E64`, `2A64`, `3020`, and `3138`; the missing `00EC` materialization and corrected renderer argument order in `1A04`; the missing `04B0`/page flip sequence in `1BD4`; two-page animation state, renderer targets, and call argument order in `15B0` and `24D4`; direct hit-test/hover paths in `2A64`; timing/input details in `2DFC`; the missing `7868` recapture in `3020`; signed `IDIV` semantics in `3208`/`3404`; and the 16-bit store/reload truncation points in the `3404` HP clamps.

The remaining routines in this checkpoint, including `1F24`, `2198`, `266C`, `28E4`, `2994`, `32F4`, `334C`, `37C0`, and `3884`, were rechecked against fresh disassembly and required no correction in this pass. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**. This checkpoint changes semantics only; it does not increase coverage.


## Semantic audit checkpoint 3 — `38A4..4976`

Fresh direct disassembly of the original `CODE.1` reconciled the stat renderer/editor and the presentation loop through `4976`. `38A4`, `44E4`, and `4520` required no semantic change. Corrections in `3C58`, `40FC`, and `45FC` preserve two machine-level details that had been flattened incorrectly: `67C8` is called cdecl as `(0, x, y, image_or_capture)` (the previous C++ placed the image/capture in the first slot), and DGROUP `7138` is deliberately read at mixed widths. Calls to `6588`/`65A4` use `MOVZX BYTE PTR [7138]`, while `55DC` and `53E4` receive `PUSH DWORD PTR [7138]`; `A390` receives the full dword after XORing only AL.

The corrected C++ now preserves those exact argument slots and width aliases. The two nested animation loops, `44E4` counter wrapping, `4520` resource calls, capture dimensions, `87D8` cadence, and `C4E0` trigger frames were also rechecked and remain unchanged. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 4 — `4978..6053`

Fresh disassembly from the immutable original `CODE.1` reconciled the post-generation controller, palette transforms/tree builder/search, cursor state routines, class-family mapper, and runtime-table population through `6053`. Three semantic corrections were required. In `5B64`, the final return is a partial-register operation: the binary sign-extends the selected node into EAX and then overwrites only AX with the palette index; the initialized-tree no-candidate sentinel therefore returns `0000FFFFh` (65535), not sign-extended `FFFFFFFFh`. In the final `5F4C` family comparison, BL enters the actor-0 `5D90` call as `2` from the completed actor loop, then BL is replaced by actor 0's returned family before the actor-1 call; the previous C++ incorrectly supplied fallback values 0 and 1. Finally, the encoded `6A80` invalidation loop does not skip negative words: it sign-extends the word, multiplies it by 65, and probes the resulting address exactly as encoded.

The cursor/page routines `50C0..5878`, tree insertion/build routines `5880..5B63`, the class-family decision tree `5D90`, and the remaining resource/palette routines in this checkpoint were rechecked. The unusual `MOVZX r16` sequences in `59AC`/`5B64` were retained or shown equivalent at the low-word comparison points rather than normalized by assumption. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 4b — direct recheck of `50C0..5B63`

A deeper direct recheck of the immutable original `CODE.1` found additional fidelity defects inside the region previously marked clean by checkpoint 4. This correction pass fixes five encoded details without changing byte coverage: `50C0` now consumes five 32-bit stack slots and reads its fifth argument as a guest-linear address constructed by the `6104` interrupt veneer; its initial `SAR` and signed word-depth gate are modeled exactly. `5230` retains its promoted 32-bit input slot while storing only AX. `5880` now exposes the compiler's 9-byte by-value record as three consumed dword slots plus the following 32-bit record-index slot, including the source-index word straddling slot0/slot1. `59AC` uses unsigned modulo-2^32 arithmetic to preserve IMUL/add wrap before the signed low-word comparison. `5A34` reproduces the actual by-value stack image (`green, red, blue, source-index, zero padding`) used by both callers of `5880`. `4B18`'s partial-register operations were rechecked and are equivalent because ESI's upper word is proven zero before the only full-width subtract; its word truncations remain value-preserving in the encoded 0..63 domain. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**.
