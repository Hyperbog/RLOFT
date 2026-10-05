# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `0b972d900c1d2eb8057a370debcfc095033bb776` — `Integrate CODE.2 decompilation batch 91`.
- Verified translated instruction bytes: **213,560**.
- Proven non-code/data bytes: **4,268**.
- Reconciled object bytes: **217,828 / 239,698 = 90.8760% (90.88%)**.
- Unresolved object bytes: **21,870**.
- Last integrated ownership fold: `335DC..337C7`.
- Next independently decoded instruction boundary: **`337C8`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x33740,0x33820)` — 224 bytes; SHA-256 `09a358a30621dfe697e9df5ff613141166ebaaa79aeca8c9e9352fc38e1fc034`.
- Narrow `[0x33780,0x337E0)` — 96 bytes; SHA-256 `4d5821ed783d5624d0b7131001df3d135c8d1e0dac62b006dbdd98d178513a00`.
- Narrow bytes: `8b4608a300c700006a04ff3500c70000e807bbffff83c40883c002a3e45502008b4610d1e00105e4550200eb06ff05f4c600004783c61c3b3dd8c600007595e878eeffff61c9c390c80000005351578b7d08b9000000008b550c0fbe1d215602`.

Interpretation: `337C6` is the final RET; `337C7` is an unreferenced NOP alignment byte; `337C8` begins the next independent routine. No Batch 92 bytes are counted.
