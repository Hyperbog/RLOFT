# CODE.2 continuity addendum

This addendum supplements the exact copied `Code2/DECOMPILATION_CONTINUITY.md` with the current CODE.2-specific operating state.

CODE.1 is semantically closed. Current work is CODE.2. Read `Code2/README.md` and `Code2/PROGRESS.md` after the copied playbook. Use `Code2/Code2.cpp` as the canonical reconstruction source and syntax-check it with:

```sh
g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp
```

The immutable CODE.2 binary is controlling evidence. CODE.1/`Code1.cpp` should be used aggressively for byte-signature matching, runtime identification, calling-convention hypotheses, and reusable semantic patterns, but no CODE.1 address or semantic result is proof for CODE.2 until re-established from CODE.2 bytes.

## CODE.2 working rules

1. Verify the CODE.2 SHA-256 and regenerate object 1 before each substantive batch.
2. Regenerate authoritative disassembly for every candidate range.
3. Use CODE.1 matches as leads only; reconcile instruction-by-instruction against CODE.2.
4. Preserve widths, signedness, partial-register aliases, selector/far-pointer behavior, exact stack-slot consumption, wrap/truncation, shift masking, shared tails, callback/interrupt frames, REP string semantics, and entry-before-`DEC/JNE` loops.
5. Do not normalize address-distinct wrappers, error-mapping calls, global reloads across opaque calls, or self-modifying immediates.
6. Count only integrated verified instruction bytes or directly proven non-code/data. Staging and similarity matches remain uncounted.
7. Target substantial batches without stopping at isolated blockers; maintain an unresolved queue.
8. Commit substantive work and update `Code2/PROGRESS.md` before reporting.
9. Keep temporary scripts/staging out of the permanent folder after integration; Git history is the archive.

## CODE.1 evidence worth consulting

- `Code1/Code1.cpp`: completed semantic model and reusable boundary patterns.
- `Code1/FINAL_BINARY_AUDIT.md`: catalog of normalization mistakes caught by the end-to-end audit.
- `Code1/RESIDUAL_LEDGER.md`: evidence standard for code/data/padding classification.
- `Code1/FormatterMaterializer.map.md`: detailed runtime/formatter mapping.
- `Code1/PROGRESS.md`: final accounting model.

The CODE.1 audit specifically warns against wrong cdecl push/slot order, full-register substitution for byte/word writes, signed/unsigned branch drift, host C++ overflow/overshift UB, pre-checking encoded `DEC/JNE` loops, flattening selectors, losing DS/ES/SS in interrupt frames, caching globals the binary reloads, removing address-distinct thunks, simplifying self-modifying code, and bypassing encoded DOS/runtime error wrappers.

## Continuation behavior

When the user says `continue`, perform actual CODE.2 binary analysis and commit substantive work before replying. Begin by reading the copied playbook, this addendum, `README.md`, `PROGRESS.md`, and the current `Code2.cpp`; then verify the branch head and CODE.2 source identity, regenerate disassembly, work, syntax-check, commit, and report exact ranges/bytes/coverage/commit SHA.

## Batch 01 checkpoint

