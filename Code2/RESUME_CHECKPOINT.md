# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `31c4552bbd697b1c83c1481362ba6e81e7092a19` — `Integrate CODE.2 decompilation batch 128`.
- Verified translated instruction bytes: **226,595**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **231,267 / 239,698 = 96.4827% (96.48%)**.
- Unresolved object bytes: **8,431**.
- Last integrated ownership fold: `379D0..37AFB`.
- Next independently decoded instruction boundary: **`37AFC`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Window `[0x37ABC,0x37B7C)` — 192 bytes; SHA-256 `503d545876e835042c4f9c55e2c5e9de75a23a9e3e03966c4bb97770b1c9079a`.
- Window `[0x37ADC,0x37B5C)` — 128 bytes; SHA-256 `74b58cc295e9d79f686a0568e7a0a42c4c5a27739faf6d38e364b46ba5fd78fb`.
- Batch-owned `379D0..37AFB` must not be recounted; overlap bytes are verification only.

## Batch 128 interpretation
- `379D0..37A00`: optimized byte-count fill entry; aligns destination bytewise while rotating EDX, delegates dword groups, then writes the 0..3-byte tail.
- `37A01..37A06`: six NOP alignment bytes.
- `37A07..37A72`: aligned dword-count fill helper with the encoded unrolled store schedule.
- `37A73..37A9C`: selector snapshot in ES,CS,SS,DS,FS,GS order.
- `37A9D`: one zero separator byte.
- `37A9E..37AC1`: DOS AH=48h allocation wrapper, preserving AX success and BX largest-block failure output semantics.
- `37AC2..37AD3`: DOS AH=49h free wrapper.
- `37AD4..37AFB`: DOS AH=4Ah resize wrapper, preserving BX maximum-size output on failure.
- `37AFC` begins the next independently decoded routine.
