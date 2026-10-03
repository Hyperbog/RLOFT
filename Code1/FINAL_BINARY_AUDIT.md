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


## Semantic audit checkpoint 8b — `8A04..8E1E`

Fresh direct disassembly of the immutable original `CODE.1` corrected the asset-table bootstrap and header loader through `8E1E`. `8A04` now constructs the encoded per-group filename in its 0x50-byte local buffer (conditional `2AB1` prefix plus literal `268h`, caller prefix, and formatted group index) and opens that local string rather than discarding `prefix` and opening the numeric group index. `8B94` now performs the encoded `9488` unlock followed by `91D8` release for every entry. The group/entry loops in `8A04`, `8B94`, `8BE4`, `8C40`, and `8CD4` preserve signed `JL` termination, so negative/high-bit counts perform zero iterations. `8C40` preserves the unconditional final `7240` call on the table pointer, including zero. `8D0C` now builds the full local fatal message from the 36-byte template at `028Ch`, formatted index, literal `02B0h`, and formatted kind before invoking the fatal path. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 8c — `8E3C..95A7`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the asset loader/cache lifecycle through `95A7`. `8E3C` now uses the encoded signed index-vs-count check, computes the nine-byte entry address with modulo-32-bit arithmetic, and preserves the `OR WORD PTR [entry],DX` side effect that clears entry byte `+1` while installing the type nibble before clearing the lock bit. The eviction scan `9378` and unlock sweep `94D0` now use signed `JL` loop termination, matching negative/high-bit group or entry counts rather than treating them as huge unsigned counts.

The two public cache entry points were also corrected to call the real internal loader. `9520` always invokes `8E3C(selected,0)` after `8B94`—including when `selected` is `FFFFFFFFh`, allowing `8E3C`'s bad-index path to own that case—then locks the selected entry with `9440`. `954C` likewise invokes `8E3C(index,group)` before returning the object pointer only when the entry state byte is exactly one. The previous generic release-boundary substitutions were not equivalent to these encoded loader calls. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 9 — `95BC..9BD6`

Fresh direct disassembly of the immutable original `CODE.1` reconciled startup selection, modal/error UI, and deferred-shutdown state through `9BD6`. `9724` now invokes the real `8E3C` loader after updating `2B10`, rather than substituting a generic release boundary. `9778` restores the encoded `CF98` call for its side effects before reading the configuration state.

`9874` now preserves its full screen-state machine: page selection from `23E5/23E6`, the `23D4==5` width adjustment, row-by-row `A05C` fill calls, signed centered-text division with the encoded `7D68(0,x)` argument order, slider clamp/save/restore using byte/word aliases at `23DF`, signed mode comparison, and restoration of both the captured page and prior text cursor coordinates. `9AE8` now calls `8960`, routes the `state&2` path to `5014(0Ah)`, builds the six-byte `02F8h` local prompt plus optional `2B0C` and literal `0300h` strings, and sends that local buffer through the modal boundary before applying the encoded exit/reset behavior. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 10a — `9BE0..A058`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the first planar-VGA block through `A058`. The dispatch wrappers `9BE0/9C44`, mode setup `9CA0`, mode restore `9DF4`, and planar read `9E60` were rechecked without structural change. `9E14` now performs the encoded dummy video-memory read that loads VGA latches before the write. `9E8C` now preserves its CS-relative byte lookup at `9EA3h+index` rather than masking the index to the four-byte nominal table; a code-segment read boundary makes that alias explicit, and the `9E10` tail alias forwards the same boundary.

