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


## CODE.2 Batch 55 checkpoint
Batch 55 adds **2,032 translated bytes** and **40 proven alignment/padding bytes**, bringing cumulative accounting to **169,055 translated + 3,577 non-code = 172,632 / 239,698 = 72.0206% (72.02%)**. The sequential frontier is `2873C`. Preserve `27F24` REP-copy remainder semantics, signed queue priority arbitration, the `28210` stack-copy/error fall-through, `282BC`'s four-argument stack walk into `366BB/366C5`, its stale/live ESI behavior and repeated-close oddity at `28564`, and `28670`'s explicit `EE8C` reload on the match path.


## CODE.2 Batch 56 checkpoint
Batch 56 adds **3,214 translated bytes** and **74 proven alignment/padding bytes**, bringing cumulative accounting to **172,269 translated + 3,651 non-code = 175,920 / 239,698 = 73.3924% (73.39%)**. The mandatory Batch-55 overlap hashes were reproduced exactly before forward work. The sequential frontier is `29414`; preserve `2878C`'s zero-map early exit, `28AE0`'s encoded current-slot guard interaction, `29058`'s non-boolean live-state return, BX-only/high-half behavior in `290D0`, `290F4`'s partial-width state updates, signed clipping distinctions, and the shared earlier RET/tail transfer in the final graphics wrappers.


## CODE.2 Batch 57 checkpoint
Batch 57 adds **1,281 translated bytes** and **27 proven alignment/padding bytes**, bringing cumulative accounting to **173,550 translated + 3,678 non-code = 177,228 / 239,698 = 73.9380% (73.94%)**. The sequential frontier is `29930`; preserve the clipped/full renderer distinction, `EF69..EF6F` low-word global writes, `296C0` signed division behavior, and EFD4 scratch-depth balancing.


## CODE.2 Batch 58 checkpoint
Batch 58 adds **939 translated bytes** and **17 proven alignment/table bytes**, bringing cumulative accounting to **174,489 translated + 3,695 non-code = 178,184 / 239,698 = 74.3369% (74.34%)**. The sequential frontier is `29CEC`; preserve the VGA register/BIOS ordering, `29B33` direct CS table, `29B40` latch/mask and partial-register behavior, and `29CA0` retrace-bounded DAC streaming.


## CODE.2 Batch 59 checkpoint
Batch 59 adds **1,048 translated bytes** and **16 proven alignment bytes**, bringing cumulative accounting to **175,537 translated + 3,711 non-code = 179,248 / 239,698 = 74.7808% (74.78%)**. The sequential frontier is `2A114`; preserve the partial-register plane-mask and signed Bresenham paths in `29CEC`, low-word sign gates in `29EAC/29F40`, CX-only shift semantics in `2A020`, and page save/select/restore behavior in `2A078`.


## CODE.2 Batch 60 checkpoint
Batch 60 adds **1,427 translated bytes** and **9 proven alignment bytes**, bringing cumulative accounting to **176,964 translated + 3,720 non-code = 180,684 / 239,698 = 75.3799% (75.38%)**. The sequential frontier is `2A6B0`; preserve the plane-major `2A114/2A178` asymmetry, 16-bit wrap in `2A258`, transparent BL-only byte counts, and self-modifying code operands at `2A4F6`, `2A54F`, `2A60B`, and `2A672`.


## CODE.2 Batch 61 checkpoint
Batch 61 adds **1,453 translated bytes** and **35 proven alignment/padding/table bytes**, bringing cumulative accounting to **178,417 translated + 3,755 non-code = 182,172 / 239,698 = 76.0006% (76.00%)**. The sequential frontier is `2AC80`; preserve the self-modifying bytes at `2A816`, `2AA0D`, and `2AA12`, the ROL/SHL and 32-/16-bit clipped-blitter asymmetries, `2AAEC`'s register ABI, the directly referenced `2AC70` table, and VGA retrace/CRTC ordering.

## Batch 62 checkpoint

Batch 62 adds **1,022 translated bytes** and **10 proven non-code/alignment bytes**, bringing cumulative accounting to **179,439 translated + 3,765 non-code = 183,204 / 239,698 = 76.4312% (76.43%)**. The sequential frontier is now `2B088`. Preserve the signed DWORD comparisons in `2ACDC`/`2AD18`/`2ADE0`, the two real resource-state calls from `2AD18`, the `C050` low-bit page toggle, the entry-stable record base but per-iteration count reload in `2AE30`, the second load of record word `+8` after rectangle acceptance, and the address-distinct inline predicate copies at `2AEBD` and `2B01D`.

## Batch 63 checkpoint

