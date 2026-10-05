# CODE.2 explicit reconstruction repair — 39A8A..39BBB

## Scope

The historical Batch-141 gap `[0x39A8A,0x39BBC)` has now been directly disassembled from immutable CODE.2 object 1 and represented explicitly in `.audit_gap_39a8a.fragment.cpp`.

- Range: `39A8A..39BBB`
- Length: **306 bytes**
- Immutable span SHA-256: `a9287a29464272bd2ace173544a54f3d34989fc6032b0b6faaa07b2e310435b3`
- Overlap `39B80..39C4F` SHA-256: `d805fdba6e3a35cf8afccd63c7ed0a166668f4cd8233094887af435213e31846`
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Reconstructed behavior

`function_39a8a` is the missing arena grow/resize helper. It preserves the following encoded behavior:

- two cdecl arguments: existing arena pointer and requested payload size;
- initial `39A2C` exhausted-arena cleanup;
- unsigned sorted-list search and exact `node+2C == arena` ownership match;
- requirement that the arena allocation count at `node+18` is exactly one;
- unlink through `399EB` before backing-kind validation or resize attempt;
- rejection of DOS-backed records (`owner+4 != 0`), leaving the already-unlinked state as encoded;
- old DPMI handle words loaded from `node-8`;
- requested resize computed from old backing extent plus the requested-payload delta, then rounded upward to a 4-KiB boundary;
- DPMI AX=0503h resize semantics, with carry determining success, BX:CX providing the new linear base, and SI:DI the updated handle;
- rewritten backing header, zero DOS-selector field, and `resize_bytes-0C` arena-record size;
- reinsertion/reinitialization through `39BBC`;
- allocation-count publication of one before rebuilding the sole live allocation;
- requested payload rounded to an even byte count exactly as encoded;
- split only when the remainder is at least 12 bytes;
- temporary allocated marking of the split tail followed by the encoded `37C45` release path, which reinserts/coalesces it as free;
- whole-block allocated marking when the remainder is too small;
- return of the rebuilt arena pointer, or zero on failure.

The fragment owns the exact interval with:

`static_assert(0x39BBCu-0x39A8Au==306u);`

## Integration state

`.audit_gap_39a8a_integrate.py` is staged for deterministic insertion immediately before `function_39bbc`. This does **not** by itself mean `Code2.cpp` has been modified; main-source integration and compile verification remain separate gates.

## Audit accounting

The 306 bytes were already present in final aggregate accounting, so translated/non-code totals do not change. They are now source-explicit and semantically inspected.

Cumulative semantic inspection rises from **236,270** to **236,576 / 239,698 = 98.6975%**, leaving **3,122 bytes = 1.3025%**.

## Resume

Continue at `39C30` with the explicit Batch-142 source.