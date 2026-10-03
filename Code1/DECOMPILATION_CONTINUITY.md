# CODE.1 / CODE.2 faithful decompilation continuity playbook

This file is a durable handoff for future ChatGPT conversations working on the Ravenloft: Strahd's Possession DOS decompilation. Read this file and `Code1/PROGRESS.md` before doing any new work. The purpose is to preserve the exact fidelity standard and working methodology across chat boundaries.

## Primary objective

Reconstruct the original DOS `CODE.1` machine code into instruction-accountable C++, then apply the same methodology to `CODE.2`. This phase is a faithful decompilation/reconstruction, NOT a modernization or port. Preserve encoded semantics, integer widths, signedness, overflow/truncation behavior, pointer/selector behavior, calling conventions, shared tails, register aliases, unusual control flow, and runtime/compiler idioms. Do not clean up behavior merely because a modern implementation would be simpler.

The immutable original CODE.1/CODE.2 files are source evidence only. Never modify them. The binary bytes outrank documentation, compiler conventions, comments, names, and prior hypotheses.

## Non-negotiable fidelity rules

1. Never guess missing semantics. If an edge cannot yet be proved, leave it unresolved and work another independently verifiable region.
2. Every counted byte must be instruction-bounded, translated, reconciled against authoritative disassembly, and physically integrated into `Code1.cpp` (or the future CODE.2 equivalent).
3. Structural/pseudocode lifts, scaffolding, research, data classification, and staging do not count as translated coverage.
4. Preserve overlapping register/local accesses. Example: if assembly accesses the same dword through high/low words or bytes, model the actual aliasing rather than replacing it with a mathematically convenient abstraction unless equivalence is proved.
5. Preserve selector-qualified/far-pointer behavior. Do not silently convert segmented/far memory accesses into ordinary host pointers.
6. Preserve exact argument consumption and stack/cursor advancement, including promoted 16-bit values occupying 32-bit slots and far values occupying multiple slots.
7. Preserve shared epilogues/tails and cross-function jumps when present. Do not force a conventional C function boundary if the machine code does not have one.
8. Keep uncertain routines address-named (`function_dad9`, etc.) until semantics are established. Names must not create false certainty.
9. Classify code versus embedded data before counting. Embedded data remains in the conservative denominator until proven otherwise.
10. Comments should record original logical offsets and important encoded behavior so C++ can be compared back to assembly.
11. Run a C++17 syntax check after integration: `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code1/Code1.cpp`.
12. Do not stop at the first obstacle. Maintain an unresolved queue and continue through independent regions.

## Authoritative CODE.1 identity and extraction

- Original file: `CODE.1`
- Size: 277,787 bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- Object-1 executable extent: 70,402 bytes (`0x11302`)
- Object-1 file start: `0x2C724`
- Object-1 virtual base: `0x10000`
- Entry logical offset: `0xCC1C`

Local extraction command used repeatedly:

```sh
dd if=/mnt/data/CODE.1 of=/tmp/obj1.bin bs=1 skip=$((0x2c724)) count=$((0x11302)) status=none
```

Authoritative raw disassembly pattern:

```sh
objdump -D -b binary -m i386 -M intel --start-address=0xSTART --stop-address=0xSTOP /tmp/obj1.bin
```

Logical object offset `X` corresponds to original file offset `0x2C724 + X`.

Before trusting any prior translation, regenerate the relevant disassembly directly from CODE.1 and compare instruction-by-instruction. The original binary is controlling evidence.

## Progress accounting

The conservative denominator is the complete CODE.1 object-1 executable extent: 70,402 bytes (`0x11302`).

Coverage = integrated, verified, instruction-accounted application machine-code bytes / 70,402.

Do not increase the official percentage for staging. Keep the badge RED below 100%; turn it GREEN only when verified conversion reaches exactly 100%.

At the time this handoff was written, the latest integration folded `DAD9..DDD7` into `Code1.cpp`, bringing the expected integrated total to 8,089 bytes, approximately 11.48%. ALWAYS read `Code1/PROGRESS.md` and the `kVerifiedTranslatedBytes` constant in current `Code1.cpp` instead of trusting this snapshot, because work may have advanced after this file was written.

## Working batch strategy

Target roughly 4,000 newly verified bytes per substantial batch, but fidelity controls. This is a target, not a quota.

For each batch:

1. Extract/regenerate raw disassembly from immutable CODE.1.
2. Establish coherent routine/control-flow boundaries and distinguish code/data.
3. Trace calls, jumps, shared tails, register lifetimes, argument sources, globals, and selector behavior.
4. Translate only semantics that are directly justified by the bytes.
5. Put uncertain-but-useful structural lifts in a clearly marked `*.staging.cpp`; explicitly mark them uncounted.
6. Reconcile staging instruction-by-instruction. Correct every discrepancy found; do not rationalize it away.
7. Once a range is cleared, fold it into `Code1.cpp`, syntax-check, remove staging so it cannot be double-counted, and only then update coverage.
8. Update `PROGRESS.md` with exact ranges/byte counts, what was proved, and the unresolved queue.
9. Commit substantive work to branch `code1-decompilation` before reporting progress.

If one path blocks, record the dependency and move to another region. Do not treat a single unresolved call or boundary as a batch-ending event.

## GitHub workflow

Repository: `Hyperbog/RLOFT`
Working branch: `code1-decompilation`
Primary source: `Code1/Code1.cpp`
Progress ledger: `Code1/PROGRESS.md`

Always fetch the latest file and SHA immediately before an update. Do not assume a remembered SHA is current. GitHub code search generally reflects the default branch, so absence from search is not evidence that a file is absent from `code1-decompilation`; use direct branch-aware file fetches.

