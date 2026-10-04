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

## Mandatory interruption/resume safety protocol

Before continuing after any interruption, context/thread rollover, timeout, failed workflow, or uncertainty about the prior stopping point, read and follow `Code2/INTERRUPTION_RESUME_PROTOCOL.md` and `Code2/RESUME_CHECKPOINT.md`. Do not trust chat-only frontier state. Re-establish live HEAD and accounting from the repository, reverify immutable CODE.2/object-1 hashes, and re-disassemble a small overlap spanning the last committed frontier before moving forward. The overlap is verification only and must never be counted twice. If the overlap, instruction boundary, ownership, address convention, or accounting disagrees with the committed checkpoint, freeze forward progress and repair the prior batch first.

After every substantive batch, refresh `RESUME_CHECKPOINT.md` with the last substantive commit, exact cumulative accounting/frontier, and a SHA-256 for a small binary overlap window around the new frontier. Mark any analysis beyond the committed frontier as `UNCOUNTED / NOT COMMITTED`.

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


## CODE.2 Batch 21 checkpoint

Batch 21 added **2,743 translated bytes** plus **53 proven non-code/data bytes**. Cumulative accounting is **71,409 translated + 1,411 non-code = 72,820 / 239,698 = 30.3799% (30.38%)**; unresolved is **166,878 bytes**.

The sequential frontier now reaches `10155`; `10156..10157` is proven alignment and the next routine begins at `10158`. Preserve `0F790`'s ten-entry CS item-type dispatch (`2..4`, `5..8`, `9..11` families), recursive type-1 container weight and half-unit suffix behavior, `0FD84/0FE44` burden/rank thresholds, `0FF3C`'s portrait/HP/status presentation, and `100C8/10108`'s `BF44=3Ah` range gate plus `C050` toggle. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 22 checkpoint

Batch 22 added **3,662 translated bytes** plus **6 proven alignment bytes**. Cumulative accounting is **75,071 translated + 1,417 non-code = 76,488 / 239,698 = 31.9102% (31.91%)**; unresolved is **163,210 bytes**.

The sequential frontier now reaches `10FA8`; `10FA9..10FAB` is proven alignment and the next routine begins at `10FAC`. Preserve `10158`'s paired status lists and three-track class/XP/level display, its HP-ratio divide edge and threshold color selection, `10A70`'s mode-3 table/list geometry, and `10BBC`'s side-dependent equipment coordinates plus exact sprite flip/unflip around mirrored equipment slots. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 23 checkpoint

Batch 23 added **3,174 translated bytes** plus **42 proven non-code/data bytes**. Cumulative accounting is **78,245 translated + 1,459 non-code = 79,704 / 239,698 = 33.2518% (33.25%)**; unresolved is **159,994 bytes**.

The sequential frontier now reaches `11C2B`; `11C2C..11C3B` is a proven four-entry CS table and the next routine begins at `11C3C`. Preserve `10FAC/110B4`'s four `C050` page cycles, `11404`'s four-way type table and raw shifted variant-byte flag update, `11630`'s exact `EEB0` range mapping plus encoded stale-local boundary for invalid external input, `117A0`'s store-before-`0F238` behavior and double `50DC` call, and `11AD8`'s low-word-vs-low-byte toggle asymmetry. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 24 checkpoint

Batch 24 added **3,364 translated bytes** plus **40 proven non-code/data bytes**. Cumulative accounting is **81,609 translated + 1,499 non-code = 83,108 / 239,698 = 34.6720% (34.67%)**; unresolved is **156,590 bytes**.

The sequential frontier now reaches `12985`; `12986..12987` is proven `MOV EAX,EAX` alignment and the next routine begins at `12988`. Preserve `11C3C`'s encoded grid geometry, same-container rejection, capacity arithmetic, and byte-shift variant semantics; `12180`'s six-way type table plus empty-cursor hitbox scan and slot-0F container close/reopen behavior; `12470`'s complete 248h actor-block swap, four-byte panel-state swap, and 3-bit actor-index rewrite in both record families; and `12724`'s sparse status-bit scan, which stops after fourteen displayed statuses or bit `02000000h`, followed by its two distinct modal wait phases. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 25 checkpoint

