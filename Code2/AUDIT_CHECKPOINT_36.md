# CODE.2 semantic audit checkpoint 36

## Scope

- Previous frontier: `0x35A0C`
- Audited range: `0x35A0C..0x35F87`
- Bytes audited: **1,404**
- Cumulative semantic audit: **221,048 / 239,698 = 92.2194%**
- Semantic defect classes: **6**; no new class established.
- Next ownership boundary: `0x35F88`

## Immutable verification

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- owned span `[0x35A0C,0x35F88)`: `1dfce61cd6ec710db998d4a3ea510c01f7730a6b0110633e863ccea82561b78d`
- next-boundary overlap `[0x35F48,0x36008)`: `86d66e8c1e699e59f33ffe130941882a72a7b47709978884769cc56660389bc9`

## Result

Batch 117's loader retains raw frame aliasing, direct/file source split, flag-4 allocation ownership, 16-byte alignment, frame-local base tables, and relocation validation/store ordering. `35F87` is the proven one-byte alignment separator. No new semantic mismatch was established.
