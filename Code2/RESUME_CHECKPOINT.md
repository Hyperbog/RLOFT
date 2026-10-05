# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `259ed2def276bddb7b242d8db47a6836340d4eca` — `Integrate CODE.2 decompilation batch 110`.
- Verified translated instruction bytes: **220,354**.
- Proven non-code/data bytes: **4,320**.
- Reconciled object bytes: **224,674 / 239,698 = 93.7321% (93.73%)**.
- Unresolved object bytes: **15,024**.
- Last integrated ownership fold: `34DE8..35285`.
- Next independently decoded instruction boundary: **`35286`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x35230,0x35310)` — 224 bytes; SHA-256 `c2c54180610da71375b7543f0fc1a35fb0f236137fb241fd35df979a667694c5`.
- Narrow `[0x35260,0x352C0)` — 96 bytes; SHA-256 `bf7c2f7c947d09e661befe04e0986e358ef3d97ec5e2b16a2c5cab22a8a64e56`.
- Narrow bytes: `0050e86dbaffff83c40c6a206a00b8d0b7020050e8b7baffff83c40ce95bfcffffc95f5e5bc3528b4424088b54240c57515389d789c2b436cd21663dffff741366891766895f026689470466894f0629c0eb05e8fa2900005b595f5ac3c3ff15`.

Interpretation: `35285` is the Batch-110 RET; `35286` begins a distinct DOS free-space wrapper with an independent caller. No Batch 111 bytes are counted.
