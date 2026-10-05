# CODE.2 semantic audit checkpoint 15

- Audited span: `11C2C..12987` — 3,420 bytes.
- Cumulative semantic audit: **76,152 / 239,698 = 31.7700%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Span SHA-256: `79ca61ce712932c29a4e06bcc47ddfddea7807114eac8b8705179ef6b6c61326`.
- Mandatory overlap `[0x11BEC,0x11CAC)`: `c75a68863cec721ed8a14ec26f3669f37d4b189ee3ad19dec2e958589a89a523`.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. `11C2C..11C3B` is the four-entry CS dispatch table consumed by the executable family beginning at `11C3C`. The family preserves encoded grid geometry, same-container rejection, capacity arithmetic, signed actor/container fields, and byte-shift variant semantics. The six-way type dispatch around `12168` remains table-driven; the `12180` family preserves the empty-cursor hitbox scan and slot-0F container close/reopen behavior. `12470` preserves the complete `248h` actor-block swap, separate four-byte panel-state swap, and three-bit actor-index rewrite in both record families. `12724` preserves the sparse status-bit scan, stopping after fourteen displayed statuses or bit `02000000h`, followed by two distinct modal wait phases.

High-risk review covered signed `MOVSX` actor fields, unsigned byte fields, signed `IDIV` coordinate arithmetic, AL-only shift/update behavior, global/page reloads across opaque calls, cdecl slot ordering, and address-distinct wrappers. No additional CODE.1 defect class reproduced here.

`12986..12987` is encoded `MOV EAX,EAX` alignment. Next semantic-audit frontier: **`12988`**.
