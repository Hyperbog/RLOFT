# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `d230f1bf72fd15c7827178d8a3589fcda12e1210` — `Integrate CODE.2 decompilation batch 99`.
- Verified translated instruction bytes: **214,442**.
- Proven non-code/data bytes: **4,278**.
- Reconciled object bytes: **218,720 / 239,698 = 91.2482% (91.25%)**.
- Unresolved object bytes: **20,978**.
- Last integrated ownership fold: `33AA0..33B43`.
- Next independently decoded instruction boundary: **`33B44`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33B00,0x33BE0)` — 224 bytes; SHA-256 `3afe4bd94c04b02ab3e198971b13aad9e92022d9d46913810960f9c9ddd10ecd`.
- Narrow `[0x33B40,0x33BA0)` — 96 bytes; SHA-256 `6bd8dd43d4a8d4ec53d7f92c2a88b8a7841325942ec7596c67e8f21c33cd259a`.
- Narrow bytes: `c0c38bc0535589e583ec08c745f8000000008b45f88945fcb800000000cd338985fcffffff899df8ffffff8b45fc66a3a2b702008b45f866c705a6b70200000066a3a4b7020066833da2b70200000f849a0000006818fd00006814fd00006a40`.

Interpretation: `33B41` is the second routine's final RET; `33B42..33B43` is unreferenced `MOV EAX,EAX` alignment; `33B44` begins the next independently referenced mouse/runtime initialization routine. No Batch 100 bytes are counted.