Batch 63 adds **1,600 translated bytes** and **8 proven alignment bytes**, bringing cumulative accounting to **181,039 translated + 3,773 non-code = 184,812 / 239,698 = 77.1020% (77.10%)**. The sequential frontier is now `2B6D0`. Preserve `2B088`'s cross-entry carried value and signed neighborhood tables, `2B1BC`'s signed comparison after the runtime-style absolute value and `+8000h` rounding, and `2B2F4`'s mutable `2B608` cache, signed `IDIV` quotient/remainders, heading normalization loops, low-word additions, and final `27100 + mode*2` word adjustment.

## CODE.2 Batch 64 checkpoint

Batch 64 adds **3,005 translated bytes** and **7 proven alignment bytes**, bringing cumulative accounting to **184,044 translated + 3,780 non-code = 187,824 / 239,698 = 78.3586% (78.36%)**; unresolved is **51,874 bytes**. The sequential frontier is now `2C294`. Preserve per-loop signed count reloads in the four record-population passes, the exact descriptor families (`22230`, `255DC`, `255D8`, `22234`), callback-sensitive metadata reloads, `2B2F4` mode ownership, the `7D00h`/`20h` temporary rectangle transformation, and the encoded signed-division sequence in `2C230`.

## CODE.2 Batch 65 checkpoint

Batch 65 adds **3,418 translated bytes** and **22 proven alignment bytes**, bringing cumulative accounting to **187,462 translated + 3,802 non-code = 191,264 / 239,698 = 79.7937% (79.79%)**; unresolved is **48,434 bytes**. The sequential frontier is now `2D004`. Preserve `2C294`'s non-linear saved-rectangle restore order, its dynamically re-read clip globals and render-kind fields, the callback-sensitive metadata/source reloads in `2C8E4`, independent transition branches in `2CAD4`, and `2CEB8`'s word-only transition writes and explicit `258EC` reload after `2E510`.

## CODE.2 Batch 66 checkpoint

Batch 66 adds **3,862 translated bytes** and **18 proven alignment bytes**, bringing cumulative accounting to **191,324 translated + 3,820 non-code = 195,144 / 239,698 = 81.4124% (81.41%)**; unresolved is **44,554 bytes**. The sequential frontier is now `2DF2C`. Preserve the signed versus unsigned comparisons in `2D3D8`/`2D5D0`, the `2D004` saved-but-not-restored coordinate locals, AX-only acceptance tests in `2DBAC`, the first-qualifying-tile early return in `2DA7C`, and the word/byte-specific tile-code mutations around `198E0`.

## CODE.2 Batch 67 checkpoint

Batch 67 adds **2,161 translated bytes** and **83 proven non-code/data bytes**, bringing cumulative accounting to **193,485 translated + 3,903 non-code = 197,388 / 239,698 = 82.3486% (82.35%)**; unresolved is **42,310 bytes**. The sequential frontier is now `2E7F0`. Preserve the exact `2DFEC` eight-entry CS table owned by `2DF2C`, the signed-high-word-only distance clamp, `2E120`'s word-sized negation before sign extension, page-stack state in `2E590/2E5D4`, and `2E614`'s intentionally dead post-mask return branch.

## CODE.2 Batch 68 checkpoint

Batch 68 adds **2,173 translated bytes** and **71 proven non-code/data bytes**, bringing cumulative accounting to **195,658 translated + 3,974 non-code = 199,632 / 239,698 = 83.2848% (83.28%)**; unresolved is **40,066 bytes**. The sequential frontier is now `2F0B4`. Preserve the two exact seven-entry CS tables, `2E7F0`'s partial initialization of the nine-byte item records, `2ECB4`'s continue-after-error indexing and selector dispatch, and `2EFD0`'s low-nibble cleanup routing. Proven earlier ABI signatures at `28020` and `286C4` were corrected to retain the ignored first argument and stream-valued second argument.
## CODE.2 Batch 71 checkpoint

Batch 71 added **1,843 translated instruction bytes** plus **85 directly proven non-code/data bytes**, reconciling through `30D2F`. Cumulative accounting is **202,780 translated + 4,144 non-code = 206,924 / 239,698 = 86.3270% (86.33%)**; unresolved is **32,774 bytes**.

The fold covers text layout through `306F7`, the tick/keyboard interrupt and queue/vector family through `30C90`, RNG state through `30CD3`, and the overlap-safe move at `30CD4..30D25`. Preserve the signed-vs-unsigned glyph-index distinction, the keyboard handler's signed `80h` split and CODE.2-only `19h -> word [1457C]=1` branch, the exact `307C8` direction table, selector-bearing saved vectors, and overlap-copy direction. The next independent routine begins at `30D30`.
## CODE.2 Batch 72 checkpoint

