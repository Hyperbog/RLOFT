# CODE.2 final semantic audit

## Source identity

- Original file: `CODE.2`
- Size: **558,689 bytes (`0x88661`)**
- SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 file start: `0x3AF24`
- Object-1 length: **239,698 bytes (`0x3A852`)**
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

## Audit method

This is a semantic audit of the already byte-closed CODE.2 reconstruction. Coverage accounting remains **235,022 translated instruction bytes + 4,676 proven non-code/data bytes = 239,698 / 239,698**, with **0 unresolved bytes**. Audit progress is tracked separately and never changes byte ownership unless a prior ownership error is discovered.

For every audit checkpoint, immutable hashes are reverified, disassembly is regenerated directly from the immutable object, and the current reconstruction is checked for call/argument order, stack-slot consumption, signedness, partial-register effects, reloads across calls, overflow/truncation, shared tails, selector/far-pointer state, and address-distinct control flow. The fully audited CODE.1 reconstruction and `Code1/FINAL_BINARY_AUDIT.md` are used aggressively as reference models, never as substitutes for CODE.2 evidence.

The CODE.1 final audit is specifically treated as a defect checklist: cdecl slot order, byte/word aliasing, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry behavior, selector and interrupt-frame preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrapper behavior are mandatory checks in CODE.2.

## Semantic audit checkpoint 1 — `00000..002A7`

Fresh disassembly was regenerated from the immutable CODE.2 object and reconciled through the complete early application block and the 13-entry dispatcher table boundary. Audit span: **680 object bytes (`0x2A8`) = 0.2837% of object 1**.

Rechecked routines and classifications:

- `00000..00002` trap stub and `00003..0000F` alignment.
- `00010..00072` type-54 record search/flag update.
- `00073` NOP alignment.
- `00074..000B7` type-8 cleanup and encoded runtime-call sequence.
- `000B8..001F9` slot-selection/state-machine routine.
- `001FA..001FB` MOV-EAX,EAX alignment.
- `001FC..00270` resource-46 slot allocation/flag rejection/table-bit clear.
- `00271..00273` LEA alignment.
- `00274..002A7` thirteen-entry CS jump table consumed by `002A8`.

One semantic defect was found in `000B8..001F9`. In the `1C1DCh(20h) != 0` branch, the binary executes `16524h(9)`, `1C124h(3Dh)`, and `1C160h(3Ch)` and then performs the four-slot flag-clearing loop. The fourth iteration branches directly to the function epilogue; there is **no** `1C124h(3Ch)` call on this path. The previous C++ incorrectly inserted that extra call after the loop. The audit removed it and preserved the live encoded EAX result (`0x920`). The alternate path still performs the real `1C124h(3Ch)` call at `001EE`.

## Semantic audit checkpoint 2 — `002A8..011A7`

The audit was widened from individual tiny routines to complete reconstructed families. Fresh CODE.2 disassembly was generated from `00280` through `01200`, with a full-object direct relative branch/call target scan used for separator validation. **No additional semantic correction was required in this checkpoint.**

- `002A8..006B8`: thirteen-way command dispatcher — PASS.
- `006B9..006BF`: seven-byte zero island — PASS as non-code/padding.
- `006C0..009AF`: coordinate and record-search family — PASS.
- `009B0..00B6B`: four-neighbor tile classifier — PASS.
- `00B6C..00BF9`: tile draw helper — PASS.
- `00BFC..00CAF`: fixed-point marker/line helper — PASS.
- `00CB0..010C2`: multi-family map renderer — PASS.
- `010C4..011A4`: fixed UI-frame decoration — PASS.
- separators through `011A7` — PASS.

High-risk details explicitly retained include live-register call inputs, partial-BX behavior, repeated global reloads after opaque calls, signed IDIV/remainder semantics, address-distinct search families, and asymmetric alternate-record sources.

## Semantic audit checkpoint 3 — resumed at `011A8`

The interrupted audit was resumed from the exact recorded frontier rather than restarting. Before any new semantic claim, both immutable identities were revalidated locally:

- `CODE.2` SHA-256: `853e0a4dfcabebcfeecb975cf2a92b92eb4d94806d835ad30a0bab3e265bd314`
- Object-1 SHA-256: `014952c4f003c548d928984825bab57dff54ce962dbfae21d7260f19fd3a62f3`

Fresh boundary-aware disassembly was regenerated across the previous frontier and into the next graphics/UI family. The first independently decoded entry is `011A8`; the preceding `011A5..011A7` LEA/no-op separator remains correctly owned by checkpoint 2.

The `011A8` family begins with the fixed frame setup and then enters the record/text rendering path at `01228`. Audit emphasis here is on the same CODE.1 defect classes that are most dangerous in this family: signed 16-bit record fields promoted to 32-bit, per-character width lookup, carriage-return handling, coordinate clipping, and preserving call-time globals rather than normalizing them into cached state. The disassembly confirms the encoded signed `MOVSX` operations, the six-byte record-header stride, the character loop's byte-width table lookup, and the branch back to the next record. No contradiction with the reconstructed ownership boundary has been found in the resumed overlap.

This checkpoint is intentionally left open at the next family boundary rather than claiming unaudited bytes after the interruption.

## CODE.1 cross-reference policy in the accelerated audit

The fully audited CODE.1 decompilation is part of the active audit procedure. Its discovered defect classes determine where CODE.2 receives extra scrutiny: live-register call inputs, partial-register aliases, global reloads after opaque calls, signed division/comparisons, stack-slot order, selector state, and branch convergence. Runtime/graphics regions that are mechanically proven relocation-only counterparts may therefore be audited in larger blocks than during decompilation, while any opcode/control-flow divergence forces a CODE.2-specific review.

## Current result

- Audited object bytes closed through checkpoint 2: **4,520 / 239,698 = 1.8857%**.
- Semantic corrections found so far: **1**.
- Resumed overlap at `011A8`: immutable identity and boundary revalidated; checkpoint 3 in progress.
- Current closed audited frontier: **`011A7`**.
- Active audit frontier: **`011A8`**.

The semantic-audit percentage is independent of the completed 100% decompilation/byte-accounting percentage.
