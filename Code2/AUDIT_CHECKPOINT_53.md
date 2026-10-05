# CODE.2 Semantic Audit — Checkpoint 53

## Scope
- Immutable CODE.2 object-1 range: `39589..3968D` (Batch 138)
- 261 bytes inspected.
- Object-1 SHA-256 remains `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Range SHA-256: `753921c7dc798149f0f647667ee771115032702827972404a1749bf91d7735b7`.
- Overlap `3967C..3969F` SHA-256: `59ac0b33bb584552ab938a1fdab6cffc8ed3d6dd29ae5e800573e2a3bf6bb864`.

## Result
Batch 138 ownership and function boundaries are correct: `39589..395E1`, `395E2..3962C`, `3962D..3967B`, `3967C..3968D`.

One semantic defect is established in `function_39589`.

### `39589` — failed-allocation/no-scratch branch publishes wrong watermark
Encoded flow:
- load `1264C`, add 3, clear low two bits;
- compare aligned amount with `3A165()`;
- only if aligned amount is strictly below available stack: subtract it from ESP and set EAX=the resulting ESP;
- otherwise execute `XOR EAX,EAX`;
- then reload raw `1264C`, add it to EAX, and publish `12650`.

Therefore the no-scratch branch publishes exactly `raw`, not `original_esp + raw`.

Current reconstruction computes:
`store_le32(dg(m,0x12650u),m.cpu.esp-scratch+raw);`
with `scratch=0` on the failed/no-scratch branch, yielding `original_esp+raw`. This is not encoded behavior.

Required repair is branch-sensitive base selection, e.g. conceptually:
`const u32 base=(aligned<available)?(m.cpu.esp-aligned):0u;`
`store_le32(...,base+raw);`
while preserving the strict unsigned `aligned < available` condition.

The two six-byte-record callback dispatchers compare cleanly at the audited semantic level, including tie replacement, callback-null handling, state=2 publication, ES<-DS before indirect callback, and the `DH=old DL; DL=old AL` callback argument construction in `3962D`.

`3967C` correctly forwards `seek(handle,0,1)`.

## Coverage
Previous inspected total: 234,873 / 239,698.
This checkpoint: +261.
Cumulative semantic inspection: **235,134 / 239,698 = 98.0959%**.
Remaining uninspected: **4,564 bytes = 1.9041%**.

Next source-explicit boundary: `3968E`.