Batch 72 added **1,852 translated instruction bytes** plus **68 directly proven non-code/data bytes**, reconciling through `314AF`. Cumulative accounting is **204,632 translated + 4,212 non-code = 208,844 / 239,698 = 87.1280% (87.13%)**; unresolved is **30,854 bytes**.

The fold covers the `30D30` fill primitive, DPMI/memory wrappers through `31012`, allocator recovery and checked free logic, buffered byte output, DOS/string and handle wrappers, and drive-aware pathname opening through `314AA`. Preserve signed paragraph division, partial-AL return at `31274`, the argument permutation in the three I/O forwarding thunks, the staged allocator-recovery loop, byte-vs-dword stream flags, and failure-only drive restoration. The next independent application routine begins at `314B0`.
## CODE.2 Batch 73 checkpoint

Batch 73 added **1,025 translated instruction bytes** plus **3 directly proven alignment bytes**, reconciling through `318B3`. Cumulative accounting is **205,657 translated + 4,215 non-code = 209,872 / 239,698 = 87.5568% (87.56%)**; unresolved is **29,826 bytes**.

The fold is the complete `314B0..318B0` perspective-row renderer. Preserve unconditional record-zero processing, the zero-word sentinel only on later records, cdecl stack reuse around `30CA2`/`29838`, RNG reseeding, signed wrapped projection arithmetic, and the three-position horizontal wrap search. The next independent routine begins at `318B4`.
## CODE.2 Batch 74 checkpoint

Batch 74 added **929 translated instruction bytes** plus **3 directly proven alignment bytes**, reconciling through `31C57`. Cumulative accounting is **206,586 translated + 4,218 non-code = 210,804 / 239,698 = 87.9457% (87.95%)**; unresolved is **28,894 bytes**.

The fold covers the paired packed shading/lookup generators `318B4..31A11` and `31A14..31C56`. Preserve signed division, arithmetic-shift half rounding, AL-sized quotient capture, parity-driven high-byte decrement, byte-width fade subtraction, `F954` scaling, and the exact lookup-band ranges. The next independent routine begins at `31C58`.
## CODE.2 Batch 75 checkpoint

Batch 75 added **538 translated instruction bytes** plus **2 directly proven alignment bytes**, reconciling through `31E73`. Cumulative accounting is **207,124 translated + 4,220 non-code = 211,344 / 239,698 = 88.1709% (88.17%)**; unresolved is **28,354 bytes**.

The fold is the complete two-phase packed lookup generator `31C58..31E71`. Preserve its AL-sized quotient capture, signed division and arithmetic half-rounding, parity-driven high-byte decrement, argument-dependent first-phase widths, and fixed mirrored 128-byte second phase. The next independent routine begins at `31E74`.
## CODE.2 Batch 76 checkpoint

Batch 76 reconciles `31E74..31F93`: **282 translated + 6 alignment bytes**. Cumulative accounting is **211,632 / 239,698 = 88.2911% (88.29%)** with **28,066 unresolved bytes**. Preserve the four-slot mask scan and the distinct clamp ranges. Next frontier is `31F94`.
## CODE.2 Batch 77 checkpoint

Batch 77 reconciles `31F94..320F7`: **356 translated bytes**, no new non-code. Cumulative accounting is **211,988 / 239,698 = 88.4396% (88.44%)**; unresolved is **27,710 bytes**. Preserve the 24-way high-word normalization, phase-change early return, `258A8 == FFh` bypass, signed 8-way remainder update, and phase-indexed record/shading writes. Next frontier is `320F8`.
## CODE.2 Batch 78 checkpoint

Batch 78 reconciles `320F8..3214F`: **74 translated + 14 zero-padding bytes**. Cumulative coverage is **212,076 / 239,698 = 88.4763% (88.48%)**; unresolved is **27,622 bytes**. The formatter boundary retains the encoded 320F8 callback identity and explicit varargs cursor. Next frontier is `32150`.
## CODE.2 Batch 79 checkpoint

Batch 79 reconciles `32150..3221F`: **207 translated + 1 alignment byte**. Coverage is **212,284 / 239,698 = 88.5631% (88.56%)** with **27,414 unresolved bytes**. Preserve the 256 RGB triples, peak-channel scaling, signed rounded division, and zero guards. Next frontier is `32220`.
## CODE.2 Batch 80 checkpoint

Batch 80 reconciles `32220..324CB`: **680 translated + 4 alignment bytes**. Coverage is **212,968 / 239,698 = 88.8485% (88.85%)** with **26,730 unresolved bytes**. Preserve forward-copy behavior, the 0x300-byte scratch palette, factor directions 7..0 and 1..8, low-byte RGB arguments, and x86-masked arithmetic shift in the interpolation primitive. Next frontier is `324CC`.
## CODE.2 Batch 80 post-commit fidelity correction