When the connector cannot conveniently perform a large in-place edit, a temporary integration script + GitHub Actions workflow has been used successfully. The safe pattern is: commit staging and an integration script/workflow; workflow checks out the branch, runs the script, runs the C++17 syntax check, removes staging/script/workflow, then commits the integrated result. Verify the workflow job and resulting branch head before claiming integration. Do not leave temporary workflow/integration artifacts behind after a successful fold.

## Documentation/research policy

External documentation is secondary evidence used to understand compiler/runtime behavior; it never overrides the bytes.

Prefer primary/official technical sources:

- Open Watcom documentation and official Open Watcom source/repository for compiler/runtime conventions and implementation clues.
- Historical Rational Systems / Tenberry DOS/4G, DOS/4GW, and DOS/16M documentation for extender behavior.
- Intel x86 architecture/instruction manuals for instruction semantics.
- Microsoft MS-DOS technical/programmer documentation for DOS interrupt/API behavior.
- Official executable-format documentation where available for MZ/LE/LX structures and relocations.

The binary contains direct Watcom runtime evidence, but do NOT infer that every game routine was authored in Watcom C/C++ solely from runtime/compiler signatures. Establish language/compiler-specific claims from evidence.

Third-party reverse-engineering projects may be used only as leads; independently verify everything against CODE.1 and primary documentation.

When researching on the web, record any documentation that materially resolves an ambiguity in `PROGRESS.md` or comments near the affected translation so the reasoning survives chat turnover.

## Important lessons from completed formatter work

The `EDE7..F741` formatter required several corrections that illustrate the expected standard:

- A state field at `F303` was explicitly zeroed by assembly and had initially been omitted; the C++ was corrected rather than treating the old state as irrelevant.
- Signed decimal exits shared the unsigned precision-zero suppression tail. This was preserved even though a higher-level formatter implementation could have handled it differently.
- Local scratch strings and selector-qualified external strings required different read paths; treating both through one host callback was semantically unsafe.
- `%n` stores, far/near variants, promoted arguments, two-byte `s/S` stepping, signed 16.16 fixed-point behavior, and EAX:DX far-return contracts were preserved.
- The formatter driver and materializer share an epilogue relationship; function boundaries were modeled around the encoded control flow rather than conventional source expectations.

## Important lesson from DC90

`DC90` contained `REPNE SCASW`. The correct semantics compare the word at the current EDI first, then advance EDI and decrement ECX; the encoded `SUB EDI,2` subsequently selects the matched or final examined element. A first structural lift skipped the initial element and was corrected during instruction-level reconciliation. This is a reminder to model string instructions literally, including pre/post pointer state and counter behavior.

## Current/next CODE.1 direction

Read current `PROGRESS.md` first. At the handoff-writing point:

- `DAD9..DDD7` had just been verified and integrated.
- The next contiguous startup/runtime work begins at `CD97`, immediately after the already integrated startup/recovery block and before the mouse-facing routines at `D08C`.
- `DE6F` remained unclassified; disassembly beginning exactly there had not proved a valid routine entry, while a coherent fill path begins shortly afterward.
- Continue startup successors and remaining address-named CRT/runtime helpers, while maintaining the larger-batch policy.

Do not rely on this queue if `PROGRESS.md` has newer information.

## CODE.2 transition procedure

When CODE.1 is complete and work moves to CODE.2, reuse this methodology rather than copying CODE.1 addresses or assumptions.

For CODE.2:

1. Establish immutable identity first: exact size and SHA-256.
2. Parse its actual executable/container layout from the bytes. Do not assume CODE.2 has the same object offsets, virtual sizes, entry point, or code/data organization as CODE.1.
3. Determine executable object extents and a conservative coverage denominator from CODE.2 itself.
4. Create a separate `Code2/Code2.cpp` and `Code2/PROGRESS.md` (and staging files only as needed).
5. Keep a red progress badge below 100% and count only integrated verified instruction-accounted bytes.
6. Use raw disassembly generated from CODE.2 as authority and apply the same instruction-level reconciliation rules.
7. Re-establish compiler/runtime signatures independently. CODE.1 findings may guide where to look but are not proof for CODE.2.
8. Preserve all CODE.2-specific segmented memory, calling conventions, globals, shared tails, data islands, and runtime behavior exactly as encoded.
9. Target ~4 KB verified batches without sacrificing fidelity; bypass isolated blockers and maintain an unresolved queue.
10. Commit every substantive batch and update the progress ledger so future chats can resume from repository state alone.

## Required behavior in future chats

When the user says `continue`, `keep working`, `resume`, or equivalent, DO ACTUAL BINARY ANALYSIS AND COMMIT SUBSTANTIVE WORK BEFORE REPLYING. Do not merely acknowledge that work will continue. The user has explicitly requested this behavior.

Before starting a continuation turn:

1. Read this continuity file.
2. Read current `PROGRESS.md`.
3. Fetch current `Code1.cpp` tail and relevant sections.
4. Check current branch head/most recent integration state.
5. Regenerate the next relevant disassembly from immutable CODE.1.
6. Work, verify, commit, then report exact ranges, bytes, coverage effects, unresolved dependencies, and commit SHA(s).

Never claim a GitHub change exists until the connector confirms the commit/ref. Never claim staged bytes are integrated. Never count a byte twice.

## User-facing handoff prompt

A future conversation can be started with:

> Continue the faithful CODE.1 decompilation in `Hyperbog/RLOFT` on branch `code1-decompilation`. First read `Code1/DECOMPILATION_CONTINUITY.md` and `Code1/PROGRESS.md`, then inspect the current `Code1.cpp` and branch head. Treat the immutable CODE.1 binary as the ultimate authority, regenerate disassembly for every region you work on, preserve instruction-level semantics exactly, do not guess, keep uncertain work uncounted in staging, and fold/count only fully verified code. Follow the ~4 KB batch target without stopping at isolated blockers. Perform substantive analysis and commit work before reporting. When CODE.1 is eventually complete, use the CODE.2 transition procedure in the continuity file and apply the same fidelity standard independently to CODE.2.

