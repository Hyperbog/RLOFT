# CODE.2 semantic audit checkpoint 33

## Scope

- Previous frontier: `0x339A2`
- Audited ownership-complete range: `0x339A2..0x3426F`
- Bytes audited this checkpoint: **2,254**
- Cumulative semantic audit: **213,600 / 239,698 = 89.1121%**
- Semantic defect classes: **6** (no new class established)
- Next frontier: `0x34270`

This pass closes reconstruction batches **96 through 104** in one semantic-audit checkpoint.

## Immutable source revalidation

Revalidated directly from `/mnt/data/CODE.2` before forward work:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- extracted object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- repaired prior-frontier overlap `[0x33962,0x33A22)`: `812a92cb21c279b4704ded95b14ac8e1a4572ae692c1b6b04afd8346f9d12b8d`
- checkpoint span `[0x339A2,0x34270)`: `52a0067dc3e1c2a12d5c1b537e1535be38602015e1f85ded558013f6b9420a88`
- next-frontier overlap `[0x34230,0x342F0)`: `0610773a8f59ba53b77e2cb943ac64637a14750efa6f6f8e73a764d44ac7ac2a`

## Ownership families reviewed

### `339A2..33A37` — DOS find-first/find-next and DTA shuttles

Preserved `339A2` argument reshuffling into DS:DX/CX, AH=1Ah DTA setup, AH=4Eh find-first, and carry/error translation through `3908C`. `339CF` performs the pre-find `33A15` shuttle and the post-find `339F2` shuttle is retained on both success and failure. The `12662 == 9` compatibility gate and the opposite DS/ES directions of the two 43-byte `REP MOVSB` copies were rechecked directly.

### `33A38..33A9F` — DOS directory wrappers and interrupt-frame veneer

The AH=39h create-directory and AH=3Bh change-directory wrappers retain the encoded `RCL/ROR/TEST/JGE` carry-to-sign conversion, low-16 error truncation before `390D7`, and `FFFFFFFFh` failure contract. `33A7C` retains its four register-frame roles and opaque `39148` boundary; the output EAX reload remains from `[output_regs+0]`. `33A9F` is alignment.

### `33AA0..33C8F` — DPMI mouse setup and reset/cleanup family

Preserved raw local frame offsets and the exact four-argument call into `33A7C`, including the 32h-byte frame clear, `53C1h` field, high-word extraction from `FD14`, and equality probe. The direct INT 33h reset publishes low AX/BX words, allocates through `30D80(40h,&FD14,&FD18)`, retains the allocation when the probe byte is nonzero, and frees by selector through `30DE8` when zero. Reset failure retains the encoded cleanup sequence through `311E8`, conditional `27ECC`, `285D4`, conditional `362BE`, `30C38`, and `352BE`. No host-API normalization is permitted.

### `33C90..33DDF` — mouse wrappers and polling loops

Preserved post-`2E340` register residue, signed arithmetic halving of low CX in `33CB0`, doubled second coordinate for INT 33h function 4, and address-distinct function 7/8 wrappers. `33D58` and `33D78` retain their multi-stage polling loops and exact return/register behavior. Alignment islands remain non-code as previously classified.

### `33DE0..33E6B` — VGA byte-fill pair

Each routine retains the single 16-bit sequencer `OUT DX,AX`, low-byte wrap of `input - FE94[EBX]`, 128-iteration pre-decrement loop shape, ES-qualified `REP STOSB`, and distinct post-fill EDI stepping (`-90h` versus `+10h`).

### `33E6C..3426F` — polygon/edge setup and planar rasterizer

The 8-dword DS:ESI -> ES:EDI copy, signed min/max comparisons, exact `80000001h` initial maxima, and address-distinct `3402C` edge setup calls were rechecked. `33FB4/3402C` retain their carry contract and `3410C..3410D` remains a physical shared tail.

The fixed-point edge math retains the binary's full signed32-times-65536 IDIV reconstruction, including the extra X-gradient pre-shift, machine-width wrapping, signed arithmetic shifts, and in-place `FE10 << 6` mutation. The rasterizer retains its exact VGA plane selector output derived from `FD20 & 3`, `50h` destination stride, signed source-address reconstruction, and byte subtraction indexed by the scanline value. The unconditional first `34110`/`33FB4` iteration and CF-controlled exit remain intact.

`3410E..3410F` is alignment and `34268..3426F` is proven padding. `34270` is the next independent frontier.

## CODE.1 cross-reference defect checklist

Rechecked against the established CODE.1 defect classes: cdecl slot order, partial registers, signed/unsigned drift, modulo-32-bit arithmetic, sentinel widening, reloads across opaque calls, address-distinct wrappers, selector/far-pointer state, and shared-tail ownership. No new CODE.2 mismatch was established.

## Result

Checkpoint 33 establishes semantic closure through object offset **`0x3426F`**. No reconstruction edit is required. The next independent semantic frontier is **`0x34270`**.
