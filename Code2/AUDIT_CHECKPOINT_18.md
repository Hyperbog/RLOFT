# CODE.2 semantic audit checkpoint 18

- Audited span: `148F0..1548B` — 2,972 bytes.
- Cumulative semantic audit: **87,164 / 239,698 = 36.3649%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Span SHA-256: `1f2d80c7e788b7f61995899693f40b85c6502688c3bf35d710208dba795189bf`.
- Mandatory overlap `[0x148B0,0x149B0)`: `bd9579628074fd65de4d8dd6a3836b4aa6dcd1bd0f8f1a1d2847fcb5dbc1300`.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. Fresh immutable-object disassembly was regenerated across the complete ownership family beginning at `148F0` and ending at the RET at `1548B`. The mandatory overlap also revalidated the prior `148EE..148EF` `MOV EAX,EAX` alignment and the independent `148F0` entry.

High-risk review preserved the reconstruction-era ownership notes directly against the bytes: `148F0` retains class-dependent activation side effects; `14A60` retains the low-byte `0080h` test rather than widening it; the later helper preserves the live-EBX second argument into `1C160`; `14E9C` retains its asymmetric A6 handling; `15050` keeps the item value full-width until the encoded low-word store; `15280` retains the second-pass encoded no-op flag clear; and `153FC` preserves once-per-cursor recursive presence counting versus multiplicity counting for party inventory. Signed/unsigned extensions, partial-register writes, cdecl slot order, global reloads across opaque calls, and address-distinct helper behavior were rechecked with the CODE.1 audit defect list.

No new semantic correction was required. `1548C` begins the next CS data/dispatcher ownership unit; the owning dispatcher begins at `15534`. Next semantic-audit frontier: **`1548C`**.