Batch 25 added **4,052 translated bytes** plus **80 proven non-code/data bytes**. Cumulative accounting is **85,661 translated + 1,579 non-code = 87,240 / 239,698 = 36.3958% (36.40%)**; unresolved is **152,458 bytes**.

The sequential frontier now reaches `139AA`; `139AB` is proven `NOP` alignment and the next routine begins at `139AC`. Preserve `12988/12C20`'s two-phase modal input waits and page-toggle redraws, `12E70`'s ordered overlapping hit regions and cleanup gating, `13268/13284`'s x86 partial-register return semantics, `1330C/133CC`'s compact stream decode state in `7F88`, `13488`'s unsigned countdown/seek behavior, and `135C4`'s exact ten-way CS packet dispatch. `131D0` is referenced `CINE00.ANM` data, not code. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 26 checkpoint

Batch 26 added **3,837 translated bytes** plus **71 proven non-code/data bytes**. Cumulative accounting is **89,498 translated + 1,650 non-code = 91,148 / 239,698 = 38.0262% (38.03%)**; unresolved is **148,550 bytes**.

The sequential frontier now reaches `148ED`; `148EE..148EF` is proven `MOV EAX,EAX` alignment and the next routine begins at `148F0`. Preserve `139AC`'s signed decimal filename substitution into `CINE00.ANM`, `13AD8`'s mode-`0Ch` stale-stack output boundary, `13C88`'s exact nine-way teardown table and `-2/-1` restore sentinels, `13FD4`'s in-place word-wrap terminator/restore behavior, `14118`'s party-count-dependent `[left|right]` text selection, `1430C`'s first-matching condition scan through terminator `21h`, and `146DC`'s two distinct wait paths plus `7FD4`-dependent preview remap. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 27 checkpoint

Batch 27 added **2,904 translated bytes** plus **68 proven non-code/data bytes**. Cumulative accounting is **92,402 translated + 1,718 non-code = 94,120 / 239,698 = 39.2661% (39.27%)**; unresolved is **145,578 bytes**.

The sequential accounting frontier now reaches `1548B`; the next unresolved ownership unit begins at `1548C`, with the owning dispatcher directly decoded at `15534`. Preserve `148F0`'s class-dependent activation side effects, the low-byte `0080h` test in `14A60` versus the distinct word `8000h` tests later in the batch, `14ADC`'s live-EBX second argument into `1C160`, `14E9C`'s asymmetric A6 handling, `15050`'s full-width item value until the encoded low-word store, `15280`'s second-pass encoded no-op flag clear, and `153FC`'s once-per-cursor recursive presence count versus multiplicity counting for party inventory. The table at `1548C` is not yet counted. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 28 checkpoint

Batch 28 added **2,301 translated bytes** plus **175 proven non-code/data bytes**. Cumulative accounting is **94,703 translated + 1,893 non-code = 96,596 / 239,698 = 40.2990% (40.30%)**; unresolved is **143,102 bytes**.

The sequential frontier now reaches `15E37`; `15E37` is proven `NOP` alignment and the next routine begins at `15E38`. Preserve `15534`'s exact 42-way `00h..29h` CS dispatch, opcode `0Bh`'s live-EBX second argument into `1C160`, opcode `08h`'s distinct `FFFFh` fifteen-item condition, zero-extended item words passed to `0F4B0/0F540`, opcode `0Fh`'s five 12-byte timer slots and `FFFFh -> FFFFFFFFh` disable conversion, opcode `12h`'s nested interpreter with `7FB8` gating and stream-position restore, opcode `23h`'s unsigned low-32 multiply plus logical `>>15` scale, and opcode `24h`'s forced return value `3` after `13EF8`. `1548C..15533` is now proven table data. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 29 checkpoint

Batch 29 added **3,129 translated bytes** plus **23 proven non-code/data bytes**. Cumulative accounting is **97,832 translated + 1,916 non-code = 99,748 / 239,698 = 41.6140% (41.61%)**; unresolved is **139,950 bytes**.

