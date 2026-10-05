# CODE.2 semantic audit checkpoint 16

- Audited span: `12988..139AA` — 4,131 bytes.
- Cumulative semantic audit: **80,283 / 239,698 = 33.4930%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Span SHA-256: `d487c378144cd4d6dca232f046388b839ddfba1279b0b8c3981566d2c4abe8f5`.
- Mandatory overlap `[0x12948,0x12A48)`: `0aa8e11e8f4bb8eaee4fb4a8d42f877d87fec6e5bb83b5a8863e9bf35f50e563`.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. The checkpoint covers the modal input/redraw families beginning at `12988`, ordered hit-region interaction around `12E70`, the partial-register helpers at `13268/13284`, compact stream decoding at `1330C/133CC`, unsigned countdown/seek behavior at `13488`, and the ten-way CS packet dispatcher at `135C4`. Referenced `CINE00.ANM` bytes at `131D0` remain data rather than executable code.

High-risk review applied the fully audited CODE.1 defect checklist to cdecl stack-slot order, global reloads across opaque calls, signed/unsigned branch conditions, low-byte/low-word register effects, truncation, and address-distinct control flow. The encoded two-phase modal waits and page-toggle redraw ordering are retained. The overlapping hit regions remain ordered rather than normalized. The `13268/13284` returns preserve x86 partial-register semantics, and the stream decoder retains its compact state in `7F88`. No additional semantic drift was established from the controlling bytes.

`139AB` is encoded NOP alignment and is left to the next checkpoint. Next semantic-audit frontier: **`139AB`**.