Update this continuity file whenever the methodology changes or a newly discovered pitfall would materially help a future conversation preserve fidelity.


## Latest heap-growth checkpoint
Verified total is 14,578 bytes (20.70%). `1083F..10973` and `109C1..10C45` are integrated after direct disassembly. Preserve DPMI 0501/0503 linear-address+handle results, DOS segment allocation/free, mode-9 AX=80004800 allocation, and the 10DD7 break-extension boundary as distinct mechanisms. Active staging remains 974 bytes.


## Runtime-tail checkpoint
Verified total is 16,261 bytes (23.10%). `10C6F..11301` is integrated after direct disassembly; `10C6D..10C6E` are alignment zeros and excluded. This reaches the end of object 1. Preserve 10DD7 mode-0 versus nonzero DOS/extender resize boundaries, 8E84 active-stream list, 8E70 recycled-node list, and the 26-byte stream-record layout. Active staging remains 974 bytes.


## 2026-10-01 increased batch 02 checkpoint
- Source SHA-256 reverified: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`.
- Verified total after this fold: **17,960 / 70,402 = 25.5106% (25.51%)**.
- Newly counted this fold: 1,699 bytes: `0010..0048` 57, `004C..00EA` 159, `035C..043A` 223, `043C..04AF` 116, `FC98..FCE6` 79, FCE8 boundary correction +1, `FD6E..FE20` 179, `D2CD..D312` 70.
- `FCE8` is the true first dispatcher byte (`56 57 53 06 ...`); `FCE7` is zero alignment. Do not revert to FCE9.
- Interrupt veneer slot 3 is `CC 90 C3`, unlike the other `CD imm8 C3` slots.
- Active staging: **904 bytes** = startup 747 + E2C3..E35F 157.
- Early renderer translations `04B0..0648` and `064C..07E1` are integrated/countable after direct push-order reconciliation of 6EB0/6C60/7D80/67C8/00EC call sites.
- Next high-output target: reconcile those renderer ABIs, then continue through the early-game address range instead of stopping at selector-sensitive runtime edges.


## 2026-10-01 mid-UI batch 03
Verified total is now **19,557 / 70,402 = 27.78%**. Newly integrated: `1464..15AD`, `1A04..1EE0`, `1EE4..1EFE` (1,597 bytes). `1F00..1F23` is the nine-entry CS jump table used by the following routine and must remain data. Continue independently at `07E4..10A1`, `10A4..1461`, `15B0..1A00`, or from the function beginning at `1F24`; do not count alignment bytes.


## 2026-10-01 actor/UI batch 04
Verified total **20,970 / 70,402 = 29.79%**. Added 1,413 bytes at `1F24..2196`, `24D4..266B`, `28E4..2990`, `2994..2A60`. 1F24 preserves incoming DL until the encoded decision tree assigns it and resolves the 1F00 jump-table targets. 24D4 preserves guest scratch handles and nine-frame page alternation. Alignment bytes excluded. Active staging unchanged.


## 2026-10-01 actor generation batch 05
Verified total **24,603 / 70,402 = 34.95%**. Added **3,633 bytes** across `2A64..38A3` (alignment excluded). This covers class/order editing, bounded name input, final actor accept/delete, progression derivation, base/random stat generation, race/class minima, aggregate/clamp logic, and the 3884 initialization driver. Guest actor records remain dgroup-backed; external renderer/input/timing calls remain explicit boundaries. Active staging unchanged.


## 2026-10-01 actor stat/presentation batch 06
Verified total **28,889 / 70,402 = 41.03%**. Added **4,286 bytes** at `38A4..3C54`, `3C58..40FA`, `40FC..44E0`, `44E4..451E`, `4520..45EE`, `45FC..4976`. `45EF..45F8` is proven lookup data (`678456789:`); adjacent LEA/NOP alignment is excluded. 3C58 preserves event 7/8/9 stat-edit behavior and the `4D4` bit-0 alternate set/inc/dec semantics. 40FC preserves its EBX three-state control flow. 45FC preserves the 10-frame then 25-frame loops for each of two groups, including 44E4 paired-counter wrapping. Active staging is unchanged.


## 2026-10-01 post-generation batch 07
Verified total **33,000 / 70,402 = 46.87%**. Added **4,111** instruction-accounted bytes from `4978..59AB`, excluding enumerated alignment/data. This batch covers the character-generation controller, 5x256 palette remap construction, resource 6C/74/78/94 setup, actor finalization, shutdown wrapper, cursor callback/page state (`50C0..5878`), and first 9-byte palette search-tree insertion (`5880..59AB`). Preserve 0101h tree sentinel, 9-byte record stride, cursor depth at 73EC, page byte 73EE, 400-byte backing pages at 73EF, and the encoded zero-count resource-78 no-close behavior. Active staging remains **904 bytes**.


## 2026-10-01 mid-runtime batch 08
Verified total **36,671 / 70,402 = 52.09%**. Added **3,671 bytes** across palette nearest/tree construction/search (`59AC..5D8E`), actor class/table population (`5D90..6053`), RNG/DOS wrappers and display initialization (`6184..6653`), and clipped display wrappers (`6654..6851`). Preserve the AX-only MOVZX aliasing in 59AC, BL-as-input behavior in 5D90, explicit 16-bit tree stack/sentinels in 5B64, and guest/global address semantics. `6060..6183` is deliberately uncounted pending exact hardware/interrupt stack-register modeling.


## 2026-10-01 mid-graphics batch 09
Verified total **39,995 / 70,402 = 56.81%** after adding **3,324 bytes** from selected instruction-bounded ranges in `6854..71C7` and `72A0..7669`. `71C8..729F` remains unresolved and uncounted. Preserve the clipped-image output structure at `240C + depth*18h`, DPMI guest-address boundaries, and buffered stream state globals `2870/2874/2878/287C`.

## 2026-10-01 mid UI batch 10 checkpoint
Verified total **43,936 / 70,402 = 62.41%**. Added 3,941 bytes across the resolved `71C8..729F` allocator pair and selected `767C..87C1` UI/resource/font/input/vector/render helpers. Preserve the 766C four-entry CS jump table as data, the explicit incoming high EDX dependency in 7844, the uninitialized local-byte dependency in 79C8, signed-vs-unsigned glyph indexing differences, and the interrupt queue globals 2919/291A/2930. Interrupt bodies 7F5C, 80BC, and 8338 remain uncounted and must be modeled with their full saved-register/segment/IRET frames before integration.

## Batch 11 checkpoint

Verified total is now 48,052 / 70,402 bytes = 68.25%. Batch 11 integrated 4,116 instruction-accounted bytes from the asset/input lifecycle regions. Keep `8E3C..91BB` unresolved until its seven-way CS table dispatch is reconciled; do not count the tables at `8E20..8E3B` or `91BC..91D7`.

## Batch 12 checkpoint
Verified total **51,819 / 70,402 = 73.60%**. Batch 12 added 3,767 directly reconciled bytes spanning VGA state save/restore and interrupt wrapper (`6060..6183`), the seven-way asset dispatcher (`8E3C..91BB`), selected video/image helpers (`9BE0..A058`, `A558..A6EF`), startup/register bridge (`CD97..CE8A`, `CEC0..CEE8`, `CEF0..D08B`, `FEFC..FFD3`), and the INT24 trampoline (`E2C3..E35F`). `CE8B..CEBF` remains staged; do not count it.


## Batch 13 checkpoint
Verified total **55,948 / 70,402 = 79.47%**. Added 4,129 instruction-accounted bytes from `AE0C..BE60` excluding explicit zero/alignment islands. Preserve AE5C register-input ESI/EDX semantics, AEA8 register-result address/plane calculation, AEDC EAX-preserving side effect, AF34/AF4C retrace loops, and the resource/audio globals 2B84/2B90/2B98/2B9C/2C08/2C28/2C30/2C34/2C38/2C3C/2C40/2C44. CE8B..CEBF remains the only active startup staging.


## Batch 14 checkpoint
- Verified total after this fold: **59,869 / 70,402 = 85.0388% (85.04%)**.
- Newly integrated: **3,921 bytes**: `00EC..035B` (624), `10A4..1461` (958), selected `BE70..C79E` plus `CBD4..CBEE` (2,339).
- `C79F..CBD0` is a self-modifying VGA blitter cluster: encoded immediates are patched through stores into instruction bytes. Do **not** lift it as ordinary immutable C++ control flow. Re-disassemble and model its patch points explicitly before counting.
- Batch 14 preserves guest pointers through callback boundaries; the `C5B0` four-byte stack-local read remains an explicit `call_6304_local32` boundary rather than an invented guest address.
- Next high-output candidates: the unresolved `C79F..CBD0` cluster only after a self-modifying-code model, plus remaining early-game gaps (`07E2..10A1`, `15B0..1A03`, `2197..24D3`, `266C..28E3`) that can be harvested independently.

## Batch 15 checkpoint
Verified coverage is **63,755 / 70,402 = 90.56%**. Batch 15 integrated `15B0..1A00`, `2198..24D0`, `266C..28E1`, functional `A21C..A554`, and `A6F0..A8F3` for **3,886 bytes** after direct CODE.1 disassembly. Preserve these specific facts: `2198` invalid modes return the incoming EDX value; `15B0` byte-toggles 7138 but pushes the full dword at 7138 to 53E4/5454; `A21C/A2B0` select copy direction from signed CX/DX while negating the full 32-bit register; `A7E8` patches two subtraction immediates on every entry, so its C++ delta is per-call rather than persistent hidden state. `07E4..10A1`, `C79F..CBD0`, interrupt-handler islands, `CE8B..CEBF`, and `DE6F..DEA0` remain high-priority unresolved/accounting candidates.


## Batch 16 integration checkpoint
Integrated **2,867 verified bytes**: the full `07E4..10A1` staggered transition renderer, interrupt handlers `7F5C..7F89`, `80BC..826D`, `8338..8397`, and shared startup exit tail `CE8B..CEBF`. The startup staging pool is now empty. Hardware frame entry/IRET and DOS termination remain explicit machine-boundary callbacks rather than host substitutions. Verified total: **66,622 / 70,402 = 94.63%**.


## Batch 17 integration checkpoint
Integrated **1,748 verified bytes** from `A05C..A219`, `A8F4..AA1F`, `AA20..AC0E`, and `AC10..AE0A`. The line rasterizer preserves the encoded endpoint normalization, major-axis decision, error-step tie behavior, VGA map-mask rotation, and graphics-controller mode setup/restore. The sprite family preserves frame offsets, four-plane rotation, explicit/default transparency keys, translation-table mapping, clipping scratch globals `2B55..2B79`, and the self-patched subtraction immediate as an explicit byte parameter. Verified total: **68,370 / 70,402 = 97.11%**. The next high-priority unresolved code is the call-targeted self-modifying blitter family `C7A0..CBD0`; its patched immediates are to be modeled as explicit fixed-point/runtime parameters before counting.


## Batch 18 integration checkpoint
Integrated **1,066 verified bytes** from the four call-targeted self-modifying VGA blitters `C7A0..C8C0`, `C8C4..C9D1`, `C9D4..CAC9`, and `CACC..CBD0`; alignment islands remain excluded. Runtime writes into instruction immediates are represented as explicit variables: destination plane masks, 8.8-style scale increments retaining the literal low byte `0x78`, source-plane strides, key bytes, and source wrap corrections. The translation-table variants preserve the binary distinction that `C7A0` compares the translated byte against the key while `CACC` writes every translated byte. Verified total: **69,436 / 70,402 = 98.63%**.


## Batch 19 integration checkpoint
Resolved the old DE6F ambiguity: the real direct-call entry is **DE70**, reached by `84B8`; `DE6F` is the preceding zero byte. Integrated `DE70..DEA0` (**49 bytes**) as the byte-aligning front end to the existing `DEA7` dword-fill core, preserving the initial destination read, 32-bit pattern rotation during alignment, exact dword-count handoff, and 1/2/3-byte tail. `DEA1..DEA6` are alignment NOPs. Verified total: **69,485 / 70,402 = 98.70%**.


## Batch 20 integration checkpoint
Residual-code and historical-accounting audit: added/accounted **186 unique machine-code bytes** at `0000..0002`, `7F50..7F5B`, `95A8..95BA`, `9E10..9E11`, shared `9EA7..9EAD`, `1058B..1058F`, `CBEF`, `CBF0..CC18`, and the previously translated-but-omitted `DF13..DF72`. Removed a **19-byte duplicate historical count** for `10384..10396` by renaming the older DOS-memory result helper so the actual machine address has one canonical representation. Net verified-accounting increase: **167 bytes**, to **69,652 / 70,402 = 98.93%**. The badge remains red pending complete classification of the remaining object bytes.


## 2026-10-02 integrity repair: 95BC
A post-interruption audit re-disassembled `95BC..9723` directly from immutable CODE.1 and found four semantic discrepancies in the older batch-11 lift. The integrated C++ now preserves the encoded `E280(95A8, CS)` callback installation before `8398`, calls `64B0(0)` rather than `64DC(0)`, passes `argv[1]` from the guest argv table at `30A0` to `E360`, and preserves the cdecl argument order of the `D10C(0,27E)` and `D130(0,C7)` calls. This changes no coverage count because all affected bytes were already counted; it is a fidelity correction only.


## 2026-10-02 final residual-classification checkpoint

Verified translated code remains **69,652 / 70,402 = 98.93%**. The new `Code1/RESIDUAL_LEDGER.md` is the authoritative curated residual-classification ledger for already-established non-code bytes. It contains **117 non-overlapping ranges totaling 448 bytes**: **118 table bytes** plus **330 alignment/zero/padding bytes**. Because the conservative residual before classification is 750 bytes, **302 bytes remain to classify or translate**.

Do not derive remaining coverage from `static_assert` gaps. A repository audit found 64,729 unique bytes enclosed by current range asserts and 5,673 bytes outside them, but many verified regions intentionally lack one assert per machine interval. Use the verified counter plus `RESIDUAL_LEDGER.md`, and investigate the remaining 302 bytes individually. The next priority is to classify undocumented inter-function gaps (especially the small gaps around the already-integrated self-modifying blitters and the F7xx/F8xx resource helpers), then sweep the remaining one-to-three-byte islands. Never promote a gap merely because it resembles compiler alignment; require current direct evidence or preserved authoritative disassembly evidence.


## 2026-10-02 residual pass 22 checkpoint

Fourteen current-source one-byte alignment comments were missing from the curated residual ledger and are now recorded: `4B17`, `4C7F`, `522F`, `550B`, `55DB`, `57DB`, `6303`, `782F`, `7843`, `7B73`, `7D7F`, `7E03`, `7E43`, `7E6B`. Classified non-code is now **462 bytes** and the unresolved conservative residual is **288 bytes**. Verified translated code remains **69,652 bytes (98.93%)**. Continue from repository evidence only; do not infer padding for the remaining islands without an explicit current or preserved integration record.


## 2026-10-02 residual pass 23 checkpoint

Preserved batch/staging records supplied explicit non-code evidence for 38 more bytes: `AEA6..AEA7`, `AF4B`, `AF63`, `B4D2..B4D3`, `B51B`, `B596..B597`, `B5E1..B5E3`, `B6BE..B6BF`, `B6EB`, `B75B`, `B76B`, `CEE9..CEEF`, `CF41..CF43`, `E187..E18F`, `E207`, and `E27F`. Classified non-code is now **500 bytes**; **250 conservative residual bytes remain**. Verified translated code remains **69,652 bytes (98.93%)**. `C79F` was deliberately not classified in this pass because the older exclusion record describes the formerly unresolved self-modifying cluster, not the exact semantic role of that one byte after batch 18.


## 2026-10-02 residual pass 24 checkpoint

Historical batch-12 evidence proves `9EA3..9EA7` was lookup data at that stage, while batch 20 later verified `9EA7..9EAD` as code. Therefore only `9EA3..9EA6` is newly classified as table data. Classified non-code is **504 bytes** and **246 conservative residual bytes remain**; verified translated code remains **69,652 bytes (98.93%)**. Continue by reconciling preserved integration scripts and their exclusion lists, not by assuming that every small inter-function gap is padding.


## 2026-10-02 residual pass 25 checkpoint

Integrator arithmetic, not padding inference, classified **104 more bytes**. Batch 11 contributes 50 alignment bytes; batch 13 contributes 4 generic non-code bytes omitted from its prose list but forced by its 4,181-byte span minus 4,129 verified bytes; batch 14 contributes 39 generic alignment/data bytes because its exact asserted low-continuation ranges sum to the declared code total; batch 18 contributes 11 alignment bytes as the complement of its four exact 1,066-byte code ranges inside `C79F..CBD3`. Classified non-code is now **608 bytes**, leaving **142 conservative residual bytes**. Verified translated code remains **69,652 bytes (98.93%)**. Do not use the stale batch-08 3,671-byte prose total for arithmetic classification; its named ranges do not reconcile to that number.


## 2026-10-02 residual pass 26 checkpoint

Batch 09 is arithmetic-closed from its preserved integrator: 3,390 selected bytes = 3,324 exact asserted verified code + 66 explicitly excluded alignment/data bytes. Those 66 bytes are now `noncode` in `RESIDUAL_LEDGER.md`. Classified residual is **674 bytes**, leaving **76 conservative residual bytes**; verified translated code remains **69,652 bytes (98.93%)**. Next priority is batch-12 reconciliation: decode its preserved compressed integrator as text only (do not execute it), verify its exact asserted intervals and exclusion language, and classify only an arithmetic-closed complement.


## 2026-10-02 residual pass 27 checkpoint

Corrected batch-10 arithmetic proves `7D67` and `7F8A..7F8B` as excluded non-code after subtracting later verified code `7F50..7F89` and all already-classified alignment/data islands. The exact batch-10 asserts, not its slightly short prose endpoint at `848F`, control the reconciliation. Classified non-code is now **677 bytes** and **73 conservative residual bytes remain**; verified translated code remains **69,652 bytes (98.93%)**. Batch-12's six remaining local gaps (20 bytes total) still lack explicit exclusion evidence and remain unresolved.


## 2026-10-02 residual pass 28 checkpoint

Small-checkpoint policy applied. Batch-08 preserved function boundaries plus its explicit `alignment excluded` integration note classify `631F`, `633B`, `6357`, and `63A2..63A3` as alignment (**5 bytes**). Classified residual is now **682 bytes** and **68 bytes remain unresolved**. Verified translated code remains **69,652 bytes (98.93%)**.


## 2026-10-02 residual pass 29 checkpoint

Batch-08 alignment checkpoint 2: `63B7` and `63D5..63D7` (**4 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **686 bytes**; **64 bytes remain unresolved**. Verified translated code remains **69,652 bytes (98.93%)**.


## 2026-10-02 accounting pass 30 checkpoint

Batch-08 accounting was re-audited from `integrate_mid_runtime_batch_08.py`. Its 47 exact asserted translated intervals total **3,384 bytes**, not the **3,671** historically added to the progress scalar. The integrator explicitly leaves `6060..6183` uncounted, so the **287-byte difference is an overcount**, not omitted batch-08 code. Batches 02-07 reconcile exactly to their historical deltas; batches 09-12 have independent exact reconciliations. Current verified translated coverage is corrected to **69,365 / 70,402 = 98.53%**. The conservative residual is now **1,037 bytes**; with **686 bytes classified non-code**, **351 bytes remain unresolved**. No translation semantics changed in this accounting-only pass.


## 2026-10-02 residual pass 31 checkpoint

Batch-08 alignment checkpoint 3: `63F1..63F3` and `6409..640B` (**6 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **692 bytes**; **345 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.


## 2026-10-02 residual pass 32 checkpoint

Batch-08 alignment checkpoint 4: `648D..648F` and `6632..6633` (**5 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **697 bytes**; **340 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.


## 2026-10-02 residual pass 33 checkpoint

Batch-08 alignment checkpoint 5: `6672..6673`, `66A7`, `6743`, and `67C7` (**5 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. Classified residual is now **702 bytes**; **335 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.


## 2026-10-02 residual pass 34 checkpoint

Batch-08 alignment checkpoint 6 closes the batch: `67F2..67F3` and `6823` (**3 bytes**) are classified from preserved exact function boundaries and the batch's `alignment excluded` note. All 62 complement bytes inside batch 08's advertised spans are now classified. Global classified residual is **705 bytes**; **332 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**.


## 2026-10-02 residual pass 35 checkpoint
Preserved `RuntimeGapE0D0.staging.cpp` proves `E0C8..E0CF` are eight zero bytes; they are now classified non-code. Classified residual is **713 bytes** and **324 bytes remain unresolved**. Verified translated code remains **69,365 bytes (98.53%)**. The audit also confirmed that `E174..E186`, `E1F4..E206`, and `E26C..E27E` were already integrated/countable in the historical +57 wrapper fold, preventing a duplicate count.


## 2026-10-02 residual classification pass 36

Historical commit `e825575221e2c2af5489201e18bfab21dc7f8c9d` introduced the structured mouse/copy fold with five explicit translated ranges: `D08C..D0B0` (**37**), `D0B4..D108` (**85**), `D10C..D12E` (**35**), `D130..D152` (**35**), and `D1E8..D239` (**82**), totaling the preserved **274 translated bytes**. The complementary bytes inside `D08C..D153` are therefore exactly `D0B1..D0B3`, `D109..D10B`, `D12F`, and `D153` (**8 bytes**). Because preserved evidence does not identify their narrower padding/data subtype, they are conservatively classified as generic non-code. Exact `static_assert` boundaries are now restored beside the existing C++ functions. Classified non-code rises from **713** to **721 bytes** and unresolved residual falls from **324** to **316 bytes**. Verified translated code remains **69,365 / 70,402 = 98.53%**; no already-counted code is credited twice.


## 2026-10-02 residual classification pass 37

Two preserved integration folds close six additional residual bytes by exact arithmetic without assuming an alignment subtype. Batch 12's `6060..6183` span contains **292 bytes**; its three exact translated intervals `6060..60B1` (**82**), `60B4..6100` (**77**), and `6104..6183` (**128**) total **287**, proving `60B2..60B3` and `6101..6103` (**5 bytes**) were outside translated instructions. Batch 17's contiguous sprite family `A8F4..AE0A` contains **1,303 bytes**; its exact translated ranges `A8F4..AA1F` (**300**), `AA20..AC0E` (**495**), and `AC10..AE0A` (**507**) total **1,302**, proving `AC0F` is the sole excluded byte. All six are classified conservatively as generic non-code because the preserved repository evidence does not uniquely recover a padding-vs-data subtype. Classified non-code rises from **721** to **727 bytes** and unresolved residual falls from **316** to **310 bytes**. Verified translated code remains **69,365 / 70,402 = 98.53%**; no translated coverage is added or double-counted.


## 2026-10-02 residual classification pass 38

Historical integration evidence closes **194 additional non-code bytes** without promoting any unverified instructions. Commit `bcb06766a868cec3bf7c8b93eda2faa128088046` records the encoded `CC1C` short jump to `CC96` and explicitly identifies `CC1E..CC95` as **120 bytes of embedded Watcom run-time text/data**, not executable instructions. This exactly reconciles the startup inventory: `CC1C..CD96` is 379 bytes total, partitioned into **259 translated bytes** (`CC1C..CC1D` plus `CC96..CD96`) and **120 data bytes**.

Commit `c6fa0ed1974736c6c124b3f600f2b7d36fc2ed94` explicitly identifies `DF8E..DFB2` and `E02B..E04F` as two identical embedded radix digit alphabets, **37 bytes each**. Together with the already-classified `DE6F` and `DEA1..DEA6`, these **74 data bytes** close the full `DDE0..E0C7` inventory exactly: **663 translated + 81 non-code = 744 bytes**.

Classified residual therefore rises from **727** to **921 bytes** (479 alignment/zero/padding, 196 table/lookup data, 246 generic proven non-code), leaving **116 bytes unresolved**. Verified translated code remains **69,365 / 70,402 = 98.53%** and the badge remains red.


## 2026-10-02 accounting correction pass 39

A full address-union audit exposed a second historical duplicate: `integrate_heap_core.py` had already credited the entire `104E0..1083E` heap core as **863 bytes**. Its explicit subrange asserts total 858 bytes because the five-byte shared jump `1058B..1058F` is represented by the early-exit semantics around `function_10590`, but those five bytes are part of the original 863-byte scalar increment. Batch 20 later credited `1058B..1058F` again as if newly unique.

The verified scalar is corrected by **-5 bytes**, from **69,365** to **69,360 / 70,402 = 98.5199% (98.52%)**. Batch 20's corrected contribution is **181 newly unique bytes - 19 duplicate bytes = +162**, not +167. Classified non-code remains **921 bytes**. The conservative residual becomes **1,042 bytes**, leaving **121 bytes unresolved**. No translated semantics are removed; this is accounting-only. The badge remains red.


## 2026-10-02 residual classification pass 40

A repository-history/reference audit closed **11 additional bytes** using arithmetic-bounded regions that were explicitly preserved before their later translations. The batch-14 continuation named `07E2..10A1` as the remaining early-game candidate; batch 16 subsequently verified `07E4..10A1`, leaving `07E2..07E3` outside instructions. Batch 10 bounded the deferred second interrupt/state-machine block as `80BA..826F`; batch 16 verified `80BC..826D`, leaving `80BA..80BB` and `826E..826F`. Batch 10 likewise bounded `8336..8397`; batch 16 verified `8338..8397`, leaving `8336..8337`. Finally, the declared `DAD9..DDD7` allocator/decoder/transform family spans 767 bytes while its three exact verified assertions total 764, proving `DC8D..DC8F` is the only excluded island.

All **11 bytes** are recorded as generic proven non-code because preserved evidence closes them arithmetically but does not uniquely identify their narrower padding/data subtype. Classified residual rises from **921** to **932 bytes**; unresolved residual falls from **121** to **110 bytes**. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.


## 2026-10-02 residual classification pass 41

The preserved batch-12 integrator closes **5 additional bytes** by exact arithmetic. Its complete `A558..A6EF` image-helper family contains **408 bytes**. The four exact translated intervals `A558..A57B` (**36**), `A57C..A5C5` (**74**), `A5C8..A674` (**173**), and `A678..A6EF` (**120**) total **403 bytes**, proving `A5C6..A5C7` and `A675..A677` are the complete five-byte exclusion. They are recorded conservatively as generic proven non-code because the preserved evidence does not uniquely recover padding-vs-data subtype.

Classified residual rises from **932** to **937 bytes**; unresolved residual falls from **110 bytes / 25 islands** to **105 bytes / 23 islands**. Verified translated code remains **69,360 / 70,402 = 98.52%** and the badge remains red.


## 2026-10-02 decoded full-history residual audit after pass 41

A repository-wide scan traversed every historical textual `Code1` blob reachable from Git history and decoded the compressed Python integration wrappers before searching the **23 remaining residual islands** and their adjacent entry markers. The scan did **not** recover an additional explicit classification strong enough to move any of the remaining **105 bytes** into translated code or proven non-code. In particular, the largest separators `6054..605F`, `BE61..BE6F`, and `F742..F74F` remain boundary-shaped but unclassified; adjacency/alignment appearance is not proof.

The audit also found a concrete reason not to arithmetic-close the older recovery batch `9BE0..9EA2` from its broad prose description alone: that historical lift treated the complement of its selected code as data/alignment, but later direct reconciliation proved `9E10..9E11` are executable code. Therefore `9C98..9C9F`, `9E89..9E8B`, and the other surviving separators remain unresolved unless independent byte-level evidence is preserved in the repository.

Authoritative accounting is unchanged: **69,360 translated + 937 classified non-code + 105 unresolved = 70,402 bytes**. The translated-code metric remains **98.52%**, and the badge remains red.


## 2026-10-02 residual classification pass 42

Repository-wide history recovery found exact address-level padding annotations in the older uppercase `CODE1/` direct-reconstruction tree. Individually named ranges totaling **59 bytes** are now classified without using alignment appearance as inference: `1462..1463`, `6054..605F`, `9BD7..9BDF`, `9C98..9C9F`, `9E89..9E8B`, `9EAE..9EAF`, `A00E..A00F`, `A059..A05B`, `A21A..A21B`, `AE0B`, and `BE61..BE6F`. Classified non-code is **996 bytes** and **46 bytes / 12 islands remain unresolved**. Verified translated code is unchanged at **69,360 / 70,402 = 98.52%**; badge remains red. Only exact range annotations were accepted; broad historical claims remain insufficient.


## 2026-10-02 residual pass 43 checkpoint

A normalized scan of the historical uppercase `CODE1/` reconstruction recovered direct byte-level evidence for `000EB`: `ObjectLayout0000.cpp` records it as a NOP alignment byte immediately after reconstructed `004C..00EA`. This raises classified residual to **997 bytes** and leaves **45 bytes in 11 islands** unresolved. Verified translated code remains **69,360 bytes (98.52%)**. The same sweep found no target-specific classification for the other islands; do not infer their status from adjacency or alignment appearance.


## 2026-10-02 direct CODE.1 residual closure (pass 44)

The original binary is controlling evidence and was used directly. Verified identity: size **277,787** bytes; SHA-256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`. Object 1 extracted at file `0x2C724`, length `0x11302`, has SHA-256 `c0bf2c313aa8a24c9a0c0a3ce498f87a1d09299bb826413acaeb844412007203`.

