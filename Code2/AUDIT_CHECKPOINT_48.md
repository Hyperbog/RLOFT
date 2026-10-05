# CODE.2 semantic audit checkpoint 48

## Scope
- Immutable object-1 range inspected: `[0x384AA,0x385B0)` / `384AA..385AF`.
- Bytes inspected this checkpoint: **262**.
- Reconstruction ownership: Batch 133, 262 translated instruction bytes.
- Cumulative semantic inspection: **229,270 / 239,698 = 95.6495%**.
- Remaining uninspected: **10,428 bytes = 4.3505%**.
- Next source-explicit boundary: **`0x385B0`**.

## Evidence and result
Direct immutable object-1 inspection was compared with the exact integrated Batch-133 reconstruction. No new semantic mismatch was established in this ownership unit.

Preserved invariants include:
- Current-directory request uses DOS AH=47h semantics with drive selector zero/current drive and writes the returned path after a three-byte prefix reservation.
- Default drive is zero-based and converted to uppercase ASCII drive letter with exact `X:\\` prefix construction.
- Current-directory failure maps to runtime error 1.
- Null destination requests allocation of exactly `strlen(path)+1`; allocation failure maps to runtime error 5.
- Caller-supplied destination/capacity is forwarded without replacing the encoded copy boundary.
- `3854E` remains an address-distinct wrapper around the ASCII-only `3855C` uppercase helper; only `a..z` are folded by subtracting `20h`.
- `3856E` remains an address-distinct all-mask wrapper around `38579`.
- Stream traversal follows the linked-list next pointer, tests `(flags & mask) != 0`, increments the matched count before the flush gate, and calls the existing stream flush helper only when state byte bit `10h` is set.

## Open repair set
Previously established repairs remain open:
1. `36CEC..372F5` — 1,546 accounting-owned bytes lack explicit semantic reconstruction.
2. `372F6` — PIT/speaker gate predicate mismatch.
3. `3736D` / `37433` — mutable stack-slot radix aliasing normalized away.
4. `37810` — planar permutation address calculation uses displaced table value rather than encoded matched table position.
5. `37D60` — encoded zero-unread shared-tail register/control-flow dependency normalized away.
6. `37EFA` — special open preprocessing uses the opposite `3A0A8` result predicate from the binary.

## Defect accounting
- New semantic defects this checkpoint: **0**.
- Existing open repair items: **6**.

## Resume
Continue from **`0x385B0`**. This boundary was historically unresolved during Batch 134 and was later folded explicitly by Batch 143, so semantic inspection must use the final explicit ownership/source rather than granting credit from the earlier aggregate accounting. After closing that fold, continue into the Batch-134 formatted-output parser at `0x38867`.