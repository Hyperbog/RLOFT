# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `4101a8d45e5b7d4ccc017a61caf41c3c15fdf2fa` — `Integrate CODE.2 decompilation batch 104`.
- Verified translated instruction bytes: **216,248**.
- Proven non-code/data bytes: **4,308**.
- Reconciled object bytes: **220,556 / 239,698 = 92.0141% (92.01%)**.
- Unresolved object bytes: **19,142**.
- Last integrated ownership fold: `33FB4..3426F`.
- Next independently decoded instruction boundary: **`34270`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x34220,0x34300)` — 224 bytes; SHA-256 `f29c190cdcaf2e5eda8134e1cd01eb8cc08c3f1b4277ff5b341b7a1ee778c2d7`.
- Narrow `[0x34260,0x342C0)` — 96 bytes; SHA-256 `735060f88f114490f9e4bee5e8b56732cef0feef3a1694b5df2abba7cc492aef`.
- Narrow bytes: `c3504975cf5dc9c300000000000000002d0e010000a90000008079070568010000ebf23d680100007c052d680100008bf82bd23d0e0100007c0fba0300000081ef68010000f7dfeb263db40000007c0dba0200000081efb4000000eb1283f85a`.

Interpretation: `34267` is the final Batch-104 RET; `34268..3426F` is eight unreferenced zero padding bytes; `34270` begins the next independently called angle/table helper. No Batch 105 bytes are counted.
