# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `a7ad968b1e76fe196097587683af8951cd6733c2` — `Integrate CODE.2 decompilation batch 129`.
- Verified translated instruction bytes: **227,018**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **231,690 / 239,698 = 96.6591% (96.66%)**.
- Unresolved object bytes: **8,008**.
- Last integrated ownership fold: `37AFC..37CA2`.
- Next independently decoded instruction boundary: **`37CA3`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x37C63,0x37D23)` — 192 bytes; SHA-256 `baf2274023170be44827c06d56f0d5646e272b101d85b0947e39232a7d52f83e`.
- Window `[0x37C83,0x37D03)` — 128 bytes; SHA-256 `219d1d66e8ae053ff7f68adbf19ebd3ef327eeacba08649015c03846d47aea96`.
- Batch-owned `37AFC..37CA2` SHA-256: `e08ee062413ce65969ee2231aea876d732a63bb9e27017af2501c86a79aa4ae8`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 129 interpretation
- `37AFC..37B33`: scan runtime arena chain rooted at `12624h`, reporting the largest free-block payload after the encoded `-4` adjustment.
- `37B34..37B41`: address-distinct allocation forwarding wrapper.
- `37B42..37C36`: core allocator; rejects zero and unsigned sizes above `FFFFFFD4h`, rounds upward to four bytes, enforces a 12-byte minimum, scans arenas, and retries through two distinct heap-growth paths.
- `37C37..37C44`: address-distinct free forwarding wrapper.
- `37C45..37CA2`: owning-arena lookup/free path; updates cached largest-free state and clears the heap-error byte at `2BCF5h`.
- `37CA3` begins the next independently decoded routine.
