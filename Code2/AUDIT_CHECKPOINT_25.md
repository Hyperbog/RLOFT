# CODE.2 semantic audit checkpoint 25

Checkpoint 25 continues the end-to-end semantic audit from `0x22E40` through `0x25DFB`, an ownership-complete **12,220-byte** span covering reconstruction Batches 50, 51, and corrected Batch 52.

## Immutable evidence

- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- regenerated object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- mandatory overlap `[0x22E00,0x22F00)`: SHA-256 `364ab5de1098fd63883353ed37d5f2e51f5a7bbf04447c84fa4f4dada848f3f8`
- audited span `[0x22E40,0x25DFC)`: SHA-256 `30598f970458450a1560fae548e3065b5c4800b9bf162e67cbbbc948d398df17`

The overlap reproduces the prior queue-dispatch tail through `22E3D` RET, the two-byte separator at `22E3E..22E3F`, and the independent signed actor-role lookup at `22E40`. The audited family closes with `25DF9` RET and encoded `MOV EAX,EAX` alignment at `25DFA..25DFB`; the exact CS dword dispatch table beginning at `25DFC` remains with its owning dispatcher for the next checkpoint.

## Semantic review

Fresh disassembly was reconciled against the reconstructed Batches 50-52 and the fully audited CODE.1 defect checklist. Particular attention was paid to:

- `22E40` signed-byte role comparisons and signed-word return values from the 248h actor records;
- `23024` equality-only list termination and callback-sensitive actor/page state;
- `231C0` signed divide-by-two setup and packed-nibble `DEC/JNE` loops without normalizing the encoded loop entry behavior;
- `23438` signed panel arithmetic, nibble wrap/decrement behavior, and signed actor flag tests;
- `2388C` rollback, angle, timer, and collision/presentation paths, including the exact stack argument order around `2DF2C` and `2D5D0`;
- `23B38` partial-width flag writes, signed record-count ownership, collision retry behavior, and live global reloads;
- `240A8` role-dependent admissibility, `-1` item sentinel handling, and the exact helper-call order;
- `243E0` empty-slot gating and the `7Eh..86h` unsigned item-range fallback;
- `24478` asymmetric left/right class/rank arbitration, per-side cooldown ownership, and selector-0/1 return behavior;
- corrected Batch 52 `24A9C..25DFB`: signed quotient/remainder construction, clipping/page-capture paths, byte-only page toggles, display/resource restoration order, inventory-group ownership, record compaction, resource gates, callback-sensitive reloads, and low-word add/clamp semantics at `25DA8..25DCA`.

The audit explicitly checked the recurrent CODE.1 defect classes: cdecl slot order, signed/unsigned drift, byte/word partial-register aliases, modulo-32 arithmetic, encoded loop entry, global reloads across opaque calls, sentinel widening, and address-distinct helper ownership. **No new semantic defect was established in this span.**

## Accounting

- prior audited bytes: **142,896**
- checkpoint 25: **12,220**
- cumulative semantic audit: **155,116 / 239,698 = 64.7131%**
- semantic defect classes: **6**
- next frontier: **`0x25DFC`**, beginning the exact 135-entry CS dispatch table owned by the following dispatcher family.
