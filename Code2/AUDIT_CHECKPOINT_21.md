# CODE.2 semantic audit checkpoint 21

Status: **PASS**

Audited object-relative range: **`171FC..1A21F`** (**12,324 bytes**).

Cumulative semantic audit: **107,024 / 239,698 = 44.6495%**.

Semantic defect classes remain **6**. No additional semantic defect was established in this ownership-complete four-batch span.

## Immutable evidence

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- Mandatory frontier overlap `[171BC,172BC)` SHA-256: `d329244d970a3ec37c7a0dbdb2f559954692474c56036deac4314686957ad2d8`
- Audited span `[171FC,1A220)` SHA-256: `c0f297223908021c479c6be0e4054ae840e09881cfa04b5aa01f028ef57170e2`

The overlap was freshly disassembled from the immutable object before extending the frontier; overlap bytes were verification-only and were not recounted.

## Ownership/family coverage

This checkpoint deliberately increases throughput by closing four complete reconstruction families in one pass: original reconstruction batches **31 through 34**.

- `171FC` save-file writer: preserved signed-IDIV three-digit filename substitution, delayed seven-dword section-offset header at file offset 1, and distinct terminators for the `CB3C` and `CD3C` dynamic-record families.
- `17838`: preserved resource-`37h` dependent level-exclusion sets and wrapped `(random*4)>>15` delay arithmetic.
- `179A4..17A67`: retained as the exact 49-entry CS table owned by `17A68`, not executable fall-through.
- `17A68`: preserved record-bit-1 clear/recompute, pre-dispatch special types, exact 49-way `22h..52h` dispatch, and type-`4Eh` mutation to type 1 with pointer `10000000h` and rerolled word.
- `181B4`: preserved direct-save versus `levels.dat` fallback split, deferred 10-byte `CB3C -> CD3C` redistribution, compact 11-byte conversions, ignored serialized primary-count word on direct-save, relative four-byte seek, signed `<7FFh` packed-text gate, close-before-post-load restoration, path-dependent live EBX (`0` direct / `3` legacy) into `1C160(20h,EBX)`, and late `BF44` level overrides.
- `18D90`: preserved one-shot `F050+72h` latch, redraw-only-on-mutation input loop, low-byte-plus-one `165C` class lookup, and three-failure `205B4(-1)` path.
- `191C0`: preserved x86 two's-complement ABS, low-32 IMUL, and signed-IDIV interpolation.
- `192CC`: preserved address-distinct tail jump to `1A9F8`.
- `19454..19463`: retained as the exact four-entry CS dispatch table; `19464` keeps unsigned dispatch gating for the `-1` hover sentinel and signed mouse-threshold comparisons.
- `197B8`: preserved full dword load from `C050` with only AL XORed before `2A020`; upper bytes remain live.
- `19888`: preserved staged menu/cinematic sequence, saved `C02C/C02B` bytes, four unsigned timed waits, ESC promotion to status 1, and unconditional `19464` cleanup.
- `19C80`: preserved strict unsigned `now > A790` delayed-click expiry.
- `19CCC/19CF4`: preserved 14-level stack semantics where only low byte of `A7E8` is saved and restore zero-extends it.
- `19D18`: preserved wrapped coordinate scale, conditional bit-8 clearing, three high-flag +/-1000 axis tests, and cursor redraw/state-save behavior.
- `1A0B0`: preserved initialization of eight `FFFFh` words and installation of mask `FFFFh`.
- `1A1D0`: preserved shared prior RET when disabled and address-distinct tail jump to `33B44` when enabled.
- `1A1F4`: preserved full button-word store at `9E35` while returning only the low two bits.

Cross-cutting CODE.1 defect checks were reapplied for cdecl slot order, partial-register aliases, signed/unsigned branches, modulo-32 arithmetic, global reloads across calls, sentinel widening, shared tails, and address-distinct wrappers. No new CODE.2 mismatch was established.

Next semantic-audit frontier: **`1A220`**, which falls directly into the large `1A288` mouse-update family.
