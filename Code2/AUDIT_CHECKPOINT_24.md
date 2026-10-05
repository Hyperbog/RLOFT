# CODE.2 semantic audit checkpoint 24

Status: **PASS**

Audited object-relative range: **`202F0..22E3F`** (**11,088 bytes**).

Cumulative semantic audit: **142,896 / 239,698 = 59.6150%**.

Semantic defect classes remain **6**. No additional semantic defect was established across reconstruction batches 46–49.

## Immutable evidence

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Mandatory frontier overlap `[202B0,203B0)` SHA-256: `cb240548e96eb2b1086e974f6ccb53500cb06dfded418ccac4e722827483fdf7`
- Audited span `[202F0,22E40)` SHA-256: `83f22281205fadab3d2e0e16fe601b78da58805b314464ff214811e842c7cd01`

Fresh overlap disassembly re-established the three-byte `202ED..202EF` LEA alignment and independent `202F0` entry before any forward accounting. At the right boundary, `22E3D` is the dispatcher RET, `22E3E..22E3F` are the already-accounted separator bytes, and `22E40` begins the next independent routine.

## Semantic review

The CODE.1 final-audit defect checklist was applied directly to the immutable CODE.2 instructions throughout the span, with particular scrutiny on cdecl stack slots, signed division/remainder, partial-register aliases, callback-sensitive global reloads, fixed-point packing, one-past-count ownership, and table-driven dispatch.

Batch 46 (`202F0..20E08`) retains the encoded resource/memory sizing thresholds at entry, stack-slot reuse in `20850/207E8`, signed quotient/remainder behavior in `20994`, byte-wrap timer semantics in `20B40`, and packed fixed-point argument construction in `20C0C`. No widening or normalization drift was found.

Batch 47 preserves the impossible `210B0` word-sentinel comparison exactly, the environment-list fallback ordering in `211D8`, and `214B8`'s separation between saved current-environment values and the explicit post-callback reload of `255D8`.

Batch 48 preserves `2195C`'s low-word fixed-point overwrite, the mixed raw-SAR versus truncation-toward-zero arithmetic in `21BF0`, ownership of the exact eight-entry `21B24` table, and the intentionally dead constant-nonzero branch in `21F70`.

Batch 49 preserves the partial-register page toggle and one-past-count ownership in `223F4`, both exact CS tables owned by `229CC`, the failed-probe `EBX -= 20h` carry-forward, `22894`'s signed division behavior, and the queue-head update before dispatch to `229CC`, `12E70`, or `02DB0`. The terminal dispatch retains its three cdecl arguments in the encoded order.

No CODE.1-derived defect class reproduced in this span, and no correction to `Code2.cpp` was required.

## Resume anchor

Next semantic-audit frontier: **`22E40`**. Reconstruction Batch 50 continues from that independent routine and reaches `23FC4`; preserve its equality-only list stop, packed-nibble DEC/JNE loops, signed panel arithmetic, rollback/angle/timer paths, and partial flag/collision retry behavior during the next pass.