`9EB0` was corrected at the mask/arithmetic level: the right-edge mask derives from `(h-1)-(plane^3)`, uses signed byte `SAR` on `F0h`, preserves the unsigned `JBE` single-byte case, rounds the middle span exactly as encoded, and applies the final stride decrement to `DI` only rather than the full dword. `A010` now calls `AF34` with the remaining byte count before each burst (including the zero-length case), uses the encoded signed `JLE` decision against `C0h`, and only then emits a `C0h` or final remainder burst. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 10b — `A05C..A219`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the complete planar line rasterizer. The prior lift rotated the sequencer plane mask through eight bits; the encoded loop is four-plane VGA progression (`1,2,4,8,1`) and advances the byte address on the `8 -> 1` wrap. `A05C` now preserves that four-plane step in both Bresenham branches and models the horizontal-span path directly with the encoded left/full/right map masks rather than expanding it into per-pixel writes.

The coordinate differences, negations, error accumulator, diagonal increment, and loop counters are now carried as modulo-32-bit values with explicit sign-bit tests, matching x86 `SUB/NEG/JNS/JLE/DEC/JNE` behavior without C++ signed-overflow undefined behavior. The `2B51` scratch map-mask word used by the encoded `XCHG` sequence is also preserved during general-line rasterization. Together with checkpoint 10a, the sequential semantic audit is now complete through `A219`; `A21A..A21B` is alignment. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 10c — `A21C..A6EF`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the page-copy, planar read/write, frame-transpose, and in-place frame-transform block through `A6EF`. `A2B0` now passes the encoded zero x-coordinate into `AEA8` in both directions; the first stack argument remains only the unsigned byte-count multiplier. The `A3E8`, `A484`, `A4E8`, and `A57C` loops now preserve the machine code's entry-before-`DEC/JNE` behavior, including zero counters wrapping through their full register width rather than silently becoming zero-iteration C++ loops.

`A5C8` was materially corrected: byte `+6` is doubled through `SHL DL,1` with 8-bit truncation, the row-span multiply truncates through AX, and the two swap passes use the encoded offsets `3*span + plane_bytes - 1` and `span + plane_bytes - 1`. The previous second pass incorrectly addressed one plane beyond the frame. `A678` is not a horizontal row mirror; it swaps top and bottom rows inside each of four planes, using `2*width` bytes per row and advancing each plane by the encoded bottom-row offset plus one row. All arithmetic/counters retain their original unsigned wrap widths. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**.


## Semantic audit checkpoint 10d — `A6F0..AEA5`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the three planar sprite renderers, both clipped sprite renderers, and the two in-place image translation helpers. The prior shared sprite abstraction incorrectly treated word `image+4` and byte `image+6` as one normalized width/row layout. `A6F0`, `A7E8`, and `A8F4` now preserve their distinct encoded transfer counts: `A6F0` uses `REP MOVSW` with the original byte field in the opaque path, while the subtracting and XLAT paths derive byte-loop counts from the low byte of the already-doubled field; their transparent paths retain the separate byte count. The original 16- versus 32-bit row counters and entry-before-`DEC/JNE` behavior are also explicit, including the `2B51` scratch write used by transparent paths.

`AA20` and `AC10` no longer share falsely identical clipping control flow. `AA20` loops over all requested leading clipped planes, whereas `AC10` performs only one leading-plane skip when the count is nonzero; `AC10` also truncates the per-plane row counter through `DX` while `AA20` uses full `EDX`. Both routines now preserve the entry-before-decrement row/pixel loops and the encoded `2B51/2B55..2B79` scratch state. `AE0C` and `AE5C` likewise use their encoded entry-before-`DEC/JNE` frame loops rather than a high-level zero-prechecked loop. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `AEA5`; `AEA6..AEA7` is alignment and the next code begins at `AEA8`.


## Semantic audit checkpoint 10e — `AEA8..B5E0`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the planar-address/page helpers, retrace/CRTC controls, formatter append helper, and external resource/audio bootstrap through `B5E0`. `AEA8` now masks both variable shift counts to x86's low five CL bits before each 32-bit `SHL`, avoiding host C++ shift undefined behavior for counts >= 32. `AF64` now preserves its unconditional `CLI`/two CRTC writes/`STI` sequence before the `AF34` retrace wait, including the encoded final interrupt-enable state.

