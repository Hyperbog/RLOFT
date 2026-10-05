# CODE.2 semantic audit checkpoint 30

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`2F580..31C57`**.
- Bytes closed by this checkpoint: **9,944**.
- Cumulative semantic audit: **203,848 / 239,698 = 85.0437%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`[2F580,31C58)`): `2c95f2384128cd879e2fde26b9731f7accfa9130a40d820adfd1bf761ab4c3ec`.
- Previous-frontier overlap `[2F540,2F640)` reproduced exactly as `04f451f264eede109af14694415dfc45fb76748713890629c2ea340b9922e80b` before forward work.
- Mandatory overlap around the next frontier `[31C18,31D18)`: `8cf7fdecf775a006c57b892c11943826e273623a074faceec4de59334118c3b2`.

## Ownership/family coverage

This increased-throughput pass closes original reconstruction **Batches 70 through 74** as one ownership-complete semantic checkpoint.

- **Batch 70 (`2F580..305A7`)**: presentation/input path, buffered/RLE resource reader, image materialization, and text metrics/drawing helpers. Rechecked live-ESI return ownership, exact four-way decode table at `2FD8C`, low-word multiply result preservation, signed count/availability comparison, image argument order, and the `305A6..305A7` alignment ownership.
- **Batch 71 (`305A8..30D2F`)**: text layout, IRQ/tick/keyboard state, vector lifecycle, RNG, and overlap-safe memory move. Preserved the signed-vs-unsigned glyph-index distinction, partial-register keyboard state, exact ten-entry direction table, interrupt/vector ordering, and the proven `30D26..30D2F` padding boundary.
- **Batch 72 (`30D30..314AF`)**: runtime memory/DPMI wrappers, allocator recovery, buffered output, DOS/string wrappers, and handle/drive helpers. Revalidated the corrected DPMI result-bit polarity, truncating signed division helpers, AL-only state changes, argument order through runtime wrappers, and the `311D5..311DF` / `314AB..314AF` padding ownership.
- **Batch 73 (`314B0..318B3`)**: perspective-row renderer. Preserved signed record-key promotion, record-zero unconditional processing, first-zero sentinel behavior only after record zero, wrapped horizontal projection arithmetic, signed world/camera delta construction, random-set prepass, and the three-byte `318B1..318B3` LEA alignment.
- **Batch 74 (`318B4..31C57`)**: packed shading lookup generators. Revalidated the reconstruction-time SAR correction against the encoded arithmetic shift, modulo-32 add/multiply helpers, signed-byte clamp behavior, packed byte/word stores, division semantics, table-address asymmetries, `31A12..31A13` MOV-self alignment, and the final `31C57` NOP.

## Cross-cutting semantic review

Fresh immutable disassembly was regenerated across the checkpoint boundaries and representative high-risk interiors. The fully audited CODE.1 defect catalogue was reapplied as a checklist only: cdecl slot order, live-register call inputs, byte/word aliases, signed/unsigned branch drift, modulo-width arithmetic, encoded division/shift behavior, selector/interrupt state, global reloads across callbacks, address-distinct wrappers, table ownership, and odd control-flow preservation. No additional CODE.1 defect class reproduced in this span.

The historical Batch-72 and Batch-74 staging corrections were specifically rechecked rather than assumed: the DPMI flag test remains `bit set -> linear base << 4`, and the Batch-74 half operation remains arithmetic SAR semantics rather than truncation-toward-zero division.

## Boundary result

Fresh disassembly confirms `31C56` is the Batch-74 RET and `31C57` is its one-byte NOP separator. **`31C58`** begins a new independent register-save/local-frame routine. The next semantic-audit frontier is therefore **`31C58`**.
