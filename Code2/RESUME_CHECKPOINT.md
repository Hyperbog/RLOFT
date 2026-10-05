# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `e7d2e8ab3147bc63aa678c43fa3ed295fa2eff9b` — `Integrate CODE.2 decompilation batch 106`.
- Verified translated instruction bytes: **218,072**.
- Proven non-code/data bytes: **4,316**.
- Reconciled object bytes: **222,388 / 239,698 = 92.7784% (92.78%)**.
- Unresolved object bytes: **17,310**.
- Last integrated ownership fold: `342E4..34997`.
- Next independently decoded instruction boundary: **`34998`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x34940,0x34A20)` — 224 bytes; SHA-256 `c9b24dc24f07d74d1d18b82df7fb31bf0289a3d524e1eec22a97b585b299f30c`.
- Narrow `[0x34960,0x349C0)` — 96 bytes; SHA-256 `50a7e9730ab7165ed9a3231431e0fcdc0d1afb33097d1afb66813a626f342564`.
- Narrow bytes: `66bac40366b8020c66ef5a58c1e502c1e302e8d9f9ffff8b3d40ff000066bac40366b8020366ef5a58e8c2f9ffffff3558020100c387db90c800000060bb000000008b1d5cc60000b8680100002b0558c600005083e8207d05056801000050e8`.

Interpretation: `34950` is the shared sampler RET used by both renderer entries; `34951..34953` is unreferenced alignment; `34994` is the second renderer RET; `34995..34997` is unreferenced alignment; `34998` begins an independently called initialization routine. No Batch 107 bytes are counted.