Direct stack tracing at the `32468` call sites proved that `32220` and `322EC` pass the raw third argument as the interpolation count, while their restore copies use the separately computed inclusive `(end-start+1)` span. The reconstruction now preserves this asymmetry. Accounting remains **208,723 translated + 4,245 non-code = 212,968 / 239,698 = 88.8485% (88.85%)** and the next frontier remains `324CC`.
## CODE.2 Batch 81 checkpoint

Batch 81 reconciles `324CC..3261F`: **331 translated + 9 non-code bytes**. Coverage is **213,308 / 239,698 = 88.9903% (88.99%)**, with **26,390 unresolved bytes**. Preserve the forward overlapping triple shift in `324F8`, resource-wrapper argument permutation, row offset encoding `((row*3)<<8)+1`, and the post-read `32150` palette rescale. Next frontier is `32620`.
## CODE.2 Batch 82 checkpoint

Batch 82 reconciles `32620..3263B`: **27 translated + 1 alignment byte**. Coverage is **213,336 / 239,698 = 89.0020% (89.00%)**, with **26,362 unresolved bytes**. Preserve the register-call ABI: target from arg1, ESI=`[255E4]`, EDI=arg2, ECX=arg3, callback EAX returned. Next frontier is `3263C`.
## CODE.2 Batch 83 checkpoint

Batch 83 reconciles `3263C..32953`: **790 translated + 2 alignment bytes**. Coverage is **214,128 / 239,698 = 89.3324% (89.33%)** with **25,570 unresolved bytes**. Preserve the original unaligned extent separately from the callback-table index, signed byte clamping of the packed shade components, x86-style MUL/IDIV row shift, explicit code-byte patch/restore, VGA sequencer map-mask writes, and optional second/third plane passes. Next frontier is `32954`.
## CODE.2 Batch 84 checkpoint

Batch 84 reconciles `32954..32F4B`: **1,526 translated + 2 alignment bytes**. Coverage is **215,656 / 239,698 = 89.9699% (89.97%)**, with **24,042 unresolved bytes**. Preserve the four region-specific step vectors and `356D8` argument pairs, dynamic opcode writes at `32C1A`/`32D04`, unsigned range tests where encoded, low-word-only arithmetic at the right sweep, and the shared backward sweep/outer-loop edges. Next frontier is `32F4C`.
## CODE.2 Batch 85 checkpoint

Batch 85 reconciles `32F4C..32F8B`: **63 translated + 1 alignment byte**. Coverage is **215,720 / 239,698 = 89.9966% (90.00%)**; unresolved is **23,978 bytes**. Next frontier is `32F8C`.
## CODE.2 Batch 86 checkpoint

Batch 86 reconciles `32F8C..33123`: **406 translated + 2 alignment bytes**. Coverage is **216,128 / 239,698 = 90.1668% (90.17%)**, with **23,570 unresolved bytes**. Preserve the modular signed-word multiply used to locate the alternate cell, high-class push/pop lifetime, conditional `1E9A0` low-word extra, record table choice `257A2/25722`, and signed min/max comparisons. Next frontier is `33124`.
## CODE.2 Batch 87 checkpoint

Batch 87 reconciles `33124..332B3`: **399 translated + 1 alignment byte**. Coverage is **216,528 / 239,698 = 90.3337% (90.33%)**, with **23,170 unresolved bytes**. Preserve its asymmetries versus Batch 86: negative `25604` increments `255EA` without offset mirroring, the alternate-cell displacement is `s16(25606)*2`, and the table pair is `256A2/25622`. Next frontier is `332B4`.
## CODE.2 Batch 88 checkpoint

Batch 88 reconciles `332B4..333BB`: **262 translated + 2 alignment bytes**. Coverage is **216,792 / 239,698 = 90.4438% (90.44%)**, with **22,906 unresolved bytes**. Preserve the low-word unsigned comparison at `255E8`, modular `FCE4-8000h` bias arithmetic on the full dword source, `0FC0h` mask after the shift, high-class extraction, and direct `256A2/25622` table writes. Next frontier is `333BC`.
## CODE.2 Batch 89 checkpoint

Batch 89 reconciles `333BC..334C3`: **262 translated + 2 alignment bytes**. Coverage is **217,056 / 239,698 = 90.5539% (90.55%)**, with **22,642 unresolved bytes**. Preserve the `255F0` source distinction from Batch 88, low-word unsigned comparison, full-dword modular bias arithmetic, `0FC0h` quantization, and direct record writes. Next frontier is `334C4`.
## CODE.2 Batch 90 checkpoint

