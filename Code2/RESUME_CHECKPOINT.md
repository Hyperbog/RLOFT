# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `0fb4605ca0e835b043974c2fcdef1851506a2c23` — `Integrate CODE.2 decompilation batch 70`.
- Verified translated instruction bytes: **200,937**.
- Proven non-code/data bytes: **4,059**.
- Reconciled object bytes: **204,996 / 239,698 = 85.5226% (85.52%)**.
- Unresolved object bytes: **34,702**.
- Last integrated ownership fold: `2F580..305A7` (instructions through `305A5`, proven separators/table through `305A7`).
- Next independently decoded instruction boundary: **`305A8`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `304F4..305D3` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x304F4, 0x305D4)` — 224 bytes.
- SHA-256: `fe54ab4feef70a58dfb46463b018e3fa8e0beb02788c8fd6e71d4cad3fce7bd6`.
- Narrow boundary window: `[0x30558, 0x305B8)` — 96 bytes.
- Narrow SHA-256: `0e41c3d36817c5b55f329b207144b080b8d7617535d2b7cea4646c44859fe94a`.
- Narrow boundary bytes:
  `0fbe030fb64401044301c2ebee8b44241029d08b54240cd1f8ff74241801c2ff742418891580b60200e842feffff83c4085bc390a184b60200c38bc0a180b60200c38bc08b442404a36cf50000c38bc053837c241000745a6a00ff7424188b5c`

Interpretation to re-prove on resume: `305A5` is the final Batch-70 `RET`; `305A6..305A7` is `MOV EAX,EAX` alignment; `305A8` begins the next independent `PUSH EBX` text-layout routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 71 bytes are authorized by this checkpoint. Any analysis at or beyond `305A8` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 70 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `305A8`.
