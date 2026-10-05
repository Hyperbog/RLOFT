# CODE.2 semantic audit checkpoint 34

## Scope

- Previous frontier: `0x34270`
- Audited ownership-complete range: `0x34270..0x35285`
- Bytes audited this checkpoint: **4,118**
- Cumulative semantic audit: **217,718 / 239,698 = 90.8301%**
- Semantic defect classes: **6** (no new class established)
- Next frontier: `0x35286`

This increased-throughput pass closes reconstruction batches **105 through 110** in one semantic-audit checkpoint.

## Immutable source revalidation

Revalidated directly from `/mnt/data/CODE.2` before forward work:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- extracted object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`
- mandatory prior-frontier overlap `[0x34230,0x342F0)`: `0610773a8f59ba53b77e2cb943ac64637a14750efa6f6f8e73a764d44ac7ac2a`
- checkpoint span `[0x34270,0x35286)`: `fbbdd6e05b3b1d4c061fb54fe0e8201dfe0e295cd2698fe35784b483dbfc0f44`
- next-frontier overlap `[0x35246,0x35306)`: `634e0145a056a4fa874c88ce98ac733dfe33dfdcfccc7afcb3e3a93f1bdc8c61`

## Reconstruction evidence reviewed

The audit compared the immutable instructions against the committed reconstruction and, where the large current `Code2.cpp` response is connector-truncated, against the exact staged fragments embedded in the substantive integration commits. This is source evidence rather than reconstruction notes: the fragments are the text integrated into `Code2.cpp`, with workflow fidelity guards and byte-range static assertions.

## Ownership families reviewed

### `34270..342E3` — dual-entry angle normalization/table helper (Batch 105)

Preserved the two address-distinct entries: `34270` subtracts `10Eh` before joining the body at `34275`, while direct `34275` callers do not. The negative-angle loop adds `168h` until the sign bit clears; the upper normalization performs one signed comparison/subtraction at `168h`. Quadrant folding retains the exact `10Eh/B4h/5Ah` thresholds, word lookup at `EBX + EDI*4`, `INC`, logical `SHR 2`, and sign inversion only when quadrant bit 1 is set. `342E2..342E3` remains alignment.

### `342E4..34997` — stack-sharing planar texture renderers (Batch 106)

Rechecked the unusual stack-sharing control flow rather than treating the renderer as an ordinary C++ call sequence. The saved return token at `10258h`, four VGA sequencer masks `0802h/0402h/0202h/0102h`, and the second two-plane entry masks `0C02h/0302h` remain distinct. Each 64-pixel sampler preserves modulo-32-bit EAX/EDX stepping, the two `SHLD ...,6` operations used to form the 64x64 texture index, texture-base reload from `[FF54h]`, AL overwrite by `LODS/SUB`, ES-qualified byte store semantics, and EDI post-store advancement. The EAX low-byte mutation is carried into the following unrolled pixel exactly as the machine code requires. Alignment at `34951..34953` and `34995..34997` remains non-code.

### `34998..34A73` — self-modifying lookup-table initializer (Batch 107)

Preserved the four generated coefficients derived from the `C65C` table and `168h-[C658h]` angle, including distinct calls through `34270` and `34275`. The four 16-bit code-immediate patch sites remain explicit rather than being normalized into ordinary variables. The 128-record output loop retains signed 16x16 multiplication followed by the exact SHLD-derived high result, byte truncation for the first pair, word stores for the second pair, two-byte source stepping, and six-byte destination stepping.

### `34A74..34BAF` — self-patched renderer driver (Batch 108)

The byte-immediate patch sites sourced from `C044h`, `C048h`, and the FE94 shading table remain explicit. Rechecked the signed clamp of `[FF50h]` to `50h`, the first four-plane path, the residual two-plane path, six-byte parameter records, `<<16` word expansion, byte-wrap additions into AL/DL, and the exact p3/p2/p1/p0 versus p2/p0 plane ordering. The stack-sharing renderer continuation tokens are preserved as guest return identities rather than flattened host returns. `34BAD..34BAF` remains padding.

### `34BB0..34DE7` — indexed quad construction/raster dispatch (Batch 109)

Rechecked the distinction between raw arithmetic `SAR 3` on incoming indices and the separately bias-corrected signed divisions by 8 and 2. Signed clamping to `[C738h]`, zero-index rejection, mirrored odd-vertex exchange, signed/unsigned word roles in the quad arrays, record flag gates, and the two raster modes (`FDB8/FDB4 = 84h/103h` versus `04h/83h`) remain intact.

The historical Batch-109 resource-base defect is confirmed repaired in the current reconstruction: the resource identifier is **`[258F8h] + record[9]`**, not address constant `258F8h + record[9]`. This is an already-known reconstruction correction and does not create a seventh semantic defect class. `34DE7` is the one-byte separator.

### `34DE8..35285` — scene-preparation scan and quad dispatch (Batch 110)

Preserved the four working arrays at `2B7F0h`, `2B870h`, `2B7B0h`, and `2B7D0h`, including their exact 80h/20h clear and copy widths. Bounds derive from signed words at `255D2h/255D6h`, expand by +/-15, clamp to zero and the signed limits at `25619h/2561Bh`, and retain the binary's exclusive loop comparisons.

The scene-record scan keeps the encoded active-run state transitions: null record and missing flag 10h clear the active state, while type 3 with flag 08h clear skips without the same reset. Coordinate transforms preserve 16.16 packing from low 16-bit grid values, word-only `+20h` post-adjustment, validity-byte caching, and the exact signed/unsigned word extraction into the local coordinate/index arrays before the `34BB0` call. The tail retains the 80h and 20h copy/zero operations and jumps back into the scan exactly as encoded. `35286` is the next independent boundary.

## Defect-checklist review

The span was specifically rechecked for the six established CODE.2 semantic defect classes and the CODE.1-derived checklist: cdecl slot order, partial-register aliasing, signed/unsigned drift, modulo-32-bit arithmetic, sentinel/address confusion, reloads across opaque calls, selector/segment state, self-modifying immediate state, stack-sharing returns, and address-distinct entry points/shared tails.

No new mismatch was established. The known Batch-109 address-vs-loaded-resource-base mistake had already been corrected before this semantic pass and the corrected form matches the immutable instructions.

## Result

Checkpoint 34 establishes semantic closure through object offset **`0x35285`**. No new reconstruction edit is required. The semantic audit is now **217,718 / 239,698 = 90.8301%** complete, leaving **21,980 bytes (9.1699%)**. The next independent semantic frontier is **`0x35286`**.
