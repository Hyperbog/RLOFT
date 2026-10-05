# CODE.2 Semantic Audit Checkpoint 52

## Scope
Corrects Checkpoint 51's disassembly interpretation and audits immutable object-1 `38BC1..39588` against historical Batches 136-137.

Immutable identities revalidated:
- CODE.2 SHA-256 `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- object-1 SHA-256 `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Correction to Checkpoint 51
Checkpoint 51's claim that `38BC1..3908B` was a runtime/state family was caused by an incorrect disassembly-address interpretation and is withdrawn. Fresh direct disassembly of object-relative bytes proves `38BC1..3906B` is the formatted-conversion dispatcher and `3906C..3908B` is the uppercase helper, exactly matching the historical ownership partition. `3908C` is a genuine next entry and Batch 137 begins there.

Span `38BC1..3908B` SHA-256: `d65e0b09ed8b4760139900eaa7f9c12c4f4312329e5eb3c78f53913a86570251`.

## Batch 136 semantic result
Ownership is correct, but the historical C++ abstraction is not semantically faithful enough for closure.

Confirmed defects include:
1. Integer argument acquisition depends on format flags `20h` and `10h` and uses dword / zero-extended word / ordinary dword paths. The reconstruction collapses this into a generic `next32` preconsume.
2. Signed decimal `i/d` acquisition is distinct and uses sign-extending word behavior for flag `10h`; the reconstruction does not preserve this path.
3. `s/S`, `c`, and `p/P` consume their own cdecl argument slots in the encoded dispatcher. The reconstruction leaves `value` at its default for these cases, so pointer/string/character semantics are materially wrong.
4. Far string forms consume offset+selector pairs and preserve selector-qualified reads. The reconstruction calls `strlen_far(value,0xFFFF,precision)` without reconstructing the encoded far argument acquisition.
5. Pointer formatting distinguishes near 8-hex-digit output from far `ssss:oooooooo` output, including a second argument slot for the selector. The reconstruction cannot express the encoded far-pointer materialization.
6. The encoded sign/prefix/precision normalization after conversion uses the actual materialized byte stream and record fields; the reconstruction's coarse switch does not preserve the full state transitions.

Therefore Batch 136 requires a fidelity rewrite, but it is not the unrelated routine family reported by Checkpoint 51.

## Batch 137 audit
Inspected `3908C..39588` = 1,277 bytes, including the historical 3-byte separator `39147..39149`.

Span SHA-256: `b5d40199fa2e8a169b9e6bc95c9a1455e36d615fd5824da2f533456edbb1ba11`.

Findings:
- `3908C..3909E` is a carry-flag wrapper whose branch is driven by incoming CPU CF (`JAE`), not by a cdecl boolean argument. The C++ `function_3908c(..., bool carry, u32 ax)` changes the encoded ABI/state dependency and is a fidelity defect.
- `3909F` is the one-byte RET stub as reconstructed.
- `390A0..390D6` error wrappers agree at the result/error level.
- `390D7..39146` DOS-error normalization agrees in its version-dependent 50h/22h/20h remapping, signed high-byte table mapping, extended-error high-byte extraction, errno publication, and -1 return.
- `39147..39149` remains a 3-byte separator/non-owned executable boundary artifact as historically accounted.
- `3914A..39225` and the 256-entry `39226..39525` `INT n; RET` table preserve the intended interrupt-dispatch ownership, but the high-level register-image boundary remains an abstraction and must retain segment/register handoff when repaired.
- `39526..39588` bounded ASCII case-insensitive compare is consistent with the encoded behavior.

## Coverage
Checkpoint 51 already credited `38BC1..3908B` (1,227 bytes). This checkpoint adds `3908C..39588` = 1,277 inspected bytes.

Previous cumulative: 233,596 / 239,698.
New cumulative: **234,873 / 239,698 = 97.9879%**.
Remaining uninspected: **4,825 bytes = 2.0121%**.

## Open repair state
Batch 135 retains the three Checkpoint-50 repairs (`38A61`, `38AC3` format-state pointer/mutation, `38AC3` normalized-magnitude rounding). Batch 136 now has multiple confirmed dispatcher fidelity defects and needs a full fidelity rewrite rather than localized patching. Batch 137 adds the `3908C` incoming-CF ABI defect.

Next audit boundary: `39589` (Batch 138).