`B01A` now performs the encoded word-granular `LODSW`/`STOSW` append: a source word whose low byte is NUL is still read but is not written over the destination terminator. `B060` restores the open-failure path that formats the data-object literal at `0318h` (`Timber file "%s" not found\n`) with the fixed `030Ch` descriptor, calls `9814` on the 0x100-byte local message, and continues to the encoded read/close path if that fatal boundary returns. In `B0F8`, the auxiliary-allocation cleanup no longer substitutes a normalized eight-selector loop: the machine code indexes `2BE8h` through live EAX, with EAX replaced by the optional `E6EA` return and by each `6F74` return, while EBX only counts eight iterations. Finally, `B4D4` now retains the 32-bit `MOVSX byte` versus `MOVZX word` comparisons, so negative record bytes cannot spuriously match `FFxxh` caller IDs. `B430`, `B51C`, `B574`, and the signed-rate helper `B598` were rechecked directly and required no further semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `B5E0`; the next code begins at `B5E4` after the `B5E1..B5E3` LEA alignment.


## Semantic audit checkpoint 10f — `B5E4..BE60`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the remainder of the audio/resource state block through `BE60`. `B5E4` now preserves the encoded early exit when `B4D4` cannot resolve an `ECBB` subrecord: that path returns immediately and therefore does **not** set the slot-loaded byte at `2C28h+slot`; only the `FFFFh` enumeration terminator reaches the loaded-flag store. The routine also reloads `2B90h` and the slot handle for each external enumeration/attach call, matching the machine code rather than retaining pre-call copies across opaque boundaries.

The playback-control routines `B6EC`, `B76C`, `B824`, `B8F4`, `BA98`, `BB80`, `BCB4`, and `BD20` now likewise preserve the binary's repeated reloads of `2B90h`, `2C30h`, `2C40h`, and per-slot handles after external audio calls. This matters because the assembly deliberately re-reads those globals between state, stop, rate, pan, start, and polling calls instead of assuming they are unchanged. Calls to `B598` are materialized at their encoded points before the corresponding `ED29` arguments are loaded.

`BDE4` was materially corrected: after `ED0B`, states `0` and `2` map to return value `1`, but every other state returns the original `ED0B` value still in EAX. The previous C++ incorrectly collapsed all nonterminal states to zero. `B6C0`, `B74C`, `B75C`, `BA04`, `BBEC`, `BC1C`, `BD84`, `BDCC`, `BDD8`, `BE28`, `BE34`, and `BE40` were rechecked and required no further semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `BE60`; `BE61..BE6F` is a fifteen-byte zero/alignment island and the next code begins at `BE70`.


## Semantic audit checkpoint 10g — `BE70..C79E`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the palette-transition and second external-resource subsystem through `C79E`. `BE70` now reads each component once during its max scan, matching the encoded load/compare/update sequence. More importantly, `BF38` and `BFE0` no longer misinterpret their third dword argument as a ready-made count: the machine code converts the inclusive final palette index to `count = last - first + 1` before every fade step and source-segment restore.

The helper loops were hardened to the actual signed branches. `C0B8` and `C110` use signed `JL` termination, so zero/high-bit counts are zero-iteration rather than enormous unsigned loops; `C110` also reproduces 32-bit IMUL wrap and x86's masked arithmetic-SAR count. `C1B0` uses the encoded signed `run <= 1` gate and routes its overlapping middle move through the existing `D9EE` copy boundary.