The final 45-byte residual was resolved byte-for-byte. Forty-four bytes are proven padding/data; `FC97` is executable `RET`. The LE header at file `0x27BDC` gives fixup-page-table offset `0x15D` and fixup-record-table offset `0x1C1`. In the fixup record table, the record at table-relative `0x4757` (file `0x2C4F4`) is `07 00 28 03 01 97 FC`. Per the IBM/Open Watcom LX/LE format, source type `07` is a 32-bit offset fixup; flags `00` select an internal fixup with byte object number and word target offset. The record therefore targets object 1 offset `FC97h`, proving the one-byte RET is an address-distinct callback rather than filler.

Final object partition after integration: **69,361 translated instruction bytes + 1,041 classified non-code/data bytes = 70,402 / 70,402 bytes**. Residual queue: **0 bytes**. The green badge denotes 100% object accounting, not a false claim that embedded data became translated C++. A full end-to-end semantic audit against the original binary remains the next CODE.1 activity; corrections discovered there do not increase coverage.


## Semantic audit checkpoint 2 — `1464..38A3`

Direct re-disassembly from the immutable original `CODE.1` found and corrected semantic mismatches in the already-counted C++ without changing byte coverage. Corrections include: encoded `954C` argument order in `1464`; exact `1464` caller slot/label placement in `1CD0`, `1DD8`, `1E64`, `2A64`, `3020`, and `3138`; the missing `00EC` materialization and corrected renderer argument order in `1A04`; the missing `04B0`/page flip sequence in `1BD4`; two-page animation state, renderer targets, and call argument order in `15B0` and `24D4`; direct hit-test/hover paths in `2A64`; timing/input details in `2DFC`; the missing `7868` recapture in `3020`; signed `IDIV` semantics in `3208`/`3404`; and the 16-bit store/reload truncation points in the `3404` HP clamps.

