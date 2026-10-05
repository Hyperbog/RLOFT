# CODE.2 explicit reconstruction repair — 36CEC..372F5

## Scope
The historical aggregate-accounting gap `[0x36CEC,0x372F6)` has now been directly disassembled from immutable CODE.2 object 1 and represented as explicit C++ in `.audit_gap_36cec.fragment.cpp`.

- Range: `36CEC..372F5`
- Length: **1,546 bytes**
- Immutable span SHA-256: `f0924e15e3109a1fe0b00a0cdc1e8748af213eaadc26048b32800dfbdc7dd80b`
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Reconstructed behavior
`function_36cec` is one coherent routine, not padding. The reconstruction preserves:
- six cdecl input slots at `EBP+08h..+1Ch`;
- `span_bits >> 3` signed count and `FFF8h` alignment-derived half-span;
- cap against `C738h` and table lookup through pointer `258C4h`, bounded by `C730h`;
- `105E7h` clamp to `0808h`;
- image selection from `18FE0h + 4*count`;
- scale-zero and scale-nonzero patch setup paths through the already reconstructed `36B9C`, `36BD8`, `36C10`, and `36C44` helpers;
- `3F29C(resource,3)` acquisition and the encoded `+2` object pointer;
- both descending and ascending word-table walks;
- global scratch/state cells `10450h`, `105D7h`, `105DBh`, `105DFh`, `105E3h`, and `105E7h`;
- 1Ch-byte record indexing through `15A18h` and clipping against `C6D8h`;
- the four first/last-threshold cases that trim or invoke the existing `36B14` renderer/helper;
- restoration through `36C90` or `36CBC` according to the original scale path.

The fragment has an exact ownership assertion:

`static_assert(0x372F6u-0x36CECu==1546u);`

## Accounting effect
This range was already part of the final translated-byte aggregate and therefore does **not** change the 235,022 translated / 4,676 non-code total. It changes semantic-audit eligibility: the previous 1,546-byte explicit-source gap is no longer merely aggregate-owned.

Using Checkpoint 48's inspection count as the running base, explicit inspection rises from **229,270** to **230,816 / 239,698 = 96.2945%**, leaving **8,882 bytes = 3.7055%** not yet semantically inspected.

## Remaining repair state
The `36CEC..372F5` missing-source defect is closed by this reconstruction. The previously identified localized semantic mismatches are handled by the separate audit repair pass and remain subject to integration/compile verification before final certification.

## Resume
Resume the forward audit at `0x385B0` using the Batch-143 explicit formatter fold, then continue into Batch 134 at `0x38867`.