# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `0fc15d8b4880efb8d940a78820649c25512abe1c` — `Integrate CODE.2 decompilation batch 107`.
- Verified translated instruction bytes: **218,292**.
- Proven non-code/data bytes: **4,316**.
- Reconciled object bytes: **222,608 / 239,698 = 92.8702% (92.87%)**.
- Unresolved object bytes: **17,090**.
- Last integrated ownership fold: `34998..34A73`.
- Next independently decoded instruction boundary: **`34A74`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x34A20,0x34B00)` — 224 bytes; SHA-256 `11bc613563d5f3e642ae6e0ae3a5f671ad9d2c5014a3d13b5240b2b433716567`.
- Narrow `[0x34A40,0x34AA0)` — 96 bytes; SHA-256 `f5f08ce261aa8b2b801d83d63444ddcdc226dc1b8bd91dbb459460982cf7d8e9`.
- Narrow bytes: `8b966096010066f7ea660fa4c2036689570266b8df3c668b966096010066f7ea660fa4c20366891783c60283c70649759e61c9c3c800000060a044c00000a2f94a0300a2694b0300a048c00000a2f64a0300a2664b0300a148ff00000305bcfd`.

Interpretation: `34A73` is the Batch-107 RET; `34A74` begins the next independently called renderer-driving routine. No Batch 108 bytes are counted.