`C2D0` now issues the two distinct `627C(path)` calls present in the binary—one before allocation and a second immediately before the file read—instead of caching one size result. Its failure cleanup order is path-accurate: an `EA0B == FFFFFFFFh` result frees selector `2F70h` before calling `E6EA(0)`, whereas the type-check and `EAE7` failures call `E6EA(0)` first and then free the selector. `EAE7/EAF1` reload `2F74h` at their encoded call sites. `C4E0` and `C57C` likewise re-read `2F74h`/`2F68h` after opaque subsystem calls instead of retaining stale cached values. The remaining routines `C07C`, `C17C`, `C228`, `C254`, `C4A0`, `C5B0`, `C608`, `C62C`, `C650`, `C660`, `C690`, `C6F0`, and `C6F8` were rechecked directly and required no additional semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `C79E`; `C79F` is a zero alignment byte and the next code begins at `C7A0`.


## Semantic audit checkpoint 10h — `C7A0..CEBF`

Fresh direct disassembly of the immutable original `CODE.1` reconciled all four position-dependent self-modifying scaled VGA blitters, the translation-table copy/RET callback, termination chain, startup recovery prelude, PSP/environment materializer, and startup exit path through `CEBF`. The blitter patch model itself was confirmed: `C7A0/C8C4` rewrite the low 16 bits of their scale immediates and sequencer-mask byte in place, while `C9D4/CACC` patch the separate outer/inner scaling immediates, plane stride, frame-minus-one, key byte, and map-mask fields exactly as represented by the C++ locals. Translated `C7A0` compares the XLAT result with the source key, `C8C4/C9D4` compare the raw byte, and translated `CACC` writes the table result unconditionally, matching the four encoded variants.

The previous shared helpers did, however, normalize the patched ECX loops into pre-checked C++ `for` loops. The machine code enters both outer and inner bodies before `DEC ECX/JNE`; a zero 16-bit patched count therefore executes once and then wraps through the full 32-bit ECX cycle rather than becoming a zero-iteration loop. Both shared models now use explicit entry-before-decrement `do/while` counters while preserving all 32-bit accumulator wrap and unsigned `JB/JAE` scale tests. `CBD4`, `CBEF`, and `CBF0..CC18` were rechecked and required no semantic correction; the classified `CC1E..CC95` Watcom copyright/string block remains data reached over by the two-byte `CC1C` jump.

The startup continuation `CC96..CEBF` was also rechecked directly. `CD97` had regressed on the environment trailer: once the double-NUL is found, the binary executes `LODSB` plus two `INC ESI` instructions, advancing three bytes from the second NUL before copying the executable path. The C++ now restores that exact `ESI += 3` transition rather than `+2`. The PSP command-tail `REPZ SCASB`, zero-fill cap, FCE8/FC98 calls, and CE8B/CE8E shared termination behavior otherwise reconcile without further changes. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `CEBF`; the next code begins at `CEC0`.


## Semantic audit checkpoint 10i — `CEC0..DDD7`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the runtime register adapter, mouse/DOS file wrappers, directory helpers, allocator retry wrapper, dictionary decoder, and image permutation transform through `DDD7`. `CEC0` is now modeled for its actual sole side effect (`MOV DS,CS:[CC90h]`) rather than as a fictitious value-return helper. The normal `CF98` return path also preserves its encoded EAX value `8A5Ah`.

`D4F2` now carries the full 16-bit DX result and carry state across both `AX=4400h` and `AX=4401h` IOCTL boundaries. Error mapping therefore receives the encoded DX word instead of the previous low-byte-only mode value. In text-mode `D5AD`, Ctrl-Z ends compaction of the current DOS-read chunk but still reaches the common kept-byte accounting and possible next read; the prior lift returned immediately at Ctrl-Z and changed the stream-consumption semantics.

`D906` was restored to its two-argument ABI `(destination,count)`: it builds the contiguous `drive:\\current-directory` stack string, records runtime error 1 on the DOS current-directory failure, allocates exactly `strlen+1` bytes and records error 5 on allocation failure only when destination is null, and otherwise passes the caller's exact count through the encoded `1047E` bounded-copy/zero-pad behavior. `D9EE` now performs an explicit forward copy instead of invoking C++ `memcpy` on overlapping regions, preserving the deterministic machine result used by existing overlapping callers.