CODE.2 object-1 identity was reverified: SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`. The first substantive fold integrates **4,017 verified instruction bytes (1.68%)** at `00010..00072`, `00074..000B7`, `000B8..001F9`, `001FC..00270`, `35F88..369A6`, and `391F2..39525`.

The key accelerator is now proven, not hypothetical: CODE.2 `35F88..369A6` is the relocated timer/event family corresponding to CODE.1 `E3B4..EDD2`. All 303 differing bytes occur in 101 three-byte address fields, with exact shifts `+B164h` for DGROUP and `+27BD4h` for code; opcodes/control flow are otherwise byte-identical. Continue using CODE.1 aggressively for such anchor discovery, but always expand to complete CODE.2 function boundaries and verify the CODE.2 bytes before counting.

The interrupt adapter/veneer family at `391F2..39525` is also established. It corresponds to CODE.1 `FFA1..102D3`, except CODE.2's veneer-address LEA carries an explicit CS override and is therefore one byte longer. Preserve all 256 address-distinct veneers; vector 3 is `CC 90 C3`.

The next application code begins at `002A8`, following the still-unclassified `00271..002A7` alignment/table area. The startup entry at `353A0` is another high-value target but differs materially enough from CODE.1 startup that it must be reconciled as a CODE.2-specific path rather than mass-translated.


## CODE.2 Batch 02 checkpoint

Batch 02 added **4,615 translated bytes** plus **58 proven non-code/data bytes**. Cumulative accounting is **8,632 translated + 58 non-code = 8,690 / 239,698 = 3.6254% (3.63%)**; unresolved is **231,008 bytes**.

The `002A8..006B8` application dispatcher is now directly reconstructed. Its `00274..002A7` table is proven 13-entry data. For the large VGA/image/sprite family, direct CODE.1/CODE.2 instruction comparison established a fixed code shift `+0x1FC90` and graphics-DGROUP shift `+0xC4B8`; all integrated counterpart ranges have matching instruction boundaries/opcodes, with differences confined to relocated operands and self-patch addresses. Continue at `006C0`, startup `353A0`, and additional independently proven CODE.1 runtime anchors. Do not classify `006B9..006BF` merely because it is zero-filled.


## CODE.2 Batch 03 checkpoint

Batch 03 added **2,773 translated bytes** plus **19 proven alignment bytes**. Cumulative accounting is **11,405 translated + 77 non-code = 11,482 / 239,698 = 4.7902% (4.79%)**; unresolved is **228,216 bytes**.

The application path is now reconstructed through `011A4`; next code begins at `011A8`. High-risk details already reconciled include the `009B0` raw-mask fallbacks for tile kinds 2/3/4, partial-BX palette extraction in `00B6C`, fixed-point overlapping locals in `00BFC`, repeated `258DC` reloads, and the alternate-layout renderer's asymmetric type source (`6048+off+0E` for the `3C..43` range but `22246+off` for the `AD` test). Keep `006B9..006BF` unresolved until independently proven.


## CODE.2 Batch 04 checkpoint

Batch 04 added **4,127 translated bytes** plus **17 proven alignment bytes**. Cumulative accounting is **15,532 translated + 94 non-code = 15,626 / 239,698 = 6.5190% (6.52%)**; unresolved is **224,072 bytes**.

The application frontier now reaches `021D7`. The new block covers the 5760h packed text list, viewport/map redraw, cell classification at 1760h, UI initialization, in-place text editing, and pointer-driven insertion. Preserve the batch's explicit `258DC`/dimension reloads across opaque callbacks and the editor's mutation-only redraw behavior. Continue at `021D8`; `006B9..006BF` is still unresolved.

## CODE.2 Batch 05 checkpoint

Batch 05 added **5,134 translated bytes** plus **78 proven non-code/data bytes**. Cumulative accounting is **20,666 translated + 172 non-code = 20,838 / 239,698 = 8.6934% (8.69%)**; unresolved is **218,860 bytes**.

The sequential application frontier now reaches `03630`; `03631..03633` is proven alignment and the next routine begins at `03634`. The `02DB0..034AE` control-panel dispatcher uses the directly decoded 16-entry table at `02D70..02DAF`; entry 12 is the rectangle sentinel and action 11 intentionally addresses entry 11. Continue at `03634`, while keeping `006B9..006BF` unresolved until separately proven.

## CODE.2 Batch 06 checkpoint

Batch 06 added **4,031 translated bytes** plus **18 proven non-code/data bytes**. Cumulative accounting is **24,697 translated + 190 non-code = 24,887 / 239,698 = 10.3826% (10.38%)**; unresolved is **214,811 bytes**.

The sequential application frontier now reaches `04619`; `0461A..0461B` is proven alignment and the next routine begins at `0461C`. Preserve three important details from this batch: `759C` is initialized with byte indices, `04040` passes full fixed-point dwords into `2C8E4`, and `044D0` may consume repeatedly within one slot. Zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

## CODE.2 Batch 07 checkpoint

Batch 07 added **2,925 translated bytes** plus **3 proven non-code/data bytes**. Cumulative accounting is **27,622 translated + 193 non-code = 27,815 / 239,698 = 11.6042% (11.60%)**; unresolved is **211,883 bytes**.

The sequential application frontier now reaches `0518B`; `050D9..050DB` is proven alignment and the next routine starts at `0518C`. Preserve the three cleanup distinctions from `0461C`: the `04100` message path skips `1E74C`; random/party hard aborts execute only `1E74C`; normal escape/no-work completion performs the key drain/reporting epilogue before `1E74C`.

## CODE.2 Batch 08 checkpoint

Batch 08 added **3,656 translated bytes** plus **53 proven non-code/data bytes**. Cumulative accounting is **31,278 translated + 246 non-code = 31,524 / 239,698 = 13.1515% (13.15%)**; unresolved is **208,174 bytes**.

The sequential application frontier now reaches `06007`; the next routine begins at `06008`. Preserve the signed quantity accumulation in `05698`, the ESI-held signed record-type continuation gate in `05C84`, the distinct `05D34` continuation rule, and the ten-entry jump table at `05EC4..05EEB`. Zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

## CODE.2 Batch 09 checkpoint

Batch 09 added **3,188 translated bytes** plus **49 proven non-code/data bytes**. Cumulative accounting is **34,466 translated + 295 non-code = 34,761 / 239,698 = 14.5020% (14.50%)**; unresolved is **204,937 bytes**.

The sequential application frontier now reaches `06CAC`; `06C67` is proven alignment and the next routine begins at `06CAD`. Preserve the `06548` polling/redraw split, the nine-handle teardown (`12B30..12B50`, not `12B54`), and the save serializer's exact close-on-short-write behavior. The ten-entry CS table at `06520..06547` is fully reconciled. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

## CODE.2 Batch 10 checkpoint

Batch 10 added **2,493 translated bytes** plus **10 proven non-code/data bytes**. Cumulative accounting is **36,959 translated + 305 non-code = 37,264 / 239,698 = 15.5462% (15.55%)**; unresolved is **202,434 bytes**.

The sequential frontier reaches `075C8`; `075C9..075CB` is proven alignment and the next routine starts at `075CC`. `07938..079DF` was also translated because it owns the backward-jump shared load tail at `06CAD..0706D`. Preserve that ownership relationship: `06CAD` is not an independent callable function. The wildcard copier at `073A0` ignores source short-read results but requires the destination write count to equal the complete file size. Known zero islands remain unresolved.

## CODE.2 Batch 11 checkpoint

Batch 11 added **2,578 translated bytes** plus **38 proven non-code/data bytes**. Cumulative accounting is **39,537 translated + 343 non-code = 39,880 / 239,698 = 16.6376% (16.64%)**; unresolved is **199,818 bytes**.

The sequential application frontier now reaches `0808A`. `0808C..080AB` is the directly decoded eight-entry CS table for the dispatcher beginning at `080AC`; continue there. Preserve the `077A0` `A79D` clear-before-`171FC` / set-before-save ordering, `07874` fall-through ownership of `07938`, and the intentional right-click handle leaks in `079E0` and `07B94`. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

## Batch 12 checkpoint — 080AC..08CD7

Batch 12 advances the sequential application frontier through `08CD7`: 3,075 instruction bytes plus 41 directly proven non-code/data bytes. The major lift is the eight-way `080AC` left-panel dispatcher and the timed-status/moving-record family through `08C88`.

Fidelity notes:
- `080AC` keeps the selected row live across loop iterations when no full redraw occurs; the binary only recomputes that value on redraw or pointer movement.
- A press is accepted only after the encoded second `1A1F4` poll at `082B2`.
- `0871C` retains byte/word decrement widths and the four-slot bit-20/bit-21 global side effects.
- `0893C` uses signed deadline comparisons and obtains a fresh `19CC4` value only when rescheduling `21ACE`.
- `08A80` forms its tile word from the high 16-bit halves of the packed coordinates with 16-bit multiply truncation, and uses the exact nine-dword `086A0` sound-offset table without adding an invented bounds check.
- `08C88` reports an out-of-range signed index but then continues with the encoded compaction arithmetic.

Cumulative object accounting after Batch 12 is 42,612 translated + 384 proven non-code/data = 42,996 / 239,698 (17.94%). Next sequential frontier: the seven-entry table at `08CD8` and its owning dispatcher.


## Batch 13 checkpoint — 08CD8..094DF

Batch 13 adds **1,992 translated bytes** plus **64 directly proven non-code/data bytes**. Cumulative accounting is **44,604 translated + 448 non-code = 45,052 / 239,698 = 18.7953% (18.80%)**; unresolved is **194,646 bytes**.

The sequential application frontier now reaches `094DC`; `094DD..094DF` is proven alignment and the next routine begins at `094E0`. The two seven-entry CS tables at `08CD8..08CF3` and `0934C..09367` are fully decoded. Preserve these Batch 13 details: geometric contact in `08CF4` clears the result before the later type filter; mode-zero moving types `57h`/`59h` in `09070` clear bit 1 and return before proximity testing; damage halving is signed truncation toward zero; and `09418` uses signed division by three with the encoded single-step angular wrap. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 14 checkpoint

Batch 14 added **1,636 translated instruction bytes** plus **8 proven alignment bytes**. Cumulative accounting is **46,240 translated + 456 non-code = 46,696 / 239,698 = 19.4812% (19.48%)**; unresolved is **193,002 bytes**.

The sequential frontier now reaches `09B49`; the next routine begins at `09B4C`. Preserve these details: `094E0` may inspect record[count] when its order-word search misses; `095C0` has two address-distinct input polling loops; `096E0` chooses different word/coordinate pairs according to its mode; and `09790` keeps old slot words distinct from the two `F218` result bytes, with separate `A5h` and `80h` paths and partial-register side toggles. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 15 checkpoint

Batch 15 added **3,136 translated bytes** plus **8 proven alignment bytes**. Cumulative accounting is **49,376 translated + 464 non-code = 49,840 / 239,698 = 20.7928% (20.79%)**; unresolved is **189,858 bytes**.

The sequential frontier now reaches `0A790`; `0A791..0A793` is proven compiler alignment and the next routine begins at `0A794`. Preserve the signed-16 current/max comparison in `09D90`, its distinct `7Bh` message/recovery exits, the positional argument behavior in `0A254`, and the save/normalize semantics of `D681`/`D682`. The zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 16 checkpoint

Batch 16 added **3,449 translated bytes** plus **11 proven alignment bytes**. Cumulative accounting is **52,825 translated + 475 non-code = 53,300 / 239,698 = 22.2363% (22.24%)**; unresolved is **186,398 bytes**.

The sequential application frontier now reaches `0B515`; `0B516..0B517` is proven alignment and the next routine begins at `0B518`. Preserve the initial out-of-bounds hard return in `0B02C` (it skips the normal sound/redraw/type-cleanup tail), the inclusive world-record scan through `258A2`, the moving-type-`0Bh` special world-record path, and `0B410`'s exact two redraw pairs plus one final page flip after sound `+21h`.


## CODE.2 Batch 17 checkpoint

Batch 17 added **4,009 translated bytes** plus **534 proven non-code/data bytes**. Cumulative accounting is **56,834 translated + 1,009 non-code = 57,843 / 239,698 = 24.1316% (24.13%)**; unresolved is **181,855 bytes**.

The sequential accounting frontier is `0C6D7`; `0C6D7..0C703` remains deliberately unresolved, while the next directly decoded routine begins at `0C704`. Preserve `0B518`'s rank-indexed stack aliases (including counter/offset/threshold writes), 32-bit two's-complement absolute-value behavior, and the `E284` loop-counter argument. `0B950` owns the fully reconciled 132-entry table at `0B740..0B94F` and preserves its distinct no-selection return values and command-`58h` forced `FFFFFFFEh`. This batch also corrects Batch 16's `1C77C` boundary to preserve its second `0320h` argument; byte accounting is unchanged by that correction.


## CODE.2 Batch 18 checkpoint

Batch 18 added **3,567 translated bytes** plus **138 proven non-code/data bytes**. Cumulative accounting is **60,401 translated + 1,147 non-code = 61,548 / 239,698 = 25.6773% (25.68%)**; unresolved is **178,150 bytes**.

The sequential code frontier is `0D550`. The formerly unresolved `0C6D7..0C703` is now closed: nine padding bytes precede the exact nine-dword data block copied by `0CBD5`. Preserve the `0CD44` countdown-vs-damage-buffer distinction, `0CF88` divide edge, class-table behavior in `0D0B0`, and random-tie behavior in `0D1E0`. The next routine at `0D550` owns the four-entry CS table `0D540..0D54F`, which is already directly proven and accounted.


## CODE.2 Batch 19 checkpoint

Batch 19 added **4,071 translated bytes** plus **65 proven non-code/data bytes**. Cumulative accounting is **64,472 translated + 1,212 non-code = 65,684 / 239,698 = 27.4028% (27.40%)**; unresolved is **174,014 bytes**.

The sequential frontier now reaches `0E577`; next code begins at `0E578`. Preserve `0D550`'s shared error/sound tails and its three-argument entry contract, `0DDA4`'s separate track-indexed experience/class/level fields, the six-entry `0DC64` and eight-entry `0E008` CS tables, `0E0F8`'s three-attempt placement rule, and `0E284`'s five-argument stack contract plus its type-18 early revival return. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 20 checkpoint

Batch 20 added **4,194 translated bytes** plus **146 proven non-code/data bytes**. Cumulative accounting is **68,666 translated + 1,358 non-code = 70,024 / 239,698 = 29.2134% (29.21%)**; unresolved is **169,674 bytes**.

The sequential frontier now reaches `0F66B`; next code begins at `0F66C`. Preserve `0E5F0`'s nine-way `7E..86h` effect dispatch and special `4Ah`/`7Fh` paths, the 17 allocated `50h`-byte text buffers spanning `12BB0..12BF0`, the exact twelve-dword source block at `0EEB0`, `0EFA0`'s equipment/stat accumulation order, and the recursive item-tree semantics in `0F390/0F40C/0F4B0/0F540/0F5F8`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.
