# CODE.2 semantic audit checkpoint 39

Checkpoint 39 resumes after checkpoint 38. It first resolves the status of the historical `36CEC..372F5` accounting island, then audits the next source-explicit ownership island, Batch 123 (`372F6..3736C`).

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Checkpoint-38 mandatory overlap `[36CAC,36D6C)`: `46ad7bec0ade95f6e224d7bd3cfac826c4380ea42a3f8238824bfd4cc451577e` (reproduced exactly).

## `36CEC..372F5` source-representation finding
The final Batch-143 accounting reconciliation superseded the earlier *accounting-unresolved* label for `36CEC..372F5`, but it did not add an explicit `function_36cec` reconstruction. Current `Code2.cpp` exposes `36CEC` only as an external boundary (`call_36cec`) used by earlier application code. Therefore these 1,546 bytes are byte-accounted by the historical aggregate baseline but are **not yet eligible for semantic-audit credit**. Accounting closure and semantic source representation are distinct requirements.

This is recorded as a new audit defect class: **aggregate-baseline ownership without explicit semantic source representation**. The semantic-audit frontier therefore retains an open gap at `36CEC..372F5`; auditing may continue over later independently source-explicit islands without falsely crediting this gap.

Immutable gap SHA-256 `[36CEC,372F6)`: `f0924e15e3109a1fe0b00a0cdc1e8748af213eaadc26048b32800dfbdc7dd80b`.

## Batch 123 audit — `372F6..3736C`
Batch-owned `[372F6,3736D)` is 119 bytes and reproduces SHA-256 `6bd1174aca75fe65210f87fb0ebe484e9cb6060f82f597e6251a6bf66403ad67`.

### `372F6..37338` PIT channel-2 helper
Direct immutable disassembly confirms:
- unsigned divisor threshold `<= 12h` returns without programming the PIT;
- unsigned `1234DCh / divisor`;
- port `61h` is sampled with `TEST AL,03h`;
- **the gate/control setup path executes only when `(AL & 3) == 0`** because encoded `JNE 37329` skips the OR/control write whenever either low bit is already set;
- counter low byte then high byte are emitted to port `42h`.

The committed Batch-123 reconstruction currently models this test as `if ((gate & 3u) != 3u)`, which is not equivalent when exactly one of bits 0/1 is set. This is a concrete semantic mismatch and requires repair to an encoded-zero test (`if ((gate & 3u) == 0u)`).

### `37339..37347` speaker disable helper
The reconstruction matches the binary: read port `61h`, clear low two bits with `AND ... FCh`, write the result back.

### `37348..3736C` radix alphabet
The 37 bytes are correctly classified as data: `0123456789abcdefghijklmnopqrstuvwxyz\0`, not linear code.

## Accounting
- Previously semantically audited: **224,476 bytes**.
- Newly inspected source-explicit ownership: **119 bytes**.
- Cumulative semantic inspection: **224,595 / 239,698 = 93.6992%**.
- Not yet semantically inspected: **15,103 bytes = 6.3008%**.
- Semantic defect classes: **7**.
- New defect classes this checkpoint: **1** (aggregate-baseline ownership without explicit semantic source representation).
- New concrete source mismatch: Batch-123 port-61 gate test (`!=3` vs encoded `==0`).

Because the Batch-123 mismatch remains unrepaired at this checkpoint, this document records inspection rather than falsely declaring that routine semantically closed. The next source-explicit independent boundary is `3736D`; the `36CEC..372F5` semantic gap remains open and must be reconstructed/audited before final 100% semantic certification.

## Next work
1. Repair Batch-123 `function_372f6` gate condition to match `TEST AL,3 / JNE` exactly.
2. Continue source-explicit audit from `3736D` while retaining the `36CEC..372F5` gap as open.
3. Reconstruct `36CEC..372F5` explicitly from immutable CODE.2 evidence before final semantic closure.
