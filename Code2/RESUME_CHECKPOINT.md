# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `c1abe4e8fcfdca9308a58f8c42444363c723a8d6` — `Integrate CODE.2 decompilation batch 61`.
- Verified translated instruction bytes: **178,417**.
- Proven non-code/data bytes: **3,755**.
- Reconciled object bytes: **182,172 / 239,698 = 76.0006% (76.00%)**.
- Unresolved object bytes: **57,526**.
- Last integrated instruction ownership fold: `2A6B0..2AC67`.
- Batch-61 compiler separators at `2A89F`, `2AA9B`, `2AAEA..2AAEB`, `2AB36..2AB37`, `2AB95..2AB97`, `2ABDB`, and `2ABF3` are directly proven non-code/data and counted in the residual ledger.
- `2AC68..2AC6F` is directly proven zero padding; `2AC70..2AC7F` is the directly referenced 16-byte table loaded by `2C2A8` and copied as four dwords.
- The prior uncounted `2A6B0/2A8A0` graphics anchor was replaced, not duplicated, by the Batch-61 instruction-accounted reconstruction so the writable-code operands at `2A816`, `2AA0D`, and `2AA12` remain explicit.
- Next independently decoded instruction boundary: **`2AC80`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2AC10..2ACEF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2AC10, 0x2ACF0)` — 224 bytes.
- SHA-256: `94182cf6e72c432b09e29cca6b1943de7322fa8d77c93aa3a6f2c5d743bd7273`.
- Narrow boundary window: `[0x2AC50, 0x2ACB0)` — 96 bytes.
- Narrow SHA-256: `ff5b518b301c3a11b13d416a31f2e7a9e1619023f45d868edfb3bc31bc233371`.
- Narrow boundary bytes:
  `efb009ee6642ec664a8ae080e4bf0ae7b00966ef5b595ac30000000000000000000000000000000000000000000000005356575589e581ec04000000837d14527509c745fc01000000eb37837d14717509c745fc01000000eb28837d146e7509`

Interpretation to re-prove on resume, not merely trust: `2AC10..2AC67` is the final Batch-61 retrace/CRTC helper and returns at `2AC67`; `2AC68..2AC6F` is eight zero-fill bytes and `2AC70..2AC7F` is the directly referenced 16-byte table. `2AC80` begins the next independent predicate with the four-register prologue/local allocation and comparisons against `52h`, `71h`, `6Eh`, and `91h`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 62 bytes are authorized by this checkpoint. Any analysis at or beyond `2AC80` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 61 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2AC80`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