`DAD9` now resets its one-shot `10AA9` retry flag when a successful `10C46` retry returns through `DAF1`, so subsequent failed allocation attempts can invoke `10AA9` again exactly as encoded. Finally, both `DC90` loops driven by `DEC ECX/JNE` are entry-before-decrement loops; zero initial table/stride counts now retain their full dword wrap behavior rather than being normalized to zero iterations. The intervening mouse, DTA, open/create, seek/delete, DOS-memory, and `DB80` dictionary paths were rechecked directly and required no additional semantic change. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `DDD7`; `DDD8..DDDF` is the known eight-byte zero-alignment island and the next code begins at `DDE0`.


## Semantic audit checkpoint 10j — `DDE0..E3B2`

Fresh direct disassembly of the immutable original `CODE.1` reconciled the low-level port/vector wrappers, Watcom fill and integer-conversion runtime, object clone/constructor helpers, INT 24h installation/trampoline, and signed-decimal parser through `E3B2`. The source object hash was reverified as `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203` before this pass.

`DEA7` retained the correct 32-byte alignment and unrolled dword-fill result but had normalized away an encoded read side effect. At `DECE` the binary executes `CMP BYTE PTR [EAX+20h],DL` before the next unrolled stores; its flags are subsequently overwritten, but the memory access itself is architecturally observable through fault/touch timing. The C++ now retains that look-ahead byte read at the exact odd-group points while preserving the original store order and tail count. `DDE0..DE35`, `DE70..DEA0`, `DF13`, and `DF73` otherwise rechecked cleanly.

The paired unsigned/signed radix converters `DFB3/DFFB` and `E050/E098` were rechecked instruction by instruction and against the Open Watcom runtime implementation in `bld/clib/convert/c/ltoa.c`. The compiler helper resets the remainder cell from the radix before each unsigned divide, matching the two address-distinct binary variants; the existing standard base conversion and decimal-only sign handling therefore remain correct and required no semantic change.

The major correction is the `E2C3` INT-24 IRET trampoline. The binary saves the interrupt frame, copies saved EAX into working EDX before restoring DS, then calls the stored far callback with exactly four cdecl dwords: `low16(saved EAX)`, `low16(saved EDI)`, the BP:SI-derived linear address, and the selected segment value (`DS` in mode 1, otherwise `34h`). The prior C++ exposed five callback arguments, incorrectly used saved EDX as the first argument, shifted the remaining slots, and left DS/ES changed after the callback. The corrected boundary uses the four encoded arguments and restores the interrupt caller's original DS and ES after restoring SS:ESP and the runtime stack-top global. `E280` installation logic and `E360..E3B2` decimal parsing rechecked without further changes.

`E0D0`, the `E174/E1F4/E26C` free wrappers, and the address-distinct `E190/E208` constructor-like readers were also rechecked directly. The known non-code islands inside the span (`DE6F`, `DEA1..DEA6`, the two 37-byte radix tables at `DF8E..DFB2` and `E02B..E04F`, `E0C8..E0CF`, `E187..E18F`, `E1F1..E1F3`, `E207`, `E269..E26B`, and `E27F`) remain unchanged. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `E3B2`; `E3B3` is the verified one-byte zero-alignment boundary and the next code begins at `E3B4`.


## Semantic audit checkpoint 10k — `E3B4..EDE6`

Fresh direct disassembly of immutable `CODE.1` reconciled the callback dispatcher, IRQ-8 handler/chain trampoline, timer/PIT subsystem, registration/event subsystem, all 48 fixed event wrappers, and the seven-byte `EDE0` frame no-op. The source and extracted-object hashes were reverified before comparison.

`E3E0` is a pure stack-forwarding dispatcher: the machine always reads the first caller dword at `[ESP+4]` to select the slot and has no encoded argument-count precheck. The C++ no longer invents a zero-count early return.