The remaining routines in this checkpoint, including `1F24`, `2198`, `266C`, `28E4`, `2994`, `32F4`, `334C`, `37C0`, and `3884`, were rechecked against fresh disassembly and required no correction in this pass. Object accounting remains **69,361 translated instruction bytes + 1,041 proven non-code/data bytes = 70,402 / 70,402**, with **0 unresolved bytes**. This checkpoint changes semantics only; it does not increase coverage.


### Semantic audit checkpoint 3 (`38A4..4976`)
Direct CODE.1 re-disassembly corrected 67C8 cdecl argument placement in `3C58/40FC/45FC` and restored the encoded mixed-width use of DGROUP `7138`: byte reads feed `6588/65A4`, dword reads feed `55DC/53E4`, and A390 receives the dword with only AL XORed. Coverage remains closed at 70,402/70,402; this is a semantic correction only.


## Semantic audit checkpoint 4b — direct recheck of `50C0..5B63`

A deeper direct recheck of the immutable original `CODE.1` found additional fidelity defects inside the region previously marked clean by checkpoint 4. This correction pass fixes five encoded details without changing byte coverage: `50C0` now consumes five 32-bit stack slots and reads its fifth argument as a guest-linear address constructed by the `6104` interrupt veneer; its initial `SAR` and signed word-depth gate are modeled exactly. `5230` retains its promoted 32-bit input slot while storing only AX. `5880` now exposes the compiler's 9-byte by-value record as three consumed dword slots plus the following 32-bit record-index slot, including the source-index word straddling slot0/slot1. `59AC` uses unsigned modulo-2^32 arithmetic to preserve IMUL/add wrap before the signed low-word comparison. `5A34` reproduces the actual by-value stack image (`green, red, blue, source-index, zero padding`) used by both callers of `5880`. `4B18`'s partial-register operations were rechecked and are equivalent because ESI's upper word is proven zero before the only full-width subtract; its word truncations remain value-preserving in the encoded 0..63 domain. Object accounting remains **69,361 translated instructions + 1,041 proven non-code/data = 70,402 / 70,402**.