The sequential frontier now reaches `16A84`; `16A85..16A87` is proven `LEA EAX,[EAX+0]` alignment and the next routine begins at `16A88`. Preserve `16400`'s non-contiguous ownership of the backward blocks at `15F7E..16033` and `163C4`'s fall-through into that renderer; `16034`'s unsigned deadline comparison and save/restore of `7FEC`; `1608C`'s permanently `-1` reset selection plus dword-width `C050` XOR after a completed choice; `1624C`'s five-item spawn and encoded `(count-index+1)*16h` record copy without decrementing `258A6`; `16524`'s live-EBX second argument to `1C160(4Eh, EBX)`, selective graphics-state restore, and exact `D680/BF38` byte restoration; the zero-count handle leaks in `16810` and `168C8`; the two-byte/full-dword count behavior in `1686C/16934`; and `16A58`'s stack-slot alias that replaces only the low 16 bits of the handle value before returning it. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 30 checkpoint

Batch 30 added **1,879 translated bytes** plus **29 proven non-code/data bytes**. Cumulative accounting is **99,711 translated + 1,945 non-code = 101,656 / 239,698 = 42.4100% (42.41%)**; unresolved is **138,042 bytes**.

The sequential frontier now reaches `171FB`; `171F9..171FB` is proven `LEA EAX,[EAX+0]` alignment and the next routine begins at `171FC`. Preserve `16A88`'s five 10-byte actor sub-blocks driven by four signed mapping bytes apiece; `16ADC`'s active-vs-disabled actor reset plus class-1/class-3/class-4 side effects; `16D04`'s handle-preloaded four-byte locals whose two-byte reads retain the handle's upper 16 bits on full-dword stores, including the offset-`10h` value copied forward to `288F0`; and `170AC`'s exact signed-width neighbor-offset table with 16-bit truncation. `170A0..170AA` is the directly referenced `levels.dat` string, preceded by nine proven zero-data bytes. The save-file writer at `171FC` remains intact for Batch 31. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 31 checkpoint

Batch 31 added **3,826 translated bytes** plus **198 proven non-code/data bytes**. Cumulative accounting is **103,537 translated + 2,143 non-code = 105,680 / 239,698 = 44.0888% (44.09%)**; unresolved is **134,018 bytes**.

The sequential frontier now reaches `181B3`; `179A2..179A3` is proven compiler alignment and `179A4..17A67` is the exact 49-entry CS table owned by `17A68`. The next routine begins at `181B4`. Preserve `171FC`'s signed-IDIV three-digit filename substitution, its delayed seven-dword section-offset header at file offset 1, and the distinct terminators used by the `CB3C` and `CD3C` dynamic record families; `17838`'s resource-`37h` dependent level exclusion sets and wrapped `(random*4)>>15` delay calculation; and `17A68`'s clear/recompute of record bit 1, pre-dispatch special types, exact 49-way `22h..52h` table, and type-`4Eh` mutation into type 1 with `10000000h` pointer and a rerolled word. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 32 checkpoint

Batch 32 added **3,032 translated bytes** plus **4 proven non-code/data bytes**. Cumulative accounting is **106,569 translated + 2,147 non-code = 108,716 / 239,698 = 45.3554% (45.36%)**; unresolved is **130,982 bytes**.

The sequential frontier now reaches `18D8F`; `18D8C..18D8F` is proven zero padding and the next routine begins at `18D90`. Preserve `181B4`'s direct-save versus `levels.dat` fallback split, the legacy deferred 10-byte record redistribution from `CB3C` into `CD3C`, and the three compact 11-byte record conversions (including type `54h -> 58h` only in the alternate family). The direct-save path consumes but ignores the serialized primary-count word and still uses header `258A6`; the common tail performs a relative four-byte seek before the map body, uses a signed `<7FFh` packed-text-length gate, and closes the file before post-load state restoration. Preserve the path-dependent live EBX value (`0` direct, `3` legacy) passed to `1C160(20h, EBX)`, exact `BF38` save/restore, and the late `BF44` overrides for levels `17h`, `15h`, `0Bh`, and `0Ch`. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 33 checkpoint

Batch 33 added **2,783 translated bytes** plus **25 proven non-code/data bytes**. Cumulative accounting is **109,352 translated + 2,172 non-code = 111,524 / 239,698 = 46.5269% (46.53%)**; unresolved is **128,174 bytes**.

