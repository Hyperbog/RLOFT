# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `13b396472bb40830194d703a2f835c4246568751` — `Integrate CODE.2 decompilation batch 127`.
- Verified translated instruction bytes: **226,302**.
- Proven non-code/data bytes: **4,665**.
- Reconciled object bytes: **230,967 / 239,698 = 96.3575% (96.36%)**.
- Unresolved object bytes: **8,731**.
- Last integrated ownership fold: `37810..379CF`.
- Next independently decoded instruction boundary: **`379D0`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x37990,0x37A50)` — 192 bytes; SHA-256 `34b82f3ef6116d25fd40e46f492b7875895935b1d7bbde380ba64034d050a1ab`.
- Window `[0x379B0,0x37A30)` — 128 bytes; SHA-256 `7b0b7268ec2b5e917a7833d48038786206b2e4cbbe2aeb95123169294003070d`.
- Batch-owned `37810..379CF` must not be recounted; overlap bytes are verification only.

## Batch 127 interpretation
- `37810..37957`: type-1 planar payload permutation/conversion helper using the scratch permutation table at `10610h`; preserves the optional header-byte skip, four-way index mapping, in-place strip swaps, and header collapse.
- `37958..3798C`: DOS set-interrupt-vector wrapper; extender modes 2..8 select the encoded extended service.
- `3798D..379C0`: DOS get-interrupt-vector wrapper with the same extender-mode split.
- `379C1..379CF`: fifteen proven zero separator bytes.
- `379D0` begins the next independently decoded routine.
