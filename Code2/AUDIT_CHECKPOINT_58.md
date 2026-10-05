# CODE.2 Semantic Audit — Checkpoint 58

## Scope

The forward audit reached the historical post-Batch-142 unresolved region. Direct immutable disassembly establishes three complete routines before the next entry at `39FED`:

- `39D4D..39E1B` — 207 bytes
- `39E1C..39F76` — 347 bytes
- `39F77..39FEC` — 118 bytes
- total direct-inspected interval: 672 bytes

Historical Batch 142 explicitly ended at `39D4C` and reported 696 aggregate bytes unresolved, with `39D4D` as the next independent boundary. No historical explicit C++ source for `function_39d4d`, `function_39e1c`, or `function_39f77` was found on the branch. Therefore these 672 bytes are direct-inspected but are NOT yet added to the source-explicit semantic coverage numerator. Coverage remains 236,861 / 239,698 = 98.8164% until explicit reconstruction is staged.

## Immutable hashes

- `39D4D..39E1B` SHA-256 `a490970960202ba10940fe447d5b708ac6a7641b7f9999788fa7c5fcc73e14cf`
- `39E1C..39F76` SHA-256 `6cec35af3f6895454b5f8d278b1cab68bb1c97bef3dbb2403bb83d459166184b`
- `39F77..39FEC` SHA-256 `8aeba8dc47b170ef592ebdfc454f78e967069c17575d4bd97ce5509f847ba8eb`
- combined `39D4D..39FEC` SHA-256 `c186d3d1ff088dc6ce9201a0d1df08bd3bf17474a9caaed64fa39fc8e4c40c48`

## `39D4D` — backing-arena acquisition/publication

This routine is gated by global `12708 != 0` and cached ceiling `12634 != FFFFFFFE`. It normalizes the caller's requested size in-place through `39F77`; normalization failure returns zero.

For mode byte `12662 == 1`, it passes the normalized size to `39C7B`. A successful backing allocation returns a header pointer; the routine replaces the caller size slot with the backing header's stored size.

For other modes it executes DOS `INT 21h` with `EAX=80004800h`, folds CF into a zero/nonzero returned pointer, and reduces the normalized size by four bytes. It rejects null backing, subtraction wrap, and arena sizes below `38h`.

Successful backing is initialized through `39BBC`. The initial arena block is marked allocated, the owner allocation count is incremented, owner `+14h` is set to `FFFFFFFFh`, and the block payload is immediately passed to `37C45`, thereby converting the newly built arena's sole block through the ordinary free/coalescing machinery. Return is one on success, zero on the ordinary rejection paths. A `39F77` zero result propagates directly.

## `39E1C` — alternate arena-extension path

Modes `(12662==1 && 12663==0)` and `12662==9` tail into `39D4D` after pushing the request pointer.

Other modes repeat the runtime-enabled/ceiling guards and normalize the request through `39F77`. In extender modes 2..8 with `12664==0`, the cached DS ceiling is refreshed with `LSL DS + 1`.

The routine forms `request + ceiling`, saturating unsigned overflow to `FFFFFFFEh`, then calls `3A467` to obtain a boundary/base candidate. It rejects `FFFFFFFFh`, values above `FFFFFFF8h`, and candidates not strictly below the requested upper bound. The residual extent is reduced by four bytes with explicit wrap rejection.

It then searches the address-sorted arena list for an existing arena whose physical end is exactly `candidate-4`. If found, it extends that arena in place, moves the terminal `FFFFFFFFh` marker, and reuses the resulting tail block. Otherwise it requires at least `38h`, constructs a new arena at the candidate through `39BBC`, and obtains its initial block. In either case the selected block is marked allocated, the arena owner count is incremented, owner `+14h` becomes `FFFFFFFFh`, and `37C45(payload)` publishes the new free space. Success returns one.

## `39F77` — request normalization

This helper mutates a caller-supplied dword size and returns boolean success.

Encoded order is material:

1. Round `(request+3)` down to a four-byte boundary by masking only `BL` with `FCh`; reject zero.
2. For `(mode==1 && 12663==0)` or mode 9, add eight bytes. Otherwise subtract the `39C30` arena-ceiling probe result.
3. Store that intermediate value.
4. Add `3Ch` with unsigned-overflow rejection.
5. Clamp upward to global minimum `1270C` when below it, then clear bit zero (`BL &= FEh`).
6. Store the pre-page-round value.
7. Add `0FFFh` with unsigned-overflow rejection.
8. Page-align by clearing the low 12 bits through `BH &= F0h; BL=0`.
9. Store the result and return whether it is nonzero.

The partial-register masks are intentional and should remain explicit in reconstruction; replacing them with unchecked high-level arithmetic can lose the encoded modulo-32-bit overflow behavior.

## Source-explicitness result

This checkpoint does not silently credit the 672 bytes. The binary behavior is now bounded and documented, but an explicit C++ fragment plus deterministic integration/verification step is still required before semantic coverage advances past `39D4D`.

## Next

Stage explicit reconstruction for `39D4D..39FEC`, then resume at `39FED`. The next bytes begin with small runtime stubs (`39FED`, `39FF0`, `39FF6`) before the subsequent descriptor/I/O helpers.