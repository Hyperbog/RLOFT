# P0 — Three-game comparison and Strahd validation baseline

**Status:** source checkpoint, no Windows build or test execution. The
candidate-export counts below were supplied from separate application runs,
not independently recomputed from the proprietary binaries in this change.

| Metric | Strahd CODE.1 | Menzo CODE.1 | Stone CODE.1 |
| --- | ---: | ---: | ---: |
| Candidate instructions | 16,711 | 14,785 | 13,615 |
| Candidate functions | 331 | 356 | 310 |
| Basic blocks | 3,980 | 3,847 | 3,372 |
| Flow edges | 18,917 | 16,876 | 15,370 |
| Data references | 1,963 | 1,863 | 1,431 |
| Recovered indirect table candidates | 1 | 0 | 0 |
| Unresolved indirect calls | 19 | 22 | 42 |
| Unresolved indirect jumps | 5 | 8 | 5 |
| Reported diagnostics | 1 | 0 | 0 |

These are not comparable measures of fully verified coverage. The exported
count of 16,711 Strahd decoded instructions **is not the same metric as**
the independent Strahd byte-verification total.

## Separate verified Strahd source evidence

`Code1/FINAL_BINARY_AUDIT.md` and `Code1/PROGRESS.md` establish:

- Strahd CODE.1 SHA-256:
  `fd9b709ceee190e6a70870f9012ebea3bc437cae6bc74e0150d6f922fa750fed`
- Original byte length: `277787`.
- LE object 1: 70,402 address bytes (`0x11302`).
- Object 1: 69,361 independently verified translated instruction bytes
  and 1,041 proven non-code/data bytes; zero unresolved in that audit.
- Proven one-byte RET callback at object 1 offset `0xFC97`, supported by LE
  relocation source evidence in the archived audit.
- Proven embedded `20 20 00 00` literals at object 1
  `0xF7B3..0xF7B6` and `0xF8BD..0xF8C0`, inclusive.

These are **Strahd-only** reference facts. They are suitable for future
hash-gated known-answer validation; they do not prove the Strahd candidate
export is complete or authorize copying labels to another game's `CODE.1`.

## Immediate report improvements

`JumpTableCandidate.DispatchKind` distinguishes
`indirect-call-table`, `indirect-jump-table`, and
`unclassified-indirect-table` (legacy stored snapshots).
`jumpTableCandidates` remains the compatible shared list for now, but
JSON includes separate call/jump counts and text shows the kind. A
relocation-backed indirect CALL is **not** a jump. Indirect runtime
possibilities remain unresolved regardless of candidate target recovery.

`FlowDiagnostic.Conflict` reports a rejected decode's proposed assembly,
retained instruction owner and assembly where present, reason, disposition,
and unresolved evidence status. The final instruction set can have no
overlapping accepted instructions while preserving an important rejected
alternative at Strahd `1:0xB04B`.

## Next P0 validation gates

1. Define versioned source-hash-gated reference manifests and machine-readable
   match/missing/unexpected/disputed outputs. Compare across application
   versions **without modifying the controlling original bytes**.
2. Maintain source game ID/archive/internal path where supplied, and isolate
   game-specific analyst labels, inferred names and verified names.
3. Compare direct edges, original-file offsets, LE fixups, callable entries,
   non-code islands, table destinations and conflict dispositions.
4. Retain candidate-versus-verified provenance in all comparisons.
5. Cross-game normalized function similarity is *only* a lead for research,
   never automatic semantic equivalence.

**Never store Menzo/Stone originals in a Strahd fixture or identify either by
the shared filename `CODE.1`.** The original Strahd game files were not
committed or uploaded to CI. Build automation remains manual-only.

## Source-bound known-answer validator (source checkpoint)

`KnownAnswerValidator` now accepts JSON manifests for a single original
executable (game ID, module, **complete** SHA-256, byte length, expected LE
format, independent research source and typed known-answer expectations).
A hash/length mismatch **rejects** validation rather than comparing a
similarly named `CODE.1` from another game.

The initial `tests/fixtures/strahd-code1-known-answer-v1.json` consists
solely of independently archived observations: the one-byte `C3` callback
at `1:0xFC97` and two embedded four-byte non-code literals. Optional archive
and internal path are null because these fields are not established by the
cited audit; never invent them. The validator does **not** modify seeds,
code/data classifications, names, or original bytes.

A comparison may report `match`, `missing`, `conflict`,
`not-observed`, or `not-classified`. Unbenchmarked instructions are
counted as such, **not** automatically marked incorrect. An independently
verified callback that is absent from a one-seed traversal remains
`not-observed`, not disproven. The CLI accepts `--known-answer
reference.json --validation-out results.json` with `analyze` only. The
result is JSON-only, created atomically without replacing existing evidence.
Regression tests were added but **not executed**.
