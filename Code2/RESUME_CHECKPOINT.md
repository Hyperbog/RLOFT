# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `7bd336d65a38f3638339b8bee762875a60af730f` — `Integrate CODE.2 decompilation batch 80`.
- Verified translated instruction bytes: **208,723**.
- Proven non-code/data bytes: **4,245**.
- Reconciled object bytes: **212,968 / 239,698 = 88.8485% (88.85%)**.
- Unresolved object bytes: **26,730**.
- Last integrated ownership fold: `32220..324CB`.
- Next independently decoded instruction boundary: **`324CC`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x32430,0x32510)` — 224 bytes; SHA-256 `7530131e28e9cb95fa673d8171e301322952ce5a5a5a74d3948ad92888250b8b`.
- Narrow `[0x32480,0x324E0)` — 96 bytes; SHA-256 `3325612dd7cf60f5ae143edfa71f0d21138485d63b7c72ce0d3f6e3d5b18b872`.
- Narrow bytes: `000029dac704240000000001d0837c2420007e2f8a4c24288b54242c8d7a030fb61a0fb63029de0faff5d3fe4001f3428858ff39fa75e8ff04248b14243b5424207cd583c4045d5f5e5bc39068000100006a006a006a006a0068d0f90000e821`.

Interpretation: `324CA` is the final RET; `324CB` is an unreferenced NOP; `324CC` begins the next wrapper. No Batch 81 bytes are counted.
