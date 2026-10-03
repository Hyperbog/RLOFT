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


## Semantic audit checkpoint 5 — `6060..6853`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the VGA save/restore helpers, interrupt veneer, RNG/DOS wrappers, display initialization, and clipping wrappers through `6853`. Corrections preserve machine-level behavior rather than altering byte coverage. `6104` now performs the CEC0 DS restore before the reentrancy test, mirrors DS into ES, models the alternate SS:ESP/IF state, and passes exactly the five dword slots actually left for `50C0` instead of a fictitious sixth argument. `618A` and `61AC` retain the encoded `6184` state-address call/test. `6200`/`6220` preserve promoted dword stack slots while consuming only the encoded low word, and `6270` preserves the incoming upper 24 EAX bits because the binary writes AL only. `640C` now supplies both D906 arguments (`892Fh`, `50h`), while `6424` preserves signed-byte promotion and modulo-32-bit drive arithmetic. The display-line and clipping helpers now express ADD/SUB/NEG with explicit modulo-2^32 arithmetic so INT_MIN/overflow cases cannot invoke host C++ signed-overflow UB. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**.


## Semantic audit checkpoint 5b — `6854..71C7`

A direct byte-level follow-up closes the remainder of checkpoint 5. In both clipped-image paths (`6854` and `6A04`), the binary initializes the source-offset dword at clip `+0` to zero, not image width, and all ADD/SUB/IMUL address arithmetic is preserved modulo 2^32. `6BC0` uses the compiler's signed divide-by-four idiom, which is exactly C/C++ truncation toward zero; the prior extra negative bias therefore double-adjusted negative values and was removed. `64B0` was also corrected to test the full 32-bit argument slot for zero before replacing it from word `046C`; only the later call to `61AC` consumes the low word.

The DPMI wrappers were reconciled directly from their register frames. `6F10` and `6FF4` form their rounded signed dividend with 32-bit wrap and then divide by 16 toward zero. The promoted selector arguments to `6F74`, `6FAC`, and `6FC0` remain 32-bit cdecl slots with low-word consumption. A successful `6FF4` allocation writes a six-byte ownership header (`WORD selector`, then zero `DWORD`) at the returned linear base. A successful `709C` allocation writes `WORD 0` followed by the 32-bit DPMI handle at `+2`. `70FC` is not a selector allocator: it issues the DOS-memory query pattern and returns the largest available DOS block in bytes on the expected failure path, so its boundary is named accordingly. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 6 — `71C8..80B9`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the guarded allocator, buffered stream/RLE helpers, descriptor/image loaders, font/text renderers, tick handler, and keyboard queue through `80B9`. The stream helpers required several machine-level corrections: `7360` writes only DX with `MOVZX DX,byte` and therefore preserves incoming EDX high 16 bits; `74D0` consumes cdecl slots as `(count,dst)` and compares buffered availability with signed `JL`; both `738C` buffer-limit tests are likewise signed. `756C` returns by overwriting only AL of the incremented stream pointer, while `75F0`/`762C` use a modulo-32-bit product followed by a signed loop bound, so negative/high-bit totals perform zero iterations exactly as encoded.

The image helpers now use direct C++ signed `/8`, which matches the compiler's truncation-toward-zero sequence without the previous `INT_MIN` negation overflow. The centered/clipped text arithmetic in `7D80` and `7E6C` is performed through explicit 32-bit wrap before signed tests/shifts. `7A84`, `7AB4`, `7AFC`, `7C0C`, and `7CB0` preserve the binary's one-time null-pointer entry test rather than re-testing for null after pointer wrap. Finally, `7FA8`, `802C`, and `808C` compare each zero-extended queue byte against the full 32-bit argument; truncation to AL occurs only when a value is inserted. The remaining descriptor/font routines and the `7F5C` tick interrupt veneer were rechecked and required no further semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 7 — `80BC..87C1`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the keyboard/divide interrupt block and the adjacent object/page helpers through `87C1`. In `80BC`, the `CMP EBX,80h` / `JLE` branch is signed, so the release path is selected only when signed EBX is greater than 128; the previous C++ used an unsigned comparison. The `802C` call in the press path also does not supply a replacement scan code: its EAX result is ignored and the following stores use BL, so the boundary is now void and BL remains authoritative. `82C0` was restored to the encoded unconditional polling loop instead of inventing an escape when the host boundary is absent.

The drawing/object walkers were also tightened to preserve 32-bit machine arithmetic: `8514` computes endpoint/width expressions modulo 2^32 before signed use, while `85D4` and `864C` preserve wrapped stop accumulation, loop increments, and X-stride updates. `8270..8484`, `84A0..8513`, `85A8`, `862C`, `86A4..87C1`, and the divide/vector handlers were directly rechecked and required no other semantic correction. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 8a — `87C4..8A03`

Fresh direct disassembly of the immutable original `CODE.1` corrected the timing/input and formatting prelude before the asset loader. `87D8` now preserves the first nonzero signed mouse word in EBX, waits for a subsequent zero sample, and returns that saved value plus `1F8h`; the previous C++ discarded the saved word and also returned from the mouse path when the first sample was zero. `88D8` and `88F8` now execute the encoded DFFB/E098 conversions into `89F6` before returning that buffer address instead of being side-effect-free constants. `8978` now zero-fills the 80-byte `2AB1` buffer before digit conversion, reads the following signed 16-bit payload length into a stack-local word, and only then reads that many bytes into `2AB1`; the previous C++ conflated the stack local with DGROUP memory.

The surrounding tick setup, critical-section wrapper, sign helper, callback setters, and callback dispatcher were directly rechecked and require no further change in this sub-pass. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.
