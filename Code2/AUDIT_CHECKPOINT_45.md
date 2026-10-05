# CODE.2 semantic audit checkpoint 45

## Scope
- Immutable object-1 range inspected: `[0x37CA3,0x37E77)` / `37CA3..37E76`.
- Bytes inspected this checkpoint: **468**.
- Cumulative semantic inspection: **227,421 / 239,698 = 94.8781%**.
- Remaining uninspected: **12,277 bytes = 5.1219%**.
- Next source-explicit boundary: **`0x37E77`**.

## Immutable evidence
- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- prior-boundary overlap `[0x37C63,0x37D23)`: `baf2274023170be44827c06d56f0d5646e272b101d85b0947e39232a7d52f83e`
- checkpoint span `[0x37CA3,0x37E77)`: `2388b8b64c8cae7d0ad1a1eac1ed70c82fde372437522672bfe2013b3f225802`
- next overlap `[0x37E37,0x37EF7)`: `10d2fb78985a49f70b7acb63f5bda9d6c3adfc8d4c863bf2402fe6c670b2dcc8`

## Result
Batch 130 ownership was compared against direct immutable i386 disassembly and the exact committed Batch-130 C++ fragment. One additional semantic mismatch was established.

### Confirmed mismatch: `37D60` input-flush zero-unread path
The reconstruction models the input side as testing seek failure only when `s.pos != 0`:

`if (!(s.flags1&0x20u) && s.pos!=0u && b.stream_seek && b.stream_seek(...)==-1) ...`

The binary is more peculiar. At `37DE9` it loads `s.pos` into EAX and tests it. When `s.pos == 0`, control jumps directly to `37E01`, where the binary still executes `CMP EAX,-1`. Because EAX is exactly zero on that path (it was just loaded from `s.pos`), the comparison is false and no error is set. Thus the high-level result for `s.pos==0` is the same, but the encoded control/data dependency is not represented: the zero case deliberately flows through the shared post-seek comparison with EAX=0 rather than bypassing the comparison. Under the semantic-audit protocol this is recorded as a control-flow fidelity mismatch, not a behavioral output mismatch.

The output side preserves the distinct `write == -1` versus partial-write path: partial write explicitly sets errno 12 before marking stream error; direct `-1` preserves the lower-layer error.

The stream initialization capacity choices `0x86`, `1`, and `0x1000`, embedded one-byte fallback, flag masking, and DOS AH=43 attribute path remain consistent with the committed reconstruction at the modeled boundary.

## Open repair set
1. `36CEC..372F5` — 1,546 accounting-owned bytes lack explicit semantic reconstruction.
2. `372F6` — PIT/speaker gate predicate mismatch.
3. `3736D` / `37433` — mutable stack-slot radix aliasing normalized away.
4. `37810` — planar permutation uses displaced table value instead of encoded matched table position.
5. `37D60` — zero-unread input path normalizes away the encoded shared `CMP EAX,-1` tail. Behavioral result is unchanged for `pos==0`, but exact control/data-flow fidelity differs.

## Resume
Continue at **`0x37E77`** (Batch 131). Preserve open/create fallback, low-byte mode handling, device flag propagation, DOS carry/error domains, IOCTL behavior, and address-distinct wrappers.