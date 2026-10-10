# DOSDis x86-16 external-decoder comparison

**Status:** SOURCE COMPLETE / UNCOMPILED / NOT RUN (2026-10-10).  
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

The report includes each expected instruction's source/oracle/graph agreement status (`match`, `missing`, `conflict`, `not-observed`), a machine-readable discrepancy detail, sample denominator, unbenchmarked decoded starts, truncation marker and source identity. A candidate match is **not** proof that the instruction executes or that every instruction has been discovered.

### Required independent G2 evidence still missing

This code is only the comparison interface. To complete the G2 accuracy gate under the [predeclared matrix](INDEPENDENT_VALIDATION_G2_PREDECLARED_SCOPE.md), authorized validation must:

- Run a genuinely distinct decoder and record its version, origin, mode and independent byte-window results; do not use DOSDis/Iced as the external producer
- Reconcile semantic instruction forms and operands beyond the currently supported byte-length/flow/direct-edge subset against independent ISA expectations
- Cross-check loader maps, x86-16 guard/dataflow and rejected/unsupported controls against original-byte research or a second independent reference class
- Record exact DOSDis **built commit SHA**, analysis flags, source hashes, reviewer, sampled denominators and the disposition of every mismatch
- Retain the distinction between compiled, synthetic verified, original-file compared, and human-accepted independent accuracy

A matching external comparison report alone never marks G2 or any release gate accepted.
