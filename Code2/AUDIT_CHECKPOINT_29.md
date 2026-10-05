# CODE.2 semantic audit checkpoint 29

Status: **PASS**

## Range and accounting

- Audited object-relative range: **`2F0B4..2F57F`**.
- Bytes closed by this checkpoint: **1,228**.
- Cumulative semantic audit: **193,904 / 239,698 = 80.8951%**.
- Semantic defect classes remain **6**; no new semantic defect was established in this checkpoint.
- Immutable CODE.2 SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`.
- Immutable object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`.
- Audited span SHA-256 (`2F0B4..2F57F`): `29e864b0c88a86c71c7f4ac8f630f9a2edc4a412ef4ffd7bea9a10fa56f9f95a`.
- Previous-frontier overlap `[2F074,2F174)` reproduced exactly as `a416f5b98763682e728abd2551ae753b2a4fca7a13c605ecb670bdb9a2e52585` before forward work.
- Mandatory overlap around the next frontier `[2F540,2F640)`: `04f451f264eede109af14694415dfc45fb76748713890629c2ea340b9922e80b`.

## Ownership/family coverage

This checkpoint closes original reconstruction **Batch 69** as one ownership-complete family (`2F0B4..2F57F`). The batch contains 1,209 translated bytes plus 19 already-proven non-code bytes; semantic accounting closes the complete 1,228-byte ownership span.

- **`2F0B4` record selection:** preserved the exact minimum-unsigned-16-bit selection rule. The scan initializes the selected index to `-1`, rejects records whose type byte differs or whose flag byte has bit `10h`, and only replaces an existing candidate when the new record's word at `+3` is strictly below the current minimum. The selected container/record indices are then passed to `2EFD0` in encoded cdecl order.
- **`2F1E4` record filtering:** preserved the slot-independent `F0C0` exemption. The exemption is not normalized into a per-slot predicate; the binary's global test and loop/control ordering remain distinct.
- **`2F2E0` callback boundary:** preserved the far-return (`RETF`) callback semantics at `2F2EB`; this remains address- and calling-convention-distinct from nearby near-return wrappers.
- **`2F2EC` hardware/runtime family:** preserved signed-word promotion and low-port behavior rather than widening the operation into an unsigned or full-register abstraction.
- **`2F304..2F464` initialization/state family:** call ordering, branch-specific initialization, multimedia/resource setup and teardown, global tests, and address-distinct success/fallback paths were rechecked directly against fresh object disassembly.
- **`2F468..2F57F` wrappers/state transitions:** preserved conditional resource dispatch, cleanup ordering, return-value gates, and the distinct short wrapper entries through the final RET at `2F57C`; the remaining bytes through `2F57F` stay with Batch-69 ownership exactly as established during reconstruction.

## CODE.1 cross-reference checklist

The fully audited CODE.1 defect catalogue was reapplied across the span: cdecl slot order, live-register call inputs, byte/word aliases, signed/unsigned branch drift, modulo-width arithmetic, global reloads, address-distinct wrappers, selector/far-pointer behavior, interrupt/runtime boundaries, and odd encoded control flow. No additional CODE.1 defect class reproduced here.

## Boundary result

Fresh immutable disassembly reproduces the prior overlap exactly and confirms the Batch-69 sequential frontier at **`2F580`**. The next semantic-audit frontier is therefore **`2F580`**.