The sequential frontier now reaches `19887`; `19886..19887` is proven `MOV EAX,EAX` alignment and the next routine begins at `19888`. Preserve `18D90`'s one-shot `F050+72h` latch, redraw-only-on-mutation input loop, low-byte-plus-one `165C` character-class lookup, and three-failure `205B4(-1)` path. Preserve `191C0`'s x86 two's-complement ABS plus low-32 IMUL/signed-IDIV interpolation, `192CC`'s tail jump to `1A9F8`, and the exact four-entry CS table at `19454..19463`. `19464` keeps unsigned dispatch gating for the `-1` hover sentinel and signed mouse-threshold comparisons. `197B8` preserves the later dword load from `C050` with only AL XORed before `2A020`, so bytes `C051..C053` remain live. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 34 checkpoint

Batch 34 added **2,428 translated bytes** plus **28 proven non-code/data bytes**. Cumulative accounting is **111,780 translated + 2,200 non-code = 113,980 / 239,698 = 47.5515% (47.55%)**; unresolved is **125,718 bytes**.

The sequential frontier now reaches `1A21F`; the next ownership unit begins at `1A220` and falls forward into the large `1A288` mouse-update routine. Preserve `19888`'s staged menu/cinematic sequence, including the saved `C02C/C02B` bytes, four unsigned timed waits, ESC promotion to status 1, and unconditional `19464` cleanup. Preserve `19C80`'s strict unsigned `now > A790` delayed-click expiry and the `19CCC/19CF4` 14-level stack that saves only the low byte of `A7E8` and zero-extends it on restore. `19D18` keeps the wrapped coordinate scale, the bit-8 clear only when creating the delayed-click latch or consuming an early second click, the three high-flag +/-1000 axis tests, and the cursor redraw/state-save path. `1A0B0` initializes eight `FFFFh` words at `9E21..9E2F` and installs mask `FFFFh`; `1A1D0` uses the shared prior RET when disabled and otherwise tail-jumps `33B44`; `1A1F4` stores the full button word at `9E35` but returns only its low two bits. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 35 checkpoint

Batch 35 added **3,042 translated bytes** plus **46 proven non-code/data bytes**. Cumulative accounting is **114,822 translated + 2,246 non-code = 117,068 / 239,698 = 48.8398% (48.84%)**; unresolved is **122,630 bytes**.

The sequential frontier now reaches `1AE2F`; the next routine begins at `1AE30`. Preserve `1A220`'s byte-only `CF48` toggle followed by a signed-word test, `1A288`'s non-contiguous shared cold tail at `1AB6C..1AC0B`, and `1A3B8`'s two distinct cursor-clamp regions. The page/item helpers deliberately differ between active `XOR SI,1` and fallback `XOR AL,1` paths, while `1AACC/1AB20` perform the encoded word `DEC` before testing for zero. `1AC38` owns the exact eight-entry table at `1AC18..1AC37`; preserve its two hit-map families, `A6BC=index+3` update, and axis-specific `1F7F8` dispatch values. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 36 checkpoint

Batch 36 added **3,472 translated bytes** plus **24 proven non-code/data bytes**. Cumulative accounting is **118,294 translated + 2,270 non-code = 120,564 / 239,698 = 50.2983% (50.30%)**; unresolved is **119,134 bytes**.

The sequential frontier now reaches `1BBD7`; the next routine begins at `1BBD8`. Preserve `1AE30`'s disabled-path zeroing, INT 33h coordinate capture, right-button `C04C` eight-step repeat, signed `C6E8` clamp, and tail jump to `1F820`. `1AF90` builds its eight-byte direction priority map from signed high-word deltas and returns either the matching rank, the incoming fallback, or a random index when rank four is requested. `1B1C4` rotates the 1C0h orientation field modulo six and only rerolls packed coordinates on wrap to zero. `1B4F8` preserves the descriptor-controlled orientation divisor and the encoded low-word stack alias used to derive the tile index from the high words of the two new fixed-point coordinates. `1B6EC` returns true when any active actor has status bit 20h or when every active actor meets the descriptor/status qualification. `1B760` preserves the repeated `1B6EC` calls, signed comparisons after x86 two's-complement ABS, the `10h/20h` record-state transitions, type-`09h` and type-`21h` exceptions, signed steering-table remainder, and the exact early-return paths that skip the final RNG reseed. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.

## CODE.2 Batch 37 checkpoint

