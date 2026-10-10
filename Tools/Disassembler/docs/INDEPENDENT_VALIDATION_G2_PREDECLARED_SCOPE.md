# DOSDis G2 independent validation — predeclared scope

**Status: PLANNED / NOT RUN (2026-10-10).** This defines comparison obligations and selection before any new current-HEAD validation run. It is **not** a test result or permission to compile, execute original binaries or dispatch GitHub Actions.

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

No G2 source build, external-tool invocation, original-file comparison, machine-readable accuracy measurement or human review has yet been performed at this checkpoint.
