# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `578377b11fe89f3847ab0f9b234abd0d912cac26` — `Integrate CODE.2 decompilation batch 75`.
- Verified translated instruction bytes: **207,124**.
- Proven non-code/data bytes: **4,220**.
- Reconciled object bytes: **211,344 / 239,698 = 88.1709% (88.17%)**.
- Unresolved object bytes: **28,354**.
- Last integrated ownership fold: `31C58..31E73` (instructions through `31E71`, proven alignment at `31E72..31E73`).
- Next independently decoded instruction boundary: **`31E74`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `31DF0..31ECF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x31DF0, 0x31ED0)` — 224 bytes.
- SHA-256: `d43121acce1172188763eff2bf76c739ef5b9ed7fca5c4854c7ccd18c9466fa0`.
- Narrow boundary window: `[0x31E20, 0x31E80)` — 96 bytes.
- Narrow SHA-256: `34455be5efb500028a1b1f9767873899ac1b14fba8ed573aeb0cfb76622c2253`.
- Narrow boundary bytes:
  `45f808eb1c8b45dc2b45ec406bd0078b45f4d1f801c289d0c1fa1ff77df48845f80fbe45f883f8087e04c645f8088a45f88b55dc888294fe0000ba7f0000002b55dc8a45f8888214fe0000eb8fc95f5e5bc38bc05356575589e581ec0c000000`.

Interpretation to re-prove on resume: `31E71` is the final Batch-75 `RET`; `31E72..31E73` is two-byte `MOV EAX,EAX` alignment with no incoming direct reference; `31E74` begins the next independent routine.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 76 bytes are authorized by this checkpoint. Any analysis at or beyond `31E74` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 75 merely because the overlap is re-disassembled. Verify overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `31E74`.
