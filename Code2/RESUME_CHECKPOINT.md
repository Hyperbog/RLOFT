# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `36a455faf4a0a27c97d227f6e492b77e60c09e13` — `Integrate CODE.2 decompilation batch 119`.
- Verified translated instruction bytes: **223,874**.
- Proven non-code/data bytes: **4,502**.
- Reconciled object bytes: **228,376 / 239,698 = 95.2766% (95.28%)**.
- Unresolved object bytes: **11,322**.
- Last integrated ownership fold: `39526..3967B`.
- Next work: select another independently proven range from the unresolved complement; `3967C` and later runtime helpers already have ownership.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x394E0,0x395B0)` — 208 bytes; SHA-256 `c9ac1da39ecdfe4cbde0e42bf05cc75bf1c3330e5a996e204e31beaadfd6bfce`.
- Narrow `[0x39500,0x39590)` — 144 bytes; SHA-256 `cc29d7509bd62c692e4d2562e4f80f11f31a3c577eb50f02b5c5372ade3cfa3d`.
- The overlap bytes are verification only and must not be recounted.

## Batch 119 interpretation
- `39526..39588`: ASCII-only length-bounded case-insensitive comparator used by `352EE`.
- `39589..395E1`: dynamic-stack/runtime initialization helper; strict unsigned threshold is preserved.
- `395E2..3962C`: four-record packed callback-table drain, selecting the lowest eligible priority and restarting after status writeback.
- `3962D..3967B`: one-record callback-table drain with lower/upper priority gating and preserved full EDX callback payload.
- Batch 118's 30-byte non-code proof was already in the ledger/docs; Batch 119 synchronizes the previously stale `Code2.cpp` constant to 4,502.
