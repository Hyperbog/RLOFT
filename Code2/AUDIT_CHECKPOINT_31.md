# CODE.2 semantic audit checkpoint 31

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`31C58..3261F`**.
- Bytes closed by this checkpoint: **2,504**.
- Cumulative semantic audit: **206,352 / 239,698 = 86.0883%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`31C58..3261F`): `d170c2733cc5bd36cadf0ac38fd541a3f18be972388a0f67e4d78061d1a03e28`.
- Previous-frontier overlap `[31C18,31D18)` reproduced exactly as `8cf7fdecf775a006c57b892c11943826e273623a074faceec4de59334118c3b2` before forward work.
- Mandatory overlap around the next frontier `[325E0,326A0)`: `58de01293de72c3c9b2b4f6e43920ba483aca96fb88e5c143fd9e29525b9a3ad`.

## Ownership/family coverage

This checkpoint closes original reconstruction **Batches 75 through 81** as one combined ownership pass. These batches are individually small but form a coherent low-level graphics/palette helper region, so combining them avoids checkpoint overhead without crossing the independent routine at `32620`.

- **Batch 75 (`31C58..31E73`)**: palette lookup builder and paired-table expansion. Rechecked signed arithmetic, low-byte table values, loop bounds, and exact store widths.
- **Batch 76 (`31E74..31F93`)**: palette/remap helper family. Preserved partial-register behavior and encoded table-index arithmetic rather than widening intermediate values.
- **Batch 77 (`31F94..320F7`)**: compact graphics-state helper family. Rechecked call argument order, global reloads, and address-distinct wrapper behavior.
- **Batch 78 (`320F8..3214F`)**: short state/table helper. Preserved exact word/dword accesses and return-register semantics.
- **Batch 79 (`32150..3221F`)**: small graphics operation family. Rechecked cdecl slot order and the caller-visible return/side-effect ordering.
- **Batch 80 (`32220..324CB`)**: graphics/palette transformation family. Rechecked width-sensitive arithmetic, table traversal, signed/unsigned comparisons, and wrapped 32-bit address arithmetic.
- **Batch 81 (`324CC..3261F`)**: compact draw/dispatch helpers. The closing routine pushes three arguments to `312EC` in encoded stack order, then calls `32150` and `312DC`; `3261C..3261F` remain the established four-byte non-code/padding ownership before the independent `32620` entry.

## High-risk semantic checks

The fully audited CODE.1 defect catalogue was reapplied across all seven batches: cdecl slot order, live-register inputs, byte/word partial-register aliases, signed/unsigned branch drift, modulo-32-bit arithmetic, global reloads across calls, address-distinct wrappers, and encoded oddities. Fresh immutable disassembly was used as controlling evidence throughout. No additional mismatch was established.

## Boundary result

The prior overlap reproduces exactly. Fresh next-frontier disassembly confirms `3261B` as the final RET, `3261C..3261F` as the established four-byte ownership gap, and **`32620`** as an independent `ENTER 0,0` routine. The next semantic-audit frontier is therefore **`32620`**.
