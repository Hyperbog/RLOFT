# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `a528fbd28850e49f1fdeadb3a0433740e8dd2714` — `Integrate CODE.2 decompilation batch 126`.
- Verified translated instruction bytes: **225,869**.
- Proven non-code/data bytes: **4,650**.
- Reconciled object bytes: **230,519 / 239,698 = 96.1706% (96.17%)**.
- Unresolved object bytes: **9,179**.
- Last integrated ownership fold: `375FD..3780F`.
- Next independently decoded instruction boundary: **`37810`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x377D0,0x37890)` — 192 bytes; SHA-256 `9fbb7c74d0f7954ea0ddb57b084dfcb686b94a94c29a1cae3f9d2feed316bcec`.
- Window `[0x377F0,0x37870)` — 128 bytes; SHA-256 `c6c6e2e90c74f8e0a5555c40cd08312a277e7254ebf630c98981e77b200a907a`.
- Batch-owned `375FD..3780F` must not be recounted; overlap bytes are verification only.

## Batch 126 interpretation
- `375FD`: one proven zero separator byte.
- `375FE..37641`: critical-error interrupt bridge installer with extender-mode split.
- `37642..376E8`: full interrupt bridge; preserves the private-stack switch, BP:SI linearization, saved far callback, LSS restoration, and IRET behavior.
- `376E9..376EF`: seven proven zero separator bytes.
- `376F0`: address-taken one-byte RET stub.
- `376F1..376FF`: fifteen proven zero separator bytes.
- `37700..3780C`: 4 KiB ring-dictionary decoder. Only the first `0xFEE` dictionary bytes are initialized to `FEh`, exactly matching the encoded `REP STOSB`; ring positions use 16-bit arithmetic and `0FFFh` masking.
- `3780D..3780F`: `XCHG EBX,EBX; NOP` alignment.
- `37810` begins the next decoded routine.
