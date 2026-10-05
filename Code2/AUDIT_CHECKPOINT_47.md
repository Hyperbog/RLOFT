# CODE.2 semantic audit checkpoint 47

## Scope
- Immutable object-1 range inspected: `[0x38106,0x384AA)` / `38106..384A9`.
- Bytes inspected this checkpoint: **932**.
- Reconstruction ownership: Batch 132, 932 translated instruction bytes.
- Cumulative semantic inspection: **229,008 / 239,698 = 95.5406%**.
- Remaining uninspected: **10,690 bytes = 4.4594%**.
- Next source-explicit boundary: **`0x384AA`**.

## Evidence and result
The immutable object-1 range was checked against the exact Batch-132 integrated reconstruction. No additional semantic mismatch was established in this ownership unit.

Preserved invariants include:
- Descriptor read validation using bits `01h` and runtime errors 4/6.
- Binary/direct read path under descriptor bit `40h`.
- Text read CR stripping and Ctrl-Z (`1Ah`) EOF handling with encoded backward seek correction.
- Descriptor bit `20h` single-read behavior.
- Write validation using bit `02h`, append/end seek under bit `80h`, and direct write under bit `40h`.
- Text output LF -> CRLF expansion.
- Encoded short-write runtime error 12h behavior.
- Runtime-version-dependent staging chunk selection (`200h` versus `80h`).
- Address-distinct raw/translated seek wrappers and DOS delete error translation.
- Current/default-drive helpers preserve one-based public drive numbering while DOS-facing selection remains zero-based.

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
Continue at **`0x384AA`** (Batch 133 ownership). Audit the current-directory builder, uppercase pair, and masked stream-flush behavior without normalizing partial-register, path-prefix, or flag-mask semantics.