Batch 90 reconciles `334C4..335DB`: **280 translated bytes** and no separator bytes. Coverage is **217,336 / 239,698 = 90.6708% (90.67%)**, with **22,362 unresolved bytes**. Preserve the binary's do/while grid clear (including its zero-count wrap behavior), `C65C` dynamic table addressing, argument order for both `35680` calls, full result stores before low-word truncation/negation, and signed-word expansions from `255EE/255F6/255D6/255D2`. Next frontier is `335DC`.
## CODE.2 Batch 91 checkpoint

Batch 91 reconciles `335DC..337C7`: **491 translated + 1 alignment byte**. Coverage is **217,828 / 239,698 = 90.8760% (90.88%)**, with **21,870 unresolved bytes**. Preserve the first-pass `+0Ch == 1` search/restart semantics, signed `JGE` exit after search, run grouping on record fields `+0/+10h`, the first-pass `+4` versus second-pass `+8` source for `C700`, unconditional second-pass record-zero initialization, and the binary's distinct initial-run versus changed-run ordering around `255E4` and `2F29C`. Next frontier is `337C8`.
## CODE.2 Batch 92 checkpoint

Batch 92 reconciles `337C8..33807`: **63 translated + 1 alignment byte**. Coverage is **217,892 / 239,698 = 90.9027% (90.90%)**, with **21,806 unresolved bytes**. Preserve signed-byte slot initialization, 16-bit wrapping `CX` accumulation, signed `CMP ECX,EDX / JGE`, the signed loop test after the first `SUB EBX,2`, and the separate unsigned-borrow test after the second subtraction. Next frontier is `33808`.
## CODE.2 Batch 93 checkpoint

Batch 93 reconciles `33808..338C7`: **189 translated + 3 alignment bytes**. Coverage is **218,084 / 239,698 = 90.9828% (90.98%)**, with **21,614 unresolved bytes**. Preserve unsigned record-index multiplication, logical `SHR` operations, unsigned `DIV` by the sign-extended word value, the partial-word `FFFFh` initialization of the bucket output, unsigned `JAE` table bound, and signed threshold comparison inside the 128-entry word-weight search. Next frontier is `338C8`.
## CODE.2 Batch 94 checkpoint

Batch 94 reconciles `338C8..3395B`: **146 translated + 2 alignment bytes**. Coverage is **218,232 / 239,698 = 91.0446% (91.04%)**, with **21,466 unresolved bytes**. Preserve signed `IDIV` for the source step, arithmetic shifts in start clipping, low-byte loop-count semantics (`0` means 256 iterations after `DEC AH` wrapping), `SHLD 0,phase,16` source indexing, alternating `CL/CH` subtraction bytes, `50h` destination stride, and the persistent undefined-stack residue boundary for `[EBP-8]`. Next frontier is `3395C`.
## CODE.2 Batch 95 checkpoint

Batch 95 reconciles `3395C..339A1`: **70 translated bytes** and no separator bytes. Coverage is **218,302 / 239,698 = 91.0738% (91.07%)**, with **21,396 unresolved bytes**. Preserve the single 16-bit `OUT DX,AX` value/port at `3395C`, and for `33971` preserve the no-source-advance/no-count-decrement behavior when the NUL terminator is first observed before the zero-fill loop. Next frontier is `339A2`.
## CODE.2 Batch 96 checkpoint

Batch 96 reconciles `339A2..33A37`: **150 translated bytes** and no separator bytes. Coverage is **218,452 / 239,698 = 91.1363% (91.14%)**, with **21,246 unresolved bytes**. Preserve `339A2` argument reshuffling into DS:DX path/CX attributes after AH=1A, `339CF`'s pre-find `33A15` shuttle, post-find `339F2` shuttle on both success and failure, `3908C`'s carry-sensitive error-side-effect/return-code distinction, the `12662 == 9` gate, and the exact DS/ES directions of both 43-byte REP MOVSB operations. Next frontier is `33A38`.
## CODE.2 Batch 97 checkpoint

Batch 97 reconciles `33A38..33A7B`: **68 translated bytes** and no separator bytes. Coverage is **218,520 / 239,698 = 91.1647% (91.16%)**, with **21,178 unresolved bytes**. Preserve AH=39h versus AH=3Bh, DS:DX path qualification, the exact carry-to-sign meaning of the `RCL/ROR/TEST/JGE` sequence, low-16 error truncation before `390D7`, and the distinction from Batch 96: these wrappers return `FFFFFFFFh` on failure rather than the raw DOS error number. Next frontier is `33A7C`.
## CODE.2 Batch 98 checkpoint