Using the smaller checkpoint cadence, Batch 37 added **449 translated bytes** plus **3 proven alignment bytes**. Cumulative accounting is **118,743 translated + 2,273 non-code = 121,016 / 239,698 = 50.4869% (50.49%)**; unresolved is **118,682 bytes**.

The sequential frontier is `1BD9B`. The next ownership unit begins at the four-entry table `1BD9C..1BDAB`, with code entry `1BDAC`. Preserve the `1BBD8` retry count, type-9 word-width arithmetic, movement descriptor radius/divisor reads, and the success-path orientation increment.

## CODE.2 Batch 38 checkpoint

Batch 38 continues the smaller checkpoint cadence. It added **781 translated bytes** plus **23 proven non-code/data bytes**. Cumulative accounting is **119,524 translated + 2,296 non-code = 121,820 / 239,698 = 50.8223% (50.82%)**; unresolved is **117,878 bytes**.

The sequential frontier now reaches `1C0BF`; the next routine begins at `1C0C0`. Preserve the `1BD9C` four-entry state table, descriptor timer reload after the `0D550` call, signed live count reloads for all three record arrays, compaction retry via pre-increment `DEC ESI`, the encoded `CMP EDI,EDI` artifact in the type-8 path, and zero-divisor x86 divide faults in the metadata-driven orientation updates. Continue with similarly small checkpoints.

## CODE.2 Batch 39 checkpoint

Batch 39 continues the smaller checkpoint cadence. It added **829 translated bytes** plus **15 proven non-code/data bytes**. Cumulative accounting is **120,353 translated + 2,311 non-code = 122,664 / 239,698 = 51.1744% (51.17%)**; unresolved is **117,034 bytes**.

The sequential frontier now reaches `1C40B`; the next routine begins at `1C40C`. Preserve `1C0C0`'s signed two-stage division, the bitset helpers' negative-index/remainder shift behavior and bit-31 SAR result, the unsigned timeout test in `1C2C4`, `1C328`'s low-32 multiply/wrap arithmetic, and `1C368`'s repeated handle reloads plus backward `DCh` decode. Continue with similarly small checkpoints.

## CODE.2 Batch 40 checkpoint

Batch 40 added **879 translated bytes** plus **1 proven non-code/data byte**. Cumulative accounting is **121,232 translated + 2,312 non-code = 123,544 / 239,698 = 51.5415% (51.54%)**; unresolved is **116,154 bytes**.

The sequential frontier now reaches `1C77B`; the next routine begins at `1C77C`. Preserve `1C40C`'s partial-register size rounding (`AND AL,E0h` / `AND AL,F0h`), x86 signed half-offset calculation, separate random edge families, and signed loop termination. Continue with the requested small checkpoint cadence.

## CODE.2 Batch 41 checkpoint

Batch 41 increases the checkpoint size while retaining clean ownership boundaries. It added **1,628 translated bytes** plus **144 proven non-code/data bytes**. Cumulative accounting is **122,860 translated + 2,456 non-code = 125,316 / 239,698 = 52.2808% (52.28%)**; unresolved is **114,382 bytes**.

The sequential frontier now reaches `1CE67`; the next routine begins at `1CE68`, a much larger command dispatcher and should remain intact for the next ownership pass. Preserve `1C77C`'s full-dword versus low-byte `C050` asymmetry, `1C924`'s two-phase slot search, the exact 35-entry table at `1C9A0..1CA2B`, and `1CD48`'s criterion-21h stack reuse/latch behavior.

## CODE.2 Batch 42 checkpoint

Batch 42 intentionally exceeds the nominal 2–3 KB target because `1CE68..1E49F` is one compiler-owned command dispatcher with a shared selector tail at `1E488`; splitting it would create false ownership boundaries. It added **5,688 translated bytes** and no new non-code/data bytes. Cumulative accounting is **128,548 translated + 2,456 non-code = 131,004 / 239,698 = 54.6538% (54.65%)**; unresolved is **108,694 bytes**.

The sequential frontier now reaches `1E49F`; the next routine begins directly at `1E4A0`. Preserve the selector-call ordering, live `BF44` reread in the `11h/12h` path, direct byte event mutations (`22211h|=04h`, `22213h|=10h`, `22217h|=01h`), command `2Ch`'s second criterion of zero, inventory-test ordering in `2Fh`, and random text calculation `(r+r)>>15 + 6Fh`. Semantic audit remains not started.


