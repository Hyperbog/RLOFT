# CODE.2 semantic audit checkpoint 19

- Audited span: `1548C..15E37` — 2,476 bytes.
- Cumulative semantic audit: **89,640 / 239,698 = 37.3979%**.
- Semantic defect classes: **6**; no new defect established in this checkpoint.
- Span SHA-256: `d88ab2d9f846ac7a205899564aa380cb16ade35e1f18ee0adab169ef499c4187`.
- Mandatory overlap `[0x1544C,0x1554C)`: `4040917f47d79584a1b0360d71dabca99898788197479ef5f0331939184ee4f1`.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.

## Semantic result

PASS. `1548C..15533` is the exact 42-entry CS dword dispatch table consumed by the indirect jump at `1556D`; the executable family begins at `15534`. The audit preserved the exact `00h..29h` dispatch, opcode `0Bh`'s live-EBX second argument into `1C160`, opcode `08h`'s distinct `FFFFh` fifteen-item condition, zero-extended item words passed to `0F4B0/0F540`, opcode `0Fh`'s five 12-byte timer slots and `FFFFh -> FFFFFFFFh` disable conversion, opcode `12h`'s nested interpreter with `7FB8` gating and stream-position restore, opcode `23h`'s unsigned low-32 multiply followed by logical `>>15` scaling, and opcode `24h`'s forced return value `3` after `13EF8`.

The two small interpreter wrappers after the dispatcher preserve stream-position save/restore distinctions. `15DF5..15DF7` and `15E21..15E23` are encoded `LEA EAX,[EAX+0]` alignment; `15E37` is an encoded NOP. No additional CODE.1 audit defect class reproduced in this ownership-complete family.

Next semantic-audit frontier: **`15E38`**.
