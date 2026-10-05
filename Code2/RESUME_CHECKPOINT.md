# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `7fb9652da981e6cd397e8a09c728fb2f1b7333d1` — `Integrate CODE.2 decompilation batch 73`.
- Verified translated instruction bytes: **205,657**.
- Proven non-code/data bytes: **4,215**.
- Reconciled object bytes: **209,872 / 239,698 = 87.5568% (87.56%)**.
- Unresolved object bytes: **29,826**.
- Last integrated ownership fold: `314B0..318B3` (instructions through `318B0`, proven alignment through `318B3`).
- Next independently decoded instruction boundary: **`318B4`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `31830..31913` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x31830, 0x31914)` — 228 bytes.
- SHA-256: `552f3c1e834ba1d24b05f7b63f7815a800515c1cc7517f0449789fbc31b509ec`.
- Narrow boundary window: `[0x31880, 0x318E0)` — 96 bytes.
- Narrow SHA-256: `5fa25dcd97e9ece17f701d2b370fd68c04f634a7c0fe129326b2becfaaa9ff46`.
- Narrow boundary bytes: `45e8508b45e4506a00e8de7fffff83c418e99bfdffff683f010000688f0100006a006a00e8177effff83c410c95f5e5bc38d40005356575589e581ec18000000a138c700008945ec8b451483c0098945f48a4514a218f90000c745e800000000`.

Interpretation to re-prove on resume: `318B0` is the final Batch-73 `RET`; `318B1..318B3` is three-byte LEA alignment with no incoming direct reference; `318B4` begins the next independent routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 74 bytes are authorized by this checkpoint. Any analysis at or beyond `318B4` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 73 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `318B4`.