Batch 98 reconciles `33A7C..33A9F`: **35 translated + 1 alignment byte**. Coverage is **218,556 / 239,698 = 91.1797% (91.18%)**, with **21,142 unresolved bytes**. Preserve the `33A7C` argument roles (`ESI` interrupt number, `EDI` input-register block, `EDX` output-register block, `EBX` segment-register block), the opaque `39148` selector/register-frame boundary, and the post-call reload from `[output_regs+0]`; do not flatten `39148` into an ordinary host interrupt call. Next frontier is `33AA0`.
## CODE.2 Batch 99 checkpoint

Batch 99 reconciles `33AA0..33B43`: **161 translated + 3 alignment bytes**. Coverage is **218,720 / 239,698 = 91.2482% (91.25%)**, with **20,978 unresolved bytes**. Preserve `33AA0`'s raw local frame offsets and exact four-argument call to `33A7C` rather than replacing it with a host DPMI API; preserve the 32h-byte zeroing at `2B770`, `53C1h` at `+1Ch`, high-word extraction from `FD14` into word `+22h`, and the post-call equality test at `33AF4`. `33AF3` and `33B42..33B43` are unreferenced alignment. Next frontier is `33B44`.
## CODE.2 Batch 100 checkpoint

Batch 100 reconciles `33B44..33C8F`: **332 translated bytes** and no separator bytes. Coverage is **219,052 / 239,698 = 91.3867% (91.39%)**, with **20,646 unresolved bytes**. Preserve low-word AX/BX publication from the direct INT 33h reset, the `30D80(40h,&FD14,&FD18)` allocation contract, retained allocation on a nonzero probe byte, selector-only `30DE8` free on a zero probe, the inlined real-mode frame construction rather than replacing it with the address-distinct `33AF4` helper, and the reset-failure cleanup ordering through `311E8`, `27ECC`, `285D4`, `362BE`, `30C38`, and `352BE`. Next frontier is `33C90`.
## CODE.2 Batch 101 checkpoint

Batch 101 reconciles `33C90..33DDF`: **321 translated + 15 alignment bytes**. Coverage is **219,388 / 239,698 = 91.5268% (91.53%)**, with **20,310 unresolved bytes**. Preserve post-`2E340` register residue for `33CB0`, signed arithmetic halving of low CX, the second-coordinate doubling in function 4, and the exact multi-stage polling structure/return EAX behavior of `33D58` and `33D78`. Alignment islands are `33CAF`, `33D15..17`, `33D35..37`, `33D55..57`, `33D76..77`, and `33DDD..DDF`. Next frontier is `33DE0`.
## CODE.2 Batch 102 checkpoint

Batch 102 reconciles `33DE0..33E6B`: **137 translated + 3 alignment bytes**. Coverage is **219,528 / 239,698 = 91.5852% (91.59%)**, with **20,170 unresolved bytes**. Preserve the single 16-bit VGA sequencer OUT in each routine, low-byte wrap of `input - FE94[EBX]`, entry-before-DEC 128-iteration loop shape, ES-qualified REP STOSB, and the encoded EDI post-REP stepping (`-90h` after the 40h-byte advance at `33DE0`, `+10h` after it at `33E28`). Alignment is `33E26..33E27` and `33E6B`. Next frontier is `33E6C`.
## CODE.2 Batch 103 checkpoint

Batch 103 reconciles `33E6C..33FB3`: **326 translated + 2 alignment bytes**. Coverage is **219,856 / 239,698 = 91.7221% (91.72%)**, with **19,842 unresolved bytes**. Preserve the DS:ESI -> ES:EDI `REP MOVSD` boundary, signed coordinate comparisons, exact `80000001h` initial maxima, address-distinct `3402C` edge setup calls, and both loop shapes—especially the unconditional first `34110`/`33FB4` iteration and CF-based exit. `33FB2..33FB3` is unreferenced alignment; `33FB4` is the next direct call target.
## CODE.2 Batch 104 checkpoint

Batch 104 reconciles `33FB4..3426F`: **690 translated + 10 proven non-code bytes**. Coverage is **220,556 / 239,698 = 92.0141% (92.01%)**, with **19,142 unresolved bytes**. Preserve the carry contract of `33FB4/3402C`, the physical shared tail at `3410C..3410D`, the full signed32-times-65536 IDIV reconstruction (including the additional X-gradient pre-shift), machine-width clip/count wraparound, signed arithmetic shifts, in-place `FE10` scale mutation, and the exact VGA plane selector output. `3410E..3410F` is alignment and `34268..3426F` is proven padding. Next frontier is `34270`.
## CODE.2 Batch 105 checkpoint

