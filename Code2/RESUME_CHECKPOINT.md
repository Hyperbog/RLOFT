# CODE.2 resume checkpoint

Durable handoff anchor; live repository/binary evidence wins.

## Last committed substantive decompilation state
- Last substantive batch commit: `e5c15fad82a10f9b901ea8d45d9d6fd14891cb70` — `Integrate CODE.2 decompilation batch 122`.
- Verified translated instruction bytes: **224,685**.
- Proven non-code/data bytes: **4,528**.
- Reconciled object bytes: **229,213 / 239,698 = 95.6257% (95.63%)**.
- Unresolved object bytes: **10,485**.
- Last integrated ownership fold: `36B9C..36CEB`.
- Next independently decoded instruction boundary: **`36CEC`**.

## Immutable identity
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Mandatory overlap anchor for next resume
- Window `[0x36CAC,0x36D6C)` — 192 bytes; SHA-256 `46ad7bec0ade95f6e224d7bd3cfac826c4380ea42a3f8238824bfd4cc451577e`.
- Narrow `[0x36CCC,0x36D4C)` — 128 bytes; SHA-256 `4268d5f06c4ff573fda7c6472f06e88514dd008da30c6d2dfde3549ede9f3381`.
- Overlap bytes are verification only and must not be recounted.

## Batch 122 interpretation
- `36B9C..36C0E`: reverse/forward 128-entry cumulative word-table searches.
- `36C10..36C8E`: save-and-patch helpers for callback bytes and short-jump words.
- `36C90..36CE8`: reverse restoration helpers.
- `36C0F`, `36C8F`, `36CB9..36CBB`, `36CE9..36CEB`: eight alignment bytes.
- `36CEC` begins the next large helper and remains uncounted.
