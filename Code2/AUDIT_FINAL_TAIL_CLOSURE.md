# CODE.2 final-tail semantic closure

## Scope

This checkpoint closes the five remaining ownership folds after the staged `3A1B3..3A2AC` reconstruction:

- `3A2AD..3A394` — 232 bytes
- `3A395..3A528` — 404 bytes
- `3A529..3A65B` — 307 bytes
- `3A65C..3A7DE` — 387 bytes
- `3A7DF..3A851` — 115 bytes

Total: 1,445 bytes.

Together with the previously staged `39FED..3A1B2` and `3A1B3..3A2AC` folds and the corrected 16-byte object prefix classification, the semantic-inspection ledger reaches the physical end of object-1.

## Fold semantics

### 3A2AD..3A394
Runtime process/environment setup. This fold builds/publishes environment and argument state and preserves the original packed runtime representations rather than replacing them with host-process abstractions. Allocation failure and empty-input paths remain distinct.

### 3A395..3A528
Memory-ceiling / selector / DPMI support. Encoded unsigned arithmetic, selector state, and interrupt-result status are retained as machine-visible state. Carry/error results are not collapsed into ordinary pointer-return conventions.

### 3A529..3A65B
Runtime termination/cleanup family. Address-distinct wrappers remain distinct even where they converge on shared cleanup. Exit dispatch and cleanup ordering are preserved; fatal/termination boundaries are not rewritten as normal returning helpers.

### 3A65C..3A7DE
Stream allocation and teardown support, including temporary-name construction. The temporary-name generator preserves the runtime `tXXXX_YY.tmp` form and the stream close/free paths retain descriptor flags, buffer ownership, temporary-file handling, and list/state mutation ordering.

### 3A7DF..3A851
Final stream/seek wrappers and terminal object bytes. `3A7F8..3A7FD` is a six-byte embedded ASCII data island (`WVIDEO`) jumped over by executable control flow and is therefore classified as data, not translated instructions. The final address-distinct wrapper at `3A844` remains separate.

## Fidelity requirements confirmed across the final tail

- DOS/DPMI carry state remains explicit where the binary branches/folds CF.
- Selector and segment state is not converted into flat-host assumptions.
- Address-distinct wrappers/thunks remain independently represented.
- Odd fall-through and fatal-boundary behavior is retained rather than normalized.
- Runtime stream records preserve the encoded `0x1A` stride and flag tests.
- Embedded `WVIDEO` bytes remain data.

## Accounting closure

Prior source-explicit semantic coverage after `3A1B3..3A2AC`: 238,253 / 239,698.

This checkpoint closes the remaining 1,445 bytes:

**239,698 / 239,698 = 100.0000% semantic inspection coverage.**

There are now zero semantically uninspected object-1 bytes.

This statement means the immutable object has been completely semantically inspected/classified. It does **not** mean every defect found by the audit has already been integrated into `Code2.cpp` or compile-verified.

## Remaining gate before final semantic certification

The accumulated defect ledger still must be consolidated into the actual reconstructed source and verified. Known repair classes include, at minimum:

1. `372F6` gate polarity.
2. `3736D` mutable stack-slot/radix aliasing.
3. `37433` independent remainder-slot aliasing.
4. `37810` matched-position planar source.
5. `37D60` zero-unread shared compare tail.
6. `37EFA` first descriptor-device predicate polarity.
7. `38A61` signed residual width/backward-copy behavior.
8. `38AC3` format-state pointer ABI and default precision mutation.
9. `38AC3` normalized-magnitude rounding test.
10. Batch-136 formatter ABI rewrite: integer width flags, signed word extension, conversion-specific cdecl consumption, far strings, near/far pointers, and final normalization.
11. `3908C` incoming-CF dependency.
12. `39589` rejected scratch-stack base behavior.
13. `39C7B` DPMI 0501h carry/register-result contract.
14. Review the staged `39A8A` DPMI 0503h boundary under the same carry/register-output rule.

Only after those repairs are integrated into `Code2.cpp`, built, and verification-gated should the source itself be called semantically certified.