Batch 105 reconciles `34270..342E3`: **114 translated + 2 alignment bytes**. Coverage is **220,672 / 239,698 = 92.0625% (92.06%)**, with **19,026 unresolved bytes**. Preserve the separate `34270` and `34275` entry semantics, repeated negative normalization versus single high-side subtraction, word-width table load with dword stride, and 32-bit negation in quadrants 2/3. `342E2..342E3` is alignment; `342E4` is independently called and begins the stack-sharing renderer.
## CODE.2 Batch 106 checkpoint

Batch 106 reconciles `342E4..34997`: **1,710 translated + 6 alignment bytes**. Coverage is **222,388 / 239,698 = 92.7784% (92.78%)**, with **17,310 unresolved bytes**. Preserve the stack-sharing return-token behavior at `10258`, address-distinct `342E4/34954` plane-mask sequences, the exact 64-pixel `34350` sampler, pre-sample EAX/EDX additions, two-stage SHLD index formation, byte-width AL/AH subtraction with EAX.low8 rewrite, per-pixel `FF54` reload, and ES-qualified STOSB/EDI advance. Separator islands are `34951..34953` and `34995..34997`. Next frontier is `34998`.
## CODE.2 Batch 107 checkpoint

Batch 107 reconciles `34998..34A73`: **220 translated instruction bytes**, no new non-code. Coverage is **222,608 / 239,698 = 92.8702% (92.87%)**, with **17,090 unresolved bytes**. Preserve the four explicit self-modifying immediate patches, the distinct `34270/34275` entry semantics used to derive them, signed 16-bit multiply, exact `SHLD DX,AX,3` bit extraction, byte-vs-word destination widths, and the 128-entry loop strides. Next frontier is `34A74`.
## CODE.2 Batch 108 checkpoint

Batch 108 reconciles `34A74..34BAF`: **313 translated + 3 padding bytes**. Coverage is **222,924 / 239,698 = 93.0020% (93.00%)**, with **16,774 unresolved bytes**. Preserve all byte patch sites, the signed 50h cap/remainder tests, entry-before-decrement first loop, exact raster-pair stack order into `342E4/34954`, 16-bit-to-16.16 record expansion, shade-ramp pointer stepping, FF40 row advance, and guest return-token translation. `34BAD..34BAF` is proven padding; `34BB0` is independently called.
## CODE.2 Batch 109 checkpoint

Batch 109 reconciles `34BB0..34DE7`: **567 translated + 1 alignment byte**. Coverage is **223,492 / 239,698 = 93.2390% (93.24%)**, with **16,206 unresolved bytes**. Preserve the in-place raw SAR-by-3 index mutation, signed `C738` clamp, early zero exit, distinct bias-corrected `/8` and `/2` truncation, odd-vertex 1<->3 mirror swap, record flag/`C026`/`C027` gating, exact `[258F8h] + record[9]` resource-id load, `FDC0/FDB8/FDB4` writes, and the explicit stack-local selector boundary into `33E6C`. `34DE7` is alignment; `34DE8` is independently called.
## CODE.2 Batch 110 checkpoint

Batch 110 reconciles `34DE8..35285`: **1,182 translated instruction bytes**, no new non-code. Coverage is **224,674 / 239,698 = 93.7321% (93.73%)**, with **15,024 unresolved bytes**. Preserve the half-open clamped scan bounds, active-run reset asymmetry, packed 16.16 `35798` inputs, 16-bit `+20h` corner adjustment, corner-flag reuse, exact `Acur,Anext,Bnext,Bcur` quad ordering, and row-end B-to-A copy/clear cycle. `35286` is the next independent DOS runtime entry.
## CODE.2 Batch 111 checkpoint

Batch 111 reconciles `35286..35417`: **284 translated + 118 proven non-code bytes**. Coverage is **225,076 / 239,698 = 93.8998% (93.90%)**, with **14,622 unresolved bytes**. Preserve the AH=36h register/output-width contract, the separate `352BE/352D8` callback sequences, environment prefix-plus-`=` test, wrapped ctype `(byte+1)` lookup, 32-bit decimal wrap/negation, and `353A0`'s jump over the embedded runtime-data block. `353A2..35417` is non-code; `35418` begins the startup body.
## CODE.2 Batch 112 checkpoint

