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
