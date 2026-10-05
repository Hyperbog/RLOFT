# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-25.78%25-yellow)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**
**Verified translated instruction bytes:** **235,022**
**Proven non-code/data bytes:** **4,676**
**Reconciled object bytes:** **239,698 / 239,698 = 100.0000% (100.00%)**
**Unresolved object bytes:** **0**
**Semantic audit:** **61,783 / 239,698 = 25.7754%**, closed through object offset `0x0F166`; **6 semantic defect classes** found so far.
**Status color:** GREEN — complete object-1 byte accounting. Semantic audit remains a separate follow-up phase.

## Immutable source identity

- `CODE.2` size: 558,689 bytes (`0x88661`)
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 file start: `0x3AF24`
- Object-1 virtual base: `0x10000`
- Object-1 virtual size: `0x3A852`
- Object-1 page count: 59
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Entry logical offset: `0x353A0`
- Object-2 virtual base: `0x50000`
- Object-2 virtual size: `0x2DD10`
- Object-2 physical start: `0x75F24`

## Current phase

Object-1 byte ownership is complete. The active follow-up is the end-to-end semantic audit documented in `Code2/FINAL_SEMANTIC_AUDIT.md`. Audit checkpoints are regenerated from immutable `CODE.2`, with the fully audited CODE.1 reconstruction and `Code1/FINAL_BINARY_AUDIT.md` used as a cross-reference defect checklist.
