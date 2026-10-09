# P0 — Stone Prophet cross-game regression intake (Strahd pending)

**Evidence class:** observations relayed from an external candidate-only
analysis export, **not verified against the original Stone Prophet binary**
in this conversation. The user intends to add a Strahd regression export next.

## Reported comparative metrics

| Metric | Menzo CODE.1 | Stone Prophet CODE.1 |
| --- | ---: | ---: |
| Candidate instructions | 14,785 | 13,615 |
| Candidate functions | 356 | 310 |
| Basic blocks | 3,847 | 3,372 |
| Flow edges | 16,876 | 15,370 |
| Data references | 1,863 | 1,431 |
| Jump tables recovered | 0 | 0 |
| Unresolved indirect calls | 22 | 42 |
| Unresolved indirect jumps | 8 | 5 |
| Flow diagnostics | 0 | 0 |

Reported Stone object-1 unknown range: `0x0463..0x6133` (exclusive endpoint
requires confirmation against the export). Reported interrupt count: 63 total,
including INT 21h 45, INT 10h 8, INT 33h 5, INT 31h 4, INT3 1.
The Stone file-size and SHA-256 prefix supplied in the assessment are
313,819 bytes and `004d042a70c6...`; this prefix is **insufficient for an
authoritative identity**. Never substitute it for a full SHA-256.

## Source-audited technical gaps

- `ReferenceRecovery.Inspect` currently returns early on **any segment prefix
  or nonempty memory base**. Thus the reported `cs:[edi*4+0BE38h]`,
  `cs:[edx*4+0C158h]`, `cs:[eax*4+119E4h]` dispatches and
  `call [eax+0D80h]` cannot be resolved by the existing table algorithm.
  Segment interpretation needs explicit provenance, not indiscriminate
  removal of the guard against overrides.
- The reported Stone `CMP [8BCh],7; JE; MOV EAX,[8BCh]; LEA EAX,[EAX*4];
  CALL [EAX+0D80h]` demands short bounded register data-flow and **call**
  table handling. No effective index bound is proven solely by a nearby
  comparison; alternate incoming paths and memory updates must be checked.
- Gaps cannot be called verified data. Analyze unknown-file-backed ranges
  with source-backed relocation locations, in/out references, candidate entry
  evidence, size and explicitly unknown reason. Do not linearly decode gaps
  or silently mark them as code.
- Runtime-dependent interrupt, callback, and register-indirect branches need
  dedicated transfer classes and optionally source-authenticated runtime
  observations. Register states at INT 21h or INT 31h must be evidenced,
  never fabricated.
- Build comprehensive confidence-tagged control-flow coverage ledgers; keep
  original source SHA, game ID, archive/internal path where supplied, and
  analyst/inferred/verified function namespaces strictly isolated.

## P0 source implementation in this checkpoint

`ControlFlowAnalysis.AddEdge` now emits bounded `FlowDiagnostic` entries
for unresolved indirect call/jump edges, **even when candidate indirect
destinations were also found**. `AnalysisReportExport` includes independent
`counts.unresolvedIndirectCalls` and `counts.unresolvedIndirectJumps`,
and the CLI summary and text export state those counts. Diagnostics remain
capped and candidate targets are not promoted to verified code. This avoids
mistaking a zero-error decode report for exhaustive control-flow coverage.

## P0 implementation order

1. Source identity and strict game/annotation namespace separation.
2. Full LE segment/object/relocation metadata and source-backed address maps.
3. Indexed jump and call recovery for CS overrides and bounded register flow;
   validate every recovered target and preserve unresolved edges.
4. Ranked unknown-gap audits linked to cross-references, fixups and seeds.
5. Source-verified unresolved-transfer accounting and coverage quality gates.

P1: DOS/DPMI interrupt-service annotations, callback and runtime-trace import,
function ownership confidence. P2: cross-game comparisons/annotated exports.
P3: interface polish.

**Verification status:** source-only review and new regression assertions;
no Windows compilation, test execution, proprietary executable upload or
GitHub Actions trigger. Await Strahd's separate evidence. 43% estimate unchanged.
