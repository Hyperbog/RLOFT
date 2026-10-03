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

## CODE.2 activation note

CODE.1 is now semantically closed. For current work, read `Code2/README.md` and `Code2/PROGRESS.md` immediately after this playbook. CODE.2 uses `Code2/Code2.cpp`, and its syntax check is `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp`. All CODE.1 addresses and old progress snapshots below are historical lessons only; never transplant them into CODE.2 without direct byte proof.

The currently verified CODE.2 executable-object identity is recorded in `Code2/README.md` and `Code2/PROGRESS.md`. CODE.1/`Code1.cpp` may be used aggressively for byte-signature matching, runtime identification, calling-convention hypotheses, and reusable semantic patterns, but CODE.2 bytes remain controlling evidence.

## CODE.1 historical identity and extraction

- Original file: `CODE.1`
- Size: 277,787 bytes (`0x43D1B`)
- SHA-256: `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- Object-1 executable extent: 70,402 bytes (`0x11302`)
- Object-1 file start: `0x2C724`
- Object-1 virtual base: `0x10000`
- Entry logical offset: `0xCC1C`

The complete original CODE.1 continuity record, including every historical checkpoint and semantic-audit lesson, remains canonical at `Code1/DECOMPILATION_CONTINUITY.md`. Do not duplicate the long historical append-only log here; consult that file whenever a CODE.2 routine resembles a CODE.1 compiler/runtime/game subsystem.

## Working batch strategy

Target roughly 4,000 newly verified bytes per substantial batch, but fidelity controls. This is a target, not a quota.

For each CODE.2 batch:

1. Verify the immutable CODE.2 SHA-256 and regenerate raw object-1 bytes.
2. Regenerate authoritative raw disassembly for the exact candidate range.
3. Establish coherent routine/control-flow boundaries and distinguish code/data.
4. Search CODE.1/`Code1.cpp` for likely byte/semantic matches when useful, but treat all matches as hypotheses until re-proven against CODE.2.
5. Trace calls, jumps, shared tails, register lifetimes, argument sources, globals, selector behavior, and object-2 data references.
6. Translate only semantics directly justified by CODE.2 bytes.
7. Put uncertain structural lifts in clearly marked staging only when needed; staging remains uncounted.
8. Reconcile instruction-by-instruction, including widths, signedness, partial registers, stack-slot consumption, x86 shift masking, modulo arithmetic, REP/string pointer state, and entry-before-DEC/JNE loops.
9. Fold only independently proven instruction-bounded code into `Code2.cpp`, syntax-check, then update `Code2/PROGRESS.md`.
10. Commit substantive work before reporting progress. Keep progress RED below 100%.

If one path blocks, record the dependency and move to another independent region.

## GitHub workflow

Repository: `Hyperbog/RLOFT`
Working branch: `code1-decompilation` unless deliberately changed later
Primary CODE.2 source: `Code2/Code2.cpp`
Progress ledger: `Code2/PROGRESS.md`
CODE.2 bootstrap/layout record: `Code2/README.md`

Always fetch the latest branch head and file SHA before writes. Do not leave temporary integration scripts or staging sources behind after a successful fold; Git history is the archive.

## Documentation/research policy

External documentation is secondary evidence used to understand compiler/runtime behavior; it never overrides CODE.2 bytes.

Prefer primary/official technical sources:

- Open Watcom documentation and source for compiler/runtime conventions.
- Rational/Tenberry DOS/4G and DOS/4GW documentation.
- Intel x86 architecture/instruction manuals.
- Microsoft MS-DOS technical/programmer documentation.
- Official/primary executable-format documentation for MZ/LE/LX structures and relocations.

Third-party reverse-engineering material may be used only as a lead and must be independently verified.

## CODE.1 lessons that must carry forward

The CODE.1 semantic audit repeatedly found errors caused by harmless-looking normalization. For CODE.2, specifically guard against:

- wrong cdecl argument order or promoted-slot consumption;
- treating byte/word register writes as full-register assignments;
- signed branch conditions being rewritten as unsigned comparisons or vice versa;
- C++ signed-overflow/overshift undefined behavior where x86 wraps or masks counts;
- `REP MOVS/SCAS` and `DEC/JNE` loops being converted to pre-checked loops;
- selector/far-pointer values being flattened into host pointers;
- callback/interrupt frames losing original DS/ES/SS or register aliases;
- cached globals across opaque external calls when the binary reloads them;
- address-distinct wrappers/thunks being normalized away;
- self-modifying immediates being represented as immutable code;
- code/data/padding classification inferred from appearance rather than direct evidence;
- runtime error-mapping wrappers bypassed in favor of simplified host behavior.

The completed `Code1/FINAL_BINARY_AUDIT.md` is the error catalog for these failure modes and should be searched whenever CODE.2 contains a similar routine family.

## Required behavior in future chats

When the user says `continue`, `keep working`, `resume`, or equivalent, perform actual CODE.2 binary analysis and commit substantive work before replying.

Before a continuation turn:

1. Read `Code2/DECOMPILATION_CONTINUITY.md`.
2. Read `Code2/README.md` and `Code2/PROGRESS.md`.
3. Inspect current `Code2.cpp` and branch head.
4. Verify the CODE.2 source identity.
5. Regenerate the relevant CODE.2 object and disassembly.
6. Use CODE.1 artifacts as accelerators where applicable, but independently reconcile against CODE.2.
7. Work, syntax-check, commit, then report exact ranges, byte counts, coverage effects, unresolved dependencies, and commit SHA(s).

Never claim a GitHub change exists until the connector confirms it. Never claim staging as integrated. Never count a byte twice.

## User-facing handoff prompt

> Continue the faithful CODE.2 decompilation in `Hyperbog/RLOFT` on branch `code1-decompilation`. First read `Code2/DECOMPILATION_CONTINUITY.md`, `Code2/README.md`, and `Code2/PROGRESS.md`, then inspect `Code2/Code2.cpp` and the current branch head. Treat immutable CODE.2 as the ultimate authority. Use the completed CODE.1 binary, `Code1/Code1.cpp`, and CODE.1 audit records aggressively as leads for shared runtime/compiler/game patterns, but prove every CODE.2 range directly from fresh CODE.2 disassembly. Preserve instruction-level semantics exactly, do not guess, keep uncertain work uncounted, target substantial batches without stopping at isolated blockers, and commit substantive work before reporting.
