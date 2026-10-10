# DOSDis x86-16 external-decoder comparison

**Status (2026-10-10):** Windows/Linux compile and both synthetic suites **PASSED** [run #40](https://github.com/Hyperbog/RLOFT/actions/runs/38091963405), tested source `7fa76f82`, merge checkout `5eaa1fb0`; strict oracle JSON import checks and controlled invalid-input CLI return were built. Owner-local original Strahd x86-16 samples **28/28 instructions, 15/15 structured operands, 5/5 edges MATCHED**, with 8/8 negative controls. **G2-wide independent accuracy/reviewer approval remains PENDING.** See [G2 original validation record](G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md).  
**Scope:** COM, MZ entry-code-region and explicitly mapped overlay x86-16 candidate graphs.  
**Not:** a decoder, a general 16-bit known-answer certification, an external decoder invocation, a runtime trace or a passed G2 gate.

See the [normative independent validation policy](INDEPENDENT_VALIDATION_POLICY.md), [predeclared G2 matrix](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md), and [verification-record template](INDEPENDENT_VALIDATION_RECORD_TEMPLATE.md).

## Purpose and independent evidence

An externally prepared manifest can now be compared against the same **source-identified** mapped x86-16 region and candidate graph. The expected interpretation must be established **before reviewing the new DOSDis output**, from a separately implemented decoder (such as Capstone or Ghidra) and a separately documented instruction/format oracle.

The module does **not** execute Capstone, Ghidra, or a second decoder. A tool name in JSON is an **unauthenticated claim**. The report always labels its independence `claimed-external-not-reviewed` and its analysis `candidate-comparison-not-certification`. Human verification of the claimed independent source, tool versions, raw sample selection, ISA details and discrepancies remains mandatory for G2.

The generated synthetic regression uses a hand-constructed, five-byte COM file; it verifies only the comparison implementation contract, not actual independence.

## Comparison input

Local JSON record `Dos16ExternalOracleManifest` (camelCase):

```json
{
  "schemaVersion": 1,
  "gameId": "synthetic",
  "module": "COM",
  "sourceSha256": "<64-character SHA-256 of the exact authorized input>",
  "sourceByteLength": 5,
  "regionFileOffset": 0,
  "regionByteLength": 5,
  "segment": 0,
  "origin": 256,
  "decoderName": "external-decoder-name",
  "decoderVersion": "independently-recorded-version",
  "decoderConfiguration": "x86-16 mode; exact decode origin and settings",
  "oracleEvidenceId": "independent-sample-selection-and-source-reference",
  "expectedInstructions": [
    {
      "offset": 256,
      "sourceFileOffset": 0,
      "originalHex": "EB02",
      "flowControl": "UnconditionalBranch",
      "evidenceId": "independently-reviewed-short-jmp",
      "directEdgeKind": "branch",
      "directTarget": 260
    },
    {
      "offset": 260,
      "sourceFileOffset": 4,
      "originalHex": "C3",
      "flowControl": "Return",
      "evidenceId": "independently-reviewed-near-ret",
      "directEdgeKind": null,
      "directTarget": null
    }
  ]
}
```

**The SHA field above is intentionally a placeholder** and the sample `decoderName` is not evidence of an actual outside decoder. It must be replaced by a real, externally sourced, independently reviewed record before making a real comparison.

The analyzer's `FlowControl` values are canonical names used for comparison normalization (`Call`, `ConditionalBranch`, `UnconditionalBranch`, `Return`, `Next`, etc.). Numeric aliases and unknown categories are rejected. Optional direct edges are limited to the supported decoded x86-16 near `call`, unconditional `branch`, and `branch-taken` categories. Other transfer targets remain unresolved; do not manufacture them.

A manifest is bound to full original-file SHA-256 and byte length, physical region start/length, explicit 16-bit segment and origin. Each expectation identifies the original **physical source-file offset**, instruction bytes (1–15 bytes), x86-16 flow category and optional independently established direct target. Duplicate starts, overlapping instruction windows, source-map mismatches and malformed/oversized manifests fail closed.

## Explicit local CLI use (only after owner authorizes G2)

From `Tools/Disassembler`, with a locally prepared independently sourced manifest:

```text
dosdis com-analyze /private/path/to/authorized.com --origin 100 --entry 100 --external-oracle /private/oracle.json --external-report /private/diff.json

dosdis mz-analyze /private/path/to/original.exe --external-oracle /private/mz-oracle.json --external-report /private/mz-diff.json

dosdis overlay-analyze /private/path/to/game.ovr --regions /private/regions.json --region SELECTED --external-oracle /private/overlay-oracle.json --external-report /private/overlay-diff.json
```

For an MZ loader assumption, record initial CS:IP and exact mapped original module offsets; overlay regions must be explicitly identified from the authorized source. The CLI does not select oracle windows, run an outside decoder or upload original bytes. The comparison export is a separate **non-overwriting** JSON file and is not a pass/fail release decision. Source-file paths in working commands should be redacted from shareable records.

The report includes each expected instruction's source/oracle/graph agreement status (`match`, `missing`, `conflict`, `not-observed`), a machine-readable discrepancy detail, sample denominator, unbenchmarked decoded starts, truncation marker and source identity. It also retains the exact source file length, mapped physical start and extent, 16-bit segment/origin, analyzed entry offset, source-region mapping evidence, and the claimed external decoder configuration. A candidate match is **not** proof that the instruction executes or that every instruction has been discovered. The executable build SHA and human review are recorded separately in the independent validation record; a comparison JSON alone cannot assert either.

### Required independent G2 evidence still missing

This code is only the comparison interface. To complete the G2 accuracy gate under the [predeclared matrix](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md), authorized validation must:

- Run a genuinely distinct decoder and record its version, origin, mode and independent byte-window results; do not use DOSDis/Iced as the external producer
- Reconcile semantic instruction forms and operands beyond the currently supported byte-length/flow/direct-edge subset against independent ISA expectations
- Cross-check loader maps, x86-16 guard/dataflow and rejected/unsupported controls against original-byte research or a second independent reference class
- Record exact DOSDis **built commit SHA**, analysis flags, source hashes, reviewer, sampled denominators and the disposition of every mismatch
- Retain the distinction between compiled, synthetic verified, original-file compared, and human-accepted independent accuracy

A matching external comparison report alone never marks G2 or any release gate accepted.

## Structured operand assertions (source-only extension)

A manifest instruction may now include optional `expectedFacts` to check
**decoder-reported, structured** x86-16 mnemonic and a deliberately
restricted first-two-operand subset. Unlike formatted assembly text,
these canonical fields compare register identity, operand encoding
width and immediate value directly. The source CFG rows retain
`decoderFacts` at the original decode site, derived from the
embedded Iced decoder; an external oracle must be prepared
independently from a separate decoder/manual ISA analysis.

Example of an optional `expectedFacts` member inside an
`expectedInstructions` entry for the standalone x86-16 bytes
`B8 34 12` (MOV AX,1234h):

```json
{
  "offset": 256,
  "sourceFileOffset": 0,
  "originalHex": "B83412",
  "flowControl": "Next",
  "evidenceId": "predeclared-external-mov-ax",
  "expectedFacts": {
    "mnemonic": "Mov",
    "operandCount": 2,
    "operand0": {
      "kind": "Register",
      "register": "AX",
      "immediateValue": null
    },
    "operand1": {
      "kind": "Immediate16",
      "register": null,
      "immediateValue": 4660
    }
  }
}
```

Only **zero, one or two explicit operands** may be asserted in this
version. Operand kinds supported for independent semantic agreement
are `Register`, `Immediate8`, `Immediate16`,
`Immediate32`, `Immediate8to16`, and
`Immediate8to32`. Sign-extended 8-bit immediates are represented
by their **signed semantic value** from -128 through 127:
`83 FB FE` (CMP BX,-2) is
`{"kind":"Immediate8to16","immediateValue":-2}`,
not unsigned 254. Mnemonics and register identities use exact named
Iced enum spelling only as a **normalization vocabulary**; this is
not proof that an independent decoder ran.

Memory addressing outside the explicitly supported 16-bit ModR/M
subset described below, address/operand-size prefix semantics beyond
the literal segment override, far and near branch operand encoding widths,
instruction implicit operands, flags, runtime effects and three-or-more-
operand semantics are **not** validated by `expectedFacts`. Such cases need separate
outside-oracle/ISA or original source evidence and may not be scored as
fully verified operand semantics by this comparator. The existing
`directEdgeKind`/`directTarget` checks are independent of this
restricted explicit-register/immediate subset.

Missing `expectedFacts` means **no structured operand assertion**
was attempted, not a pass. If an older graph lacks `decoderFacts`
but the oracle expects them, the result is `not-observed` with
`candidate-structured-operand-facts-unavailable`, not `match`.
A different mnemonic, operand count/kind, register or signed
immediate results in `conflict` with
`candidate-structured-operands-disagree-with-external-oracle`.
Bad, numeric, out-of-range or inconsistent oracle fields fail closed.

Reports carry separate `structuredOperandExpectations` and
`fullyMatchedOperandExpectations` counters. The latter is
**conservative**: an operand sample is counted only when the entire
source/flow/optional-edge finding also matches. Do not divide
the total instruction `matched` count by the number of operand
assertions or treat the absence of an assertion as correct semantics.

Synthetic coverage was added for MOV AX,1234h, CMP AX,1234h,
signed CMP BX,-2 with imm8-to16 extension, wrong AX/BX,
wrong immediate value, unsigned misinterpretation, unavailable
old graph facts, forged structured facts and malformed oracle
fields. These are source-only committed fixtures, **not a second
decoder run or a G2 accuracy result**.

## Narrow 16-bit ModR/M memory-address comparison (source-only)

The optional `expectedFacts.operand0` or `operand1` may now use
`"kind": "Memory"` with a **complete** `memory` object.
Only source-encoded ordinary **16-bit effective addresses** and
scalar `UInt8`, `UInt16`, or `UInt32` memory widths are accepted.
Supported base/index forms are BX+SI, BX+DI, BP+SI, BP+DI, BX,
BP, SI, DI and direct `disp16`. Encodings using an address-size
override (`67h`), 32-bit effective-address registers, scaled x86-32
indexes, VSIB or implicit string-memory operands are **outside scope**.

A source-backed `MOV AX,[BX+SI-2]` example encoded by
`8B 40 FE` has:

```json
{
  "kind": "Memory",
  "register": null,
  "immediateValue": null,
  "memory": {
    "base": "BX",
    "index": "SI",
    "displacementSize": 1,
    "displacement": 65534,
    "size": "UInt16",
    "segmentOverride": null
  }
}
```

**Displacement is a 16-bit modulo-address value**, not a signed
host integer or an inferred runtime address. The example's disp8
`FEh` sign-extends to `FFFEh` (65534) within the encoded 16-bit
effective-address calculation. `displacementSize` is the **encoded**
0, 1 or 2 byte displacement width; 1-byte displacements must
be a canonical sign extension of -128 through 127. Direct memory
`[1234h]` uses a 2-byte displacement with no base or index.

`segmentOverride` reports an **explicit source prefix only**:
`CS`, `DS`, `ES`, `SS`, `FS`, or `GS`. It is null when no
segment-override prefix is encoded, including accesses using the
default DS/SS selection. The comparator does **not** assert actual
runtime segment register contents or effective physical addresses.
For example, `3E 8B 46 00` (`MOV AX,DS:[BP+0]`) is different
from the unprefixed BP-based default segment selection.

An oracle memory operand with partial/invalid address facts, impossible
16-bit register pairing, unsigned non-sign-extended disp8, invented
memory width or segment override is **rejected**. A matching source
graph with an unsupported 32-bit-address override or missing memory
facts is **not-observed** with
`candidate-memory-address-facts-outside-supported-scope`, rather
than silently declared correct. Wrong-but-valid base, index,
displacement, width or override facts are **conflicts**.

The standalone synthetic fixtures cover four supported ModR/M
memory forms, an intentionally unsupported `67h` address override,
wrong-but-valid operand facts and malformed external expectations.
These fixtures have been authored but **not compiled or run**.
The independent decoder, human evidence review, processor state,
implicit memory accesses and full instruction semantics remain
outside this source-only checkpoint. This extension adds **no**
automatic release or accuracy certification.

## Source-decode integrity for a would-be match (SOURCE ONLY)

A cached CFG and an external expectation can contain the same wrong mnemonic,
operand facts, flow label or branch target even while their instruction hex
matches the original file. At the final matching stage DOSDis therefore
re-decodes the original physical instruction bytes at the declared
segment-relative IP. A would-be `match` is checked for source instruction
length, normalized flow, every retained cached structured fact, and any
expected direct near call/jump/conditional-taken target.

A source re-decode discrepancy yields `conflict` and the stable detail code
`candidate-decoder-or-direct-edge-disagrees-with-original-bytes`.
Missing graph edges and unobserved cached semantic facts keep their distinct
existing statuses and denominators. A memory manifest asserting a
displacement-free bare BP address is now invalid: 16-bit ModR/M encodes
`[BP+0]` using an explicit disp8 or disp16, never mod=00 BP-only.

The synthetic source adds adversarial jointly-forged reference/candidate
facts, fabricated matching branch edges, jointly-forged flow labels and an
impossible BP memory address. These checks have **NOT** been compiled or run.
The integrity check uses the same Iced decoder as DOSDis. It is **not** a
Capstone/Ghidra comparison, a reviewer-approved independent oracle or proof
of actual DOS execution; all G2 independent acceptance obligations remain
open.

## Post-build strict oracle JSON input contract (SOURCE ONLY)

The authorized Windows .NET 10 workflow
[run #38](https://github.com/Hyperbog/RLOFT/actions/runs/38088066909)
compiled the existing validator and passed synthetic x86-16 external-oracle
comparison fixtures. The runner's exact PR merge SHA was
`d8c404c56fed675457f5497859f242002e863732`, with code corresponding
to canonical `2945b1474289cb42c167000628255848b77b7896`.
This confirms compilation and synthetic contracts only. The runner did
not receive proprietary original files or execute a second decoder.

**Subsequent uncompiled source hardening** now rejects duplicate JSON
properties at *every object depth*, including case-only collisions.
Parsing checks one JSON snapshot before typed import, preventing an
ambiguous reviewed `originalHex`, `sourceSha256` or operand field
from being replaced by a second same-named field. Strict nested schema
checking also rejects unknown/misspelled keys rather than permitting
Web-default deserialization to ignore an intended assertion. Known
field names are limited to the declared manifest, instruction,
structured-facts, operand and memory objects; optional assertions may
be explicitly absent or null, but typo-created unknown names must
fail import.

Committed, synthetic-only negative cases include duplicate root and
nested instruction fields; misspelled `directEdgeKind`,
`expectedFacts` and memory `displacementSize`. The validated-field
set does not independently establish the claimed decoder producer or
the instruction's runtime reachability. Those cases still require a
new explicitly authorized compile and smoke test at the precise
source SHA.

A separately prepared owner-local GNU `objdump` 2.44 Strahd CODE.2
outer-MZ entry reference contains **11 consecutive instruction starts**
and **9 structured register/immediate assertions**. The manifest
and original byte excerpts are not committed to this repository.
This is an initial original-source comparison window only: it does
not exercise direct branch/conditional targets, signed branch guards,
finite-index proofs, MZ fixup effect assertions, or full cross-game
coverage. See `G2_STRAHD_ORIGINAL_COMPARISON_EXECUTION_PLAN.md`.
No DOSDis-versus-original manifest execution or human discrepancy
review is reported by this source-only follow-up.

## Distinguishing identical source-backed samples across binaries (SOURCE ONLY)

Whole-file hashes provide immutable **container** provenance but are
not proof that code samples are independent. Owner-supplied Strahd
`CODE.1` (277,787 bytes, SHA256 `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`)
and `CODE.2` (558,689 bytes, SHA256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`)
share the **same 62,532-byte DOS outer-MZ launcher**, SHA256
`37f3b1b3fc5e7a74d087a163f41e83dd5fc9184922a649d8a5eb128121f7204a`.
The mapped 54,900-byte source-backed CS=`01BD` entry-code region
at physical `0x1DD0` also has identical SHA256
`1be99d96f824922381da2c80faa74a58b45acaf025d072e48faa8bff16d3fa81`.
Those two original whole-file hashes must **not** be treated as two
independent observations of the earlier 28 tested 16-bit launcher
instructions.

Source-only `Dos16ExternalOracleReport` now carries two additional
non-certifying lowercase SHA256 fields, computed from **stored
original bytes**, never from the oracle's asserted `originalHex`:

- `mappedRegionSha256`: SHA256 of exactly `region.ByteLength`
  original bytes starting at `region.FileOffset`. File names,
  complete-file lengths and runtime load assumptions are not hashed.
- `benchmarkedInstructionBytesSha256`: SHA256 of concatenated
  per-expectation entries in ascending 16-bit IP order, each encoded
  as `uint16(offset LE)`, `uint16(expected byte length LE)`,
  then that many original physical file bytes mapped at the offset.
  Expected mnemonic, flow, operands, evidence ID and whole-file
  SHA are deliberately excluded from this fingerprint. Any wrong
  asserted hex is still reported as a **conflict** separately.

These fields can expose *reused sampled bytes* even when binary
containers differ; distinct fingerprints alone do **not** prove
independent code semantics, correct file mapping, reachability,
decoder independence, or review. Compare only like-for-like regions
and the same benchmark definition before assigning a shared sample
identity. Extremely short byte windows have low entropy; retain
full original-byte sample evidence privately.

Synthetic-only source changes also create a second COM artifact with
an altered unbenchmarked gap while preserving both observed
instructions. Its whole-file and mapped-region hashes differ,
but the benchmark byte fingerprint must be identical. A deliberately
incorrect oracle hex must not change that source digest. All new
hash fields and regression cases are **SOURCE ONLY, NOT BUILT OR
EXECUTED** since the last authorized run #40.

See [original-source G2 evidence and shared-launcher audit](G2_STRAHD_ORIGINAL_VALIDATION_RECORD_20261010.md)
and [G2 predeclared matrix](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md).

