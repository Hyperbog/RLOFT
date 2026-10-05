# CODE.2 semantic audit checkpoint 50

## Scope
- Immutable object-1 range inspected: `[0x38A0A,0x38BC1)` = **439 bytes**.
- Ownership: Batch 135 independent helpers plus the Batch-143 explicit `38A30..38A60` shared-tail helper.
- Cumulative semantic inspection: **232,369 / 239,698 = 96.9424%**.
- Remaining uninspected: **7,329 bytes = 3.0576%**.
- Next boundary: **`0x38BC1`**.

## Immutable anchors
- CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- `[38A0A,38BC1)` SHA-256: `1848e3d68a7418be3b0afcd556af42198689d1eb5bf5c6d56c99de3668d73532`
- overlap `[389CA,38C01)` SHA-256: `c76a9070a08dfa3571eb300d7b132a84e0838f1ac512d562561de88928c5e731`

## Clean helpers
- `38A0A..38A2F`: selector-qualified bounded byte-string length preserves the zero/limit ordering.
- `38A30..38A60`: selector-qualified bounded word-string length preserves the encoded backward jumps to shared epilogue `38862`; audited here once as explicit Batch-143 ownership.
- `38BA6..38BC0`: address-distinct three-argument forwarding wrapper preserves right-to-left cdecl push order through `[12700h]`.

## New semantic mismatches

### 1. `38A61` exact-width hexadecimal materializer underflow behavior
The binary converts into the destination first, determines the produced digit count, then performs the right-alignment copy backward. Its zero-fill loop is guarded by a signed test of the residual width index. If requested width is smaller than the produced digit count, the residual index is negative and the zero-fill loop is skipped.

The reconstruction computes unsigned `lead = width - n` and then loops `lead` times. For `width < n`, this wraps to a very large unsigned value and is not equivalent to the encoded signed residual-width behavior.

Required repair: preserve the signed residual index / backward-copy semantics rather than unsigned `width-n` normalization.

### 2. `38AC3` fixed-point formatter third argument is a format-state pointer, not precision by value
Immutable instructions load the third cdecl slot into `ESI` and repeatedly access precision as `[ESI+08h]`:
- `MOV ESI,[ESP+1Ch]`
- `CMP DWORD PTR [ESI+08h],FFFFFFFFh`
- default precision writes `4` back to `[ESI+08h]`
- subsequent loop limit also reads `[ESI+08h]`.

The reconstruction signature `function_38ac3(u8* dst,u32 bits,u32 precision)` collapses this into a by-value precision and therefore loses both pointer identity and the encoded mutation of the caller's format state when precision is `FFFFFFFFh`.

Required repair: third argument must represent the format-state object and default precision must mutate its `+08h` field.

### 3. `38AC3` rounding predicate uses the post-sign-normalization magnitude
At entry the binary stores the input dword in its local slot. On a negative value it performs `NEG EDX` and writes that magnitude back to the same local slot. The later rounding gate is `TEST BYTE PTR [ESP+01h],80h`, therefore it tests bit 15 of the **normalized magnitude**.

The reconstruction tests `(bits & 0x00008000u)` on the original pre-negation argument. This differs for negative inputs.

Required repair: test bit 15 of the local magnitude after sign normalization.

## Defect accounting
- New repair items established this checkpoint: **3**.
- The previously identified localized repair set remains subject to actual source integration/compile verification; do not mark those items closed merely because repair fragments/integrators were staged.

## Resume
Continue at `0x38BC1` with Batch 136 conversion dispatcher. Prioritize direct repair of the three Batch-135 mismatches when integrating the repair pass.