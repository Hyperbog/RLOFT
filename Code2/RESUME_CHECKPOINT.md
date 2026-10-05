# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `bfd2df3f1ec0c0886dced4e51d9b34a836b5a755` — `Integrate CODE.2 decompilation batch 130`.
- Verified translated instruction bytes: **227,486**.
- Proven non-code/data bytes: **4,672**.
- Reconciled object bytes: **232,158 / 239,698 = 96.8544% (96.85%)**.
- Unresolved object bytes: **7,540**.
- Last integrated ownership fold: `37CA3..37E76`.
- Next independently decoded instruction boundary: **`37E77`**.
- The earlier large helper `36CEC..372F5` remains unresolved.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchors for next resume
- Previous window `[0x37C63,0x37D23)` — 192 bytes; SHA-256 `baf2274023170be44827c06d56f0d5646e272b101d85b0947e39232a7d52f83e` (revalidated after interruption).
- Previous window `[0x37C83,0x37D03)` — 128 bytes; SHA-256 `219d1d66e8ae053ff7f68adbf19ebd3ef327eeacba08649015c03846d47aea96` (revalidated after interruption).
- Batch-owned `37CA3..37E76` — 468 bytes; SHA-256 `2388b8b64c8cae7d0ad1a1eac1ed70c82fde372437522672bfe2013b3f225802`.
- Next overlap window `[0x37E37,0x37EF7)` — 192 bytes; SHA-256 `10d2fb78985a49f70b7acb63f5bda9d6c3adfc8d4c863bf2402fe6c670b2dcc8`.
- Batch-owned bytes must not be recounted; overlap bytes are verification only.

## Batch 130 interpretation
- `37CA3..37CCD`: small errno entry points/common setter; public wrappers install constants 13 and 14, while the common tail writes the requested value through the runtime errno pointer.
- `37CCE..37CDF`: alternate errno-domain setter plus indirect callback wrapper.
- `37CE0..37D5F`: stream-buffer preparation; derives default capacity from stream flags, allocates storage, and falls back to the embedded one-byte buffer when allocation fails.
- `37D60..37E23`: stream flush/synchronization; writes pending buffered output or seeks input backward by unread bytes, preserving the encoded error-flag behavior.
- `37E24..37E76`: DOS INT 21h/AH=43h attribute-query wrapper, including writable-vs-read-only rejection and DOS error translation.
- `37E77` begins the next independently decoded routine; fresh overlap disassembly shows it forwards into the larger open/create path beginning at `37E99`.

## Validation
- Batch 130 integration workflow run `37331067597` completed successfully.
- Required `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp` and `git diff --check` passed in the integration gate.
- Temporary Batch 130 fragment/integrator/workflow were removed by the substantive integration commit.
