# CODE.2 faithful decompilation progress

![Progress](https://img.shields.io/badge/object%20accounting-100.00%25-brightgreen)
![Semantic audit](https://img.shields.io/badge/semantic%20audit-100.00%25-brightgreen)

**Executable object-1 denominator:** **239,698 bytes (`0x3A852`)**  
**Verified translated instruction bytes:** **235,022**  
**Proven non-code/data bytes:** **4,676**  
**Reconciled object bytes:** **239,698 / 239,698 = 100.0000% (100.00%)**  
**Unresolved object bytes:** **0**  
**Semantic audit:** **239,698 / 239,698 = 100.0000%**  
**Status:** **CERTIFIED COMPLETE** — object-1 byte accounting and end-to-end semantic inspection are complete.

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

## Original 2026-10-05 certified source (historical snapshot)

- Canonical reconstruction: `Code2/Code2.cpp`
- File size: **1,304,770 bytes**
- SHA-256: `b4ab60f12e0d5a3c986ef61ec3c2e5c6d68f0193a0e535ef35eb7af6f3aed4aa`
- Git blob SHA: `c3f16122e79c3c464382e4511d9d932f6fcd694a`
- Strict C++17 verification at certification: **0 errors, 0 warnings**

## Final state

Object-1 byte ownership and semantic inspection are complete. The accumulated semantic defect ledger was repaired in the canonical source, including the formatter/floating-format paths, incoming-CF behavior, rejected-stack behavior, explicit DPMI carry/register contracts, and final runtime-tail ownership through `0x3A851`.

`Code2/FINAL_CERTIFICATION.md` is the authoritative certification record. `Code2/FINAL_SEMANTIC_AUDIT.md` retains the detailed audit provenance.

## Post-certification semantic alignment — 2026-10-07

The previously diverging `Code2/Code2.cpp` source on `code1-decompilation` and `demo-v1` has been reconciled. Both branches now reference **the identical corrected Git blob `87f6638921c53a32bde59735515fbc175e2c96ee`**. The prior source identity above is retained as historical certification evidence. The original CODE.2 proves two source-level corrections: automap drawing offsets at `0139C` and stack argument/graphics forwarding at `2975C`. See [`POST_CERTIFICATION_GRAPHICS_CORRECTIONS_2026-10-07.md`](POST_CERTIFICATION_GRAPHICS_CORRECTIONS_2026-10-07.md).

All 239,698 executable object-1 bytes remain accounted for. This reconciliation verifies these two semantics only; a new strict whole-file syntax verification and runtime-equivalence test have **not** been completed in this checkpoint.

## Post-certification semantic correction — F790 stack-local T[3F] pre-read (2026-10-10)

Original CODE.2 F7CC..F7DA always calls 1C368 with T index 0x3F and a pointer into its 16-byte SS:ESP local after the pending 6DAC branch but before checking item==-1. The earlier canonical C++ omitted this IO call. Added `Application21Boundary::call_1c368_local(u8*,u32)` and the explicit stack-local read in `function_0f790`. Immutable binary SHA and exact 15-byte call window/target passed; original T[3F] decodes to `D`. Full-file strict C++17 recompile and DOS stream-state parity remain open gates. Object accounting and semantic certification totals are unchanged. [Complete source proof and native-engine handoff](F790_STACK_LOCAL_TEXT_PREREAD_20261010.md).


### 2026-10-10 source callback-address correction

Original F790 E8 sites `F7D3/F91C/FA9D/FB5A/FC78` and the spell-scribe reader at `116E2` all target `1C368`, **not** `2C368`. The canonical `Application21Boundary` had 28 stale textual references to `call_2c368`, now renamed to `call_1c368` / `call_1c368_local` without any other source edit. Original 21-edge F790 manifest passes; current source blob `5b0b4dd7b6a91bfcc99068970c3152f0f62ce54e`. Fresh full-file C++17 compilation and DOS runtime equivalence remain unverified; historical object-byte totals remain unchanged. See [original code proof](F790_STACK_LOCAL_TEXT_PREREAD_20261010.md).
