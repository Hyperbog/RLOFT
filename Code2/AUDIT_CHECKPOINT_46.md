# CODE.2 semantic audit checkpoint 46

## Scope
- Immutable object-1 range inspected: `0x37E77..0x38105` (end-exclusive `[0x37E77,0x38106)`).
- Bytes inspected this checkpoint: **655**.
- Reconstruction ownership: Batch 131, 655 translated instruction bytes, no newly classified non-code.
- Cumulative semantic inspection: **228,076 / 239,698 = 95.1514%**.
- Remaining uninspected: **11,622 bytes = 4.8486%**.
- Next source-explicit boundary: **`0x38106`**.

## Immutable anchors
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- prior mandatory overlap `[0x37E37,0x37EF7)`: `10d2fb78985a49f70b7acb63f5bda9d6c3adfc8d4c863bf2402fe6c670b2dcc8`
- checkpoint span `[0x37E77,0x38106)`: `c2131bceeadda4ec9b70cf3ec8256f8bcc6572a106c62d0040cf4c4048d9ede7`

## Confirmed semantic mismatch
### `37EFA` / `3A0A8` predicate polarity
The committed Batch-131 reconstruction names the `3A0A8` boundary `descriptor_is_device` and uses it positively in the pre-open/create special-case block:

`if ((mode&3u)!=0u && handle!=-1 && descriptor_is_device(handle)) { ... }`

Immutable CODE.2 encodes the opposite branch at this site:
- call `3A0A8`
- `TEST EAX,EAX`
- `JNE 37F66`
- only a zero result falls through into the `mode&0x400` rejection and `mode&0x40` zero-length-write path.

This is not explainable by the later use of the same helper: at `38004` the binary calls `3A0A8`, tests EAX, and on a nonzero result executes `OR BH,20h`, exactly matching the reconstruction's positive `descriptor_is_device` interpretation for descriptor publication. Therefore the earlier C++ predicate has reversed the encoded condition.

Required repair: preserve the zero-result predicate for the `37EFA` pre-processing block rather than normalizing both call sites to the same positive high-level test.

## Other audited behavior
No additional mismatch was established in this batch. The following remain represented:
- leading-space path skip;
- low-byte `mode & 0x83` access/share formation;
- carry/error folding around DOS open/create/close calls;
- create-on-file-not-found fallback and `0x180` default create mode;
- descriptor flag publication including BH bit `20h`;
- runtime-mode-dependent bit `40h` logic;
- `38075` IOCTL get/set path and error translation;
- address-distinct wrappers at `37E77`, `380E1`, and `380F8`.

## Open repair set
1. `36CEC..372F5` — 1,546 accounting-owned bytes lack explicit semantic reconstruction.
2. `372F6` — PIT/speaker gate predicate mismatch.
3. `3736D` / `37433` — mutable stack-slot radix aliasing normalized away.
4. `37810` — planar permutation address calculation uses displaced table value instead of encoded matched table position.
5. `37D60` — zero-unread input path loses the encoded shared post-seek compare tail.
6. `37EFA` — `3A0A8` predicate polarity reversed in the pre-processing block.

## Resume
Continue semantic audit at **`0x38106`** (Batch 132 ownership), preserving DOS descriptor read/write/seek/delete/current-drive/default-drive behavior, partial-register/carry folding, and address-distinct wrapper boundaries.