Batch 112 translates `35418..3562E`: **535 instruction bytes**, with no new non-code claim. Coverage is **225,611 / 239,698 = 94.1230% (94.12%)**, leaving **14,087 unresolved bytes**. Preserve the startup stack alignment, byte/word register writes, DOS/extender branch distinctions, selector-backed PSP/environment memory, REPZ-SCAS zero-count ZF inheritance from `AND DL,F0`, balanced startup pushes/pops, `no87=` detection, environment/program-path terminators, and the capped mode-1 zero-fill. `3562F` is not owned by this batch: it is both the fall-through after `3562A` and an independent target from `352E9`.

## CODE.2 Batch 113 checkpoint

Batch 113 reconciles `3562F..3567F`: **70 translated + 11 proven non-code/data bytes**, bringing cumulative accounting to **225,692 / 239,698 = 94.1568% (94.16%)** with **14,006 unresolved bytes**. Preserve the three address-distinct runtime entries: `3562F` stack-sharing termination, `35632` DOS console-output/termination, and `3566C` CS-qualified DS restore. `35675..35676` is the saved DS word and `35677..3567F` is proven zero alignment. Next frontier: `35680`.

## CODE.2 Batch 114 checkpoint

Batch 114 reconciles `35680..35877`: **488 translated + 16 proven non-code/data bytes**, bringing cumulative accounting to **226,196 / 239,698 = 94.3671% (94.37%)** with **13,502 unresolved bytes**. The two multiply helpers retain their asymmetric factor-sign handling; `35730` retains the signed clamp followed by unsigned DIV; `35798` retains PUSHA/POPA register restoration and its two word-only outputs. Next frontier: `35878`.

## CODE.2 Batch 115 checkpoint

Batch 115 reconciles `35878..358D3`: **89 translated + 3 proven alignment bytes**, bringing cumulative accounting to **226,288 / 239,698 = 94.4055% (94.41%)** with **13,410 unresolved bytes**. Preserve the direct path as selector-aware REP MOVS and the alternate `38407/38106/3967C` seek-read-tell ordering. Next frontier: `358D4`.

## CODE.2 Batch 116 checkpoint

Batch 116 reconciles `358D4..35A0B`: **309 translated + 3 proven alignment bytes**, bringing cumulative accounting to **226,600 / 239,698 = 94.5356% (94.54%)** with **13,098 unresolved bytes**. Preserve stack-local ownership for the four `35878` destinations rather than flattening SS/ES locals into DGROUP; preserve validation-failure cleanup even under direct-source mode and the unsigned record-count loop. Batch 115 documentation is corrected accounting-neutrally: `3967C` is current-position/tell, so that branch is seek-read-tell. Next frontier: `35A0C`.


## CODE.2 Batch 117 checkpoint

Batch 117 integrated `35A0C..35F86` (**1,403 translated bytes**) and classified `35F87` as **1 byte of alignment**. Cumulative accounting is **223,532 translated + 4,472 non-code = 228,004 / 239,698 = 95.1214% (95.12%)**; unresolved is **11,694 bytes**. The Batch 116 overlap hash matched exactly before forward work. Preserve the large loader's raw-frame aliasing, flag-1 memory/file split, flag-4 allocation path, 16-byte alignment rule, local base tables, and relocation validation/store ordering. `35F88` is already integrated; continue by closing remaining unresolved islands rather than double-counting the runtime tail.


## CODE.2 Batch 118 checkpoint

Batch 118 closes the three historical early-object zero islands without changing translated coverage: `006B9..006BF` (**7 zero-padding bytes**), `0380D..0380F` (**3 zero-padding bytes**), and `03A64..03A77` (**18 zero-padding + 2-byte MOV-EAX,EAX alignment**). Fresh object-1 disassembly and a full direct relative control-flow target scan prove the classifications. Cumulative accounting is **223,532 translated + 4,502 non-code = 228,034 / 239,698 = 95.1339% (95.13%)**; unresolved is **11,664 bytes**. Continue selecting unresolved islands from the residual complement; do not revisit these three historical gaps.


## CODE.2 Batch 119 checkpoint

Batch 119 reconciles the late runtime island `39526..3967B`: **342 translated bytes**. Preserve `39526`'s ASCII-only folding and unsigned-byte difference; `39589`'s strict unsigned stack-allocation threshold and `12650` update; and the packed six-byte callback-table selection/restart semantics in `395E2/3962D`, including the full EDX payload passed through indirect callbacks. The batch also repairs the accounting-neutral Batch 118 omission in `Code2.cpp`: `kVerifiedNonCodeBytes` now matches the already-proven/documented **4,502** bytes. Cumulative accounting is **223,874 translated + 4,502 non-code = 228,376 / 239,698 = 95.2766% (95.28%)**; unresolved is **11,322 bytes**. Continue from the residual complement rather than assuming sequential ownership after `3967B`, because `3967C` and later runtime helpers already have integrated ownership.
