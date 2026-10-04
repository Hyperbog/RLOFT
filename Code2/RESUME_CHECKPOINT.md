# CODE.2 resume checkpoint

This file is a durable handoff anchor. Read it together with `INTERRUPTION_RESUME_PROTOCOL.md`, `PROGRESS.md`, `RESIDUAL_LEDGER.md`, and the live `Code2.cpp`. If anything here disagrees with live repository/binary evidence, live evidence wins and this file must be corrected before forward accounting.

## Last committed substantive decompilation state

- Last substantive batch commit: `d7bef459987a363f84e4480b78018026dae1d7e0` — `Integrate CODE.2 decompilation batch 56`.
- Verified translated instruction bytes: **172,269**.
- Proven non-code/data bytes: **3,651**.
- Reconciled object bytes: **175,920 / 239,698 = 73.3924% (73.39%)**.
- Unresolved object bytes: **63,778**.
- Last integrated instruction ownership fold: `2873C..29413`.
- All Batch-56 inter-routine separators, including the directly proven zero-fill pad `290C3..290CF`, are counted in the residual ledger.
- Next independently decoded instruction boundary: **`29414`**.

## Immutable identity

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Object-1 file start: `0x3AF24`.
- Object-1 length: `0x3A852`.
- Address convention: all reconstruction addresses are object-relative offsets. For extracted-object `objdump -b binary`, use the tracked address directly; do not add `0x10000`.

## Mandatory overlap anchor for next resume

Re-disassemble at minimum `293B0..29490` from freshly regenerated object 1 before doing new work.

- Window: object-relative `[0x293B0, 0x29490)` — 224 bytes.
- SHA-256: `45af7194a6051f5794e2c40dd9bf651ad056ddfc09a21bd8804c172fb1a4e39b`.
- Narrow boundary window: `[0x293F0, 0x29450)` — 96 bytes.
- Narrow SHA-256: `a6b4b07519870f379449d880989b043c0b3b8111c97100fedb343952b3e08b5c`.
- Narrow boundary bytes:
  `0074a7ff742414ff742414ff742414ff742414ff742414e87811000083c414e9445300005356578b7c24148b74241c8b5c2420e83053000085f60f842d0100000fb74604894308a155ef0000c7030000000029f8c743040000000085c07e1929`

Interpretation to re-prove on resume, not merely trust: `293EC` is the final Batch-56 wrapper, conditionally branches backward to the shared `RET` at `2939A`, otherwise calls `2A584` and tail-jumps to `2E758`; its owned bytes end at `29413`. `29414` begins the next independent routine with `PUSH EBX / PUSH ESI / PUSH EDI`.

## Interrupted/in-progress state

**UNCOUNTED / NOT COMMITTED:** no Batch 57 bytes are authorized by this checkpoint. Any analysis at or beyond `29414` must be revalidated against the overlap before counting.

## Resume rule

Do not rework Batch 56 merely because the overlap is re-disassembled. Verify the overlap, immutable hashes, live accounting, and branch head; then proceed strictly forward from `29414`. If any overlap/hash/accounting mismatch appears, freeze forward progress and repair the prior batch first.
