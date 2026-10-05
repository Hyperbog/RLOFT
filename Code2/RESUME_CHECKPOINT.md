# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `8e829655bf831f9baf4835a6ef3a631f6a85ba36` — `Correct CODE.2 batch 109 resource-base load`.
- Verified translated instruction bytes: **219,172**.
- Proven non-code/data bytes: **4,320**.
- Reconciled object bytes: **223,492 / 239,698 = 93.2390% (93.24%)**.
- Unresolved object bytes: **16,206**.
- Last integrated ownership fold: `34BB0..34DE7`.
- Next independently decoded instruction boundary: **`34DE8`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x34D90,0x34E70)` — 224 bytes; SHA-256 `2b5c55b74f092f6004d88d2d7306013649306b4c1ed8b27a9c8492b4b0372beb`.
- Narrow `[0x34DC0,0x34E20)` — 96 bytes; SHA-256 `9d1b8717c5be48625d41181b24587fdb1f8dc1a5d9706c69482cd6fedc25011c`.
- Narrow bytes: `0002c705b8fd000004000000c705b4fd0000830000008d45d450e88df0ffff83c404c95f5e5bc3905356575589e581ec4c00000068800000006a00b8f0b7020050e82abfffff83c40c68800000006a00b870b8020050e815bfffff83c40c6a20`.

Interpretation: `34DE6` is the Batch-109 RET; `34DE7` is an unreferenced NOP separator; `34DE8` begins the next independently called routine. Batch 109 accounting is unchanged by the resource-base fidelity repair. No Batch 110 bytes are counted.
