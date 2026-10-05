# CODE.2 semantic audit checkpoint 20

- Audited span: `15E38..171FB` — 5,060 bytes.
- Cumulative semantic audit: **94,700 / 239,698 = 39.5080%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. This deliberately larger checkpoint closes the ownership-complete families represented by original reconstruction batches 29 and 30, ending at the proven `171F9..171FB` LEA alignment immediately before the independent save-file writer at `171FC`.

The audit retained the original encoded distinctions rather than normalizing the reconstruction: the five 10-byte actor sub-blocks in the `16A88` family remain driven by four signed mapping bytes; the associated record/state paths preserve signed/unsigned width distinctions, partial-register effects, exact sentinel handling, address-distinct calls, and reloads across opaque calls. The Batch-29 separator/alignment bytes documented in the residual ledger were treated only as their proven ownership class and were not interpreted as executable semantics.

The Batch-30 family was reviewed through the complete pre-save boundary. High-risk CODE.1 defect classes were applied explicitly: cdecl stack-slot order, live-register arguments, signed branch conditions, byte/word truncation, modulo-32-bit arithmetic, partial-register aliases, global reloads, shared-tail preservation, and exact data/alignment ownership. The encoded state/record traversal and filename/data setup retain their original width and sentinel behavior. The trailing resource-name/data area includes the documented `level.dat` string preceded by nine proven zero-data bytes; those bytes remain data, not fall-through code.

No additional semantic correction was established in `15E38..171FB`. The independent save-file writer begins at `171FC` and is intentionally left for the next checkpoint.

Next semantic-audit frontier: **`171FC`**.
