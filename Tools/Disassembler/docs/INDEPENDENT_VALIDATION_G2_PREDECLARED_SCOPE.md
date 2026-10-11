# DOSDis G2 independent validation — predeclared scope

**Current status (2026-10-10): PARTIALLY EXECUTED, NOT G2-ACCEPTED.** Latest owner-authorized [Windows/Linux build run #42](https://github.com/Hyperbog/RLOFT/actions/runs/38096831427) **PASSED** at source-equivalent canonical C#/Python code/test SHA `3956f036fee44c0b6f07361811e4f0857dbea916`; both C# synthetic suites passed on Windows/Linux and **17 synthetic Python tests** passed on Linux. Three preview artifacts were published. The new strict MZ relocation digest/geometry reconciliation has **not** been compared to any original game files. The earlier independent original Strahd outer-MZ two-window result (run #40 compiled SHA `7fa76f822e0aa56a4bf1c611076bc50e5e0a5389`) remains **28/28 selected instructions, 15/15 structured operands, 5/5 direct edges**, plus 8 negative controls, for only ONE shared launcher implementation. Those old original-code results cannot be automatically reassigned to the new #42 build. This is **not** whole-file, cross-game or human reviewer-approved G2 accuracy coverage. The remaining matrix below is still an independent acceptance obligation, not further build/CI authorization. See [G2 #42 detailed build record](G2_BUILD_GATE_RECORD_20261010_RUN42.md).

**Authority:** [Independent Validation Policy](INDEPENDENT_VALIDATION_POLICY.md), [Roadmap](ROADMAP_BUILD_GATES.txt). Applies to Phase 2's x86-16 decoder, bounded register lattice, unsigned branch guards, indexed table source provenance and conservative unresolved state.

## A. Mandatory independent sources

1. **External instruction oracle**: a separately implemented x86 instruction decoder, such as Capstone or Ghidra, with its exact version/build, architecture mode=16, supplied instruction bytes, decode origin, initial mode, endianness, and operand/display normalization recorded. Compare decoded length, opcode class, register operands, signed immediate interpretation and direct target to the same original-byte window. The built-in Iced.Intel invocation does *not* count as external.
2. **Separately sourced semantic and binary oracle**: published Intel/AMD x86 semantics and DOS executable-format documentation plus prior byte/offset research where available. For a real game fixture, record title, format, version when known, immutable SHA-256, LE/MZ/COM mapping and original source location. Do not infer 16-bit semantics from Strahd's LE CODE.1/.2 files merely because the filenames match other titles.
3. **Optional corroboration**: reproducible, locally authorized DOS/emulator execution or independently captured traces where available. Static candidate flow without execution is not a runtime trace.

Before executing tests, select the relevant original COM/MZ files using the independently documented corpus and pin their hashes. The previously proposed G3 examples include a known headerless COM and a Gold Box START.EXE; G2 may use a smaller source-identified sample. Never send proprietary binaries to public CI.

## B. Predeclared instruction / control-flow matrix

Create distinct exact-byte assertions with independent expected results for **each** category, including both positive and rejected/unknown forms:

| Sample family | Expected independently checked property |
| --- | --- |
| 16-bit MOV/AL/AX/AH/BH and partial-register writes | Register width and alias invalidation |
| CMP plus unsigned JA/JAE/JB/JBE, short and near forms | Flags, direct fallthrough/taken destination and inclusive bound count |
| Signed conditional branch control | Signed guard does not act as unsigned bound |
| ADD/SUB with immediate and 16-bit carry/wrap | Unsigned modular arithmetic and fail-closed range widening |
| SHL BX,1 and equivalent supported stride sequences | Index scaling without importing unsupported arithmetic |
| Near JMP/NOP transparent chains | Exact source-byte, length and unique predecessor proof |
| Register/memory indirect call and jump | Unresolved dynamic target unless a separate immutable source-backed pointer proof exists |
| Far pointers and segment transitions | Source, selector and relocation evidence separate; no inferred live CS/DS state |
| COM origin and MZ module relocation | Correct segment:offset versus physical-file mapping |
| Duplicated/stale/competing graph edges and instruction overlaps | Candidate proof rejection and conflict visibility |
| Truncated/budget-limited analysis | No unsupported bounded target or verified-runtime claim |

**Sampling policy:** predeclare selected cases from the external decoder/spec or preexisting original audits. Preserve the full expected set and every rejected/unsupported case. Do not select favorable examples after viewing new DOSDis outputs. Choose at least one valid and one rejection/control example for each supported feature being accepted; if not available, record that feature outside G2's acceptance scope rather than counting it as passed.

## C. Separate evidence records

For each independently checked executable and feature family, retain:
- Full source SHA-256, byte length, original image/mapping mode and address namespace.
- DOSDis exact commit/build SHA, CLI flags, decode/relocation mode, platform and test runner result.
- External decoder tool/version/configuration plus applicable specification or earlier research revision.
- Source-backed instruction original bytes (retained privately), expected lengths, operand widths and direct targets.
- Matched, missing, conflicting, not observed, not classified and unbenchmarked counts with denominators.
- All mismatches and dispositions reviewed by someone not solely relying on the fixture author's assertion.

Use [the independent evidence record template](INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md). Do not publish original game bytes, private absolute paths or full proprietary traces.

## D. Gate decision and blockers

A successful Windows compile and previously verified synthetic suite may mark their **own** states; they do not close G2's independent accuracy gate. G2 independent acceptance remains **PENDING** until a separately implemented decoder is actually run against the chosen exact-byte windows, independently grounded semantics and source identities are reconciled, all required unexpected missing/conflicting results are resolved, and a human reviewer approves the documented result.

The existing `KnownAnswerValidator` currently handles **LE object-relative comparisons**, not a blanket x86-16 external decoder driver. It must not be misrepresented as having completed G2. The newly added expected `ExpectedFlowControl` field and source-byte recheck are optional LE comparison safeguards; they are not a second decoder.

Historical note: those steps had not run when this initial matrix was written. Subsequent G2 run #40 executed the compiled/synthetic and bounded GNU objdump original-byte comparison portions; the broader coverage matrix, separate ISA evidence and independent human review remain PENDING.

## E. Source-only comparison interface (2026-10-10)

A bounded comparison interface is now authored in
`Dos16ExternalOracleValidation.cs`. The `com-analyze`,
`mz-analyze` and `overlay-analyze` CLI actions accept an
opt-in `--external-oracle file.json --external-report file.json`
pair. The manifest declares an independently researched
decoder provenance claim, exact original SHA-256 and region
mapping, and predeclared original-byte instruction length,
normalized flow category and optional direct edge.

It checks the immutable source and the returned candidate graph,
labels all reports `claimed-external-not-reviewed`, retains all
not-observed/missing/conflicting findings, and never upgrades
runtime reachability or G2 accuracy status. Its synthetic smoke
matrix tests interface integrity only. See
`X86_16_EXTERNAL_ORACLE_COMPARISON.md` for the supported
JSON schema and manual execution recipe.

**Still outstanding:** an actually executed second decoder,
independent operand/ISA semantic comparisons, reviewed
original-game samples, exact current build/test SHA, human
discrepancy adjudication and G2 owner authorization. Writing
the comparison code does **not** satisfy this gate.

## F. Restricted structured operand comparison (source-only)

The x86-16 candidate decoder now records optional structured
`decoderFacts` at instruction decoding: normalized mnemonic,
explicit operand count and the first two operand kinds, registers
and immediate semantic values. The outside-oracle manifest accepts
predeclared `expectedFacts` for the same limited subset.

The comparator reports mismatch in machine-readable form and
separate counts for all structured expectations and fully matched
structured expectations. A graph row without facts is
`not-observed`, not implicitly matched. Sign-extended imm8 values
are compared as signed numbers; mismatched register identities
are conflicts. See `X86_16_EXTERNAL_ORACLE_COMPARISON.md`.

**Uncovered:** all memory addressing **outside the restricted
16-bit ModR/M subset** below, runtime/default segment values,
full operand/address-size override effects, implicit operands and
flags, instructions with more than two operands, and runtime behavior.
Independent x86-16 operand evidence must still come from a
genuine separately implemented decoder and ISA reference;
the feature is **SOURCE ONLY** and not a completed G2 test.

## G. 16-bit ModR/M memory operand subset (SOURCE ONLY)

Optional external structured assertions may now cover ordinary
16-bit BX/BP and SI/DI ModR/M addressing combinations, direct
disp16, encoded displacement widths 0/1/2, canonical 16-bit
modulo displacement values, UInt8/UInt16/UInt32 scalar memory
operand width and explicit source segment overrides. Null
segment override means **no prefix**, not a verified runtime
default or selector. A `67h` address-size override cannot be
scored as a 16-bit ModR/M source match.

The source-only regression matrix contains a negative disp8,
explicit DS override on BP, direct disp16, byte memory access,
and unsupported 32-bit address override. Malformed/falsified
memory base, pairing, width, displacement and segment-prefix
assertions must never count as correct.

This does NOT execute an independent disassembler or address
runtime segment/flags state. Complete Capstone/Ghidra comparison,
independently reviewed original-file sample selection, negative
runtime tests and owner-authorized Windows G2 are pending.
See `X86_16_EXTERNAL_ORACLE_COMPARISON.md`.

## H. Original-byte sample deduplication (2026-10-10, SOURCE ONLY)

Do not infer *independent code behavior* from distinct filename,
whole-file size or whole-file SHA alone. User-supplied original
Strahd `CODE.1` and `CODE.2` have different full-file SHA256 values,
but share their entire 62,532-byte original DOS MZ image (SHA256
`37f3b1b3fc5e7a74d087a163f41e83dd5fc9184922a649d8a5eb128121f7204a`).
Their explicitly mapped 54,900-byte CS=`01BD` 16-bit entry windows
also share SHA256
`1be99d96f824922381da2c80faa74a58b45acaf025d072e48faa8bff16d3fa81`.
The two already independently sampled Strahd CODE.2 entry and
branch/call windows are therefore the SAME implementation if decoded
from CODE.1. They are **not a second 28-instruction benchmark**.
Record whole-file source identity *and* mapped-region and exact
sample-window byte identity; preserve unbenchmarked and shared
denominators separately.

DOSDis's external-oracle report source now exposes
`mappedRegionSha256` (backed physical region bytes) and
`benchmarkedInstructionBytesSha256` (ordered offset, encoded length
and backing bytes for explicit expectations). These fields are
**SOURCE ONLY** until another owner-authorized exact-SHA build.
Matching digest values indicate reused bytes, NOT independent
decoder accuracy or verified runtime reachability. Distinct sample
digests alone also do not prove semantic independence or correctness:
additional external instruction/ISA checks and reviewer approval
remain mandatory. See
`G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md`.

## I. Verified cross-game DOS binary candidates (source-only selection)

The original `DnD-Classics` branch was inspected at `MASTER/`,
including the full original `SOURCE_INVENTORY.csv` records. Four
different game/source paths were fetched as binary blobs and independently
SHA-256 checked against their original archives' inventories:
- Dungeon Hack `SBLASTER.COM`: 3904 bytes,
  SHA256 `da5568671a4992b05a97b3d269900671fc0c0ab1332e7ba4ec4efe509ad97757`.
- Dungeon Hack `CHECKSYS.EXE`: 15882 bytes,
  SHA256 `60070e69c2ca61e1adb6fc82bf4a3ed2652a576e40d610ec4f2b83df237fdc22`.
- Pool of Radiance `POOLRAD/START.EXE`: 65231 bytes,
  SHA256 `d825df2b174675c9088ba1489488bdeebe66ad2a22943f17d3a198e60b6a07bd`.
- Curse of the Azure Bonds `START.EXE`: 57789 bytes,
  SHA256 `42fcefd6785679568b00bc1fb5b1d81ac2773416be4a35e0363920ddf6b78acd`.

New independent-original samples are preselected from these distinct
format/mapping cases: a headerless COM with an original near relative
entry jump, a DOS MZ with 43 bounded relocation records, and two Gold Box
MZ files whose original **first 283 stored bytes at the mapped entry
are identical**. The identical Gold Box entry prefix is one shared
byte-evidence class, NOT 283 instructions or two independent code samples.
Do not infer a valid instruction boundary at the first differing byte,
assign runtime reachability or double source-backed sample denominators.

Full exact mapping, inventory verification, sample selection and
read-only preflight instructions:
`G2_MULTIGAME_ORIGINAL_SOURCE_SELECTION_20261010.md`.
The standalone Python source preflight and its wholly synthetic
regressions are SOURCE ONLY and have not run. No proprietary file
was added to development commits/CI. These four selected raw binaries
are accessible directly through GitHub; **no owner reupload is
currently required**. The corpus-wide archive/image completeness
issues in the collection audit do not invalidate the four independently
SHA-matched files, but remain separate collection-audit work.

Complete GNU objdump/another separate decoder and ISA semantic checks,
DOSDis-vs-external exact-build comparisons, relocation values with an
explicitly assumed load segment, unsupported/negative-case assertions
and a human discrepancy review are still required for G2 acceptance.

