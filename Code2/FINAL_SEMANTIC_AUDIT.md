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

For every audit checkpoint:

1. Reverify the immutable CODE.2 and object-1 hashes.
2. Regenerate disassembly directly from the immutable object.
3. Reconcile the current `Code2.cpp` against the encoded instructions, including call/argument order, stack-slot consumption, signedness, partial-register effects, reloads across calls, overflow/truncation, shared tails, selector/far-pointer state, and address-distinct control flow.
4. Use the fully audited CODE.1 reconstruction aggressively as cross-reference evidence where code families correspond. `Code1/Code1.cpp` and `Code1/FINAL_BINARY_AUDIT.md` are reference models, not proof: every CODE.2 result must still be established from CODE.2 bytes.
5. Apply semantic corrections without changing coverage totals unless the binary proves an accounting/ownership defect.
6. Run `g++ -std=c++17 -Wall -Wextra -pedantic -fsyntax-only Code2/Code2.cpp` and `git diff --check` before committing a checkpoint.

The CODE.1 final audit is specifically treated as a defect checklist: cdecl slot order, byte/word aliasing, signed/unsigned branch drift, modulo-32-bit arithmetic, encoded loop entry behavior, selector and interrupt-frame preservation, global reloads, address-distinct thunks, self-modifying immediates, and DOS/runtime wrapper behavior are all mandatory checks in CODE.2.

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

One semantic defect was found in `000B8..001F9`. In the `1C1DCh(20h) != 0` branch, the binary executes `16524h(9)`, `1C124h(3Dh)`, and `1C160h(3Ch)` and then performs the four-slot flag-clearing loop. The fourth iteration branches directly to the function epilogue at `001F6`; there is **no** `1C124h(3Ch)` call on this path. The previous C++ incorrectly inserted that extra call after the loop. The audit removes it and returns the live encoded EAX value (`0x920`) from the completed loop path.

The alternate path still performs the encoded `1C124h(3Ch)` call at `001EE` after selecting and marking a slot; that call remains intact. The signed record-count reloads in `00010`/`00074`, partial BX/AX flag masking in `000B8`, three `27C70h` probes, signed `0x920` loop comparisons, and the signed word index used by `001FC` were rechecked against the binary.

CODE.1 was consulted as the audit discipline/reference corpus, especially its documented false-normalization failures, but these early application routines do not have a directly substituted CODE.1 counterpart; the correction above is derived solely from CODE.2 control flow.

**Audit frontier:** `002A8`.
**Semantic audit progress:** **680 / 239,698 bytes = 0.2837%**.
