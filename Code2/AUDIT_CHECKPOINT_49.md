# CODE.2 semantic audit checkpoint 49

## Scope
This checkpoint closes the historical Batch-143 formatter fold and continues through Batch 134.

- Batch-143 explicit fold: `[0x385B0,0x38867)` = **695 bytes**.
- Batch-134 parser: `[0x38867,0x38A0A)` = **419 bytes**.
- Newly inspected this checkpoint: **1,114 bytes**.
- Prior inspection after explicit `36CEC..372F5` reconstruction: **230,816 bytes**.
- Cumulative semantic inspection: **231,930 / 239,698 = 96.7593%**.
- Remaining uninspected: **7,768 bytes = 3.2407%**.
- Next boundary: **`0x38A0A`**.

## Immutable identity
- CODE.2 SHA-256 reproduced: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Object-1 SHA-256 reproduced: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- `[385B0,38867)` SHA-256: `1c2d9ab63b56c3b6a6ea2552d64187fc631a94d43275302fc337c8bad2644781`.
- `[38867,38A0A)` SHA-256: `17deff407d631e6e05710b7e16a05b03075ad62a72f42620a000a6cfbf24b086`.
- next-overlap anchor `[389CA,38A4A)`: `4ff7566a20ae51bd0cc5d4b5a718078cd1ca235fcad6e713d9f05e2e7ead7f49`.

## Batch-143 formatter fold — `385B0..38866`
Direct immutable disassembly was compared against `.batch143.fragment.cpp`.

No new semantic mismatch was established.

Preserved behavior includes:
- literal emission and `%` dispatch;
- parser call into `38867` with the encoded cdecl argument order;
- `%n` stores using the exact 10h/20h/40h/80h modifier split, including selector-qualified far-pointer loads;
- output count storage width (word versus dword);
- conversion call into `38BC1` and returned ES:ESI data location;
- signed pre-padding decrement/test behavior;
- prefix emission from the three-byte prefix buffer;
- zero-fill count behavior;
- post-prefix zero-pad path;
- byte-string versus two-byte-stride string output;
- generic byte output path;
- left-justified trailing spaces;
- shared epilogue at `38862`.

## Batch 134 — `38867..38A09`
Exact Batch-134 integrated source was compared against immutable disassembly.

No new semantic mismatch was established.

Preserved behavior includes:
- 32-bit decimal width/precision accumulation with wrap semantics;
- cdecl `*` consumption through the mutable argument pointer;
- negative width `NEG`/modulo-32 behavior and left-justify bit 08h;
- precision default `FFFFFFFFh` and negative-star restoration to that sentinel;
- exact modifier mapping: `l/w -> 20h`, `h -> 10h`, `L/N -> 40h`, `F -> 80h`;
- flag parser reset at entry;
- `-`, `#`, `+`, space and `0` handling;
- `+` clearing the space bit and later spaces being ignored while plus is active;
- `0` modifying the pad byte rather than introducing a separate flag bit.

## Defect state
No new defect was found in this checkpoint.

The localized mismatches discovered earlier have deterministic repair definitions staged separately. The former `36CEC..372F5` missing-source defect is closed by explicit reconstruction. Final certification still requires integration/compile verification of the staged repair pass.

## Resume
Continue from **`0x38A0A`**. Batch 135 owns `38A0A..38A2F`, `38A61..38AC2`, `38AC3..38BA5`, and `38BA6..38BC0`; the intervening `38A30..38A60` is the Batch-143 selector-qualified bounded word-string helper and must be audited once, without double-counting.