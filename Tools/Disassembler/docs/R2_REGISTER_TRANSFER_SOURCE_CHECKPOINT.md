# R2.2 — source-backed x86-16 register transfer foundation

**Branch:** `disassembler-foundation` | **Status:** SOURCE COMPLETE,
STATIC REVIEW ONLY. No Windows build, synthetic test execution, CI run,
original-binary run, or GUI interaction occurred in this checkpoint.
Previous G1 is still READY / NOT AUTHORIZED.

## Scope

`Dos16RegisterTransfer.Apply(region, instruction, before)` performs a
single, independently validated, read-only x86-16 transfer. It checks:
- exact original SHA-256, physical file region, segment, origin, byte extent
  and mapping evidence of the input register snapshot;
- matching source physical offset, decoded length, stored opcode bytes,
  address and flow-control type with the pinned 16-bit decoder;
- exact prefix-free opcode length and register-register ModRM shape.

Only these byte-level operations are modeled:

| Encodings | Effect |
| --- | --- |
| `90` | Preserve register values and flags |
| `B8..BF iw` | MOV r16,imm16, preserve flags |
| `89 /r`, `8B /r`, register ModRM only | MOV r16,r16, preserve flags |
| `05 iw`, `2D iw` | ADD/SUB AX,imm16, flags become unmodeled |
| `81 /0,/5`, `83 /0,/5`, register ModRM only | ADD/SUB r16,imm16 or signed imm8 |
| `3D iw`, `81 /7`, `83 /7`, register ModRM only | CMP without changing registers; flags unmodeled |

All effects are candidate values and do not assert runtime execution.
The `FlagsPreserved` marker says only that the instruction does not
change flags, **not** that any flag is known to the analysis.

Any instruction outside that exhaustive subset (including partial byte
register aliases, memory operands, 32-bit overrides, prefixes, calls,
interrupts, stack operations, conditional control flow and unknown flags)
sets **all** tracked registers to Unknown and invalidates flag knowledge.
This deliberate precision loss is safer than retaining stale register
hypotheses across an unmodeled instruction.

All register snapshots are immutable, bound to the original region, and
start fully Unknown. Source-backed transfer origins use the established
full SHA-256 and physical offset evidence model; a matched original
instruction is still **not** proof of executable reachability.
No changes to the existing CFG graph or indirect target classification
are performed.

## Synthetic cases (written, NOT executed)

`Dos16RegisterTransferSmoke` runs through the existing smoke-suite
entry after `Dos16AbstractValueSmoke`. It contains only new, unencumbered
synthetic COM bytes. Coverage includes:
- MOV immediate to AX and BX; both register-copy directions;
- sign-extended negative imm8 ADD/SUB, word immediates and compare;
- byte alias invalidation and NOP preservation;
- known values after invalidation, arithmetic near 16-bit wrap;
- conservative JMP/RET invalidation;
- rejected forged original bytes, physical offsets and flow-control;
- rejected state from the same source at a different assumed origin,
  and from an entirely different source SHA-256.

These fixtures need the normal Windows G1/G2 compiler/test gate before
being classified as verified. No independent original DOS game files were
loaded for this work.

## Remaining Phase 2 work

R2.2 is intentionally narrow: it does not track flags, memory aliasing,
register high/low halves, far selectors, stack or side effects across
implicit calls. All such operations remain Unknown. R2.3 must implement
a bounded predecessor/worklist merge with safe monotonic convergence,
widening and explicit truncation. Branch refinements require original
condition-byte proof and a separate flag provenance model; do not apply
UnsignedInterval restrictions based solely on a matching CMP mnemonic.
R2.4 may use register values for pointer-table hypotheses only after
all graph paths and source-backed table entries are independently proven.

**Testing request:** None now. A single combined G1/G2 Windows build
and automated synthetic regression gate will need owner authorization
after a meaningful additional analysis batch. No user GUI interaction
is required by this source-only change.
