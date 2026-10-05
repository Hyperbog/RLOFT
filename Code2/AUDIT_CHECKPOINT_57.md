# CODE.2 Semantic Audit — Checkpoint 57

## Scope

Forward semantic inspection continued through staged Batch 142:

- `39C30..39C7A` — 75 translated bytes
- `39C7B..39D4C` — 210 translated bytes
- total newly inspected: 285 bytes

Cumulative semantic inspection is now 236,861 / 239,698 = 98.8164%; 2,837 bytes remain. Next independent boundary: `39D4D`.

## Immutable evidence

Object-1 SHA-256 remains `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

Owned span hashes:

- `39C30..39C7A`: `255f47022b57ed7efae1916e80b54253c908f053cd5c58b95663befefae649ad`
- `39C7B..39D4C`: `1a296ab0d7b3d9664b7faf7e57187092cd41e0a21ee5cc92f8be4e0534d5a526`
- whole `39C30..39D4C`: `db2187d505379cee8680d61667414737ef72986ce6154ff18b710b2e8e511610`

## `39C30` result

No definite mismatch established. The binary loads the first arena node through global `12624`, reads its arena pointer at node+24h, computes `arena + [arena] + 4`, and conditionally refreshes global `12634` from `LSL DS + 1` only when runtime mode `12662` is in the unsigned inclusive range 2..8 and byte `12664` is zero. It returns `[arena]` only when the computed arena end equals the cached/refreshed ceiling; otherwise zero.

The staged `ds_limit()` boundary is semantically acceptable provided it means the encoded `LSL DS` result and not a generic host memory limit.

## `39C7B` result

One new definite fidelity defect is established in the staged reconstruction: the DPMI 0501h allocation boundary has the wrong success/result contract.

Encoded behavior:

- input request is split as `BX = bytes >> 16`, `CX = low16(bytes)`;
- `AX=0501h`, `INT 31h`;
- Carry Flag determines failure;
- on success the linear address is `BX:CX`;
- the DPMI memory-block handle is `SI:DI`, stored as low word `DI` at `[base]` and high word `SI` at `[base+2]`;
- `[base+4]` is zero and `[base+8] = bytes-8`; return is `base+8`.

The staged boundary `u32 (*dpmi_alloc)(u32 bytes,u16* handle_hi,u16* handle_lo)` instead treats the callback return value itself as the base and rejects `base==0`. That is not an exact representation of the encoded CF + register-output ABI and also makes linear base zero indistinguishable from failure.

The DOS fallback is otherwise structurally consistent with the binary: optional `2BD08=000FFFFFh` clamp, unsigned `bytes<=10000h` gate, AH=48h probe allocation using `(2BD08>>4)|1`, second AH=48h request using `bytes>>4`, freeing the probe through AH=49h with ES=probe selector, rejection on signed/error-tagged second result, DPMI AX=0006h selector-base query, selector stored at header-4, size `bytes-8` stored at payload header, and return of linear base+8.

## Repair ledger addition

Add Batch-142 defect: replace the coarse `dpmi_alloc` callback with a boundary that preserves CF and the distinct BX:CX / SI:DI outputs (or an equivalent explicit result record). Do not model failure as `base==0`.

## Source status

`.batch142.fragment.cpp` remains staged explicit source; this checkpoint does not claim that the main `Code2.cpp` has been patched by the semantic-audit repair pass.

## Next

Resume at `39D4D`. The historical Batch-142 integration record identifies it as the next independent boundary.