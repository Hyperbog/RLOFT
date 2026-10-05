# CODE.2 semantic audit checkpoint 44

## Scope
- Immutable object-1 range inspected: `0x37AFC..0x37CA2` (end-exclusive `[0x37AFC,0x37CA3)`).
- Bytes inspected this checkpoint: **423**.
- Reconstruction ownership: Batch 129, 423 translated instruction bytes and no newly classified non-code bytes.
- Cumulative semantic inspection: **226,953 / 239,698 = 94.6829%**.
- Remaining uninspected: **12,745 bytes = 5.3171%**.
- Next source-explicit boundary: **`0x37CA3`**.

## Evidence and result
Direct immutable object-1 inspection was compared against the committed Batch-129 reconstruction and integration evidence. No new semantic mismatch was established in this ownership unit.

Preserved invariants include:
- `37AFC..37B33`: arena-chain largest-free query with the encoded payload `-4` adjustment.
- `37B34..37B41`: address-distinct allocation forwarding wrapper.
- `37B42..37C36`: unsigned rejection of zero and sizes above `0xFFFFFFD4`, four-byte upward rounding, 12-byte minimum request, arena search, and two distinct heap-growth/retry paths.
- `37C37..37C44`: address-distinct free forwarding wrapper.
- `37C45..37CA2`: owning-arena release path and associated runtime-state maintenance remain semantically represented through the reconstruction boundary.

## Open repair set
The previously established repair items remain open and are not silently credited as corrected:
1. `36CEC..372F5` — 1,546 accounting-owned bytes lack explicit semantic reconstruction.
2. `372F6` — PIT/speaker gate predicate mismatch.
3. `3736D` / `37433` — mutable stack-slot radix aliasing normalized away.
4. `37810` — planar permutation address calculation uses displaced table value rather than the encoded matched table position.

## Defect accounting
- New semantic defects this checkpoint: **0**.
- Existing open repair items: **4**.

## Resume
Continue semantic audit at **`0x37CA3`** (Batch 130 ownership). Preserve exact errno domains, stream flags, encoded capacities, fallback behavior, partial-write handling, unread-byte seek correction, and DOS AH=43 attribute semantics.