## CODE.2 Batch 43 checkpoint

Batch 43 added **2,307 translated bytes** plus **9 proven non-code/data bytes**. Cumulative accounting is **130,855 translated + 2,465 non-code = 133,320 / 239,698 = 55.6200% (55.62%)**; unresolved is **106,378 bytes**.

The sequential frontier now reaches `1EDAB`; the next routine begins at `1EDAC`. Preserve `1E4A0/1E534`'s unsigned timer expiry tests, `1E594`'s phase-gated early return and dual timed-text damage paths, `1E75C`'s signed tick-delta comparisons and 438h/21Ch cadence, `1E930/1E9A0`'s partial-register return values that retain pointer high words, and `1EA90`'s status/item renderer including the stack-carried zero that becomes `30508`'s fifth argument. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved.


## CODE.2 Batch 44 checkpoint

Batch 44 added **2,499 translated bytes** plus **137 proven non-code/data bytes**. Cumulative accounting is **133,354 translated + 2,602 non-code = 135,956 / 239,698 = 56.7197% (56.72%)**; unresolved is **103,742 bytes**.

The sequential frontier now reaches `1F7F7`; the next routine begins at `1F7F8`. The exact 32-entry table at `1EEFC..1EF7B` belongs to `1EF7C`. Preserve `1EDAC`'s one-replacement-per-lane mapping behavior, `1EF7C`'s phase-dependent random sound ranges, `1F120`'s repeated live `29048` calls, `1F1C8`'s signed truncation and asymmetric presentation paths, `1F3FC`'s flag-priority order, and `1F494`'s dead/high-bit direct jump that bypasses the common actor-status refresh. Direct inspection of `2E6F0` (`PUSH arg; CALL 373BF`) proves it has one cdecl input; Batch 43's boundary signature was corrected accordingly while retaining the encoded pre-pushed zero as `30508`'s fifth argument. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 45 checkpoint

Batch 45 added **2,776 translated bytes** plus **32 proven non-code/data bytes**. Cumulative accounting is **136,130 translated + 2,634 non-code = 138,764 / 239,698 = 57.8912% (57.89%)**; unresolved is **100,934 bytes**.

The sequential frontier now reaches `202EF`; the next routine begins at `202F0`. Preserve the five-entry `1F858..1F86B` table, `1F86C` state-4 fall-through through scripts `2Dh..31h`, `1F8E0`'s backward shared-RET / forward fall-through ownership, `1FB80`'s release-then-press polling, and `1FC14`'s non-returning main loop. In `1FC14`, only `258FC` is initialized at entry; `258FE` remains live, key dispatch compares AX, damping is skipped for the selected axis when direction*value >= 0, and the `C6E8/C6EC` scaling uses low-32 IMUL + logical SHR + CWDE semantics. Known zero islands `006B9..006BF`, `0380D..0380F`, and `03A64..03A77` remain unresolved. Semantic audit remains not started.


## CODE.2 Batch 46 checkpoint
Batch 46 adds **2,807 translated bytes** and **33 proven non-code/data bytes**, bringing cumulative accounting to **138,937 translated + 2,667 non-code = 141,604 / 239,698 = 59.0760% (59.08%)**. The sequential frontier is now `20E08`. Preserve the stack-slot reuse in `20850`/`207E8`, signed quotient/remainder behavior in `20994`, byte-wrap timer semantics in `20B40`, and packed fixed-point argument construction in `20C0C`.


## CODE.2 Batch 47 checkpoint
Batch 47 adds **2,297 translated bytes** and **3 proven alignment bytes**, bringing cumulative accounting to **141,234 translated + 2,670 non-code = 143,904 / 239,698 = 60.0355% (60.04%)**. The sequential frontier is now `21704`. Preserve the impossible `210B0` word-sentinel comparison exactly, `211D8`'s environment-list fallback order, and `214B8`'s saved current-environment values versus the explicit post-callback reload of `255D8`.

