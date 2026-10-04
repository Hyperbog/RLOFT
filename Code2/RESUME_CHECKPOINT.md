# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `6e1f7cc360f07ece2cc0971f465801afe4edb050` — `Integrate CODE.2 decompilation batch 58`.
- Verified translated instruction bytes: **174,489**.
- Proven non-code/data bytes: **3,695**.
- Reconciled object bytes: **178,184 / 239,698 = 74.3369% (74.34%)**.
- Unresolved object bytes: **61,514**.
- Last integrated instruction ownership fold: `29930..29CE8`, including the address-distinct `29AA0` tail wrapper and shared executable epilogue `29B37..29B3D`.
- Batch-58 separators through `29CEB`, including direct CS table `29B33..29B36`, are counted in the residual ledger.
- Next independently decoded instruction boundary: **`29CEC`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `29C80..29D60` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x29C80, 0x29D60)` — 224 bytes.
- SHA-256: `ee20a696989333268c61ba4dd7394cb79a4d455ddf6347240b2ee1d221d8e02a`.
- Narrow boundary window: `[0x29CC0, 0x29D20)` — 96 bytes.
- Narrow SHA-256: `fff066b0c7769e7f9a41f0774b52dfcbd7a9801234f44cdd6f98f1c2d514cd52`.
- Narrow boundary bytes:
  `000081fbc00000007e0fb9c0000000f36e81ebc0000000ebe483fb0074048bcbf36e5b595a5f5ec9c387db90c80c0000565752515366bace038a25e0ef0000b00366ef8b35f9ef00008b4d142b4d0c74737914f7d98b5d14875d0c895d148b5d`

Interpretation to re-prove on resume, not merely trust: `29CA0` is the final Batch-58 palette streamer and returns at `29CE8`; `29CE9..29CEB` is the proven `XCHG EBX,EBX / NOP` separator; `29CEC` begins the next independent routine with `ENTER 0Ch,0`, followed by `PUSH ESI / PUSH EDI / PUSH EDX / PUSH ECX / PUSH EBX` and graphics-controller setup.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 59 bytes are authorized by this checkpoint. Any analysis at or beyond `29CEC` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 58 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `29CEC`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
