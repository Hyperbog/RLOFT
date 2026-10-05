# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `6398b9932085707157079d8954cf2df268e07886` — `Integrate CODE.2 decompilation batch 69`.
- Verified translated instruction bytes: **196,867**.
- Proven non-code/data bytes: **3,993**.
- Reconciled object bytes: **200,860 / 239,698 = 83.7971% (83.80%)**.
- Unresolved object bytes: **38,838**.
- Last integrated ownership fold: `2F0B4..2F57F` (instructions through `2F57C`, proven separators through `2F57F`).
- Next independently decoded instruction boundary: **`2F580`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2F500..2F5DF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2F500, 0x2F5E0)` — 224 bytes.
- SHA-256: `08a72c74812fc0bb663a87796e84f675c02da21ee812d4d300da50fed4170435`.
- Narrow boundary window: `[0x2F540, 0x2F5A0)` — 96 bytes.
- Narrow SHA-256: `b385648efa79dafb989f25c2739e8be9eb9c51a0e2026ad8092978d3600b3601`.
- Narrow boundary bytes:
  `e8ab810000ff742404e8705d000083c404c38bc068ff000000e8969bffff83c404e87affffff837c240400740fe87e8100006a01e8455d000083c404c38d400053833dc0f00000ff74388a1db1f0000080e3800fb6db85db752853ff74240ce8`

Interpretation to re-prove on resume, not merely trust: `2F57C` is the final Batch-69 `RET`; `2F57D..2F57F` is `LEA EAX,[EAX+0]` alignment; `2F580` begins the next independent routine with `PUSH EBX`, tests `F0C0` against `FFFFFFFFh`, then tests bit 7 of `F0B1` before routing into the presentation path.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 70 bytes are authorized by this checkpoint. Any analysis at or beyond `2F580` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 69 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2F580`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