## CODE.2 Batch 48 checkpoint
Batch 48 adds **3,266 translated bytes** and **46 proven non-code/data bytes**, bringing cumulative accounting to **144,500 translated + 2,716 non-code = 147,216 / 239,698 = 61.4173% (61.42%)**. The sequential frontier is now `223F4`. Preserve `2195C`'s low-word fixed-point overwrite, the mixed raw-SAR versus truncation-toward-zero arithmetic in `21BF0`, the exact eight-entry `21B24` table, and the intentionally dead constant-nonzero branch in `21F70`.

## CODE.2 Batch 49 checkpoint
Batch 49 adds **2,503 translated bytes** and **133 proven non-code/data bytes**, bringing cumulative accounting to **147,003 translated + 2,849 non-code = 149,852 / 239,698 = 62.5170% (62.52%)**. The sequential frontier is now `22E40`. Preserve the partial-register page toggle and one-past-count ownership in `223F4`, both exact `229CC` CS tables, the failed-probe `EBX -= 20h` carry-forward, `22894`'s signed division behavior, and the queue head update before `229CC`/`12E70`/`02DB0` dispatch.

## CODE.2 Batch 50 checkpoint
Batch 50 adds **4,444 translated bytes** and **40 proven non-code/data bytes**, bringing cumulative accounting to **151,447 translated + 2,889 non-code = 154,336 / 239,698 = 64.3877% (64.39%)**. The sequential frontier is now `23FC4`. Preserve `23024`'s equality-only list stop, `231C0`'s packed-nibble DEC/JNE loops, `23438`'s signed panel arithmetic and nibble wrap decrement, `2388C`'s rollback/angle/timer paths, and `23B38`'s partial flag writes and collision retry loop.

## CODE.2 Batch 51 checkpoint
Batch 51 adds **2,771 translated bytes** and **5 proven alignment bytes**, bringing cumulative accounting to **154,218 translated + 2,894 non-code = 157,112 / 239,698 = 65.5458% (65.55%)**. The sequential frontier is `24A9C`. Preserve `240A8` role-dependent admissibility, `243E0` empty-slot gating, and `24478` asymmetric class/rank arbitration plus per-side cooldown and selector-0/1 return ownership.

## CODE.2 Batch 52 integrity repair
The prior Batch 52 was removed because it analyzed object offsets beginning at `14A60` while labeling them with an erroneous `+10000h` shift. Immutable CODE.2 proves `24478` continues through `24A9B` and `24A9C` is the true continuation; Batch 51's original accounting is restored.

## CODE.2 Batch 52 checkpoint
Corrected Batch 52 adds **4,937 translated bytes** and **23 proven alignment bytes**, bringing cumulative accounting to **159,155 translated + 2,917 non-code = 162,072 / 239,698 = 67.6151% (67.62%)**. The sequential frontier is now `25DFC`, whose CS dword dispatch table remains with its owning dispatcher for the next fold. Preserve the signed clipping/page-capture behavior, byte-only page toggles, display/resource restoration order, inventory-group ownership, record compaction, resource gates, and low-word add/clamp behavior described in `PROGRESS.md`.

## CODE.2 Batch 53 checkpoint
Batch 53 adds **5,071 translated bytes** and **540 proven table bytes**, bringing cumulative accounting to **164,226 translated + 3,457 non-code = 167,683 / 239,698 = 69.9559% (69.96%)**. The sequential frontier reaches `273E7`; the nine bytes through `273EF` remain unresolved and the next independently decoded routine begins at `273F0`. Preserve the exact 135-entry `25DFC` table, `26018` cooldown early-return, load-before-`-1` kind override, AL-only peer-slot XORs, post-call global reloads, partial-word status test, and the item-`73h` pre-pushed `B02C` frame.


## CODE.2 Batch 54 checkpoint
Batch 54 adds **2,797 translated bytes** and **80 proven alignment/padding bytes**, bringing cumulative accounting to **167,023 translated + 3,537 non-code = 170,560 / 239,698 = 71.1562% (71.16%)**. The resume-overlap hashes from Batch 53 were reproduced exactly before any forward accounting. The sequential frontier is `27F24`; preserve the low-level VGA plane/register ordering, `27894` interrupt stack/selector swap, screenshot filename retry semantics, stream cursor/RLE ownership, `27C70` dword zero-detect loads, and `27D20/27ECC` multimedia state lifecycle.
