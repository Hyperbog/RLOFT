# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `7e24c19795cb4200502e861f72082644e581287d` — `Integrate CODE.2 decompilation batch 62`.
- Verified translated instruction bytes: **179,439**.
- Proven non-code/data bytes: **3,765**.
- Reconciled object bytes: **183,204 / 239,698 = 76.4312% (76.43%)**.
- Unresolved object bytes: **56,494**.
- Last integrated instruction ownership fold: `2AC80..2B085`.
- Batch-62 compiler separators `2ACDA..2ACDB`, `2AD15..2AD17`, `2ADDD..2ADDF`, and `2B086..2B087` are directly proven non-code/alignment and counted in the residual ledger.
- `2AE30` retains its entry-time record base but reloads the `C050`-selected 16-bit record count at every loop test; the two resource-state probes reached through `2AD18` remain opaque/address-distinct calls.
- Next independently decoded instruction boundary: **`2B088`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `2B000..2B0DF` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x2B000, 0x2B0E0)` — 224 bytes.
- SHA-256: `96140f0994fb0a075f5064ff8151e3d8be8049e6289f175229fcd83e1d537050`.
- Narrow boundary window: `[0x2B040, 0x2B0A0)` — 96 bytes.
- Narrow SHA-256: `5ccd80e3fb9008a17f98f1e426a9ffc4cd8514973fc0460dc4f88884e210b471`.
- Narrow boundary bytes:
  `09c745d401000000eb19817dd0910000007509c745d401000000eb07c745d400000000837dd4007409c745fc02000000eb0ce912feffffc745fc000000008b45fcc95f5e5bc38bc05356575589e581ec10000000c745f0ffffffffc745f80000`

Interpretation to re-prove on resume, not merely trust: `2B000..2B085` is the final tail of the Batch-62 rectangle/type classifier and returns at `2B085`; `2B086..2B087` is a `MOV EAX,EAX` compiler separator. `2B088` begins the next independent routine with the four-register prologue, a 10h-byte local frame, local `-10h = -1`, local `-8 = 0`, and a signed loop bound of `100h`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 63 bytes are authorized by this checkpoint. Any analysis at or beyond `2B088` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 62 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `2B088`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