The largest correction is the split between `E3F2` and `E4A7`. A reentrant `E3F2` entry (`WORD 510E != 0`) jumps directly to the common restore/EOI/IRET tail; it does **not** chain to the previous INT-8 vector. The old-vector chain is the separate `E4A7..E4C6` far trampoline whose address is stored at `5150` by `E514`. That trampoline loads the saved vector from `5202:5206`, swaps those values into the saved ECX/EAX frame slots, restores the remaining registers, and executes `RETF`. The reconstruction now represents `E4A7` as its own address-distinct chain boundary and leaves reentrant `E3F2` on the encoded handled/EOI path.

`EA0B` was also corrected to preserve DGROUP `52DA` as the actual registration-slot scan cursor. The binary initializes it to zero, increments the global after each occupied slot, leaves it at 16 on exhaustion, and returns the global value after a successful descriptor/version check; the former C++ used only a local loop variable and lost those observable writes.

Finally, `EBF3..EDD2` is no longer represented solely by a normalized indexed helper. All 48 ten-byte `MOV EAX,event ; JMP E3E0` thunks now have address-distinct C++ entry points while retaining the verified event sequence. `E4C7..EA0A`, `EA9A..EBF2`, and `EDE0..EDE6` otherwise rechecked cleanly; `EDD3..EDDF` remains the known thirteen-byte zero-alignment island. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `EDE6`; the next code begins at `EDE7`.


## Semantic audit checkpoint 10l — `EDE7..FC97`

Fresh direct disassembly of immutable `CODE.1` reconciled the complete formatter driver/parser/materializer and helper block `EDE7..F741`, the source reader and resource-size parser `F750..F8BC`, the resource image loader/relocator `F8C4..FC96`, and the LE-relocated one-byte `FC97` RET callback. The original file and extracted-object SHA-256 values were reverified before this pass.

Three semantic corrections were required. `F163` and `F18C` are not ordinary bounded strlen loops: the machine loads the current byte/word first, advances its local source pointer, and only then compares the already-counted length against the limit. Consequently a non-NUL string incurs one look-ahead memory read at `limit` (including when the limit is zero). The prior C++ prechecked `n != limit` and omitted that architecturally observable read. Both helpers now preserve the encoded read-before-limit-test ordering while returning the same count.

`F206`'s 16.16 fixed-point fractional-digit loop uses `CMP EAX,[precision]` / signed `JL`. The previous C++ used an unsigned `i < precision` loop, which diverged if a parsed precision wrapped into the signed-negative range. The loop now compares the modulo-32-bit counter and precision as signed 32-bit values exactly as encoded. The surrounding sign handling, decimal-point generation, fractional multiply/truncate sequence, and carry/rounding propagation were rechecked unchanged.

In `F8C4`, the page-data offset uses `SHL EAX,CL`; x86 masks the shift count to five bits. The previous C++ shifted by the raw header byte and could invoke host undefined behavior for counts of 32 or more. The reconstructed expression now applies `page_shift & 31` before the 32-bit shift. The object/page loops, 16-byte alignment gate, virtual-size clipping, fixup validation, relocation patch addresses, and cleanup paths were otherwise rechecked directly.

`EDE7..F006` retains the encoded `%n` near/far and word/dword store variants, two-byte `s/S` source stepping, prefix/zero/field padding loops, and the shared `F001` epilogue relationship. `F007..F162`, `F1B3..F2E8`, `F2E9..F721`, `F722..F741`, `F750..F7B2`, and `F7B8..F8BC` were otherwise instruction-reconciled without further semantic change. The known non-code/alignment islands `F742..F74F`, `F7B3..F7B6`, `F7B7`, `F8BD..F8C0`, and `F8C1..F8C3` remain unchanged. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**, with **0 unresolved bytes**. The sequential semantic frontier is now `FC97`; the next code begins at `FC98`.
