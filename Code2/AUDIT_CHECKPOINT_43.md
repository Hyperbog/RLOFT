# CODE.2 semantic audit checkpoint 43

## Immutable source
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Ownership audited
- Batch 128: `[0x379D0,0x37AFC)` = 300 bytes total.
- Reconstruction accounting: 293 translated + 7 proven alignment/separator bytes.
- Span SHA-256 `[0x379D0,0x37AFC)`: `52279b03746156084a0aef60ff958886a4569390949951ad7fc550acdd42c8c7`.
- Next overlap `[0x37990,0x37B3C)`: `8f927ba56264c7c59111202454938024dfcd3087b8605704674cfc7e8c67b6f6`.

## Semantic findings
Batch 128 was checked against immutable object bytes and its exact integration source (`31c4552bbd697b1c83c1481362ba6e81e7092a19`). No new semantic defect class was established.

Preserved invariants include:
- `379D0` byte-count fill aligns the destination bytewise while rotating the 32-bit pattern by one byte per leading store, then uses the aligned dword fill body and emits the 0..3-byte tail in little-endian pattern order.
- `37A07` is the independently addressable aligned dword-count fill helper; it must remain distinct from `379D0` despite shared fill semantics.
- `37A73` snapshots segment selectors in encoded order ES, CS, SS, DS, FS, GS.
- `37A9E`, `37AC2`, and `37AD4` retain distinct DOS AH=48h/49h/4Ah allocation/free/resize semantics and failure-result handling.
- `37A01..37A06` are six NOP alignment bytes and `37A9D` is the one-byte separator before the allocation wrapper.

## Coverage
- Previous cumulative inspection: 226,230 / 239,698.
- Newly inspected: 300 bytes.
- Cumulative inspection: **226,530 / 239,698 = 94.5064%**.
- Remaining uninspected: **13,168 bytes = 5.4936%**.
- Next semantic frontier: **0x37AFC**.

## Open reconstruction repairs retained
1. `36CEC..372F5`: 1,546-byte accounting-owned region without explicit semantic reconstruction.
2. `372F6`: PIT gate predicate mismatch.
3. `3736D` / `37433`: mutable stack-slot radix aliasing normalized away.
4. `37810`: permutation payload address uses displaced table value rather than encoded matched table position.

No repair was silently applied in this checkpoint.